#include "jsonstorage.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QIODevice>


void JsonStorage::saveToFile(const QVector<BrokenLine> &lines)
{
    QFile file("file.json");
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning("can't open file for writing");
        return ;
    }

    QJsonObject jsonObject;

    writeLines(jsonObject, lines);

    QJsonDocument saveDoc(jsonObject);

    file.write(saveDoc.toJson());
}

void JsonStorage::writeLines(QJsonObject &jsonObject, const QVector<BrokenLine> &lines) const
{
    QJsonArray linesArray;

    for (const auto & line: lines)
    {
        QJsonArray lineObject;
        writeLine(line, lineObject);
        QJsonObject a;
        linesArray.append(lineObject);
    }

    jsonObject["lines"] = linesArray;
}

void JsonStorage::writeLine(const BrokenLine &line, QJsonArray & lineObject) const
{
    for (const auto & point: line.points())
    {
        QJsonObject pointObject;
        pointObject["x"] = point.point().x();
        pointObject["y"] = point.point().y();
        if (point.color())
        {
            pointObject["c"] = point.color()->name();
        }

        lineObject.append(pointObject);
    }
}

void JsonStorage::loadFromFile(QVector<BrokenLine> &lines) const
{
    QFile file("file.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("can't open file for reading");
        return ;
    }

    QByteArray saveData = file.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    readLines(loadDoc.object(), lines);
}

void JsonStorage::readLines(const QJsonObject &jsonObject, QVector<BrokenLine> &lines) const
{
    if (jsonObject.contains("lines") && jsonObject["lines"].isArray())
    {
        QJsonArray linesArray = jsonObject["lines"].toArray();
        for (auto i = linesArray.begin(); i != linesArray.end(); ++i)
        {
            lines.push_back(readLine(i->toArray()));
        }
    }
}

BrokenLine JsonStorage::readLine(const QJsonArray &jsonObject) const
{
    BrokenLine line;
    for ( auto i = jsonObject.begin(); i != jsonObject.end(); ++i)
    {
        std::optional<QColor> color;
        if ((*i).toObject().contains("c"))
        {
            color = std::make_optional<QColor>((*i)["c"].toString());
        }

        line.points().push_back(
                    BrokenLine::ControlPoint(
                        QPoint((*i)["x"].toInt(), (*i)["y"].toInt()),
                        color));
    }
    line.updateLength();
    line.updateGradient();

    return line;
}

