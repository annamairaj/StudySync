#ifndef STUDYSESSION_H
#define STUDYSESSION_H

#include <QString>

struct StudySession {
    int id;
    int userId;
    QString date;
    QString subject;
    int duration;
    QString notes;
};

#endif // STUDYSESSION_H
