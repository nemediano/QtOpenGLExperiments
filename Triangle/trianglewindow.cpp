#include "stdafx.h"
#include "trianglewindow.h"


TriangleWindow::TriangleWindow() :
m_program(nullptr),
m_frame(0), m_mouse_draging(false)
{
}


void TriangleWindow::initialize() {
	
	m_program = new QOpenGLShaderProgram(this);
	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/OpenGLWindow/Resources/vertexShader.glsl")) {
		qDebug() << "Could not compile vertex shader!!";
		qDebug() << m_program->log();
	}
	if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/OpenGLWindow/Resources/fragmentShader.glsl")) {
		qDebug() << "Could not compile fragment shader!!";
		qDebug() << m_program->log();
	}
	if (!m_program->link()) {
		qDebug() << "Could not link the program!!";
		qDebug() << m_program->log();
	}

	m_posAttr = m_program->attributeLocation("posAttr");
	m_colAttr = m_program->attributeLocation("colAttr");
	m_matrixUniform = m_program->uniformLocation("matrix");

	m_fovy_y = 51.42f;
	create_primitive();

	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

	//Camera control
	m_new_rotation = m_base_rotation = QQuaternion(1.0f, QVector3D(0.0f, 0.0f, 0.0f));
}

void TriangleWindow::render() {
	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);
	m_aspect_ratio = (width() * retinaScale) / (height() * retinaScale);

	glEnable(GL_DEPTH_TEST); //This call needs to be here is reseted every frame (I don't now why)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	m_program->bind();

	QMatrix4x4 matrix;
	//Projection
	float zNear = 0.1f;
	float zFar = 100.0f;
	matrix.perspective(m_fovy_y, m_aspect_ratio, zNear, zFar);
	//View
	QVector3D eye(0.0, 0.0, 3.0);
	QVector3D center(0.0f, 0.0f, 0.0f);
	QVector3D up(0.0f, 1.0f, 0.0f);
	matrix.lookAt(eye, center, up);
	//Model
	//Calculate the trackball rotation
	matrix.rotate((m_new_rotation * m_base_rotation));
	matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);
	matrix.rotate(45.0f, QVector3D(1.0, 0.0, 0.0));
	matrix.rotate(-45.0f, QVector3D(0.0, 1.0, 0.0));
	matrix.scale(0.5);

	m_program->setUniformValue(m_matrixUniform, matrix);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	if (m_posAttr != -1) {
		glEnableVertexAttribArray(m_posAttr);
		glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(Vertex, position));
	}
	if (m_colAttr != -1) {
		glEnableVertexAttribArray(m_colAttr);
		glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(Vertex, color));
	}
	//Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

	/* Draw */
	glDrawElements(GL_TRIANGLES, 3 * m_nTriangles, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0 * sizeof(unsigned short)));

	/* Unbind and clean */
	if (m_posAttr != -1) {
		glDisableVertexAttribArray(m_posAttr);
	}
	if (m_colAttr != -1) {
		glDisableVertexAttribArray(m_colAttr);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_program->release();

	++m_frame;
}

void TriangleWindow::mouseMoveEvent(QMouseEvent* event) {
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

void TriangleWindow::mousePressEvent(QMouseEvent* event) {
	if (!m_mouse_draging) {
		qDebug() << "Click!";
	}
	m_mouse_draging = true;
	m_mouse_start_drag = QVector2D(event->pos().x(), event->pos().y());
}

void TriangleWindow::mouseReleaseEvent(QMouseEvent* event) {
	m_mouse_draging = false;
	//Accumulate the rotation
	m_base_rotation = m_new_rotation * m_base_rotation;
	m_base_rotation.normalize();
	//Reset the new rotation to identity
	m_new_rotation = QQuaternion(1.0f, QVector3D(0.0f, 0.0f, 0.0f));
}

void TriangleWindow::wheelEvent(QWheelEvent* event) {
	QPoint numDegrees = event->angleDelta() / 8;
	m_fovy_y += numDegrees.y();
	if (m_fovy_y <= 10.0f) {
		m_fovy_y = 10.0f;
	} else if (m_fovy_y >= 170.0f) {
		m_fovy_y = 170.0f;
	}
	event->accept();
}

void TriangleWindow::create_primitive() {
	
	m_nVertex = 8;
	m_nIndices = 36;
	
	//Regular tetrahedral inscribed in unit circle
	Vertex points[] = {
		{ { -1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f, 1.0f } },
		{ {  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 1.0f } },
		{ {  1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f, 1.0f } },
		{ { -1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f, 1.0f } },
		{ { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f } },
		{ {  1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f } },
		{ {  1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f } },
		{ { -1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f } },
	};

	//Create four triangles using the point indexes
	unsigned short indices[] = {
		0, 1, 2,
		0, 2, 3,
		1, 5, 6,
		1, 6, 2,
		5, 4, 6,
		4, 6, 7,
		4, 0, 3,
		4, 3, 7,
		3, 2, 6,
		3, 6, 7,
		4, 5, 1,
		4, 1, 0,
	};

	m_nTriangles = 12;

	//Create the buffers
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_indexBuffer);

	//Send data to GPU
	//First send the vertices
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_nVertex * sizeof(Vertex), points, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Now, the indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nIndices * sizeof(unsigned short), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


float TriangleWindow::projection_on_curve(const QVector2D& projected) {
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