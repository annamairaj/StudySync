#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

class DatabaseManager
{
public:
    DatabaseManager();

    static bool initialize();
    static QSqlDatabase db();
    bool saveOrUpdateProgress(int userId, const QString &subject, int progress, const QString &notes, const QString &date);
    QMap<QString, QMap<QString, QString>> loadNotesBySubjectAndDate(int userId);
    QStringList loadMasteredSubjects(int userId);
};

#endif // DATABASEMANAGER_H
