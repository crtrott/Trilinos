C    Copyright (c) 2014, Sandia Corporation.
C    Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
C    the U.S. Government retains certain rights in this software.
C    
C    Redistribution and use in source and binary forms, with or without
C    modification, are permitted provided that the following conditions are
C    met:
C    
C        * Redistributions of source code must retain the above copyright
C          notice, this list of conditions and the following disclaimer.
C    
C        * Redistributions in binary form must reproduce the above
C          copyright notice, this list of conditions and the following
C          disclaimer in the documentation and/or other materials provided
C          with the distribution.
C    
C        * Neither the name of Sandia Corporation nor the names of its
C          contributors may be used to endorse or promote products derived
C          from this software without specific prior written permission.
C    
C    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
C    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
C    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
C    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
C    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
C    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
C    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
C    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
C    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
C    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
C    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
C    

C $Id: lupang.f,v 1.3 2004/01/26 17:28:18 gdsjaar Exp $
C $Log: lupang.f,v $
C Revision 1.3  2004/01/26 17:28:18  gdsjaar
C Removed several unused variables from getang subroutine.
C
C Initialized a variable
C
C Revision 1.2  2004/01/22 14:25:22  gdsjaar
C Attempt to fix strange problem on x86_64 AMD Opteron system using
C Portland Group 5.1-3 compilers. The getang function would work
C correctly if compiled with no optimization and in debug mode, but
C would crash if compiled optimized. The location of the crash was not
C in a place that made any sense that something was wrong.
C
C After much trial and error, it was found that adding a 'SAVE'
C statement at the beginning of the file fixed the problem.
C
C Also cleaned out some unused parameters being passed to the function.
C
C Revision 1.1.1.1  1990/11/30 11:11:47  gdsjaar
C FASTQ Version 2.0X
C
c Revision 1.1  90/11/30  11:11:45  gdsjaar
c Initial revision
c 
C
CC* FILE: [.PAVING]LUPANG.FOR
CC* MODIFIED BY: TED BLACKER
CC* MODIFICATION DATE: 7/6/90
CC* MODIFICATION: COMPLETED HEADER INFORMATION
C
      SUBROUTINE LUPANG (MXND, MLN, XN, YN, ZN, LXK, KXL, NXL, LXN,
     &   NLOOP, ANGLE, LNODES, NSTART, LLL, XMIN, XMAX, YMIN, YMAX,
     &   ZMIN, ZMAX, DEV1, KREG, ERR)
C***********************************************************************
C
C  SUROUTINE LUPANG = CALCULATES THE NEW ANGLES FOR ALL NODES IN A LOOP
C
C***********************************************************************
C
      DIMENSION XN (MXND), YN (MXND), ZN(MXND)
      DIMENSION LXN(4, MXND), NXL(2, 3*MXND)
      DIMENSION LXK(4, MXND), KXL(2, 3*MXND)
      DIMENSION ANGLE (MXND), LNODES (MLN, MXND)
C
      LOGICAL ERR
C
      CHARACTER*3 DEV1
C
      ERR = .FALSE.
C
C  LOOP AROUND THE INTERIOR PERIMETER CALCULATING THE NEW
C  ANGLES
C
      N1 = NSTART
      KOUNT = 0
  100 CONTINUE
      N0 = LNODES (2, N1)
      N2 = LNODES (3, N1)
      CALL GETANG (MXND, MLN, XN, YN, LNODES, LXK, KXL, NXL, LXN,
     &   N0, N1, N2, ANGLE (N1), ERR)
      IF (ERR) THEN
         CALL MESAGE (' ** ERROR IN LUPANG ** ')
         GOTO 110
      ENDIF
      N1 = N2
      IF (N1 .EQ. NSTART) GOTO 110
      KOUNT = KOUNT+1
      IF (KOUNT .GT. NLOOP) THEN
         CALL MESAGE (' ** ERROR IN LUPANG ** ')
         ERR = .TRUE.
         GOTO 110
      ENDIF
      GOTO 100
C
  110 CONTINUE
      RETURN
C
      END
