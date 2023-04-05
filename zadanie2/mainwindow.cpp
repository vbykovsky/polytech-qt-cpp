#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
{
}


void MainWindow::on_pushButton_clicked()
{

}

