#ifndef MATRIX_LIB_HPP
#define MATRIX_LIB_HPP

#include <cstdio>
// #include <cmath>
#include <cstdint>
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

template<typename INT_TYPE>
class IMat {
public:
    matClass info;
    INT_TYPE *data;
    uint64_t ncol;
    uint64_t nrow; //data(irow,icol) data[irow*ncol+icol]
    IMat() : IMat(0, 0, nullptr, _null)
    {

    }

    IMat(const IMat& inMatrix)
    {
        ncol = inMatrix.ncol;
        nrow = inMatrix.nrow;
        if (ncol <= 0 || nrow <= 0 || (!(inMatrix.data))) {
            ncol = 0;
            nrow = 0;
            data = nullptr;
            info = _null;
            return;
        }
        data = new INT_TYPE [ncol * nrow];
        for (uint64_t i = 0; i < nrow; ++i) {
            for (uint64_t j = 0; j < ncol; ++j) {
                data[i * ncol + j] = inMatrix.data[i * ncol + j];
            }
        }
        info = inMatrix.info;
    }

    IMat(IMat&& inMatrix)
    {
        data = inMatrix.data;
        inMatrix.data = nullptr;
        ncol = inMatrix.ncol;
        nrow = inMatrix.nrow;
        info = inMatrix.info;
    }

    IMat(uint64_t inNRow, uint64_t inNCol, INT_TYPE init = 0, matClass inInfo = _general)
        : nrow(inNRow), ncol(inNCol)
    {
        if (inNRow == 0 || inNCol == 0) {
            data = nullptr;
            info = _null;
            return;
        }
        uint64_t ncounts = inNRow * inNCol;
        data = new INT_TYPE [ncounts];
        for (uint64_t i = 0; i < ncounts; ++i) {
            data[i] = init;
        }
        info = inInfo;
    }

    IMat(uint64_t inNRow, uint64_t inNCol, const INT_TYPE *inData, matClass inInfo = _general)
        : nrow(inNRow), ncol(inNCol)
    {
        if (inNRow == 0 || inNCol == 0 || inData == nullptr) {
            data = nullptr;
            info = _null;
            return;
        }
        uint64_t ncounts = inNRow * inNCol;
        data = new INT_TYPE [ncounts];
        for (uint64_t i = 0; i < ncounts; ++i) {
            data[i] = inData[i];
        }
        info = inInfo;
    }

    ~IMat()
    {
        if (data) {
            delete [] data;
        }
        info = _null;
        ncol = 0;
        nrow = 0;
        data = nullptr;
    }

    void print() const
    {
        for (uint64_t i = 0; i < nrow; ++i) {
            for (uint64_t j = 0; j < ncol; ++j) {
                printf("\t%d", data[i * ncol + j]);
            }
            printf("\n");
        }
    }

    void print(const char* output) const
    {
        printf("%s\n", output);
        print();
    }

    void print(std::fstream& output) const
    {
        for (uint64_t i = 0; i < nrow; ++i) {
            for (uint64_t j = 0;j < ncol; ++j) {
                output << '\t' << data[i * ncol + j];
            }
            output << '\n';
        }
    }

    bool setMatClass(matClass inInfo)
    {
        info = inInfo;
        return true;
    }

    bool setData(uint64_t inNRow, uint64_t inNCol, const INT_TYPE *inData, matClass inInfo = _general)
    {
        if (inNCol == 0 || inNRow == 0) {
            return false;
        }
        if (ncol != inNCol || nrow != inNRow) {
            if (data) {
                delete [] data;
            }
            data = new INT_TYPE [inNRow*inNCol];
        }
        for (uint64_t i = 0; i < inNRow; ++i) {
            for (uint64_t j = 0; j < inNCol; ++j) {
                data[i * inNCol + j] = inData[i * inNCol + j];
            }
        }
        ncol = inNCol;
        nrow = inNRow;
        info = inInfo;
        return true;
    }

