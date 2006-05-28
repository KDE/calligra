#ifndef __VCURVEFIT_H__
#define __VCURVEFIT_H__

#include <QString>
#include <q3ptrlist.h>
#include <QPointF>

VPath *bezierFit(Q3PtrList<QPointF> &points,float error);

#endif

