#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <string>
#include <map>
#include <GetOpt.h>
#include <cstdlib>
#include <stdio.h>

#include "Matrix.h"
#include "ArgumentException.h"

#include <Rcpp.h>
using namespace std;

/********************************************************************
 * .Call() Entry points  sc2pv
 * *****************************************************************/
RcppExport SEXP sc2pv (SEXP mat, SEXP Rscore, SEXP bg, SEXP type){
  
  Rcpp::NumericVector background(bg);
  Rcpp::NumericMatrix matrix(mat);
  Rcpp::NumericVector RScoreVec(Rscore);
  Rcpp::CharacterVector Type(type);
  // Fill with background
  Matrix m(background[0], background[1], background[2], background[3]);
  // Fill with matrix
  int i=0, j=0;
  m.mat = new double*[4];
  int ncol = matrix.ncol();
  int nrow = matrix.nrow();
  m.length = ncol;
  for(i=0; i<nrow; i++){
    m.mat[i] = new double[ncol];
    for(j=0; j<ncol; j++){
      m.mat[i][j] = matrix[j*nrow+i];
    }
  }
  /*cout << "INITIAL MATRIX" << endl;
  for(j=0; j<ncol; j++){
    for(i=0; i<nrow; i++){
      cout << m.mat[i][j] << "\t";
    }
    cout << endl;
  }*/
  //cout << "Matrix length  : " << m.length << endl;
  
  // toPWM when it is PFM
  if(!strcmp(Type[0], "PFM")){
    m.toLogOddRatio();
  }
  
  // testScoreToPvalue
  double initialGranularity = 0.1;
  bool forcedGranularity = false;
  double maxGranularity = 1e-9;
  double requestedScore = RScoreVec[0];
  long long totalSize = 0;
  long long totalOp = 0;
  long long max;
  long long min;
  double ppv;
  double pv;
  long long score;
  for (double granularity = initialGranularity; granularity >= maxGranularity; granularity /= 10){
    //cout << "Computing rounded matrix with granularity " << granularity << endl;
    m.computesIntegerMatrix(granularity);
    max = requestedScore*m.granularity + m.offset + m.errorMax+1;
    min = requestedScore*m.granularity + m.offset - m.errorMax-1;
    score = requestedScore*m.granularity + m.offset;
    /*cout << "Score range : " << m.scoreRange << endl;
    cout << "Min         : " << min << endl;
    cout << "Max         : " << max << endl;
    cout << "Precision   : " << m.granularity << endl;
    cout << "Error max   : " << m.errorMax << endl;
    cout << "Computing pvalue for requested score " << requestedScore << " " << score << endl;*/
#ifdef MEMORYCOUNT
    m.totalMapSize = 0;
    m.totalOp = 0;
#endif
    m.lookForPvalue(score,min,max,&ppv,&pv);
 /*   cout << "Prev. Pvalue  : " << ppv << endl;
    cout << "Pvaluex       : " << pv << endl;
    cout << "Comp. score   : " << score << endl;*/
#ifdef MEMORYCOUNT
    totalSize += m.totalMapSize;
    totalOp += m.totalOp;
#endif
    //cout << "***********************************************" << endl;
    if (ppv == pv) {
      if (!forcedGranularity) {
        break;
      }
    }
  }
  Rcpp::NumericVector ans(1);
  ans[0] = pv;
  // free the memory allocated, not typical Rcpp way
  for(i=0; i<nrow; i++){
    delete[] m.mat[i];
  }
  delete[] m.mat;

  return Rcpp::wrap(ans);
}

