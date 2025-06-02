#include "InsertTextEditorFrame.hpp"
#include "ui_InsertTextEditorFrame.h"

InsertTextEditorFrame::InsertTextEditorFrame(QWidget *parent)
    : QWidget(parent), ui(new Ui::InsertTextEditorFrame)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint);
}

InsertTextEditorFrame::~InsertTextEditorFrame()
{
    delete ui;
}
