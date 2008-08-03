/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */

/*
 *     This file contains all the Report Entity classes. Each one is a
 * derivative of ReportEntity, which in turn is derived from QCanvasItem.
 */

#ifndef __REPORTENTITIES_H__
#define __REPORTENTITIES_H__

// qt
#include <QGraphicsItem>
// common
#include <parsexmlutils.h>


// forward declarations
class ReportWindow;

class QDomNode;
class QDomDocument;
class QDomElement;
class ReportDesigner;

namespace KoProperty
{
class Editor;
}

//
// ReportEntity
//
class ReportEntity
{
public:
    static void buildXML(QGraphicsItem * item, QDomDocument & doc, QDomElement & parent);
    virtual void buildXML(QDomDocument & doc, QDomElement & parent) = 0;

    static void buildXMLRect(QDomDocument & doc, QDomElement & entity, QRectF rect);
    static void buildXMLFont(QDomDocument & doc, QDomElement & entity, QFont font);
    static void buildXMLTextStyle(QDomDocument & doc, QDomElement & entity, ORTextStyleData ts);
    static void buildXMLLineStyle(QDomDocument & doc, QDomElement & entity, ORLineStyleData ls);

    static QFont getDefaultEntityFont();
    static void  setDefaultEntityFont(const QFont &);

    virtual ReportEntity* clone() = 0;
    ReportDesigner* designer() const {
        return _rd;
    }
    void setDesigner(ReportDesigner* rd) {
        _rd = rd;
    }
    virtual ~ReportEntity() {};
protected:
    ReportEntity(ReportDesigner*);

    ReportDesigner* _rd;



private:
    static bool _readDefaultFont;
    static QFont _defaultFont;
};

#if 0
//
// ReportEntityGraph
//
class ReportEntityGraph : public Q3CanvasRectangle, public ReportEntity
{
public:
    ReportEntityGraph(ReportWindow *, Q3Canvas * canvas);
    ReportEntityGraph(QDomNode & element, ReportWindow *, Q3Canvas * canvas);
    ReportEntityGraph(ORGraphData &, ReportWindow *, Q3Canvas *);

    virtual ~ReportEntityGraph();

    void copyData(ORGraphData &);

    void setQuery(const QString &);
    QString query();

    virtual void propertyDialog(QMap<QString, QColor>*, QuerySourceList * qsl, QWidget * parent);
    virtual void buildXML(QDomDocument & doc, QDomElement & parent);

    virtual int rtti() const;
    static int RTTI;
protected:
    virtual void drawShape(QPainter&);

private:
    ReportWindow * _rw;
    ORGraphData _graphData;

};
#endif
#endif

