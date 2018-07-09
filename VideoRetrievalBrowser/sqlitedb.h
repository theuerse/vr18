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
    bool operator < (const imgstruct& str) const {
        return (concept_confidence < str.concept_confidence);
    }
    bool operator > (const imgstruct& str) const {
        return (concept_confidence > str.concept_confidence);
    }
};

class SqliteDb
{
public:
    SqliteDb(QString path);

    std::vector<imgstruct> hsvSearch(QColor color, double tol);
    std::vector<imgstruct> cnnSearch(QString synsetId);
    std::vector<imgstruct> nameSearch(QString videoId);

private:
    QString path;
    QSqlDatabase m_db;
    std::vector<imgstruct> images;
    int rc;
};

#endif // SQLITEDB_H
