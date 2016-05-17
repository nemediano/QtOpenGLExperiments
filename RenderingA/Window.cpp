#include <QDebug>
#include <QString>

#include "Vertex.h"
#include "Window.h"

// Create data to send to GPU
void Window::createGeomerty() {
	//Vertex
	m_vertices.clear();
	//Front Vertexes
	m_vertices.push_back(Vertex(QVector3D( 0.5f,  0.5f,  0.5f), QVector3D(1.0f, 0.0f, 0.0f)));
	m_vertices.push_back(Vertex(QVector3D(-0.5f,  0.5f,  0.5f), QVector3D(0.0f, 1.0f, 0.0f)));
	m_vertices.push_back(Vertex(QVector3D(-0.5f, -0.5f,  0.5f), QVector3D(0.0f, 0.0f, 1.0f)));
	m_vertices.push_back(Vertex(QVector3D( 0.5f, -0.5f,  0.5f), QVector3D(0.0f, 0.0f, 0.0f)));
	//Back Vertexes
	m_vertices.push_back(Vertex(QVector3D( 0.5f,  0.5f, -0.5f), QVector3D(1.0f, 1.0f, 0.0f)));
	m_vertices.push_back(Vertex(QVector3D(-0.5f,  0.5f, -0.5f), QVector3D(0.0f, 1.0f, 1.0f)));
	m_vertices.push_back(Vertex(QVector3D(-0.5f, -0.5f, -0.5f), QVector3D(1.0f, 0.0f, 1.0f)));
	m_vertices.push_back(Vertex(QVector3D( 0.5f, -0.5f, -0.5f), QVector3D(1.0f, 1.0f, 1.0f)));

	//Creating geometry using index
	m_indexes.clear();
	//Face 1 (Front)
	m_indexes.push_back(0); m_indexes.push_back(1); m_indexes.push_back(2);
	m_indexes.push_back(2); m_indexes.push_back(3); m_indexes.push_back(0);
	// Face 2 (Back)
	m_indexes.push_back(7); m_indexes.push_back(5); m_indexes.push_back(4);
	m_indexes.push_back(5); m_indexes.push_back(7); m_indexes.push_back(6);
	// Face 3 (Top)
	m_indexes.push_back(0); m_indexes.push_back(4); m_indexes.push_back(5);
	m_indexes.push_back(5); m_indexes.push_back(1); m_indexes.push_back(0);
	// Face 4 (Bottom)
	m_indexes.push_back(3); m_indexes.push_back(2); m_indexes.push_back(6);
	m_indexes.push_back(6); m_indexes.push_back(7); m_indexes.push_back(3);
	// Face 5 (Left)
	m_indexes.push_back(2); m_indexes.push_back(1); m_indexes.push_back(5);
	m_indexes.push_back(2); m_indexes.push_back(5); m_indexes.push_back(6);
	// Face 6 (Right)
	m_indexes.push_back(0); m_indexes.push_back(3); m_indexes.push_back(7);
	m_indexes.push_back(7); m_indexes.push_back(4); m_indexes.push_back(0);
}


Window::~Window() {
	makeCurrent();
	teardownGL();
}

void Window::update() {
	// Update instance information
	m_angle += 1.0;
	
	// Schedule a redraw to continue this cycle
	QOpenGLWindow::update();
}

void Window::initializeGL() {
	// Initialize OpenGL Backend
	initializeOpenGLFunctions();
	//This is equivalent to register an IDLE function callback for animation
	//We don't control the frame rate is as fast as the system is available
	//Maybe using the vsync
	connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
	printVersionInformation();
	
	//Global GL configurations
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	// Application-specific initialization
	{
		// Create Shader (Do not release until VAO is created)
		m_program_ptr = new QOpenGLShaderProgram();
		m_program_ptr->addShaderFromSourceFile(QOpenGLShader::Vertex, "Resources/shaders/simpleVertex.glsl");
		m_program_ptr->addShaderFromSourceFile(QOpenGLShader::Fragment, "Resources/shaders/simpleFragment.glsl");
		m_program_ptr->link();
		m_program_ptr->bind();

		// Cache Uniform Locations
		//u_model = m_program_ptr->uniformLocation("M");
		//u_view =  m_program_ptr->uniformLocation("V");
		//u_projection = m_program_ptr->uniformLocation("P");

		//Fill the arrays with date in CPU side
		createGeomerty();
		// Create Vertex Array Object (Remember this needs to be done BEFORE binding the vertex)
		m_vao.create();
		m_vao.bind();

		// Create Buffers (Do not release until VAO is created)
		m_vertex_buffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
		m_vertex_buffer.create();
		m_vertex_buffer.bind();
		m_vertex_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
		m_vertex_buffer.allocate(m_vertices.constData(), m_vertices.size() * sizeof(Vertex));

		m_index_buffer = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
		m_index_buffer.create();
		m_index_buffer.bind();
		m_index_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
		m_index_buffer.allocate(m_indexes.constData(), m_indexes.size() * sizeof(unsigned int));
		
		
		m_program_ptr->enableAttributeArray(0);
		m_program_ptr->enableAttributeArray(1);
		m_program_ptr->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
		m_program_ptr->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

		// Release (unbind) all
		m_vao.release();
		m_index_buffer.release();
		m_index_buffer.release();
		m_program_ptr->release();

		//Once we have a copy in the GPU there is no need to keep a CPU copy (unless yopu want to)
		m_indexes.clear();
		m_vertices.clear();
	}

	//Program main logic
	m_angle = 0.0f;
}

void Window::teardownGL() {
	// Actually destroy our OpenGL information
	m_vao.destroy();
	m_vertex_buffer.destroy();
	m_index_buffer.destroy();
	
	delete m_program_ptr;
}

void Window::resizeGL(int width, int height) {
	glViewport(0, 0, width, width);
	m_projection.setToIdentity();
	m_projection.perspective(45.0f, width / float(height), 0.0f, 1000.0f);

	//View
	m_view.setToIdentity();
	QVector3D eye(0.0, 0.0, 1.0);
	QVector3D center(0.0f, 0.0f, 0.0f);
	QVector3D up(0.0f, 1.0f, 0.0f);
	m_view.lookAt(eye, center, up);
}

void Window::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Render using our shader
	m_program_ptr->bind();
	{
		m_model.setToIdentity();
		m_model.scale(0.25f);
		m_model.rotate(m_angle, QVector3D(0.4f, 0.3f, 0.3f));
		
		m_program_ptr->setUniformValue("M", m_model);
		m_program_ptr->setUniformValue("V", m_view);
		m_program_ptr->setUniformValue("P", m_projection);

		m_vao.bind();
		{
			//First element to draw is the first one in the buffer: 0
			//I want to draw all elements: m_index.size() == 3 * numTriangles
			glDrawElements(GL_TRIANGLES, m_index_buffer.size(), GL_UNSIGNED_INT, (const void*)(0 * sizeof(unsigned int)));
		}
		m_vao.release();
	}
	m_program_ptr->release();
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
