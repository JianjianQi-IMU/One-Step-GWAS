#ifndef VALPOINTS_HPP
#define VALPOINTS_HPP

class ValPoints{
public:
    double val;
    long long pos;
    int idchr;
    ValPoints();
    explicit ValPoints(int inidchr, long long inPos);
    bool operator<(const ValPoints& B) const;
    bool operator<(long long Pos) const;
};

class ValPoints2{
public:
    double val1;
    double val2;
    long long pos;
    int idchr;
    ValPoints2();
    explicit ValPoints2(int inidchr, long long inPos);
    bool operator<(const ValPoints2& B) const;
    bool operator<(long long Pos) const;
};
#endif // VALPOINTS_HPP
