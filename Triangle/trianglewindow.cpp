#include "stdafx.h"
#include "trianglewindow.h"


TriangleWindow::TriangleWindow() :
m_program(nullptr),
m_frame(0)
{
}


void TriangleWindow::initialize() {
	m_program = new QOpenGLShaderProgram(this);
	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/OpenGLWindow/Resources/vertexShader.glsl")) {
		qDebug() << "Could not compile vertex shader!!";
		qDebug() << m_program->log();
	}
	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/OpenGLWindow/Resources/fragmentShader.glsl")) {
		qDebug() << "Could not compile fragment shader!!";
		qDebug() << m_program->log();
	}
	if (!m_program->link()) {
		qDebug() << "Could not link the program!!";
		qDebug() << m_program->log();
	}

	m_posAttr = m_program->attributeLocation("posAttr");
	m_colAttr = m_program->attributeLocation("colAttr");
	m_matrixUniform = m_program->uniformLocation("matrix");

	m_fovy_y = 60.0f;
}

void TriangleWindow::render() {
	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);
	m_aspect_ratio = (width() * retinaScale) / (height() * retinaScale);

	glClear(GL_COLOR_BUFFER_BIT);

	m_program->bind();


	QMatrix4x4 matrix;
	//Projection
	float zNear = 0.1f;
	float zFar = 100.0f;
	matrix.perspective(m_fovy_y, m_aspect_ratio, zNear, zFar);
	//View
	QVector3D eye(0.0, 0.0, 2.0);
	QVector3D center(0.0f, 0.0f, 0.0f);
	QVector3D up(0.0f, 1.0f, 0.0f);
	matrix.lookAt(eye, center, up);
	//Model
	//matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);

	m_program->setUniformValue(m_matrixUniform, matrix);

	GLfloat vertices[] = {
		 //0.0f,  0.707f,
		-0.5f,  0.5f,
		 0.5f,  0.5f,
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f, 0.5f,
		-0.5f, -0.5f,
	};

	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};

	glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

	glEnableVertexAttribArray(m_posAttr);
	glEnableVertexAttribArray(m_colAttr);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(m_posAttr);
	glDisableVertexAttribArray(m_colAttr);

	m_program->release();

	++m_frame;
}

