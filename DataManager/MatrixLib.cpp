#include "MatrixLib.hpp"

#include "openblas/lapacke.h"
#include "openblas/cblas.h"

MML::Mat::Mat():Mat(0, 0, nullptr, _null)
{

}

MML::Mat::Mat(const Mat& inMatrix)
{
    ncol = inMatrix.ncol;
    nrow = inMatrix.nrow;
    if (ncol <= 0 || nrow <= 0|| (!(inMatrix.data))) {
        ncol = 0;
        nrow = 0;
        data = nullptr;
        info = _null;
        return;
    }
    data = new double [ncol * nrow];
    for (uint64_t i = 0; i < nrow; ++i) {
        for (uint64_t j = 0; j < ncol; ++j) {
            data[i * ncol + j] = inMatrix.data[i * ncol + j];
        }
    }
    info = inMatrix.info;
}

MML::Mat::Mat(Mat &&inMatrix)
{
    data = inMatrix.data;
    inMatrix.data = nullptr;
    info = inMatrix.info;
    ncol = inMatrix.ncol;
    nrow = inMatrix.nrow;
}

MML::Mat::Mat(uint64_t nRow, uint64_t nCol, double init, matClass inInfo)
    : nrow(nRow), ncol(nCol)
{
    if (nRow == 0 || nCol == 0) {
        data = nullptr;
        info = _null;
        return;
    }
    uint64_t ncounts = nRow * nCol;
    data = new double [ncounts];
    for (uint64_t i = 0; i < ncounts; ++i) {
        data[i] = init;
    }
    info = inInfo;
}

MML::Mat::Mat(uint64_t nRow, uint64_t nCol, const double* inData, matClass inInfo)
    : nrow(nRow), ncol(nCol)
{
    if (nRow == 0 || nCol == 0 || !inData) {
        data = nullptr;
        info = _null;
        return;
    }
    uint64_t ncounts = nRow * nCol;
    data = new double [ncounts];
    for (uint64_t i = 0; i < ncounts; ++i) {
        data[i] = inData[i];
    }
    info = inInfo;
}

MML::Mat::~Mat()
{
    if(data) {
        delete [] data;
    }
    info = _null;
    ncol = 0;
    nrow = 0;
    data = nullptr;
}

uint64_t MML::Mat::getNCol() const
{
    return ncol;
}

uint64_t MML::Mat::getNRow() const
{
    return nrow;
}

MML::matClass MML::Mat::getMatClass() const
{
    return info;
}

void MML::Mat::print() const
{
    for (uint64_t i = 0; i < nrow; ++i) {
        for (uint64_t j = 0; j < ncol; ++j) {
            printf("\t%f", data[i * ncol + j]);
        }
        printf("\n");
    }
}

void MML::Mat::print(const char* output) const
{
    printf("%s\n", output);
    print();
}

void MML::Mat::print(std::fstream &output) const
{
    for (uint64_t i = 0; i < nrow; ++i) {
        for (uint64_t j = 0; j < ncol; ++j) {
            output << '\t' << data[i * ncol + j];
        }
        output << '\n';
    }
}

bool MML::Mat::setMatClass(matClass inInfo)
{
    info = inInfo;
    return true;
}

