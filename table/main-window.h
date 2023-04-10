#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMap>
#include <QList>
#include <QFile>
#include <QClipboard>
#include <QTextStream>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QRegularExpression>

#include "./table-cell.h"
#include "./find-dialog.h"
#include "./goto-cell-dialog.h"

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
    Dialogs::GotoCellDialog* gotoCellDialog = nullptr;

private:
    QString fileName = "";
    QStringList recentFiles = { };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void update();
    void updateWindowTitle();
    void updateStatusBarText();
    void updateRecentFilesMenu();
    void updateFormulaInputText();
    void updateSelectedCellInputText();

private:
    bool openFile(QString fileName);

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

    void on_actionOpenRecent_triggered(int fileIndex);

    void on_actionExit_triggered();

    void on_actionSelectRow_triggered();

    void on_actionSelectColumn_triggered();

    void on_actionSelectAll_triggered();

    void on_actionGotoCell_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
