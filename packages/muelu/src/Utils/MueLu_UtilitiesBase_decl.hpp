// @HEADER
//
// ***********************************************************************
//
//        MueLu: A package for multigrid based preconditioning
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
// Questions? Contact
//                    Jonathan Hu       (jhu@sandia.gov)
//                    Andrey Prokopenko (aprokop@sandia.gov)
//                    Ray Tuminaro      (rstumin@sandia.gov)
//
// ***********************************************************************
//
// @HEADER
#ifndef MUELU_UTILITIESBASE_DECL_HPP
#define MUELU_UTILITIESBASE_DECL_HPP

#include <unistd.h> //necessary for "sleep" function in debugging methods
#include <string>

#include "MueLu_ConfigDefs.hpp"

#include <Teuchos_DefaultComm.hpp>
#include <Teuchos_ScalarTraits.hpp>
#include <Teuchos_ParameterList.hpp>

#include <Xpetra_BlockedCrsMatrix_fwd.hpp>
#include <Xpetra_CrsMatrix_fwd.hpp>
#include <Xpetra_CrsMatrixWrap_fwd.hpp>
#include <Xpetra_Map_fwd.hpp>
#include <Xpetra_MapFactory_fwd.hpp>
#include <Xpetra_Matrix_fwd.hpp>
#include <Xpetra_MatrixFactory_fwd.hpp>
#include <Xpetra_MultiVector_fwd.hpp>
#include <Xpetra_MultiVectorFactory_fwd.hpp>
#include <Xpetra_Operator_fwd.hpp>
#include <Xpetra_Vector_fwd.hpp>
#include <Xpetra_VectorFactory_fwd.hpp>
#include <Xpetra_ExportFactory.hpp>

#include <Xpetra_Import.hpp>
#include <Xpetra_ImportFactory.hpp>
#include <Xpetra_MatrixMatrix.hpp>
#include <Xpetra_CrsMatrixWrap.hpp>


#include "MueLu_Exceptions.hpp"




namespace MueLu {

// MPI helpers
#define MueLu_sumAll(rcpComm, in, out)                                        \
    Teuchos::reduceAll(*rcpComm, Teuchos::REDUCE_SUM, in, Teuchos::outArg(out))
#define MueLu_minAll(rcpComm, in, out)                                        \
    Teuchos::reduceAll(*rcpComm, Teuchos::REDUCE_MIN, in, Teuchos::outArg(out))
#define MueLu_maxAll(rcpComm, in, out)                                        \
    Teuchos::reduceAll(*rcpComm, Teuchos::REDUCE_MAX, in, Teuchos::outArg(out))

  /*!
    @class Utilities
    @brief MueLu utility class.

    This class provides a number of static helper methods. Some are temporary and will eventually
    go away, while others should be moved to Xpetra.
  */
  template <class Scalar,
            class LocalOrdinal  = int,
            class GlobalOrdinal = LocalOrdinal,
            class Node          = KokkosClassic::DefaultNode::DefaultNodeType>
  class UtilitiesBase {
  public:
#undef MUELU_UTILITIESBASE_SHORT
//#include "MueLu_UseShortNames.hpp"
  private:
    typedef Xpetra::CrsMatrixWrap<Scalar,LocalOrdinal,GlobalOrdinal,Node> CrsMatrixWrap;
    typedef Xpetra::CrsMatrix<Scalar,LocalOrdinal,GlobalOrdinal,Node> CrsMatrix;
    typedef Xpetra::Matrix<Scalar,LocalOrdinal,GlobalOrdinal,Node> Matrix;
    typedef Xpetra::Vector<Scalar,LocalOrdinal,GlobalOrdinal,Node> Vector;
    typedef Xpetra::MultiVector<Scalar,LocalOrdinal,GlobalOrdinal,Node> MultiVector;
    typedef Xpetra::Map<LocalOrdinal,GlobalOrdinal,Node> Map;
  public:
    typedef typename Teuchos::ScalarTraits<Scalar>::magnitudeType Magnitude;