bool MML::Mat::setData(uint64_t inNRow, uint64_t inNCol, const double* inData, matClass inInfo)
{
    if (inNCol == 0 || inNRow == 0) {
        return false;
    }
    if (ncol != inNCol || nrow != inNRow) {
        if (data) delete [] data;
        data = new double [inNRow * inNCol];
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

bool MML::Mat::setData(uint64_t inNRow, uint64_t inNCol, double inValues, matClass inInfo)
{
    if (inNCol == 0 || inNRow == 0) {
        return false;
    }
    if (ncol != inNCol || nrow != inNRow) {
        if (data) {
            delete [] data;
        }
        data = new double [inNRow * inNCol];
    }
    for (uint64_t i = 0; i < inNRow; ++i) {
        for (uint64_t j = 0; j < inNCol; ++j) {
            data[i * inNCol + j] = inValues;
        }
    }
    ncol = inNCol;
    nrow = inNRow;
    info = inInfo;
    return true;
}

double& MML::Mat::operator()(uint64_t irow, uint64_t icol)
{
    return data[ncol * irow + icol];
}

double MML::Mat::operator()(uint64_t irow, uint64_t icol) const
{
    return data[ncol * irow + icol];
}

double& MML::Mat::operator()(uint64_t index)
{
    return data[index];
}

double MML::Mat::operator()(uint64_t index) const
{
    return data[index];
}

MML::Mat MML::Mat::operator()(uint64_t irow1, uint64_t icol1, uint64_t irow2, uint64_t icol2) const
{
    if (irow1 > irow2 || icol1 > icol2 || info == _null) {
        return Mat();
    }
    uint64_t oncol = icol2 - icol1 + 1;
    uint64_t onrow = irow2 - irow1 + 1;
    double *values = new double[oncol * onrow];
    for (uint64_t i = irow1; i <= irow2; ++i){
        for (uint64_t j = icol1; j <= icol2; ++j){
            values[(i - irow1) * oncol + j - icol1] = data[i * ncol + j];
        }
    }
    Mat out;
    out.data = values;
    out.ncol = oncol;
    out.nrow = onrow;
    out.info = _general;
    return out;
}

MML::Mat& MML::Mat::operator=(const Mat& inMatrix)
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
        data = new double [ncol * nrow];
    }
    for (uint64_t i = 0; i < nrow; ++i) {
        for (uint64_t j = 0; j < ncol; ++j) {
            data[i * ncol + j] = (inMatrix.data)[i * ncol + j];
        }
    }
    info = inMatrix.info;
    return *this;
}

MML::Mat& MML::Mat::operator=(Mat&& inMatrix)
{
    if (data) {
        delete [] data;
    }
    data = inMatrix.data;
    inMatrix.data = nullptr;
    info = inMatrix.info;
    ncol = inMatrix.ncol;
    nrow = inMatrix.nrow;
    return *this;
}

MML::Mat& MML::Mat::operator=(const SIMat& inMatrix)
{
    if (inMatrix.getMatClass() == _null) {
        info = _null;
        if (data) {
            delete [] data;
        }
        data = nullptr;
        ncol = 0, nrow=0;
        return *this;
    }
    if (data) {
        delete [] data;
    }
    ncol = inMatrix.getNCol();
    nrow = inMatrix.getNRow();
    data = new double [ncol * nrow];
    for (uint64_t i = 0; i < nrow; ++i) {
        for (uint64_t j = 0; j < ncol; ++j) {
            data[i * ncol + j] = (double)inMatrix(i, j);
        }
    }
    info = inMatrix.info;
    return *this;
}

bool MML::Mat::resize(uint64_t nRow,uint64_t nCol)
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
    data = new double [nRow * nCol];
    ncol = nCol;
    nrow = nRow;
    info = _general;
    return true;
}

