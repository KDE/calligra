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


#ifndef _EMFDEVICECONTEXT_H_
#define _EMFDEVICECONTEXT_H_


#include <QColor>
#include <QRect>
#include <QPen>
#include <QFont>
#include <QPolygon>
#include <QRegion>
#include <QPainterPath>
#include <QTransform>
#include <QVariant>


/**
   Namespace for Windows Extended Metafile (EMF) classes
*/
namespace Libemf
{


/**
 * EmfDeviceContext contains the EMF Playback Device Context.
 *
 * Unfortunately [MS-EMF].pdf doesn't have a formal description of the
 * Device Context, so we will have to piece together information from
 * many different places in the spec.
 *
 * For a start, we will use the WMF device context as a template.
 */


enum DeviceContextMembers {
    // Graphic objects
    DCBrush          = 0x00000001,
    DCFont           = 0x00000002,
    DCPalette        = 0x00000004,
    DCPen            = 0x00000008,
    DCClipRegion     = 0x00000010,
    DCPath           = 0x00000020,

    // Structure objects
    DCBgColor        = 0x00000040,
    DCCurrentPos     = 0x00000080,
    DCFgTextColor    = 0x00000100,
    // Output surface not supported
    DCViewportExt    = 0x00000200,
    DCViewportorg    = 0x00000400,
    DCWindowExt      = 0x00000800,
    DCWindoworg      = 0x00001000,
    DCWorldTransform = 0x00001E00, // All the viewport/window bits together

    // Graphic properties
    DCBkMode         = 0x00002000,
    DCBrExtraSpace   = 0x00004000,
    DCFontMapMode    = 0x00008000,
    DCRop2Mode       = 0x00010000,
    DCLayoutMode     = 0x00020000,
    DCMapMode        = 0x00040000,
    DCPolyFillMode   = 0x00080000,
    DCStretchBltMode = 0x00100000,
    DCTextAlignMode  = 0x00200000,
    DCTextExtraSpace = 0x00400000
};

/**
   EMF Playback Device Context
*/
class EmfDeviceContext
{
public:
    EmfDeviceContext();
    void reset();

    // Graphic Objects
    QBrush        brush;        // Brush
    QImage        image;        //   - extra image
    QFont         font;         // Font
    int           escapement;   //   - rotation of the text in 1/10th of a degree
    int           orientation;  //   - rotation of characters in 1/10th of a degree
    int           height;       //   - original font height; can be negative
    //Palette                   // Palette not supported yet
    QPen          pen;          // Pen
    QRegion       clipRegion;   // Region
    QPainterPath  path;         // Path
    bool          isDefiningPath; // True when we are defining a path,
                                // i.e. after an EMR_BEGINPATH record.

    // Structure Objects
    QColor  backgroundColor;    // Background color
    QPoint  currentPoint;       // Drawing position (Current point)
    QColor  foregroundTextColor; // Foreground text color
    ;    //Output Surface**  (what is this good for?  Mixing colors?)
    QPoint viewportOrg;         // Viewport origin
    QSize  viewportExt;         // Viewport extent
    QPoint windowOrg;           // Window origin
    QSize  windowExt;           // Window extent

    // Graphic Properties
    quint32  bkMode;         // Background mode
    //Break extra space NYI
    //Font mapping mode NYI
    quint32  rop2Mode;          // Foreground mix mode (FIXME: Or is it?)
    quint32  layoutMode;        // Layout mode
    quint32  mapMode;           // Mapping mode
    quint32  polyFillMode;      // Polygon fill mode
    quint32  stretchBltMode;    // Stretchbitblit mode
    quint32  textAlignMode;     // Text alignment mode
    //Text extra space NYI

    // ----------------------------------------------------------------
    // Object table

    // Table of objects, like pens, brushes, etc.
    QMap<quint32, QVariant>  objectTable;

    // ----------------------------------------------------------------
    //                         Helper data

    // This is not part of the actual device context, but indicates
    // changed items.  It is used by the backends to update their
    // internal state.
    quint32  changedItems;      // bitmap of DeviceContextMembers


    // Cached values

    // window and viewport calculation
    bool        m_windowExtIsSet;
    bool        m_viewportExtIsSet;
    QTransform  m_worldTransform;

    // Convenience functions
    void setWindowOrg(const QPoint &origin);
    void setWindowExt(const QSize &size);
    void setViewportOrg(const QPoint &origin);
    void setViewportExt(const QSize &size);
    void modifyWorldTransform(const quint32 mode, float M11, float M12,
                              float M21, float M22, float Dx, float Dy);
    void setWorldTransform(float M11, float M12, float M21, float M22, float Dx, float Dy);

private:
    void recalculateWorldTransform();
};


}

#endif
