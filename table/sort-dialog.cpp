#include "sort-dialog.h"
#include "ui_sort-dialog.h"

Dialogs::SortDialog::SortDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SortDialog)
{
    ui->setupUi(this);
    ui->secondKeyGroup->setVisible(false);
    ui->thirdKeyGroup->setVisible(false);
}

Dialogs::SortDialog::~SortDialog()
{
    delete ui;
}


void Dialogs::SortDialog::on_moreButton_clicked()
{
    ui->secondKeyGroup->setVisible(!ui->secondKeyGroup->isVisible());
    ui->thirdKeyGroup->setVisible(!ui->thirdKeyGroup->isVisible());

    if(ui->secondKeyGroup->isVisible() && ui->thirdKeyGroup->isVisible())
    {
        ui->moreButton->setText("Less");
    }
    else if (!ui->secondKeyGroup->isVisible() && !ui->thirdKeyGroup->isVisible())
    {
        ui->moreButton->setText("More");
    }
}


QList<int> Dialogs::SortDialog::keys()
{
    QList<int> keys;

    keys.push_back(ui->firstKeyGroupColumnSelect->currentIndex() - 1);
    keys.push_back(ui->secondKeyGroupColumnSelect->currentIndex() - 1);
    keys.push_back(ui->thirdKeyGroupColumnSelect->currentIndex() - 1);

    return keys;
}


QList<bool> Dialogs::SortDialog::ascending()
{
    QList<bool> keys;

    keys.push_back(ui->firstKeyGroupSortSelect->currentIndex() == 0);
    keys.push_back(ui->secondKeyGroupSortSelect->currentIndex() == 0);
    keys.push_back(ui->thirdKeyGroupSortSelect->currentIndex() == 0);

    return keys;

}


void Dialogs::SortDialog::setColumns(int fromColumn, int toColumn)
{
    QStringList columns = { "Empty" };

    ui->firstKeyGroupColumnSelect->clear();
    ui->secondKeyGroupColumnSelect->clear();
    ui->thirdKeyGroupColumnSelect->clear();

    for(int i = fromColumn; i <= toColumn; i++){
        auto columnName = QString(QChar(QChar('A').toLatin1() + i));
        columns.push_back(columnName);
    }

    ui->firstKeyGroupColumnSelect->addItems(columns);
    ui->secondKeyGroupColumnSelect->addItems(columns);
    ui->thirdKeyGroupColumnSelect->addItems(columns);

    ui->firstKeyGroupColumnSelect->setCurrentIndex(1);
}
