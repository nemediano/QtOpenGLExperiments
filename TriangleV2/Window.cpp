#include <QDebug>
#include <QString>

#include "Vertex.h"
#include "Window.h"

// Create a colored triangle
static const Vertex sg_vertexes[] = {
	Vertex(QVector3D(0.00f, 0.75f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f)),
	Vertex(QVector3D(0.75f, -0.75f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f)),
	Vertex(QVector3D(-0.75f, -0.75f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f))
};


Window::~Window() {
	makeCurrent();
	teardownGL();
}

void Window::initializeGL() {
	// Initialize OpenGL Backend
	initializeOpenGLFunctions();
	printVersionInformation();

	//Global GL configurations
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Application-specific initialization
	{
		// Create Shader (Do not release until VAO is created)
		m_program = new QOpenGLShaderProgram();
		m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "Resources/simpleVertex.glsl");
		m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "Resources/simpleFragment.glsl");
		m_program->link();
		m_program->bind();

		// Create Buffer (Do not release until VAO is created)
		m_vertex.create();
		m_vertex.bind();
		m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
		m_vertex.allocate(sg_vertexes, sizeof(sg_vertexes));

		// Create Vertex Array Object
		m_object.create();
		m_object.bind();
		m_program->enableAttributeArray(0);
		m_program->enableAttributeArray(1);
		m_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
		m_program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

		// Release (unbind) all
		m_object.release();
		m_vertex.release();
		m_program->release();
	}
}

void Window::teardownGL() {
	// Actually destroy our OpenGL information
	m_object.destroy();
	m_vertex.destroy();
	delete m_program;
}

void Window::resizeGL(int width, int height) {
	
}

void Window::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT);
	// Render using our shader
	m_program->bind();
	{
		m_object.bind();
		glDrawArrays(GL_TRIANGLES, 0, sizeof(sg_vertexes) / sizeof(sg_vertexes[0]));
		m_object.release();
	}
	m_program->release();
}


void Window::printVersionInformation() {
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
