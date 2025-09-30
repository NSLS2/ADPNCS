TOP=..
include $(TOP)/configure/CONFIG

#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

SRC_DIRS += ..

INC += nlohmann/json.hpp
INC += nlohmann/json_fwd.hpp

SRC_DIRS += ../date

USR_CPPFLAGS += -std=c++23

LIBRARY_IOC = date

INC += date/date.h
INC += date/tz.h
INC += date/iso_week.h
INC += date/chrono_io.h
INC += date/ios.h
INC += date/julian.h
INC += date/iso_week.h
INC += date/ptz.h
INC += date/tz.h
INC += date/tz_private.h
INC += date/solar_hijri.h

LIB_SRCS += tz.cpp

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE