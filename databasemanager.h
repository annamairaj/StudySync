
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>

class DatabaseManager
{
public:
    DatabaseManager();

    static bool initialize();
    static QSqlDatabase db();
    static bool addStudySession(int userId, const QString &date, const QString &subject, int duration, const QString &notes);

};

#endif // DATABASEMANAGER_H
