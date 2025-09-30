// This file is auto-generated. Do not edit directly.
// Generated from ADPNCS.template

#include "ADPNCS.h"

void ADPNCS::createAllParams() {
    createParam(ADPNCS_FrameRateString, asynParamInt32, &ADPNCS_FrameRate);
    createParam(ADPNCS_BeamBlankStateString, asynParamInt32, &ADPNCS_BeamBlankState);
    createParam(ADPNCS_TempSetpointString, asynParamFloat64, &ADPNCS_TempSetpoint);
    createParam(ADPNCS_SecondaryThresholdString, asynParamFloat64, &ADPNCS_SecondaryThreshold);
    createParam(ADPNCS_ConnectionStatusString, asynParamInt32, &ADPNCS_ConnectionStatus);
    createParam(ADPNCS_CameraModeString, asynParamInt32, &ADPNCS_CameraMode);
    createParam(ADPNCS_WindowBinModeString, asynParamInt32, &ADPNCS_WindowBinMode);
    createParam(ADPNCS_CommonModeCorrectionString, asynParamInt32, &ADPNCS_CommonModeCorrection);
    createParam(ADPNCS_PrimaryThresholdString, asynParamFloat64, &ADPNCS_PrimaryThreshold);
    createParam(ADPNCS_HeatsinkTempString, asynParamInt32, &ADPNCS_HeatsinkTemp);
    createParam(ADPNCS_CalibrateString, asynParamInt32, &ADPNCS_Calibrate);
    createParam(ADPNCS_LiveRunningCorrectionString, asynParamInt32, &ADPNCS_LiveRunningCorrection);
    createParam(ADPNCS_StateString, asynParamOctet, &ADPNCS_State);
    createParam(ADPNCS_TempString, asynParamInt32, &ADPNCS_Temp);
    createParam(ADPNCS_HeatingStateString, asynParamInt32, &ADPNCS_HeatingState);
    createParam(ADPNCS_DynamicCalibrationString, asynParamInt32, &ADPNCS_DynamicCalibration);
    createParam(ADPNCS_PowerStateString, asynParamInt32, &ADPNCS_PowerState);
    createParam(ADPNCS_GainModeString, asynParamInt32, &ADPNCS_GainMode);
    createParam(ADPNCS_VoltageStateString, asynParamInt32, &ADPNCS_VoltageState);
    createParam(ADPNCS_CoolingStateString, asynParamInt32, &ADPNCS_CoolingState);
}
