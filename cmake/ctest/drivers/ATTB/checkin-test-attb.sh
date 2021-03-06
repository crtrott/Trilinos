#!/bin/bash

# Used to test Trilinos on any ATTB machine
#
# NOTE: You can pass through arguments with spaces using quotes like:
#
#   --ctest-options="-E '(Test1|Test2)'"
#
# and it will preserve the spaces correctly.  If you want to pass along
# quotes, you have to escape them like:
#
#   --ctest-options="-E \"(Test1|Test2)\""
#
# The default location for this directory tree is:
#
#  Trilinos.base
#    Trilinos    (your Trilinos soruce tree)
#    BUILDS
#      CHECKIN   (where you run this script from)
#

if [ "$TRILINOS_DIR" == "" ] ; then
  _ABS_FILE_PATH=`readlink -f $0`
  _SCRIPT_DIR=`dirname $_ABS_FILE_PATH`
  TRILINOS_DIR=$_SCRIPT_DIR/../../../..
fi

TRILINOS_DIR_ABS=$(readlink -f $TRILINOS_DIR)

DRIVERS_BASE_DIR="$TRILINOS_DIR_ABS/cmake/ctest/drivers/ATTB"

# Packages in Trilinos to disable (mostly for auotmated CI server)
DISABLE_PACKAGES=PyTrilinos,Pliris,Claps,TriKota

# Check to make sure that the env has been loaded correctly
if [ "$ATTB_ENV" == "" ] ; then
  echo "Error, must local module module load devpack/openmpi/1.10.0/gcc/4.8.4/cuda/7.5.18"
  exit 1
fi

echo "
-DTrilinos_CONFIGURE_OPTIONS_FILE:FILEPATH='$DRIVERS_BASE_DIR/ATTBDevEnv.cmake'
-DTrilinos_DISABLE_ENABLED_FORWARD_DEP_PACKAGES=ON
-DBUILD_SHARED_LIBS=ON
" > COMMON.config

#
# Built-in Primary Tested (PT) --default-builds (DO NOT MODIFY)
#

echo "
" > MPI_DEBUG.config

echo "
" > SERIAL_RELEASE.config

#
# Standard Secondary Tested (ST) --st-extra-builds (DO NOT MODIFY)
#

echo "
-DCMAKE_BUILD_TYPE=RELEASE
-DTrilinos_ENABLE_DEBUG=ON
-DTPL_ENABLE_MPI=ON
" > MPI_DEBUG_ST.config

echo "
-DCMAKE_BUILD_TYPE=RELEASE
-DTrilinos_ENABLE_DEBUG=OFF
-DTPL_ENABLE_MPI=OFF
" > SERIAL_RELEASE_ST.config

#
# --extra-builds
#

echo "
-DCMAKE_BUILD_TYPE:STRING=RELEASE
-DTrilinos_ENABLE_DEBUG:BOOL=OFF
" > MPI_RELEASE.config

# Create local defaults file if one does not exist
_LOCAL_CHECKIN_TEST_DEFAULTS=local-checkin-test-defaults.py
if [ -f $_LOCAL_CHECKIN_TEST_DEFAULTS ] ; then
  echo "File $_LOCAL_CHECKIN_TEST_DEFAULTS already exists, leaving it!"
else
  echo "Creating default file $_LOCAL_CHECKIN_TEST_DEFAULTS!"
  echo "
defaults = [
  \"-j16\",
  \"--ctest-timeout=180\",
  \"--st-extra-builds=MPI_DEBUG_ST,SERIAL_RELEASE_ST\",
  \"--disable-packages=$DISABLE_PACKAGES\",
  \"--skip-case-no-email\",
  \"--ctest-options=\\\"-E '(MueLu_ParameterListInterpreterEpetra|MueLu_ParameterListInterpreterTpetra)'\\\"\",
  ]
  " > $_LOCAL_CHECKIN_TEST_DEFAULTS
fi

#
# Invocation
#

$TRILINOS_DIR_ABS/checkin-test.py \
"$@"
