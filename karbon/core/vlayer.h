/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VLAYER_H__
#define __VLAYER_H__

#include <KoShapeContainer.h>
#include <koffice_export.h>

#include <QRectF>

class QDomElement;
class DCOPObject;
class KoStore;
class KoXmlWriter;
class KoGenStyles;
class KoOasisLoadingContext;
class VVisitor;

/**
 * VLayer manages a set of vobjects. It keeps the objects from bottom to top
 * in a list, ie. objects higher in the list are drawn above lower objects.
 * Objects in a layer can be manipulated and worked on independant of objects
 * in other layers.
 */

class KARBONBASE_EXPORT KoLayerShape : public KoShapeContainer
{
public:
	KoLayerShape();
    virtual void paintComponent(QPainter &, KoViewConverter &) {};
	virtual void saveOasis( KoStore *store, KoXmlWriter *docWriter, KoGenStyles &mainStyles, int &index ) const;
	virtual bool loadOasis( const QDomElement &element, KoOasisLoadingContext &context );
	void load( const QDomElement& element );
	void accept( VVisitor& visitor );

    bool hitTest( const QPointF &position ) const;
private:
    class LayerMembers: public KoGraphicsContainerModel {
        public:
            LayerMembers();
            ~LayerMembers();
            void add(KoShape *child);
            void setClipping(const KoShape *child, bool clipping);
            bool childClipped(const KoShape *child) const;
            void remove(KoShape *child);
            int count() const;
            QList<KoShape*> iterator() const;
            void containerChanged(KoShapeContainer *container);

        private: // members
            QList <KoShape *> m_layerMembers;
    };
};

#endif

