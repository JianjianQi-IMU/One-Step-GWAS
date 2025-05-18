#ifndef CLUSTER_HPP
#define CLUSTER_HPP

template <typename T,typename TVal>
class TreeNode{
private:
    T val;
    TVal tval;
    int n;
    TreeNode<T,TVal>* left;
    TreeNode<T,TVal>* right;
public:
    explicit TreeNode(const T& inVal,
                      TreeNode<T,TVal>* inLeft = nullptr,
                      TreeNode<T,TVal>* inRight = nullptr){
        val = inVal;
        left = inLeft;
        right = inRight;
    }
    ~TreeNode()
    {
        if(left) delete left;
        if(right) delete right;
    }
    const T& getValue()
    {
        return val;
    }
    bool setValue(const T& inVal)
    {
        val = inVal;
        return true;
    }
    const TVal& getTVal()
    {
        return tval;
    }
    bool setTVal(const TVal& inTVal)
    {
        tval = inTVal;
        return true;
    }
    int getNum()
    {
        return n;
    }
    bool setNum(int inNum)
    {
        n = inNum;
        return true;
    }
    bool setLChild(TreeNode<T,TVal>* inLeft = nullptr)
    {
        left = inLeft;
        return true;
    }
    bool setRChild(TreeNode<T,TVal>* inRight = nullptr)
    {
        right = inRight;
        return true;
    }
    TreeNode<T,TVal>* lChild()
    {
        return left;
    }
    TreeNode<T,TVal>* rChild()
    {
        return right;
    }
};

template <typename T, typename Dis, typename TVal = double, typename DisVal = double>
class HierarchicalCluster
{
private:
    TreeNode<T, TVal>* root;
    Dis* dis;
    bool sumValGroup(TreeNode<T, TVal>* p, TVal* outVal, int* outNum)
    {
        (*outVal) += (*dis)(p->getValue());
        ++(*outNum);
        if (p->lChild()) sumValGroup(p->lChild(), outVal, outNum);
        if (p->rChild()) sumValGroup(p->rChild(), outVal, outNum);
    }
    TVal getMeanValGroup(TreeNode<T, TVal>* p){
        TVal mean = 0;
        int sumNum = 0;
        sumValGroup(p, &mean, &sumNum);
        return mean / sumNum;
    }
    bool sumDisOneGroup(TreeNode<T, TVal>* p, TreeNode<T, TVal>* pGroup, DisVal* outVal, int* outNum)
    {
        (*outVal) = (*outVal)+(*dis)(p->getValue(), pGroup->getValue());
        ++(*outNum);
        if (pGroup->lChild()) sumDisOneGroup(p, pGroup->lChild(), outVal, outNum);
        if (pGroup->rChild()) sumDisOneGroup(p, pGroup->rChild(), outVal, outNum);
        return true;
    }
    bool sumDisGroupGroup(TreeNode<T, TVal>* pGroup1, TreeNode<T, TVal>* pGroup2, DisVal* outVal, int* outNum)
    {
        sumDisOneGroup(pGroup1, pGroup2, outVal, outNum);
        if (pGroup1->lChild()) sumDisGroupGroup(pGroup1->lChild(), pGroup2, outVal, outNum);
        if (pGroup1->rChild()) sumDisGroupGroup(pGroup1->rChild(), pGroup2, outVal, outNum);
        return true;
    }
    DisVal meanDistance(TreeNode<T, TVal>* a, TreeNode<T, TVal>* b)
    {
        DisVal mean = 0;
        int sumNum = 0;
        sumDisGroupGroup(a, b, &mean, &sumNum);
        return mean / sumNum;
    }
    void getSeqRecursion(TreeNode<T, TVal>* p, T* out, int* id)
    {
        if (p->lChild()) getSeqRecursion(p->lChild(), out, id);
        if (!(p->lChild()) && !(p->rChild())) {
            out[*id] = p->getValue();
            ++(*id);
        }
        if (p->rChild()) getSeqRecursion(p->rChild(), out, id);
    }
public:
    explicit HierarchicalCluster(Dis* inDistance)
    {
        dis = inDistance;
        root = nullptr;
    }
    ~HierarchicalCluster()
    {
        if (root) delete root;
    }

    bool meanCluster(T* inData, int n)
    {
        if (!inData || n == 0) return false;
        if (root) delete root;
        TreeNode<T, TVal>** list = new TreeNode<T, TVal>*[n];
        int i, j, k, tj, tk;
        for (i = 0; i < n; ++i) {
            list[i] = new TreeNode<T, TVal>(inData[i]);
            list[i]->setNum(1);
            list[i]->setTVal((*dis)(inData[i]));
        }
        DisVal tmin, tval;
        TreeNode<T, TVal>* tpointer = nullptr;
        for (i = n - 1; i > 0; --i) {
            tmin=meanDistance(list[0], list[1]);
            tj = 0, tk = 1;
            for (j = 0; j < i; ++j) {
                for (k = j + 1; k <= i; ++k) {
                    tval = meanDistance(list[j], list[k]);
                    if (tval < tmin) {
                        tj = j, tk = k;
                        tmin = tval;
                    }
                }
            }
            tpointer = new TreeNode<T, TVal>(0);
            if (list[tj]->getTVal() < list[tk]->getTVal()) {
                tpointer->setLChild(list[tj]);
                tpointer->setRChild(list[tk]);
                tpointer->setNum(list[tj]->getNum() + list[tk]->getNum());
                tpointer->setTVal(
                                  (list[tj]->getNum() * list[tj]->getTVal() + list[tk]->getNum() * list[tk]->getTVal()) /
                                  (list[tj]->getNum() + list[tk]->getNum())
                                  );
                list[tj] = tpointer;
                list[tk] = list[i];
            } else {
                tpointer->setLChild(list[tk]);
                tpointer->setRChild(list[tj]);
                tpointer->setNum(list[tj]->getNum() + list[tk]->getNum());
                tpointer->setTVal(
                                  (list[tj]->getNum() * list[tj]->getTVal() + list[tk]->getNum() * list[tk]->getTVal()) /
                                  (list[tj]->getNum() + list[tk]->getNum())
                                  );
                list[tk] = tpointer;
                list[tj] = list[i];
            }

        }
        root = list[0];
        delete [] list;
        return true;
    }
    bool getSeq(T* outData)
    {
        if (!root) return false;
        int id;
        getSeqRecursion(root, outData, &id);
        return true;
    }
};

#endif // CLUSTER_HPP
