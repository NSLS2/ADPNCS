#ifndef ADPNCS_PARAM_DEFS_H
#define ADPNCS_PARAM_DEFS_H

// This file is auto-generated. Do not edit directly.
// Generated from ADPNCS.template

// String definitions for parameters
#define ADPNCS_TemperatureSetpointString "CXC_TEMPERATURE_SETPOINT"
#define ADPNCS_LiveRunningCorrectionString "CXC_LIVE_RUNNING_CORRECTION"
#define ADPNCS_VoltageStateString "CXC_VOLTAGE_STATE"
#define ADPNCS_CoolingStateString "CXC_COOLING_STATE"
#define ADPNCS_SecondaryThresholdString "CXC_SECONDARY_THRESHOLD"
#define ADPNCS_PowerStateString "CXC_POWER_STATE"
#define ADPNCS_CalibrateString "CXC_CALIBRATE"
#define ADPNCS_HeatsinkTemperatureString "CXC_HEATSINK_TEMPERATURE"
#define ADPNCS_PrimaryThresholdString "CXC_PRIMARY_THRESHOLD"
#define ADPNCS_DynamicCalibrationString "CXC_DYNAMIC_CALIBRATION"
#define ADPNCS_CommonModeCorrectionString "CXC_COMMON_MODE_CORRECTION"
#define ADPNCS_StateString "CXC_STATE"
#define ADPNCS_CameraModeString "CXC_CAMERA_MODE"
#define ADPNCS_HeatingStateString "CXC_HEATING_STATE"
#define ADPNCS_BeamBlankStateString "CXC_BEAM_BLANK_STATE"
#define ADPNCS_ConnectionStatusString "CXC_CONNECTION_STATUS"
#define ADPNCS_TemperatureString "CXC_TEMPERATURE"

// Parameter index definitions
int ADPNCS_TemperatureSetpoint;
int ADPNCS_LiveRunningCorrection;
int ADPNCS_VoltageState;
int ADPNCS_CoolingState;
int ADPNCS_SecondaryThreshold;
int ADPNCS_PowerState;
int ADPNCS_Calibrate;
int ADPNCS_HeatsinkTemperature;
int ADPNCS_PrimaryThreshold;
int ADPNCS_DynamicCalibration;
int ADPNCS_CommonModeCorrection;
int ADPNCS_State;
int ADPNCS_CameraMode;
int ADPNCS_HeatingState;
int ADPNCS_BeamBlankState;
int ADPNCS_ConnectionStatus;
int ADPNCS_Temperature;

#define ADPNCS_FIRST_PARAM ADPNCS_TemperatureSetpoint
#define ADPNCS_LAST_PARAM ADPNCS_Temperature

#define NUM_ADPNCS_PARAMS ((int)(&ADPNCS_LAST_PARAM - &ADPNCS_FIRST_PARAM + 1))

#endif
