#include "find-dialog.h"
#include "ui_find-dialog.h"

Dialogs::FindDialog::FindDialog(QWidget *parent) :
    QDialog(parent, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint),
    ui(new Dialogs::Ui::FindDialog)
{
    ui->setupUi(this);

    connect(this, SIGNAL(finished(int)), this, SLOT(on_finished(int)));
}

Dialogs::FindDialog::~FindDialog()
{
    delete ui;
}

void Dialogs::FindDialog::on_findButton_clicked()
{
    auto query = ui->findInput->text();

    if(query == ""){
        return;
    }

    Qt::MatchFlags flags;

    if(ui->matchCaseCheckbox->isChecked()){
        flags |= Qt::MatchFlag::MatchCaseSensitive;
    }

    if(ui->searchBackwardCheckbox->isChecked()){
        this->index -= 1;
    }
    else
    {
        this->index += 1;
    }

    emit search(query, flags, this->index);
}

void Dialogs::FindDialog::on_closeButton_clicked()
{
    this->reject();
}

void Dialogs::FindDialog::on_findInput_textChanged(const QString &arg1)
{
    this->index = 0;
}

void Dialogs::FindDialog::on_finished(int result)
{
    ui->findInput->clear();
    ui->matchCaseCheckbox->setCheckState(Qt::Unchecked);
    ui->searchBackwardCheckbox->setCheckState(Qt::Unchecked);

}
