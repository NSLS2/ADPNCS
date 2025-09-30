/**
 * Main source file for the ADPNCS EPICS driver
 *
 * This file was initially generated with the help of the ADDriverTemplate:
 * https://github.com/NSLS2/ADDriverTemplate on 07/07/2025
 *
 * Author: Jakub Wlodek
 *
 * Copyright (c) : Brookhaven National Laboratory, 2025
 *
 */

#include "ADPNCS.h"

using namespace std;

const char *driverName = "ADPNCS";

std::ostream &operator<<(std::ostream &os, const pncs::types::json::JSON &o) {
    pncs::types::json::JSON j = o;
    os << j.Dump();
    return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &o) {
    for (auto &v : o) {
        std::cout << v << " ";
    }
    return os;
}

/*
 * External configuration function for ADPNCS.
 * Envokes the constructor to create a new ADPNCS object
 * This is the function that initializes the driver, and is called in the IOC startup script
 *
 * @params[in]: all passed into constructor
 * @return:     status
 */
extern "C" int ADPNCSConfig(const char *portName, const char *detectorAddr) {
    new ADPNCS(portName, detectorAddr);
    return (asynSuccess);
}

/*
 * Callback function fired when IOC is terminated.
 * Deletes the driver object and performs cleanup
 *
 * @params[in]: pPvt -> pointer to the driver object created in ADPNCSConfig
 * @return:     void
 */
static void exitCallbackC(void *pPvt) {
    ADPNCS *pPNCS = (ADPNCS *)pPvt;
    delete (pPNCS);
}

/**
 * @brief Wrapper C function passed to epicsThreadCreate to create acquisition thread
 *
 * @param drvPvt Pointer to instance of ADPNCS driver object
 */
static void acquisitionThreadC(void *drvPvt) {
    ADPNCS *pPvt = (ADPNCS *)drvPvt;
    pPvt->acquisitionThread();
}

static void monitorThreadC(void *drvPvt) {
    ADPNCS *pPvt = (ADPNCS *)drvPvt;
    pPvt->monitorThread();
}
// -----------------------------------------------------------------------
// ADPNCS Acquisition Functions
// -----------------------------------------------------------------------

/**
 * Function that spawns new acquisition thread, if able
 */
/**
 * @brief Starts acquisition by spawning main acq thread
 */
void ADPNCS::acquireStart() {
    const char *functionName = "acquireStart";

    // Spawn the acquisition thread. Make sure it's joinable.
    INFO("Spawning main acquisition thread...");

    epicsThreadOpts opts;
    opts.priority = epicsThreadPriorityHigh;
    opts.stackSize = epicsThreadGetStackSize(epicsThreadStackBig);
    opts.joinable = 1;

    this->acquisitionThreadId =
        epicsThreadCreateOpt("acquisitionThread", (EPICSTHREADFUNC)acquisitionThreadC, this, &opts);
}

void ADPNCS::monitorThread() {
    const char *functionName = "monitorThread";

    pncs::API api = pncs::API(this->detectorAddr, 50054, this->detectorAddr, 50052, "test");
    papi = &api;
    papi->Connect();

    while (this->alive) {
        if (this->papi == nullptr) {
            ERR("API was destroyed!");
            sleep(1);
            continue;
        } else {
            parseStatus();
        }
        sleep(2);
    }
}

/**
 * @brief Main acquisition function for ADKinetix
 */
