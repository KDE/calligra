#include "StepStepBase_p.h"
#include "StepStepLocation.h"

StepStepBase_p::StepStepBase_p(QObject *parent) :
    QObject(parent)
{
}
bool StepStepBase_p::operator==(StepStepBase_p* other)
{
  if (other->Foreign() == Foreign() &&
      other->Position() == Position() &&
      other->Step() == Step() &&
      other->Type() == Type()
  ) {
    return true;
  }
  else {
    return false;
  }
}
bool StepStepBase_p::Foreign()
{
  return m_isForeign;
}
int StepStepBase_p::Position()
{
  return m_position;
}
void StepStepBase_p::setForeign(bool foreign)
{
  m_isForeign = foreign;

}
void StepStepBase_p::setPosition(int position)
{
  m_position = position;

}
QString StepStepBase_p::Step()
{
  return m_step;
}
QString StepStepBase_p::toString()
{
  QString string = (QString)"";
  return string;
}
QString StepStepBase_p::toXML()
{
  QString string = (QString)"";
  return string;
}
QString StepStepBase_p::Type()
{
  return m_type;

}