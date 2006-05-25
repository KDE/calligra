#ifndef __VCURVEFIT_H__
#define __VCURVEFIT_H__

#include <QString>
#include <q3ptrlist.h>
#include "KoPoint.h"

VPath *bezierFit(Q3PtrList<KoPoint> &points,float error);

#endif

