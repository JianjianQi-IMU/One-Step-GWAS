#include "ProjectFilesSet.h"

namespace FD {

BaseProjectFilesSet::BaseProjectFilesSet()
{
    info = PROJECT_NULL;
}

BaseProjectFilesSet::~BaseProjectFilesSet()
{

}

const QString GWASProjectFilesSet::genoName = "GENO";

const QString GWASProjectFilesSet::bimName  = "BIM";

const QString GWASProjectFilesSet::covName  = "COV";

const QString GWASProjectFilesSet::kinName  = "KIN";

const QString GWASProjectFilesSet::pheName  = "PHE";

const QString GWASProjectFilesSet::GWASName = "GWAS";

GWASProjectFilesSet::GWASProjectFilesSet() : BaseProjectFilesSet()
{
    info = PROJECT_GWAS;
}



}; // namespace FD
