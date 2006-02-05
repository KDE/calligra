/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
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
#ifndef KIVIO_LAYER_H
#define KIVIO_LAYER_H

#include <qdom.h>
#include <qptrlist.h>
#include <qobject.h>
#include <KoPoint.h>
class KivioConnectorPoint;
class KivioConnectorTarget;
class KivioPage;
class KivioPainter;
class KoPoint;
class DCOPObject;
class KoZoomHandler;
class QDomElement;
class KoStore;
class KoXmlWriter;

#include "kivio_stencil.h"

#define FLOW_LAYER_VISIBLE 0x0001
#define FLOW_LAYER_CONNECTABLE 0x0002
#define FLOW_LAYER_NOT_EDITABLE 0x0004
#define FLOW_LAYER_NOT_PRINTABLE 0x0008

class KivioLayer
{
protected:
    friend class KivioGroupStencil;

    int m_flags;
    QPtrList <KivioStencil> *m_pStencilList;
    QPtrList <KivioStencil> *m_pDeletedStencilList;
    QString m_name;
    KivioPage *m_pPage;
    DCOPObject* m_dcop;

    KivioStencil *loadSMLStencil( const QDomElement & );
    KivioStencil *loadGroupStencil( const QDomElement & );
    KivioStencil *loadPluginStencil( const QDomElement & );


public:
    KivioLayer( KivioPage * );
    virtual ~KivioLayer();

    KivioPage *page()const{return m_pPage;}

    virtual DCOPObject* dcopObject();

    QPtrList<KivioStencil> *stencilList() { return m_pStencilList; }

    bool visible() { return (m_flags & FLOW_LAYER_VISIBLE); }
    void setVisible( bool f );

    bool connectable() { return (m_flags & FLOW_LAYER_CONNECTABLE); }
    void setConnectable( bool f );

    void setEditable(bool f);
    bool editable() { return !(m_flags & FLOW_LAYER_NOT_EDITABLE); }

    void setPrintable(bool f);
    bool printable() { return !(m_flags & FLOW_LAYER_NOT_PRINTABLE); }

    QString name() const { return m_name; }
    void setName( const QString &n ) { m_name = QString(n); }

    bool addStencil( KivioStencil * );
    bool removeStencil( KivioStencil * );

    bool loadXML( const QDomElement & );
    void loadOasis(const QDomElement& layer);
    QDomElement saveXML( QDomDocument & );
    void saveOasis(KoXmlWriter* layerWriter);

    KivioStencil *checkForStencil( KoPoint *, int *, float, bool );

    void printContent( KivioPainter& painter, int xdpi = 0, int ydpi = 0 );
    void printContent(KivioPainter& painter, KoZoomHandler* zoomHandler);
    void paintContent( KivioPainter& painter, const QRect& rect, bool transparent, QPoint p0,
      KoZoomHandler* zoom );
    void paintConnectorTargets( KivioPainter& painter, const QRect& rect, bool transparent,
      QPoint p0, KoZoomHandler* zoom );
    void paintSelectionHandles( KivioPainter& painter, const QRect& rect, bool transparent,
      QPoint p0, KoZoomHandler* zoom );

    KivioStencil *firstStencil() { return m_pStencilList->first(); }
    KivioStencil *nextStencil() { return m_pStencilList->next(); }
    KivioStencil *prevStencil() { return m_pStencilList->prev(); }
    KivioStencil *takeStencil() { return m_pStencilList->take(); }
    KivioStencil *currentStencil() { return m_pStencilList->current(); }
    KivioStencil *lastStencil() { return m_pStencilList->last(); }

    KivioStencil *takeStencil( KivioStencil * );

    KivioConnectorTarget *connectPointToTarget( KivioConnectorPoint *, float );

    KoPoint snapToTarget( const KoPoint& p, double thresh, bool& hit );

    int generateStencilIds( int );

    void searchForConnections( KivioPage * );
    void takeStencilFromList(  KivioStencil *pStencil );
    void insertStencil( KivioStencil *pStencil );

};

#endif


