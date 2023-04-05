#include<QDebug>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->findDialog = new Find::FindDialog(this);

    QCursor cellCursor1 = QCursor(QPixmap(":/resources/icons/cell-cursor.svg"), 5, 5);
    ui->tableWidget->setCursor(cellCursor1);
    ui->tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableWidget->verticalHeader()->setStyleSheet("::section:checked { color: #217346; background-color: #D2D2D2; border-right: 2px solid #217346; }");
    ui->tableWidget->horizontalHeader()->setStyleSheet("::section:checked { color: #217346; background-color: #D2D2D2; border-bottom: 2px solid #217346; }");

    connect(this->findDialog, SIGNAL(search(QString,Qt::MatchFlags,int)), this, SLOT(on_search(QString,Qt::MatchFlags,int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

QTableWidgetItem* MainWindow::getSelectedItem()
{
    return ui->tableWidget->item(this->selectedRow, this->selectedColumn);
}
\

void MainWindow::updateStatusBarText()
{
    auto selectedItem = this->getSelectedItem();

    QString cellContent = "";
    QString cellFormula = "";

    if(selectedItem){
        cellContent = selectedItem->text();
        cellFormula = selectedItem->toolTip();
    }

    auto rowStr = QString::number(this->selectedRow + 1);
    auto columnStr = QString(QChar(QChar('A').toLatin1() + this->selectedColumn));

    this->statusBar()->showMessage("Selected " + columnStr + rowStr + (cellContent != "" ? ": " + cellContent : ""));
}

void MainWindow::updateFormulaInputText()
{
    auto selectedItem = this->getSelectedItem();

    QString cellFormula = "";

    if(selectedItem){
        cellFormula = selectedItem->toolTip();
    }

    if(cellFormula != ui->formulaInput->text())
    {
        ui->formulaInput->setText(cellFormula);
    }
}

void MainWindow::updateSelectedCellInputText()
{
    auto rowStr = QString::number(this->selectedRow + 1);
    auto columnStr = QString(QChar(QChar('A').toLatin1() + this->selectedColumn));

    ui->selectedCellInput->setText(columnStr + rowStr + ":");
}

QString MainWindow::handleFormula(QString mayBeFormula)
{
    if(!mayBeFormula.size() || mayBeFormula[0] != '='){
        return mayBeFormula;
    }

    auto formula = mayBeFormula.remove(0, 1);

    if(formula == "")
    {
        return "";
    }

    QRegularExpression regExp("[A-Za-z][1-9][0-9]{0,2}");

    for (const QRegularExpressionMatch &match : regExp.globalMatch(formula)) {
        auto cellPosition = match.captured(0);

        int row = QString(cellPosition).remove(0, 1).toInt() - 1;
        int column = int(cellPosition[0].toUpper().toLatin1()) - int(QChar('A').toLatin1());

        auto item = ui->tableWidget->item(row, column);

        if(item != nullptr)
        {
            formula.replace(cellPosition, item->text());
        }
        else
        {
            formula.replace(cellPosition, "0");
        }

    }

    QJSEngine myEngine;
    QJSValue value = myEngine.evaluate(formula);

    if(value.isError() || value.isNull() || value.isUndefined()){
        return "#####";
    }

    return value.toString();
}

void MainWindow::handleCellTextChanged(int row, int column, QString newText)
{
    auto item = ui->tableWidget->item(row, column);

    if(item == nullptr){
        item = new QTableWidgetItem("");
        ui->tableWidget->setItem(row, column, item);
    }

    auto cellContent = newText;
    auto cellFormula = item->toolTip();

    if(handleFormula(cellFormula) == cellContent){
        return;
    }

    this->hasChanged = true;

    ui->tableWidget->blockSignals(true);

    item->setToolTip(cellContent);
    item->setText(handleFormula(item->toolTip()));

    ui->tableWidget->blockSignals(false);

    this->updateStatusBarText();
    this->updateFormulaInputText();
    this->updateSelectedCellInputText();
}

void MainWindow::on_formulaInput_textChanged(const QString &newText)
{
    this->handleCellTextChanged(this->selectedRow, this->selectedColumn, newText);
}


void MainWindow::on_formulaInput_returnPressed()
{
    this->handleCellTextChanged(this->selectedRow, this->selectedColumn, ui->formulaInput->text());
}

void MainWindow::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    this->selectedRow = currentRow;
    this->selectedColumn = currentColumn;

    auto previousItem = ui->tableWidget->item(previousRow, previousColumn);
    if(previousItem != nullptr){
        this->handleCellTextChanged(previousRow, previousColumn, previousItem ? previousItem->text() : "");
    }

    for(int i = 0; i< ui->tableWidget->rowCount(); i++){
        for(int j = 0; j< ui->tableWidget->columnCount(); j++){
            auto item = ui->tableWidget->item(i, j);

            if(item){
                this->handleCellTextChanged(item->row(), item->column(), item->toolTip());
            }
        }
    }

    this->updateStatusBarText();
    this->updateFormulaInputText();
    this->updateSelectedCellInputText();
}

void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    auto item = ui->tableWidget->item(row, column);

    if(item == nullptr){
        return;
    }

    ui->tableWidget->blockSignals(true);

    item->setText(item->toolTip());

    ui->tableWidget->blockSignals(false);
}

QDialog::DialogCode MainWindow::on_actionSave_triggered()
{
    if(this->fileName == ""){
        this->fileName = QFileDialog::getSaveFileName(this, "Save file", "./", "*.txt");
    }

    if(this->fileName == ""){
        return QDialog::Rejected;
    }

    QFile file(this->fileName);

    auto maxRow = 0;
    auto maxColumn = 0;

    for(int i = 0; i< ui->tableWidget->rowCount(); i++){
        for(int j = 0; j< ui->tableWidget->columnCount(); j++){
            auto item = ui->tableWidget->item(i, j);

            if(!item)
            {
                continue;
            }

            if(item->row() >= maxRow)
            {
                maxRow = item->row();
            }

            if(item->column() >= maxColumn)
            {
                maxColumn = item->column();
            }
        }
    }

    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);

        for(int i = 0; i <= maxRow; i++){
            for(int j = 0; j <= maxColumn; j++){
                auto item = ui->tableWidget->item(i, j);

                if(item){
                    stream << item->text();
                }

                stream << ",";
            }

            stream << "\n";
        }

        file.close();

        this->hasChanged = true;
    }

    return QDialog::Accepted;
}


