#include "tkpagelayout.h"

#include "tkunits.h"
#include "tkcommon.h"

#include <qtextstream.h>

TKPageLayout::TKPageLayout()
{
}

TKPageLayout::~TKPageLayout()
{
}

float TKPageLayout::ptWidth()
{
  return cvtUnitToPt(unit, width);
}

float TKPageLayout::ptHeight()
{
  return cvtUnitToPt(unit, height);
}

float TKPageLayout::ptLeft()
{
  return cvtUnitToPt(unit, marginLeft);
}

float TKPageLayout::ptRight()
{
  return cvtUnitToPt(unit, marginRight);
}

float TKPageLayout::ptTop()
{
  return cvtUnitToPt(unit, marginTop);
}

float TKPageLayout::ptBottom()
{
  return cvtUnitToPt(unit, marginBottom);
}

void TKPageLayout::save(QDomElement& e)
{
  XmlWriteInt(e, "unit", unit);
  XmlWriteFloat(e, "width", width);
  XmlWriteFloat(e, "height", height);
  XmlWriteFloat(e, "marginLeft", marginLeft);
  XmlWriteFloat(e, "marginRight", marginRight);
  XmlWriteFloat(e, "marginTop", marginTop);
  XmlWriteFloat(e, "marginBottom", marginBottom);
}

void TKPageLayout::load(QDomElement& e)
{
  unit = XmlReadInt(e, "unit", 0);
  width = XmlReadFloat(e, "width", 0.0);
  height = XmlReadFloat(e, "height", 0.0);
  marginLeft = XmlReadFloat(e, "marginLeft", 0.0);
  marginRight = XmlReadFloat(e, "marginRight", 0.0);
  marginTop = XmlReadFloat(e, "marginTop", 0.0);
  marginBottom = XmlReadFloat(e, "marginBottom", 0.0);
}

void TKPageLayout::setDefault()
{
  unit = UnitMillimeter;
  width  = 210.0;
  height = 297.0;

  marginLeft   = 20.0;
  marginRight  = 20.0;
  marginTop    = 20.0;
  marginBottom = 20.0;
}
