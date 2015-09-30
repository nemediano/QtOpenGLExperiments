#ifndef TRIANGLEWINDOW_H
#define TRIANGLEWINDOW_H

/*
This is the window that we are actually using to OpenGL 
related rendering.
This is where all the application logic should go.
All the global constant usually committed in a
freeglut application should go here
*/

#include "openglwindow.h"

class TriangleWindow : public OpenGLWindow
{
public:
	TriangleWindow();

	void initialize() Q_DECL_OVERRIDE;
	void render() Q_DECL_OVERRIDE;
	
protected:
	/* Override this methods form QWindow to implement the user interaction */
	//virtual void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	//virtual void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	//virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	//virtual void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
	
	//virtual void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;

private:
	GLfloat m_aspect_ratio;
	GLfloat m_fovy_y;
	GLuint m_posAttr;
	GLuint m_colAttr;
	GLuint m_matrixUniform;

	QOpenGLShaderProgram* m_program;
	int m_frame;
};

#endif // TRIANGLEWINDOW_H