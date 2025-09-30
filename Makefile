#Makefile at top of application tree
TOP = .
include $(TOP)/configure/CONFIG
DIRS := $(DIRS) configure
DIRS := $(DIRS) pncsApp
DIRS := $(DIRS) pncsSupport

pncsApp_DEPEND_DIRS += pncsSupport
ifeq ($(BUILD_IOCS), YES)
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard iocs))
iocs_DEPEND_DIRS += pncsApp
endif
include $(TOP)/configure/RULES_TOP

uninstall: uninstall_iocs
uninstall_iocs:
	$(MAKE) -C iocs uninstall
.PHONY: uninstall uninstall_iocs

realuninstall: realuninstall_iocs
realuninstall_iocs:
	$(MAKE) -C iocs realuninstall
.PHONY: realuninstall realuninstall_iocs

bobfiles:
	epicsdb2bob pncsApp/Db pncsApp/op/bob -m P=DEV:PNCS1: R=cam1: PORT=PNCS1 ADDR=0 TIMEOUT=1 -d -r _RBV -t none --macro_level launcher -b pncsApp/op/bob

paramdefs:
	scripts/generate_param_defs.py pncsApp/Db/ADPNCS.template pncsApp/src
