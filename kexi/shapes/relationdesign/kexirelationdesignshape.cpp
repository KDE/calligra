/*
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

KexiRelationDesignShape::KexiRelationDesignShape() {
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
    
    writer.startElement("kexirelation:shape");
    writer.addAttribute("xmlns:kexirelation", "http://www.koffice.org/kexi");
    writer.startElement("relation");
    writer.addAttribute("database", m_database);
    writer.addAttribute("relation", m_relation);
    foreach(SimpleField* column, m_fieldData) {
        writer.startElement("relation:column");
        writer.addAttribute("name", column->name);
        writer.addAttribute("type", column->type);
        writer.addAttribute("primarykey", column->pkey);
        writer.addAttribute("notnull", column->notnull);
        writer.endElement();
    }
    writer.endElement(); //relation
    writer.endElement(); //kexirelation:shape

#if 0
    // Save a preview image
    qreal previewDPI = 150;
    QSizeF imgSize = size(); // in points
    imgSize *= previewDPI / 72;
    QImage img(imgSize.toSize(), QImage::Format_ARGB32);
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
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
    
    // TODO: Save a preview svg

#endif
    saveOdfCommonChildElements(context);
    writer.endElement(); // draw:frame

}

bool KexiRelationDesignShape::loadOdf ( const KoXmlElement& element, KoShapeLoadingContext& context ) {

}

void KexiRelationDesignShape::paint ( QPainter& painter, const KoViewConverter& converter ) {

    QSizeF viewSize = converter.documentToView(size());
    QLinearGradient linearGrad(QPointF(0, 0), QPointF(viewSize.width(), viewSize.height()));
    linearGrad.setColorAt(0, Qt::white);
    linearGrad.setColorAt(1, QColor(240,240,240));

    painter.save();
    painter.setClipRect(QRectF(QPointF(0,0), viewSize));
    painter.setBrush(linearGrad);
    
    painter.setPen(Qt::black);
    painter.drawRoundedRect(QRectF(QPointF(0,0), viewSize), converter.documentToViewX(3.0), converter.documentToViewY(3.0));
    painter.drawLine(0, converter.documentToViewY(15), viewSize.width(), converter.documentToViewY(15));

    QFont f;
    f.setFamily("sans-serif");
    f.setPixelSize(converter.documentToViewX(10));
    
    painter.setFont(f);
    
    if (m_connection) {
        painter.drawText(converter.documentToView(QPointF(5.0, 11.0)), m_database + " : " + m_relation);

        uint i = 0;
        uint offset;
        foreach (SimpleField *column, m_fieldData) {
            ++i;
            offset = (13.0*i) + 20;
            painter.drawText(converter.documentToView(QPointF(15.0, offset)), column->name + " - " + column->type);
            if (column->pkey) {
                painter.drawEllipse(converter.documentToView(QPointF(8.0, offset - 4)), converter.documentToViewX(4.0), converter.documentToViewY(4.0));
            }
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
        
        //Connect to the new database
        //m_connection = new KexiDB::Connection();
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
        
        update();
    }
}
