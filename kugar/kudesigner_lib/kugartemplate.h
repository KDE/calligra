/* This file is part of the KDE project
 Copyright (C) 2003-2004 Alexander Dymo <adymo@mksat.net>

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
#ifndef KUGARTEMPLATE_H
#define KUGARTEMPLATE_H

#include <map>

#include "commdefs.h"
#include "section.h"

class QPainter;

namespace Kudesigner
{

class DetailHeader;
class DetailFooter;
class Detail;
class Band;
class ReportHeader;
class ReportFooter;
class PageHeader;
class PageFooter;
class Canvas;

typedef QPair< QPair<DetailHeader*, DetailFooter*>, Detail*> DetailBand;

class KugarTemplate: public Section
{
public:
    KugarTemplate( int x, int y, int width, int height, Canvas *canvas );
    ~KugarTemplate();

    virtual int rtti() const
    {
        return Rtti_KugarTemplate;
    }
    virtual void draw( QPainter &painter );

    void arrangeSections( bool destructive = true );
    void updatePaperProps();

    QString fileName() const
    {
        return m_reportFileName;
    }
    void setFileName( const QString &fName )
    {
        m_reportFileName = fName;
    }

    virtual QString getXml();

    void removeSection( Band *section, DetailHeader **header, DetailFooter **footer );
    bool removeReportItem( QCanvasItem *item );

    Band *band( Kudesigner::RttiValues type, int level = -1 );

    ReportHeader *reportHeader;
    ReportFooter *reportFooter;
    PageHeader *pageHeader;
    PageFooter *pageFooter;
    std::map<int, DetailBand> details;
    unsigned int detailsCount;

private:
    QString m_reportFileName;
};

}

#endif
