/* This file is part of the KDE project
 * Copyright (C) 2011 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoMarkerModel.h"

#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <KoOdfStylesReader.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoPathShape.h>

#include <KoMarker.h>

#include <QBuffer>
#include <QPointF>
#include <QSize>
#include <QList>


KoMarkerModel::KoMarkerModel(const QList<KoMarker*> markers, KoMarkerData::MarkerPosition position, QObject *parent)
: QAbstractListModel(parent)
, m_markers(markers)
, m_markerPosition(position)
{
#if 0
    if(m_markers.size() == 0){
        // No arrow
        m_markers.append(0);

        // Big arrow
        KoXmlDocument doc;
        QString errorMsg;
        int errorLine;
        int errorColumn;
        
        QBuffer xmldevice;
        xmldevice.open(  QIODevice::WriteOnly );
        QTextStream xmlstream(  &xmldevice );
        
        xmlstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        xmlstream << "<office:document-content xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:config=\"urn:oasis:names:tc:opendocument:xmlns:config:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\" xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\" xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" xmlns:form=\"urn:oasis:names:tc:opendocument:xmlns:form:1.0\" xmlns:script=\"urn:oasis:names:tc:opendocument:xmlns:script:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\" xmlns:math=\"http://www.w3.org/1998/Math/MathML\" xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" xmlns:koffice=\"http://www.koffice.org/2005/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">";
        xmlstream << "<draw:marker draw:name=\"Arrow\" svg:viewBox=\"0 0 20 30\" svg:d=\"m10 0-10 30h20z\"/>";
        xmlstream << "</office:document-content>";
        xmldevice.close();
        doc.setContent( &xmldevice, true, &errorMsg, &errorLine, &errorColumn );
        KoXmlElement content = doc.documentElement();
        KoXmlElement element( KoXml::namedItemNS( content, KoXmlNS::draw, "marker" ) );
        KoOdfStylesReader stylesReader;
        KoOdfLoadingContext odfContext( stylesReader, 0 );
        KoShapeLoadingContext shapeContext( odfContext, 0 );
        
        KoMarker *bigArrowMarker = new KoMarker();
        bigArrowMarker->loadOdf( element, shapeContext );
        m_markers.append(bigArrowMarker);
        
        // Small arrow
        QBuffer smallxmldevice;
        smallxmldevice.open(  QIODevice::WriteOnly );
        QTextStream smallxmlstream(  &smallxmldevice );
        
        smallxmlstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        smallxmlstream << "<office:document-content xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:config=\"urn:oasis:names:tc:opendocument:xmlns:config:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\" xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\" xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" xmlns:form=\"urn:oasis:names:tc:opendocument:xmlns:form:1.0\" xmlns:script=\"urn:oasis:names:tc:opendocument:xmlns:script:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\" xmlns:math=\"http://www.w3.org/1998/Math/MathML\" xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" xmlns:koffice=\"http://www.koffice.org/2005/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">";
        smallxmlstream << "<draw:marker draw:name=\"Arrow\" svg:viewBox=\"0 0 20 30\" svg:d=\"m2 0-2 6h4z\"/>";
        smallxmlstream << "</office:document-content>";
        smallxmldevice.close();
        doc.setContent( &smallxmldevice, true, &errorMsg, &errorLine, &errorColumn );
        KoXmlElement smallContent = doc.documentElement();
        KoXmlElement smallElement( KoXml::namedItemNS( smallContent, KoXmlNS::draw, "marker" ) );
        KoOdfStylesReader smallStylesReader;
        KoOdfLoadingContext smallOdfContext( smallStylesReader, 0 );
        KoShapeLoadingContext smallShapeContext( odfContext, 0 );

        KoMarker *smallArrowMarker = new KoMarker();
        smallArrowMarker->loadOdf( smallElement, smallShapeContext );
        m_markers.append(smallArrowMarker);
    }
#endif
}

KoMarkerModel::~KoMarkerModel()
{
}

int KoMarkerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_markers.count();
}

QVariant KoMarkerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
         return QVariant();
    }

    switch(role) {
    case Qt::DecorationRole:
        if (index.row() < m_markers.size()) {
            KoPathShape *pathShape = new KoPathShape();
            pathShape->moveTo(QPointF(10, 15));
            pathShape->lineTo(QPointF(70, 15));
            pathShape->setMarker(m_markers.at(index.row()), m_markerPosition);
            return QVariant::fromValue<KoPathShape*>(pathShape);
        }
        return QVariant();
    case Qt::SizeHintRole:
        return QSize(80,30);
    default:
        return QVariant();
    }
}

int KoMarkerModel::markerIndex(KoMarker *marker) const
{
    return m_markers.indexOf(marker);
}

QVariant KoMarkerModel::marker(int index, int role) const
{
    if (index < 0){
        return QVariant();
    }
    
    switch(role) {
        case Qt::DecorationRole:
            if (index< m_markers.size()) {
                return QVariant::fromValue<KoMarker*>(m_markers.at(index));
            }
            return QVariant();
        case Qt::SizeHintRole:
            return QSize(80, 30);
        default:
            return QVariant();
    }
}

KoMarkerData::MarkerPosition KoMarkerModel::position() const
{
    return m_markerPosition;
}
