#include "jsonreader.h"

JsonReader::JsonReader(const QString &filePath)
    : m_file(new QFile())
{
    m_file->setFileName(filePath);
}

bool JsonReader::openFile()
{
    return m_file->open(QIODevice::ReadOnly);
}

void JsonReader::closeFile()
{
    m_file->close();
}

QMap<QString, Sequence> JsonReader::loadSequencesFromJson()
{
    QMap<QString, Sequence> sequences;

    if (!openFile()) {
        emit sendInformation("Cannot open file:" + m_file->errorString());
        return sequences;
    }

    QByteArray data = m_file->readAll();
    closeFile();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

    if (!jsonDoc.isObject()) {
        emit sendInformation("Invalid JSON format");
        return sequences;
    }

    QJsonObject rootObject = jsonDoc.object();
    QJsonValue sequencesValue = rootObject.value("Sequences");

    if (!sequencesValue.isObject()) {
        emit sendInformation("Cannot find section \"Sequences\" in JSON file");
        return sequences;
    }

    QJsonObject sequencesObject = sequencesValue.toObject();

    for (const QString &sequenceName : sequencesObject.keys()) {
        QJsonValue sequenceValue = sequencesObject.value(sequenceName);

        if (!sequenceValue.isObject()) {
            emit sendInformation("Invalid sequence:" + sequenceName);
            continue;
        }

        QJsonObject stepsObject = sequenceValue.toObject();
        Sequence sequence;

        for (const QString &description : stepsObject.keys()) {
            QJsonObject stepObject = stepsObject.value(description).toObject();
            QString seq = stepObject.value("sequence").toString();
            QString res = stepObject.value("response").toString();

            sequence.steps.insert(description, {seq, res});
        }

        sequences.insert(sequenceName, sequence);
    }

    return sequences;
}
