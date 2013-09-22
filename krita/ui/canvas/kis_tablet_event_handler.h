#ifndef KIS_TABLET_EVENT_HANDLER_H
#define KIS_TABLET_EVENT_HANDLER_H

#include <QObject>
#include <QWidget>

#if defined(Q_WS_MAC)
#include "kis_mac_tablet_handler.h"
#endif
#if defined(Q_WS_WIN)
#include "kis_wintab_tablet_handler.h"
#endif
#if defined(Q_WS_X11)
#include "kis_x11_tablet_handler.h"
#endif

class KisTabletEventHandler : public QObject
{
    Q_OBJECT
public:
    explicit KisTabletEventHandler(QWidget *parent = 0);
#if defined(Q_WS_MAC)
    virtual bool macEvent(EventHandlerCallRef, EventRef);
#endif
#if defined(Q_WS_WIN)
    virtual bool winEvent(MSG *message, long *result);
#endif
#if defined(Q_WS_X11)
    virtual bool x11Event(XEvent *event);
#endif
private:
    QWidget *m_widget;

#if defined(Q_WS_MAC)
   KisMacTabletHandler *m_tabletHandlerImpl;
#endif
#if defined(Q_WS_WIN)
    KisWintabTabletHander *m_tabletHandlerImpl;
#endif
#if defined(Q_WS_X11)
    KisX11TabletHandler *m_tabletHandlerImpl;
#endif

};

#endif // KIS_TABLET_EVENT_HANDLER_H