void ADPNCS::acquisitionThread() {
    const char *functionName = "acquisitionThread";

    NDDataType_t dataType;
    NDColorMode_t colorMode;
    NDArrayInfo_t arrayInfo;
    NDArray *pArray = NULL;
    getIntegerParam(NDColorMode, (int *)&colorMode);
    getIntegerParam(NDDataType, (int *)&dataType);

    int ndims = 3;                                // For color
    if (colorMode == NDColorModeMono) ndims = 2;  // For monochrome

    size_t dims[ndims];
    if (ndims == 2) {
        getIntegerParam(ADSizeX, (int *)&dims[0]);
        getIntegerParam(ADSizeY, (int *)&dims[1]);
    } else {
        dims[0] = 3;
        getIntegerParam(ADSizeX, (int *)&dims[1]);
        getIntegerParam(ADSizeY, (int *)&dims[2]);
    }

    int collectedImages = 0;

    // Start the acquisition given resolution, data type, color mode here
    this->acquisitionActive = true;

    while (acquisitionActive) {
        setIntegerParam(ADStatus, ADStatusAcquire);

        // Get a new frame using the vendor SDK here here

        // Allocate the NDArray of the correct size
        this->pArrays[0] = pNDArrayPool->alloc(ndims, dims, dataType, 0, NULL);
        if (this->pArrays[0] != NULL) {
            pArray = this->pArrays[0];
        } else {
            this->pArrays[0]->release();
            ERR("Failed to allocate array!");
            setIntegerParam(ADStatus, ADStatusError);
            callParamCallbacks();
            break;
        }

        collectedImages += 1;
        setIntegerParam(ADNumImagesCounter, collectedImages);
        updateTimeStamp(&pArray->epicsTS);

        // Set array size PVs based on collected frame
        pArray->getInfo(&arrayInfo);
        setIntegerParam(NDArraySize, (int)arrayInfo.totalBytes);
        setIntegerParam(NDArraySizeX, arrayInfo.xSize);
        setIntegerParam(NDArraySizeY, arrayInfo.ySize);

        // Copy data from new frame to pArray
        // memcpy(pArray->pData, POINTER_TO_FRAME_DATA, arrayInfo.totalBytes);

        // increment the array counter
        int arrayCounter;
        getIntegerParam(NDArrayCounter, &arrayCounter);
        arrayCounter++;
        setIntegerParam(NDArrayCounter, arrayCounter);

        // set the image unique ID to the number in the sequence
        pArray->uniqueId = arrayCounter;
        pArray->pAttributeList->add("ColorMode", "Color Mode", NDAttrInt32, &colorMode);

        getAttributes(pArray->pAttributeList);
        doCallbacksGenericPointer(pArray, NDArrayData, 0);

        // If in single mode, finish acq, if in multiple mode and reached target number
        // complete acquisition.
        // if (acquisitionMode == ADImageSingle) {
        //     this->acquisitionActive = false;
        // } else if (acquisitionMode == ADImageMultiple && collectedImages == targetNumImages) {
        //     this->acquisitionActive = false;
        // }
        // Release the array
        pArray->release();

        // refresh all PVs
        callParamCallbacks();
    }

    setIntegerParam(ADStatus, ADStatusIdle);
    setIntegerParam(ADAcquire, 0);
    callParamCallbacks();
}

/**
 * @brief stops acquisition by aborting exposure and joining acq thread
 */
void ADPNCS::acquireStop() {
    const char *functionName = "acquireStop";

    if (this->acquisitionActive) {
        // Mark acquisition as inactive
        this->acquisitionActive = false;

        // Wait for acquisition thread to join
        INFO("Waiting for acquisition thread to join...");
        epicsThreadMustJoin(this->acquisitionThreadId);
        INFO("Acquisition stopped.");

        // Refresh all PV values
        callParamCallbacks();
    } else {
        WARN("Acquisition is not active!");
    }
}

//-------------------------------------------------------------------------
// ADDriver function overwrites
//-------------------------------------------------------------------------

/*
 * Function overwriting ADDriver base function.
 * Takes in a function (PV) changes, and a value it is changing to, and processes the input
 *
 * @params[in]: pasynUser       -> asyn client who requests a write
 * @params[in]: value           -> int32 value to write
 * @return: asynStatus      -> success if write was successful, else failure
 */
