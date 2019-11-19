#ifndef JSONSTORAGE_H
#define JSONSTORAGE_H

#include "brokenline.h"

#include <QJsonArray>
#include <QVector>


class JsonStorage
{
public:
    void saveToFile(const QVector<BrokenLine> &lines, const QString &fileName);
    void writeLines(QJsonObject &jsonObject, const QVector<BrokenLine> &lines) const;
    void writeLine(const BrokenLine & line, QJsonArray &lineObject) const;

    void loadFromFile(QVector<BrokenLine> &lines, const QString &fileName) const;
    void readLines(const QJsonObject &jsonObject, QVector<BrokenLine> &lines) const;
    BrokenLine readLine(const QJsonArray &jsonObject) const;
};

#endif // JSONSTORAGE_H
