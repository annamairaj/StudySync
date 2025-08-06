#include "addcarddialog.h"
#include "ui_addcarddialog.h"
#include <QMessageBox>

AddCardDialog::AddCardDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddCardDialog)
{
    ui->setupUi(this);
}

AddCardDialog::~AddCardDialog()
{
    delete ui;
}

QString AddCardDialog::getQuestion() const {
    return ui->questionTextEdit->toPlainText();
}

QString AddCardDialog::getAnswer() const {
    return ui->answerTextEdit->toPlainText();
}

void AddCardDialog::on_createCardButton_clicked()
{
    if (getQuestion().isEmpty() || getAnswer().isEmpty()) {
        QMessageBox::warning(this, "Empty Field", "Please fill out both the question and the answer.");
        return;
    }
    accept();  // Signal success
}

void AddCardDialog::on_cancelCardButton_clicked()
{
    reject();  // Close without saving
}
