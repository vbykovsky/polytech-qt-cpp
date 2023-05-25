#ifndef DIALOGS_SORTDIALOG_H
#define DIALOGS_SORTDIALOG_H

#include <QDialog>

namespace Dialogs {

    namespace Ui {
        class SortDialog;
    }

    class SortDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit SortDialog(QWidget *parent = nullptr);
        ~SortDialog();

    public:
        QList<int> keys();
        QList<bool> ascending();
        void setColumns(int fromColumn, int toColumn);

    private slots:
        void on_moreButton_clicked();

    private:
        Ui::SortDialog *ui;
    };

} // namespace Dialogs
#endif // DIALOGS_SORTDIALOG_H
