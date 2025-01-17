#include "taskoperatons.h"

TaskOperatons::TaskOperatons(QObject *parent, QMap<QString, Sequence> sequences)
    : QObject{parent}
    , m_sequences(sequences)
{}

void TaskOperatons::startTask()
{
    for (const auto &sequenceKey : m_sequences.keys()) {
        const auto &sequence = m_sequences[sequenceKey];

        emit sendSequenceName("<span style='color: blue; font: bold;'>Testowana sekwencja: " + sequenceKey.toUpper() + "</span>");

        for (const auto &testCase : sequence.steps.keys()) {
            const auto &step = sequence.steps[testCase];
            emit sendTestCaseName("&nbsp;&nbsp;&nbsp;&nbsp;<span style='color: black; font: bold;'>Przypadek testowy: " + testCase + "</span>");
            emit sendResponseAwaited(step.response);
            emit sendSequence(step.sequence);
            m_pause = true;

            m_sync.lock();
            if(m_pause) {
                m_pauseCond.wait(&m_sync);
            }
            m_sync.unlock();
        }
    }
    emit finished();
}

void TaskOperatons::pauseTask()
{
    m_sync.lock();
    m_pause = true;
    m_sync.unlock();
}

void TaskOperatons::resumeTask()
{
    qApp->processEvents();
    m_sync.lock();
    m_pause = false;
    m_sync.unlock();
    m_pauseCond.wakeAll();
}

void TaskOperatons::endTask()
{
    qApp->processEvents();
}
