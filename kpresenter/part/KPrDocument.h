/* This file is part of the KDE project
   Copyright (C) 2006-2007 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef KPRDOCUMENT_H
#define KPRDOCUMENT_H

#include <QObject>

#include <KoPADocument.h>

class KPrDocument : public KoPADocument
{
    Q_OBJECT
public:
    explicit KPrDocument( QWidget* parentWidget, QObject* parent, bool singleViewMode = false );
    ~KPrDocument();

    void paintContent( QPainter &painter, const QRect &rect, bool transparent = false,
                       double zoomX = 1.0, double zoomY = 1.0 );

    bool loadXML( QIODevice *, const KoXmlDocument & doc );

protected:
    KoView * createViewInstance( QWidget *parent );
    const char *odfTagName();
};

#endif /* KPRDOCUMENT_H */
