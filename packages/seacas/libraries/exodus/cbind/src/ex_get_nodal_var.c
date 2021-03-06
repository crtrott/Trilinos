/*
 * Copyright (c) 2005 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
 * retains certain rights in this software.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.  
 * 
 *     * Neither the name of Sandia Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <stddef.h>                     // for size_t
#include <stdio.h>                      // for sprintf
#include <sys/types.h>                  // for int64_t
#include "exodusII.h"                   // for exerrval, ex_err, etc
#include "exodusII_int.h"               // for EX_NOERR, EX_WARN, etc
#include "netcdf.h"                     // for nc_inq_varid, NC_NOERR, etc

/*!
The function ex_get_nodal_var() reads the values of a single nodal
variable for a single time step. Memory must be allocated for the
nodal variable values array before this function is invoked.

Because nodal variables are floating point values, the application
code must declare the array passed to be the appropriate type (\c
float or \c double) to match the compute word size passed in
ex_create() or ex_open().

\return In case of an error, ex_get_nodal_var() returns a negative
number; a warning will return a positive number. Possible causes of
errors include:
  -  data file not properly opened with call to ex_create() or ex_open()
  -  specified nodal variable does not exist.
  -  a warning value is returned if no nodal variables are stored in the file.

\param[in] exoid                exodus file ID returned from a previous call to ex_create() 
                                or ex_open().

\param[in] time_step            The time step, as described under ex_put_time(), at which the
                                nodal variable values are desired. This is essentially an index (in
				the time dimension) into the nodal variable values array stored in
				the database. The first time step is 1.

\param[in] nodal_var_index      The index of the desired nodal variable. The first variable 
                                has an index of 1.

\param[in] num_nodes            The number of nodal points.

\param[out]  nodal_var_vals     Returned array of \c num_nodes values of the \c nodal_var_index-th
                                nodal variable for the \c time_step-th time step.


For example, the following demonstrates how this function would be
used:

\code
int num_nodes, time_step, var_index;
float *var_values;

\comment{read the second nodal variable at the first time step}
time_step = 1;
var_index = 2;

var_values = (float *) calloc (num_nodes, sizeof(float));
error = ex_get_nodal_var(exoid, time_step, var_index, num_nodes,
                         var_values);
\endcode

*/

int ex_get_nodal_var (int   exoid,
                      int   time_step,
                      int   nodal_var_index,
                      int64_t   num_nodes, 
                      void *nodal_var_vals)
{
  int varid;
  int status;
  size_t start[3], count[3];
  char errmsg[MAX_ERR_LENGTH];

  exerrval = 0; /* clear error code */

  /* inquire previously defined variable */

  /* Need to see how this works in the parallel-aware exodus... */
  if (num_nodes == 0)
    return EX_NOERR;
  
  /* Verify that time_step is within bounds */
  {
    int num_time_steps = ex_inquire_int (exoid, EX_INQ_TIME);
    if (time_step <= 0 || time_step > num_time_steps) {
      sprintf(errmsg,
	      "ERROR: time_step is out-of-range. Value = %d, valid range is 1 to %d in file id %d",
	      time_step, num_time_steps, exoid);
      ex_err("ex_get_nodal_var",errmsg,EX_BADPARAM);
      return (EX_FATAL);
    }
  }

  if (ex_large_model(exoid) == 0) {
    /* read values of the nodal variable */
    if ((status = nc_inq_varid(exoid, VAR_NOD_VAR, &varid)) != NC_NOERR) {
      exerrval = status;
      sprintf(errmsg,
              "Warning: could not find nodal variables in file id %d",
              exoid);
      ex_err("ex_get_nodal_var",errmsg,exerrval);
      return (EX_WARN);
    }

    start[0] = --time_step;
    start[1] = --nodal_var_index;
    start[2] = 0;

    count[0] = 1;
    count[1] = 1;
    count[2] = num_nodes;

  } else {
    /* read values of the nodal variable  -- stored as separate variables... */
    /* Get the varid.... */
    if ((status = nc_inq_varid(exoid, VAR_NOD_VAR_NEW(nodal_var_index), &varid)) != NC_NOERR) {
      exerrval = status;
      sprintf(errmsg,
              "Warning: could not find nodal variable %d in file id %d",
              nodal_var_index, exoid);
      ex_err("ex_get_nodal_var",errmsg,exerrval);
      return (EX_WARN);
    }

    start[0] = --time_step;
    start[1] = 0;

    count[0] = 1;
    count[1] = num_nodes;
  }

  if (ex_comp_ws(exoid) == 4) {
    status = nc_get_vara_float(exoid, varid, start, count, nodal_var_vals);
  } else {
    status = nc_get_vara_double(exoid, varid, start, count, nodal_var_vals);
  }

  if (status != NC_NOERR) {
      exerrval = status;
      sprintf(errmsg,
              "Error: failed to get nodal variables in file id %d",
              exoid);
      ex_err("ex_get_nodal_var",errmsg,exerrval);
      return (EX_FATAL);
    }
  return (EX_NOERR);
}
