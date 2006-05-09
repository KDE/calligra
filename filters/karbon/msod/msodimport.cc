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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

DESCRIPTION
*/

#include <kdebug.h>
#include <ktempfile.h>
#include <kmimetype.h>
#include <kgenericfactory.h>
#include <KoFilterChain.h>
#include <qfile.h>
//Added by qt3to4:
#include <Q3CString>
#include <msodimport.h>
#include <q3pointarray.h>

typedef KGenericFactory<MSODImport> MSODImportFactory;
K_EXPORT_COMPONENT_FACTORY( libmsodimport, MSODImportFactory( "kofficefilters" ) )

const int MSODImport::s_area = 30505;

MSODImport::MSODImport(
    KoFilter *,
    const char *,
    const QStringList&) :
        KoEmbeddingFilter(), Msod(100)
{
}

MSODImport::~MSODImport()
{
}

KoFilter::ConversionStatus MSODImport::convert( const QByteArray& from, const QByteArray& to )
{
    if (to != "application/x-karbon" || from != "image/x-msod")
        return KoFilter::NotImplemented;

    // Get configuration data: the shape id, and any delay stream that we were given.
    unsigned shapeId;
    emit commSignalShapeID( shapeId );
    const char *delayStream = 0L;
    emit commSignalDelayStream( delayStream );
    kDebug( s_area ) << "##################################################################" << endl;
    kDebug( s_area ) << "shape id: " << shapeId << endl;
    kDebug( s_area ) << "delay stream: " << delayStream << endl;
    kDebug( s_area ) << "##################################################################" << endl;
/*
    QString config = ""; // ###### FIXME: We aren't able to pass config data right now
    QStringList args = QStringList::split(";", config);
    unsigned i;

    kDebug(s_area) << "MSODImport::filter: config: " << config << endl;
    for (i = 0; i < args.count(); i++)
    {
        if (args[i].startsWith("shape-id="))
        {
            shapeId = args[i].mid(9).toUInt();
        }
        else
        if (args[i].startsWith("delay-stream="))
        {
            delayStream = (const char *)args[i].mid(13).toULong();
        }
        else
        {
            kError(s_area) << "Invalid argument: " << args[i] << endl;
            return KoFilter::StupidError;
        }
    }
*/
    // doc header
    m_text = "";
    m_text += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    m_text += "<!DOCTYPE DOC>\n";
    m_text += "<DOC mime=\"application/x-karbon\" syntaxVersion=\"0.1\" editor=\"WMF import filter\">\n";
    m_text += "  <LAYER name=\"Layer\" visible=\"1\">\n";

    if (!parse(shapeId, m_chain->inputFile(), delayStream))
        return KoFilter::WrongFormat;

    // close doc
    m_text += "  </LAYER>\n";
    m_text += "</DOC>\n";

    emit sigProgress(100);

    KoStoreDevice* dev = m_chain->storageFile( "root", KoStore::Write );
    if (!dev)
    {
        kError(s_area) << "Cannot open output file" << endl;
        return KoFilter::StorageCreationError;
    }
    Q3CString cstring ( m_text.utf8() );
    dev->write(cstring.data(), cstring.size()-1);

    return KoFilter::OK;
}

void MSODImport::gotEllipse(
    const DrawContext &/*dc*/,
    QString /*type*/,
    QPoint /*topLeft*/,
    QSize /*halfAxes*/,
    unsigned /*startAngle*/,
    unsigned /*stopAngle*/)
{
// ### TODO
#if 0
    m_text += "<ellipse angle1=\"" + QString::number(startAngle) +
                "\" angle2=\"" + QString::number(stopAngle) +
                "\" x=\"" + QString::number(topLeft.x()) +
                "\" y=\"" + QString::number(topLeft.y()) +
                "\" kind=\"" + type +
                "\" rx=\"" + QString::number(halfAxes.width()) +
                "\" ry=\"" + QString::number(halfAxes.height()) +
                "\">\n";
    m_text += " <gobject fillcolor=\"#" + QString::number(dc.m_brushColour, 16) +
                "\" fillstyle=\"" + QString::number(1 /*m_winding*/) +
                "\" linewidth=\"" + QString::number(dc.m_penWidth) +
                "\" strokecolor=\"#" + QString::number(dc.m_penColour, 16) +
                "\" strokestyle=\"" + QString::number(dc.m_penStyle) +
                "\">\n";
    m_text += "  <matrix dx=\"0\" dy=\"0\" m21=\"0\" m22=\"1\" m11=\"1\" m12=\"0\"/>\n";
    m_text += " </gobject>\n";
    m_text += "</ellipse>\n";
#endif
}

static void toRGB(int c, double &r, double &g, double &b)
{
	r = (c >> 16) / 255.0;
	g = ((c >> 8) & 0xFF) / 255.0;
	b = (c & 0xFF) / 255.0;
}