asynStatus ADPNCS::writeInt32(asynUser *pasynUser, epicsInt32 value) {
    int function = pasynUser->reason;
    int acquiring;
    asynStatus status = asynSuccess;
    static const char *functionName = "writeInt32";
    getIntegerParam(ADAcquire, &acquiring);

    // Certain functions can only be performed in off/on states
    int powerState;
    getIntegerParam(ADPNCS_PowerState, &powerState);

    int calibrating;
    getIntegerParam(ADPNCS_Calibrate, &calibrating);

    // start/stop acquisition
    if (function == ADAcquire) {
        if (value && !acquiring) {
            acquireStart();
        }
        if (!value && acquiring) {
            acquireStop();
        }
    } else if (function == ADPNCS_PowerState) {
        if (acquiring == 1) acquireStop();
        if (value == 1) {
            papi->TurnOn();
        } else {
            papi->TurnOff();
        }
    } else if (function == ADPNCS_Calibrate) {
        if (acquiring == 1) acquireStop();
        if (value == 1) {
            papi->StartCalibration();
        } else {
            papi->StopCalibration();
        }
    } else if (function == ADPNCS_BeamBlankState) {
        if (acquiring == 1) acquireStop();
        if (value == 1) {
            papi->ConfirmBeamBlank();
        } else {
            papi->ConfirmBeamUnblank();
        }
    } else if (function == ADPNCS_VoltageState) {
        if (acquiring == 1) acquireStop();
        if (powerState == 0) {
            if (value == 1)
                papi->VoltageOn();
            else
                papi->VoltageOff();
        } else {
            ERR_TO_STATUS("Can't toggle voltage while power is ON!");
            status = asynError;
        }
    } else if (function == ADPNCS_CoolingState) {
        if (acquiring == 1) acquireStop();
        if (powerState == 0) {
            if (value == 1)
                papi->CoolDown();
            else
                papi->StopCooling();
        } else {
            ERR_TO_STATUS("Can't toggle cooling while power is ON!");
            status = asynError;
        }
    } else if (function == ADPNCS_HeatingState) {
        if (acquiring == 1) acquireStop();
        if (powerState == 0) {
            if (value == 1)
                papi->HeatUp();
            else
                papi->StopHeating();
        } else {
            ERR_TO_STATUS("Can't toggle heating while power is ON!");
            status = asynError;
        }
    } else if (function == ADImageMode) {
        if (acquiring == 1) acquireStop();

    } else {
        if (function < ADPNCS_FIRST_PARAM) {
            status = ADDriver::writeInt32(pasynUser, value);
        }
    }
    callParamCallbacks();

    if (status) {
        ERR_ARGS("status=%d, function=%d, value=%d", status, function, value);
    } else
        DEBUG_ARGS("function=%d value=%d\n", function, value);

    return status;
}

/*
 * Function overwriting ADDriver base function.
 * Takes in a function (PV) changes, and a value it is changing to, and processes the input
 * This is the same functionality as writeInt32, but for processing doubles.
 *
 * @params[in]: pasynUser       -> asyn client who requests a write
 * @params[in]: value           -> int32 value to write
 * @return: asynStatus      -> success if write was successful, else failure
 */
asynStatus ADPNCS::writeFloat64(asynUser *pasynUser, epicsFloat64 value) {
    int function = pasynUser->reason;
    int acquiring;
    asynStatus status = asynSuccess;
    static const char *functionName = "writeFloat64";
    getIntegerParam(ADAcquire, &acquiring);

    if (function == ADAcquireTime) {
        if (acquiring == 1) acquireStop();
    } else {
        if (function < ADPNCS_FIRST_PARAM) {
            status = ADDriver::writeFloat64(pasynUser, value);
        }
    }
    callParamCallbacks();

    if (status) {
        ERR_ARGS("status=%d, function=%d, value=%d", status, function, value);
    } else
        DEBUG_ARGS("function=%d value=%d\n", function, value);

    return status;
}

