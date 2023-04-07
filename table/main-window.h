#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMap>
#include <QList>
#include <QFile>
#include <QJSEngine>
#include <QClipboard>
#include <QTextStream>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QRegularExpression>

#include "./find-dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    const QString BASE_TITLE = "Excel";

private:
    Dialogs::FindDialog* findDialog = nullptr;

private:
    int selectedRow = 0;
    int selectedColumn = 0;

private:
    QString fileName = "";

private:
    int hasChanged = false;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QTableWidgetItem* getSelectedItem();

private:
    void updateWindowTitle();
    void updateStatusBarText();
    void updateFormulaInputText();
    void updateSelectedCellInputText();

private:
    QString handleFormula(QString formula);
    void handleCellTextChanged(int row, int column, QString newText);

private slots:
    void on_formulaInput_returnPressed();

    void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void on_tableWidget_cellDoubleClicked(int row, int column);

    bool on_actionSave_triggered();

    bool on_actionOpen_triggered();

    void on_actionNew_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionDelete_triggered();

    void on_actionFind_triggered();

    void on_search(QString query, Qt::MatchFlags flags, int index);

    void on_formulaInput_textChanged(const QString &arg1);

    void on_actionCut_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
