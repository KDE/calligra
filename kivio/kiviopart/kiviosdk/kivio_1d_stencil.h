#ifndef _KIVIO_1D_STENCIL_H
#define _KIVIO_1D_STENCIL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qdom.h>
#include <qlist.h>

#include "kivio_stencil.h"
#include "kivio_text_style.h"

class KivioArrowHead;
class KivioConnectorPoint;
class KivioCustomDragData;
class KivioFillStyle;
class KivioLineStyle;
class KivioIntraStencilData;
class KivioStencil;


class Kivio1DStencil : public KivioStencil
{
protected:
    QList <KivioConnectorPoint> *m_pConnectorPoints;
    
    KivioConnectorPoint *m_pStart, *m_pEnd;
    KivioConnectorPoint *m_pLeft, *m_pRight;
    KivioConnectorPoint *m_pTextConn;

    float m_connectorWidth;
    bool m_needsWidth;
    bool m_needsText;

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
    void drawText( KivioIntraStencilData * );

public:
    Kivio1DStencil();
    virtual ~Kivio1DStencil();

    // fg color functions
    virtual void setFGColor( QColor c );
    virtual QColor fgColor();


    // bg color functions
    virtual void setBGColor( QColor c );
    virtual QColor bgColor();

    virtual void setText( const QString & t ) { m_pTextStyle->setText(t); }
    virtual QString text() { return m_pTextStyle->text(); }

    virtual void setTextColor( QColor c ) { m_pTextStyle->setColor(c); }
    virtual QColor textColor() { return m_pTextStyle->color(); }

    virtual QFont textFont() { return m_pTextStyle->font(); }
    virtual void setTextFont( const QFont &f ) { m_pTextStyle->setFont(f); }

    virtual int hTextAlign() { return m_pTextStyle->hTextAlign(); }
    virtual int vTextAlign() { return m_pTextStyle->vTextAlign(); }

    virtual void setHTextAlign(int i) { m_pTextStyle->setHTextAlign(i); }
    virtual void setVTextAlign(int i) { m_pTextStyle->setVTextAlign(i); }


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
