#ifndef TABLE_H
#define TABLE_H

#include <algorithm>

#include <QWidget>
#include <QTextStream>
#include <QHeaderView>
#include <QTableWidget>

#include "./table-cell.h"

#define TABLE_CURSOR_IMAGE QPixmap(":/resources/icons/cell-cursor.svg")
#define TABLE_VERTICAL_HEADER_STYLES "::section:checked { color: #217346; background-color: #D2D2D2; border-right: 2px solid #217346; }"
#define TABLE_HORIZONTAL_HEADER_STYLES "::section:checked { color: #217346; background-color: #D2D2D2; border-bottom: 2px solid #217346; }"


class CellsCompare
{
private:
    QList<int> keys;
    QList<bool> ascending;

public:
    CellsCompare(QList<int> keys, QList<bool> ascending);

public:
  bool operator()(const QStringList &row1, const QStringList &row2) const;

};


class Table : public QTableWidget
{
    friend class QTableModel;

private:
    bool _isChanged = false;
    int _selectedRow = 0;
    int _selectedColumn = 0;

public:
    explicit Table(QWidget *parent = nullptr);
    Table(int rows, int columns, QWidget *parent = nullptr);

public:
    inline bool isChanged() const
        { return this->_isChanged; }

    inline void setIsChanged(const bool &newIsChanged)
        { this->_isChanged = newIsChanged; }

    inline QTableWidgetSelectionRange selectedRange() const
        { return this->selectedRanges().size() == 0 ? QTableWidgetSelectionRange() : this->selectedRanges().first(); }

    inline int selectedRow() const
        { return this->_selectedRow; }

    inline void setSelectedRow(const int &newSelectedRow)
        { this->_selectedRow = newSelectedRow; }

    inline int selectedColumn() const
        { return this->_selectedColumn; }

    inline void setSelectedColumn(const int &newSelectedColumn)
        { this->_selectedColumn = newSelectedColumn; }

public:
    friend QTextStream& operator<<(QTextStream& stream, Table* table);
    friend QTextStream& operator>>(QTextStream& stream, Table* table);

public:
    TableCell* selectedItem();
    QList<TableCell*> getItems();
    void setItems(QList<TableCell*>&);

public:
    void onCellDoubleClicked(int row, int column);
    void onCellTextChanged(int row, int column, QString newText);
    void onCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void sort(const CellsCompare &compare);

public slots:
    void onSearch(QString query, Qt::MatchFlags flags, int index);
};

#endif // TABLE_H