    static RCP<Matrix>                Crs2Op(RCP<CrsMatrix> Op) {
      if (Op.is_null())
        return Teuchos::null;
      return rcp(new CrsMatrixWrap(Op));
    }

    /*! @brief Extract Matrix Diagonal

    Returns Matrix diagonal in ArrayRCP.

    NOTE -- it's assumed that A has been fillComplete'd.
    */
    static Teuchos::ArrayRCP<Scalar> GetMatrixDiagonal(const Matrix& A) {
      size_t numRows = A.getRowMap()->getNodeNumElements();
      Teuchos::ArrayRCP<Scalar> diag(numRows);
      Teuchos::ArrayView<const LocalOrdinal> cols;
      Teuchos::ArrayView<const Scalar> vals;
      for (size_t i = 0; i < numRows; ++i) {
        A.getLocalRowView(i, cols, vals);
        LocalOrdinal j = 0;
        for (; j < cols.size(); ++j) {
          if (Teuchos::as<size_t>(cols[j]) == i) {
            diag[i] = vals[j];
            break;
          }
        }
        if (j == cols.size()) {
          // Diagonal entry is absent
          diag[i] = Teuchos::ScalarTraits<Scalar>::zero();
        }
      }
      return diag;
    }

    /*! @brief Extract Matrix Diagonal

    Returns inverse of the Matrix diagonal in ArrayRCP.

    NOTE -- it's assumed that A has been fillComplete'd.
    */
    static RCP<Vector> GetMatrixDiagonalInverse(const Matrix& A, Magnitude tol = Teuchos::ScalarTraits<Scalar>::eps()*100) {
      RCP<const Map> rowMap = A.getRowMap();
      RCP<Vector> diag = Xpetra::VectorFactory<Scalar,LocalOrdinal,GlobalOrdinal,Node>::Build(rowMap);
      ArrayRCP<Scalar> diagVals = diag->getDataNonConst(0);
      size_t numRows = rowMap->getNodeNumElements();
      Teuchos::ArrayView<const LocalOrdinal> cols;
      Teuchos::ArrayView<const Scalar> vals;
      for (size_t i = 0; i < numRows; ++i) {
        A.getLocalRowView(i, cols, vals);
        LocalOrdinal j = 0;
        for (; j < cols.size(); ++j) {
          if (Teuchos::as<size_t>(cols[j]) == i) {
            if(Teuchos::ScalarTraits<Scalar>::magnitude(vals[j]) > tol)
              diagVals[i] = Teuchos::ScalarTraits<Scalar>::one() / vals[j];
            else
              diagVals[i]=Teuchos::ScalarTraits<Scalar>::zero();
            break;
          }
        }
        if (j == cols.size()) {
          // Diagonal entry is absent
          diagVals[i]=Teuchos::ScalarTraits<Scalar>::zero();
        }
      }
      diagVals=null;
      return diag;
    }



    /*! @brief Extract Matrix Diagonal of lumped matrix

    Returns Matrix diagonal of lumped matrix in ArrayRCP.

    NOTE -- it's assumed that A has been fillComplete'd.
    */
    static Teuchos::ArrayRCP<Scalar> GetLumpedMatrixDiagonal(const Matrix& A) {
      size_t numRows = A.getRowMap()->getNodeNumElements();
      Teuchos::ArrayRCP<Scalar> diag(numRows);
      Teuchos::ArrayView<const LocalOrdinal> cols;
      Teuchos::ArrayView<const Scalar> vals;
      for (size_t i = 0; i < numRows; ++i) {
        A.getLocalRowView(i, cols, vals);
        diag[i] = Teuchos::ScalarTraits<Scalar>::zero();
        for (LocalOrdinal j = 0; j < cols.size(); ++j) {
          diag[i] += Teuchos::ScalarTraits<Scalar>::magnitude(vals[j]);
        }
      }
      return diag;
    }

