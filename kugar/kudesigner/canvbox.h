/***************************************************************************
                          canvbox.h  -  description
                             -------------------
    begin                : 07.06.2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef CANVBOX_H
#define CANVBOX_H

#include <map.h>
#include <qcanvas.h>
#include "canvdefs.h"

class QString;
class QStringList;
class CanvasDetailHeader;
class CanvasDetailFooter;
class CanvasDetail;
class CanvasReportHeader;
class CanvasReportFooter;
class CanvasPageHeader;
class CanvasPageFooter;

class CanvasBox: public QCanvasRectangle{
public:
    CanvasBox(int x, int y, int width, int height, QCanvas * canvas):
	    QCanvasRectangle(x, y, width, height, canvas) {}
    
    virtual int rtti() const { return RttiCanvasBox; }
    virtual void draw(QPainter &painter);
    void scale(int scale);
    virtual QString getXml() { return ""; }
    
    map<QString, pair<QString, QStringList> > props;
    /*
      properties format
      map<key_name, pair<key_value, key_description> >
      key_description << key_hint -- to display helpful tips
                      << key_type (string, int, int_from_list, color, font)
                         -- to organize appropriate input
		      << probably_key_value << ... -- values
    */
};

class CanvasSection: public CanvasBox{
public:
    CanvasSection(int x, int y, int width, int height, QCanvas * canvas):
	    CanvasBox(x, y, width, height, canvas) {}
    virtual QString getXml() { return ""; }
    virtual void draw(QPainter &painter);
    virtual int rtti() const { return RttiCanvasSection; }
};

class CanvasBand: public CanvasSection{
public:
    CanvasBand(int x, int y, int width, int height, QCanvas * canvas):
	    CanvasSection(x, y, width, height, canvas)
    {
	setZ(10);
    }
    ~CanvasBand();
    virtual void draw(QPainter &painter);
    virtual int rtti() const { return RttiCanvasBand; }
    virtual QString getXml();
    void arrange(int base, bool destructive = TRUE);
    
    QCanvasItemList items;
};

typedef pair< pair<CanvasDetailHeader*, CanvasDetailFooter*>, CanvasDetail*>  DetailBand;

class CanvasKugarTemplate: public CanvasSection{
public:
    CanvasKugarTemplate(int x, int y, int width, int height, QCanvas * canvas);
    ~CanvasKugarTemplate();
    virtual int rtti() const { return RttiKugarTemplate; }
    virtual void draw(QPainter &painter);
    void arrangeSections(bool destructive=TRUE);
    
    QString fileName() const { return reportFileName; }
    void setFileName(QString fName) { reportFileName = fName; }
    
    virtual QString getXml();
    
    void removeSection(CanvasBand *section, CanvasDetailHeader **header,
		       CanvasDetailFooter **footer);
    
    CanvasReportHeader *reportHeader;
    CanvasReportFooter *reportFooter;
    CanvasPageHeader *pageHeader;
    CanvasPageFooter *pageFooter;
    map<int, DetailBand> details;
    unsigned int detailsCount;
private:
    QString reportFileName;
};

class CanvasReportHeader: public CanvasBand{
public:
    CanvasReportHeader(int x, int y, int width, int height, QCanvas * canvas);
    virtual int rtti() const { return RttiReportHeader; }
    virtual void draw(QPainter &painter);
    virtual QString getXml();
};

class CanvasReportFooter: public CanvasBand{
public:
    CanvasReportFooter(int x, int y, int width, int height, QCanvas * canvas);
    virtual int rtti() const { return RttiReportFooter; }
    virtual void draw(QPainter &painter);
    virtual QString getXml();
};

class CanvasPageHeader: public CanvasBand{
public:
    CanvasPageHeader(int x, int y, int width, int height, QCanvas * canvas);
    virtual void draw(QPainter &painter);
    virtual int rtti() const { return RttiPageHeader; }
    virtual QString getXml();
};

class CanvasPageFooter: public CanvasBand{
public:
    CanvasPageFooter(int x, int y, int width, int height, QCanvas * canvas);
    virtual int rtti() const { return RttiPageFooter; }
    virtual void draw(QPainter &painter);
    virtual QString getXml();
};

class CanvasDetailHeader: public CanvasBand{
public:
    CanvasDetailHeader(int x, int y, int width, int height, QCanvas * canvas);
    virtual int rtti() const { return RttiDetailHeader; }
    virtual void draw(QPainter &painter);
    virtual QString getXml();
};

class CanvasDetail: public CanvasBand{
public:
    CanvasDetail(int x, int y, int width, int height, QCanvas * canvas);
    virtual int rtti() const { return RttiDetail; }
    virtual void draw(QPainter &painter);
    virtual QString getXml();
};

class CanvasDetailFooter: public CanvasBand{
public:
    CanvasDetailFooter(int x, int y, int width, int height, QCanvas * canvas);
    virtual int rtti() const { return RttiDetailFooter; }
    virtual void draw(QPainter &painter);
    virtual QString getXml();
};

#endif
