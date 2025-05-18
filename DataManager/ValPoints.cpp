#include "ValPoints.hpp"

ValPoints::ValPoints()
{

}

ValPoints::ValPoints(int inidchr, long long inPos)
{
    idchr = inidchr;
    pos = inPos;
}

bool ValPoints::operator<(const ValPoints &B) const
{
    if (idchr != B.idchr) return idchr < B.idchr;
    return pos < B.pos;
}

bool ValPoints::operator<(long long Pos) const
{
    return pos < Pos;
}

ValPoints2::ValPoints2()
{

}

ValPoints2::ValPoints2(int inidchr, long long inPos)
{
    idchr = inidchr;
    pos = inPos;
}

bool ValPoints2::operator<(const ValPoints2 &B) const
{
    if (idchr != B.idchr) return idchr < B.idchr;
    return pos < B.pos;
}

bool ValPoints2::operator<(long long Pos) const
{
    return pos < Pos;
}
