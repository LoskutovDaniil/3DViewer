#include "mainwindow.h"

#include "../GL/widget.h"
#include "../s21_viewer.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)

{
	ui->setupUi(this);

	modelWidget = new Widget();
	ui->gridLayout->addWidget(modelWidget);

	shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":../fshader.fsh");
	shaderProgram.link();
    timerGif = new QTimer(this);

    modelWidget->default_colors = true;
    qInfo() << "default_colors" << modelWidget->default_colors;
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_buttn_Exit_clicked() { close(); }

void MainWindow::on_choose_file_clicked() {
    file_name = QFileDialog::getOpenFileName(this, tr("Choose 3D model"), "/Users", tr("Object Files (*.obj)"));
    ui->Input_file_name->setText(file_name);
}

void MainWindow::on_Input_file_name_textChanged() { file_name = ui->Input_file_name->text(); }

void MainWindow::on_buttn_Start_clicked() {

    /*----- Установка имени файлов -----*/

    QString fullFilePath = file_name;

    QFileInfo fileInfo(fullFilePath);

    QString fileName = fileInfo.fileName();

    ui->output_file_name->setText(fileName);

    // вызов основной функции обработки файла с бэкенда
    s21_data *modelInfo = (s21_data *)calloc(1, sizeof(s21_data));

    if (file_name.isEmpty()) {
        QString projectPath = QCoreApplication::applicationDirPath();
        file_name = projectPath + "/../../../../src/skull.obj";
    }

    // toUtf8() переводит формат из QString в char, функция data создает указатель на переведенную строку
    parser(file_name.toUtf8().data(), modelInfo);

    //  блок отпринтовки ошибок, ошибки забираем из структуры
    if (modelInfo->error.code != 0) {
        ui->Output_ERROR->setText(modelInfo->error.message);
        ui->vertex_number->setText(" ");
        ui->poligon_number->setText(" ");
    } else {
        ui->Output_ERROR->setText(" ");

        if (modelInfo->count_of_vertexes > 0) ui->vertex_number->setText(QString::number(modelInfo->count_of_vertexes));

        if (modelInfo->count_of_facets > 0) ui->poligon_number->setText(QString::number(modelInfo->count_of_facets));
    }

    s21_simple_clear_memory(modelInfo);
}

/*---------------------------------------------Линии-и-Вершины---------------------------------------------*/

void MainWindow::on_horizontalSlider_LineSize_valueChanged(int value) {
	double newValue = static_cast<double>(value);  
	ui->doubleSpinBox_LineSize->setValue(newValue);
}

void MainWindow::on_horizontalSlider_VertexSize_valueChanged(int value) {
	double newValue = static_cast<double>(value); 
	ui->doubleSpinBox_VertexSize->setValue(newValue);
    
}


void MainWindow::on_horizontalSlider_LineSize_sliderMoved() {
    renderingOptions.lineWidth = ui->doubleSpinBox_LineSize->value();
    modelWidget->updateModel(renderingOptions);
}


void MainWindow::on_horizontalSlider_VertexSize_sliderMoved(int position) {
    renderingOptions.vertexSize = float(position);
    modelWidget->updateModel(renderingOptions);
}


/*-------------------------------------Передвижение/Вращение-модельки-------------------------------------*/
void MainWindow::on_horizontalSlider_Scale_sliderMoved(int position) {
    ui->scale_result->setText(QString::number((double)position));

    double scale = 1.0; 
    if (position > 0) {
        scale = 1.0 + position * 0.01;	
    } else if (position < 0) {
        scale = 1.0 / (1.0 - position * 0.01);
    }

    modelWidget->init_3D_Model_Scaled(scale);
    modelWidget->update();
}


void MainWindow::on_accept_move_clicked() {
	modelWidget->init_3D_Model_Move(ui->doubleSpinBox_Move_X->value(), ui->doubleSpinBox_Move_Y->value(),
					ui->doubleSpinBox_Move_Z->value());
	modelWidget->update();
}


void MainWindow::on_accepr_rotate_clicked() {
	modelWidget->init_3D_Model_Rotate(ui->doubleSpinBox_Rotate_X->value(), ui->doubleSpinBox_Rotate_Y->value(),
					  ui->doubleSpinBox_Rotate_Z->value());
	modelWidget->update();
}