asynStatus ADPNCS::readEnum(asynUser *pasynUser, char *strings[], int values[], int severities[],
                            size_t nElements, size_t *nIn) {
    int function = pasynUser->reason;
    const char *functionName = "readEnum";

    // Clear previous maps
    gainToIdxMap.clear();
    speedToIdxMap.clear();
    windowBinToIdxMap.clear();

    *nIn = 0;
    if (function == ADPNCS_GainMode) {
        std::vector<std::string> gains = this->papi->GetAvailableGains();
        std::cout << gains << std::endl;
        for (size_t i = 0; i < gains.size(); i++) {
            strings[i] = epicsStrDup(gains[i].c_str());
            gainToIdxMap[gains[i]] = (int)i;
            values[i] = (int)i;
            severities[i] = 0;
            (*nIn)++;
        }
    } else if (function == ADPNCS_FrameRate) {
        std::vector<std::string> speeds = this->papi->GetAvailableSpeeds();
        std::cout << speeds << std::endl;
        for (size_t i = 0; i < speeds.size(); i++) {
            strings[i] = epicsStrDup(speeds[i].c_str());
            speedToIdxMap[speeds[i]] = (int)i;
            values[i] = (int)i;
            severities[i] = 0;
            (*nIn)++;
        }
    } else if (function == ADPNCS_WindowBinMode) {
        std::vector<std::string> modes = this->papi->GetAvailableWindowingBinning();
        std::cout << modes << std::endl;
        for (size_t i = 0; i < modes.size(); i++) {
            strings[i] = epicsStrDup(modes[i].c_str());
            windowBinToIdxMap[modes[i]] = (int)i;
            values[i] = (int)i;
            severities[i] = 0;
            (*nIn)++;
        }
    } else {
        return asynError;
    }
    return asynSuccess;
}

void ADPNCS::report(FILE *fp, int details) {
    const char *functionName = "report";
    if (details > 0) {
        ADDriver::report(fp, details);
    }
}

