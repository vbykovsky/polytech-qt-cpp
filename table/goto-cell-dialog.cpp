#include "goto-cell-dialog.h"
#include "ui_goto-cell-dialog.h"

Dialogs::GotoCellDialog::GotoCellDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GotoCellDialog)
{
    ui->setupUi(this);
}


Dialogs::GotoCellDialog::~GotoCellDialog()
{
    delete ui;
}


void Dialogs::GotoCellDialog::on_buttonBox_accepted()
{
    this->cellLocation = this->ui->cellLocationInput->text();
    this->ui->cellLocationInput->clear();
    this->accept();
}


void Dialogs::GotoCellDialog::on_buttonBox_rejected()
{
    this->cellLocation = this->ui->cellLocationInput->text();
    this->ui->cellLocationInput->clear();
    this->reject();
}
