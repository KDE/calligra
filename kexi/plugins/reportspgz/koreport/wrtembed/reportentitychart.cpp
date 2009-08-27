/*
 * Kexi Report Plugin
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
#include "reportentitychart.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include "reportentities.h"
#include "reportdesigner.h"

#include <qdom.h>
#include <qpainter.h>
#include <kdebug.h>
#include <klocalizedstring.h>

#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <koproperty/EditorView.h>



void ReportEntityChart::init(QGraphicsScene* s, ReportDesigner *r)
{
    m_reportDesigner = r;
    setPos(0, 0);

    setFlags(ItemIsSelectable | ItemIsMovable);
    setAcceptsHoverEvents(true);

    if (s)
        s->addItem(this);

    connect(m_set, SIGNAL(propertyChanged(KoProperty::Set &, KoProperty::Property &)), this, SLOT(propertyChanged(KoProperty::Set &, KoProperty::Property &)));

    ReportRectEntity::init(&m_pos, &m_size, m_set);
    setZValue(Z);

    setConnection(m_reportDesigner->theConn());
}

ReportEntityChart::ReportEntityChart(ReportDesigner * rd, QGraphicsScene* scene)
        : ReportRectEntity(rd)
{
    init(scene, rd);
    m_size.setSceneSize(QSizeF(m_dpiX, m_dpiY));
    setSceneRect(m_pos.toScene(), m_size.toScene());

    m_name->setValue(m_reportDesigner->suggestEntityName("Chart"));
}

ReportEntityChart::ReportEntityChart(QDomNode & element, ReportDesigner * rd, QGraphicsScene* scene) : ReportRectEntity(rd), KRChartData(element)
{
    init(scene, rd);
    populateData();
    setSceneRect(m_pos.toScene(), m_size.toScene());

}

ReportEntityChart::~ReportEntityChart()
{
}

void ReportEntityChart::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget *widget)
{
    // store any values we plan on changing so we can restore them
    QFont f = painter->font();
    QPen  p = painter->pen();
    QColor bg = Qt::white;

    painter->fillRect(QGraphicsRectItem::rect(), bg);

    if (m_chartWidget) {
        m_chartWidget->setFixedSize(m_size.toScene().toSize());
        painter->drawImage(rect().left(), rect().top(), 
            QPixmap::grabWidget(m_chartWidget).toImage(), 
            0, 0, rect().width(), rect().height());
    }
    bg.setAlpha(255);

    painter->setBackground(bg);
    painter->setPen(Qt::black);

    painter->drawText(rect(), 0, m_dataSource->value().toString() + QObject::tr(":") + QObject::tr("chart"));


// if ( ( Qt::PenStyle ) _lnStyle->value().toInt() == Qt::NoPen || _lnWeight->value().toInt() <= 0 )
// {
    painter->setPen(QPen(QColor(224, 224, 224)));
// }
// else
// {
//  painter->setPen ( QPen ( _lnColor->value().value<QColor>(), _lnWeight->value().toInt(), ( Qt::PenStyle ) _lnStyle->value().toInt() ) );
// }

    painter->drawRect(rect());

    painter->setBackgroundMode(Qt::TransparentMode);

    drawHandles(painter);

    // restore an values before we started just in case
    painter->setFont(f);
    painter->setPen(p);
}

ReportEntityChart* ReportEntityChart::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement("clone");;
    QDomNode n;
    buildXML(d, e);
    n = e.firstChild();
    return new ReportEntityChart(n, designer(), 0);
}

void ReportEntityChart::buildXML(QDomDocument & doc, QDomElement & parent)
{
    QDomElement entity = doc.createElement("chart");

    //Size, position
    buildXMLRect(doc, entity, pointRect());

    // name
    QDomElement n = doc.createElement("name");
    n.appendChild(doc.createTextNode(entityName()));
    entity.appendChild(n);

    // z
    QDomElement z = doc.createElement("zvalue");
    z.appendChild(doc.createTextNode(QString::number(zValue())));
    entity.appendChild(z);

    //Data source
    QDomElement data = doc.createElement("data");
    QDomElement dcolumn = doc.createElement("datasource");
    dcolumn.appendChild(doc.createTextNode(m_dataSource->value().toString()));
    data.appendChild(dcolumn);
    entity.appendChild(data);
    //TODO Link child-master

    //type
    QDomElement type = doc.createElement("type");
    type.appendChild(doc.createTextNode(m_chartType->value().toString()));
    entity.appendChild(type);

    //sub type
    QDomElement subtype = doc.createElement("subtype");
    subtype.appendChild(doc.createTextNode(m_chartSubType->value().toString()));
    entity.appendChild(subtype);

    //3d
    QDomElement d3 = doc.createElement("threed");
    d3.appendChild(doc.createTextNode(m_threeD->value().toBool() ? "true" : "false"));
    entity.appendChild(d3);

    //color scheme
    QDomElement cs = doc.createElement("colorscheme");
    cs.appendChild(doc.createTextNode(m_colorScheme->value().toString()));
    entity.appendChild(cs);

    //aa
    QDomElement aa = doc.createElement("antialiased");
    aa.appendChild(doc.createTextNode(m_aa->value().toBool() ? "true" : "false"));
    entity.appendChild(aa);

    //x-title
    QDomElement xt = doc.createElement("xtitle");
    xt.appendChild(doc.createTextNode(m_xTitle->value().toString()));
    entity.appendChild(xt);

    //y-title
    QDomElement yt = doc.createElement("ytitle");
    yt.appendChild(doc.createTextNode(m_yTitle->value().toString()));
    entity.appendChild(yt);

    //background color
    QDomElement bc = doc.createElement("backgroundcolor");
    bc.appendChild(doc.createTextNode(m_backgroundColor->value().value<QColor>().name()));
    entity.appendChild(bc);

    //legend
    QDomElement dl = doc.createElement("displaylegend");
    dl.appendChild(doc.createTextNode(m_displayLegend->value().toBool() ? "true" : "false"));
    entity.appendChild(dl);

    //link master/child
    QDomElement lm = doc.createElement("linkmaster");
    lm.appendChild(doc.createTextNode(m_linkMaster->value().toString()));
    entity.appendChild(lm);
    QDomElement lc = doc.createElement("linkchild");
    lc.appendChild(doc.createTextNode(m_linkChild->value().toString()));
    entity.appendChild(lc);


    //Line Style
// buildXMLLineStyle(doc, entity, lineStyle());

    parent.appendChild(entity);
}

void ReportEntityChart::propertyChanged(KoProperty::Set &s, KoProperty::Property &p)
{
    kDebug() << s.typeName() << ":" << p.name() << ":" << p.value();

    //Handle Position
    if (p.name() == "Position") {
        m_pos.setUnitPos(p.value().value<QPointF>());
    } else if (p.name() == "Name") {
        //For some reason p.oldValue returns an empty string
        if (!m_reportDesigner->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(m_oldName);
        } else {
            m_oldName = p.value().toString();
        }
    } else if (p.name() == "ThreeD") {
        set3D(p.value().toBool());
    } else if (p.name() == "Antialiased") {
        setAA(p.value().toBool());
    } else if (p.name() == "ColorScheme") {
        setColorScheme(p.value().toString());
    } else if (p.name() == "DataSource") {
        populateData();
    } else if (p.name() == "XAxis" ||   p.name() == "YAxis") {
        setAxis(m_xTitle->value().toString(), m_yTitle->value().toString());
    } else if (p.name() == "BackgroundColor") {
        setBackgroundColor(p.value().value<QColor>());
    } else if (p.name() == "DisplayLegend") {
        setLegend(p.value().toBool());
    } else if (p.name() == "ChartType") {
        if (m_chartWidget) {
            m_chartWidget->setType((KDChart::Widget::ChartType) m_chartType->value().toInt());
        }
    } else if (p.name() == "ChartSubType") {
        if (m_chartWidget) {
            m_chartWidget->setSubType((KDChart::Widget::SubType) m_chartSubType->value().toInt());
        }
    }

    if (m_reportDesigner) m_reportDesigner->setModified(true);

    if (scene()) scene()->update();

}

void ReportEntityChart::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    QStringList ql = queryList(m_reportDesigner->theConn());
    m_dataSource->setListData(ql,ql);
    ReportRectEntity::mousePressEvent(event);
}

QStringList ReportEntityChart::queryList(KexiDB::Connection* conn)
{
    //Get the list of queries in the database
    QStringList qs;
    if (conn && conn->isConnected()) {
        QList<int> tids = conn->tableIds();
        qs << "";
        for (int i = 0; i < tids.size(); ++i) {
            KexiDB::TableSchema* tsc = conn->tableSchema(tids[i]);
            if (tsc)
                qs << tsc->name();
        }
        
        QList<int> qids = conn->queryIds();
        qs << "";
        for (int i = 0; i < qids.size(); ++i) {
            KexiDB::QuerySchema* qsc = conn->querySchema(qids[i]);
            if (qsc)
                qs << qsc->name();
        }
    }
    
    return qs;
}

