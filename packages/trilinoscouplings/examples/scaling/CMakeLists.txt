
IF(Trilinos_ENABLE_Epetra AND Trilinos_ENABLE_EpetraExt AND
   Trilinos_ENABLE_Amesos AND Trilinos_ENABLE_AztecOO AND
   Trilinos_ENABLE_IntrepidCore AND Trilinos_ENABLE_ML AND
   Trilinos_ENABLE_Pamgen)
  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    Example_Poisson_NoFE
    SOURCES example_Poisson_NoFE.cpp
      TrilinosCouplings_Pamgen_Utils.cpp
    NUM_MPI_PROCS 1
    )
IF(Trilinos_ENABLE_SEACAS)
  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    Example_Poisson
    SOURCES example_Poisson.cpp
      TrilinosCouplings_Pamgen_Utils.cpp
    NUM_MPI_PROCS 1
    )
ENDIF()

  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    Example_Poisson_BlockMaterials
    SOURCES example_Poisson_BlockMaterials.cpp
      TrilinosCouplings_Pamgen_Utils.cpp
    NUM_MPI_PROCS 1
    )
  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    Example_Poisson2D
    SOURCES example_Poisson2D.cpp
      TrilinosCouplings_Pamgen_Utils.cpp
    NUM_MPI_PROCS 1
    )

  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    Example_Maxwell
    SOURCES example_Maxwell.cpp
      TrilinosCouplings_Pamgen_Utils.cpp
    NUM_MPI_PROCS 1
    )



  TRIBITS_COPY_FILES_TO_BINARY_DIR(TestCopyPoissonFiles
  SOURCE_FILES Poisson.xml Poisson2D.xml Maxwell.xml
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}
  ) 
ENDIF()

IF(Trilinos_ENABLE_Epetra AND Trilinos_ENABLE_EpetraExt AND
   Trilinos_ENABLE_Amesos AND Trilinos_ENABLE_AztecOO AND
   Trilinos_ENABLE_IntrepidCore AND Trilinos_ENABLE_ML AND
   Trilinos_ENABLE_Pamgen AND Trilinos_ENABLE_Teko)
  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    Example_Darcy
    SOURCES example_Darcy.cpp
      TrilinosCouplings_Pamgen_Utils.cpp
    NUM_MPI_PROCS 1
    )
  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    Example_DivLSFEM
    SOURCES example_DivLSFEM.cpp
      TrilinosCouplings_Pamgen_Utils.cpp
    NUM_MPI_PROCS 1
    )
  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    Example_CurlLSFEM
    SOURCES example_CurlLSFEM.cpp
      TrilinosCouplings_Pamgen_Utils.cpp
    NUM_MPI_PROCS 1
    )
  TRIBITS_ADD_EXECUTABLE_AND_TEST(
    Example_StabilizatedADR
    SOURCES example_StabilizedADR.cpp
      TrilinosCouplings_Pamgen_Utils.cpp
    NUM_MPI_PROCS 1
    )
  TRIBITS_COPY_FILES_TO_BINARY_DIR(TestCopyFiles
  SOURCE_FILES ADR.xml
  	       Darcy.xml
               CurlLSFEMin.xml
               CurlLSFEMblock_in_block.xml
               DivLSFEMin.xml
	      
               ML_nonsym.xml
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}
  ) 
ENDIF()

IF(Trilinos_ENABLE_Epetra AND Trilinos_ENABLE_EpetraExt AND
   Trilinos_ENABLE_Amesos AND Trilinos_ENABLE_AztecOO AND
   Trilinos_ENABLE_IntrepidCore AND Trilinos_ENABLE_ML AND
   Trilinos_ENABLE_SEACAS AND Trilinos_ENABLE_STKClassic)
  TRIBITS_ADD_EXECUTABLE(
    Example_Poisson_STKclassic
    SOURCES example_Poisson_stkclassic.cpp
    )
  TRIBITS_COPY_FILES_TO_BINARY_DIR(CopyMeshFilesSTKclassic
  SOURCE_FILES unit_cube_10int_hex.exo
               unit_cube_5int_tet.exo
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}
  ) 
