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

public:
    static TableCell* cast(QTableWidgetItem* item);
    static QString evaluateFormula(QTableWidget* table, QString mayBeFormula);

public:
    TableCell();
    TableCell(QString content);
    TableCell(const TableCell &other);

public:
    virtual TableCell *clone() const;

    inline QString formula() const
        { return this->_formula; }

    inline void setFormula(const QString &newFormula)
        { this->_formula = newFormula; }
};

#endif // TABLECELL_H