bool MML::Mat::t()
{
    if (info == _null) {
        return false;
    }
    double* toBe = new double[ncol * nrow];
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

bool MML::Mat::t(Mat& outMatrix)
{
    if (info == _null) {
        return false;
    }
    if (outMatrix.ncol * outMatrix.nrow != ncol * nrow) {
        if (outMatrix.data) {
            delete [] outMatrix.data;
        }
        outMatrix.data = new double [ncol*nrow];
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

double MML::Mat::sum() const
{
    if (info == _null) return 0;
    uint64_t N = ncol * nrow;
    double out = 0;
    for (uint64_t i = 0; i < N; ++i) {
        out += data[i];
    }
    return out;
}

bool MML::Mat::add(double values)
{
    if (info == _null) return false;
    uint64_t N = ncol * nrow;
    for (uint64_t i = 0; i < N; ++i) {
        data[i] += values;
    }
    return true;
}

bool MML::Mat::mul(double values)
{
    if (info == _null) return false;
    uint64_t N = ncol * nrow;
    for (uint64_t i = 0; i < N; ++i) {
        data[i] *= values;
    }
    return true;
}

bool MML::Mat::toSym(char target)
{
    if (info == _null || (target != 'L' && target != 'U')) return false;
    uint64_t lim = ncol > nrow ? nrow : ncol;
    double* toBe = nullptr;
    if (ncol != nrow) {
        toBe = new double[lim * lim];
        if (target == 'L') {
            for (uint64_t i = 0; i < lim; ++i) {
                for (uint64_t j = i; j < lim; ++j) {
                    toBe[i * lim + j] = data[j * lim + i];
                    toBe[j * lim + i] = data[j * lim + i];
                }
            }
        } else {
            for (uint64_t i = 0; i < lim; ++i) {
                for (uint64_t j = i; j < lim; ++j) {
                    toBe[i * lim + j] = data[i * lim + j];
                    toBe[j * lim + i] = data[i * lim + j];
                }
            }
        }
        if (data) delete [] data;
        data = toBe;
    } else {
        if (target == 'L') {
            for (uint64_t i = 0; i < lim; ++i) {
                for (uint64_t j = i + 1; j < lim; ++j) {
                    data[i * lim + j] = data[j * lim + i];
                }
            }
        } else {
            for (uint64_t i = 0; i < lim; ++i) {
                for (uint64_t j = i + 1; j < lim; ++j) {
                    data[j * lim + i] = data[i * lim + j];
                }
            }
        }
    }
    info = _sym;
    ncol = lim;
    nrow = lim;
    return true;
}

bool MML::Mat::toUpper()
{
    if (info == _null) return false;
    for (uint64_t i = 0; i < nrow; ++i) {
        for (uint64_t j = 0; j < i; ++j) {
            data[i * ncol + j] = 0;
        }
    }
    info = _upper;
    return true;
}

bool MML::Mat::toLower()
{
    if (info == _null) return false;
    for (uint64_t i = 0; i < nrow && i < ncol; ++i) {
        for (uint64_t j = i + 1; j < ncol; ++j) {
            data[i * ncol + j] = 0;
        }
    }
    info = _lower;
    return true;
}

bool MML::Mat::toColVec()
{
    if (info == _null) return false;
    nrow *= ncol;
    ncol = 1;
    info = _colvec;
    return true;
}

bool MML::Mat::toRowVec()
{
    if (info == _null) return false;
    ncol *= nrow;
    nrow = 1;
    info = _rowvec;
    return true;
}

bool MML::Mat::toDiag()
{
    if (info == _null) return false;
    uint64_t lim = ncol > nrow ? nrow : ncol;
    double* toBe = nullptr;
    if (ncol != nrow) {
        toBe = new double[lim * lim]();
        for (uint64_t i = 0; i < lim; ++i) {
            toBe[i * lim + i] = data[i * lim + i];
        }
        if (data) delete [] data;
        data = toBe;
    } else {
        for (uint64_t i = 0; i < lim; ++i) {
            for (uint64_t j = 0; j < lim; ++j) {
                if (i != j) data[j * lim + i] = 0;
            }
        }
    }
    info = _diag;
    ncol = lim;
    nrow = lim;
    return true;
}

bool MML::Mat::toSym(char target, Mat& outMatrix)
{
    if (info == _null || (target != 'L' && target != 'U')) return false;
    uint64_t lim = ncol > nrow ? nrow : ncol;
    double* toBe = nullptr;
    toBe = new double[lim * lim];
    if (target == 'L') {
        for (uint64_t i = 0; i < lim; ++i) {
            for (uint64_t j = i; j < lim; ++j) {
                toBe[i * lim + j] = data[j * lim + i];
                toBe[j * lim + i] = data[j * lim + i];
            }
        }
    } else {
        for (uint64_t i = 0; i < lim; ++i) {
            for (uint64_t j = i; j < lim; ++j) {
                toBe[i * lim + j] = data[i * lim + j];
                toBe[j * lim + i] = data[i * lim + j];
            }
        }
    }
    if (outMatrix.data) delete [] outMatrix.data;
    outMatrix.data = toBe;
    outMatrix.info = _sym;
    outMatrix.ncol = lim;
    outMatrix.nrow = lim;
    return true;
}

bool MML::Mat::toUpper(Mat& outMatrix)
{
    if (info == _null) return false;
    double* toBe = nullptr;
    toBe = new double[ncol * nrow]();
    for (uint64_t i = 0; i < nrow; ++i) {
        for (uint64_t j = 0; j < ncol; ++j) {
            if (i > j) toBe[i * ncol + j] = 0;
            else toBe[i * ncol + j] = data[i * ncol + j];
        }
    }
    if (outMatrix.data) delete [] outMatrix.data;
    outMatrix.data = toBe;
    outMatrix.info = _upper;
    outMatrix.ncol = ncol;
    outMatrix.nrow = nrow;
    return true;
}

bool MML::Mat::toLower(Mat& outMatrix)
{
    if (info == _null) return false;
    double* toBe = nullptr;
    toBe = new double[ncol * nrow]();
    for (uint64_t i = 0; i < nrow; ++i) {
        for (uint64_t j = i; j < ncol; ++j) {
            if (j > i) toBe[i * ncol + j] = 0;
            else toBe[i * ncol + j] = data[i * ncol + j];
        }
    }
    if (outMatrix.data) delete [] outMatrix.data;
    outMatrix.data = toBe;
    outMatrix.info = _lower;
    outMatrix.ncol = ncol;
    outMatrix.nrow = nrow;
    return true;
}

bool MML::Mat::toColVec(Mat& outMatrix)
{
    if (info == _null) return false;
    outMatrix.nrow = nrow * ncol;
    outMatrix.ncol = 1;
    outMatrix.info = _colvec;
    if (outMatrix.data) delete [] outMatrix.data;
    outMatrix.data = new double[outMatrix.nrow];
    for (uint64_t i = 0; i < outMatrix.nrow; ++i) {
        (outMatrix.data)[i] = data[i];
    }
    return true;
}

bool MML::Mat::toRowVec(Mat& outMatrix)
{
    if (info == _null) return false;
    outMatrix.ncol = nrow * ncol;
    outMatrix.nrow = 1;
    outMatrix.info = _rowvec;
    if (outMatrix.data) delete [] outMatrix.data;
    outMatrix.data = new double[outMatrix.ncol];
    for (uint64_t i = 0; i < outMatrix.ncol; ++i) {
        (outMatrix.data)[i] = data[i];
    }
    return true;
}

bool MML::Mat::toDiag(Mat& outMatrix)
{
    if (info == _null) return false;
    uint64_t lim = ncol > nrow ? nrow : ncol;
    double* toBe = nullptr;
    toBe = new double[lim * lim]();
    for (uint64_t i = 0; i < lim; ++i) {
        toBe[i * lim + i] = data[i * lim + i];
    }
    if (outMatrix.data) delete [] outMatrix.data;
    outMatrix.data = toBe;
    outMatrix.info = _diag;
    outMatrix.ncol = lim;
    outMatrix.nrow = lim;
    return true;
}

bool MML::Mat::qr(Mat &Q, Mat &R) const
{
    if (info == _null) return false;
    int m = nrow, n = ncol, lda = ncol;
    Mat A(*this);
    Q.setData(m, m, 0.0);
    double *tau = new double[ncol];
    for (int i = 0; i < m; ++i) {
        Q(i, i) = 1.0;
    }
    LAPACKE_dgeqrf(LAPACK_ROW_MAJOR, m, n, A.data, lda, tau);
    A.toUpper(R);
    LAPACKE_dormqr(LAPACK_ROW_MAJOR, 'L', 'N', m, m, n, A.data, lda, tau, Q.data, m);
    delete [] tau;
    return true;
}

bool MML::Mat::qr2(Mat &Q, Mat &R) const
{
    if (info == _null) return false;
    int m = nrow, n = ncol, lda = ncol;
    int lim = std::min(m, n);
    Mat A(*this);
    double *tau = new double[ncol];
    LAPACKE_dgeqrf(LAPACK_ROW_MAJOR, m, n, A.data, lda, tau);
    R.setData(lim, lim, 0.0);
    for (int i = 0; i < lim; ++i) {
        for (int j = i; j < lim; ++j) {
            R(i, j) = A(i, j);
        }
    }
    LAPACKE_dorgqr(LAPACK_ROW_MAJOR, m, lim, lim, A.data, lda, tau);
    Q = A(0, 0, m - 1, lim - 1);
    delete [] tau;
    return true;
}

bool MML::Mat::symInv(Mat& outMatrix) const
{
    if (info != _sym) return false;
    bool re = true;
    outMatrix = *this;
    char uplo = 'L';
    double forWork = .0;
    int forLwork = 1, outInfo = 0, N = ncol;
    int* ipivot = new int [ncol];
    double *forWork2 = nullptr;
    LAPACK_dsytrf(&uplo, &N, outMatrix.data, &N, ipivot, &forWork, &forLwork, &outInfo);
    if (outInfo == 0) {
        forWork2 = new double[N];
        LAPACK_dsytri(&uplo, &N, outMatrix.data, &N, ipivot, forWork2, &outInfo);
        if (outInfo != 0) re = false;
        delete [] forWork2;
    }
    else re = false;
    delete [] ipivot;
    outMatrix.toSym('U');
    return re;
}

bool MML::Mat::symEig(Mat& EVector, Mat& EValues) const
{
    if (info != _sym) return false;
    bool re = true;
    EVector = *this;
    char uplo = 'L', jobz = 'V';
    int LDA = nrow, N = ncol, forLwork = 3 * ncol - 1, outInfo;
    EValues.resize(nrow, 1);
    double* forWork = new double[3 * ncol - 1];
    LAPACK_dsyev(&jobz, &uplo, &N, EVector.data, &LDA, EValues.data, forWork, &forLwork, &outInfo);
    delete [] forWork;
    if (outInfo == 0) {
        EVector.t();
        EVector.info = _general;
        EValues.toColVec();
    } else {
        re = false;
    }
    return re;
}

bool MML::Mat::setRows(uint64_t N, const uint64_t* inIndex)
{
    if (N == 0 || !inIndex || info == _null) return false;
    double* toBe = new double [N * ncol];
    for (uint64_t i = 0; i < N; ++i) {
        for (uint64_t j = 0; j < ncol; ++j) {
            toBe[i * ncol + j] = data[inIndex[i] * ncol + j];
        }
    }
    if(data) delete [] data;
    data = toBe;
    nrow = N;
    return true;
}

bool MML::Mat::setCols(uint64_t N, const uint64_t* inIndex)
{
    if (N == 0 || !inIndex || info == _null) return false;
    double* toBe = new double [N * nrow];
    for (uint64_t i = 0; i < nrow; ++i) {
        for (uint64_t j = 0; j < N; ++j) {
            toBe[i * N + j] = data[i * ncol + inIndex[j]];
        }
    }
    if (data) delete [] data;
    data = toBe;
    ncol = N;
    return true;
}

bool MML::Mat::sortRows(uint64_t N, const uint64_t* inIndex)
{
    if (N != nrow) return false;
    bool isDiff = false;
    double** pTmp = new double*[N];
    uint64_t M = ncol;
    for (uint64_t i = 0; i < N; ++i) {
        if (inIndex[i] != i) {
            isDiff = true;
            pTmp[i] = new double[M];
            for (uint64_t j = 0; j < M; ++j) {
                pTmp[i][j] = data[i * M + j];
            }
        } else {
            pTmp[i] = nullptr;
        }
    }
    if (!isDiff) {
        delete [] pTmp;
        return false;
    }
    for (uint64_t i = 0; i < N; ++i) {
        if (inIndex[i] != i) {
            for (uint64_t j = 0; j < M; ++j) {
                data[i * M + j] = pTmp[inIndex[i]][j];
            }
        }
    }
    for (uint64_t i = 0; i < N; ++i) {
        if (pTmp[i]) delete [] pTmp[i];
    }
    delete [] pTmp;
    return true;
}

bool MML::Mat::sortCols(uint64_t N, const uint64_t* inIndex)
{
    if (N != ncol) return false;
    bool isDiff = false;
    double** pTmp = new double*[N];
    uint64_t M = nrow;
    for (uint64_t i = 0; i < N; ++i) {
        if (inIndex[i] != i) {
            isDiff = true;
            pTmp[i] = new double[M];
            for (uint64_t j = 0; j < M; ++j) {
                pTmp[i][j] = data[j * N + i];
            }
        } else {
            pTmp[i] = nullptr;
        }
    }
    if (!isDiff) {
        delete [] pTmp;
        return false;
    }
    for (uint64_t i = 0; i < N; ++i) {
        if (inIndex[i] != i) {
            for (uint64_t j = 0; j < M; ++j) {
                data[j * N + i] = pTmp[inIndex[i]][j];
            }
        }
    }
    for (uint64_t i = 0; i < N; ++i) {
        if (pTmp[i]) delete [] pTmp[i];
    }
    delete [] pTmp;
    return true;
}

bool MML::Mat::copyToRows(uint64_t istart, uint64_t istop, Mat& outMatrix)
{
    if (info == _null || istop >= nrow || istop < istart) return false;
    if (outMatrix.info != _null) {
        if (outMatrix.data) delete [] outMatrix.data;
    }
    uint64_t ilen = istop - istart + 1;
    outMatrix.data = new double [ncol * ilen];
    outMatrix.info = _general;
    outMatrix.nrow = ilen;
    outMatrix.ncol = ncol;
    uint64_t i = 0, j = 0;
    for (i = 0; i < outMatrix.nrow; ++i) {
        for (j = 0; j < ncol; ++j) {
            outMatrix.data[i * ncol + j] = data[(i + istart) * ncol + j];
        }
    }
    return true;
}

bool MML::Mat::copyToCols(uint64_t istart, uint64_t istop, Mat& outMatrix)
{
    if (info == _null || istop >= ncol || istop < istart) return false;
    if (outMatrix.info != _null) {
        if (outMatrix.data) delete [] outMatrix.data;
    }
    uint64_t ilen = istop - istart + 1;
    outMatrix.data = new double [nrow * ilen];
    outMatrix.info = _general;
    outMatrix.ncol = ilen;
    outMatrix.nrow = nrow;
    uint64_t i = 0, j = 0;
    for (i = 0; i < nrow; ++i) {
        for (j = 0; j < ilen; ++j) {
            outMatrix.data[i * ilen + j] = data[i * ncol + j + istart];
        }
    }
    return true;
}

bool MML::Mat::appendRows(uint64_t nRows)
{
    if (info == _null || nRows <= 0) return false;
    double *toBe = new double [(nRows + nrow) * ncol];
    for (uint64_t i = 0; i < nrow; ++i) {
        for(uint64_t j = 0; j < ncol; ++j) {
            toBe[i * ncol + j] = data[i * ncol + j];
        }
    }
    delete [] data;
    data = toBe;
    nrow += nRows;
    return true;
}

bool MML::Mat::appendCols(uint64_t nCols)
{
    if (info == _null || nCols <= 0) return false;
    double *toBe = new double [nrow * (ncol + nCols)];
    for (uint64_t i = 0; i < nrow; ++i) {
        for (uint64_t j = 0; j < ncol; ++j) {
            toBe[i * (ncol + nCols) + j] = data[i * ncol + j];
        }
    }
    delete [] data;
    data = toBe;
    ncol += nCols;
    return true;
}

bool MML::Mat::anyNan()
{
    if (info == _null || ncol == 0 || nrow == 0) return false;
    for (uint64_t i = 0; i < nrow; ++i) {
        for (uint64_t j = 0; j < ncol; ++j) {
            if (std::isnan(data[i * ncol + j])) return true;
        }
    }
    return false;
}

void MML::Mat::clear()
{
    if (data) delete [] data;
    data = nullptr;
    ncol = 0;
    nrow = 0;
}

bool MML::Mat::MatSub(const Mat& X, const Mat& Y, Mat& outMat, bool isAdd)
{
    if (X.info == _null || Y.info == _null) {
        return false;
    }
    if (X.ncol != Y.ncol || X.nrow != Y.nrow) {
        return false;
    }
    if (X.ncol == 0 || X.nrow == 0) {
        return false;
    }
    int m = X.ncol;
    int n = X.nrow;
    int i, j;
    outMat.resize(m, n);
    for (i = 0; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            outMat.data[i * n + j] = X.data[i * n + j] - Y.data[i * n + j];
        }
    }
    return true;
}

bool MML::Mat::XYmul(const Mat& X, const Mat& Y, Mat& outMatrix, bool isAdd)
{
    if (X.ncol != Y.nrow || X.info == _null || Y.info == _null) return false;
    int M = X.nrow, N = Y.ncol, K = X.ncol;
    double alpha = 1.0, beta = 0.0;
    if (isAdd) {
        if (outMatrix.info == _null || outMatrix.nrow != M || outMatrix.ncol != N) {
            return false;
        }
        beta = 1.0;
    } else {
        outMatrix.resize(M, N);
        outMatrix.info = _general;
    }
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                M, N, K, alpha, X.data, K, Y.data, N, beta, outMatrix.data, N);
    return true;
}

bool MML::Mat::XtYmul(const Mat &X, const Mat &Y, Mat &outMatrix,bool isAdd)
{
    if (X.nrow!=Y.nrow || X.info==_null || Y.info==_null) return false;
    int M = X.ncol, N = Y.ncol, K = X.nrow;
    double alpha = 1.0, beta = 0.0;
    if (isAdd) {
        if (outMatrix.info == _null || outMatrix.nrow != M || outMatrix.ncol != N) {
            return false;
        }
        beta = 1.0;
    } else {
        outMatrix.resize(M, N);
        outMatrix.info = _general;
    }
    cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans,
                M, N, K, alpha, X.data, M, Y.data, N, beta, outMatrix.data, N);
    return true;
}

