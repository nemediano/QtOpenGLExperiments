#ifndef WINDOW_H_
#define WINDOW_H_

#include "Vertex.h"

#include <QVector>
#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Window : public QOpenGLWindow, protected QOpenGLFunctions {
	Q_OBJECT

protected:
	//We are forced to re implement these if we inherit from OpenGLWindow
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	//Helper for cleaning
	void teardownGL();

protected slots:
	void update();

public:
	~Window();

private:
	// OpenGL State Information
	QOpenGLBuffer m_vertex_buffer;
	QOpenGLBuffer m_index_buffer;
	QOpenGLVertexArrayObject m_vao;
	QOpenGLShaderProgram *m_program_ptr;
	
	QVector<Vertex> m_vertices;
	QVector<unsigned int> m_indexes;

	//Vertex Pipeline
	QMatrix4x4 m_model;
	QMatrix4x4 m_view;
	QMatrix4x4 m_projection;

	//Program state variables
	float m_angle = 0.0f;

	//Cache the shader locations
	//int u_model;
	//int u_view;
	//int u_projection;

	// Private Helpers
	void printVersionInformation();
	void createGeomerty();
};

#endif


