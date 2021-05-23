      SUBROUTINE PRVECR(VEC,LENGTH)
C
C THIS ROUTINE PRINTS THE FIRST LENGTH ELEMENTS OF VECTOR VEC
C  AND THEIR INDICES IN I3,F13.10 FORMAT.  USEFUL FOR QUANTITIES
C  WHICH HAVE MAGNITUDES COMPARABLE TO INTEGRALS AND AMPLITUDES.
C
CEND
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      COMMON /FILES/ LUOUT,MOINTS
      DIMENSION VEC(LENGTH)
      WRITE(LUOUT,100)(I,VEC(I),I=1,LENGTH)
100   FORMAT(4(1X,:'[',I4,']',F13.10))
      RETURN
      END
