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
	
	virtual void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
	virtual void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
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

	//Camera state variables
	bool m_mouse_draging;
	QVector2D m_mouse_start_drag;
	QQuaternion m_base_rotation;
	QQuaternion m_new_rotation;
	float projection_on_curve(const QVector2D& projected);
	float m_fovy_y;

	// Private Helpers
	void printVersionInformation();
	void createGeomerty();
};

#endif


