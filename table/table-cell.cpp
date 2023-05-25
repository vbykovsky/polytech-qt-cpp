#include "table-cell.h"

TableCell::TableCell()
{ }

TableCell::TableCell(QString content): QTableWidgetItem(content)
{ }

TableCell::TableCell(QString content, QString formula): QTableWidgetItem(content), _formula(formula)
{ }

TableCell* TableCell::cast(QTableWidgetItem* item)
{
    return static_cast<TableCell*>(item);
}

QString TableCell::evaluateFormula(QTableWidget* table, QString mayBeFormula)
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

        auto item = table->item(row, column);

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

TableCell::TableCell(const TableCell &other): QTableWidgetItem(other.text())
{
    this->setFormula(other.formula());
}

TableCell* TableCell::clone() const
{
    return new TableCell(*this);
}