    /*! @brief Extract Overlapped Matrix Diagonal

    Returns overlapped Matrix diagonal in ArrayRCP.

    The local overlapped diagonal has an entry for each index in A's column map.
    NOTE -- it's assumed that A has been fillComplete'd.
    */
    static RCP<Vector> GetMatrixOverlappedDiagonal(const Matrix& A) {
      RCP<const Map> rowMap = A.getRowMap(), colMap = A.getColMap();
      RCP<Vector> localDiag = Xpetra::VectorFactory<Scalar,LocalOrdinal,GlobalOrdinal,Node>::Build(rowMap);

      try {
         const CrsMatrixWrap* crsOp = dynamic_cast<const CrsMatrixWrap*>(&A);
         if (crsOp == NULL) {
           throw Exceptions::RuntimeError("cast to CrsMatrixWrap failed");
         }
         Teuchos::ArrayRCP<size_t> offsets;
         crsOp->getLocalDiagOffsets(offsets);
         crsOp->getLocalDiagCopy(*localDiag,offsets());
      }
      catch (...) {
        ArrayRCP<Scalar>   localDiagVals = localDiag->getDataNonConst(0);
        Teuchos::ArrayRCP<Scalar> diagVals = GetMatrixDiagonal(A);
        for (LocalOrdinal i = 0; i < localDiagVals.size(); i++)
          localDiagVals[i] = diagVals[i];
        localDiagVals = diagVals = null;
      }

      RCP<Vector> diagonal = Xpetra::VectorFactory<Scalar,LocalOrdinal,GlobalOrdinal,Node>::Build(colMap);
      RCP< const Xpetra::Import<LocalOrdinal,GlobalOrdinal,Node> > importer;
      importer = A.getCrsGraph()->getImporter();
      if (importer == Teuchos::null) {
        importer = Xpetra::ImportFactory<LocalOrdinal,GlobalOrdinal,Node>::Build(rowMap, colMap);
      }
      diagonal->doImport(*localDiag, *(importer), Xpetra::INSERT);
      return diagonal;
    }


    // TODO: should NOT return an Array. Definition must be changed to:
    // - ArrayRCP<> ResidualNorm(Matrix const &Op, MultiVector const &X, MultiVector const &RHS)
    // or
    // - void ResidualNorm(Matrix const &Op, MultiVector const &X, MultiVector const &RHS, Array &)
    static Teuchos::Array<Magnitude> ResidualNorm(const Xpetra::Operator<Scalar,LocalOrdinal,GlobalOrdinal,Node>& Op, const MultiVector& X, const MultiVector& RHS) {
      TEUCHOS_TEST_FOR_EXCEPTION(X.getNumVectors() != RHS.getNumVectors(), Exceptions::RuntimeError, "Number of solution vectors != number of right-hand sides")
       const size_t numVecs = X.getNumVectors();
       RCP<MultiVector> RES = Residual(Op, X, RHS);
       Teuchos::Array<Magnitude> norms(numVecs);
       RES->norm2(norms);
       return norms;
    }

