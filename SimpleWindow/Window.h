#ifndef WINDOW_H_
#define WINDOW_H_

#include <QOpenGLWindow>
#include <QOpenGLFunctions>

class Window : public QOpenGLWindow, protected QOpenGLFunctions {
	Q_OBJECT

public:

	~Window();
	
	//These are the three method that we need to re implement if we inherit QOpenGLWindow
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	
	//Helper to clean the OpenGL stuff
	void teardownGL();

private:
	//Helper to print context information
	void printContextInformation();
};


#endif

