/* This file is part of the Calligra project

  Copyright 2011 Inge Wallin <inge@lysator.liu.se>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either 
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public 
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "EmfDeviceContext.h"

#include <qglobal.h>

#include <kdebug.h>

#include "EmfEnums.h"


/**
   Namespace for Windows Metafile (EMF) classes
*/
namespace Libemf
{


EmfDeviceContext::EmfDeviceContext()
{
    reset();
}


void EmfDeviceContext::reset()
{
    // Graphics Objects
    brush       = QBrush(Qt::NoBrush);
    image       = QImage();
    font        = QFont();
    escapement  = 0;
    orientation = 0;
    height      = 0;
    //Palette NYI
    pen         = QPen(Qt::black);
    clipRegion  = QRegion();
    path        = QPainterPath();
    isDefiningPath = false;

    // Structure Objects
    backgroundColor     = QColor(Qt::white);
    currentPoint        = QPoint(0, 0);
    foregroundTextColor = QColor(Qt::black);
    //Output Surface**  (what is this good for?  Mixing colors?)
    viewportExt = QSize();
    viewportOrg = QPoint();
    windowExt   = QSize();
    windowOrg   = QPoint();

    // Graphic Properties
    bkMode = 0;// FIXME: Check the real default
    //Break extra space NYI
    //Font mapping mode NYI
    rop2Mode = 0;// FIXME: Check the real default
    layoutMode = 0;// FIXME: Check the real default
    mapMode = MM_TEXT;
    polyFillMode = ALTERNATE;
    //Stretchblt mode NYI
    textAlignMode = TA_NOUPDATECP; // == TA_TOP == TA_LEFT
    //Text extra space NYI

    changedItems = 0xffffffff;  // Everything changed the first time.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    // Derivative values.  These are calculated from the base values above.

    // World transform
    m_windowExtIsSet = false;
    m_viewportExtIsSet = false;
    m_worldTransform.reset();
}


// ----------------------------------------------------------------
//                         World transform


// General note about coordinate spaces and transforms:
//
// There are several coordinate spaces in use when drawing an EMF file:
//  1. The object space, in which the objects' coordinates are expressed inside the EMF.
//     In general there are several of these.
//  2. The page space, which is where they end up being painted in the EMF picture.
//     The union of these form the bounding box of the EMF.
//  3. (possibly) the output space, where the EMF picture itself is placed
//     and/or scaled, rotated, etc
//
// The transform between spaces 1. and 2. is called the World Transform.
// The world transform can be changed either through calls to change
// the window or viewport or through calls to setWorldTransform() or
// modifyWorldTransform().
//
// The transform between spaces 2. and 3. is the transform that the QPainter
// already contains when it is given to us.  We need to save this and reapply
// it after the world transform has changed. We call this transform the Output
// Transform in lack of a better word. (Some sources call it the Device Transform.)
//


// FIXME:
// To change those functions it's better to have
// a large set of EMF files. EMF special case includes :
// - without call to setWindowOrg and setWindowExt
// - change the origin or the scale in the middle of the drawing
// - negative width or height
// and relative/absolute coordinate


void EmfDeviceContext::setWindowOrg(const QPoint &origin)
{
#if DEBUG_EMFPAINT
    kDebug(31000) << origin;
#endif

    // FIXME: See unanswered question at the start of this section.
    if (windowOrg == origin) {
        //kDebug(31000) << "same origin as before";
        return;
    }

    windowOrg = origin;

    // FIXME: Only recalculate when needed. Set DCWorldTransform in changedItems instead.
    recalculateWorldTransform();
}

void EmfDeviceContext::setWindowExt(const QSize &size)
{
#if DEBUG_EMFPAINT
    kDebug(31000) << size;
#endif

    // FIXME: See unanswered question at the start of this section.
    if (windowExt == size) {
        //kDebug(31000) << "same extension as before";
        return;
    }

    windowExt = size;
    m_windowExtIsSet = true;

    // FIXME: Only recalculate when needed. Set DCWorldTransform in changedItems instead.
    recalculateWorldTransform();
}

void EmfDeviceContext::setViewportOrg(const QPoint &origin)
{
#if DEBUG_EMFPAINT
    kDebug(31000) << origin;
#endif

    // FIXME: See unanswered question at the start of this section.
    if (viewportOrg == origin) {
        //kDebug(31000) << "same origin as before";
        return;
    }

    viewportOrg = origin;

    // FIXME: Only recalculate when needed. Set DCWorldTransform in changedItems instead.
    recalculateWorldTransform();
}

void EmfDeviceContext::setViewportExt(const QSize &size)
{
#if DEBUG_EMFPAINT
    kDebug(31000) << size;
#endif

    // FIXME: See unanswered question at the start of this section.
    if (viewportExt == size) {
        //kDebug(31000) << "same extension as before";
        return;
    }

    viewportExt = size;
    m_viewportExtIsSet = true;

    // FIXME: Only recalculate when needed. Set DCWorldTransform in changedItems instead.
    recalculateWorldTransform();
}

void EmfDeviceContext::modifyWorldTransform(const quint32 mode, float M11, float M12,
                                            float M21, float M22, float Dx, float Dy)
{
#if DEBUG_EMFPAINT
    if (mode == MWT_IDENTITY)
        kDebug(31000) << "Identity matrix";
    else
        kDebug(31000) << mode << M11 << M12 << M21 << M22 << Dx << Dy;
#endif

    QTransform matrix( M11, M12, M21, M22, Dx, Dy);

    if ( mode == MWT_IDENTITY ) {
        m_worldTransform = QTransform();
    } else if ( mode == MWT_LEFTMULTIPLY ) {
        m_worldTransform = matrix * m_worldTransform;
    } else if ( mode == MWT_RIGHTMULTIPLY ) {
        m_worldTransform = m_worldTransform * matrix;
    } else if ( mode == MWT_SET ) {
        m_worldTransform = matrix;
    } else {
	qWarning() << "Unimplemented transform mode" << mode;
    }

    changedItems |= DCWorldTransform;
}

void EmfDeviceContext::setWorldTransform(float M11, float M12, float M21, float M22,
                                         float Dx, float Dy )
{
#if DEBUG_EMFPAINT
    kDebug(31000) << M11 << M12 << M21 << M22 << Dx << Dy;
#endif

    QTransform matrix( M11, M12, M21, M22, Dx, Dy);

    m_worldTransform = matrix;
    changedItems |= DCWorldTransform;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

// Calculate the world transform from Window and Viewport
void EmfDeviceContext::recalculateWorldTransform()
{
    m_worldTransform = QTransform();

    // If neither the window nor viewport extension is set, then there
    // is no way to perform the calculation.  Just give up.
    if (!m_windowExtIsSet && !m_viewportExtIsSet)
        return;

    // Negative window extensions mean flip the picture.  Handle this here.
    bool  flip = false;
    qreal midpointX = 0.0;
    qreal midpointY = 0.0;
    qreal scaleX = 1.0;
    qreal scaleY = 1.0;
    if (windowExt.width() < 0) {
        midpointX = windowOrg.x() + windowExt.width() / qreal(2.0);
        scaleX = -1.0;
        flip = true;
    }
    if (windowExt.height() < 0) {
        midpointY = windowOrg.y() + windowExt.height() / qreal(2.0);
        scaleY = -1.0;
        flip = true;
    }
    if (flip) {
        //kDebug(31000) << "Flipping" << midpointX << midpointY << scaleX << scaleY;
        m_worldTransform.translate(midpointX, midpointY);
        m_worldTransform.scale(scaleX, scaleY);
        m_worldTransform.translate(-midpointX, -midpointY);
        //kDebug(31000) << "After flipping for window" << mWorldTransform;
    }

    // Update the world transform if both window and viewport are set...
    // FIXME: Check windowExt == 0 in any direction
    if (m_windowExtIsSet && m_viewportExtIsSet) {
        // Both window and viewport are set.
        qreal windowViewportScaleX = qreal(viewportExt.width()) / qreal(windowExt.width());
        qreal windowViewportScaleY = qreal(viewportExt.height()) / qreal(windowExt.height());

        m_worldTransform.translate(-windowOrg.x(), -windowOrg.y());
        m_worldTransform.scale(windowViewportScaleX, windowViewportScaleY);
        m_worldTransform.translate(viewportOrg.x(), viewportOrg.y());
    }

    changedItems |= DCWorldTransform;
}


}