bool MML::Mat::XtXmul(const Mat& X, Mat& outMatrix, bool isAdd)
{
    if (X.info == _null) return false;
    int M = X.ncol, N = X.ncol, K = X.nrow;
    double alpha = 1.0, beta = 0.0;
    if (isAdd) {
        if (outMatrix.info == _null || outMatrix.nrow != M || outMatrix.ncol != N) {
            return false;
        }
        beta = 1.0;
    } else {
        outMatrix.resize(M, N);
        outMatrix.info = _sym;
    }
    cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans,
                M, N, K, alpha, X.data, M, X.data, N, beta, outMatrix.data, M);
    return true;
}

bool MML::Mat::XXtmul(const Mat &X, Mat &outMatrix, bool isAdd)
{
    if (X.info == _null) return false;
    int M = X.nrow, N = X.nrow, K = X.ncol;
    double alpha = 1.0, beta = 0.0;
    if (isAdd) {
        if (outMatrix.info == _null || outMatrix.nrow != M || outMatrix.ncol != N) {
            return false;
        }
        beta = 1.0;
    } else {
        outMatrix.resize(M, N);
        outMatrix.info = _sym;
    }
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans,
                M, N, K, alpha, X.data, K, X.data, K, beta, outMatrix.data, M);
    return true;
}

