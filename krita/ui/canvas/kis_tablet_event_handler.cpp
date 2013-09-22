#include "kis_tablet_event_handler.h"


KisTabletEventHandler::KisTabletEventHandler(QWidget *parent)
  : QObject(parent)
  , m_widget(parent)
{
#if defined(Q_WS_MAC)
    m_tabletHandlerImpl = new KisMacTabletHandler;
#endif
#if defined(Q_WS_WIN)
    m_tabletHandlerImpl = new KisWintabTabletHandler;
#endif
#if defined(Q_WS_X11)
    m_tabletHandlerImpl = new KisX11TabletHandler;
#endif

}


#if defined(Q_WS_MAC)
bool KisTabletEventHandler::macEvent(EventHandlerCallRef, EventRef)
{
    return false;
}
#endif

#if defined(Q_WS_WIN)
bool KisTabletEventHandler::winEvent(MSG *message, long *result)
{
    Q_UNUSED(message);
    Q_UNUSED(result);
    return false;
}
#endif

#if defined(Q_WS_X11)
bool KisTabletEventHandler::x11Event(XEvent *event)
{
    return m_tabletHandlerImpl->x11Event(event);
}
#endif
