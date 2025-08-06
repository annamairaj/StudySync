#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>

DatabaseManager::DatabaseManager() {}

QSqlDatabase DatabaseManager::db() {
    return QSqlDatabase::database();
}

bool DatabaseManager::initialize() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/app.db";

    QDir().mkpath(QFileInfo(path).absolutePath());

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError();
        return false;
    }

    QSqlQuery query;

    // Create tables
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            email TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL
        )
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS cards (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            question TEXT NOT NULL,
            answer TEXT NOT NULL,
            FOREIGN KEY(user_id) REFERENCES users(id)
        )
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS progress (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            subject TEXT NOT NULL,
            progress INTEGER NOT NULL,
            notes TEXT,
            date TEXT,
            FOREIGN KEY(user_id) REFERENCES users(id)
        )
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS certificates (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            subject TEXT NOT NULL,
            date_generated TEXT NOT NULL,
            FOREIGN KEY(user_id) REFERENCES users(id)
        )
    )");

    qDebug() << "Database full path:" << path;
    return true;
}

// Define these OUTSIDE initialize() - not nested

bool DatabaseManager::saveOrUpdateProgress(int userId, const QString &subject, int progress, const QString &notes, const QString &date) {
    QSqlQuery query;

    query.prepare("SELECT id FROM progress WHERE user_id = ? AND subject = ?");
    query.addBindValue(userId);
    query.addBindValue(subject);
    if (!query.exec()) {
        qDebug() << "Failed to check existing progress:" << query.lastError();
        return false;
    }

    if (query.next()) {
        int progressId = query.value(0).toInt();
        query.prepare("UPDATE progress SET progress = ?, notes = ?, date = ? WHERE id = ?");
        query.addBindValue(progress);
        query.addBindValue(notes);
        query.addBindValue(date);
        query.addBindValue(progressId);
        if (!query.exec()) {
            qDebug() << "Failed to update progress:" << query.lastError();
            return false;
        }
    } else {
        query.prepare("INSERT INTO progress (user_id, subject, progress, notes, date) VALUES (?, ?, ?, ?, ?)");
        query.addBindValue(userId);
        query.addBindValue(subject);
        query.addBindValue(progress);
        query.addBindValue(notes);
        query.addBindValue(date);
        if (!query.exec()) {
            qDebug() << "Failed to insert progress:" << query.lastError();
            return false;
        }
    }

    return true;
}

QMap<QString, QMap<QString, QString>> DatabaseManager::loadNotesBySubjectAndDate(int userId) {
    QMap<QString, QMap<QString, QString>> notesMap;

    QSqlQuery query;
    query.prepare("SELECT subject, date, notes FROM progress WHERE user_id = ? ORDER BY subject, date");
    query.addBindValue(userId);
    if (!query.exec()) {
        qDebug() << "Failed to load notes:" << query.lastError();
        return notesMap;
    }

    while (query.next()) {
        QString subject = query.value(0).toString();
        QString date = query.value(1).toString();
        QString notes = query.value(2).toString();
        notesMap[subject][date] = notes;
    }

    return notesMap;
}

QStringList DatabaseManager::loadMasteredSubjects(int userId) {
    QStringList masteredSubjects;

    QSqlQuery query;
    query.prepare("SELECT DISTINCT subject FROM progress WHERE user_id = ? AND progress >= 100");
    query.addBindValue(userId);
    if (!query.exec()) {
        qDebug() << "Failed to load mastered subjects:" << query.lastError();
        return masteredSubjects;
    }

    while (query.next()) {
        masteredSubjects.append(query.value(0).toString());
    }

    return masteredSubjects;
}
