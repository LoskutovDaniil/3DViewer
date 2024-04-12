#ifndef WIDGET_H
#define WIDGET_H

#define GL_SILENCE_DEPRECATION
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QCoreApplication>


#ifdef __cplusplus

extern "C" {
#endif

#include "../s21_viewer.h"

#ifdef __cplusplus
}
#endif

// Структура, в которой хранится информация о вершинах
// (Эта структура заполняется функцией initCube)
struct VertexData {
	VertexData() {}
	VertexData(QVector3D p) : position(p) {}
	QVector3D position;
};

struct RenderingOptions {
	int projectionType;
	int lineType;
	float lineColor[3];
	float lineWidth;
	int vertexDisplay;
    float vertexSize;
	float vertexColor[3];
	float backgroundColor[3];
	QString new_path;
};

class Widget : public QOpenGLWidget {
	Q_OBJECT

       public:
	Widget(QWidget* parent = 0);
	~Widget();
	void get_path(const QString& new_path);
	void updateModel(RenderingOptions& renderingOptions);
	s21_data* getMyData() const { return my_data; }
	void init_3D_Model_Scaled(float scaleFactor);
	void init_3D_Model_Move(double x, double y, double z);
	void init_3D_Model_Rotate(double x, double y, double z);
	QVector3D color;
	QVector3D color_line;
	QVector3D color_vertex;
    bool default_colors;
//    void draw_points(QOpenGLShaderProgram m_program, QOpenGLFunctions *);

	void initializeGL();  // Вызывается один раз при создании окна OGL

       protected:
	void resizeGL(int w, int h);  // Принимает ширину и высоту
	void paintGL();

	void mousePressEvent(QMouseEvent* event);  // Для перемещения мышкой
	void mouseMoveEvent(QMouseEvent* event);   // ДЛя перемещения мышкой

	void initShaders();
	void init_3D_Model();

       private:
	QMatrix4x4 m_projectionMatrix;	// Используется экземпляр класса, а не указатель на него
	QOpenGLShaderProgram m_program;
	QOpenGLBuffer m_arrayBuffer;
	QOpenGLBuffer m_indexBuffer;
	QVector2D m_mousePosition;
	QQuaternion m_rotation;	 // Содержит информацию о векторе, о угле, вокруг которого идет поворт
	s21_data* my_data;
	QString path;
	void cleanupModel();
	RenderingOptions renderingOptions;
};

#endif	// WIDGET_H
