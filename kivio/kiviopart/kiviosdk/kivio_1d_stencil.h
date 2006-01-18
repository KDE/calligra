#ifndef _KIVIO_1D_STENCIL_H
#define _KIVIO_1D_STENCIL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qdom.h>
#include <qptrlist.h>

#include <koffice_export.h>

#include "kivio_stencil.h"
#include "kivio_text_style.h"

class KivioArrowHead;
class KivioConnectorPoint;
class KivioCustomDragData;
class KivioFillStyle;
class KivioLineStyle;
class KivioIntraStencilData;
class KivioStencil;


class KIVIO_EXPORT Kivio1DStencil : public KivioStencil
{
protected:
    QPtrList <KivioConnectorPoint> *m_pConnectorPoints;

    KivioConnectorPoint *m_pStart, *m_pEnd;
    KivioConnectorPoint *m_pLeft, *m_pRight;
    KivioConnectorPoint *m_pTextConn;

    double m_connectorWidth;
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

    virtual bool connected();

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
    virtual void setLineWidth( double f );
    virtual double lineWidth();

    virtual void setLinePattern(int p);
    virtual int linePattern();

    virtual void setFillPattern(int p);
    virtual int fillPattern();

    // position functions
    virtual void setX( double );
    virtual void setY( double );
    virtual void setPosition( double, double );


    // required for connector tool
    virtual void setStartPoint( double, double );
    virtual void setEndPoint( double, double );


    // painting functions
    virtual void paint( KivioIntraStencilData * );
    virtual void paintOutline( KivioIntraStencilData * );
    virtual void paintConnectorTargets( KivioIntraStencilData * );
    virtual void paintSelectionHandles( KivioIntraStencilData * );


    // collision detection
    virtual KivioCollisionType checkForCollision( KoPoint *, double );


    // custom dragging
    virtual void customDrag( KivioCustomDragData * );


    // geometry management
    virtual void updateGeometry();
    virtual void updateConnectorPoints( KivioConnectorPoint *, double, double );


    // file i/o routines
    virtual bool loadXML( const QDomElement & );
    virtual QDomElement saveXML( QDomDocument & );


    // duplication routines
    virtual KivioStencil* duplicate();


    // connection search routines
    virtual void searchForConnections( KivioPage * );
    virtual void searchForConnections( KivioPage *pPage, double threshold );


    // returns which resize handles are valid
    virtual int resizeHandlePositions();

    virtual void disconnectFromTargets();
    virtual KivioLineStyle lineStyle();
    virtual void setLineStyle(KivioLineStyle ls);

    virtual void setCustomIDPoint(int customID, const KoPoint& point, KivioPage* page);
    virtual KoPoint customIDPoint(int customID);
};


#endif
