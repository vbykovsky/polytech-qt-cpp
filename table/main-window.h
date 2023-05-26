#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMap>
#include <QList>
#include <QFile>
#include <QSettings>
#include <QClipboard>
#include <QTextStream>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QCloseEvent>
#include <QRadioButton>
#include <QTableWidgetItem>
#include <QRegularExpression>

#include "./table-cell.h"
#include "./sort-dialog.h"
#include "./find-dialog.h"
#include "./goto-cell-dialog.h"

#define EXCEL_ICON QIcon(QPixmap(":/resources/icons/excel.png"))

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
    Dialogs::SortDialog* sortDialog = nullptr;
    Dialogs::GotoCellDialog* gotoCellDialog = nullptr;

private:
    QString sheet = "";
    QMap<QString, QList<TableCell*>> sheets = { };

    QString fileName = "";
    QStringList recentFiles = { };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent *bar);

private:
    void update();
    void updateWindowTitle();
    void updateStatusBarText();
    void updateRecentFilesMenu();
    void updateFormulaInputText();
    void updateSelectedCellInputText();

private:
    bool openFile(QString fileName);
    void saveSettings();
    void uploadSettings();
    void createSheet();
    void changeSheet(QString);

private slots:
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

    void on_actionSort_triggered();

    void on_addSheetButton_clicked();

    void on_actionClose_triggered();

    void on_actionShowGrid_toggled(bool arg1);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
