#include "mainwindow.h"
#include "ui_mainwindow.h"

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

void MainWindow::on_searchButton_clicked()
{

}

void MainWindow::on_colorHistButton_clicked()
{

}

void MainWindow::on_edgeHistButton_clicked()
{

}

void MainWindow::on_keypointsButton_clicked()
{

}