ENDIF()

IF(Trilinos_ENABLE_Epetra AND Trilinos_ENABLE_EpetraExt AND
   Trilinos_ENABLE_Amesos AND Trilinos_ENABLE_AztecOO AND
   Trilinos_ENABLE_IntrepidCore AND Trilinos_ENABLE_ML AND
   Trilinos_ENABLE_SEACAS AND Trilinos_ENABLE_STK
   AND Trilinos_ENABLE_STKIO AND Trilinos_ENABLE_STKMesh)
  TRIBITS_ADD_EXECUTABLE(
    Example_Poisson_STK
    SOURCES example_Poisson_stk.cpp
    )
  TRIBITS_COPY_FILES_TO_BINARY_DIR(CopyMeshFilesSTK
  SOURCE_FILES unit_cube_10int_hex.exo
               unit_cube_5int_tet.exo
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}
  ) 
ENDIF()

# ik 20 Mar 2012, mfh 17 May 2012
#
# Just build this executable; don't run it as a test (yet).
#
# Intrepid has required dependencies on Teuchos, Shards, and Sacado,
# so we don't have to name those packages explicitly here.
# Belos and Tpetra also both have required dependencies on Teuchos.
IF(Trilinos_ENABLE_Belos AND
   Trilinos_ENABLE_IntrepidCore AND
   Trilinos_ENABLE_Pamgen AND
   Trilinos_ENABLE_Tpetra)
  TRIBITS_ADD_EXECUTABLE(
    Example_Poisson_NoFE_Tpetra
    SOURCES example_Poisson_NoFE_Tpetra.cpp
      TrilinosCouplings_Pamgen_Utils.cpp
    COMM serial mpi
    )
ENDIF()

# ik 20 Mar 2012, mfh 17 May 2012
#
# Just build this executable; don't run it as a test (yet).
#
# Intrepid has required dependencies on Teuchos, Shards, and Sacado,
# so we don't have to name those packages explicitly here.
IF(Trilinos_ENABLE_AztecOO AND
   Trilinos_ENABLE_Epetra AND
   Trilinos_ENABLE_EpetraExt AND
   Trilinos_ENABLE_IntrepidCore AND
   Trilinos_ENABLE_ML AND
   Trilinos_ENABLE_Pamgen)
  TRIBITS_ADD_EXECUTABLE(
    Example_Poisson_NoFE_Epetra
    SOURCES example_Poisson_NoFE_Epetra.cpp
      TrilinosCouplings_Pamgen_Utils.cpp
    COMM serial mpi
    )
ENDIF()

# mfh 18 May 2012
#
# Just build this executable; don't run it as a test (yet).
#
# Intrepid has required dependencies on Teuchos, Shards, and Sacado,
# so we don't have to name those packages explicitly here.
# Belos and Tpetra also both have required dependencies on Teuchos.
# The executable may use MueLu only if the MueLu package is enabled.
IF(Trilinos_ENABLE_Belos AND
   Trilinos_ENABLE_IntrepidCore AND
   Trilinos_ENABLE_Pamgen AND
   Trilinos_ENABLE_Tpetra)
  TRIBITS_ADD_EXECUTABLE(
    IntrepidPoisson_Pamgen_Tpetra
    SOURCES IntrepidPoisson_Pamgen_Tpetra_main.cpp
      TrilinosCouplings_IntrepidPoissonExampleHelpers.cpp
      TrilinosCouplings_TpetraIntrepidPoissonExample.cpp
      TrilinosCouplings_TpetraIntrepidPoissonExample_SolveWithBelos.cpp
      TrilinosCouplings_Pamgen_Utils.cpp
    COMM serial mpi
    )
ENDIF()

