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
        kDebug() << m_connection->currentDatabase();
        painter.drawText(converter.documentToView(QPointF(5.0, 11.0)), m_connection->currentDatabase() + " : " + m_relation);

        if (m_relationSchema) { //We have the schema, so lets lets paint it
            KexiDB::QueryColumnInfo::Vector columns = m_relationSchema->columns(true);
            uint i = 0;
            foreach(KexiDB::QueryColumnInfo *column, columns) {
                ++i;
                painter.drawText(converter.documentToView(QPointF(15.0, (13.0 * i) + 20)), column->aliasOrName() + " - " + column->field->typeName());
                
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
                m_connection->useDatabase(m_connectionData->dbFileName());
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
    if (m_relation != rel) {
        m_relation = rel;

        if ( m_connection->tableSchema ( m_relation ) ) {
            kDebug() << m_relation <<  " is a table..";
            m_relationSchema = new KexiDB::TableOrQuerySchema ( m_connection->tableSchema ( m_relation ) );
        } else if ( m_connection->querySchema ( m_relation ) ) {
            kDebug() << m_relation <<  " is a query..";
            m_relationSchema = new KexiDB::TableOrQuerySchema ( m_connection->querySchema ( m_relation ) );
        }
        update();
    }
}