    bool setData(uint64_t inNRow, uint64_t inNCol, INT_TYPE init = 0, matClass inInfo = _general)
    {
        if (inNCol == 0 || inNRow == 0) {
            return false;
        }
        if (ncol != inNCol || nrow != inNRow) {
            if (data) {
                delete [] data;
            }
            data = new INT_TYPE [inNRow * inNCol];
        }
        for (uint64_t i = 0; i < inNRow; ++i) {
            for (uint64_t j = 0; j < inNCol; ++j) {
                data[i * inNCol + j] = init;
            }
        }
        ncol = inNCol;
        nrow = inNRow;
        info = inInfo;
        return true;
    }

    uint64_t getNCol() const
    {
        return ncol;
    }

    uint64_t getNRow() const
    {
        return nrow;
    }

    matClass getMatClass() const
    {
        return info;
    }

    INT_TYPE &operator()(uint64_t irow, uint64_t icol)
    {
        return data[ncol * irow + icol];
    }

    INT_TYPE operator()(uint64_t irow, uint64_t icol) const
    {
        return data[ncol * irow + icol];
    }

    INT_TYPE &operator()(uint64_t index)
    {
        return data[index];
    }

    INT_TYPE operator()(uint64_t index) const
    {
        return data[index];
    }

    IMat operator()(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2) const
    {
        if (x1 > x2 || y1 > y2 || info == _null) {
            return IMat();
        }
        uint64_t oncol = y2 - y1 + 1;
        uint64_t onrow = x2 - x1 + 1;
        INT_TYPE *values = new INT_TYPE[oncol * onrow];
        for (uint64_t i = x1; i <= x2; ++i) {
            for (uint64_t j = y1; j <= y2; ++j) {
                values[(i - x1) * oncol + j - y1] = data[i * ncol + j];
            }
        }
        IMat out;
        out.data = values;
        out.ncol = oncol;
        out.nrow = onrow;
        out.info = _general;
        return out;
    }

    IMat& operator=(const IMat& inMatrix)
    {
        if (inMatrix.info == _null) {
            info = _null;
            if (data) {
                delete [] data;
            }
            data = nullptr;
            ncol = 0, nrow=0;
            return *this;
        }
        if (ncol != inMatrix.ncol || nrow != inMatrix.nrow) {
            if (data) {
                delete [] data;
            }
            ncol = inMatrix.ncol;
            nrow = inMatrix.nrow;
            data = new INT_TYPE [ncol * nrow];
        }
        for (uint64_t i = 0; i < nrow; ++i) {
            for (uint64_t j = 0; j < ncol; ++j) {
                data[i * ncol + j] = (inMatrix.data)[i * ncol + j];
            }
        }
        info = inMatrix.info;
        return *this;
    }

    IMat& operator=(IMat&& inMatrix)
    {
        if (data) {
            delete [] data;
        }
        data = inMatrix.data;
        inMatrix.data = nullptr;
        ncol = inMatrix.ncol;
        nrow = inMatrix.nrow;
        info = inMatrix.info;
        return *this;
    }

    bool resize(uint64_t nRow, uint64_t nCol)
    {
        if (ncol == nCol && nrow == nRow) return false;
        if (ncol * nrow == nCol * nRow) {
            ncol = nCol;
            nrow = nRow;
            return true;
        }
        if (data) delete [] data;
        if (nRow == 0 || nCol == 0) {
            data = nullptr;
            info = _null;
            return false;
        }
        data = new INT_TYPE [nRow * nCol];
        ncol = nCol;
        nrow = nRow;
        info = _general;
        return true;
    }

