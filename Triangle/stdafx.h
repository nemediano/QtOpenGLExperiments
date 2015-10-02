/*
I can include all QtGui and this automatically 
includes everything I need. 
However, I try to take advantage of precompiled headers 
and try to include only what I need and have all at once*/
//#include <QtGui>
#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>

#include <QtGui/QScreen>
#include <QtGui/QMatrix4x4>
#include <QtGui/QMatrix3x3>
#include <QtGui/QVector3D>
#include <QtGui/QVector2D>
#include <QtGui/QQuaternion>

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>

#include <QtGui/QMouseEvent>

#include <QtCore/QtMath>
#include <cmath>