#include "Window.h"
#include <QSurfaceFormat>
#include <QGuiApplication>

#define GL_DEBUG 1

int main(int argc, char *argv[]) {
	QGuiApplication app(argc, argv);

	// Set OpenGL Version information
	// Note: This format must be set before show() is called.
	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setSamples(8);
	//If no format is specified it uses the highest available possible
	//In my case 4.5
	//format.setVersion(3, 3);

	//Try to create a debug context in OpenGL
#ifdef    GL_DEBUG
	format.setOption(QSurfaceFormat::DebugContext);
#endif // GL_DEBUG

	Window window;
	window.setTitle("Testing the Debug log");
	window.setFormat(format);
	window.resize(QSize(800, 600));
	window.show();

	return app.exec();
}