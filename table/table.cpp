#include "table.h"

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

void Table::read(QTextStream& stream)
{
    auto row = 0;
    auto rowCount = this->rowCount();
    auto columnCount = this->columnCount();

    while(!stream.atEnd() && row <= rowCount) {
        auto line = stream.readLine();
        auto fields = line.split(",");

        for(auto j = 0; j < columnCount && j < fields.size(); j++){
            auto cellData = fields[j];

            if(cellData != ""){
                auto item = new TableCell(cellData);
                item->setFormula(cellData);

                this->setItem(row, j, item);
            }
            else {
                auto item = this->item(row, j);

                if(item != nullptr){
                    delete item;
                }
            }

        }

        row++;
    }
}

void Table::write(QTextStream& stream)
{
    auto maxRow = 0;
    auto maxColumn = 0;

    for(int i = 0; i< this->rowCount(); i++){
        for(int j = 0; j< this->columnCount(); j++){
            auto item = this->item(i, j);

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
            auto item = this->item(i, j);

            if(item){
                stream << item->text();
            }

            stream << ",";
        }

        stream << "\n";
    }

    this->setIsChanged(false);
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
