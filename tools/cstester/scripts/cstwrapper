#!/bin/bash
# This wrapper is there to limit the resources used by cstester and make sure the processing stops

# make sure to stop processing of cstester to 60 seconds
ulimit -t 60
# make sure not to eat all available memory
ulimit -v 1269760

cstester $@
