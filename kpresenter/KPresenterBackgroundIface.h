#ifndef KPRESENTER_DOC_IFACE_H
#define KPRESENTER_DOC_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <qstring.h>
#include <qcolor.h>

class KPBackGround;

class KPresenterBackgroundIface : virtual public DCOPObject
{
    K_DCOP
public:
    KPresenterBackgroundIface( KPBackGround *back_ );

k_dcop:
    int getBackType();
    int getBackView();
    QColor getBackColor1();
    QColor getBackColor2();
    int getBackColorType();
    QString getBackPixFilename();
    QString getBackClipFilename();
    int getPageEffect();
    bool getBackUnbalanced();
    int getBackXFactor();
    int getBackYFactor();
    QSize getSize();

private:
    KPBackGround *back;

};

#endif