void ADPNCS::parseStatus() {
    const char *functionName = "parseStatus";
    auto status = papi->GetStatus();

    pncs::types::json::JSON hardwareConfig = status.Get<pncs::types::json::JSON>(
        "/parameter_storage_content/hardware_config", pncs::types::json::JSON());
    setStringParam(ADModel, hardwareConfig.Get<std::string>("/system_flavor", "Unknown"));

    pncs::types::json::JSON hardwareStatus = status.Get<pncs::types::json::JSON>(
        "/parameter_storage_content/hardware_status", pncs::types::json::JSON());

    // Get number of columns in x and y
    int xMaxSize = hardwareStatus.Get<int>("/area_columns", 0);
    int yMaxSize = hardwareStatus.Get<int>("/area_rows", 0);
    setIntegerParam(ADMaxSizeX, xMaxSize);
    setIntegerParam(ADMaxSizeY, yMaxSize);

    int xSize = hardwareStatus.Get<int>("/frame_dims/x", 0);
    int ySize = hardwareStatus.Get<int>("/frame_dims/y", 0);
    setIntegerParam(ADSizeX, xSize);
    setIntegerParam(ADSizeY, ySize);

    setDoubleParam(ADPNCS_TempSetpoint, hardwareStatus.Get<double>("/operating_temperature", 0.0));
    setIntegerParam(ADPNCS_Temp, hardwareStatus.Get<int>("/pnbrain/temperature/detector", 0));
    setIntegerParam(ADPNCS_HeatsinkTemp, hardwareStatus.Get<int>("/pnbrain/temperature/sink", 0));

    pncs::types::json::JSON timingSettings =
        hardwareStatus.Get<pncs::types::json::JSON>("/timing_settings", pncs::types::json::JSON());

    std::string gainMode = timingSettings.Get<std::string>("/gain", "Unknown");
    std::string speedMode = timingSettings.Get<std::string>("/speed", "Unknown");
    std::string windowBinMode = timingSettings.Get<std::string>("/windowing_binning", "Unknown");
    if (gainToIdxMap.find(gainMode) == gainToIdxMap.end()) {
        ERR_ARGS("Unknown gain mode: %s", gainMode.c_str());
    } else {
        setIntegerParam(ADPNCS_GainMode, gainToIdxMap[gainMode]);
    }
    if (speedToIdxMap.find(speedMode) == speedToIdxMap.end()) {
        ERR_ARGS("Unknown speed mode: %s", speedMode.c_str());
    } else {
        setIntegerParam(ADPNCS_FrameRate, speedToIdxMap[speedMode]);
    }
    if (windowBinToIdxMap.find(windowBinMode) == windowBinToIdxMap.end()) {
        ERR_ARGS("Unknown window/binning mode: %s", windowBinMode.c_str());
    } else {
        setIntegerParam(ADPNCS_WindowBinMode, windowBinToIdxMap[windowBinMode]);
    }
    pncs::types::json::JSON measurementStatus = status.Get<pncs::types::json::JSON>(
        "/parameter_storage_content/measurement_settings", pncs::types::json::JSON());

    bool commonModeCorrection =
        measurementStatus.Get<bool>("/common_mode_correction/enabled", false);
    setIntegerParam(ADPNCS_CommonModeCorrection, commonModeCorrection ? 1 : 0);

    bool dynamicCalibration = measurementStatus.Get<bool>("/dynamic_calibration", false);
    setIntegerParam(ADPNCS_DynamicCalibration, dynamicCalibration ? 1 : 0);

    double primaryThresh = measurementStatus.Get<double>("/event_analysis/primary_threshold", 0.0);
    setDoubleParam(ADPNCS_PrimaryThreshold, primaryThresh);
    double secondaryThresh =
        measurementStatus.Get<double>("/event_analysis/secondary_threshold", 0.0);
    setDoubleParam(ADPNCS_SecondaryThreshold, secondaryThresh);

    bool liveRunningCorrection = measurementStatus.Get<bool>("/live_running_correction", false);
    setIntegerParam(ADPNCS_LiveRunningCorrection, liveRunningCorrection ? 1 : 0);

    pncs::types::json::JSON softwareConfig = status.Get<pncs::types::json::JSON>(
        "/parameter_storage_content/software_config", pncs::types::json::JSON());

    pncs::types::json::JSON softwareStatus = status.Get<pncs::types::json::JSON>(
        "/parameter_storage_content/software_status", pncs::types::json::JSON());
    bool daqConnected = softwareStatus.Get<bool>("/daq/connected", false);
    bool hwConnected = softwareStatus.Get<bool>("/hwc/connected", false);
    bool pnbrainConnected = softwareStatus.Get<bool>("/pnbrain/connected", false);
    int connectionStatus = 0;
    if (daqConnected) connectionStatus = connectionStatus + 1;
    if (hwConnected) connectionStatus = connectionStatus + 2;
    if (pnbrainConnected) connectionStatus = connectionStatus + 4;
    setIntegerParam(ADPNCS_ConnectionStatus, connectionStatus);

    pncs::types::json::JSON stateMachineState = softwareStatus.Get<pncs::types::json::JSON>(
        "/master/state_machine", pncs::types::json::JSON());

    std::string baseState = stateMachineState.Get<std::string>("/state", "Unknown");
    std::string subState = stateMachineState.Get<std::string>("/substate", "Unknown");
    std::string orthoState = stateMachineState.Get<std::string>("/orthogonal_state", "Unknown");

    if (baseState == "ready" || baseState == "cold") {
        setIntegerParam(ADPNCS_PowerState, 1);
        if (orthoState == "busy") {
            setIntegerParam(ADStatus, ADStatusWaiting);
        } else if (subState == "operating") {
            setIntegerParam(ADStatus, ADStatusIdle);
        } else if (subState == "live_view") {
            setIntegerParam(ADStatus, ADStatusAcquire);
        }

        if (subState == "calibrating") {
            setIntegerParam(ADPNCS_Calibrate, 1);
            if (orthoState == "all_ok") {
                INFO_TO_STATUS("Calibration complete");
                this->papi->StopCalibration();
            } else {
                INFO_TO_STATUS("Waiting for beam blank");
            }
        } else {
            setIntegerParam(ADPNCS_Calibrate, 0);
        }
    } else {
        setIntegerParam(ADPNCS_PowerState, 0);
    }

    if (orthoState == "error") {
        setIntegerParam(ADStatus, ADStatusError);
    }

    std::string cameraState = baseState + ", " + subState + ", " + orthoState;
    setStringParam(ADPNCS_State, cameraState.c_str());

    callParamCallbacks();
}

