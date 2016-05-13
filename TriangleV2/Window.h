#ifndef WINDOW_H_
#define WINDOW_H_

#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

class Window : public QOpenGLWindow, protected QOpenGLFunctions {
	Q_OBJECT

protected:
	//We are forced to re implement these if we inherit from OpenGLWindow
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	//Helper for cleaning
	void teardownGL();
public:
	~Window();
private:
	// OpenGL State Information
	QOpenGLBuffer m_vertex;
	QOpenGLVertexArrayObject m_object;
	QOpenGLShaderProgram *m_program;

	// Private Helpers
	void printVersionInformation();
};

#endif