QDialog::DialogCode MainWindow::on_actionOpen_triggered()
{
    auto openedFileName = QFileDialog::getOpenFileName(this, "Open file", "./", "*.txt");

    QFile file(openedFileName);
    if(!file.open(QIODevice::ReadOnly)) {
        return QDialog::Rejected;
    }

    this->fileName = openedFileName;

    QTextStream fin(&file);

    int row = 0;
    int rowCount = ui->tableWidget->rowCount();
    int columnCount = ui->tableWidget->columnCount();

    while(!fin.atEnd() && row <= rowCount) {
        QString line = fin.readLine();
        QStringList fields = line.split(",");

        for(int j = 0; j < columnCount && j < fields.size(); j++){
            auto cellData = fields[j];

            if(cellData != ""){
                auto item = new QTableWidgetItem(cellData);
                item->setToolTip(cellData);

                ui->tableWidget->setItem(row, j, item);
            }
            else {
                auto item = ui->tableWidget->item(row, j);

                if(item != nullptr){
                    delete item;
                }
            }

        }

        row++;
    }

    file.close();

    return QDialog::Accepted;
}


void MainWindow::on_actionNew_triggered()
{
    if(this->hasChanged){
        auto shouldSaveChanges =
                QMessageBox::question(
                    this,
                    "New file",
                    "You are createing new file, but have unsaved changes. Do you want to save it?"
                );

        if(shouldSaveChanges == QMessageBox::Yes){
            if(this->fileName != ""){
                if(this->on_actionSave_triggered() == QDialog::Rejected){
                    return;
                }
            }
        }
    }

    this->fileName = "";
    this->hasChanged = false;

    ui->tableWidget->blockSignals(true);
    ui->tableWidget->clearContents();
    ui->tableWidget->blockSignals(false);
}


void MainWindow::on_actionCut_triggered()
{
    QClipboard *clipboard = QGuiApplication::clipboard();

    auto selectedItem = this->getSelectedItem();

    if(selectedItem != nullptr)
    {
        clipboard->setText(selectedItem->text());

        selectedItem->setText("");
        selectedItem->setToolTip("");
    }
    else
    {
        clipboard->setText("");
    }

    this->updateStatusBarText();
    this->updateFormulaInputText();
    this->updateSelectedCellInputText();
}


void MainWindow::on_actionCopy_triggered()
{
    QClipboard *clipboard = QGuiApplication::clipboard();

    auto selectedItem = this->getSelectedItem();

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

    auto item = this->getSelectedItem();

    if(item != nullptr){
        item->setText(clipboard->text());
    }
    else
    {
        item = new QTableWidgetItem(clipboard->text());
        ui->tableWidget->setItem(this->selectedRow, this->selectedColumn, item);
    }

    this->updateStatusBarText();
    this->updateFormulaInputText();
    this->updateSelectedCellInputText();
}


void MainWindow::on_actionDelete_triggered()
{
    auto item = this->getSelectedItem();

    if(item != nullptr){
        delete item;
    }

    this->updateStatusBarText();
    this->updateFormulaInputText();
    this->updateSelectedCellInputText();
}

void MainWindow::on_actionFind_triggered()
{
    this->findDialog->exec();
}

void MainWindow::on_search(QString query, Qt::MatchFlags flags, int index)
{
    auto results = ui->tableWidget->findItems(query, Qt::MatchFlag::MatchContains | flags);

    if(results.size()){
        auto resultIndex = (results.size() + index % results.size()) % results.size();
        auto result = results[resultIndex];

        ui->tableWidget->scrollToItem(result);
        ui->tableWidget->setCurrentItem(result);
        ui->tableWidget->setCurrentCell(result->row(), result->column());
    }
}
