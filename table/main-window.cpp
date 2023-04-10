#include<QDebug>

#include "main-window.h"
#include "./ui_main-window.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->findDialog = new Dialogs::FindDialog(this);
    this->gotoCellDialog = new Dialogs::GotoCellDialog(this);

    connect(this->findDialog, SIGNAL(search(QString,Qt::MatchFlags,int)), this, SLOT(on_search(QString,Qt::MatchFlags,int)));

    ui->menuOpenRecent->setDisabled(true);
    this->update();
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::update()
{
    this->updateWindowTitle();
    this->updateStatusBarText();
    this->updateRecentFilesMenu();
    this->updateFormulaInputText();
    this->updateSelectedCellInputText();
}


void MainWindow::updateWindowTitle(){
    auto fileName = this->fileName.split("/").takeLast();
    this->setWindowTitle((fileName != "" ? fileName : "Unsaved file") + (ui->tableWidget->isChanged() ? "*" : "") + " - " + this->BASE_TITLE);
}


void MainWindow::updateStatusBarText()
{
    auto selectedItem = ui->tableWidget->selectedItem();

    QString cellContent = "";
    QString cellFormula = "";

    if(selectedItem){
        cellContent = selectedItem->text();
        cellFormula = selectedItem->formula();
    }

    auto rowStr = QString::number(ui->tableWidget->selectedRow() + 1);
    auto columnStr = QString(QChar(QChar('A').toLatin1() + ui->tableWidget->selectedColumn()));

    this->statusBar()->showMessage("Selected " + columnStr + rowStr + (cellContent != "" ? ": " + cellContent : ""));
}


void MainWindow::updateFormulaInputText()
{
    auto selectedItem = ui->tableWidget->selectedItem();

    QString cellFormula = "";

    if(selectedItem){
        cellFormula = selectedItem->formula();
    }

    if(cellFormula != ui->formulaInput->text())
    {
        ui->formulaInput->setText(cellFormula);
    }
}


void MainWindow::updateSelectedCellInputText()
{
    auto rowStr = QString::number(ui->tableWidget->selectedRow() + 1);
    auto columnStr = QString(QChar(QChar('A').toLatin1() + ui->tableWidget->selectedColumn()));

    ui->selectedCellInput->setText(columnStr + rowStr + ":");
}


void MainWindow::updateRecentFilesMenu()
{
    if(!this->recentFiles.size()){
        ui->menuOpenRecent->setDisabled(true);
        return;
    }

    ui->menuOpenRecent->setDisabled(false);
    ui->menuOpenRecent->clear();

    QIcon excelIcon(QPixmap(":/resources/icons/excel.png"));

    for(int i = 0; i < 5 && i < this->recentFiles.size(); i++)
    {
        auto action = new QAction(excelIcon, recentFiles[i].split("/").takeLast());

        connect(action, &QAction::triggered, [=](){ this->on_actionOpenRecent_triggered(i); });

        ui->menuOpenRecent->insertAction(nullptr, action);
        ui->menuOpenRecent->close();
    }
}


bool MainWindow::openFile(QString fileName)
{
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    if(!this->fileName.isEmpty()){
        this->recentFiles.removeAll(this->fileName);
        this->recentFiles.push_front(this->fileName);
    }

    this->fileName = fileName;

    QTextStream fin(&file);

    ui->tableWidget->blockSignals(true);
    ui->tableWidget->clearContents();
    ui->tableWidget->blockSignals(false);

    ui->tableWidget->read(fin);
    ui->tableWidget->setIsChanged(false);

    file.close();

    this->update();

    return true;
}


void MainWindow::on_formulaInput_textChanged(const QString &newText)
{
    ui->tableWidget->onCellTextChanged(ui->tableWidget->selectedRow(), ui->tableWidget->selectedColumn(), newText);

    this->update();
}


void MainWindow::on_formulaInput_returnPressed()
{
    ui->tableWidget->onCellTextChanged(ui->tableWidget->selectedRow(), ui->tableWidget->selectedColumn(), ui->formulaInput->text());

    this->update();
}


void MainWindow::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    ui->tableWidget->onCurrentCellChanged(currentRow, currentColumn, previousRow, previousColumn);

    this->update();
}