# mfh 24 May 2012
#
# Just build this executable; don't run it as a test (yet).
#
# Intrepid has required dependencies on Teuchos, Shards, and Sacado,
# so we don't have to name those packages explicitly here.
# Belos both also have required dependencies on Teuchos.
IF(Trilinos_ENABLE_Belos AND
   Trilinos_ENABLE_IntrepidCore AND
   Trilinos_ENABLE_Pamgen AND
   Trilinos_ENABLE_Epetra)

  # The example has an option to use MueLu, if it is enabled.  _If_
  # MueLu is enabled, the example needs MueLu to support Epetra.  It
  # is possible to turn off MueLu's support for Epetra, so we have to
  # check here whether we are allowed to use MueLu with Epetra.
  IF(Trilinos_ENABLE_MueLu)
    # This variable only exists if MueLu is enabled.
    ASSERT_DEFINED(MueLu_ENABLE_Epetra)
    SET(TrilinosCouplings_ENABLE_EXAMPLE_IntrepidPoisson_Pamgen_Epetra ${MueLu_ENABLE_Epetra})
  ELSE()
    # The example can still build without MueLu.
    SET(TrilinosCouplings_ENABLE_EXAMPLE_IntrepidPoisson_Pamgen_Epetra ON)
  ENDIF()
   
  IF(TrilinosCouplings_ENABLE_EXAMPLE_IntrepidPoisson_Pamgen_Epetra)
    TRIBITS_ADD_EXECUTABLE(
      IntrepidPoisson_Pamgen_Epetra
      SOURCES IntrepidPoisson_Pamgen_Epetra_main.cpp
        TrilinosCouplings_IntrepidPoissonExampleHelpers.cpp
        TrilinosCouplings_EpetraIntrepidPoissonExample.cpp
        TrilinosCouplings_EpetraIntrepidPoissonExample_SolveWithBelos.cpp
        TrilinosCouplings_Pamgen_Utils.cpp
      COMM serial mpi
      )
  ENDIF()
ENDIF()

IF(Trilinos_ENABLE_AztecOO AND
   Trilinos_ENABLE_IntrepidCore AND
   Trilinos_ENABLE_Pamgen AND
   Trilinos_ENABLE_Epetra AND
   Trilinos_ENABLE_ML AND
   Trilinos_ENABLE_MueLu)

  # The example requires MueLu and needs MueLu to support Tpetra.  It
  # is possible to enable MueLu, yet turn off MueLu's support for
  # Epetra, so we have to check here whether we are allowed to use
  # MueLu with Epetra.  We can't check above because if MueLu is not
  # enabled, the MueLu_ENABLE_Epetra variable does not exist.

  IF(MueLu_ENABLE_Epetra)
    TRIBITS_ADD_EXECUTABLE(
      IntrepidPoisson_Pamgen_EpetraAztecOO
      SOURCES IntrepidPoisson_Pamgen_EpetraAztecOO_main.cpp
        TrilinosCouplings_IntrepidPoissonExampleHelpers.cpp
        TrilinosCouplings_EpetraIntrepidPoissonExample.cpp
        TrilinosCouplings_EpetraIntrepidPoissonExample_SolveWithAztecOO.cpp
        TrilinosCouplings_Pamgen_Utils.cpp
      COMM serial mpi
      )
  ENDIF() # MueLu_ENABLE_Epetra
ENDIF()

# Copy over example xml file for [E|T]petraIntrepidPoisson example
TRIBITS_COPY_FILES_TO_BINARY_DIR(copyIntrepidPoissonFiles
  DEST_FILES   intrepid_poisson.xml intrepid_poisson_muelu_cheby.xml intrepid_poisson_pamgen.xml
  )

# mfh 14 Feb 2013
#
# Just build this executable; don't run it as a test.
#
# Tpetra has a required dependency on Teuchos and Kokkos,
# so we don't have to name those packages explicitly here.
# This test uses the default Kokkos Node type.
IF(Trilinos_ENABLE_Epetra AND Trilinos_ENABLE_Tpetra)
  TRIBITS_ADD_EXECUTABLE(
    epetraTpetraImportBenchmark
    SOURCES epetraTpetraImportBenchmark.cpp
    COMM serial mpi
    )
ENDIF()