void MSODImport::gotPicture(
    unsigned key,
    QString extension,
    unsigned length,
    const char *data)
{
// ### TODO
#if 0
    kDebug() << "##########################################MSODImport::gotPicture" << endl;
    kDebug() << "MSODImport::gotPicture -- " << extension << endl;
    if ((extension == "wmf") ||
        (extension == "emf") ||
        (extension == "pict"))
    {
        int partRef = internalPartReference( QString::number( key ) );

        if (partRef == -1)
        {
            m_embeddeeData = data;
            m_embeddeeLength = length;

            QString srcMime( KoEmbeddingFilter::mimeTypeByExtension( extension ) );
            if ( srcMime == KMimeType::defaultMimeType() )
                kWarning( s_area ) << "Couldn't determine the mimetype from the extension" << endl;

            Q3CString destMime; // intentionally empty, the filter manager will do the rest
            KoFilter::ConversionStatus status;
            partRef = embedPart( srcMime.latin1(), destMime, status, QString::number( key ) );

            m_embeddeeData = 0;
            m_embeddeeLength = 0;

            if ( status != KoFilter::OK ) {
                kWarning(s_area) << "Couldn't convert the image!" << endl;
                return;
            }
        }
        m_text += "<object url=\"" + QString::number( partRef ) + "\" mime=\"";
        m_text += internalPartMimeType( QString::number( key ) );
        m_text += "\" x=\"0\" y=\"0\" width=\"100\" height=\"200\"/>\n";
    }
    else
    {
        // We could not import it as a part. Try as an image.
        KTempFile tempFile( QString::null, '.' + extension );
        tempFile.file()->write( data, length );
        tempFile.close();

        m_text += "<pixmap src=\"" + tempFile.name() + "\">\n"
                    " <gobject fillstyle=\"0\" linewidth=\"1\" strokecolor=\"#000000\" strokestyle=\"1\">\n"
                    "  <matrix dx=\"0\" dy=\"0\" m21=\"0\" m22=\"1\" m11=\"1\" m12=\"0\"/>\n"
                    " </gobject>\n"
                    "</pixmap>\n";

        // Note that we cannot delete the file...
    }
#endif
}

void MSODImport::gotPolygon(
    const DrawContext &dc,
    const Q3PointArray &points)
{
    kDebug(s_area) << "MSODImport::gotPolygon" << endl;
    kDebug(s_area) << QString::number(dc.m_penWidth, 16) << endl;
    kDebug(s_area) << dc.m_penStyle << endl;
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


void MSODImport::gotPolyline(
    const DrawContext &dc,
    const Q3PointArray &points)
{
	kDebug(s_area) << "MSODImport::gotPolyline" << endl;
	return; // ### TODO
    m_text += "<COMPOSITE>\n";
    m_text += "<STROKE lineWidth=\"" + QString::number(dc.m_penWidth) + "\">\n";
    m_text += "</STROKE>\n";
    m_text += "<PATH isClosed=\"1\" >\n";
    pointArray(points);
    m_text += "</PATH>\n";
    m_text += "</COMPOSITE>\n";
}

void MSODImport::gotRectangle(
    const DrawContext &dc,
    const Q3PointArray &points)
{
// ### TODO
#if 0
    QRect bounds = points.boundingRect();

    m_text += "<rectangle width=\"" + QString::number(bounds.width()) +
                "\" x=\"" + QString::number(bounds.x()) +
                "\" y=\"" + QString::number(bounds.y()) +
                "\" height=\"" + QString::number(bounds.height()) +
                "\" rounding=\"0\">\n";
    m_text += "<polyline arrow1=\"0\" arrow2=\"0\">\n";
    pointArray(points);
    m_text += " <gobject fillcolor=\"#" + QString::number(dc.m_brushColour, 16) +
                "\" fillstyle=\"" + QString::number(1 /*m_winding*/) +
                "\" linewidth=\"" + QString::number(dc.m_penWidth) +
                "\" strokecolor=\"#" + QString::number(dc.m_penColour, 16) +
                "\" strokestyle=\"" + QString::number(dc.m_penStyle) +
                "\">\n";
    m_text += "  <matrix dx=\"0\" dy=\"0\" m21=\"0\" m22=\"1\" m11=\"1\" m12=\"0\"/>\n";
    m_text += " </gobject>\n";
    m_text += "</polyline>\n";
    m_text += "</rectangle>\n";
#endif
}

void MSODImport::savePartContents( QIODevice* file )
{
    if ( m_embeddeeData != 0 && m_embeddeeLength != 0 )
        file->write( m_embeddeeData, m_embeddeeLength );
}

void MSODImport::pointArray(
    const Q3PointArray &points)
{

    m_text += "<MOVE x=\"" + QString::number(points.point(0).x()) +
                "\" y=\"" + QString::number(points.point(0).y()) +
                "\" />\n";
    kDebug(s_area) << "\n<MOVE x=\"" + QString::number(points.point(0).x()) +
                            "\" y=\"" + QString::number(points.point(0).y()) +
                                        "\" />" << endl;
    for (unsigned int i = 1; i < points.count(); i++)
    {
        m_text += "<LINE x=\"" + QString::number(points.point(i).x()) +
                    "\" y=\"" + QString::number(points.point(i).y()) +
                    "\" />\n";
        kDebug(s_area) << "<LINE x=\"" + QString::number(points.point(i).x()) +
                            "\" y=\"" + QString::number(points.point(i).y()) +
                                            "\" />" << endl;
    }

}

#include <msodimport.moc>
