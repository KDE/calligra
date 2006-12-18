/* This file is part of the KDE project
   Copyright (C) 2006 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef KPRPAGE_H
#define KPRPAGE_H

#include <QList>

#include <KoPageLayout.h>
#include <KoShapeControllerBase.h>

class KoShape;
class KPrDocument;

class KPrPage : public KoShapeControllerBase
{
public:    
    KPrPage(KPrDocument *_doc);
    ~KPrPage();

    virtual void addShape( KoShape * shape );
    virtual void removeShape( KoShape *shape );

    KoPageLayout & pageLayout() { return m_pageLayout; }

    /// @return all shapes.
    QList<KoShape*> shapes() const;

protected:
    QList<KoShape *> m_shapes;
    KoPageLayout m_pageLayout;
    KPrDocument *m_doc;
};

#endif /* KPRPAGE_H */
