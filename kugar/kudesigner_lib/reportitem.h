/* This file is part of the KDE project
 Copyright (C) 2002-2004 Alexander Dymo <adymo@mksat.net>

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
#ifndef REPORTITEM_H
#define REPORTITEM_H

#include "commdefs.h"
#include "box.h"

class QString;
class QStringList;

namespace Kudesigner
{

class Band;

class ReportItem: public Box
{
public:
    ReportItem( int x, int y, int width, int height, Canvas *canvas )
            : Box( x, y, width, height, canvas )
    {
        setZ( 20 );
        parentSection = 0;
    }
    virtual int rtti() const
    {
        return Rtti_ReportItem;
    }

    virtual void updateGeomProps();
    virtual void draw( QPainter &painter )
    {
        Box::draw( painter );
    }
    virtual QString getXml();

    QRect topLeftResizableRect();
    QRect bottomLeftResizableRect();
    QRect topRightResizableRect();
    QRect bottomRightResizableRect();
    QRect topMiddleResizableRect();
    QRect bottomMiddleResizableRect();
    QRect leftMiddleResizableRect();
    QRect rightMiddleResizableRect();

    virtual Band *section();
    virtual void setSection( Band *section );
    virtual void setSectionUndestructive( Band *section );

    virtual int isInHolder( const QPoint p );
    virtual void drawHolders( QPainter &painter );
    virtual void fastProperty()
    { }
    ;
    bool intersects( ReportItem *item );
    QString escape( QString string );

protected:
    friend class View;
    Band *parentSection;
};

}

#endif
