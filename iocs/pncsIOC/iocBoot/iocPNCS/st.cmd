#!/bin/bash

# Export library path in case building against shared libraries
export LD_LIBRARY_PATH=../../../../lib/linux-x86_64:/opt/grpc/lib:/opt/grpc/lib64:/opt/pncs_sdk/lib:$LD_LIBRARY_PATH

../../bin/linux-x86_64/PNDetectorCXCApp st_base.cmd