bool MML::Mat::XtDiXmul(const Mat& X, const Mat& D, Mat& outMatrix, bool isAdd)
{
    if (X.info == _null || (D.info != _colvec && D.info != _rowvec) || X.nrow != D.ncol * D.nrow) {
        return false;
    }
    double *XtDi = new double[X.ncol * X.nrow];
    for (uint64_t i = 0; i < X.nrow; ++i) {
        for (uint64_t j = 0; j < X.ncol; ++j) {
            XtDi[i + j * X.nrow] = X.data[i * X.ncol + j] / D.data[i];
        }
    }
    int M = X.ncol, N = X.ncol, K = X.nrow;
    double alpha = 1.0, beta = 0.0;
    if (isAdd) {
        if (outMatrix.info == _null || outMatrix.nrow != M || outMatrix.ncol != N) {
            return false;
        }
        beta = 1.0;
    } else {
        outMatrix.resize(M, N);
        outMatrix.info = _sym;
    }
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                M, N, K, alpha, XtDi, K, X.data, N, beta, outMatrix.data, N);
    delete [] XtDi;
    return true;
}

bool MML::Mat::XtDiVmul(const Mat& X, const Mat& D, const Mat& V, Mat& outVec, bool isAdd)
{
    if (X.info == _null || (V.info != _colvec && V.info != _rowvec) ||
        (D.info != _colvec && D.info != _rowvec) || X.nrow != D.ncol * D.nrow || V.nrow != X.nrow) {
        return false;
    }
    int M = X.ncol, N = X.nrow;
    double alpha = 1.0, beta = 0.0;
    double *DiV = new double[N];
    for (uint64_t i = 0; i < N; ++i) {
        DiV[i] = V.data[i] / D.data[i];
    }
    if (isAdd) {
        if (outVec.info == _null || outVec.nrow != M || outVec.ncol != 1) {
            return false;
        }
        beta = 1.0;
    } else {
        outVec.resize(M, 1);
        outVec.info = _colvec;
    }
    cblas_dgemv(CblasRowMajor, CblasTrans,
                N, M, alpha, X.data, M, DiV, 1, beta, outVec.data, 1);
    delete [] DiV;
    return true;
}

