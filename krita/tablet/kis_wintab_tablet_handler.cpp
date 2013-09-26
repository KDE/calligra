#include "kis_wintab_tablet_handler.h"
#include "kis_tablet_handler_p.h"
#include "kis_tablet_event.h"

#include <QDebug>
#include <QLibrary>

#define PACKETDATA  (PK_X | PK_Y | PK_BUTTONS | PK_NORMAL_PRESSURE | PK_TANGENT_PRESSURE \
                     | PK_ORIENTATION | PK_CURSOR | PK_Z)
#define PACKETMODE  0

#include <wintab.h>
#ifndef CSR_TYPE
#define CSR_TYPE 20 // Some old Wacom wintab.h may not provide this constant.
#endif
#include <pktdef.h>

typedef HCTX (API *PtrWTOpen)(HWND, LPLOGCONTEXT, BOOL);
typedef BOOL (API *PtrWTClose)(HCTX);
typedef UINT (API *PtrWTInfo)(UINT, UINT, LPVOID);
typedef BOOL (API *PtrWTEnable)(HCTX, BOOL);
typedef BOOL (API *PtrWTOverlap)(HCTX, BOOL);
typedef int  (API *PtrWTPacketsGet)(HCTX, int, LPVOID);
typedef BOOL (API *PtrWTGet)(HCTX, LPLOGCONTEXT);
typedef int  (API *PtrWTQueueSizeGet)(HCTX);
typedef BOOL (API *PtrWTQueueSizeSet)(HCTX, int);

static PtrWTInfo ptrWTInfo = 0;
static PtrWTEnable ptrWTEnable = 0;
static PtrWTOverlap ptrWTOverlap = 0;
static PtrWTPacketsGet ptrWTPacketsGet = 0;
static PtrWTGet ptrWTGet = 0;

typedef QHash<quint64, QTabletDeviceData> QTabletCursorInfo;
Q_GLOBAL_STATIC(QTabletCursorInfo, tCursorInfo)
QTabletDeviceData currentTabletPointer;

// resolve the WINTAB api functions
static void initWinTabFunctions()
{
    qDebug() << "Initializing WinTab" << WT_PACKET;
    QLibrary library(QLatin1String("wintab32"));
    Q_ASSERT(library.isLoaded());
    ptrWTInfo = (PtrWTInfo)library.resolve("WTInfoW");
    ptrWTGet = (PtrWTGet)library.resolve("WTGetW");
    ptrWTEnable = (PtrWTEnable)library.resolve("WTEnable");
    ptrWTOverlap = (PtrWTEnable)library.resolve("WTOverlap");
    ptrWTPacketsGet = (PtrWTPacketsGet)library.resolve("WTPacketsGet");
}

static void tabletInit(const quint64 uniqueId, const UINT csr_type, HCTX hTab)
{
    qDebug() << "tabletInit" << uniqueId << csr_type << hTab;
    Q_ASSERT(ptrWTInfo);
    Q_ASSERT(ptrWTGet);

    Q_ASSERT(!tCursorInfo()->contains(uniqueId));

    /* browse WinTab's many info items to discover pressure handling. */
    AXIS np;
    LOGCONTEXT lc;

    /* get the current context for its device variable. */
    ptrWTGet(hTab, &lc);

    /* get the size of the pressure axis. */
    QTabletDeviceData tdd;
    tdd.llId = uniqueId;

    ptrWTInfo(WTI_DEVICES + lc.lcDevice, DVC_NPRESSURE, &np);
    tdd.minPressure = int(np.axMin);
    tdd.maxPressure = int(np.axMax);

    ptrWTInfo(WTI_DEVICES + lc.lcDevice, DVC_TPRESSURE, &np);
    tdd.minTanPressure = int(np.axMin);
    tdd.maxTanPressure = int(np.axMax);

    LOGCONTEXT lcMine;

    /* get default region */
    ptrWTInfo(WTI_DEFCONTEXT, 0, &lcMine);

    tdd.minX = 0;
    tdd.maxX = int(lcMine.lcInExtX) - int(lcMine.lcInOrgX);

    tdd.minY = 0;
    tdd.maxY = int(lcMine.lcInExtY) - int(lcMine.lcInOrgY);

    tdd.minZ = 0;
    tdd.maxZ = int(lcMine.lcInExtZ) - int(lcMine.lcInOrgZ);

    const uint cursorTypeBitMask = 0x0F06; // bitmask to find the specific cursor type (see Wacom FAQ)
    if (((csr_type & 0x0006) == 0x0002) && ((csr_type & cursorTypeBitMask) != 0x0902)) {
        tdd.currentDevice = QTabletEvent::Stylus;
    } else {
        switch (csr_type & cursorTypeBitMask) {
            case 0x0802:
                tdd.currentDevice = QTabletEvent::Stylus;
                break;
            case 0x0902:
                tdd.currentDevice = QTabletEvent::Airbrush;
                break;
            case 0x0004:
                tdd.currentDevice = QTabletEvent::FourDMouse;
                break;
            case 0x0006:
                tdd.currentDevice = QTabletEvent::Puck;
                break;
            case 0x0804:
                tdd.currentDevice = QTabletEvent::RotationStylus;
                break;
            default:
                tdd.currentDevice = QTabletEvent::NoDevice;
        }
    }
    tCursorInfo()->insert(uniqueId, tdd);
}

