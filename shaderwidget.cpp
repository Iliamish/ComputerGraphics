#include "ShaderWidget.h"

#include <QtOpenGL\qglfunctions.h>
#include <QKeyEvent>
#include <qmessagebox.h>

ShaderWidget::ShaderWidget(QWidget* parent)
{
    vertData = new GLfloat[12]{ -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f };
    
    position_x = 0;
    position_y = 0;
    position_z = -10;

    view = 0;
};

ShaderWidget::~ShaderWidget()
{
    delete[] vertData;
};
//------------------------------------------------

QVector3D ShaderWidget::getView() const
{
    switch (view)
    {
    case 0:
        return QVector3D(0.0, 0.0, 1.0);
    case 1:
        return QVector3D(1.0, 0.0, 0.0);
    case 2:
        return QVector3D(0.0, 0.0, -1.0);
    case 3:
        return QVector3D(-1.0, 0.0, 0.0);
    }
};

QVector3D ShaderWidget::getSide() const
{
    switch (view)
    {
    case 0:
        return QVector3D(1.0, 0.0, 0.0);
    case 1:
        return QVector3D(0.0, 0.0, -1.0);
    case 2:
        return QVector3D(-1.0, 0.0, 0.0);
    case 3:
        return QVector3D(0.0, 0.0, 1.0);
    }
};

