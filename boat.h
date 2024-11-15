#ifndef BOAT_H
#define BOAT_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Boat {
private:
    int boat_id;
    QString owner;
    QString model;

public:
    Boat();
    Boat(int boat_id, QString owner, QString model);

    bool addBoat();
    bool remove(int boat_id);
    bool update(int boat_id);
    QSqlQueryModel* display();
};

#endif // BOAT_H
