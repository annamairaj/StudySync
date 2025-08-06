#ifndef ADDCARDDIALOG_H
#define ADDCARDDIALOG_H

#include <QDialog>

namespace Ui {
class AddCardDialog;
}

class AddCardDialog : public QDialog
{
    Q_OBJECT
    friend class StudySyncTests;

public:
    explicit AddCardDialog(QWidget *parent = nullptr);
    ~AddCardDialog();

    QString getQuestion() const;
    QString getAnswer() const;


private slots:
    void on_createCardButton_clicked();
    void on_cancelCardButton_clicked();

private:
    Ui::AddCardDialog *ui;
};

#endif // ADDCARDDIALOG_H
