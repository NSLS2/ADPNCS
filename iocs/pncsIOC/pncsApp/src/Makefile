TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

PROD_NAME = pncsApp
PROD_IOC += $(PROD_NAME)

USR_CPPFLAGS += -std=c++23 -I$(PNCS_INCLUDE) -I$(GRPC_INCLUDE)
USR_LDFLAGS += -L$(PNCS_LIB) -L$(GRPC_LIB) -L$(GRPC_LIB64) -Wl,-rpath,$(PNCS_LIB) -Wl,-rpath,$(GRPC_LIB) -Wl,-rpath,$(GRPC_LIB64)

DBD += $(PROD_NAME).dbd
$(PROD_NAME)_DBD += pncsSupport.dbd

$(PROD_NAME)_SRCS += $(PROD_NAME)_registerRecordDeviceDriver.cpp $(PROD_NAME)Main.cpp

# Add any libraries built by the EPICS build system here
$(PROD_NAME)_LIBS += ADPNCS date

# Add any system libraries here
$(PROD_NAME)_SYS_LIBS += grpc grpc++ pncs_cpp_api

include $(ADCORE)/ADApp/commonDriverMakefile

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
