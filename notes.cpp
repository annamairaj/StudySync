#include "notes.h"
#include "ui_notes.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <QDir>

Notes::Notes(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Notes)
{
    ui->setupUi(this);

    //connect save button to function
    connect(ui->saveNoteButton, &QPushButton::clicked, this, &Notes::saveNote);

}

Notes::~Notes()
{
    delete ui;
}

void Notes::saveNote()
{
    QString title = ui->lineEdit->text().trimmed();
    QString body = ui->textEdit->toPlainText().trimmed();

    if (title.isEmpty() || body.isEmpty()) {
        QMessageBox::warning(this, "Missing Information", "Please fill in both the title and the note.");
        return;
    }

    // Sanitize title
    title.remove(QRegExp("[\\/:*?\"<>|]"));

    // Ensure notes directory exists
    QDir dir("notes");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString fileName = title + ".txt";
    QString filePath = "notes/" + fileName;

    // Optional: ask before overwriting
    if (QFile::exists(filePath)) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "File Exists",
                                      "A note with this title already exists. Overwrite?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Save Error", "Could not save the note.");
        return;
    }

    QTextStream out(&file);
    out << body;
    file.close();

    QMessageBox::information(this, "Success", "Note saved successfully!");

    ui->lineEdit->clear();
    ui->textEdit->clear();
    ui->lineEdit->setFocus();
}
