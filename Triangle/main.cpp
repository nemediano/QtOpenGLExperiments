#include "stdafx.h"
#include "trianglewindow.h"

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

	QSurfaceFormat format;
	format.setSamples(16);

	TriangleWindow window;
	window.setTitle("OpenGL trackball cube");
	window.setFormat(format);
	window.resize(512, 512);
	window.show();
	window.setAnimating(true);

	return app.exec();
}
