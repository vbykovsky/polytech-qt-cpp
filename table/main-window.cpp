#include<QDebug>

#include "main-window.h"
#include "./ui_main-window.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->findDialog = new Dialogs::FindDialog(this);
    this->sortDialog = new Dialogs::SortDialog(this);
    this->gotoCellDialog = new Dialogs::GotoCellDialog(this);

    connect(this->findDialog, SIGNAL(search(QString,Qt::MatchFlags,int)), this, SLOT(on_search(QString,Qt::MatchFlags,int)));

    this->createSheet();

    ui->menuOpenRecent->setDisabled(true);
    this->uploadSettings();
    this->update();
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent (QCloseEvent *event)
{
    if(ui->tableWidget->isChanged()){
        auto shouldSaveChanges =
                QMessageBox::question(
                    this,
                    "New file",
                    "You have unsaved changes. Do you want to save it?"
                );

        if(shouldSaveChanges == QMessageBox::Yes){
            if(this->on_actionSave_triggered() == QDialog::Rejected){
                return;
            }
        }
    }

    this->saveSettings();

    event->accept();
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

    for(int i = 0; i < 5 && i < this->recentFiles.size(); i++)
    {
        auto action = new QAction(EXCEL_ICON, recentFiles[i].split("/").takeLast());

        connect(action, &QAction::triggered, [=](){ this->on_actionOpenRecent_triggered(i); });

        ui->menuOpenRecent->insertAction(nullptr, action);
        ui->menuOpenRecent->close();
    }
}


void MainWindow::saveSettings()
{
    QSettings settings("vbykvosky", "Excel");

    settings.setValue("geometry", this->geometry());
    settings.setValue("recentFiles", this->recentFiles);
    settings.setValue("showGrid", ui->actionShowGrid->isChecked());
    settings.setValue("autoRecalculate", ui->actionAutoRecalculate->isChecked());
}


void MainWindow::uploadSettings()
{
    QSettings settings("vbykvosky", "Excel");

    QRect rect = settings.value("geometry", QRect(200, 200, 400, 400)).toRect();
    move(rect.topLeft());
    resize(rect.size());

    recentFiles = settings.value("recentFiles").toStringList();

    bool showGrid = settings.value("showGrid", true).toBool();
    ui->actionShowGrid->setChecked(showGrid);

    bool autoRecalculate = settings.value("autoRecalculate", true).toBool();
    ui->actionAutoRecalculate->setChecked(autoRecalculate);
}


void MainWindow::createSheet()
{
    this->sheets[this->sheet] = ui->tableWidget->getItems();
    ui->tableWidget->clearContents();

    this->sheet = "Sheet " + QString::number(ui->sheetsDock->children().length() - 1);
    this->sheets[this->sheet] = {};

    auto newSheetsRadioButton = new QRadioButton(this->sheet);

    ui->sheetsDock->layout()->removeItem(ui->sheetsDockSpacer1);
    ui->sheetsDock->layout()->removeWidget(ui->addSheetButton);
    ui->sheetsDock->layout()->removeItem(ui->sheetsDockSpacer2);

    ui->sheetsDock->layout()->addWidget(newSheetsRadioButton);

    ui->sheetsDock->layout()->addItem(ui->sheetsDockSpacer1);
    ui->sheetsDock->layout()->addWidget(ui->addSheetButton);
    ui->sheetsDock->layout()->addItem(ui->sheetsDockSpacer2);

    connect(newSheetsRadioButton, &QRadioButton::clicked, this, [newSheetsRadioButton, this](bool checked) {
        this->sheets[this->sheet] = ui->tableWidget->getItems();
        this->changeSheet(newSheetsRadioButton->text());
    });

    newSheetsRadioButton->click();
}


void MainWindow::changeSheet(QString sheet)
{
    if(!this->sheets.contains(sheet))
    {
        this->sheets[sheet] = {};
    }

    this->sheet = sheet;

    ui->tableWidget->clearContents();
    ui->tableWidget->setItems(this->sheets[sheet]);

    ui->tableWidget->setSelectedRow(0);
    ui->tableWidget->setSelectedColumn(0);

    this->update();
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

    fin >> ui->tableWidget;
    ui->tableWidget->setIsChanged(false);

    file.close();

    this->update();

    return true;
}


void MainWindow::on_formulaInput_textChanged(const QString &newText)
{
    auto item = TableCell::cast(ui->tableWidget->item(ui->tableWidget->selectedRow(), ui->tableWidget->selectedColumn()));

    if(item == nullptr){
        item = new TableCell("");
        ui->tableWidget->setItem(ui->tableWidget->selectedRow(), ui->tableWidget->selectedColumn(), item);
    }

    item->setFormula(newText);
    item->setText(newText);

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
        this->fileName = QFileDialog::getSaveFileName(this, "Save file", "./", "CSV(*.csv);;Spredsheet(*.sp)");
    }

    if(this->fileName == ""){
        return false;
    }

    QFile file(this->fileName);

    if (!file.open(QIODevice::ReadWrite)) {
        return false;
    }

    QTextStream stream(&file);

    stream << ui->tableWidget;

    file.close();

    this->update();

    return true;
}


