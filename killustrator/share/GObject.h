/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef GObject_h_
#define GObject_h_

#include <qobject.h>
#include <qlist.h>
#include <qvaluelist.h>
#include <qdict.h>
#include <qwmatrix.h>

#include <Gradient.h>
#include <Coord.h>
#include <GradientShape.h>

#include <math.h>

#ifndef M_PI // not ANSI C++, so it maybe...
#define M_PI            3.14159265358979323846  /* pi */
#endif

#define RAD_FACTOR 180.0 / M_PI

class GOState;
class GLayer;
class GCurve;
class QDomDocument;
class QDomElement;

/**
 * The base class for all graphical objects.
 *
 * Documentation
 *
 * @author  Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
 * @version $Revision$
 */
class GObject : public QObject {
    Q_OBJECT
protected:
    GObject ();
    GObject (const QDomElement &element);
    GObject (const GObject& obj);

    virtual void initState (GOState* state);

    void updateRegion (bool recalcBBox = true);

public:
    enum Property { Prop_Outline, Prop_Fill };

    struct OutlineInfo {
        enum {
            Color = 1, Style = 2, Width = 4, Custom = 8, All = 15
        };
        unsigned int mask;   // indicates the valid fields of the structure
        QColor color;        // outline color
        Qt::PenStyle style;      // outline style
        float width;         // outline width
        // custom properties (depend on object type)
        float roundness;     // roundness for rectangles
        enum Shape
        {
            DefaultShape, ArcShape, PieShape
        };
        Shape shape;         // shape of ellipse
        int startArrowId,    // arrow heads (for lines and bezier curves)
            endArrowId;
    };

    struct FillInfo {
        enum { Color = 1, FillStyle = 2, Pattern = 4, GradientInfo = 8, All = 15 };
        enum Style {
            NoFill, SolidFill, PatternFill, TileFill, GradientFill
        };
        unsigned int mask;  // indicates the valid fields of the structure
        QColor color;       // fill color
        Qt::BrushStyle pattern; // pattern
        Style fstyle;       // fill style
        Gradient gradient;  // gradient info
    };

    static void setDefaultOutlineInfo (const OutlineInfo& oi);
    static void setDefaultFillInfo (const FillInfo& fi);
    static OutlineInfo getDefaultOutlineInfo ();
    static FillInfo getDefaultFillInfo ();

    /**
     * The destructor.
     */
    virtual ~GObject ();

    /**
     * Set the outline properties for the object.
     *
     * @param info The outline info structure.
     */
    void setOutlineInfo (const OutlineInfo& info);

    /**
     * Retrieve the outline properties of the object.
     *
     * @return A structure with all outline properties.
     */
    OutlineInfo getOutlineInfo () const;

    /**
     * Set the ouline color of the object.
     *
     * @param c The outline color.
     */
    void setOutlineColor (const QColor& c);

    /**
     * Set the style of the ouline.
     *
     * @param p The outline pen style.
     */
    void setOutlineStyle (Qt::PenStyle p);

    /**
     * Set the ouline width.
     *
     * @param width The width of the outline.
     */
    void setOutlineWidth (float width);

    void setOutlineShape (OutlineInfo::Shape s);

    /**
     * Retrieve the color of the object outline.
     *
     * @return The ouline color.
     */
    const QColor& getOutlineColor () const;

    /**
     * Retrieve the style of the ouline pen.
     *
     * @return The pen style.
     */
    Qt::PenStyle getOutlineStyle () const;

    /**
     * Retireve the width of the ouline pen.
     *
     * @return The outline width.
     */
    float getOutlineWidth () const;

    /**
     * Set the fill properties for the object.
     *
     * @param info The FillInfo structure with all properties.
     */
    void setFillInfo (const FillInfo& info);

    /**
     * Retrieve the fill properties of the object.
     *
     * @return The FillInfo properties.
     */
    FillInfo getFillInfo () const;

    /**
     * Set the fill color.
     *
     * @param c The color for filling the object.
     */
    void setFillColor (const QColor& c);

    /**
     * Set the fill style of the object.
     *
     * @param b the brush style for fillig the object.
     */
    void setFillStyle (FillInfo::Style s);

    void setFillGradient (const Gradient& g);

    void setFillPattern (Qt::BrushStyle b);

    /**
     * Retrieve the fill color.
     *
     * @return The color.
     */
    const QColor& getFillColor () const;

    /**
     * Retrieve the brush style of object filling.
     *
     * @return The brush style.
     */
    FillInfo::Style getFillStyle () const;

    const Gradient& getFillGradient () const;

    Qt::BrushStyle getFillPattern () const;

    /**
     * Retrieve the transformation matrix associated with the object.
     *
     * @return The current matrix.
     */
    const QWMatrix& matrix () const { return tMatrix; }

    /**
     * Initialize a temporary matrix for transformation from the values of
     * the transformation matrix. The temporary matrix is used by interactive
     * tools like the SelectionTool.
     */
    void initTmpMatrix ();

    /**
     * Transform the object according to the given matrix.
     *
     * @param m      The matrix for combining with the current transformation
     *               matrix.
     * @param update if true, the bounding box of the object is immediatly
     *               updated, otherwise not. This is usefull for a sequence of
     *               transformations in order to avoid flickering.
     */
    void transform (const QWMatrix& m, bool update = false);

    /**
     * Transform the object temporary according to the given matrix. The
     * transformation matrix is not modified.
     *
     * @param m      The matrix for combining with the current transformation
     *               matrix.
     * @param update if true, the bounding box of the object is immediatly
     *               updated, otherwise not.
     * @see
     */
    void ttransform (const QWMatrix& m, bool update = false);