bool MML::Mat::VtUmul(const Mat& V, const Mat& U, double& outVal, bool isAdd)
{
    if ((V.info != _colvec && V.info != _rowvec) || (U.info != _colvec && U.info != _rowvec) ||
        V.ncol * V.nrow != U.ncol * U.nrow) {
        return false;
    }
    if (!isAdd) outVal = 0.0;
    for (uint64_t i = 0; i < V.ncol * V.nrow; ++i) {
        outVal += V.data[i] * U.data[i];
    }
    return true;
}

bool MML::Mat::XVmul(const Mat& X, const Mat& V, Mat& outVec, bool isAdd)
{
    if ((V.info != _colvec && V.info != _rowvec) || X.ncol != V.ncol * V.nrow) {
        return false;
    }
    int M = X.nrow, N = X.ncol;
    double alpha = 1.0, beta = 0.0;
    if (isAdd) {
        if (outVec.info == _null || outVec.nrow != M || outVec.ncol != 1) {
            return false;
        }
        beta = 1.0;
    } else {
        outVec.resize(M, 1);
        outVec.info = _colvec;
    }
    cblas_dgemv(CblasRowMajor, CblasNoTrans,
                M, N, alpha, X.data, N, V.data, 1, beta, outVec.data, 1);
    return true;
}

