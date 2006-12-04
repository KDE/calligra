/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KIVIOLAYER_H
#define KIVIOLAYER_H

#include <QList>

#include <KoShapeContainer.h>
#include <KoShapeControllerBase.h>

class KivioAbstractPage;
class KoShape;

class KivioLayer : public KoShapeControllerBase
{
    public:
        KivioLayer(const QString& title, KivioAbstractPage* page);
        ~KivioLayer();

        /// Set the title to @p newTitle
        void setTitle(const QString& newTitle);
        /// Get layer title
        QString title() const;

        /// Add @p shape to layer (will also update the views)
        virtual void addShape(KoShape* shape);
        /// Remove @p shape from layer (will also update the views)
        virtual void removeShape(KoShape* shape);

        /// @return shapes contained in the layer
        QList<KoShape*> shapes() const;

    private:
        QString m_title;
        KivioAbstractPage* m_page;
        QList<KoShape*> m_shapes;
};

#endif // KIVIOLAYER_H
