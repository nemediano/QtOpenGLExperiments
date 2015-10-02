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
	virtual void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	//virtual void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
	
	virtual void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;

private:
	GLfloat m_aspect_ratio;
	GLfloat m_fovy_y;
	GLuint m_posAttr;
	GLuint m_colAttr;
	GLuint m_matrixUniform;
	GLuint m_vbo;
	GLuint m_indexBuffer;

	QOpenGLShaderProgram* m_program;
	int m_frame;
	
	size_t m_nVertex;
	size_t m_nIndices;
	size_t m_nTriangles;

	//Trackball camera related
	bool m_mouse_draging;
	QVector2D m_mouse_start_drag;
	QQuaternion m_base_rotation;
	QQuaternion m_new_rotation;
	float projection_on_curve(const QVector2D& projected);
	
	
	void create_primitive();
};

struct Vertex {
	QVector3D position;
	QVector3D color;
};

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))
#define OFFSET_OF(type, member) ((GLvoid*)(offsetof(type, member)))

#endif // TRIANGLEWINDOW_H