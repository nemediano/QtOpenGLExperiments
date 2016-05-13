#include "Window.h"
#include <QDebug>
#include <QString>

Window::~Window() {
	makeCurrent();
	teardownGL();
}

void Window::initializeGL() {
	// Initialize OpenGL Backend
	//After this call any OpenGL function will be valid
	//For the current context
	initializeOpenGLFunctions();
	printContextInformation();

	//Global GL configurations
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Window::resizeGL(int width, int height) {
	//Currently we are not handling width/height changes
}

void Window::paintGL() {
	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

}

void Window::teardownGL() {
	//Nothing to destroy here yet
}

void Window::printContextInformation() {
	QString glType;
	QString glVersion;
	QString glProfile;
	QString glRenderer;
	QString glVendor;
	QString qtVersion = QT_VERSION_STR;

	// Get Version Information
	glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL (Desktop)";
	glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	glRenderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	glVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	// Get Profile Information

	switch (format().profile())	{
		case QSurfaceFormat::OpenGLContextProfile::CoreProfile:
			glProfile = "Core";
		break;

		case QSurfaceFormat::OpenGLContextProfile::CompatibilityProfile:
			glProfile = "Compatibility";
		break;

		case QSurfaceFormat::OpenGLContextProfile::NoProfile:
		default:
			glProfile = "unspecified";
		break;
	}
	
	qDebug().noquote() << "Using Qt version: " << qtVersion;
	qDebug().noquote() << "GPU:" << glRenderer;
	qDebug().noquote() << "Using" << glType << "in" << glProfile << "profile";
	qDebug().noquote() << "Version:" << glVersion;
	qDebug().noquote() << "Driver provided by" << glVendor;
	
}
