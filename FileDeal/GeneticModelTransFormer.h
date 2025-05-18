#ifndef GENETICMODELTRANSFORMER_H
#define GENETICMODELTRANSFORMER_H

enum GeneticModelType {
    GMODEL_ADDITIVE = 0,
    GMODEL_REF_DOMINANT = 1,
    GMODEL_ALT_DOMINANT = 2,
    GMODEL_NUM
};

class GeneticModelTransFormer
{
private:
    GeneticModelType mType;

protected:
    void additiveTransform();

public:
    GeneticModelTransFormer(GeneticModelType inType = GMODEL_ADDITIVE);
    void setModelType(GeneticModelType inType);


};

#endif // GENETICMODELTRANSFORMER_H
