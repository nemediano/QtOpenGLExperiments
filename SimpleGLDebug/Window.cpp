#include <QDebug>
#include <QString>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QtMath>
#include <QOpenGLDebugMessage>
#include <QOpenGLDebugLogger>

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

Window::Window() : m_debugLogger_ptr(Q_NULLPTR) {

}


Window::~Window() {
	makeCurrent();
	teardownGL();
}

void Window::update() {
	// Schedule a redraw to continue this cycle
	QOpenGLWindow::update();
}
#define GL_DEBUG 1
void Window::initializeGL() {
	// Initialize OpenGL Backend
	initializeOpenGLFunctions();
	//This is equivalent to register an IDLE function callback for animation
	//We don't control the frame rate is as fast as the system is available
	//Maybe using the vsync
	connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));

#ifdef GL_DEBUG
	m_debugLogger_ptr = new QOpenGLDebugLogger(this);
	if (m_debugLogger_ptr->initialize())
	{
		qDebug() << "GL_DEBUG Debug Logger" << m_debugLogger_ptr << "\n";
		connect(m_debugLogger_ptr, SIGNAL(messageLogged(QOpenGLDebugMessage)), this, SLOT(messageLogged(QOpenGLDebugMessage)));
		m_debugLogger_ptr->startLogging();
	}
#endif // GL_DEBUG

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
	//Camera start with no rotation
	m_new_rotation = m_base_rotation = QQuaternion(1.0f, QVector3D(0.0f, 0.0f, 0.0f));
	m_mouse_draging = false;
	m_fovy_y = 45.0f;
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
	m_projection.perspective(m_fovy_y, width / float(height), 0.0f, 1000.0f);
}

void Window::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Render using our shader
	m_program_ptr->bind();
	{
		//Rotate the camera  using the
		//View
		m_view.setToIdentity();
		QVector3D eye(0.0, 0.0, 1.0);
		QVector3D center(0.0f, 0.0f, 0.0f);
		QVector3D up(0.0f, 1.0f, 0.0f);
		m_view.lookAt(eye, center, up);
		m_view.rotate((m_new_rotation * m_base_rotation));
		
		m_model.setToIdentity();
		m_model.scale(0.5f);
				
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

float Window::projection_on_curve(const QVector2D& projected) {
	//This is the distance where the curves changed in terms of the window size
	const float radius = 0.8f;
	float z = 0.0f;
	if (projected.lengthSquared() <= (0.5f * radius * radius)) {
		//Inside the sphere
		z = sqrt(radius * radius - projected.lengthSquared());
	}
	else {
		//Outside of the sphere using hyperbolic sheet
		z = (0.5f * radius * radius) / projected.length();
	}
	return z;
}

void Window::mousePressEvent(QMouseEvent* event) {
	/*if (!m_mouse_draging) {
		qDebug() << "Click!";
	}*/
	m_mouse_draging = true;
	m_mouse_start_drag = QVector2D(event->pos().x(), event->pos().y());
}

void Window::mouseReleaseEvent(QMouseEvent* event) {
	m_mouse_draging = false;
	//Accumulate the rotation
	m_base_rotation = m_new_rotation * m_base_rotation;
	m_base_rotation.normalize();
	//Reset the new rotation to identity
	m_new_rotation = QQuaternion(1.0f, QVector3D(0.0f, 0.0f, 0.0f));
}

void Window::mouseMoveEvent(QMouseEvent* event) {
	if (m_mouse_draging) {
		//Transform mouse coordinates from pixels to world units
		QVector2D mouse_current_in_pixels = QVector2D(event->pos());
		QVector2D window_center = 0.5f * QVector2D(size().width(), size().height());
		QVector2D scale_factors = QVector2D(2.0f / size().width(), -2.0f / size().height());
		QVector2D mouse_current = scale_factors * (mouse_current_in_pixels - window_center);
		QVector2D mouse_start = scale_factors * (m_mouse_start_drag - window_center);
		//Now we have mouse_current and mouse_start in world coordinates
		//Use the algorithm 
		QVector3D v_1 = QVector3D(mouse_current, projection_on_curve(mouse_current));
		QVector3D v_2 = QVector3D(mouse_start, projection_on_curve(mouse_start));
		v_1.normalize();
		v_2.normalize();
		QVector3D axis = QVector3D::crossProduct(v_1, v_2);
		float angle = qAcos(QVector3D::dotProduct(v_1, v_2));
		m_new_rotation = QQuaternion::fromAxisAndAngle(axis, qRadiansToDegrees(angle));
		m_new_rotation.normalize();
	}
}

void Window::keyPressEvent(QKeyEvent* event) {
	switch(event->key()) {
		case Qt::Key_Escape:
			this->close();
		break;
	}
}

void Window::wheelEvent(QWheelEvent* event) {
	QPoint numDegrees = event->angleDelta() / 8;
	m_fovy_y += numDegrees.y();
	if (m_fovy_y <= 10.0f) {
		m_fovy_y = 10.0f;
	}
	else if (m_fovy_y >= 170.0f) {
		m_fovy_y = 170.0f;
	}

	m_projection.setToIdentity();
	m_projection.perspective(m_fovy_y, width() / float(height()), 0.0f, 1000.0f);

	event->accept();
}

void Window::messageLogged(const QOpenGLDebugMessage &msg)
{
	QString error = "Severity:";

	// Format based on severity
	switch (msg.severity())	{
	case QOpenGLDebugMessage::NotificationSeverity:
		error += "Notification";
		break;
	case QOpenGLDebugMessage::HighSeverity:
		error += "High";
		break;
	case QOpenGLDebugMessage::MediumSeverity:
		error += "Medium";
		break;
	case QOpenGLDebugMessage::LowSeverity:
		error += "Low";
		break;
	}

	error += " Source:";

	// Format based on source

	switch (msg.source()) {
		
		case QOpenGLDebugMessage::APISource:
			error += "API";
		break;

		case QOpenGLDebugMessage::WindowSystemSource:
			error += "Window system";
		break;

		case QOpenGLDebugMessage::ShaderCompilerSource:
			error += "Shader compiler";
		break;
		
		case QOpenGLDebugMessage::ThirdPartySource:
			error += "Third party";
		break;

		case QOpenGLDebugMessage::ApplicationSource:
			error += "Application";
		break;
		
		case QOpenGLDebugMessage::OtherSource:
			error += "Other";
		break;

		case QOpenGLDebugMessage::InvalidSource:
			error += "Invalid source";
		break;
	}

	error += " : Type:";

	// Format based on type
	switch (msg.type()) {
		case QOpenGLDebugMessage::ErrorType:
			error += "Error";
		break;

		case QOpenGLDebugMessage::DeprecatedBehaviorType:
			error += "Deprecated behavior";
		break;

		case QOpenGLDebugMessage::UndefinedBehaviorType:
			error += "Undefinded behavior";
		break;
		
		case QOpenGLDebugMessage::PortabilityType:
			error += "Portabillity";
		break;
		
		case QOpenGLDebugMessage::PerformanceType:
			error += "Performance";
		break;
		
		case QOpenGLDebugMessage::OtherType:
			error += "Other";
		break;

		case QOpenGLDebugMessage::MarkerType:
			error += "Marker";
		break;

		case QOpenGLDebugMessage::GroupPushType:
			error += "Group push";
		break;

		case QOpenGLDebugMessage::GroupPopType:
			error += "Group pop";
		break;
	}

	error += " type";

	qDebug().noquote() << error;
	qDebug().noquote() << msg.message();
}