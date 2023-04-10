#ifndef TABLE_H
#define TABLE_H

#include <QWidget>
#include <QTextStream>
#include <QHeaderView>
#include <QTableWidget>

#include "./table-cell.h"

#define TABLE_CURSOR_IMAGE QPixmap(":/resources/icons/cell-cursor.svg")
#define TABLE_VERTICAL_HEADER_STYLES "::section:checked { color: #217346; background-color: #D2D2D2; border-right: 2px solid #217346; }"
#define TABLE_HORIZONTAL_HEADER_STYLES "::section:checked { color: #217346; background-color: #D2D2D2; border-bottom: 2px solid #217346; }"

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

    inline int selectedRow() const
        { return this->_selectedRow; }

    inline void setSelectedRow(const int &newSelectedRow)
        { this->_selectedRow = newSelectedRow; }

    inline int selectedColumn() const
        { return this->_selectedColumn; }

    inline void setSelectedColumn(const int &newSelectedColumn)
        { this->_selectedColumn = newSelectedColumn; }

public:
    void read(QTextStream& stream);
    void write(QTextStream& stream);

public:
    TableCell* selectedItem();

public:
    void onCellDoubleClicked(int row, int column);
    void onCellTextChanged(int row, int column, QString newText);
    void onCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

public slots:
    void onSearch(QString query, Qt::MatchFlags flags, int index);
};

#endif // TABLE_H
