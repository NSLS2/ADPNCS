/*
 * Header file for the ADPNCS EPICS driver
 *
 * This file was initially generated with the help of the ADDriverTemplate:
 * https://github.com/NSLS2/ADDriverTemplate on 07/07/2025
 *
 * Author: Jakub Wlodek
 *
 * Copyright (c) : Brookhaven National Laboratory, 2025
 *
 */

// header guard
#ifndef ADPNCS_H
#define ADPNCS_H

// version numbers
#define ADPNCS_VERSION 0
#define ADPNCS_REVISION 0
#define ADPNCS_MODIFICATION 0

typedef enum ADPNCXC_LOG_LEVEL {
    ADPNCXC_LOG_LEVEL_NONE = 0,      // No logging
    ADPNCXC_LOG_LEVEL_ERROR = 10,    // Error messages only
    ADPNCXC_LOG_LEVEL_WARNING = 20,  // Warnings and errors
    ADPNCXC_LOG_LEVEL_INFO = 30,     // Info, warnings, and errors
    ADPNCXC_LOG_LEVEL_DEBUG = 40     // Debugging information
} ADPNCXC_LogLevel_t;

// Error message formatters
#define ERR(msg)                                   \
    if (this->logLevel >= ADPNCXC_LOG_LEVEL_ERROR) \
        printf("ERROR | %s::%s: %s\n", driverName, functionName, msg);

#define ERR_ARGS(fmt, ...)                         \
    if (this->logLevel >= ADPNCXC_LOG_LEVEL_ERROR) \
        printf("ERROR | %s::%s: " fmt "\n", driverName, functionName, __VA_ARGS__);

// Warning message formatters
#define WARN(msg)                                    \
    if (this->logLevel >= ADPNCXC_LOG_LEVEL_WARNING) \
        printf("WARNING | %s::%s: %s\n", driverName, functionName, msg);

#define WARN_ARGS(fmt, ...)                          \
    if (this->logLevel >= ADPNCXC_LOG_LEVEL_WARNING) \
        printf("WARNING | %s::%s: " fmt "\n", driverName, functionName, __VA_ARGS__);

// Info message formatters. Because there is no ASYN trace for info messages, we just use `printf`
// here.
#define INFO(msg)                                 \
    if (this->logLevel >= ADPNCXC_LOG_LEVEL_INFO) \
        printf("INFO | %s::%s: %s\n", driverName, functionName, msg);

#define INFO_ARGS(fmt, ...)                       \
    if (this->logLevel >= ADPNCXC_LOG_LEVEL_INFO) \
        printf("INFO | %s::%s: " fmt "\n", driverName, functionName, __VA_ARGS__);

// Debug message formatters
#define DEBUG(msg)                                 \
    if (this->logLevel >= ADPNCXC_LOG_LEVEL_DEBUG) \
    printf("DEBUG | %s::%s: %s\n", driverName, functionName, msg)

#define DEBUG_ARGS(fmt, ...)                       \
    if (this->logLevel >= ADPNCXC_LOG_LEVEL_DEBUG) \
        printf("DEBUG | %s::%s: " fmt "\n", driverName, functionName, __VA_ARGS__);

// Define macros that correspond to string representations of PVs from EPICS database template here.
// For example: For a record with `OUT` field set to `@asyn($(PORT), $(ADDR=0),
// $(TIMEOUT=1))PV_NAME`, add, where the record represents an integer value: #define
// ADPNCS_PVNameString          "PV_NAME"            //asynInt32

#include <epicsExit.h>
#include <epicsExport.h>
#include <epicsStdio.h>
#include <epicsString.h>
#include <epicsThread.h>
#include <epicsTime.h>
#include <iocsh.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "ADDriver.h"
#include "pncs_api.h"
#include "rpc_lib_base/server_builder.h"
#include "types_lib/measurement.h"

/*
 * Class definition of the ADPNCS driver
 */
class ADPNCS : ADDriver {
   public:
    // Constructor for the ADPNCS driver
    pncs::API* papi;  // API reference object for communicating with PNBrain

    ADPNCS(const char* portName, const char* detectorAddr);

    // ADDriver overrides
    virtual asynStatus writeInt32(asynUser* pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser* pasynUser, epicsFloat64 value);
    virtual void report(FILE* fp, int details);

    // Destructor. Disconnects from the detector and performs cleanup
    ~ADPNCS();
    void acquisitionThread();
    void monitorThread();

   protected:
// Load auto-generated parameter string and index definitions
#include "ADPNCSParamDefs.h"

   private:
    ADPNCXC_LogLevel_t logLevel = ADPNCXC_LOG_LEVEL_INFO;  // Logging level for the driver

    const char* detectorAddr;  // Address of the PNBrain detector
    bool acquisitionActive;    // Flag to indicate if acquisition is active
    bool alive = true;         // Flag to control the monitor thread
    epicsThreadId acquisitionThreadId,
        monitorThreadId;  // Thread IDs for acquisition and monitoring threads

    // void parseStatus(const pncs::types::json::JSON &status);
    void parseStatus();
    void acquireStart();
    void acquireStop();
    void createAllParams();
};

// Stores number of additional PV parameters are added by the driver
#define NUM_PNCS_PARAMS ((int)(&ADPNCS_LAST_PARAM - &ADPNCS_FIRST_PARAM + 1))

#endif
