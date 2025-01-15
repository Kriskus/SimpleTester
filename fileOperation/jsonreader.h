#pragma once

#include <QMap>
#include <QFile>
#include <QString>
#include <QObject>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QScopedPointer>

#include "sequencestructure.h"

class JsonReader : public QObject
{
    Q_OBJECT
public:
    JsonReader(const QString &filePath);

public slots:
    bool openFile();
    void closeFile();
    QMap<QString, Sequence> loadSequencesFromJson();

private:
    QScopedPointer<QFile> m_file;

signals:
    void sendInformation(QString);
};