    static RCP<MultiVector> Residual(const Xpetra::Operator<Scalar,LocalOrdinal,GlobalOrdinal,Node>& Op, const MultiVector& X, const MultiVector& RHS) {
      TEUCHOS_TEST_FOR_EXCEPTION(X.getNumVectors() != RHS.getNumVectors(), Exceptions::RuntimeError, "Number of solution vectors != number of right-hand sides")
        const size_t numVecs = X.getNumVectors();
        Scalar one = Teuchos::ScalarTraits<Scalar>::one(), negone = -one, zero = Teuchos::ScalarTraits<Scalar>::zero();
        RCP<MultiVector> RES = Xpetra::MultiVectorFactory<Scalar,LocalOrdinal,GlobalOrdinal,Node>::Build(Op.getRangeMap(), numVecs, false); // no need to initialize to zero
        Op.apply(X, *RES, Teuchos::NO_TRANS, one, zero);
        RES->update(one, RHS, negone);
        return RES;
    }

#ifndef _WIN32
#include <unistd.h>
    static void PauseForDebugger() {
      RCP<const Teuchos::Comm<int> > comm = Teuchos::DefaultComm<int>::getComm();
      int myPID = comm->getRank();
      int pid   = getpid();
      char hostname[80];
      for (int i = 0; i <comm->getSize(); i++) {
        if (i == myPID) {
          gethostname(hostname, sizeof(hostname));
          std::cout << "Host: " << hostname << "\tMPI rank: " << myPID << ",\tPID: " << pid << "\n\tattach " << pid << std::endl;
          sleep(1);
        }
      }
      if (myPID == 0) {
        std::cout << "** Enter a character to continue > " << std::endl;
        char go = ' ';
        int r = scanf("%c", &go);
        (void)r;
        assert(r > 0);
      }
      comm->barrier();
    }
#else
    static void PauseForDebugger() {
         throw(Exceptions::RuntimeError("MueLu Utils: PauseForDebugger not implemented on Windows."));
     }
#endif

    /*! @brief Simple transpose for Tpetra::CrsMatrix types

        Note:  This is very inefficient, as it inserts one entry at a time.
    */

    /*! @brief Power method.

    @param A matrix
    @param scaleByDiag if true, estimate the largest eigenvalue of \f$ D^; A \f$.
    @param niters maximum number of iterations
    @param tolerance stopping tolerance
    @verbose if true, print iteration information

    (Shamelessly grabbed from tpetra/examples.)
    */
    static Scalar PowerMethod(const Matrix& A, bool scaleByDiag = true,
                              LocalOrdinal niters = 10, Magnitude tolerance = 1e-2, bool verbose = false, unsigned int seed = 123) {
      TEUCHOS_TEST_FOR_EXCEPTION(!(A.getRangeMap()->isSameAs(*(A.getDomainMap()))), Exceptions::Incompatible,
          "Utils::PowerMethod: operator must have domain and range maps that are equivalent.");

      // Create three vectors, fill z with random numbers
      RCP<Vector> q = Xpetra::VectorFactory<Scalar,LocalOrdinal,GlobalOrdinal,Node>::Build(A.getDomainMap());
      RCP<Vector> r = Xpetra::VectorFactory<Scalar,LocalOrdinal,GlobalOrdinal,Node>::Build(A.getRangeMap());
      RCP<Vector> z = Xpetra::VectorFactory<Scalar,LocalOrdinal,GlobalOrdinal,Node>::Build(A.getRangeMap());

      z->setSeed(seed);  // seed random number generator
      z->randomize(true);// use Xpetra implementation: -> same results for Epetra and Tpetra

      Teuchos::Array<Magnitude> norms(1);

      typedef Teuchos::ScalarTraits<Scalar> STS;

      const Scalar zero = STS::zero(), one = STS::one();

      Scalar lambda = zero;
      Magnitude residual = STS::magnitude(zero);

      // power iteration
      RCP<Vector> diagInvVec;
      if (scaleByDiag) {
        RCP<Vector> diagVec = Xpetra::VectorFactory<Scalar,LocalOrdinal,GlobalOrdinal,Node>::Build(A.getRowMap());
        A.getLocalDiagCopy(*diagVec);
        diagInvVec = Xpetra::VectorFactory<Scalar,LocalOrdinal,GlobalOrdinal,Node>::Build(A.getRowMap());
        diagInvVec->reciprocal(*diagVec);
      }

      for (int iter = 0; iter < niters; ++iter) {
        z->norm2(norms);                                  // Compute 2-norm of z
        q->update(one/norms[0], *z, zero);                // Set q = z / normz
        A.apply(*q, *z);                                  // Compute z = A*q
        if (scaleByDiag)
          z->elementWiseMultiply(one, *diagInvVec, *z, zero);
        lambda = q->dot(*z);                              // Approximate maximum eigenvalue: lamba = dot(q,z)

        if (iter % 100 == 0 || iter + 1 == niters) {
          r->update(1.0, *z, -lambda, *q, zero);          // Compute A*q - lambda*q
          r->norm2(norms);
          residual = STS::magnitude(norms[0] / lambda);
          if (verbose) {
            std::cout << "Iter = " << iter
                      << "  Lambda = " << lambda
                      << "  Residual of A*q - lambda*q = " << residual
                      << std::endl;
          }
        }
        if (residual < tolerance)
          break;
      }
      return lambda;
    }



