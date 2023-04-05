#include "finddialog.h"
#include "ui_finddialog.h"

Find::FindDialog::FindDialog(QWidget *parent) :
    QDialog(parent, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint),
    ui(new Find::Ui::FindDialog)
{
    ui->setupUi(this);

    connect(this, SIGNAL(finished(int)), this, SLOT(on_finished(int)));
}

Find::FindDialog::~FindDialog()
{
    delete ui;
}

void Find::FindDialog::on_findButton_clicked()
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

void Find::FindDialog::on_closeButton_clicked()
{
    this->reject();
}

void Find::FindDialog::on_findInput_textChanged(const QString &arg1)
{
    this->index = 0;
}

void Find::FindDialog::on_finished(int result)
{
    ui->findInput->clear();
    ui->matchCaseCheckbox->setCheckState(Qt::Unchecked);
    ui->searchBackwardCheckbox->setCheckState(Qt::Unchecked);

}
