#ifndef KPRESENTER_DOC_IFACE_H
#define KPRESENTER_DOC_IFACE_H

#include <dcopobject.h>
#include <dcopref.h>

#include <qstring.h>
#include <qcolor.h>

class KPObject;

class KPresenterObjectIface : virtual public DCOPObject
{
    K_DCOP
public:
    KPresenterObjectIface( KPObject *obj_ );

k_dcop:
    int getType();
    QRect getBoundingRect();
    bool isSelected();
    float getAngle();
    int getShadowDistance();
    int getShadowDirection();
    QColor getShadowColor();
    QSize getSize();
    QPoint getOrig();
    int getEffect();
    int getEffect2();
    int getPresNum();
    int getSubPresSteps();
    bool getDisappear();
    int getDisappearNum();
    int getEffect3();

    
private:
    KPObject *obj;

};

#endif
