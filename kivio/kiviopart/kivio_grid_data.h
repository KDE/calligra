#ifndef KIVIO_GRID_DATA_H
#define KIVIO_GRID_DATA_H

#include "tkunits.h"
#include <qcolor.h>
#include <qdom.h>

class KivioGridData
{
public:
  KivioGridData();
  ~KivioGridData();

  void save(QDomElement&, const QString&);
  void load(QDomElement&, const QString&);

  TKSize freq;
  TKSize snap;
  QColor color;
  bool isSnap;
  bool isShow;
};

#endif

