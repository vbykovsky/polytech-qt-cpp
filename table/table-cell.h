#ifndef TABLECELL_H
#define TABLECELL_H

#include <QString>
#include <QJSEngine>
#include <QTableWidget>
#include <QTableWidgetItem>

class TableCell : public QTableWidgetItem
{
    friend class QTableWidget;
    friend class QTableModel;

private:
    QString _formula = "";
    int _lastRow = -1;
    int _lastColumn = -1;

public:
    static TableCell* cast(QTableWidgetItem* item);
    static QString evaluateFormula(QTableWidget* table, QString mayBeFormula);

public:
    TableCell();
    TableCell(QString content);
    TableCell(QString content, QString formula);
    TableCell(const TableCell &other);

public:
    virtual TableCell *clone() const;

    inline QString formula() const
        { return this->_formula; }

    inline void setFormula(const QString &newFormula)
        { this->_formula = newFormula; }

    inline int lastRow() const
        { return this->_lastRow; }

    inline void setLastRow(const int &newLastRow)
        { this->_lastRow = newLastRow; }

    inline int lastColumn() const
        { return this->_lastColumn; }

    inline void setLastColumn(const int &newLastColumn)
        { this->_lastColumn = newLastColumn; }
};

#endif // TABLECELL_H
