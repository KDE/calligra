#ifndef STEPSTEPLOCATION_H
#define STEPSTEPLOCATION_H

#include <QObject>
#include <QtCore/QQueue>

class QTextCursor;
class StepStepLocation : public QObject
{
    Q_OBJECT
public:
    explicit StepStepLocation(QObject *parent = 0);
    StepStepLocation(QTextCursor cursor, QObject *parent =0);
    operator QTextCursor();
signals:
    
public slots:
private:
    QQueue<int> location;
    
};

#endif // STEPSTEPLOCATION_H
