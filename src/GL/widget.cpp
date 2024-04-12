#include "widget.h"

#include <QMouseEvent>

Widget::Widget(QWidget* parent) : QOpenGLWidget(parent), m_indexBuffer(QOpenGLBuffer::IndexBuffer) {}

Widget::~Widget() {}

void Widget::initializeGL() {
	glClearColor(0.1f, 0.1f, 0.1f, 0.8f);  // rgb формат; 4й пункт - прозрачность
	glEnable(GL_DEPTH_TEST);  // Включает буфер глубины для отображения Z-координаты
	glEnable(GL_CULL_FACE);	  // Скрывает стенки
    glCullFace(GL_BACK);	  // Скрывает задние стенки
	initShaders();
	init_3D_Model();
}

void Widget::resizeGL(int w, int h)  // вызывается у при изменение размера окна
{
	float aspect = w / (h ? (float)h : 1);	
	m_projectionMatrix.setToIdentity();  // аспект projectionMatrix делается единичным (матрица, на главной диагонали стоят единиы,
                         // остальные равны нулю)
    m_projectionMatrix.perspective(45, aspect, 0.1, 10.0f);	 // с помощью perspective формируется необходимая матрица для отображения
}

void Widget::paintGL() {
    if (default_colors) {
        glClearColor(0.44, 0.53, 0.49, 0.8f);
    } else {
        glClearColor(color[0] / 255, color[1] / 255, color[2] / 255, 1.0f);
    }

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 modelViewMatrix;
    modelViewMatrix.setToIdentity();
    modelViewMatrix.translate(0.0, 0.0, -2.4);
	modelViewMatrix.rotate(m_rotation);

	m_program.bind();  

	m_program.setUniformValue("qt_ModelViewProjectionMatrix", m_projectionMatrix * modelViewMatrix);
	m_program.setUniformValue("qt_Texture0", 0);  

	m_arrayBuffer.bind();

	int offset = 0;

	// указываю шейдерам атрибуты
    int vertLoc = m_program.attributeLocation("qt_Vertex");
    m_program.enableAttributeArray(vertLoc);  // включаем использование этого атрибута
    m_program.setAttributeBuffer(vertLoc, GL_FLOAT, offset, 3, sizeof(VertexData));

	m_indexBuffer.bind();

    /*---------------------------Отрисовка-ЛИНИЙ---------------------------*/

    QVector4D colorValue(this->renderingOptions.lineColor[0] / 255, this->renderingOptions.lineColor[1] / 255, this->renderingOptions.lineColor[2] / 255, 1.0f);
    int colorLoc = m_program.uniformLocation("color");
    m_program.setUniformValue(colorLoc, colorValue);

	glLineWidth(this->renderingOptions.lineWidth);

	if (this->renderingOptions.lineType) {

		glDisable(GL_LINE_STIPPLE);
		glDrawElements(GL_LINES, m_indexBuffer.size() / 4, GL_UNSIGNED_INT, 0);
	} else {

        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0x00ff);
        glDrawElements(GL_LINES, m_indexBuffer.size() / 4, GL_UNSIGNED_INT, 0);
	}

    /*------------------------Отрисовка-по-Веришинам------------------------*/
    QVector4D vertex_color(this->renderingOptions.vertexColor[0] / 255, this->renderingOptions.vertexColor[1] / 255, this->renderingOptions.vertexColor[2] / 255, 1.0f);
    int v_color = m_program.uniformLocation("color");
    m_program.setUniformValue(v_color, vertex_color);

    if (this->renderingOptions.vertexDisplay == 0) {
        // Вершины не рисуются
    } else {
        if (this->renderingOptions.vertexDisplay == 2) {
            glPointSize(this->renderingOptions.vertexSize);
            glDisable(GL_POINT_SMOOTH);
            glDrawElements(GL_POINTS, m_indexBuffer.size() / 4, GL_UNSIGNED_INT, 0);
        } else if (this->renderingOptions.vertexDisplay == 1) {
            glPointSize(this->renderingOptions.vertexSize);
            glEnable(GL_POINT_SMOOTH);
            glDrawElements(GL_POINTS, m_indexBuffer.size() / 4, GL_UNSIGNED_INT, 0);
        }
    }
}



void Widget::mousePressEvent(QMouseEvent* event) {

	if (event->buttons() == Qt::LeftButton) {

		m_mousePosition = QVector2D(event->localPos());
	}

	event->accept();
}

void Widget::mouseMoveEvent(QMouseEvent* event) {
	if (event->buttons() != Qt::LeftButton) return;
	QVector2D diff = QVector2D(event->localPos()) - m_mousePosition;
	m_mousePosition = QVector2D(event->localPos());
    float angle = diff.length() / 2.0;
	QVector3D axis = QVector3D(diff.y(), diff.x(), 0.0);

	m_rotation = QQuaternion::fromAxisAndAngle(axis, angle) * m_rotation;
	// Вызываем update, чтобы перерисовать сцену с учетом нового вращения
	update();
}