bool MML::Mat::VtXmul(const Mat& V, const Mat& X, Mat& outVec, bool isAdd)
{
    if ((V.info != _colvec && V.info != _rowvec) || X.nrow != V.ncol * V.nrow) {
        return false;
    }
    int M = X.nrow, N = X.ncol;
    double alpha = 1.0, beta = 0.0;
    if (isAdd) {
        if (outVec.info == _null || outVec.nrow != 1 || outVec.ncol != N) {
            return false;
        }
        beta = 1.0;
    } else {
        outVec.resize(1, N);
        outVec.info = _rowvec;
    }
    cblas_dgemv(CblasRowMajor, CblasTrans,
                M, N, alpha, X.data, N, V.data, 1, beta, outVec.data, 1);
    return true;
}

bool MML::Mat::VtDiUmul(const Mat& V, const Mat& D, const Mat& U, double& outVal, bool isAdd)
{
    if (V.ncol * V.nrow != U.ncol * U.nrow || V.ncol * V.nrow != D.ncol * D.nrow ||
        (V.info != _colvec && V.info != _rowvec) || (D.info != _colvec && D.info != _rowvec) ||
        (U.info != _colvec && U.info != _rowvec)) {
        return false;
    }
    double tmp = .0;
    int N = V.ncol * V.nrow;
    for (uint64_t i = 0; i < N; ++i) {
        tmp += V.data[i] * U.data[i] / D.data[i];
    }
    if (!isAdd) outVal = tmp;
    else outVal += tmp;
    return true;
}

