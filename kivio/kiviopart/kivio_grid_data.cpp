#include "kivio_grid_data.h"
#include "kivio_common.h"

KivioGridData::KivioGridData()
{
  color = QColor(200,200,200);
  freq.set(10,10,UnitPoint);
  snap.set(10,10,UnitPoint);
  isSnap = true;
  isShow = true;
}

KivioGridData::~KivioGridData()
{
}

void KivioGridData::save(QDomElement& element, const QString& name)
{
  freq.save(element,name+"Freg");
  snap.save(element,name+"Snap");
  XmlWriteColor(element,name+"Color",color);
  element.setAttribute(name+"IsSnap",(int)isSnap);
  element.setAttribute(name+"IsShow",(int)isShow);
}

void KivioGridData::load(QDomElement& element, const QString& name)
{
  TKSize sdef;
  sdef.set(10,10,UnitPoint);
  freq.load(element,name+"Freg",sdef);
  snap.load(element,name+"Snap",sdef);

  QColor def(QColor(228,228,228));
  color = XmlReadColor(element, name+"Color", def);

  isSnap = (bool)element.attribute(name+"IsSnap","0").toInt();
  isShow = (bool)element.attribute(name+"IsShow","0").toInt();
}