    /**
     * Mark the object as selected or not selected.
     *
     * @param flag if true, the object is selected, otherwise the object will
     *             be unselected.
     */
    virtual void select (bool flag = true);

    /**
     * Retrieve the selection status of the object.
     *
     * @return true, if the object is selected, otherwise false.
     */
    bool isSelected () const { return sflag; }

    virtual bool isValid () { return true; }

    bool gradientFill () const {
        return fillInfo.fstyle == GObject::FillInfo::GradientFill;
    }

    /**
     * Retrieve the bounding box for the object.
     *
     * @return The rectangle with the bounding box parameters.
     */
    const Rect& boundingBox () const { return box; }
    virtual const Rect& redrawBox () const { return box; }

    void setLayer (GLayer* l);
    GLayer* getLayer () { return layer; }

    void writePropertiesToXml (QDomElement &element, QDomDocument &document);

    void ref ();
    void unref ();
    unsigned int ref_count () const { return rcount; }

    virtual QString typeName () const = 0;

    virtual GOState* saveState ();
    virtual void restoreState (GOState* state);

    virtual int getNeighbourPoint (const Coord& ) { return -1; }
    virtual void movePoint (int /*idx*/, float /*dx*/, float /*dy*/, bool /*ctrlPressed*/=false) {}
    virtual void removePoint (int , bool) {}

    /**
     * Draw the object with the given painter.
     * NOTE: This method has to be implemented in every subclass.
     *
     * @param p The Painter for drawing the object.
     * @param withBasePoints If true, draw the base points of the
     *                       object.
     */
    virtual void draw (QPainter& /*p*/, bool /*withBasePoints*/ = false,
                       bool /*outline*/ = false) {}

    /**
     * Test, if the object contains the given point.
     * NOTE: This method has to be implemented in every subclass.
     *
     * @param p  The coodinates of a point.
     * @return   true, if the object contains the point, otherwise false.
     */
    virtual bool contains (const Coord& p);

    virtual bool intersects (const Rect& r);

    /**
     * Create a new object with identical properties.
     * NOTE: This method has to be implemented in every subclass.
     *
     * @return A copy of this object.
     */
    virtual GObject* copy () = 0;
    virtual GObject* clone (const QDomElement &element) = 0;

    virtual QDomElement writeToXml (QDomDocument &document);

    /**
     * At the moment only valid for lines and bezier curves.
     */
    virtual bool findNearestPoint (const Coord& , float /*max_dist*/,
                                   float& /*dist*/, int& /*pidx*/, bool /*all*/ = false) {
        return false;
    }

    virtual void calcBoundingBox () = 0;

    virtual void getPath(QValueList<Coord>& /*path*/) {}

    virtual GCurve* convertToCurve () const { return 0L; }

    bool workInProgress () const { return inWork; }
    void setWorkInProgress (bool flag) { inWork = flag; }

    void invalidateClipRegion  ();

    // used for saving/restoring of object connections (like text on path)
    QString getId ();
    bool hasId () const { return ! id.isEmpty (); }

    QString getRefId () { return refid; }
    bool hasRefId () const { return !refid.isEmpty(); }

    static void registerPrototype (const QString &className, GObject *proto);
    static GObject* lookupPrototype (const QString &className);

signals:
    void deleted ();
    void changed ();
    void changed (const Rect& r);
    void propertiesChanged (GObject::Property p, int mask);

protected:
    void initBrush (QBrush& b);
    void initPen (QPen& p);

    void updateBoundingBox (const Rect& r);
    void updateBoundingBox (const Coord& p1, const Coord& p2);

    void calcUntransformedBoundingBox (const Coord& tleft, const Coord& tright,
                                       const Coord& bright, const Coord& bleft);

    bool sflag;              // object is selected
    Rect box;                // the bounding box
    QWMatrix tMatrix;        // transformation matrix
    QWMatrix tmpMatrix;      // temporary transformation matrix
    QWMatrix iMatrix;        // inverted transformation matrix
    OutlineInfo outlineInfo; // outline attributes
    FillInfo fillInfo;       // fill attributes
    unsigned int rcount;     // the reference counter
    GLayer *layer;           // the layer containing this object
    GradientShape gShape;    // the shape (clipped pixmap) for gradient fill
    bool inWork;             // the object is currently manipulated,
    // so don't fill it now

    QString id, refid;

    // default value
    static OutlineInfo defaultOutlineInfo;
    static FillInfo defaultFillInfo;

    static QDict<GObject> *prototypes;
};

class GOState {
    friend class GObject;
protected:
    GOState () : rcount (1) {}

public:
    // should be protected, but ...
    virtual ~GOState () {}

    void ref () { rcount++; }
    void unref () { if (--rcount == 0) delete this; }

private:
    QWMatrix matrix;
    GObject::OutlineInfo oInfo;
    GObject::FillInfo fInfo;
    unsigned int rcount;
};

// Fixes an --enable-final problem (Werner)
inline float seg_length (const Coord& c1, const Coord& c2) {
    float dx = c2.x () - c1.x ();
    float dy = c2.y () - c1.y ();
    return std::sqrt (dx * dx + dy * dy);
}

namespace KIllustrator {
    QDomElement createMatrixElement(const QString &tag, const QWMatrix &matrix, QDomDocument &document);
    QWMatrix toMatrix(const QDomElement &matrix);
    GObject *objectFactory(const QDomElement &element);
};

#endif
