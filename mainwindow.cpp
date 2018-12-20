#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QString>
#include <QPixmap>
#include <QDebug>
#include <QPushButton>
#include <QEventLoop>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Select Image","","Bmp Files (*.bmp)");
        ui->lineEdit->setText(fileName);
        QPixmap im(fileName);
        ui->label->setPixmap(im);

}
