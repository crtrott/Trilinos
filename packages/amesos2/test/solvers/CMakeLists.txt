

TRIBITS_ADD_EXECUTABLE(
  Solver_Test
  SOURCES Solver_Test
  )


##### KLU2 Tests #####

IF (${PACKAGE_NAME}_ENABLE_KLU2)
    TRIBITS_ADD_EXECUTABLE_AND_TEST(
    KLU2_UnitTests
    SOURCES
      KLU2_UnitTests
      ${TEUCHOS_STD_UNIT_TEST_MAIN}
    COMM serial mpi
    CATEGORIES NIGHTLY
    STANDARD_PASS_OUTPUT
    )
ENDIF()

##### SuperLu Tests #####


IF (${PACKAGE_NAME}_ENABLE_SuperLU)

  TRIBITS_COPY_FILES_TO_BINARY_DIR(SolverTestCopySuperLUFiles
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    SOURCE_FILES superlu_test.xml
    EXEDEPS Solver_Test
    )

    
  TRIBITS_ADD_TEST(
    Solver_Test
    NAME SuperLU_Solver_Test
    ARGS "--xml-params=superlu_test.xml --filedir=${CMAKE_CURRENT_BINARY_DIR}/../matrices/ --multiple-solves --refactor"
    STANDARD_PASS_OUTPUT
    COMM serial mpi
    )


  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    Superlu_UnitTests
    SOURCES
      Superlu_UnitTests
      ${TEUCHOS_STD_UNIT_TEST_MAIN}
    COMM serial mpi
    CATEGORIES NIGHTLY
    STANDARD_PASS_OUTPUT
    )

ENDIF()


##### SuperLU_MT Tests #####

IF (${PACKAGE_NAME}_ENABLE_SuperLUMT)
  TRIBITS_COPY_FILES_TO_BINARY_DIR(SolverTestCopySuperLUMTFiles
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    SOURCE_FILES superlumt_test.xml
    EXEDEPS Solver_Test
    )

  TRIBITS_ADD_TEST(
    Solver_Test
    NAME SuperLU_MT_Solver_Test
    ARGS "--xml-params=superlumt_test.xml --filedir=${CMAKE_CURRENT_BINARY_DIR}/../matrices/ --multiple-solves --refactor"
    STANDARD_PASS_OUTPUT
    COMM serial mpi
    )
ENDIF()


##### SuperLU_DIST Tests #####


IF (${PACKAGE_NAME}_ENABLE_SuperLUDist)
  TRIBITS_COPY_FILES_TO_BINARY_DIR(SolverTestCopySuperLUDistFiles
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    SOURCE_FILES superludist_test.xml
    EXEDEPS Solver_Test
    )

  TRIBITS_ADD_TEST(
    Solver_Test
    NAME SuperLU_DIST_Solver_Test
    ARGS "--xml-params=superludist_test.xml --filedir=${CMAKE_CURRENT_BINARY_DIR}/../matrices/ --multiple-solves --refactor"
    STANDARD_PASS_OUTPUT
    COMM mpi
    )
ENDIF()


##### PARDISO_MKL Tests #####

IF (${PACKAGE_NAME}_ENABLE_PARDISO_MKL)
  TRIBITS_COPY_FILES_TO_BINARY_DIR(SolverTestCopyPardisoMKLFiles
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    SOURCE_FILES pardiso_mkl_test.xml
    EXEDEPS Solver_Test
    )

  TRIBITS_ADD_TEST(
    Solver_Test
    NAME Pardiso_MKL_Solver_Test
    ARGS "--xml-params=pardiso_mkl_test.xml --filedir=${CMAKE_CURRENT_BINARY_DIR}/../matrices/ --multiple-solves --refactor"
    STANDARD_PASS_OUTPUT
    COMM serial mpi
    )
ENDIF()


##### LAPACK Tests #####

IF (${PACKAGE_NAME}_ENABLE_LAPACK)
  TRIBITS_COPY_FILES_TO_BINARY_DIR(SolverTestCopyLapackFiles
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    SOURCE_FILES lapack_test.xml
    EXEDEPS Solver_Test
    )

  TRIBITS_ADD_TEST(
    Solver_Test
    NAME LAPACK_Solver_Test
    ARGS "--xml-params=lapack_test.xml --filedir=${CMAKE_CURRENT_BINARY_DIR}/../matrices/ --multiple-solves --refactor"
    STANDARD_PASS_OUTPUT
    COMM serial mpi
    )
ENDIF()


####   BASKER Tests ####
IF (${PACKAGE_NAME}_ENABLE_Basker)

    TRIBITS_ADD_EXECUTABLE_AND_TEST(
    Basker_UnitTests
    SOURCES
      Basker_UnitTests
      ${TEUCHOS_STD_UNIT_TEST_MAIN}
    COMM serial mpi
    STANDARD_PASS_OUTPUT
    )

IF (Tpetra_INST_INT_INT)

   TRIBITS_COPY_FILES_TO_BINARY_DIR(Amesos2BaskerCopyFiles
     DEST_FILES
     test_basker_wathen120_mm.xml
     
     SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
     DEST_DIR   ${CMAKE_CURRENT_BINARY_DIR}
    )
   
   TRIBITS_ADD_EXECUTABLE(
    Solver_Test_Basker
    SOURCES Solver_Test_Basker
   )


ENDIF()
ENDIF()

##### MUMPS Tests ####
#MUMPS requires int/int type
IF (Tpetra_INST_INT_INT)
IF (${PACKAGE_NAME}_ENABLE_MUMPS)
   TRIBITS_ADD_EXECUTABLE_AND_TEST(
   MUMPS_UnitTests
   SOURCES
      MUMPS_UnitTests
      ${TEUCHOS_STD_UNIT_TEST_MAIN}
   COMM serial mpi
   STANDARD_PASS_OUTPUT
   )
ENDIF()
ENDIF()


ASSERT_DEFINED (Trilinos_ENABLE_LINEAR_SOLVER_FACTORY_REGISTRATION)
IF (Trilinos_ENABLE_LINEAR_SOLVER_FACTORY_REGISTRATION)
  # SolverFactory test (doesn't make sense to run unless Trilinos is
  # doing run-time registration of solver factories).
  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    SolverFactory_UnitTests
    SOURCES
    SolverFactory
    ${TEUCHOS_STD_UNIT_TEST_MAIN}
    COMM serial mpi
    STANDARD_PASS_OUTPUT
    )
ENDIF ()

