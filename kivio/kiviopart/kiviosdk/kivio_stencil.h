/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef KIVIO_STENCIL_H
#define KIVIO_STENCIL_H

#include <qfont.h>
#include <qcolor.h>
#include <qdom.h>
#include <qlist.h>
#include <qbitarray.h>

class KivioCustomDragData;
class KivioConnectorPoint;
class KivioConnectorTarget;
class KivioFillStyle;
class KivioIntraStencilData;
class KivioLineStyle;
class KivioPage;
class KivioPoint;
class KivioRect;
class KivioStencilSpawner;
class KivioStencil;

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



class KivioStencil
{
protected:
    // Dimensions, size
    float m_x, m_y, m_w, m_h;    
    
    // The spawner that created this stencil
    KivioStencilSpawner *m_pSpawner;    
    

    // Indicates if this stencil is selected
    bool m_selected;

    // The protection bits of the stencil
    QBitArray *m_pProtection;
    QBitArray *m_pCanProtect;
    
public:
    KivioStencil();
    virtual ~KivioStencil();

    virtual KivioStencil *duplicate() { return NULL; }
    
    
    virtual float x() { return m_x; }
    virtual void setX( float f ) { m_x=f; updateGeometry(); }
    
    virtual float y() { return m_y; }
    virtual void setY( float f ) { m_y=f; updateGeometry(); }
    
    virtual float w() { return m_w; }
    virtual void setW( float f ) { if( f > 0 ) { m_w=f;  updateGeometry(); } }
    
    virtual float h() { return m_h; }
    virtual void setH( float f ) { if( f > 0 ) { m_h=f;  updateGeometry(); } }

    virtual KivioRect rect();

    virtual void setPosition( float f1, float f2 ) { m_x=f1; m_y=f2;  updateGeometry(); }
    virtual void setDimensions( float f1, float f2 ) { m_w=f1; m_h=f2;  updateGeometry(); }

    virtual QColor fgColor() { return QColor(0,0,0); }
    virtual void setFGColor( QColor ) { ; }

    virtual void setBGColor( QColor ) { ; }
    virtual QColor bgColor() { return QColor(0,0,0); }

    virtual KivioFillStyle *fillStyle() { return NULL; }

    virtual void setLineWidth( float ) { ; }
    virtual float lineWidth() { return 1.0f; }

    // FOnt stuff
    virtual QColor textColor() { return QColor(0,0,0); }
    virtual void setTextColor( QColor ) {;}

    virtual QFont textFont() { return QFont("times",12); }
    virtual void setTextFont( const QFont & ) {;}

    virtual int hTextAlign() { return -1; }
    virtual int vTextAlign() { return -1; }

    virtual void setHTextAlign(int) { ; }
    virtual void setVTextAlign(int) { ; }

    virtual void setText( const QString & ) { ; }
    virtual QString text() { return QString(""); }


    virtual KivioStencilSpawner *spawner() { return m_pSpawner; }
    virtual void setSpawner( KivioStencilSpawner *s ) { m_pSpawner=s; }

    virtual void paint( KivioIntraStencilData * );
    virtual void paintOutline( KivioIntraStencilData * );
    virtual void paintConnectorTargets( KivioIntraStencilData * );
    virtual void paintSelectionHandles( KivioIntraStencilData * );

    virtual KivioCollisionType checkForCollision( KivioPoint *, float );
    virtual void customDrag( KivioCustomDragData * );


    virtual bool loadXML( const QDomElement & );
    virtual QDomElement saveXML( QDomDocument & );

    virtual bool isSelected() { return m_selected; }
    virtual void select() { m_selected = true; }
    virtual void unselect() { m_selected = false; }
    virtual void subSelect( const float &, const float & ) { ; }

    virtual QBitArray *protection() { return m_pProtection; }
    virtual QBitArray *canProtect() { return m_pCanProtect; }

    virtual void addToGroup( KivioStencil * );
    virtual QList<KivioStencil>* groupList() { return NULL; }

    virtual void updateGeometry();
    virtual void updateConnectorPoints(KivioConnectorPoint *, float oldX, float oldY);

    // This attempts to connect based on position
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, float );
    
    // This attempts to connect based on a targetID.  This should  ***ONLY*** be used
    // right after a load
    virtual KivioConnectorTarget *connectToTarget( KivioConnectorPoint *, int );
    
    virtual void searchForConnections( KivioPage * );

    virtual int generateIds( int );

    virtual int resizeHandlePositions() { return krhpNone; }


    /****** ARROW HEAD STUFF *******/
    virtual void setStartAHType( int ) { ; }
    virtual void setStartAHWidth( float ) { ; }
    virtual void setStartAHLength( float ) { ; }
    virtual void setEndAHType( int ) { ; }
    virtual void setEndAHWidth( float ) { ; }
    virtual void setEndAHLength( float ) { ; }

    virtual int startAHType() { return 0; }
    virtual float startAHWidth() { return 0.0f; }
    virtual float startAHLength() { return 0.0f; }
    virtual int endAHType() { return 0; }
    virtual float endAHWidth() { return 0.0f; }
    virtual float endAHLength() { return 0.0f; }

};

#endif