    static RCP<Teuchos::FancyOStream> MakeFancy(std::ostream& os) {
      RCP<Teuchos::FancyOStream> fancy = Teuchos::fancyOStream(Teuchos::rcpFromRef(os));
      return fancy;
    }

    /*! @brief Squared distance between two rows in a multivector

       Used for coordinate vectors.
    */
    static typename Teuchos::ScalarTraits<Scalar>::magnitudeType Distance2(const Xpetra::MultiVector<Scalar,LocalOrdinal,GlobalOrdinal,Node>& v, LocalOrdinal i0, LocalOrdinal i1) {
      size_t numVectors = v.getNumVectors();

      Scalar d = Teuchos::ScalarTraits<Scalar>::zero();
      for (size_t j = 0; j < numVectors; j++) {
        Teuchos::ArrayRCP<const Scalar> vv = v.getData(j);
        d += (vv[i0] - vv[i1])*(vv[i0] - vv[i1]);
      }
      return Teuchos::ScalarTraits<Scalar>::magnitude(d);
    }

    /*! @brief Detect Dirichlet rows

        @param[in] A matrix
        @param[in] tol If a row entry's magnitude is less than or equal to this tolerance, the entry is treated as zero.

        @return boolean array.  The ith entry is true iff row i is a Dirichlet row.
    */
    static Teuchos::ArrayRCP<const bool> DetectDirichletRows(const Xpetra::Matrix<Scalar,LocalOrdinal,GlobalOrdinal,Node>& A, const Magnitude& tol = Teuchos::ScalarTraits<Scalar>::zero()) {
      LocalOrdinal numRows = A.getNodeNumRows();
      typedef Teuchos::ScalarTraits<Scalar> STS;
      ArrayRCP<bool> boundaryNodes(numRows, true);
      for (LocalOrdinal row = 0; row < numRows; row++) {
        ArrayView<const LocalOrdinal> indices;
        ArrayView<const Scalar> vals;
        A.getLocalRowView(row, indices, vals);
        size_t nnz = A.getNumEntriesInLocalRow(row);
        if (nnz > 1)
          for (size_t col = 0; col < nnz; col++)
            if ( (indices[col] != row) && STS::magnitude(vals[col]) > tol) {
              boundaryNodes[row] = false;
              break;
            }
      }
      return boundaryNodes;
    }

