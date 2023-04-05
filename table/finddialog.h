#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QMainWindow>


namespace Find {
    namespace Ui { class FindDialog; }

    class FindDialog : public QDialog
    {
        Q_OBJECT

    private:
        int index = 0;

    public:
        explicit FindDialog(QWidget *parent = nullptr);
        ~FindDialog();

    signals:
        void search(QString query, Qt::MatchFlags flags, int index);

    private slots:
        void on_findButton_clicked();

        void on_closeButton_clicked();

        void on_findInput_textChanged(const QString &arg1);

        void on_finished(int result);

    private:
        Ui::FindDialog *ui;
    };
}

#endif // FINDDIALOG_H
