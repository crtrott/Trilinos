#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "shylubasker_decl.hpp"
#include "shylubasker_def.hpp"
 
#ifdef BASKER_KOKKOS
#include <Kokkos_Core.hpp>
#else
#include <omp.h>
#endif

using namespace std;

int main(int argc, char* argv[])
{
  // test
  /* 
     ./driver.exe matrixfilename.mtx nthreads 
  */

  //typedef long long Int;
  typedef long Int;
  //typedef int Int;
  typedef double Entry;
  #ifdef BASKER_KOKKOS
  typedef Kokkos::OpenMP Exe_Space;
  #else
  typedef void*          Exe_Space;
  #endif
    
  std::string fname = std::string(argv[1]);
  //std::string rhsname = std::string(argv[2]);
  //Int numthreads = atoi(argv[3]);

  //std::string fname = "matrix1.mtx";
 
  Int numthreads = atoi(argv[2]);

  cout << "using " << numthreads << "threads" << endl;


  #ifdef BASKER_KOKKOS
  Exe_Space::initialize(numthreads);
  cout << "---------------USING KOKKOS-------------" << endl;
  #else
  //omp_set_num_threads(numthreads);
  cout << "-------------- USING OMP---------------" << endl;
  #endif

  {

  #ifdef BASKER_KOKKOS
  cout << "true: " << true << endl;
  cout << "hwloc aval: " << Kokkos::hwloc::available()<<endl;
  cout << "numa count: " << Kokkos::hwloc::get_available_numa_count() << endl;
  cout << "thrd numa:  " << Kokkos::hwloc::get_available_cores_per_numa() << endl;
  #endif


  //Read in MTX
  //Note: Adapted from Siva's original bsk_util
  Int m,n, nnz, innz;
  Int *col_ptr;
  Int *row_idx;
  Entry *vals;

  n = m = 0;
  nnz = 0;
  innz = 0;

  cout << "ReadingMTX " << fname << endl;
  ifstream inp_str;
  inp_str.open(fname, ios::in);
  Int i, j;
  Int nrows, ncols, true_nnz;
  Entry val;
  std::string s;
  size_t p1, p2, p3;
  Int ptype, sym_type;
  
  if (inp_str.is_open())
    {
      getline(inp_str, s);
      //cout << s << endl;
      
      // Check if matrix is pattern-only or symmetric
      p1 = s.find("pattern");
      if (p1 != string::npos)
	ptype = 2;
      else
	ptype = 3;
      p1 = s.find("symmetric");
      p2 = s.find("hermitian");
      p3 = s.find("skew-symmetric");
      if ((p1 != string::npos) || (p2 != string::npos) || 
	  (p3 != string::npos))
	sym_type = 1;
      else
	sym_type = 0;
      
      while (inp_str.peek() == '%') // Skip the comments.
	getline(inp_str, s);
      
      // Find the # of rows, cols and nnzs.
      inp_str >> nrows;
      inp_str >> ncols;
      inp_str >> nnz;
      
      cout << nrows << " " << ncols  << " " << nnz << endl;
      n = ncols;
      //M.mcol = ncols;
      m = nrows;
      //M.nrow = nrows;
      //M.nnz = nnz;
      
      col_ptr = new Int[ncols+1]();
      //MALLOC_INT_1DARRAY(M.col_ptr, ncols+1);
      //init_value(M.col_ptr, ncols+1,(Int) 0);
      row_idx = new Int[nnz]();
      //MALLOC_INT_1DARRAY(M.row_idx, nnz);
      //init_value(M.row_idx, nnz, (Int) 0);
      vals = new Entry[nnz]();
      //MALLOC_ENTRY_1DARRAY(M.val, nnz);
      //init_value(M.val, nnz, (Entry) 0.0);
      //Int innz = 0;
      //cout << "MTX Malloc Done " << endl;
      
      while(nnz > 0)
	{
	  inp_str >> i;
	  //cout << "row: " << i-1 ;
	  row_idx[innz] = i-1;
	  //M.row_idx[innz] = i-1;
	  inp_str >> j;
	  //cout << " col: " << j-1;
	  col_ptr[j] = col_ptr[j]+1;
	  //M.col_ptr[j] = M.col_ptr[j]+1;
	  inp_str >> val;
	  //cout << " val: " << val << endl;
	  vals[innz] = val;
	  //M.val[innz] = val;
	  
	  //Other type options..
	  innz++;
	  nnz--;
	}
      inp_str.close();
      
      //cout << "MTX done reading" << endl;
      
      //count col_sums
      
      for(Int k =1 ; k<(ncols+1); k++)
	{
	  col_ptr[k] = col_ptr[k] + col_ptr[k-1];
	  //M.col_ptr[k] = M.col_ptr[k] +M.col_ptr[k-1];
	}
      //cout << "MTX done sorting " << endl;
      
      //Sort index in column...
      
    }//end if open

  cout << "NNZ " << nnz
       << " "    << innz
       << " "    << col_ptr[ncols]
       << endl;
  nnz = innz;


  //====Load righthand side
  Entry* y = new Entry[n]();
  Entry* x = new Entry[n]();
  Int ii = 0;
  string t;
  ifstream fprhs;
  /*
  fprhs.open(rhsname.c_str());
  while(fprhs >> t)
    {
      y[ii] = (Entry) atof(t.c_str());
      ii++;
    }
  fprhs.close();
  */

  //Before Init
  //{
  int result = 0;
  BaskerNS::Basker<Int, Entry, Exe_Space> mybasker;
  //----Basker - Options
  mybasker.Options.no_pivot   = true;
  mybasker.Options.symmetric  = true;
  mybasker.Options.realloc    = true;
  mybasker.Options.btf        = false;
  mybasker.Options.matching   = false;
  mybasker.Options.amd_dom    = false;
  mybasker.Options.incomplete = true;
  mybasker.Options.incomplete_type = 
    BASKER_INCOMPLETE_LVL;
  mybasker.Options.inc_lvl    = 3;
  mybasker.Options.user_fill  = 1.0;

  mybasker.SetThreads(numthreads);
  cout << "--------------Done Setting Threads----------" << endl;
  mybasker.Symbolic(m,n,nnz,col_ptr,row_idx,vals);
  cout << "--------------Done SFactor------------------" << endl;
  mybasker.Factor_Inc(0);
  cout << "--------------Done NFactor-----------------" << endl;
  mybasker.DEBUG_PRINT();
  cout << "--------------Done Print----------------------"<<endl;
  // mybasker.SolveTest();
  //mybasker.Solve(y,x);
  //cout << "--------------Done Solve----------------------"<<endl;

  /*
  Int *lperm;
  Int *rperm;
  mybasker.GetPerm(&lperm, &rperm);
  Int found_lnnz;
  mybasker.GetLnnz(found_lnnz);
  Int found_unnz;
  mybasker.GetUnnz(found_unnz);
  Int found_ln;
  Int *row_idx_l;
  Int *col_ptr_l;
  Entry *val_l;
  mybasker.GetL(found_ln, found_lnnz, 
		(&row_idx_l), (&col_ptr_l), (&val_l));

  Int found_un;
  Int *row_idx_u;
  Int *col_ptr_u;
  Entry *val_u;
  mybasker.GetU(found_un, found_unnz,
		(&row_idx_u), (&col_ptr_u), (&val_u));

  */
  mybasker.Finalize();
  cout << "--------------Called Finalize-----------------"<<endl;
 
  //}//After
  //Kokkos::fence();

  }
  //#ifdef BASKER_KOKKOS
  Kokkos::finalize();
  //#endif

}
