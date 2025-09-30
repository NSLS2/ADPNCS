// This file is auto-generated. Do not edit directly.
// Generated from ADPNCS.template

#include "ADPNCS.h"

void ADPNCS::createAllParams() {
    createParam(ADPNCS_BeamBlankStateString, asynParamInt32, &ADPNCS_BeamBlankState);
    createParam(ADPNCS_HeatsinkTemperatureString, asynParamFloat64, &ADPNCS_HeatsinkTemperature);
    createParam(ADPNCS_LiveRunningCorrectionString, asynParamInt32, &ADPNCS_LiveRunningCorrection);
    createParam(ADPNCS_TemperatureString, asynParamFloat64, &ADPNCS_Temperature);
    createParam(ADPNCS_HeatingStateString, asynParamInt32, &ADPNCS_HeatingState);
    createParam(ADPNCS_CalibrateString, asynParamInt32, &ADPNCS_Calibrate);
    createParam(ADPNCS_CameraModeString, asynParamInt32, &ADPNCS_CameraMode);
    createParam(ADPNCS_CommonModeCorrectionString, asynParamInt32, &ADPNCS_CommonModeCorrection);
    createParam(ADPNCS_ConnectionStatusString, asynParamInt32, &ADPNCS_ConnectionStatus);
    createParam(ADPNCS_PowerStateString, asynParamInt32, &ADPNCS_PowerState);
    createParam(ADPNCS_CoolingStateString, asynParamInt32, &ADPNCS_CoolingState);
    createParam(ADPNCS_VoltageStateString, asynParamInt32, &ADPNCS_VoltageState);
    createParam(ADPNCS_PrimaryThresholdString, asynParamFloat64, &ADPNCS_PrimaryThreshold);
    createParam(ADPNCS_TemperatureSetpointString, asynParamFloat64, &ADPNCS_TemperatureSetpoint);
    createParam(ADPNCS_SecondaryThresholdString, asynParamFloat64, &ADPNCS_SecondaryThreshold);
    createParam(ADPNCS_DynamicCalibrationString, asynParamInt32, &ADPNCS_DynamicCalibration);
    createParam(ADPNCS_StateString, asynParamOctet, &ADPNCS_State);
}
