// @HEADER
//
// ***********************************************************************
//
//   Zoltan2: A package of combinatorial algorithms for scientific computing
//                  Copyright 2012 Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Karen Devine      (kddevin@sandia.gov)
//                    Erik Boman        (egboman@sandia.gov)
//                    Siva Rajamanickam (srajama@sandia.gov)
//
// ***********************************************************************
//
// @HEADER
//
// Testing Zoltan2::MachineRepresentation

#include <Zoltan2_MachineRepresentation.hpp>
#include <Zoltan2_TestHelpers.hpp>
#include <Teuchos_DefaultComm.hpp>

template <typename nCoord_t, typename part_t>
int checkAllCoords(const Teuchos::Comm<int> &comm,
                   const Zoltan2::MachineRepresentation<nCoord_t,part_t> &mach)
{
  // Verify that getAllMachineCoordinatesView returns the same values
  // as individual calls by rank.
  // Verify that returned coordinates lie within the extent of the coordinates.

  int fail = 0;
  const int failval = 100000000;

  int np = comm.getSize();
  int me = comm.getRank();

  int dim = mach.getMachineDim();
  int *nxyz = new int[dim];

  nCoord_t *xyz = new nCoord_t[dim];

  nCoord_t *all = new nCoord_t[dim*np];
  nCoord_t **allCoords = new nCoord_t *[dim];
  allCoords[0] = all;
  for (int i = 1; i < dim; i++) allCoords[i] = allCoords[i-1]+np;

  bool haveExtent = mach.getMachineExtent(nxyz);

  if (mach.getAllMachineCoordinatesView(allCoords)) {

    // Check all ranks
    for (int i = 0; i < np; i++) {
      if (mach.getMachineCoordinate(i, xyz)) {
        for (int d = 0; d < dim; d++) {
          if (xyz[d] != allCoords[d][i]) fail = failval;
          if (haveExtent && (xyz[d] < 0 || xyz[d] >= nxyz[d])) fail = failval;
        }
      }
      else
        fail = failval;  // getMachineCoordinate failed
    }

    // Check my rank
    if (mach.getMyMachineCoordinate(xyz)) {
      for (int d = 0; d < dim; d++) 
        if (xyz[d] != allCoords[d][me]) fail = failval;
    }
    else
      fail = failval;  // getMyMachineCoordinate failed
  }
  else 
    fail = failval;  // couldn't retrieve coordinates

  delete [] xyz;
  delete [] nxyz;
  delete [] all;
  delete [] allCoords;

  return fail;
}

/////////////////////////////////////////////////////////////////////////////
int checkErrorCode(const Teuchos::Comm<int> &comm, int code)
{
  int rank = comm.getRank();
  if (code > 0) 
    std::cerr << "Proc " << rank << " error: " << code << std::endl;
  comm.barrier();

  // Will return 1 if any process has a non-zero code
  TEST_FAIL_AND_RETURN_VALUE(comm, code==0, "TEST FAILED", 1);

  return 0;
}

/////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  Teuchos::GlobalMPISession session(&argc, &argv);
  Teuchos::RCP<const Comm<int> > comm = Teuchos::DefaultComm<int>::getComm();

  int me = comm->getRank();
  int np = comm->getSize();
  char *name = "node0";
  int fail = 0;

  typedef zlno_t ncoord_t;
  typedef zlno_t part_t;

  Zoltan2::MachineRepresentation<ncoord_t, part_t> mach(*comm);

  // Tests that all machines should pass
  if (mach.getNumRanks() != np) fail += 1;

  if (mach.hasMachineCoordinates())
    fail += checkAllCoords<ncoord_t,part_t>(*comm, mach);

  if (checkErrorCode(*comm, fail))
    return 1;

#if defined(HAVE_ZOLTAN2_LDMS)
  { // Add tests specific to LDMS
    if (checkErrorCode(*comm, fail))
      return 1;
  }
#elif defined(HAVE_ZOLTAN2_RCA)
  { // Add tests specific to RCA
    if (checkErrorCode(*comm, fail))
      return 1;
  }
#elif defined(HAVE_ZOLTAN2_TOPOMGR)
  { // Add tests specific to TopoMgr
    if (checkErrorCode(*comm, fail))
      return 1;
  }
#else

  { // Tests specific to MachineForTesting

    if (mach.getMachineDim() != 3) fail += 10;

    int nxyz[3];
    if (!mach.getMachineExtent(nxyz)) fail += 100;
    if (nxyz[0] != 10 || nxyz[1] != 20 || nxyz[2] != 30)
      fail += 1000;

    ncoord_t xyz[3];
    ncoord_t xyz_expected[3] = {10, 20, 30};  // TODO FIX

    if (!mach.getMyMachineCoordinate(xyz)) fail += 1000;
  
    if ((xyz[0] != xyz_expected[0]) || 
        (xyz[1] != xyz_expected[1]) ||
        (xyz[2] != xyz_expected[2]))
      fail += 10000;

    // MachineForTesting cannot retrieve coords by node name
    if (mach.getMachineCoordinate(name, xyz)) fail += 10000000;

    if (checkErrorCode(*comm, fail))
      return 1;
  }
#endif

  if (me == 0) std::cout << "PASS" << std::endl;

  return 0;
}