void ShaderWidget::initializeGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    QOpenGLShader vShader(QOpenGLShader::Vertex);
    vShader.compileSourceFile("raytracing.vert");

    QOpenGLShader fShader(QOpenGLShader::Fragment);
    fShader.compileSourceFile("raytracing.frag");

    mProgram.addShader(&vShader);
    mProgram.addShader(&fShader);

    if (!mProgram.link())
    {
        QMessageBox::warning(this, "Warning", "Error bind programm shader!");
        return;
    }

    vertDataLocation = mProgram.attributeLocation("vertex");

    if (!mProgram.bind())
    {
        QMessageBox::warning(this, "Warning", "Error bind programm shader!");
        return;
    }

    mProgram.setUniformValue("camera.position", QVector3D(position_x, position_y, position_z));
    mProgram.setUniformValue("camera.view", getView());
    mProgram.setUniformValue("camera.up", QVector3D(0.0, 1.0, 0.0));
    mProgram.setUniformValue("camera.side", getSide());

    mProgram.setUniformValue("spherein[0].center", QVector3D(1.0, 1.0, 2.0));
    mProgram.setUniformValue("spherein[0].color", QVector3D(0.5, 0.0, 1.0));
    mProgram.setUniformValue("spherein[0].radius", 1.0f);
    mProgram.setUniformValue("spherein[1].center", QVector3D(3.0, -3.0, 1.0));
    mProgram.setUniformValue("spherein[1].color", QVector3D(0.0, 0.5, 1.0));
    mProgram.setUniformValue("spherein[1].radius", 2.0f);

    // left
    mProgram.setUniformValue("trianglein[0].v1", QVector3D(-5.0, -5.0, -5.0));
    mProgram.setUniformValue("trianglein[0].v2", QVector3D(-5.0, 5.0, -5.0));
    mProgram.setUniformValue("trianglein[0].v3", QVector3D(-5.0, 5.0, 5.0));
    mProgram.setUniformValue("trianglein[0].color", QVector3D(1.0, 1.0, 0.0));
    mProgram.setUniformValue("trianglein[1].v1", QVector3D(-5.0, -5.0, -5.0));
    mProgram.setUniformValue("trianglein[1].v2", QVector3D(-5.0, 5.0, 5.0));
    mProgram.setUniformValue("trianglein[1].v3", QVector3D(-5.0, -5.0, 5.0));
    mProgram.setUniformValue("trianglein[1].color", QVector3D(1.0, 1.0, 0.0));

    // right
    mProgram.setUniformValue("trianglein[2].v1", QVector3D(5.0, -5.0, -5.0));
    mProgram.setUniformValue("trianglein[2].v2", QVector3D(5.0, 5.0, 5.0));
    mProgram.setUniformValue("trianglein[2].v3", QVector3D(5.0, 5.0, -5.0));
    mProgram.setUniformValue("trianglein[2].color", QVector3D(1.0, 1.0, 0.0));
    mProgram.setUniformValue("trianglein[3].v1", QVector3D(5.0, -5.0, -5.0));
    mProgram.setUniformValue("trianglein[3].v2", QVector3D(5.0, -5.0, 5.0));
    mProgram.setUniformValue("trianglein[3].v3", QVector3D(5.0, 5.0, 5.0));
    mProgram.setUniformValue("trianglein[3].color", QVector3D(1.0, 1.0, 0.0));

    // front
    mProgram.setUniformValue("trianglein[4].v1", QVector3D(-5.0, -5.0, 5.0));
    mProgram.setUniformValue("trianglein[4].v2", QVector3D(-5.0, 5.0, 5.0));
    mProgram.setUniformValue("trianglein[4].v3", QVector3D(5.0, -5.0, 5.0));
    mProgram.setUniformValue("trianglein[4].color", QVector3D(1.0, 1.0, 0.0));
    mProgram.setUniformValue("trianglein[5].v1", QVector3D(5.0, 5.0, 5.0));
    mProgram.setUniformValue("trianglein[5].v2", QVector3D(5.0, -5.0, 5.0));
    mProgram.setUniformValue("trianglein[5].v3", QVector3D(-5.0, 5.0, 5.0));
    mProgram.setUniformValue("trianglein[5].color", QVector3D(1.0, 1.0, 0.0));

    // up
    mProgram.setUniformValue("trianglein[6].v1", QVector3D(-5.0, 5.0, -5.0));
    mProgram.setUniformValue("trianglein[6].v2", QVector3D(5.0, 5.0, -5.0));
    mProgram.setUniformValue("trianglein[6].v3", QVector3D(-5.0, 5.0, 5.0));
    mProgram.setUniformValue("trianglein[6].color", QVector3D(1.0, 1.0, 0.0));
    mProgram.setUniformValue("trianglein[7].v1", QVector3D(5.0, 5.0, 5.0));
    mProgram.setUniformValue("trianglein[7].v2", QVector3D(-5.0, 5.0, 5.0));
    mProgram.setUniformValue("trianglein[7].v3", QVector3D(5.0, 5.0, -5.0));
    mProgram.setUniformValue("trianglein[7].color", QVector3D(1.0, 1.0, 0.0));

    // down
    mProgram.setUniformValue("trianglein[8].v1", QVector3D(-5.0, -5.0, -5.0));
    mProgram.setUniformValue("trianglein[8].v2", QVector3D(-5.0, -5.0, 5.0));
    mProgram.setUniformValue("trianglein[8].v3", QVector3D(5.0, -5.0, -5.0));
    mProgram.setUniformValue("trianglein[8].color", QVector3D(1.0, 1.0, 0.0));
    mProgram.setUniformValue("trianglein[9].v1", QVector3D(5.0, -5.0, 5.0));
    mProgram.setUniformValue("trianglein[9].v2", QVector3D(5.0, -5.0, -5.0));
    mProgram.setUniformValue("trianglein[9].v3", QVector3D(-5.0, -5.0, 5.0));
    mProgram.setUniformValue("trianglein[9].color", QVector3D(1.0, 1.0, 0.0));

    // tetr
    mProgram.setUniformValue("trianglein[10].v1", QVector3D(-5.0, -5.0, 5.0));
    mProgram.setUniformValue("trianglein[10].v2", QVector3D(-1.0, -5.0, 5.0));
    mProgram.setUniformValue("trianglein[10].v3", QVector3D(-3.0, -5.0, 1.54));
    mProgram.setUniformValue("trianglein[10].color", QVector3D(0.0, 0.5, 1.0));
    mProgram.setUniformValue("trianglein[11].v1", QVector3D(-5.0, -5.0, 5.0));
    mProgram.setUniformValue("trianglein[11].v2", QVector3D(-1.0, -5.0, 5.0));
    mProgram.setUniformValue("trianglein[11].v3", QVector3D(-3.0, -1.73, 3.85));
    mProgram.setUniformValue("trianglein[11].color", QVector3D(0.0, 0.5, 1.0));
    mProgram.setUniformValue("trianglein[12].v1", QVector3D(-5.0, -5.0, 5.0));
    mProgram.setUniformValue("trianglein[12].v2", QVector3D(-3.0, -5.0, 1.54));
    mProgram.setUniformValue("trianglein[12].v3", QVector3D(-3.0, -1.73, 3.85));
    mProgram.setUniformValue("trianglein[12].color", QVector3D(0.0, 0.5, 1.0));
    mProgram.setUniformValue("trianglein[13].v1", QVector3D(-1.0, -5.0, 5.0));
    mProgram.setUniformValue("trianglein[13].v2", QVector3D(-3.0, -5.0, 1.54));
    mProgram.setUniformValue("trianglein[13].v3", QVector3D(-3.0, -1.73, 3.85));
    mProgram.setUniformValue("trianglein[13].color", QVector3D(0.0, 0.5, 1.0));

    mProgram.setUniformValue("scale", QVector2D(width(), height()));

    mProgram.release();
};

