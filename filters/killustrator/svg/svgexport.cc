/* This file is part of the KDE project
   Copyright (C) 2000 Kai-Uwe Sattler <kus@iti.cs.uni-magdeburg.de>
   Copyright (C) 2001 Rob Buis <rwlbuis@wanadoo.nl>
   
   
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
#include <svgexport.h>

#include <GDocument.h>
#include "GPage.h"
#include <GPolygon.h>
#include <GPolyline.h>
#include <GOval.h>
#include <GCurve.h>
#include <GBezier.h>
#include <GGroup.h>
#include <GText.h>
#include <GPixmap.h>
#include <GLayer.h>

#include <qdom.h>
#include <qstring.h>
#include <qfile.h>
#include <kdebug.h>

#include <KIllustrator_doc.h>


SVGExport::SVGExport( KoFilter *parent, const char *name ) :
                     KoFilter( parent, name ) {
}


SVGExport::~SVGExport( ) {
}


// The reason why we use the KoDocument* approach and not the QDomDocument
// approach is because we don't want to export formulas but values !
bool SVGExport::filterExport( const QString &file, KoDocument * docu,
                              const QString &from, const QString &to,
                              const QString & ) {
    if( strcmp( docu->className(), "KIllustratorDocument" ) != 0 )  // it's safer that way :)
    {
        kdWarning(30501) << "document isn't a KIllustratorDocument but a " << docu->className() << endl;
        return false;
    }
    if( to != "image/x-svg" || from!="application/x-killustrator" )
    {
        kdWarning(30501) << "Invalid mimetypes " << to << " " << from << endl;
        return false;
    }

    KIllustratorDocument *kidoc=(KIllustratorDocument *) docu;
    GDocument *doc = kidoc->gdoc();
    QDomDocument document( "svg" );
    //document.doctype().setPublicId("svg PUBLIC \"-//W3C//DTD SVG December 1999//EN\"\n\"http:://www.w3.org/Graphics/SVG/SVG-19991203.dtd\"");

    QDomElement svg=document.createElement( "svg" );
    svg.setAttribute( "width", doc->activePage()->getPaperWidth() );
    svg.setAttribute( "height", doc->activePage()->getPaperHeight() );
    document.appendChild( svg );

    // contents
    for( QListIterator<GLayer> li(doc->activePage()->getLayers() );
         li.current(); ++li) {
        if( (*li)->isInternal () )
            continue;

        const QList<GObject>& contents = (*li)->objects();
        for (QListIterator<GObject> oi( contents ); oi.current(); ++oi)
            svg.appendChild( exportObject ( document, *oi ) );
    }
    QFile out( file );
    if( !out.open( IO_WriteOnly ) ) {
        kdError( 38000 ) << "Could not open the output file: " << file << endl;
        return false;
    }
    QTextStream s( &out );
    document.save( s, 4 );
    out.close();

    return true;
}


QDomDocumentFragment SVGExport::exportObject( QDomDocument &document, GObject* obj ) {

    if( obj->isA ("GPolygon") )
        return exportPolygon (document, (GPolygon *) obj);
    else if( obj->isA ("GPolyline") )
        return exportPolyline (document, (GPolyline *) obj);
    else if( obj->isA ("GText") )
        return exportText (document, (GText *) obj);
    else if( obj->isA ("GOval") )
        return exportEllipse (document, (GOval *) obj);
    else if( obj->isA ("GBezier") )
        return exportBezier (document, (GBezier *) obj);
    else if( obj->isA ("GGroup") )
        return exportGroup (document, (GGroup *) obj);
    else if( obj->isA ("GCurve") )
        return exportCurve (document, (GCurve *) obj);
    else if( obj->isA ("GPixmap") )
        return exportPixmap (document, (GPixmap *) obj);
    return QDomDocumentFragment();
}

QDomDocumentFragment SVGExport::exportText (QDomDocument &document, GText* obj) {

    QDomDocumentFragment textlines=document.createDocumentFragment();
    if (obj->lines () > 1) {
        GText::TextInfo tInfo = obj->getTextInfo ();
        QFontMetrics fm (tInfo.font);
        float yoff = fm.ascent ();

        for (int i = 0; i < obj->lines (); i++) {
            int ws = fm.width (obj->line (i));
            float xoff = 0;
            if (tInfo.align == GText::TextInfo::AlignCenter)
                xoff = -ws / 2;
            else if (tInfo.align == GText::TextInfo::AlignRight)
                xoff = -ws;
            textlines.appendChild(exportTextLine (document, obj, i, xoff, yoff));
            yoff += fm.height ();
        }
    }
    else
        textlines.appendChild(exportTextLine (document, obj, 0, 0, 0));
    return textlines;
}

QDomDocumentFragment SVGExport::exportTextLine (QDomDocument &document, GText* obj, int line,
                                       float xoff, float yoff) {

    QDomDocumentFragment fragment=document.createDocumentFragment();
    QDomElement element=document.createElement("text");
    element.setAttribute ("x", xoff);
    element.setAttribute ("y", yoff);
    addTransformationAttribute (element, obj);
    addTextStyleAttribute (element, obj);
    element.appendChild(document.createTextNode(obj->line(line)));
    fragment.appendChild(element);
    return fragment;
}

QDomDocumentFragment SVGExport::exportBezier (QDomDocument &document, GBezier* obj) {

    QDomDocumentFragment fragment=document.createDocumentFragment();
    QDomElement element=document.createElement("path");
    QString s;
    Coord p = obj->getPoint (1);
    s = "M " + QString::number(p.x()) + " " + QString::number(p.y()) + " ";
    unsigned int i = 2;
    while (i < obj->numOfPoints () - 2) {
        s += "C ";
        for (int n = 0; n < 3; n++) {
            p = obj->getPoint (i + n);
            s += QString::number(p.x()) + " " + QString::number(p.y()) + " ";
        }
        i += 3;
    }
    element.setAttribute("d", s);
    addTransformationAttribute (element, obj);
    addStyleAttribute (element, obj);
    fragment.appendChild(element);
    return fragment;
}

QDomDocumentFragment SVGExport::exportCurve (QDomDocument &document, GCurve* obj) {

    QDomDocumentFragment fragment=document.createDocumentFragment();
    QDomElement element=document.createElement("path");
    QString s;
    bool first = true;
    for (int i = 0; i < obj->numOfSegments (); i++) {
        const GSegment& seg = obj->getSegment (i);
        if (first) {
            s += "M " + QString::number(seg.pointAt(0).x ()) + " " +
                 QString::number(seg.pointAt (0).y()) + " ";
            first = false;
        }
        if (seg.kind () == GSegment::sk_Line) {
            s += "L " + QString::number(seg.pointAt (1).x()) + " " +
                 QString::number(seg.pointAt (1).y()) + " ";
        }
        else if (seg.kind () == GSegment::sk_Bezier) {
            s += "C ";
            for (int n = 1; n < 4; n++)
                s += QString::number(seg.pointAt (n).x()) + " " +
                     QString::number(seg.pointAt (n).y()) + " ";
        }
    }
    if (obj->isClosed ())
        s += "Z";

    element.setAttribute("d", s);
    addTransformationAttribute (element, obj);
    addStyleAttribute (element, obj);
    fragment.appendChild(element);
    return fragment;
}

QDomDocumentFragment SVGExport::exportGroup (QDomDocument &document, GGroup* obj) {

    QDomDocumentFragment fragment=document.createDocumentFragment();
    QDomElement element=document.createElement("g");
    addTransformationAttribute(element, obj);
    addStyleAttribute(element, obj);

    const QList<GObject>& objs = obj->getMembers ();
    for (QListIterator<GObject> i(objs); i.current(); ++i)
        element.appendChild(exportObject (document, *i));
    fragment.appendChild(element);
    return fragment;
}

QDomDocumentFragment SVGExport::exportPolyline (QDomDocument &document, GPolyline* obj) {

    QDomDocumentFragment fragment=document.createDocumentFragment();
    QDomElement element=document.createElement("polyline");
    QString s;
    for (unsigned int i = 0; i < obj->numOfPoints (); i++) {
        Coord p = obj->getPoint (i);
        s += QString::number(p.x()) + "," + QString::number(p.y()) + " ";
    }
    element.setAttribute("points", s);
    addTransformationAttribute (element, obj);
    addStyleAttribute (element, obj);
    fragment.appendChild(element);
    return fragment;
}

QDomDocumentFragment SVGExport::exportEllipse (QDomDocument &document, GOval* obj) {

    QDomDocumentFragment fragment=document.createDocumentFragment();
    QDomElement element;
    const Coord& p0 = obj->startPoint ();
    const Coord& p1 = obj->endPoint ();
    if (obj->isCircle ()) {
        element=document.createElement("circle");
        element.setAttribute ("cx", (p1.x () + p0.x ()) / 2.0);
        element.setAttribute ("cy", (p1.y () + p0.y ()) / 2.0);
        element.setAttribute ("r", (p1.x () - p0.x ()) / 2.0);
        addTransformationAttribute (element, obj);
        addStyleAttribute (element, obj);
    }
    else {
        GObject::OutlineInfo oInfo = obj->getOutlineInfo ();
        if (oInfo.shape == GObject::OutlineInfo::DefaultShape) {
            element=document.createElement("ellipse");
            element.setAttribute ("cx", (p1.x () + p0.x ()) / 2.0);
            element.setAttribute ("cy", (p1.y () + p0.y ()) / 2.0);
            element.setAttribute ("rx", (p1.x () - p0.x ()) / 2.0);
            element.setAttribute ("ry", (p1.y () - p0.y ()) / 2.0);
            addTransformationAttribute (element, obj);
            addStyleAttribute (element, obj);
        }
        else if (oInfo.shape == GObject::OutlineInfo::ArcShape) {
            // TODO
        }
        else {
            // TODO
        }
    }
    fragment.appendChild(element);
    return fragment;
}

QDomDocumentFragment SVGExport::exportPolygon (QDomDocument &document, GPolygon* obj) {

    QDomDocumentFragment fragment=document.createDocumentFragment();
    QDomElement element;
    if (obj->isRectangle ()) {
        element=document.createElement("rect");
        Coord p0 = obj->getPoint (0);
        Coord p2 = obj->getPoint (2);
        //    const QWMatrix& m = obj->matrix ();
        element.setAttribute ("x", p0.x ());
        element.setAttribute ("y", p0.y ());
        element.setAttribute ("width", p2.x () - p0.x ());
        element.setAttribute ("height", p2.y () - p0.y ());
        addTransformationAttribute (element, obj);
        addStyleAttribute (element, obj);
    }
    else {
        element=document.createElement("polygon");
        QString s;
        for (unsigned int i = 0; i < obj->numOfPoints (); i++) {
            Coord p = obj->getPoint (i);
            s += QString::number(p.x()) + "," + QString::number(p.y()) + " ";
        }
        element.setAttribute ("points", s);
        addTransformationAttribute (element, obj);
        addStyleAttribute (element, obj);
    }
    fragment.appendChild(element);
    return fragment;
}

QDomDocumentFragment SVGExport::exportPixmap (QDomDocument &/*document*/, GPixmap* /*obj*/) {
    return QDomDocumentFragment();
}

