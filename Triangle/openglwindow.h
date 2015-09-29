#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H


/*
 This class take the place of QOpenGLwidget, but is a window.
 Or if you prefer takes the place of freeglut. (very primitive)
 It just defines, the OpenGL context and an animation feature
 with a simple interface.
 So you use it inheriting form it and then implementing:
 1.- The initialize
 2.- The render
 3.- The resize (Only if you need it)

*/
class OpenGLWindow : public QWindow, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit OpenGLWindow(QWindow *parent = nullptr);
	~OpenGLWindow();

	virtual void render(QPainter * painter);
	virtual void render();

	virtual void initialize();

	void setAnimating(bool animating);

public slots:
	void renderLater();
	void renderNow();

protected: 
	bool event(QEvent* event) Q_DECL_OVERRIDE;
	void exposeEvent(QExposeEvent* event) Q_DECL_OVERRIDE;
	
private:
	bool m_update_pending;
	bool m_animating;

	QOpenGLContext* m_context;
	QOpenGLPaintDevice* m_device;
};

#endif // OPENGLWINDOW_H