//----------------------------------------------------------------------------
// ADPNCS Constructor/Destructor
//----------------------------------------------------------------------------

ADPNCS::ADPNCS(const char *portName, const char *detectorAddr)
    : ADDriver(portName, 1, (int)NUM_PNCS_PARAMS, 0, 0, asynEnumMask, asynEnumMask, ASYN_CANBLOCK,
               1, 0, 0) {
    static const char *functionName = "ADPNCS";
    createAllParams();

    // Sets driver version PV (version numbers defined in header file)
    char versionString[25];
    epicsSnprintf(versionString, sizeof(versionString), "%d.%d.%d", ADPNCS_VERSION, ADPNCS_REVISION,
                  ADPNCS_MODIFICATION);
    setStringParam(NDDriverVersion, versionString);
    setStringParam(ADManufacturer, "PNDetector GmbH");

    this->detectorAddr = detectorAddr;

    epicsThreadOpts opts;
    opts.priority = epicsThreadPriorityMedium;
    opts.stackSize = epicsThreadGetStackSize(epicsThreadStackMedium);
    opts.joinable = 1;

    this->monitorThreadId =
        epicsThreadCreateOpt("monitorThread", (EPICSTHREADFUNC)monitorThreadC, this, &opts);

    // when epics is exited, delete the instance of this class
    epicsAtExit(exitCallbackC, this);
}

ADPNCS::~ADPNCS() {
    const char *functionName = "~ADPNCS";

    INFO("Shutting down ADPNCS driver...");
    if (this->acquisitionActive && this->acquisitionThreadId != NULL) acquireStop();
    this->alive = false;
    if (this->monitorThreadId != NULL) {
        INFO("Waiting for monitor thread to join...");
        epicsThreadMustJoin(this->monitorThreadId);
        INFO("Monitor thread joined.");
    }

    // Destroy any resources allocated by the vendor SDK here
    INFO("Done.");
}

//-------------------------------------------------------------
// ADPNCS ioc shell registration
//-------------------------------------------------------------

/* PNCSConfig -> These are the args passed to the constructor in the epics config function */
static const iocshArg PNCSConfigArg0 = {"Port name", iocshArgString};

// This parameter must be customized by the driver author. Generally a URL, Serial Number, ID, IP
// are used to connect.
static const iocshArg PNCSConfigArg1 = {"Connection Param", iocshArgString};

/* Array of config args */
static const iocshArg *const PNCSConfigArgs[] = {&PNCSConfigArg0, &PNCSConfigArg1};

/* what function to call at config */
static void configPNCSCallFunc(const iocshArgBuf *args) {
    ADPNCSConfig(args[0].sval, args[1].sval);
}

/* information about the configuration function */
static const iocshFuncDef configPNCS = {"ADPNCSConfig", 2, PNCSConfigArgs};

/* IOC register function */
static void PNCSRegister(void) { iocshRegister(&configPNCS, configPNCSCallFunc); }

/* external function for IOC register */
extern "C" {
epicsExportRegistrar(PNCSRegister);
}