void SVGExport::addTransformationAttribute (QDomElement &element, GObject* obj) {

    QString s;
    QWMatrix im;

    const QWMatrix& m = obj->matrix ();
    if (m != im) {
        s += "matrix(" + QString::number(m.m11()) + " " + QString::number(m.m12()) + " " +
             QString::number(m.m21()) + " " + QString::number(m.m22()) + " " + QString::number(m.dx()) +
             " " + QString::number(m.dy()) + ")";
        element.setAttribute ("transform", s);
    }
}

void SVGExport::addStyleAttribute (QDomElement &element, GObject* obj) {

    QString s;
    GObject::FillInfo fInfo = obj->getFillInfo ();
    GObject::OutlineInfo oInfo = obj->getOutlineInfo ();

    s += "fill:";
    if (fInfo.fstyle == GObject::FillInfo::NoFill || obj->isA ("GPolyline") ||
        (obj->isA ("GBezier") && ! ((GBezier *) obj)->isClosed ()) ||
        (obj->isA ("GCurve") && ! ((GCurve *) obj)->isClosed ()))
        s += "none ; ";
    else if (fInfo.fstyle == GObject::FillInfo::SolidFill) {
        s += "rgb(" + QString::number(fInfo.color.red()) + "," +
             QString::number(fInfo.color.green()) + "," + QString::number(fInfo.color.blue()) + ") ; ";
    }
    s += "stroke:";
    if (oInfo.style == Qt::NoPen)
        s += "none ; ";
    else
        s += "rgb(" + QString::number(oInfo.color.red()) + "," +
             QString::number(oInfo.color.green()) + "," + QString::number(oInfo.color.blue()) + ") ; ";
    s += "stroke-width:" + QString::number(oInfo.width);
    element.setAttribute ("style", s);
}

void SVGExport::addTextStyleAttribute (QDomElement &element, GText* obj) {

    QString s;
    GObject::OutlineInfo oInfo = obj->getOutlineInfo ();
    GText::TextInfo tInfo = obj->getTextInfo ();

    s += "font-family:" + tInfo.font.family ();
    s += "; font-size:" + QString::number(tInfo.font.pointSize ());
    s += "; fill:";
    if (oInfo.style == Qt::NoPen)
        s += "none" ;
    else {
        s += "rgb(" + QString::number(oInfo.color.red()) + "," + QString::number(oInfo.color.green()) +
             + "," + QString::number(oInfo.color.blue()) + ")";
    }
    element.setAttribute ("style", s);
}

#include <svgexport.moc>
