#ifndef STRUCTURESORTPAINTSEQ_HPP
#define STRUCTURESORTPAINTSEQ_HPP

#include <vector>

template <typename Value,typename IDClass = int>
class stdVectorSmall
{
private:
    std::vector<Value>* p;

public:
    stdVectorSmall():p(nullptr){};
    stdVectorSmall(std::vector<Value>* inVec):p(nullptr){
        setVector(inVec);
    }
    bool setVector(std::vector<Value>* inVec){
        if(!inVec) return false;
        p = inVec;
        return true;
    }
    Value operator () (IDClass id){
        return (*p)[id];
    }
};


template<typename CompValue,typename IDClass = int>
class StructureSortPaintSeq
{
private:
    CompValue* pCompValue;
    void pri_reQSortUp(int a,int b,std::vector<IDClass>& L){
        if(a<b){
            int i=a,j=b;
            IDClass t=L[i];
            while (i<j){
                while (i<j&&!((*pCompValue)(L[j])<(*pCompValue)(t))) --j;
                L[i] = L[j];
                while (i<j&&(*pCompValue)(L[i])<(*pCompValue)(t)) ++i;
                L[j] = L[i];
            }
            L[i] = t;
            pri_reQSortUp(a,i-1,L);
            pri_reQSortUp(i+1,b,L);
        }
    }
    void pri_reQSortDown(int a,int b,std::vector<IDClass>& L){
        if(a<b){
            int i=a,j=b;
            IDClass t=L[i];
            while (i<j){
                while (i<j&&!((*pCompValue)(L[j])>(*pCompValue)(t))) --j;
                L[i] = L[j];
                while (i<j&&(*pCompValue)(L[i])>(*pCompValue)(t)) ++i;
                L[j] = L[i];
            }
            L[i] = t;
            pri_reQSortUp(a,i-1,L);
            pri_reQSortUp(i+1,b,L);
        }
    }
public:
    StructureSortPaintSeq():pCompValue(nullptr){}
    StructureSortPaintSeq(CompValue* compval):StructureSortPaintSeq(){
        setCompValue(compval);
    }
    bool setCompValue(CompValue* compval){
        if(compval){
            pCompValue = compval;
            return true;
        }
        return false;
    }
    bool sortUp(int first,int last,std::vector<IDClass>& idList){
        if(!pCompValue) return false;
        if(first > last) return false;
        pri_reQSortUp(first,last,idList);
        return true;
    }
    bool sortDown(int first,int last,std::vector<IDClass>& idList){
        if(!pCompValue) return false;
        if(first > last) return false;
        pri_reQSortDown(first,last,idList);
        return true;
    }
};

#endif // STRUCTURESORTPAINTSEQ_HPP
