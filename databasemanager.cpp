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


    qDebug() << "Database full path:" << path;

    return true;
}
