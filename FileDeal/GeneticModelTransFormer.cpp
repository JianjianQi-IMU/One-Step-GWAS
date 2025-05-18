#include "GeneticModelTransFormer.h"


GeneticModelTransFormer::GeneticModelTransFormer(GeneticModelType inType)
{
    setModelType(inType);
}

void GeneticModelTransFormer::setModelType(GeneticModelType inType)
{
    mType = inType;
}
