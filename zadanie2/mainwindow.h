#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; class Dialog; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;
};
#endif // MAINWINDOW_H
