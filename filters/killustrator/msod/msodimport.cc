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
#include <koStore.h>
#include <msodimport.h>
#include <msodimport.moc>
#include <qpointarray.h>

MSODImport::MSODImport(
    KoFilter *parent,
    const char *name) :
        KoFilter(parent, name), Msod(100)
{
}

MSODImport::~MSODImport()
{
}

const bool MSODImport::filter(
    const QString &fileIn,
    const QString &fileOut,
    const QString &from,
    const QString &to,
    const QString &config)
{
    if (to != "application/x-killustrator" || from != "image/x-msod")
        return false;

    // Get configuration data: the shape id, and any delay stream that we were given.

    unsigned shapeId = (unsigned)-1;
    const char *delayStream = 0L;
    QStringList args = QStringList::split(";", config);
    unsigned i;

    kdDebug(s_area) << "MSODImport::filter: config: " << config << endl;
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
            kdError(s_area) << "Invalid argument: " << args[i] << endl;
            return false;
        }
    }

    m_text = "";
    m_text += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    m_text += "<!DOCTYPE killustrator>\n";
    m_text += "<killustrator mime=\"application/x-killustrator\" version=\"2\" editor=\"MSOD import filter\">\n";
    m_text += "<head>\n";
    m_text += "<layout width=\"210\" lmargin=\"0\" format=\"a4\" bmargin=\"0\" height=\"297\" rmargin=\"0\" tmargin=\"0\" orientation=\"portrait\"/>\n";
    m_text += "<grid dx=\"20\" dy=\"20\" align=\"0\">\n";
    m_text += "<helplines align=\"0\"/>\n";
    m_text += "</grid>\n";
    m_text += "</head>\n";
    m_text += "<layer>\n";

    if (!parse(shapeId, fileIn, delayStream))
        return false;
    m_text += "</layer>\n";
    m_text += "</killustrator>\n";

    emit sigProgress(100);

    KoStore out = KoStore(QString(fileOut), KoStore::Write);
    if (!out.open("root"))
    {
        kdError(s_area) << "Unable to open output file!" << endl;
        out.close();
        return false;
    }
    QCString cstring = m_text.utf8();
    out.write((const char*)cstring, cstring.length());
    out.close();
    return true;
}

void MSODImport::pointArray(
    const QPointArray &points)
{

    for (unsigned i = 0; i < points.count(); i++)
    {
        m_text += "<point x=\"" + QString::number(points.point(i).x()) +
                    "\" y=\"" + QString::number(points.point(i).y()) +
                     "\"/>\n";
    }
}

//-----------------------------------------------------------------------------
void MSODImport::gotPolygon(
    unsigned penColour,
    unsigned penStyle,
    unsigned penWidth,
    unsigned brushColour,
    unsigned brushStyle,
    const QPointArray &points)
{
    QRect bounds = points.boundingRect();

    m_text += "<polygon width=\"" + QString::number(bounds.width()) +
                "\" x=\"" + QString::number(bounds.x()) +
                "\" y=\"" + QString::number(bounds.y()) +
                "\" height=\"" + QString::number(bounds.height()) +
                "\" rounding=\"0\">\n";
    m_text += "<polyline arrow1=\"0\" arrow2=\"0\">\n";
    pointArray(points);
    m_text += " <gobject fillcolor=\"#" + QString::number(brushColour, 16) +
                "\" fillstyle=\"" + QString::number(1 /*m_winding*/) +
                "\" linewidth=\"" + QString::number(penWidth) +
                "\" strokecolor=\"#" + QString::number(penColour, 16) +
                "\" strokestyle=\"" + QString::number(penStyle) +
                "\">\n";
    m_text += "  <matrix dx=\"0\" dy=\"0\" m21=\"0\" m22=\"1\" m11=\"1\" m12=\"0\"/>\n";
    m_text += " </gobject>\n";
    m_text += "</polyline>\n";
    m_text += "</polygon>\n";
}


//-----------------------------------------------------------------------------
void MSODImport::gotPolyline(
    unsigned penColour,
    unsigned penStyle,
    unsigned penWidth,
    const QPointArray &points)
{
    m_text += "<polyline arrow1=\"0\" arrow2=\"0\">\n";
    pointArray(points);
    m_text += " <gobject fillstyle=\"" + QString::number(1 /*m_winding*/) +
                "\" linewidth=\"" + QString::number(penWidth) +
                "\" strokecolor=\"#" + QString::number(penColour, 16) +
                "\" strokestyle=\"" + QString::number(penStyle) +
                "\">\n";
    m_text += "  <matrix dx=\"0\" dy=\"0\" m21=\"0\" m22=\"1\" m11=\"1\" m12=\"0\"/>\n";
    m_text += " </gobject>\n";
    m_text += "</polyline>\n";
}
