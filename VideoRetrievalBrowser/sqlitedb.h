#ifndef SQLITEDB_H
#define SQLITEDB_H

#include <QString>
#include <QSqlDatabase>

struct imgstruct{
    QString filename;
    QString concept;
    double concept_confidence;
    double h;
    double s;
    double v;
};

class SqliteDb
{
public:
    SqliteDb(QString path);

    std::vector<imgstruct> hsvSearch(QColor color, double tol);
    std::vector<imgstruct> cnnSearch(QString synsetId);

private:
    QString path;
    QSqlDatabase m_db;
    std::vector<imgstruct> images;
    int rc;
};

#endif // SQLITEDB_H
