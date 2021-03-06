
INCLUDE(TribitsLibraryMacros)

#
# A) Package-specific configuration options
#

# TRIBITS_CONFIGURE_FILE(${PACKAGE_NAME}_config.h)

#
# B) Define the header and source files (and directories)
#

#
# src
#

SET(HEADERS "")
SET(SOURCES "")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../)

#
# Core headers
#

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

APPEND_SET(HEADERS
FArray.hpp
PrintTimer.hpp
Timer.hpp
Trace.hpp
TimerMetricTraits.hpp
WriterExt.hpp
Writer_fwd.hpp
Writer.hpp
WriterManip.hpp
WriterOStream.hpp
WriterParser.hpp
Platform.hpp
Env.hpp
Option.hpp
Mapv.hpp
PrintTable.hpp
SlibDiagWriter.hpp
String.hpp
StringUtil.hpp
WriterRegistry.hpp
  )

APPEND_SET(SOURCES
PrintTimer.cpp
Timer.cpp
Trace.cpp
TimerMetricTraits.cpp
Writer.cpp
WriterExt.cpp
WriterManip.cpp
WriterParser.cpp
Platform.cpp
Option.cpp
PrintTable.cpp
SlibDiagWriter.cpp
String.cpp
WriterRegistry.cpp
  )

IF (TPL_ENABLE_MPI)
ENDIF()

#
# C) Define the targets for package's library(s)
#

TRIBITS_ADD_LIBRARY(
  stkclassic_util_diag
  DEPLIBS stkclassic_util_env
  NOINSTALLHEADERS ${HEADERS}
  SOURCES ${SOURCES}
  )


INSTALL(FILES ${HEADERS} DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${${PROJECT_NAME}_INSTALL_INCLUDE_DIR}/stk_util/diag/)

