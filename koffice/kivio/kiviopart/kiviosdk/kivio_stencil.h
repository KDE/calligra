/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2003 theKompany.com & Dave Marotti,
 *                         Peter Simonsson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef KIVIO_STENCIL_H
#define KIVIO_STENCIL_H

#include <qfont.h>
#include <qcolor.h>
#include <qdom.h>
#include <qptrlist.h>
#include <qbitarray.h>
#include <KoPoint.h>
#include <kglobalsettings.h>
#include <KoRect.h>
#include <koffice_export.h>

#include "kivio_line_style.h"

class KivioCustomDragData;
class KivioConnectorPoint;
class KivioConnectorTarget;
class KivioFillStyle;
class KivioIntraStencilData;
class KivioPage;
class KoPoint;
class KoRect;
class KivioStencilSpawner;
class KivioStencil;
class KivioStencilIface;

// If a custom draggable is clicked, an ID must be returned in the range of kctCustom to MAXINT
typedef enum
{
    kctNone = 0,    // No collision
    kctBody,        // The body of the stencil was clicked
    kctCustom=500  // A custom draggable portion was clicked.
} KivioCollisionType;

typedef enum
{
    kpX = 0,
    kpY,
    kpWidth,
    kpHeight,
    kpAspect,
    kpDeletion
} KivioProtection;
#define NUM_PROTECTIONS 6

typedef enum
{
    krhpNone=0x0000,
    krhpNW=0x0001,
    krhpN=0x0002,
    krhpNE=0x0004,
    krhpE=0x0008,
    krhpSE=0x0010,
    krhpS=0x0020,
    krhpSW=0x0040,
    krhpW=0x0080,
    krhpC=0x0100
} KivioResizeHandlePosition;

typedef enum
{
  kstNormal = 0,
  kstConnector,
  kstGroup,
  kstText
} KivioStencilType;

#define KIVIO_RESIZE_HANDLE_POSITION_ALL \
    ( krhpNW    \
      | krhpN   \
      | krhpNE  \
      | krhpE   \
      | krhpSE  \
      | krhpS   \
      | krhpSW  \
      | krhpW   \
      | krhpC )

#define KIVIO_RESIZE_HANDLE_POSITION_BORDER \
    ( krhpNW    \
      | krhpN   \
      | krhpNE  \
      | krhpE   \
      | krhpSE  \
      | krhpS   \
      | krhpSW  \
      | krhpW )

#define KIVIO_RESIZE_HANDLE_POSITION_CORNERS \
    ( krhpNW    \
      | krhpNE  \
      | krhpSE  \
      | krhpSW )



class KIVIO_EXPORT KivioStencil
{
  protected:
    // Dimensions, size
    double m_x, m_y, m_w, m_h;

    // Rotation
    int m_rotation;
    KoPoint m_pinPoint;

    // The spawner that created this stencil
    KivioStencilSpawner *m_pSpawner;

    // The protection bits of the stencil
    QBitArray *m_pProtection;
    QBitArray *m_pCanProtect;

    // The stemcil type
    KivioStencilType m_type;
    bool m_connected;

    // Indicates if this stencil is selected
    bool m_selected;
    
    // Indicates if the stencil is hidden
    bool m_hidden;

  protected:
    void rotatePainter(KivioIntraStencilData *);
    KoRect calculateBoundingBox();

  public:
    KivioStencil();
    virtual ~KivioStencil();

    virtual KivioStencil *duplicate() { return NULL; }

    virtual KivioStencilType type() { return m_type; }
    virtual void setType(KivioStencilType t) { m_type = t; }
    virtual bool connected() { return m_connected; }
    virtual void setConnected(bool c) { m_connected = c; }

    virtual double x() { return m_x; }
    virtual void setX( double f ) { m_x=f; updateGeometry(); }

    virtual double y() { return m_y; }
    virtual void setY( double f ) { m_y=f; updateGeometry(); }

    virtual double w() { return m_w; }
    virtual void setW( double f ) { if( f > 0 ) { m_w=f;  updateGeometry(); } }

    virtual double h() { return m_h; }
    virtual void setH( double f ) { if( f > 0 ) { m_h=f;  updateGeometry(); } }
    
    virtual void move(double xOffset, double yOffset);

    virtual KoRect rect();

    virtual void setPosition( double f1, double f2 ) { m_x=f1; m_y=f2;  updateGeometry(); }
    virtual void setDimensions( double f1, double f2 ) { m_w=f1; m_h=f2;  updateGeometry(); }

    virtual QColor fgColor() { return QColor(0,0,0); }
    virtual void setFGColor( QColor ) { ; }

    virtual void setBGColor( QColor ) { ; }
    virtual QColor bgColor() { return QColor(0,0,0); }
    virtual void setFillPattern(int) { ; }
    virtual int fillPattern() { return 1; }

    virtual KivioFillStyle *fillStyle() { return NULL; }
    virtual KivioLineStyle lineStyle() { return KivioLineStyle(); }
    virtual void setLineStyle(KivioLineStyle) { ; }

