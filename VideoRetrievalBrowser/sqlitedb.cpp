#include <iostream>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QColor>


#include "sqlitedb.h"


SqliteDb::SqliteDb(QString dbPath)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open())
    {
        std::cout << "Error: connection with database fail" << std::endl;
    }
    else
    {
        std::cout << "DataBase: connection ok." << std::endl;
    }
}

std::vector<imgstruct> SqliteDb::hsvSearch(QColor color, double tol){
    images.clear();

    /* Create SQL statement */
    double lower_h = color.hue() * (1-tol);
    double upper_h = color.hue() * (1+tol);
    double lower_s = ((color.saturation() / 255.0) * 100) * (1-tol);
    double upper_s = ((color.saturation() / 255.0) * 100) * (1+tol);
    double lower_v = ((color.value() / 255.0) * 100) * (1-tol);
    double upper_v = ((color.value() / 255.0) * 100) * (1+tol);

    std::cout << "H:" << color.hue()
              << "\tS:" << ((color.saturation() / 255.0) * 100)
              << "\tV:" << ((color.value() / 255.0) * 100)
              << "\tTol:" << tol
              << std::endl;

    QSqlQuery query(m_db);
    query.prepare("SELECT * from images where "
                  "(h BETWEEN (:lower_h) AND (:upper_h)) AND"
                  "(s BETWEEN (:lower_s) AND (:upper_s)) AND"
                  "(s BETWEEN (:lower_v) AND (:upper_v));");
    query.bindValue(":lower_h", lower_h);
    query.bindValue(":upper_h", upper_h);
    query.bindValue(":lower_s", lower_s);
    query.bindValue(":upper_s", upper_s);
    query.bindValue(":lower_v", lower_v);
    query.bindValue(":upper_v", upper_v);


    if (query.exec())
    {
        while (query.next())
        {
            imgstruct row = {
                query.value(0).toString(),
                query.value(1).toString(),
                query.value(2).toDouble(),
                query.value(3).toDouble(),
                query.value(4).toDouble(),
                query.value(5).toDouble()
            };
            double avgHue = (color.hue() + row.h) / 2;
            double dist = std::abs(color.hue() - avgHue);
            row.concept_confidence = dist;
            images.push_back(row);
        }
    }

    return images;
}

std::vector<imgstruct> SqliteDb::cnnSearch(QString synsetId){
    images.clear();

    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM images WHERE concept == '" + synsetId + "';");

    if (query.exec())
    {
        while (query.next())
        {
            imgstruct row = {
                query.value(0).toString(),
                query.value(1).toString(),
                query.value(2).toDouble(),
                query.value(3).toDouble(),
                query.value(4).toDouble(),
                query.value(5).toDouble()
            };
            images.push_back(row);
        }
    }
    return images;
}

std::vector<imgstruct> SqliteDb::nameSearch(QString videoId){
    images.clear();

    QSqlQuery query(m_db);
    query.prepare("Select * from images where name LIKE '" + videoId + "@%';");

    if (query.exec())
    {
        while (query.next())
        {
            imgstruct row = {
                query.value(0).toString(),
                query.value(1).toString(),
                query.value(2).toDouble(),
                query.value(3).toDouble(),
                query.value(4).toDouble(),
                query.value(5).toDouble()
            };
            QString filename = row.filename;
            QString fileNameWithoutType = filename.split(".")[0]; // get rid of ".png"
            QStringList filenameComponents = fileNameWithoutType.split("@");
            row.concept_confidence = filenameComponents[1].toDouble();
            images.push_back(row);
        }
    }
    return images;
}


