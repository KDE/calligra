#ifndef STEPSTEPLOCATION_P_H
#define STEPSTEPLOCATION_P_H

#include <QtCore/QObject>
#include <QtCore/QQueue>

class StepStepLocation_p : public QObject
{
    Q_OBJECT
public:
    explicit StepStepLocation_p(QObject *parent = 0);

signals:

public slots:
private:
  QQueue<int> location;

};

#endif // STEPSTEPLOCATION_P_H
