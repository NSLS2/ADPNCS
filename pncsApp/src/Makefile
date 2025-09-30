TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

ifeq (windows-x64, $(findstring windows-x64, $(T_A)))
USR_CPPFLAGS += -D__X64
endif

USR_CPPFLAGS += -std=c++23 -I$(PNCS_INCLUDE) -I$(GRPC_INCLUDE)
USR_LDFLAGS += -L$(PNCS_LIB) -L$(GRPC_LIB) -L$(GRPC_LIB64)

INC += ADPNCS.h ADPNCSParamDefs.h

LIBRARY_IOC = ADPNCS
LIB_SRCS += ADPNCS.cpp ADPNCSParamDefs.cpp


DBD += pncsSupport.dbd

include $(ADCORE)/ADApp/commonLibraryMakefile

LIB_LIBS += date

LIB_SYS_LIBS += pncs_cpp_api grpc grpc++

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