    virtual void setLineWidth( double ) { ; }
    virtual double lineWidth() { return 1.0f; }

    virtual void setLinePattern(int) { ; }
    virtual int linePattern() { return 1; }

    // FOnt stuff
    virtual QColor textColor() { return QColor(0,0,0); }
    virtual void setTextColor( QColor ) {;}

    virtual QFont textFont() { return KGlobalSettings::generalFont(); }
    virtual void setTextFont( const QFont & ) {;}

    virtual int hTextAlign() { return -1; }
    virtual int vTextAlign() { return -1; }

    virtual void setHTextAlign(int) { ; }
    virtual void setVTextAlign(int) { ; }

    virtual void setText( const QString & ) { ; }
    virtual QString text() { return QString(""); }

    virtual void setRotation(int d);
    virtual int rotation() { return m_rotation; }

    virtual KivioStencilSpawner *spawner() { return m_pSpawner; }
    virtual void setSpawner( KivioStencilSpawner *s ) { m_pSpawner=s; }

    virtual void paint( KivioIntraStencilData * );
    virtual void paintOutline( KivioIntraStencilData * );
    virtual void paintConnectorTargets( KivioIntraStencilData * );
    virtual void paintSelectionHandles( KivioIntraStencilData * );

    virtual KivioCollisionType checkForCollision( KoPoint *, double );
    virtual void customDrag( KivioCustomDragData * );


    virtual bool loadXML( const QDomElement & );
    virtual QDomElement saveXML( QDomDocument & );

    virtual bool isSelected() { return m_selected; }
    virtual void select() { m_selected = true; }
    virtual void unselect() { m_selected = false; }
    virtual void subSelect( const double &, const double & ) { ; }

    virtual QBitArray *protection() { return m_pProtection; }
    virtual QBitArray *canProtect() { return m_pCanProtect; }

    virtual void addToGroup( KivioStencil * );
    virtual QPtrList<KivioStencil>* groupList() { return NULL; }

    virtual void updateGeometry();
    virtual void updateConnectorPoints(KivioConnectorPoint *, double oldX, double oldY);

    // This attempts to connect based on position
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, double );

    // This attempts to connect based on a targetID.  This should  ***ONLY*** be used
    // right after a load
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, int );

    virtual KoPoint snapToTarget( const KoPoint& p, double /*thresh*/, bool& hit );

    virtual void searchForConnections( KivioPage * );
    virtual void searchForConnections( KivioPage *pPage, double threshold );

    virtual int generateIds( int );

    virtual int resizeHandlePositions() { return krhpNone; }
    
    /**
      Returns the name of the text box that is at the specified point.
      If there exist no text box at the specified point then it returns QString::null.
    */
    virtual QString getTextBoxName(const KoPoint& /*p*/) { return QString::null; }

    virtual void setText(const QString& /*text*/, const QString& /*name*/) { ; }
    virtual QString text(const QString& /*name*/) { return QString(""); }

    /// Returns true if the stencil has a text box
    virtual bool hasTextBox() const { return false; }

    virtual QColor textColor(const QString& /*textBoxName*/) { return QColor(); }
    virtual void setTextColor(const QString& /*textBoxName*/, const QColor& /*color*/) {}

    virtual QFont textFont(const QString& /*textBoxName*/) { return KGlobalSettings::generalFont(); }
    virtual void setTextFont(const QString& /*textBoxName*/, const QFont& /*font*/) {}

    virtual int hTextAlign(const QString& /*textBoxName*/) { return -1; }
    virtual int vTextAlign(const QString& /*textBoxName*/) { return -1; }

    virtual void setHTextAlign(const QString& /*textBoxName*/, int /*align*/) {}
    virtual void setVTextAlign(const QString& /*textBoxName*/, int /*align*/) {}

    virtual void setPinPoint(const KoPoint& p) { m_pinPoint = p; }
    virtual KoPoint pinPoint() const { return m_pinPoint; }
    
    virtual void addConnectorTarget(const KoPoint&) {}
    virtual void removeConnectorTarget(const KoPoint&) {}
    
    virtual bool isInRect(const KoRect& rect);
    
    virtual void setHidden(bool hide) { m_hidden = hide; }
    virtual bool hidden() { return m_hidden; }

    virtual void setCustomIDPoint(int, const KoPoint&, KivioPage*) {}
    virtual KoPoint customIDPoint(int) { return KoPoint(); }

    /****** ARROW HEAD STUFF *******/
    virtual void setStartAHType( int ) { ; }
    virtual void setStartAHWidth( double ) { ; }
    virtual void setStartAHLength( double ) { ; }
    virtual void setEndAHType( int ) { ; }
    virtual void setEndAHWidth( double ) { ; }
    virtual void setEndAHLength( double ) { ; }

    virtual int startAHType() { return 0; }
    virtual double startAHWidth() { return 0.0f; }
    virtual double startAHLength() { return 0.0f; }
    virtual int endAHType() { return 0; }
    virtual double endAHWidth() { return 0.0f; }
    virtual double endAHLength() { return 0.0f; }

  private:
    KivioStencilIface *iface;
};

#endif