void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    ui->tableWidget->onCellDoubleClicked(row, column);
}


bool MainWindow::on_actionSave_triggered()
{
    if(this->fileName == ""){
        this->fileName = QFileDialog::getSaveFileName(this, "Save file", "./", "*.txt");
    }

    if(this->fileName == ""){
        return false;
    }

    QFile file(this->fileName);

    if (!file.open(QIODevice::ReadWrite)) {
        return false;
    }

    QTextStream stream(&file);

    ui->tableWidget->write(stream);

    file.close();

    this->update();

    return true;
}


bool MainWindow::on_actionOpen_triggered()
{
    auto openedFileName = QFileDialog::getOpenFileName(this, "Open file", "./", "*.txt");

    return openFile(openedFileName);
}


void MainWindow::on_actionNew_triggered()
{
    if(ui->tableWidget->isChanged()){
        auto shouldSaveChanges =
                QMessageBox::question(
                    this,
                    "New file",
                    "You are createing new file, but have unsaved changes. Do you want to save it?"
                );

        if(shouldSaveChanges == QMessageBox::Yes){
            if(this->on_actionSave_triggered() == QDialog::Rejected){
                return;
            }
        }
    }

    this->fileName = "";
    ui->tableWidget->setIsChanged(false);

    ui->tableWidget->blockSignals(true);
    ui->tableWidget->clearContents();
    ui->tableWidget->blockSignals(false);

    this->update();
}


void MainWindow::on_actionCut_triggered()
{
    QClipboard *clipboard = QGuiApplication::clipboard();

    auto selectedItem = ui->tableWidget->selectedItem();

    if(selectedItem != nullptr)
    {
        clipboard->setText(selectedItem->text());

        selectedItem->setText("");
        selectedItem->setFormula("");
    }
    else
    {
        clipboard->setText("");
    }

    this->update();
}


void MainWindow::on_actionCopy_triggered()
{
    QClipboard *clipboard = QGuiApplication::clipboard();

    auto selectedItem = ui->tableWidget->selectedItem();

    if(selectedItem != nullptr)
    {
        clipboard->setText(selectedItem->text());
    }
    else
    {
        clipboard->setText("");
    }
}


void MainWindow::on_actionPaste_triggered()
{
    QClipboard *clipboard = QGuiApplication::clipboard();

    auto item = ui->tableWidget->selectedItem();

    if(item != nullptr){
        item->setText(clipboard->text());
    }
    else
    {
        item = new TableCell(clipboard->text());
        ui->tableWidget->setItem(ui->tableWidget->selectedRow(), ui->tableWidget->selectedColumn(), item);
    }

    this->update();
}


void MainWindow::on_actionDelete_triggered()
{
    auto item = ui->tableWidget->selectedItem();

    if(item != nullptr){
        delete item;
    }

    this->update();
}


void MainWindow::on_actionFind_triggered()
{
    this->findDialog->exec();
}


void MainWindow::on_search(QString query, Qt::MatchFlags flags, int index)
{
    ui->tableWidget->onSearch(query, flags, index);
}


void MainWindow::on_actionOpenRecent_triggered(int fileIndex)
{
    openFile(recentFiles[fileIndex]);
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}


void MainWindow::on_actionSelectRow_triggered()
{
    ui->tableWidget->selectRow(ui->tableWidget->selectedRow());
}


void MainWindow::on_actionSelectColumn_triggered()
{
    ui->tableWidget->selectColumn(ui->tableWidget->selectedColumn());
}


void MainWindow::on_actionSelectAll_triggered()
{
    ui->tableWidget->selectAll();
}


void MainWindow::on_actionGotoCell_triggered()
{
    if(this->gotoCellDialog->exec())
    {
        auto cellLocation = this->gotoCellDialog->cellLocation;

        int row = QString(cellLocation).remove(0, 1).toInt() - 1;
        int column = int(cellLocation[0].toUpper().toLatin1()) - int(QChar('A').toLatin1());

        ui->tableWidget->setCurrentCell(row, column);
    }

    this->gotoCellDialog->cellLocation = "";
}

