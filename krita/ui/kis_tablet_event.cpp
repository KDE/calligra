#include "kis_tablet_event.h"

KisTabletEvent::KisTabletEvent(Type type, const QPoint &pos, const QPoint &globalPos,
                           const QPointF &hiResGlobalPos, int device, int pointerType,
                           qreal pressure, int xTilt, int yTilt, qreal tangentialPressure,
                           qreal rotation, int z, Qt::KeyboardModifiers keyState, qint64 uniqueID, Qt::MouseButtons buttons)
    : QInputEvent(type, keyState),
      mPos(pos),
      mGPos(globalPos),
      mHiResGlobalPos(hiResGlobalPos),
      mDev(device),
      mPointerType(pointerType),
      mXT(xTilt),
      mYT(yTilt),
      mZ(z),
      mPress(pressure),
      mTangential(tangentialPressure),
      mRot(rotation),
      mUnique(uniqueID),
      mouseState(buttons)
{
}


KisTabletEvent::~KisTabletEvent()
{
}
