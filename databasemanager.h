#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

class DatabaseManager
{
public:
    DatabaseManager();

    static bool initialize();
    static QSqlDatabase db();
};

#endif // DATABASEMANAGER_H
