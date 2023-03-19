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

void MainWindow::on_addButton_clicked()
{
    auto name = ui->nameInput->text();
    auto formula = ui->formulaInput->text();
    auto value = ui->valueInput->text();

    if(name.isEmpty() || formula.isEmpty() || value.isEmpty()){
        return;
    }

    qInfo("Name: %s", name.toUtf8().data());
    qInfo("Formula: %s", formula.toUtf8().data());
    qInfo("Value: %s", value.toUtf8().data());

    ui->nameInput->clear();
    ui->formulaInput->clear();
    ui->valueInput->clear();
}

