/* This file is part of the KDE project
   Copyright (C) 2003 Alexander Dymo <cloudtemple@mksat.net>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef CANVKUTEMPLATE_H
#define CANVKUTEMPLATE_H

#include <map>

#include "canvdefs.h"
#include "canvsection.h"

class CanvasDetailHeader;
class CanvasDetailFooter;
class CanvasDetail;
class CanvasBand;
class CanvasReportHeader;
class CanvasReportFooter;
class CanvasPageHeader;
class CanvasPageFooter;
class QCanvas;
class QPainter;

typedef std::pair< std::pair<CanvasDetailHeader*, CanvasDetailFooter*>, CanvasDetail*>  DetailBand;

class CanvasKugarTemplate: public CanvasSection{
public:
    CanvasKugarTemplate(int x, int y, int width, int height, QCanvas * canvas);
    ~CanvasKugarTemplate();
    virtual int rtti() const { return KuDesignerRttiKugarTemplate; }
    virtual void draw(QPainter &painter);
    void arrangeSections(bool destructive=TRUE);
    void updatePaperProps();

    QString fileName() const { return reportFileName; }
    void setFileName(const QString &fName) { reportFileName = fName; }

    virtual QString getXml();

    void removeSection(CanvasBand *section, CanvasDetailHeader **header,
        CanvasDetailFooter **footer);
    bool removeSection(CanvasBand *section);

    CanvasReportHeader *reportHeader;
    CanvasReportFooter *reportFooter;
    CanvasPageHeader *pageHeader;
    CanvasPageFooter *pageFooter;
    std::map<int, DetailBand> details;
    unsigned int detailsCount;
private:
    QString reportFileName;
};

#endif
