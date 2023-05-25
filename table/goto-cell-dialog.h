#ifndef DIALOGS_GOTOCELLDIALOG_H
#define DIALOGS_GOTOCELLDIALOG_H

#include <QDialog>

namespace Dialogs {

    namespace Ui {
    class GotoCellDialog;
    }

    class GotoCellDialog : public QDialog
    {
        Q_OBJECT

    public:
        QString cellLocation = "";

    public:
        explicit GotoCellDialog(QWidget *parent = nullptr);
        ~GotoCellDialog();

    private slots:
        void on_buttonBox_accepted();

        void on_buttonBox_rejected();

    private:
        Ui::GotoCellDialog *ui;
    };

}

#endif // DIALOGS_GOTOCELLDIALOG_H
