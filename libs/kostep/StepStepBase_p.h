#ifndef STEPSTEPBASE_P_H
#define STEPSTEPBASE_P_H

#include <QObject>
#include "StepStepLocation.h"
class StepStepBase_p : public QObject
{
    Q_OBJECT
public:
    explicit StepStepBase_p(QObject *parent = 0);

    bool operator==(StepStepBase_p* other);
    QString toString();
    QString toXML();
    bool Foreign();
    void setForeign(bool foreign);
    int Position();
    void setPosition(int position);
    //StepStepLocation Location();
    //virtual void setLocation(StepStepLocation location);
    QString Step();
    QString Type();

signals:

public slots:

private:
  bool m_isForeign;
  int m_position;
  QString m_step;
  const QString m_type;
  StepStepLocation m_location;
};

#endif // STEPSTEPBASE_P_H