/*-------------------------------------Настройки--Цвета-------------------------------------*/

void MainWindow::on_pushButton_background_color_clicked() {
    QColor color_ui = QColorDialog::getColor(Qt::white, this, "Выберите цвет");

    if (color_ui.isValid()) {
        modelWidget->color[0] = color_ui.red();
        modelWidget->color[1] = color_ui.green();
        modelWidget->color[2] = color_ui.blue();

        modelWidget->default_colors = false;
    }
}

void MainWindow::on_Line_color_clicked() {
	QColor color_ui = QColorDialog::getColor(Qt::white, this, "Выберите цвет");

	if (color_ui.isValid()) {
		modelWidget->color_line[0] = color_ui.red();
		modelWidget->color_line[1] = color_ui.green();
		modelWidget->color_line[2] = color_ui.blue();

        modelWidget->default_colors = false;
	}
}


void MainWindow::on_vertex_color_clicked() {
	QColor color_ui = QColorDialog::getColor(Qt::white, this, "Выберите цвет");

	if (color_ui.isValid()) {
		modelWidget->color_vertex[0] = color_ui.red();
		modelWidget->color_vertex[1] = color_ui.green();
		modelWidget->color_vertex[2] = color_ui.blue();

        modelWidget->default_colors = false;
	}
}



/*-------------------------------------Применить-все-настройки-------------------------------------*/

void MainWindow::on_accept_clicked() {

    renderingOptions.projectionType = ui->projection_type->isChecked() ? 1 : 0;  // Если да - Central, нет - Parallel
    renderingOptions.lineType = ui->checkBox_lin_type->isChecked() ? 1 : 0;
    renderingOptions.lineWidth = ui->doubleSpinBox_LineSize->value();  


    if (ui->radioButton_VertexNone->isChecked())
        renderingOptions.vertexDisplay = 0;
    else if (ui->radioButton_VertexCircle->isChecked())
        renderingOptions.vertexDisplay = 1;
    else
        renderingOptions.vertexDisplay = 2;


    renderingOptions.lineColor[0] = modelWidget->color_line[0];  
    renderingOptions.lineColor[1] = modelWidget->color_line[1];  
    renderingOptions.lineColor[2] = modelWidget->color_line[2];  

    renderingOptions.vertexColor[0] = modelWidget->color_vertex[0];	 
    renderingOptions.vertexColor[1] = modelWidget->color_vertex[1];	 
    renderingOptions.vertexColor[2] = modelWidget->color_vertex[2];	 


    renderingOptions.new_path = file_name;


    modelWidget->updateModel(renderingOptions);
}

/*-------------------------------------Скриншот и Гифка-------------------------------------*/

void MainWindow::on_pushButton_screenshot_clicked()
{
    QPixmap screenshot = modelWidget->grab();


        QString filter = "JPEG (*.jpg);;BMP (*.bmp)";
        QString selectedFilter;
        QString fileName = QFileDialog::getSaveFileName(this, "Save screenshot", "", filter, &selectedFilter);

        if (!fileName.isEmpty()) {
            if (selectedFilter == "BMP (*.bmp)") {
                screenshot.save(fileName, "BMP");
            } else if (selectedFilter == "JPEG (*.jpg)") {
                screenshot.save(fileName, "JPEG", 90);
            }
        }
}


void MainWindow::recordSlot(){

    QImage modelImage = modelWidget->grabFramebuffer();
    gifka->addFrame(modelImage);
    if(gifka->frameCount() == 50){
        timerGif->stop();
        gifka->setDefaultDelay(10);
        gifka->save(gifFilePath);
        delete gifka;
        gifka = NULL;
    }
}


void MainWindow::on_pushButton_gif_clicked()
{
    gifka = new QGifImage(QSize(640, 480));
    gifFilePath = QFileDialog::getSaveFileName(this, "Specify where to save a file", "/", tr("GIF files (*.gif)"), 0, QFileDialog::DontUseNativeDialog);
    timerGif->start(100);

    connect(timerGif, SIGNAL(timeout()), this, SLOT(recordSlot()));
}

void MainWindow::on_buttn_Exit_2_clicked()
{
    close();
}

