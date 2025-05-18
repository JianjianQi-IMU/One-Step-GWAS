#ifndef INSERTTEXTEDITORFRAME_HPP
#define INSERTTEXTEDITORFRAME_HPP

#include <QWidget>
#include <QPaintEvent>

namespace Ui {
class InsertTextEditorFrame;
}

class InsertTextEditorFrame : public QWidget
{
    Q_OBJECT

public:
    explicit InsertTextEditorFrame(QWidget *parent = nullptr);
    ~InsertTextEditorFrame();

private:
    Ui::InsertTextEditorFrame *ui;
};

#endif // INSERTTEXTEDITORFRAME_HPP
