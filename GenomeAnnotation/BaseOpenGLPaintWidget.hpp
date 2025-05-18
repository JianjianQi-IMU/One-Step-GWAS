#ifndef BASEOPENGLPAINTWIDGET_HPP
#define BASEOPENGLPAINTWIDGET_HPP

#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QList>

class BaseOpenGLPaintWidget : public QOpenGLWidget,public QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    QVector3D iResolution;
protected:
    QList<QOpenGLShaderProgram*>  shaderProgramList;
public:

    explicit BaseOpenGLPaintWidget(QWidget *parent = nullptr)
        : QOpenGLWidget(parent)
    {
        shaderProgramList.append(new QOpenGLShaderProgram);
    }
    virtual ~BaseOpenGLPaintWidget()
    {
        for(QOpenGLShaderProgram* pointer : shaderProgramList){
            if (pointer) {
                delete pointer;
            }
        }
        shaderProgramList.clear();
    }
    inline int addShaderProgram()
    {
        shaderProgramList.append(new QOpenGLShaderProgram);
        return shaderProgramList.size() - 1;
    }
    inline bool shaderProgramBind(int id = 0)
    {
        if (id < 0 || id >= shaderProgramList.size()) {
            return false;
        }
        return shaderProgramList[id]->bind();
    }
    inline void shaderProgramRelease(int id = 0)
    {
        if (id < 0 || id >= shaderProgramList.size()) {
            return ;
        }
        shaderProgramList[id]->release();
    }
    inline void setUniformValue(const char *name, const QColor &color, int id = 0)
    {
        if (id < 0 || id >= shaderProgramList.size()) {
            return ;
        }
        shaderProgramList[id]->setUniformValue(name,color);
    }
    inline void setUniformValue(const char *name, GLfloat value, int id = 0)
    {
        if (id < 0 || id >= shaderProgramList.size()) {
            return ;
        }
        shaderProgramList[id]->setUniformValue(name,value);
    }
    inline void setUniformValue(const char *name, const QVector3D &value, int id = 0)
    {
        if (id < 0 || id >= shaderProgramList.size()) {
            return ;
        }
        shaderProgramList[id]->setUniformValue(name,value);
    }
    inline void setUniformValue(const char *name, const QMatrix4x4 &value, int id = 0)
    {
        if (id < 0 || id >= shaderProgramList.size()) {
            return ;
        }
        shaderProgramList[id]->setUniformValue(name,value);
    }
    inline void setUniformValueArray(const char *name, const QVector3D* values,int count, int id = 0)
    {
        if (id < 0 || id >= shaderProgramList.size()) {
            return ;
        }
        shaderProgramList[id]->setUniformValueArray(name,values,count);
    }
    inline void setGLShaderProgram(const QString &vertShaderFile,
                                   const QString &fragShaderFile,
                                   int id = 0)
    {
        if (id < 0 || id >= shaderProgramList.size()) {
            return ;
        }
        shaderProgramList[id]->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, vertShaderFile);
        shaderProgramList[id]->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, fragShaderFile);
        shaderProgramList[id]->link();
    }

protected:
    virtual void leftMousePressEvent(QMouseEvent* e, bool &isNeedUpdate)
    {
        Q_UNUSED(e);
        Q_UNUSED(isNeedUpdate);
    }
    virtual void rightMousePressEvent(QMouseEvent* e, bool &isNeedUpdate)
    {
        Q_UNUSED(e);
        Q_UNUSED(isNeedUpdate);
    }
    virtual void middleMousePressEvent(QMouseEvent* e, bool &isNeedUpdate)
    {
        Q_UNUSED(e);
        Q_UNUSED(isNeedUpdate);
    }
    virtual void leftMouseReleaseEvent(QMouseEvent* e, bool &isNeedUpdate)
    {
        Q_UNUSED(e);
        Q_UNUSED(isNeedUpdate);
    }
    virtual void rightMouseReleaseEvent(QMouseEvent* e, bool &isNeedUpdate)
    {
        Q_UNUSED(e);
        Q_UNUSED(isNeedUpdate);
    }
    virtual void middleMouseReleaseEvent(QMouseEvent* e, bool &isNeedUpdate)
    {
        Q_UNUSED(e);
        Q_UNUSED(isNeedUpdate);
    }
    virtual void leftMouseMoveEvent(QMouseEvent* e, bool &isNeedUpdate)
    {
        Q_UNUSED(e);
        Q_UNUSED(isNeedUpdate);
    }
    virtual void rightMouseMoveEvent(QMouseEvent* e, bool &isNeedUpdate)
    {
        Q_UNUSED(e);
        Q_UNUSED(isNeedUpdate);
    }
    virtual void middleMouseMoveEvent(QMouseEvent* e, bool &isNeedUpdate)
    {
        Q_UNUSED(e);
        Q_UNUSED(isNeedUpdate);
    }

    void mousePressEvent(QMouseEvent* e) override
    {
        bool isNeedUpdate = false;
        if (e->button() == Qt::LeftButton) {
            leftMousePressEvent(e, isNeedUpdate);
        } else if (e->button() == Qt::MiddleButton) {
            middleMousePressEvent(e, isNeedUpdate);
        } else if (e->button() == Qt::RightButton) {
            rightMousePressEvent(e, isNeedUpdate);
        }
        QOpenGLWidget::mousePressEvent(e);
        if (isNeedUpdate)
        {
            update();
        }
    } // basic
    void mouseReleaseEvent(QMouseEvent* e) override
    {
        bool isNeedUpdate = false;
        if (e->button() == Qt::LeftButton) {
            leftMouseReleaseEvent(e, isNeedUpdate);
        } else if (e->button() == Qt::MiddleButton) {
            middleMouseReleaseEvent(e, isNeedUpdate);
        } else if (e->button() == Qt::RightButton) {
            rightMouseReleaseEvent(e, isNeedUpdate);
        }
        QOpenGLWidget::mouseReleaseEvent(e);
        if (isNeedUpdate)
        {
            update();
        }
    } // basic
    void mouseMoveEvent(QMouseEvent* e) override
    {
        bool isNeedUpdate = false;
        leftMouseMoveEvent(e, isNeedUpdate);
        middleMouseMoveEvent(e, isNeedUpdate);
        rightMouseMoveEvent(e, isNeedUpdate);
        QOpenGLWidget::mouseMoveEvent(e);
        if (isNeedUpdate)
        {
            update();
        }
    } // basic

signals:

};

#endif // BASEOPENGLPAINTWIDGET_HPP
