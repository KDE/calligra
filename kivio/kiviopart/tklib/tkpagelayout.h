#ifndef TKPAGELAYOUT_H
#define TKPAGELAYOUT_H

#include <qdom.h>

class TKPageLayout
{
public:
  TKPageLayout();
  ~TKPageLayout();

  int unit;
  float width;
  float height;

  float marginLeft;
  float marginRight;
  float marginTop;
  float marginBottom;

  float ptWidth();
  float ptHeight();
  float ptLeft();
  float ptRight();
  float ptTop();
  float ptBottom();

  void save(QDomElement&);
  void load(QDomElement&);

  void setDefault();
};
#endif

