#include <QSurfaceFormat>
#include <QGuiApplication>
#include "Window.h"

int main(int argc, char *argv[]) {
	QGuiApplication app(argc, argv);
	
	// Set OpenGL Version information
	// Note: This format must be set before show() is called.
	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CoreProfile);
	//If no format is specified it uses the highest available possible
	//In my case 4.5
	//format.setVersion(3, 3);

	Window window;
	window.setTitle("Qt and OpenGL Simple Window");
	window.setFormat(format);
	window.resize(QSize(800, 600));
	window.show();

	return app.exec();
}
