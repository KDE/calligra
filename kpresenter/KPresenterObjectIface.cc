#include "KPresenterObjectIface.h"

#include "kpobject.h"

#include <kapp.h>
#include <dcopclient.h>

KPresenterObjectIface::KPresenterObjectIface( KPObject *obj_ )
    : DCOPObject()
{
   obj = obj_;
}

int KPresenterObjectIface::getType()
{
    return (int)obj->getType();
}

QRect KPresenterObjectIface::getBoundingRect()
{
    return obj->getBoundingRect( 0, 0 );
}

bool KPresenterObjectIface::isSelected()
{
    return obj->isSelected();
}

float KPresenterObjectIface::getAngle()
{
    return obj->getAngle();
}

int KPresenterObjectIface::getShadowDistance()
{
    return obj->getShadowDistance();
}

int KPresenterObjectIface::getShadowDirection()
{
    return (int)obj->getShadowDirection();
}

QColor KPresenterObjectIface::getShadowColor()
{
    return obj->getShadowColor();
}

QSize KPresenterObjectIface::getSize()
{
    return obj->getSize();
}

QPoint KPresenterObjectIface::getOrig()
{
    return obj->getOrig();
}

int KPresenterObjectIface::getEffect()
{
    return (int)obj->getEffect();
}

int KPresenterObjectIface::getEffect2()
{
    return (int)obj->getEffect2();
}

int KPresenterObjectIface::getPresNum()
{
    return obj->getPresNum();
}

int KPresenterObjectIface::getSubPresSteps()
{
    return obj->getSubPresSteps();
}

bool KPresenterObjectIface::getDisappear()
{
    return obj->getDisappear();
}

int KPresenterObjectIface::getDisappearNum()
{
    return obj->getDisappearNum();
}

int KPresenterObjectIface::getEffect3()
{
    return (int)obj->getEffect3();
}

