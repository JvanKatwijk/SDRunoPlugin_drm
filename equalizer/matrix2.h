//
//	source:
//	http://bbs.dartmouth.edu/~fangq/MATH/download/source/inverse_c.htm
// inverse.c
// Source Code for "GRPP, A Scientific Programming Language 
// Processor Designed for Lex and Yacc."
// Author: James Kent Blackburn
// Goddard Space Flight Center, Code 664.0, Greenbelt, MD. 20771
// Computers in Physics, Journal Section, Jan/Feb 1994

/* 
   Matrix Inversion using 
   LU Decomposition from
   Numerical Recipes in C
   Chapter 2
*/
#ifndef	__MATRIX_INVERSION
#define	__MATRIX_INVERSION

#include	"..\basics.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define   TINY 1.0e-20

void	inverse	(DRM_FLOAT**,int);
void	ludcmp	(DRM_FLOAT**, int, int*, DRM_FLOAT*);
void	lubksb	(DRM_FLOAT**, int, int*, DRM_FLOAT*);
DRM_FLOAT **matrix	(int,int,int,int);
DRM_FLOAT *vector	(int,int);
void	free_matrix(DRM_FLOAT**,int,int,int,int);
void	free_vector(DRM_FLOAT*,int,int);
#endif

