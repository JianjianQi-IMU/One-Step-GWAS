#ifndef GENOMEVIEWTEXTEDITPAINTER_HPP
#define GENOMEVIEWTEXTEDITPAINTER_HPP

#include <QTextEdit>

class GenomeViewTextEdit : public QTextEdit
{
    Q_OBJECT
private:
public:
    GenomeViewTextEdit(QWidget* parent = nullptr):
        QTextEdit(parent)
    {

    }
};



#endif // GENOMEVIEWTEXTEDITPAINTER_HPP