bool MainWindow::on_actionOpen_triggered()
{
    auto openedFileName = QFileDialog::getOpenFileName(this, "Open file", "./", "CSV(*.csv);;Spredsheet(*.sp)");

    return openFile(openedFileName);
}


void MainWindow::on_actionNew_triggered()
{
    /*
    if(ui->tableWidget->isChanged()){
        auto shouldSaveChanges =
                QMessageBox::question(
                    this,
                    "New file",
                    "You are creating new file, but have unsaved changes. Do you want to save it?"
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
    */

    MainWindow* childWindow = new MainWindow();
    childWindow->show();
}


void MainWindow::on_actionCut_triggered()
{
    this->on_actionCopy_triggered();
    this->on_actionDelete_triggered();

    this->update();
}


void MainWindow::on_actionCopy_triggered()
{
    QClipboard *clipboard = QGuiApplication::clipboard();

    auto selectedRange = ui->tableWidget->selectedRange();

    QString text = "";

    for(int i = selectedRange.topRow(); i <= selectedRange.bottomRow(); i++)
    {
        for(int j = selectedRange.leftColumn(); j <= selectedRange.rightColumn(); j++)
        {
            auto item = ui->tableWidget->item(i, j);

            if(item != nullptr)
            {
                text += item->text() + (j == selectedRange.rightColumn() ? "" : "\t");
            }
            else
            {
                text += "\t";
            }
        }

        text += "\n";
    }

    clipboard->setText(text);

}


void MainWindow::on_actionPaste_triggered()
{
    QClipboard *clipboard = QGuiApplication::clipboard();

    QList<QList<QString>> clipboardData;

    auto rows = clipboard->text().split("\n");

    for(int i = 0; i < rows.size() - 1; i++)
    {
        auto cellsTexts = rows[i].split("\t");
        clipboardData.push_back({});

        for(int j = 0; j < cellsTexts.size(); j++)
        {
            clipboardData[i].push_back(cellsTexts[j]);
        }
    }

    if(clipboardData.size() == 0)
    {
        return;
    }

    auto selectedRow = ui->tableWidget->selectedRow();
    auto selectedColumn = ui->tableWidget->selectedColumn();

    for(int i = 0; i < clipboardData.size(); i++)
    {
        for(int j = 0; j < clipboardData[0].size(); j++)
        {
            auto item = ui->tableWidget->item(selectedRow + i, selectedColumn + j);
            auto tableCell = TableCell::cast(item);

            if(item != nullptr){
                if(clipboardData[i][j] != "")
                {
                    tableCell->setText(clipboardData[i][j]);
                    tableCell->setFormula(clipboardData[i][j]);
                    ui->tableWidget->onCurrentCellChanged(selectedRow + i, selectedColumn + j, i == 0 ? selectedRow : selectedRow + i - 1, j == 0 ? selectedColumn : selectedColumn + j - 1);
                }
                else
                {
                    delete item;
                }
            }
            else
            {
                if(clipboardData[i][j] != "")
                {
                    tableCell = new TableCell(clipboardData[i][j], clipboardData[i][j]);
                    ui->tableWidget->setItem(selectedRow + i, selectedColumn + j, tableCell);
                    ui->tableWidget->onCurrentCellChanged(selectedRow + i, selectedColumn + j, i == 0 ? selectedRow : selectedRow + i - 1, j == 0 ? selectedColumn : selectedColumn + j - 1);
                }
            }

        }
    }

    this->update();
}


void MainWindow::on_actionDelete_triggered()
{
    auto selectedRange = ui->tableWidget->selectedRange();

    auto selectedRow = ui->tableWidget->selectedRow();
    auto selectedColumn = ui->tableWidget->selectedColumn();

    for(int i = selectedRange.topRow(); i <= selectedRange.bottomRow(); i++)
    {
        for(int j = selectedRange.leftColumn(); j <= selectedRange.rightColumn(); j++)
        {
            auto item = ui->tableWidget->item(i, j);

            if(item != nullptr)
            {
                delete item;
                ui->tableWidget->onCurrentCellChanged(i, j, i == 0 ? selectedRow : i - 1, j == 0 ? selectedColumn : j - 1);
            }
        }
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
    qApp->closeAllWindows();
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


void MainWindow::on_actionSort_triggered()
{
    this->sortDialog->setColumns(ui->tableWidget->selectedRange().leftColumn(), ui->tableWidget->selectedRange().rightColumn());

    if(this->sortDialog->exec()){
        CellsCompare compare(this->sortDialog->keys(), this->sortDialog->ascending());
        ui->tableWidget->sort(compare);

        this->update();
    }
}


void MainWindow::on_addSheetButton_clicked()
{
   this->createSheet();
}

void MainWindow::on_actionClose_triggered()
{
    this->close();
}