bool MML::Mat::VtXUmul(const Mat& V, const Mat& X, const Mat& U, double& outVal, bool isAdd)
{
    if (U.ncol * U.nrow != X.ncol || V.ncol * V.nrow != X.nrow ||
        (V.info != _colvec && V.info != _rowvec) || (X.info == _null) ||
        (U.info != _colvec && U.info != _rowvec)) {
        return false;
    }
    int M = X.nrow, N = X.ncol;
    double alpha = 1.0, beta = .0, tmp = .0;
    double* XU = new double [M];
    cblas_dgemv(CblasRowMajor, CblasNoTrans,
                M, N, alpha, X.data, N, U.data, 1, beta, XU, 1);
    for (uint64_t i = 0; i < M; ++i) {
        tmp += V.data[i] * XU[i];
    }
    delete [] XU;
    if (!isAdd) outVal = tmp;
    else outVal += tmp;
    return true;
}

void MML::_reQuickSortEigen(uint64_t* index, Mat &EVal, uint64_t a, uint64_t b, bool isLess)
{
    if (a < b) {
        double tval = EVal(a);
        uint64_t tind = index[a];
        uint64_t i = a, j = b;
        while (a < b) {
            if (isLess) {
                while (a < b && tval < EVal(b)) --b;
                EVal(a) = EVal(b), index[a] = index[b];
                while (a < b && tval >= EVal(a)) ++a;
                EVal(b) = EVal(a), index[b] = index[a];
            } else {
                while (a < b && tval >= EVal(b)) --b;
                EVal(a) = EVal(b), index[a] = index[b];
                while (a < b && tval < EVal(a)) ++a;
                EVal(b) = EVal(a), index[b] = index[a];
            }
        }
        EVal(a) = tval;
        index[a] = tind;
        if (a > 0) {
            _reQuickSortEigen(index, EVal, i, a - 1, isLess);
        }
        _reQuickSortEigen(index, EVal, a + 1, j, isLess);
    }
}

bool MML::quickSortEigen(Mat &EVec, Mat &EVal, bool isLess)
{
    if ((EVal.info != _colvec && EVal.info != _rowvec) || EVec.info == _null ||
        EVec.getNCol() != EVal.getNCol() * EVal.getNRow()) {
        return false;
    }
    uint64_t nCol = EVec.getNCol();
    uint64_t* index = new uint64_t[nCol];
    for (uint64_t i = 0; i < nCol; ++i) {
        index[i] = i;
    }
    _reQuickSortEigen(index, EVal, 0, nCol - 1, isLess);
    EVec.sortCols(nCol, index);
    delete [] index;
    return true;
}
