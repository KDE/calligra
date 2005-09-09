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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KIVIO_LAYER_H
#define KIVIO_LAYER_H

#include <qdom.h>
#include <qvaluelist.h>
#include <qobject.h>
#include <koPoint.h>
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

namespace Kivio {
  class Object;
}

class KivioLayer
{
  protected:
    friend class KivioGroupStencil;

    int m_flags;
    QValueList<Kivio::Object*> m_objectList;
    QString m_name;
    KivioPage *m_pPage;
    DCOPObject* m_dcop;

    Kivio::Object *loadSMLStencil( const QDomElement & );
    Kivio::Object *loadGroupStencil( const QDomElement & );
    Kivio::Object *loadPluginStencil( const QDomElement & );

  public:
    KivioLayer( KivioPage * );
    virtual ~KivioLayer();

    KivioPage *page()const{return m_pPage;}

    virtual DCOPObject* dcopObject();

    QValueList<Kivio::Object*>* objectList() { return &m_objectList; }

    bool visible() { return (m_flags & FLOW_LAYER_VISIBLE); }
    void setVisible( bool f );

    bool connectable() { return (m_flags & FLOW_LAYER_CONNECTABLE); }
    void setConnectable( bool f );

    QString name() const { return m_name; }
    void setName( const QString &n ) { m_name = n; }

    bool addStencil( Kivio::Object * );
    bool removeStencil( Kivio::Object * );
    Kivio::Object* takeStencil( Kivio::Object * );

    bool loadXML( const QDomElement & );
    void loadOasis(const QDomElement& layer);
    QDomElement saveXML( QDomDocument & );
    void saveOasis(KoXmlWriter* layerWriter);

    Kivio::Object* checkForStencil( KoPoint *, int *, float, bool );
    Kivio::Object* checkForCollision(const KoPoint& point, int& collisionType);

    void printContent( QPainter& painter, int xdpi = 0, int ydpi = 0 );
    void paintContent( QPainter& painter, const QRect& rect, bool transparent, QPoint p0,
      KoZoomHandler* zoom );
    void paintConnectorTargets( QPainter& painter, const QRect& rect, bool transparent,
      QPoint p0, KoZoomHandler* zoom );
    void paintSelectionHandles( QPainter& painter, const QRect& rect, bool transparent,
      QPoint p0, KoZoomHandler* zoom );

    KivioConnectorTarget *connectPointToTarget( KivioConnectorPoint *, float );

    KoPoint snapToTarget( const KoPoint& p, double thresh, bool& hit );

    int generateStencilIds( int );

    void searchForConnections( KivioPage * );
};

#endif
