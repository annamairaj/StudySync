#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include <QStandardPaths>
#include <QDir>

DatabaseManager::DatabaseManager() {}

QSqlDatabase DatabaseManager::db() {
    return QSqlDatabase::database();
}

bool DatabaseManager::initialize() {


    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/app.db";

    // Ensure directory exists
    QDir().mkpath(QFileInfo(path).absolutePath());

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);


    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError();
        return false;
    }

    QSqlQuery query;
    query.exec(R"(
    CREATE TABLE IF NOT EXISTS users (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        username TEXT UNIQUE NOT NULL,
        email TEXT UNIQUE NOT NULL,
        password_hash TEXT NOT NULL
    )
)");

    query.exec(R"(
    CREATE TABLE IF NOT EXISTS study_sessions (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        user_id INTEGER,
        date TEXT,
        subject TEXT,
        duration INTEGER,
        notes TEXT,
        FOREIGN KEY (user_id) REFERENCES users(id)
    )
)");

    qDebug() << "Database full path:" << path;

    return true;
}

bool DatabaseManager::addStudySession(int userId, const QString &date, const QString &subject, int duration, const QString &notes)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO study_sessions (user_id, date, subject, duration, notes)
        VALUES (?, ?, ?, ?, ?)
    )");
    query.addBindValue(userId);
    query.addBindValue(date);
    query.addBindValue(subject);
    query.addBindValue(duration);
    query.addBindValue(notes);

    if (!query.exec()) {
        qDebug() << "Failed to insert study session:" << query.lastError();
        return false;
    }

    return true;
}
