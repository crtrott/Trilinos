

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(UNIT_TEST_DRIVER
  ${TEUCHOS_STD_UNIT_TEST_MAIN})

TRIBITS_ADD_EXECUTABLE_AND_TEST(
  periodic_bcs 
  SOURCES periodic_bcs.cpp ${UNIT_TEST_DRIVER}
  COMM serial mpi
  NUM_MPI_PROCS 4
  )
  
TRIBITS_ADD_EXECUTABLE_AND_TEST(
  periodic_mesh
  SOURCES periodic_mesh.cpp ${UNIT_TEST_DRIVER}
  COMM serial mpi
  NUM_MPI_PROCS 2
  )

# TRIBITS_ADD_EXECUTABLE_AND_TEST(
#   periodic_mesh_nosubcells
#   SOURCES periodic_mesh_nosubcells.cpp ${UNIT_TEST_DRIVER}
#   COMM serial mpi
#   NUM_MPI_PROCS 1
#   )
