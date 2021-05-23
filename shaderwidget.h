#ifndef _SHADERWIDGET_H_
#define _SHADERWIDGET_H_

#include <QOpenGLWidget>
#include <QtOpenGL/qgl.h>
#include <QOpenGLShaderProgram>
#include <iostream>

class ShaderWidget : public QOpenGLWidget
{
    Q_OBJECT

private:
	QOpenGLShaderProgram mProgram;
	GLfloat* vertData;
	int vertDataLocation;

    float position_x, position_y, position_z;
    int view; // 0 - z, 1 - x, 2 - -z, 3 - -x

    QVector3D getView() const;
    QVector3D getSide() const;

protected:
	void initializeGL() override;
	void resizeGL(int nWidth, int nHeight) override;
	void paintGL() override;

public:
	ShaderWidget(QWidget* parent = 0);
	~ShaderWidget();

public slots:
    void keyPressEvent(QKeyEvent* _event) override;
};

#endif