    /*! @brief Frobenius inner product of two matrices

       Used in energy minimization algorithms
    */
    static Scalar Frobenius(const Xpetra::Matrix<Scalar,LocalOrdinal,GlobalOrdinal,Node>& A, const Xpetra::Matrix<Scalar,LocalOrdinal,GlobalOrdinal,Node>& B) {
      // We check only row maps. Column may be different. One would hope that they are the same, as we typically
      // calculate frobenius norm of the specified sparsity pattern with an updated matrix from the previous step,
      // but matrix addition, even when one is submatrix of the other, changes column map (though change may be as
      // simple as couple of elements swapped)
      TEUCHOS_TEST_FOR_EXCEPTION(!A.getRowMap()->isSameAs(*B.getRowMap()),   Exceptions::Incompatible, "MueLu::CGSolver::Frobenius: row maps are incompatible");
      TEUCHOS_TEST_FOR_EXCEPTION(!A.isFillComplete() || !B.isFillComplete(), Exceptions::RuntimeError, "Matrices must be fill completed");

      const Map& AColMap = *A.getColMap();
      const Map& BColMap = *B.getColMap();

      Teuchos::ArrayView<const LocalOrdinal> indA, indB;
      Teuchos::ArrayView<const Scalar>       valA, valB;
      size_t nnzA = 0, nnzB = 0;

      // We use a simple algorithm
      // for each row we fill valBAll array with the values in the corresponding row of B
      // as such, it serves as both sorted array and as storage, so we don't need to do a
      // tricky problem: "find a value in the row of B corresponding to the specific GID"
      // Once we do that, we translate LID of entries of row of A to LID of B, and multiply
      // corresponding entries.
      // The algorithm should be reasonably cheap, as it does not sort anything, provided
      // that getLocalElement and getGlobalElement functions are reasonably effective. It
      // *is* possible that the costs are hidden in those functions, but if maps are close
      // to linear maps, we should be fine
      Teuchos::Array<Scalar> valBAll(BColMap.getNodeNumElements());

      LocalOrdinal  invalid = Teuchos::OrdinalTraits<LocalOrdinal>::invalid();
      Scalar        zero    = Teuchos::ScalarTraits<Scalar>       ::zero(),    f = zero, gf;
      size_t numRows = A.getNodeNumRows();
      for (size_t i = 0; i < numRows; i++) {
        A.getLocalRowView(i, indA, valA);
        B.getLocalRowView(i, indB, valB);
        nnzA = indA.size();
        nnzB = indB.size();

        // Set up array values
        for (size_t j = 0; j < nnzB; j++)
          valBAll[indB[j]] = valB[j];

        for (size_t j = 0; j < nnzA; j++) {
          // The cost of the whole Frobenius dot product function depends on the
          // cost of the getLocalElement and getGlobalElement functions here.
          LocalOrdinal ind = BColMap.getLocalElement(AColMap.getGlobalElement(indA[j]));
          if (ind != invalid)
            f += valBAll[ind] * valA[j];
        }

        // Clean up array values
        for (size_t j = 0; j < nnzB; j++)
          valBAll[indB[j]] = zero;
      }

      MueLu_sumAll(AColMap.getComm(), f, gf);

      return gf;
    }

    /*! @brief Set seed for random number generator.

      Distribute the seeds evenly in [1,INT_MAX-1].  This guarantees nothing
      about where random number streams on difference processes will intersect.
      This does avoid overflow situations in parallel when multiplying by a PID.
      It also avoids the pathological case of having the *same* random number stream
      on each process.
    */

    static void SetRandomSeed(const Teuchos::Comm<int> &comm) {
      // Distribute the seeds evenly in [1,maxint-1].  This guarantees nothing
      // about where in random number stream we are, but avoids overflow situations
      // in parallel when multiplying by a PID.  It would be better to use
      // a good parallel random number generator.
      double one = 1.0;
      int maxint = INT_MAX; //= 2^31-1 = 2147483647 for 32-bit integers
      int mySeed = Teuchos::as<int>((maxint-1) * (one -(comm.getRank()+1)/(comm.getSize()+one)) );
      if (mySeed < 1 || mySeed == maxint) {
        std::ostringstream errStr;
        errStr << "Error detected with random seed = " << mySeed << ". It should be in the interval [1,2^31-2].";
        throw Exceptions::RuntimeError(errStr.str());
      }
      std::srand(mySeed);
      // For Tpetra, we could use Kokkos' random number generator here.
      Teuchos::ScalarTraits<Scalar>::seedrandom(mySeed);
      // Epetra
      //   MultiVector::Random() -> Epetra_Util::RandomDouble() -> Epetra_Utils::RandomInt()
      // Its own random number generator, based on Seed_. Seed_ is initialized in Epetra_Util constructor with std::rand()
      // So our setting std::srand() affects that too
    }


  }; // class Utils



  ///////////////////////////////////////////

} //namespace MueLu

#define MUELU_UTILITIESBASE_SHORT
#endif // MUELU_UTILITIESBASE_DECL_HPP
