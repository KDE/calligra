#include "KPresenterBackgroundIface.h"

#include "kpbackground.h"

#include <kapp.h>
#include <dcopclient.h>

KPresenterBackgroundIface::KPresenterBackgroundIface( KPBackGround *back_ )
    : DCOPObject()
{
   back = back_;
}

int KPresenterBackgroundIface::getBackType()
{
    return (int)back->getBackType();
}

int KPresenterBackgroundIface::getBackView()
{
    return (int)back->getBackView();
}

QColor KPresenterBackgroundIface::getBackColor1()
{
    return back->getBackColor1();
}

QColor KPresenterBackgroundIface::getBackColor2()
{
    return back->getBackColor2();
}

int KPresenterBackgroundIface::getBackColorType()
{
    return (int)back->getBackColorType();
}

QString KPresenterBackgroundIface::getBackPixFilename()
{
    return back->getBackPixFilename();
}

QString KPresenterBackgroundIface::getBackClipFilename()
{
    return back->getBackClipFilename();
}

int KPresenterBackgroundIface::getPageEffect()
{
    return (int)back->getPageEffect();
}

bool KPresenterBackgroundIface::getBackUnbalanced()
{
    return back->getBackUnbalanced();
}

int KPresenterBackgroundIface::getBackXFactor()
{
    return back->getBackXFactor();
}

int KPresenterBackgroundIface::getBackYFactor()
{
    return back->getBackYFactor();
}

QSize KPresenterBackgroundIface::getSize()
{
    return back->getSize();
}
