#ifndef _KIVIO_1D_STENCIL_H
#define _KIVIO_1D_STENCIL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qdom.h>
#include <qlist.h>

#include "kivio_stencil.h"

class KivioArrowHead;
class KivioConnectorPoint;
class KivioCustomDragData;
class KivioFillStyle;
class KivioLineStyle;
class KivioIntraStencilData;
class KivioStencil;
class KivioTextStyle;

class Kivio1DStencil : public KivioStencil
{
protected:
    QList <KivioConnectorPoint> *m_pConnectorPoints;
    
    KivioConnectorPoint *m_pStart, *m_pEnd;
    KivioConnectorPoint *m_pLeft, *m_pRight;


    float m_connectorWidth;
    bool m_needsWidth;

    KivioFillStyle *m_pFillStyle;
    KivioLineStyle *m_pLineStyle;
    KivioTextStyle *m_pTextStyle;

    // Utility routines
    bool boolAllTrue( bool *, int );
    bool boolContainsFalse( bool *, int );
    
    virtual QDomElement createRootElement( QDomDocument & );

    virtual bool saveCustom( QDomElement &, QDomDocument & );
    QDomElement saveConnectors( QDomDocument &doc );
    QDomElement saveProperties( QDomDocument &doc );

    virtual bool loadCustom( const QDomElement & );
    bool loadConnectors( const QDomElement & );
    bool loadProperties( const QDomElement & );

    // For easier duplication
    void copyBasicInto( Kivio1DStencil * );

public:
    Kivio1DStencil();
    virtual ~Kivio1DStencil();

    // fg color functions
    virtual void setFGColor( QColor c );
    virtual QColor fgColor();


    // bg color functions
    virtual void setBGColor( QColor c );
    virtual QColor bgColor();


    // line width functions
    virtual void setLineWidth( float f );
    virtual float lineWidth();


    // position functions
    virtual void setX( float );
    virtual void setY( float );
    virtual void setPosition( float, float );


    // required for connector tool
    virtual void setStartPoint( float, float );
    virtual void setEndPoint( float, float );


    // painting functions
    virtual void paint( KivioIntraStencilData * );
    virtual void paintOutline( KivioIntraStencilData * );
    virtual void paintConnectorTargets( KivioIntraStencilData * );
    virtual void paintSelectionHandles( KivioIntraStencilData * );


    // collision detection
    virtual KivioCollisionType checkForCollision( KivioPoint *, float );


    // custom dragging
    virtual void customDrag( KivioCustomDragData * );


    // geometry management
    virtual void updateGeometry();
    virtual void updateConnectorPoints( KivioConnectorPoint *, float, float );


    // file i/o routines
    virtual bool loadXML( const QDomElement & );
    virtual QDomElement saveXML( QDomDocument & );


    // duplication routines
    virtual KivioStencil* duplicate();


    // connection search routines
    virtual void searchForConnections( KivioPage * );


    // returns which resize handles are valid
    virtual int resizeHandlePositions();

};


#endif
