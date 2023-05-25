#include "table.h"


CellsCompare::CellsCompare(QList<int> keys, QList<bool> ascending): keys(keys), ascending(ascending)
{};

bool CellsCompare::operator()(const QStringList &row1, const QStringList &row2) const
{
    for (int i = 0; i < this->keys.size() && i < this->ascending.size(); i++){
        int column = keys[i];

        if (column != -1){
            if (row1[column] != row2[column]){
                if (ascending[i]){
                    return row1[column] < row2[column];
                }else{
                    return row1[column] > row2[column];
                }

            }

        }

    }

    return false;
}


Table::Table(QWidget *parent): QTableWidget(parent)
{
    this->setItemPrototype(new TableCell);

    QCursor cellCursor = QCursor(TABLE_CURSOR_IMAGE, 5, 5);
    this->setCursor(cellCursor);

    this->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    this->verticalHeader()->setStyleSheet(TABLE_VERTICAL_HEADER_STYLES);
    this->horizontalHeader()->setStyleSheet(TABLE_HORIZONTAL_HEADER_STYLES);
}

Table::Table(int rows, int columns, QWidget *parent): QTableWidget(rows, columns, parent)
{ }

TableCell* Table::selectedItem()
{
    return TableCell::cast(this->item(this->selectedRow(), this->selectedColumn()));
}


QList<TableCell*> Table::getItems()
{
    QList<TableCell*> items;

    for(int i = 0; i< this->rowCount(); i++){
        for(int j = 0; j< this->columnCount(); j++){
            auto item = this->item(i, j);

            if(item != nullptr)
            {
                auto tableCell = TableCell::cast(item->clone());

                tableCell->setLastRow(i);
                tableCell->setLastColumn(j);
                items.push_back(tableCell);
            }
        }
    }

    return items;
}


void Table::setItems(QList<TableCell*>& items)
{
    for(auto item: items){
        this->setItem(item->lastRow(), item->lastColumn(), item);
    }
}

QTextStream& operator<<(QTextStream& stream, Table* table)
{
    auto maxRow = 0;
    auto maxColumn = 0;

    for(int i = 0; i< table->rowCount(); i++){
        for(int j = 0; j< table->columnCount(); j++){
            auto item = table->item(i, j);

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

    for(int i = 0; i <= maxRow; i++){
        for(int j = 0; j <= maxColumn; j++){
            auto item = table->item(i, j);

            if(item){
                stream << item->text();
            }

            stream << ",";
        }

        stream << "\n";
    }

    table->setIsChanged(false);

    return stream;
}

QTextStream& operator>>(QTextStream& stream, Table* table)
{
    auto row = 0;
    auto rowCount = table->rowCount();
    auto columnCount = table->columnCount();

    while(!stream.atEnd() && row <= rowCount) {
        auto line = stream.readLine();
        auto fields = line.split(",");

        for(auto j = 0; j < columnCount && j < fields.size(); j++){
            auto cellData = fields[j];

            if(cellData != ""){
                auto item = new TableCell(cellData);
                item->setFormula(cellData);

                table->setItem(row, j, item);
            }
            else {
                auto item = table->item(row, j);

                if(item != nullptr){
                    delete item;
                }
            }

        }

        row++;
    }

    return stream;
}

void Table::onCellTextChanged(int row, int column, QString newText)
{
    auto item = TableCell::cast(this->item(row, column));

    if(item == nullptr){
        item = new TableCell("");
        this->setItem(row, column, item);
    }

    auto cellNewFormula = newText;
    auto cellNewContent = TableCell::evaluateFormula(this, cellNewFormula);

    this->blockSignals(true);

    if(cellNewFormula != item->formula()){
        this->setIsChanged(true);
        item->setFormula(cellNewFormula);
    }

    if(cellNewContent != item->text()){
        this->setIsChanged(true);
        item->setText(cellNewContent);
    }

    this->blockSignals(false);
}

void Table::onCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    this->setSelectedRow(currentRow);
    this->setSelectedColumn(currentColumn);

    auto previousItem = this->item(previousRow, previousColumn);

    if(previousItem != nullptr){
        this->onCellTextChanged(previousRow, previousColumn, previousItem ? previousItem->text() : "");
    }

    for(int i = 0; i< this->rowCount(); i++){
        for(int j = 0; j< this->columnCount(); j++){
            auto item = TableCell::cast(this->item(i, j));

            if(item){
                this->onCellTextChanged(item->row(), item->column(), item->formula());
            }
        }
    }
}

void Table::onCellDoubleClicked(int row, int column)
{
    auto item = TableCell::cast(this->item(row, column));

    if(item == nullptr){
        return;
    }

    this->blockSignals(true);

    item->setText(item->formula());

    this->blockSignals(false);
}

void Table::onSearch(QString query, Qt::MatchFlags flags, int index)
{
    auto results = this->findItems(query, Qt::MatchFlag::MatchContains | flags);

    if(results.size()){
        auto resultIndex = (results.size() + index % results.size()) % results.size();
        auto result = results[resultIndex];

        this->scrollToItem(result);
        this->setCurrentItem(result);
        this->setCurrentCell(result->row(), result->column());
    }
}

void Table::sort(const CellsCompare &compare)
{
    QList<QStringList> rows;
    QTableWidgetSelectionRange range = this->selectedRange();

    for (int i = 0; i < range.rowCount(); ++i) {
        QStringList row;

        for (int j = 0; j < range.columnCount(); ++j) {
            auto item = this->item(range.topRow() + i, range.leftColumn() + j);
            row.append(item != nullptr ? TableCell::cast(item)->formula() : "");
        }

        rows.append(row);
    }

    std::sort(rows.begin(), rows.end(), compare);

    for (int i = 0; i < range.rowCount(); ++i) {
        for (int j = 0; j < range.columnCount(); ++j){
            auto item = this->item(range.topRow() + i, range.leftColumn() + j);

            if(item != nullptr){
                TableCell::cast(item)->setFormula(rows[i][j]);
            }
            else if (rows[i][j] != "")
            {
                item = new TableCell(rows[i][j]);
                this->setItem(range.topRow() + i, range.leftColumn() + j, item);
            }

            this->onCurrentCellChanged(
                        range.topRow() + i,
                        range.leftColumn() + j,
                        i == 0 && j == 0 ? this->selectedRow() : i == 0 ? range.topRow() : range.topRow() + i - 1,
                        i == 0 && j == 0 ? this->selectedColumn() : j == 0 ? range.leftColumn() : range.leftColumn() + j - 1
                );
        }
    }

    clearSelection();
}
