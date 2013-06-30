#include "StepStepLocation.h"
#include <QtGui/QTextCursor>
StepStepLocation::StepStepLocation(QObject *parent) :
    QObject(parent)
{
}
StepStepLocation::StepStepLocation(QTextCursor cursor, QObject *parent):
    QObject(parent)
{

}

StepStepLocation::operator QTextCursor()
{
    return QTextCursor();
}
