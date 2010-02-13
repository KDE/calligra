/* This file is part of the KDE project
   Copyright (C) 2009-2010 Adam Pigg <adam@piggz.co.uk>
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kexirelationdesignshape.h"
#include <QPainter>
#include "KoViewConverter.h"
#include "kexidb/connection.h"
#include <kexidb/drivermanager.h>
#include <kexidb/utils.h>
#include <kdebug.h>
#include <kexidb/queryschema.h>
#include <KoXmlWriter.h>
#include <KoShapeSavingContext.h>
#include <KoXmlReader.h>
#include <KoShapeBackground.h>
#include <KoZoomHandler.h>

KexiRelationDesignShape::KexiRelationDesignShape() : KoFrameShape("http://www.koffice.org/kexirelationdesign", "shape"){
    m_connection = 0;
    m_connectionData = 0;
    m_relationSchema = 0;
}

KexiRelationDesignShape::~KexiRelationDesignShape() {
    if (m_connection) {
        m_connection->disconnect();
    }
    delete m_connection;
    m_connection = 0;
}

void KexiRelationDesignShape::saveOdf ( KoShapeSavingContext& context ) const {
    KoXmlWriter& writer = context.xmlWriter();
    writer.startElement("draw:frame");
    saveOdfAttributes(context, OdfAllAttributes);
    
    writer.startElement("kexirelationdesign:shape");
    writer.addAttribute("xmlns:kexirelationdesign", "http://www.koffice.org/kexirelationdesign");
    writer.startElement("kexirelationdesign:relation");
    writer.addAttribute("database", m_database);
    writer.addAttribute("relation", m_relation);
    foreach(SimpleField* column, m_fieldData) {
        column->save(writer);
    }
    writer.endElement(); //relation
    writer.endElement(); //kexirelation:shape


    // Save a preview image
    qreal previewDPI = 200;
    QSizeF imgSize = size(); // in points
    imgSize *= previewDPI / 72;
    QImage img(imgSize.toSize(), QImage::Format_ARGB32);
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    //Fill to white
    painter.fillRect(QRectF(QPointF(0,0), imgSize), Qt::white);
    
    KoZoomHandler converter;
    converter.setZoomAndResolution(100, previewDPI, previewDPI);
    constPaint(painter, converter);
    writer.startElement("draw:image");
    // In the spec, only the xlink:href attribute is marked as mandatory, cool :)
    QString name = context.imageHref(img);
    writer.addAttribute("xlink:type", "simple" );
    writer.addAttribute("xlink:show", "embed" );
    writer.addAttribute("xlink:actuate", "onLoad");
    writer.addAttribute("xlink:href", name);
    writer.endElement(); // draw:image

    saveOdfCommonChildElements(context);
    writer.endElement(); // draw:frame

}

bool KexiRelationDesignShape::loadOdf ( const KoXmlElement& element, KoShapeLoadingContext& context ) {
    loadOdfAttributes(element, context, OdfAllAttributes);
    return loadOdfFrame(element, context);
}

bool KexiRelationDesignShape::loadOdfFrameElement( const KoXmlElement & element, KoShapeLoadingContext & context )
{

KoXmlElement relation = KoXml::namedItemNS(element, "http://www.koffice.org/kexirelationdesign", "relation");
if (relation.isNull()) {
    kWarning() << "no relation element as first child";
    return false;
}

kDebug() << relation.attributeNames();

m_database = relation.attribute("database");
m_relation = relation.attribute("relation");

KoXmlElement e;
m_fieldData.clear();

forEachElement(e, relation) {
    SimpleField *sf = new SimpleField();
    sf->name = e.attribute("name");
    sf->type = e.attribute("type");
    sf->pkey = e.attribute("primarykey").toInt();
    sf->notnull = e.attribute("notnull").toInt();
    m_fieldData.append(sf);
}
addConnectionPoints();
return true;
}

void KexiRelationDesignShape::paint ( QPainter& painter, const KoViewConverter& converter ) {
    constPaint(painter, converter);
}

void KexiRelationDesignShape::constPaint ( QPainter& painter, const KoViewConverter& converter ) const {
    applyConversion(painter, converter);
    
    painter.save();
    //painter.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath pp;
    pp.addRoundedRect(QRectF(QPointF(0.0,0.0), size()), 3.0, 3.0);

    painter.setClipPath(pp);
    painter.setPen(QPen(Qt::black, 1.0));
    
    //Draw user specified background
    if (background()) {
        background()->paint(painter, pp);
    }
    painter.setClipping(false);
    painter.drawRoundedRect(QRectF(QPointF(0.0,0.0), (size())), 3.0, 3.0);
    painter.drawLine(0, 15, size().width(), 15);

    QFont f;
    f.setFamily("sans-serif");
    f.setPixelSize(10);
    
    painter.setFont(f);
    
    painter.drawText(QPointF(5.0, 11.0), m_database + " : " + m_relation);

    uint i = 0;
    uint offset;
    foreach (SimpleField *column, m_fieldData) {
        ++i;
        offset = (13.0*i) + 20;
        painter.drawText(QPointF(15.0, offset), column->name + " - " + column->type);
        if (column->pkey) {
            painter.drawEllipse(QPointF(8.0, offset - 4), 4,4);
        }
    }
    
    painter.restore();
}

void KexiRelationDesignShape::setConnectionData(KexiDB::ConnectionData* cd) {
    if (m_connectionData != cd) {
        
        //Close any existing connection
        if (m_connection) {
            m_connection->disconnect();
            delete m_connection;
            m_connection = 0;
        }
        m_connectionData = cd;
        
        KexiDB::DriverManager dm;
        
        kDebug() << m_connectionData->driverName;
        KexiDB::Driver *_driver = dm.driver(m_connectionData->driverName);

        if (_driver) {
            m_connection = _driver->createConnection(*m_connectionData);
        }
        else {
            kDebug() << "Unable to create driver";
        }

        if (m_connection) {
            if (m_connection->connect()) {
                if(m_connection->useDatabase(m_connectionData->dbFileName())){
                    m_database = m_connection->currentDatabase();
                }
            }
            else {
                kDebug() << "Unable to connect";
            }
        }
        else {
            kDebug() << "No connection";
        }
        update();
        
    }
}

KexiDB::ConnectionData* KexiRelationDesignShape::connectionData() {
    return m_connectionData;
}

KexiDB::Connection* KexiRelationDesignShape::connection(){
    return m_connection;
}

void KexiRelationDesignShape::setRelation(const QString& rel){
    kDebug() << rel;
    if (m_relation != rel) {
        m_relation = rel;

        m_fieldData.clear();
        m_relationSchema = 0;
        if (m_connection && m_connection->isConnected()) {
            if ( m_connection->tableSchema ( m_relation ) ) {
                kDebug() << m_relation <<  " is a table..";
                m_relationSchema = new KexiDB::TableOrQuerySchema ( m_connection->tableSchema ( m_relation ) );
            } else if ( m_connection->querySchema ( m_relation ) ) {
                kDebug() << m_relation <<  " is a query..";
                m_relationSchema = new KexiDB::TableOrQuerySchema ( m_connection->querySchema ( m_relation ) );
            }
        }
        if (m_relationSchema) { //We have the schema, so lets lets paint it
            KexiDB::QueryColumnInfo::Vector columns = m_relationSchema->columns(true);
            uint i = 0;
            foreach(KexiDB::QueryColumnInfo *column, columns) {
                m_fieldData.append(new SimpleField(column));
            }
            
        }
        addConnectionPoints();
        update();
    }
}

void KexiRelationDesignShape::addConnectionPoints()
{
    uint i = 0;
    int offset = 0;

    int point_count = connectionPoints().count();

    for (int j = 0; j < point_count; ++j) {
        removeConnectionPoint(0);
    }
    
    foreach (SimpleField *column, m_fieldData) {
        ++i;
        offset = (13.0*i) + 15;
        addConnectionPoint(QPointF(0,offset));
        addConnectionPoint(QPointF(boundingRect().width(), offset));

    }
}

void KexiRelationDesignShape::setSize(const QSizeF &size)
{
    addConnectionPoints();
    KoShape::setSize(size);
}
