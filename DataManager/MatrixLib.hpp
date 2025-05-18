#ifndef MATRIX_LIB_HPP
#define MATRIX_LIB_HPP

#include <cstdio>
// #include <cmath>

#include <iostream>
#include <fstream>

namespace MML
{

enum matClass{
    _null = 0,
    _general = 1,
    _sym = 2,
    _upper = 3,
    _lower = 4,
    _colvec = 5,
    _rowvec = 6,
    _diag = 7,
    _lu = 8
};

class IMat {
public:
    matClass info;
    short *data;
    size_t ncol;
    size_t nrow; //data(irow,icol) data[irow*ncol+icol]
    IMat();
    IMat(const IMat& inMatrix);
    IMat(IMat&& inMatrix);
    IMat(size_t inNRow, size_t inNCol, short init = 0, matClass inInfo = _general);
    IMat(size_t inNRow, size_t inNCol, const short *inData, matClass inInfo = _general);
    ~IMat();
    void print() const;
    void print(const char* output) const;
    void print(std::fstream& output) const;
    bool setMatClass(matClass inInfo);
    bool setData(size_t inNRow, size_t inNCol, const short *inData, matClass inInfo = _general);
    bool setData(size_t inNRow, size_t inNCol, short init = 0, matClass inInfo = _general);
    size_t getNCol() const;
    size_t getNRow() const;
    matClass getMatClass() const;
    short &operator()(size_t irow, size_t icol);
    short operator()(size_t irow, size_t icol) const;
    short &operator()(size_t index);
    short operator()(size_t index) const;
    IMat operator()(size_t x1, size_t y1, size_t x2, size_t y2) const;
    IMat& operator=(const IMat& inMatrix);
    IMat& operator=(IMat&& inMatrix);
    bool resize(size_t nRow, size_t nCol);
    bool t();
    bool t(IMat& outMatrix);
};

class Mat {
public:
    matClass info;
    double* data;
    size_t ncol;
    size_t nrow; //data(irow,icol) data[irow*ncol+icol]
    Mat();
    Mat(const Mat& inMatrix);
    Mat(Mat&& inMatrix);
    Mat(size_t inNRow, size_t inNCol,
        double init, matClass inInfo = _general);
    Mat(size_t inNRow, size_t inNCol,
        const double* inData, matClass inInfo = _general);
    ~Mat();
    void print() const;
    void print(const char* output) const;
    void print(std::fstream& output) const;
    bool setMatClass(matClass inInfo);
    bool setData(size_t inNRow, size_t inNCol,
                 const double* inData, matClass inInfo=_general);
    bool setData(size_t inNRow, size_t inNCol,
                 double inValues, matClass inInfo=_general);
    size_t getNCol() const;
    size_t getNRow() const;
    matClass getMatClass() const;
    double& operator()(size_t irow, size_t icol);
    double operator()(size_t irow, size_t icol) const;
    double& operator()(size_t index);
    double operator()(size_t index) const;
    Mat operator()(size_t x1, size_t y1, size_t x2, size_t y2) const;
    Mat& operator=(const Mat& inMatrix);
    Mat& operator=(Mat&& inMatrix);
    Mat& operator=(const IMat& inMatrix);
    bool resize(size_t nRow, size_t nCol);
    bool t();
    bool t(Mat& outMatrix);
    double sum() const;
    bool add(double values);
    bool mul(double values);
    bool toSym(char target);
    bool toUpper();
    bool toLower();
    bool toColVec();
    bool toRowVec();
    bool toDiag();
    bool toSym(char target, Mat& outMatrix);
    bool toUpper(Mat& outMatrix);
    bool toLower(Mat& outMatrix);
    bool toColVec(Mat& outMatrix);
    bool toRowVec(Mat& outMatrix);
    bool toDiag(Mat& outMatrix);
    bool qr(Mat& Q, Mat& R) const; // = Qm*m %*% Rm*n
    bool qr2(Mat& Q, Mat& R) const; // = Qm*min(m,n) %*% Rmin(m,n)*min(m,n)
    bool symInv(Mat& outMatrix) const;
    bool symEig(Mat& EVector, Mat& EValues) const; //Mat = EVector %*% EValues %*% t(EVector)
    bool setRows(size_t N, const size_t* inIndex);
    bool setCols(size_t N, const size_t* inIndex);
    /* Mat.setCols(N,Index)
     * Mat = [
     *        0,1,2,3,4
     *        0,1,2,3,4
     *        0,1,2,3,4
     *        ]
     * Index = [0,2,2,4,1]
     * out Mat = [
     *        0,2,2,4,1
     *        0,2,2,4,1
     *        0,2,2,4,1
     *        ]
     *
    */
    // bool exchangeRows(size_t i1,size_t i2);
    // bool exchangeCols(size_t i1,size_t i2);
    bool sortRows(size_t N,  const size_t* inIndex);
    bool sortCols(size_t N,  const size_t* inIndex);
    /* Mat.sortCols(N,Index)
     * Mat = [
     *        0,1,2,3,4
     *        0,1,2,3,4
     *        0,1,2,3,4
     *        ]
     * Index = [0,2,3,4,1]
     * out Mat = [
     *        0,4,1,2,3
     *        0,4,1,2,3
     *        0,4,1,2,3
     *        ]
     *
    */    
    bool copyToRows(size_t start, size_t stop, Mat& outMatrix);
    bool copyToCols(size_t start, size_t stop, Mat& outMatrix);
    bool appendRows(size_t nRows);
    bool appendCols(size_t nCols);
    bool anyNan();
    void clear();
    static bool XYmul(const Mat& X, const Mat& Y, Mat& outMatrix, bool isAdd = false);
    static bool XtYmul(const Mat& X, const Mat& Y, Mat& outMatrix, bool isAdd = false);
    static bool XtXmul(const Mat& X, Mat& outMatrix, bool isAdd = false);
    static bool XXtmul(const Mat& X, Mat& outMatrix, bool isAdd = false);
    static bool XtDiXmul(const Mat& X, const Mat& D, Mat& outMatrix, bool isAdd = false);
    static bool XtDiVmul(const Mat& X, const Mat& D, const Mat& V, Mat& outVec, bool isAdd = false);
    static bool VtUmul(const Mat& V, const Mat& U, double& outVal, bool isAdd = false);
    static bool XVmul(const Mat& X, const Mat& V, Mat& outVec, bool isAdd = false);
    static bool VtXmul(const Mat& V, const Mat& X, Mat& outVec, bool isAdd = false);
    static bool VtDiUmul(const Mat& V, const Mat& D, const Mat& U, double& outVal, bool isAdd = false);
    static bool VtXUmul(const Mat& V, const Mat& X, const Mat& U, double& outVal, bool isAdd = false);
};

void _reQuickSortEigen(size_t* index, Mat& EVal, size_t a, size_t b, bool isLess);
bool quickSortEigen(Mat& EVec, Mat& EVal, bool isLess = true); //Mat = EVector %*% EValues %*% t(EVector)

}

#endif
