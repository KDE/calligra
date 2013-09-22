#ifndef KIS_X11_TABLET_HANDLER_H
#define KIS_X11_TABLET_HANDLER_H

#include <QByteArray>

extern "C" {
#   define class c_class  //XIproto.h has a name member named 'class' which the c++ compiler doesn't like
#   include <wacomcfg.h>
#   undef class
}

class KisX11TabletHandler
{
public:
    KisX11TabletHandler();

private:

    Q_GLOBAL_STATIC(QTabletDeviceDataList, tablet_devices)
    QTabletDeviceDataList *qt_tablet_devices()
    {
        return tablet_devices();
    }

    bool x11Event(XEvent *event);

    extern bool qt_tabletChokeMouse;

    bool translateXinputEvent(const XEvent*, QTabletDeviceData *tablet);


    // from include/Xwacom.h
    #  define XWACOM_PARAM_TOOLID 322
    #  define XWACOM_PARAM_TOOLSERIAL 323

    typedef WACOMCONFIG * (*PtrWacomConfigInit) (Display*, WACOMERRORFUNC);
    typedef WACOMDEVICE * (*PtrWacomConfigOpenDevice) (WACOMCONFIG*, const char*);
    typedef int *(*PtrWacomConfigGetRawParam) (WACOMDEVICE*, int, int*, int, unsigned*);
    typedef int (*PtrWacomConfigCloseDevice) (WACOMDEVICE *);
    typedef void (*PtrWacomConfigTerm) (WACOMCONFIG *);

    static PtrWacomConfigInit ptrWacomConfigInit = 0;
    static PtrWacomConfigOpenDevice ptrWacomConfigOpenDevice = 0;
    static PtrWacomConfigGetRawParam ptrWacomConfigGetRawParam = 0;
    static PtrWacomConfigCloseDevice ptrWacomConfigCloseDevice = 0;
    static PtrWacomConfigTerm ptrWacomConfigTerm = 0;
    Q_GLOBAL_STATIC(QByteArray, wacomDeviceName)


};

#endif // KIS_X11_TABLET_HANDLER_H
