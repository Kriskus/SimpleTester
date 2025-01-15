#pragma once

#include <QObject>
#include <QMutex>
#include <QObject>
#include <QWaitCondition>
#include <QApplication>
#include <QThread>

#include <QDebug>

#include "../fileOperation/sequencestructure.h"

class TaskOperatons : public QObject
{
    Q_OBJECT
public:
    explicit TaskOperatons(QObject *parent = nullptr, QMap<QString, Sequence> sequences = {});

private:
    QMap<QString, Sequence> m_sequences{};
    QByteArray m_responseAwaited{};

    bool m_status{true};
    QMutex m_sync;
    QWaitCondition m_pauseCond;
    bool m_pause{false};

public slots:
    void startTask();
    void pauseTask();
    void resumeTask();
    void endTask();

signals:
    void finished();
    void sendSequence(const QString&);
    void sendTestCaseName(const QString&);
    void sendResult(const QString&);
    void sendResponseAwaited(const QString&);
};
