#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <../GL/widget.h>

#include <QColorDialog>
#include <QFileDialog>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QString>
#include <QCoreApplication>
#include <QFileInfo>

#include "../gifLib/src/gifimage/qgifimage.h"
#include <QTimer>

#ifdef __cplusplus

extern "C" {
#endif

#include "../s21_viewer.h"

#ifdef __cplusplus
}
#endif

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

       public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();


       private slots:
	void on_buttn_Exit_clicked();

	void on_Input_file_name_textChanged();

	void on_buttn_Start_clicked();

	void on_choose_file_clicked();

	void on_accept_clicked();

	void on_horizontalSlider_Scale_sliderMoved(int position);

	void on_horizontalSlider_LineSize_valueChanged(int value);

	void on_horizontalSlider_VertexSize_valueChanged(int value);

	void on_pushButton_background_color_clicked();

	void on_accept_move_clicked();

	void on_accepr_rotate_clicked();

	void on_Line_color_clicked();

	void on_vertex_color_clicked();

    void on_horizontalSlider_VertexSize_sliderMoved(int position);

    void on_horizontalSlider_LineSize_sliderMoved();
    void on_pushButton_screenshot_clicked();

    void on_pushButton_gif_clicked();

    void recordSlot();

    void on_buttn_Exit_2_clicked();

private:
	QString file_name;
	Ui::MainWindow *ui;
	Widget *modelWidget;
    RenderingOptions renderingOptions;
	QOpenGLShaderProgram shaderProgram;
	float redValue;
	float greenValue;
	float blueValue;

    QTimer* timerGif;
    QGifImage*  gifka;
    QString gifFilePath;
    s21_data *str;

};

#endif	// MAINWINDOW_H
