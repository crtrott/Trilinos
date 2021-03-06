/*
 * Copyright (c) 2014, Sandia Corporation.
 * Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
 * the U.S. Government retains certain rights in this software.
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

/* Normalizes a double n-vector over range. */
double    ch_normalize(double *vec, int beg, int end)
{
    int       i;
    double    scale;
    double    ch_norm(double *vec, int beg, int end);

    scale = ch_norm(vec, beg, end);
    vec = vec + beg;
    for (i = end - beg + 1; i; i--) {
	*vec = *vec / scale;
	vec++;
    }
    return (scale);
}

/* Normalizes such that element k is positive */
double    sign_normalize(double *vec, int beg, int end, int k)
{
    int       i;
    double    scale, scale2;
    double    ch_norm(double *vec, int beg, int end);

    scale = ch_norm(vec, beg, end);
    if (vec[k] < 0) {
	scale2 = -scale;
    }
    else {
	scale2 = scale;
    }
    vec = vec + beg;
    for (i = end - beg + 1; i; i--) {
	*vec = *vec / scale2;
	vec++;
    }
    return (scale);
}

/* Normalizes a float n-vector over range. */
double    normalize_float(float *vec, int beg, int end)
{
    int       i;
    float     scale;
    double    norm_float(float *vec, int beg, int end);

    scale = norm_float(vec, beg, end);
    vec = vec + beg;
    for (i = end - beg + 1; i; i--) {
	*vec = *vec / scale;
	vec++;
    }
    return ((double) scale);
}