void ShaderWidget::resizeGL(int nWidth, int nHeight)
{
    glViewport(0, 0, nWidth, nHeight);
    
    if (!mProgram.bind())
    {
        QMessageBox::warning(this, "Warning", "Error bind programm shader!");
        return;
    }

    mProgram.setUniformValue("scale", QVector2D(width(), height()));
    mProgram.release();
};

void ShaderWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (!mProgram.bind())
        return;

    mProgram.enableAttributeArray(vertDataLocation);
    mProgram.setAttributeArray(vertDataLocation, vertData, 3);

    glDrawArrays(GL_QUADS, 0, 4);

    mProgram.disableAttributeArray(vertDataLocation);

    mProgram.release();
};

void ShaderWidget::keyPressEvent(QKeyEvent* _event)
{
    
    if (_event->nativeVirtualKey() == Qt::Key_W)
    {
        switch (view)
        {
        case 0:
            position_z++;
            break;
        case 1:
            position_x++;
            break;
        case 2:
            position_z--;
            break;
        case 3:
            position_x--;
            break;
        }
    }
    else if (_event->nativeVirtualKey() == Qt::Key_S)
    {
        switch (view)
        {
        case 0:
            position_z--;
            break;
        case 1:
            position_x--;
            break;
        case 2:
            position_z++;
            break;
        case 3:
            position_x++;
            break;
        }
    }
    else if (_event->nativeVirtualKey() == Qt::Key_D)
    {
        switch (view)
        {
        case 0:
            position_x++;
            break;
        case 1:
            position_z--;
            break;
        case 2:
            position_x--;
            break;
        case 3:
            position_z++;
            break;
        }
    }
    else if (_event->nativeVirtualKey() == Qt::Key_A)
    {
        switch (view)
        {
        case 0:
            position_x--;
            break;
        case 1:
            position_z++;
            break;
        case 2:
            position_x++;
            break;
        case 3:
            position_z--;
            break;
        }
    }
    else if (_event->nativeVirtualKey() == Qt::Key_I)
    {
        position_y++;
    }
    else if (_event->nativeVirtualKey() == Qt::Key_K)
    {
        position_y--;
    }
    if (_event->nativeVirtualKey() == Qt::Key_J)
    {
        view = (view + 3) % 4;
    }
    else if (_event->nativeVirtualKey() == Qt::Key_L)
    {
        view = (view + 1) % 4;
    }

    if (!mProgram.bind())
    {
        QMessageBox::warning(this, "Warning", "Error bind programm shader!");
        return;
    }
    
    mProgram.setUniformValue("camera.position", QVector3D(position_x, position_y, position_z));
    mProgram.setUniformValue("camera.view", getView());
    mProgram.setUniformValue("camera.side", getSide());
    mProgram.release();

    update();
};