static void tabletUpdateCursor(QTabletDeviceData &tdd, const UINT currentCursor)
{
    qDebug() << "tabletUpdateCursor()";
    switch (currentCursor % 3) { // %3 for dual track
    case 0:
        tdd.currentPointerType = QTabletEvent::Cursor;
        break;
    case 1:
        tdd.currentPointerType = QTabletEvent::Pen;
        break;
    case 2:
        tdd.currentPointerType = QTabletEvent::Eraser;
        break;
    default:
        tdd.currentPointerType = QTabletEvent::UnknownPointer;
    }
}

KisWinTabTabletHandler::KisWinTabTabletHandler(QWidget *widget)
    : m_widget(widget)
{
    qDebug() << "Creating KisWinTabTabletHandler for" << widget;
    initWinTabFunctions();
}

KisWinTabTabletHandler::~KisWinTabTabletHandler()
{
    qDebug() << "Closing KisWinTabTabletHandler";
}

bool KisWinTabTabletHandler::winEvent(MSG *ev, long */*result*/)
{
    switch(ev->message){
    case WT_CTXOPEN:
        // The owning window (and all manager windows) will immediately receive
        // a WT_CTXOPEN message when the context has been opened.
        qDebug() << "Received WT_CTXOPEN";
        break;
    case WT_INFOCHANGE:
        // Applications can respond to information changes by fielding the
        // WT_INFOCHANGE message.
        qDebug() << "Recevied WT_INFOCHANGE";
        break;
    case WM_LBUTTONDBLCLK:
        qDebug() << "Recevied WM_LBUTTONDBLCLCK";
        break;
    case WM_LBUTTONDOWN:
        qDebug() << "Received WM_LBUTTONDOWN";
        break;
    case WM_LBUTTONUP:
        qDebug() << "Received WM_LBUTTONUP";
        break;
    case WT_PROXIMITY:
        // When a pointing device enters or leaves a context, the application
        // that opened the context is notified through the WT_PROXIMITY message.
        qDebug() << "Received WT_PROXIMITY";
        break;
    case WT_CSRCHANGE:
        // When a different pointing device enters proximity of the context, the
        // application is informed through a WT_CSRCHANGE message, if the application
        // requested this message when the context was created.
        qDebug() << "received WT_CSRCHANGE";
        break;
    case WM_ACTIVATE:
        // When applications receive the WM_ACTIVATE message, they should push their
        // contexts to the bottom of the overlap order if their application is being
        // deactivated, and should bring their context to the top if they are being
        // activated
        qDebug() << "WM_ACTIVATE";
        break;
    case WT_PACKET:
        qDebug() << "WT_PACKET";
        break;
    default:
        qDebug() << "KisWintabTabletHandler::winEvent" << ev->message;
    }
    return false;
}

