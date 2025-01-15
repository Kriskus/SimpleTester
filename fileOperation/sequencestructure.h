#pragma once

#include <QString>
#include <QMap>

struct SequenceStep {
    QString sequence;
    QString response;
};

struct Sequence {
    QMap<QString, SequenceStep> steps;
};
