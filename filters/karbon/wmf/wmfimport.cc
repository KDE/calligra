/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

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

DESCRIPTION
*/

#include <config.h>
#include <kdebug.h>
#include <koStoreDevice.h>
#include <koFilterChain.h>
#include <kgenericfactory.h>
#include <qpointarray.h>
#include <wmfimport.h>

typedef KGenericFactory<WMFImport, KoFilter> WMFImportFactory;
K_EXPORT_COMPONENT_FACTORY( libwmfimport, WMFImportFactory( "wmfimport" ) );

const int WMFImport::s_area = 38000;

WMFImport::WMFImport(
    KoFilter *,
    const char *,
    const QStringList&) :
        KoFilter(), KWmf(75.0 )/// (2.5 *1.4))
{
}

WMFImport::~WMFImport()
{
}

KoFilter::ConversionStatus WMFImport::convert( const QCString& from, const QCString& to )
{
	if( to != "application/x-karbon" || from != "image/x-wmf" )
		return KoFilter::NotImplemented;

	// doc header
	m_text = "";
	m_text += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	m_text += "<!DOCTYPE DOC>\n";
	m_text += "<DOC mime=\"application/x-karbon\" syntaxVersion=\"0.1\" editor=\"WMF import filter\">\n";
	m_text += "  <LAYER name=\"Layer\" visible=\"1\">\n";

	// parse body
	if( !parse( m_chain->inputFile() ) )
		return KoFilter::WrongFormat;
	// close doc
	m_text += "  </LAYER>\n";
	m_text += "</DOC>\n";

	KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
	if(!out)
	{
		kdError(s_area) << "Unable to open output stream" << endl;
		return KoFilter::StorageCreationError;
	}
	//kdDebug(s_area) << "m_text : " << m_text.latin1() << endl;
	QCString cstring = m_text.latin1();
	out->writeBlock( (const char*)cstring, cstring.size() - 1 );
	return KoFilter::OK;
}

void WMFImport::gotEllipse(
    const DrawContext &/*dc*/,
    QString /*type*/,
    QPoint /*topLeft*/,
    QSize /*halfAxes*/,
    unsigned /*startAngle*/,
    unsigned /*stopAngle*/)
{
    //m_text += "<ellipse angle1=\"" + QString::number(startAngle) +
     //           "\" angle2=\"" + QString::number(stopAngle) +
     //           "\" x=\"" + QString::number(topLeft.x()) +
//                "\" y=\"" + QString::number(topLeft.y()) +
//                "\" kind=\"" + type +
//                "\" rx=\"" + QString::number(halfAxes.width()) +
//                "\" ry=\"" + QString::number(halfAxes.height()) +
//                "\">\n";
}

void toRGB(int c, double &r, double &g, double &b)
{
	r = (c >> 16) / 255.0;
	g = ((c >> 8) & 0xFF) / 255.0;
	b = (c & 0xFF) / 255.0;
}

void WMFImport::gotPolygon(
    const DrawContext &dc,
    const QPointArray &points)
{
	kdDebug(s_area) << "WMFImport::gotPolygon" << endl;
	kdDebug(s_area) << QString::number(dc.m_penWidth, 16) << endl;
	kdDebug(s_area) << dc.m_penStyle << endl;
    m_text += "<COMPOSITE>\n";
	if( dc.m_penWidth > 0 )
	{
    	m_text += "<STROKE lineWidth=\"1\">\n";// + QString::number(dc.m_penWidth, 16) + "\">\n";
		double r, g, b;
		toRGB(dc.m_penColour, r, g, b);
		m_text += "<COLOR v1=\"" + QString::number(r) + "\" v2=\"" + QString::number(g) + "\"  v3=\"" + QString::number(b) + "\" opacity=\"1\" colorSpace=\"0\"  />\n";
	m_text += "</STROKE>\n";
	}
	else
		m_text += "<STROKE lineWidth=\"1\" />\n";
	m_text += "<FILL fillRule=\"" + QString::number(dc.m_winding) + "\">\n";
	double r, g, b;
	toRGB(dc.m_brushColour, r, g, b);
	m_text += "<COLOR v1=\"" + QString::number(r) + "\" v2=\"" + QString::number(g) + "\"  v3=\"" + QString::number(b) + "\" opacity=\"1\" colorSpace=\"0\"  />\n";
	m_text += "</FILL>\n";

    m_text += "<PATH isClosed=\"1\" >\n";
    pointArray(points);
    m_text += "</PATH>\n";
    m_text += "</COMPOSITE>\n";
}


void WMFImport::gotPolyline(
    const DrawContext &dc,
    const QPointArray &points)
{
	kdDebug(s_area) << "WMFImport::gotPolyline" << endl;
	return;
    m_text += "<COMPOSITE>\n";
    m_text += "<STROKE lineWidth=\"" + QString::number(dc.m_penWidth) + "\">\n";
    m_text += "</STROKE>\n";
    m_text += "<PATH isClosed=\"1\" >\n";
    pointArray(points);
    m_text += "</PATH>\n";
    m_text += "</COMPOSITE>\n";
}

void WMFImport::gotRectangle(
    const DrawContext &/*dc*/,
    const QPointArray &/*points*/)
{
	kdDebug(s_area) << "WMFImport::gotRectangle" << endl;
    //QRect bounds = points.boundingRect();
}

void WMFImport::pointArray(
    const QPointArray &points)
{

    m_text += "<MOVE x=\"" + QString::number(points.point(0).x()) +
                "\" y=\"" + QString::number(points.point(0).y()) +
                 "\" />\n";
	kdDebug(s_area) << "\n<MOVE x=\"" + QString::number(points.point(0).x()) +
			                "\" y=\"" + QString::number(points.point(0).y()) +
						                 "\" />" << endl;
    for (unsigned i = 1; i < points.count(); i++)
    {
        m_text += "<LINE x=\"" + QString::number(points.point(i).x()) +
                    "\" y=\"" + QString::number(points.point(i).y()) +
                     "\" />\n";
	kdDebug(s_area) << "<LINE x=\"" + QString::number(points.point(i).x()) +
			                "\" y=\"" + QString::number(points.point(i).y()) +
							                 "\" />" << endl;
    }

}

#include <wmfimport.moc>