    bool t()
    {
        if (info == _null) {
            return false;
        }
        INT_TYPE *toBe = new INT_TYPE[ncol * nrow];
        uint64_t tmp;
        for (uint64_t i = 0; i < ncol; ++i) {
            for (uint64_t j = 0; j < nrow; ++j) {
                toBe[i * nrow + j] = data[j * ncol + i];
            }
        }
        if (data) {
            delete [] data;
        }
        data = toBe;
        tmp = ncol;
        ncol = nrow;
        nrow = tmp;
        if (info == _rowvec) {
            info = _colvec;
        } else if (info == _colvec) {
            info=_rowvec;
        }
        return true;
    }

    bool t(IMat& outMatrix)
    {
        if (info == _null) {
            return false;
        }
        if (outMatrix.ncol * outMatrix.nrow != ncol * nrow) {
            if (outMatrix.data) {
                delete [] outMatrix.data;
            }
            outMatrix.data = new INT_TYPE [ncol*nrow];
        }
        for (uint64_t i = 0; i < ncol; ++i) {
            for (uint64_t j = 0; j < nrow; ++j){
                outMatrix.data[i * nrow + j] = data[j * ncol + i];
            }
        }
        outMatrix.ncol = nrow;
        outMatrix.nrow = ncol;
        outMatrix.info = _general;
        if (info == _rowvec) {
            outMatrix.info = _colvec;
        } else if (info == _colvec) {
            outMatrix.info = _rowvec;
        }
        return true;
    }
};

typedef IMat<int16_t> SIMat;

class Mat {
public:
    matClass info;
    double* data;
    uint64_t ncol;
    uint64_t nrow; //data(irow,icol) data[irow*ncol+icol]
    Mat();
    Mat(const Mat& inMatrix);
    Mat(Mat&& inMatrix);
    Mat(uint64_t inNRow, uint64_t inNCol,
        double init, matClass inInfo = _general);
    Mat(uint64_t inNRow, uint64_t inNCol,
        const double* inData, matClass inInfo = _general);
    ~Mat();
    void print() const;
    void print(const char* output) const;
    void print(std::fstream& output) const;
    bool setMatClass(matClass inInfo);
    bool setData(uint64_t inNRow, uint64_t inNCol,
                 const double* inData, matClass inInfo=_general);
    bool setData(uint64_t inNRow, uint64_t inNCol,
                 double inValues, matClass inInfo=_general);
    uint64_t getNCol() const;
    uint64_t getNRow() const;
    matClass getMatClass() const;
    double& operator()(uint64_t irow, uint64_t icol);
    double operator()(uint64_t irow, uint64_t icol) const;
    double& operator()(uint64_t index);
    double operator()(uint64_t index) const;
    Mat operator()(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2) const;
    Mat& operator=(const Mat& inMatrix);
    Mat& operator=(Mat&& inMatrix);
    Mat& operator=(const SIMat& inMatrix);
    bool resize(uint64_t nRow, uint64_t nCol);
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
    bool setRows(uint64_t N, const uint64_t* inIndex);
    bool setCols(uint64_t N, const uint64_t* inIndex);
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
    // bool exchangeRows(uint64_t i1,uint64_t i2);
    // bool exchangeCols(uint64_t i1,uint64_t i2);
    bool sortRows(uint64_t N,  const uint64_t* inIndex);
    bool sortCols(uint64_t N,  const uint64_t* inIndex);
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
    bool copyToRows(uint64_t start, uint64_t stop, Mat& outMatrix);
    bool copyToCols(uint64_t start, uint64_t stop, Mat& outMatrix);
    bool appendRows(uint64_t nRows);
    bool appendCols(uint64_t nCols);
    bool anyNan();
    void clear();
    static bool MatSub(const Mat& X, const Mat& Y, Mat& outMat, bool isAdd = false);
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

void _reQuickSortEigen(uint64_t* index, Mat& EVal, uint64_t a, uint64_t b, bool isLess);
bool quickSortEigen(Mat& EVec, Mat& EVal, bool isLess = true); //Mat = EVector %*% EValues %*% t(EVector)

}

#endif