void Widget::initShaders() {
	if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.vsh")) close();
	if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.fsh")) close();
	if (!m_program.link()) close();

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		qDebug() << "OpenGL error:" << error;
	}
}


void Widget::init_3D_Model_Move(double x, double y, double z) {

	// Умножаем my_data на scaleFactor
	s21_move_matrix(my_data, x, y, z);

	QVector<VertexData> vertexes_array;  

	for (int r = 0; r < my_data->count_of_vertexes; r++) {
		vertexes_array.append(VertexData(
		    QVector3D(my_data->matrix_3d->matrix[r][0], my_data->matrix_3d->matrix[r][1], my_data->matrix_3d->matrix[r][2])));
	}

	m_arrayBuffer.create();
	m_arrayBuffer.bind();
	m_arrayBuffer.allocate(vertexes_array.constData(), vertexes_array.size() * sizeof(VertexData));
	m_arrayBuffer.release();
}

void Widget::init_3D_Model_Rotate(double x, double y, double z) {
	// вращение матрицы по осям координат

	s21_rotate_matrix(my_data, x, 1, 0, 0);

	s21_rotate_matrix(my_data, y, 0, 1, 0);

    s21_rotate_matrix(my_data, z, 0, 0, 1);

	QVector<VertexData> vertexes_array;  

	for (int r = 0; r < my_data->count_of_vertexes; r++) {
		vertexes_array.append(VertexData(
		    QVector3D(my_data->matrix_3d->matrix[r][0], my_data->matrix_3d->matrix[r][1], my_data->matrix_3d->matrix[r][2])));
	}

	m_arrayBuffer.create();
	m_arrayBuffer.bind();
	m_arrayBuffer.allocate(vertexes_array.constData(), vertexes_array.size() * sizeof(VertexData));
	m_arrayBuffer.release();
	qInfo() << "x y z =" << x << y << z;
}

void Widget::init_3D_Model() {
	my_data = (s21_data*)calloc(1, sizeof(s21_data));

	parser(path.toStdString().c_str(), my_data);

	QVector<VertexData> vertexes_array;  
	for (int r = 0; r < my_data->count_of_vertexes; r++) {
		vertexes_array.append(VertexData(
		    QVector3D(my_data->matrix_3d->matrix[r][0], my_data->matrix_3d->matrix[r][1], my_data->matrix_3d->matrix[r][2])));
	}

	QVector<GLuint> indexes_array;
	for (unsigned int i = 0; i < my_data->count_of_facets; i++) {

		indexes_array.append(my_data->polygons[i]->vertexes[0]);
		indexes_array.append(my_data->polygons[i]->vertexes[1]);

		indexes_array.append(my_data->polygons[i]->vertexes[1]);
		indexes_array.append(my_data->polygons[i]->vertexes[2]);

		indexes_array.append(my_data->polygons[i]->vertexes[2]);
		indexes_array.append(my_data->polygons[i]->vertexes[0]);
	}
	m_arrayBuffer.create();
	m_arrayBuffer.bind();
	m_arrayBuffer.allocate(vertexes_array.constData(), vertexes_array.size() * sizeof(VertexData));
	m_arrayBuffer.release();

	m_indexBuffer.create();
	m_indexBuffer.bind();
	m_indexBuffer.allocate(indexes_array.constData(), indexes_array.size() * sizeof(GLuint));
	m_indexBuffer.release();
}

void Widget::init_3D_Model_Scaled(float scaleFactor) {

	s21_scaling_matrix(my_data, scaleFactor);

	QVector<VertexData> vertexes_array;  

	for (int r = 0; r < my_data->count_of_vertexes; r++) {
		vertexes_array.append(VertexData(
		    QVector3D(my_data->matrix_3d->matrix[r][0], my_data->matrix_3d->matrix[r][1], my_data->matrix_3d->matrix[r][2])));
	}

	m_arrayBuffer.create();
	m_arrayBuffer.bind();
	m_arrayBuffer.allocate(vertexes_array.constData(), vertexes_array.size() * sizeof(VertexData));
	m_arrayBuffer.release();
}

void Widget::get_path(const QString& new_path) { path = new_path.toStdString().c_str(); }

void Widget::updateModel(RenderingOptions& renderingOptions) {
	s21_simple_clear_memory(my_data);
	cleanupModel();
	get_path(renderingOptions.new_path);
	float aspect = width() / (height() ? static_cast<float>(height()) : 1.0f);

	if (renderingOptions.projectionType == 1) {
		m_projectionMatrix.setToIdentity();
		m_projectionMatrix.perspective(90, aspect, 0.1, 10.0f);
	} else {
		m_projectionMatrix.setToIdentity();
		m_projectionMatrix.ortho(-1.0f * aspect, 1.0f * aspect, -1.0f, 1.0f, 0.1f, 10.0f);
	}

	this->renderingOptions = renderingOptions;  

	init_3D_Model();
	update();

}

void Widget::cleanupModel() {
	m_arrayBuffer.destroy();
	m_indexBuffer.destroy();
}

