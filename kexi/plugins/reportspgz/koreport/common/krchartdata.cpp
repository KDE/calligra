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
#include "krchartdata.h"
#include <KoGlobal.h>
#include <kglobalsettings.h>
#include <klocalizedstring.h>

#include <KDChartBarDiagram>
#include <KDChartThreeDBarAttributes>
#include <KDChartLineDiagram>
#include <KDChartThreeDLineAttributes>
#include <KDChartPieDiagram>
#include <KDChartThreeDPieAttributes>
#include <KDChartLegend>
#include <KDChartCartesianAxis>

#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidb/parser/parser.h>
#include <kexidb/field.h>
#include <kexidb/queryschema.h>

#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <QMotifStyle>
#include <kdebug.h>

#include <KDChartChart>
#include <KDChartBackgroundAttributes>

typedef QVector<double> datalist;

KRChartData::KRChartData(QDomNode & element)
{
    m_reportData = 0;
    createProperties();

    QDomNodeList nl = element.childNodes();

    QString n;
    QDomNode node;
    QDomElement e = element.toElement();
    m_name->setValue(e.attribute("report:name"));
    m_dataSource->setValue(e.attribute("report:data-source"));
    Z = e.attribute("report:z-index").toDouble();
    m_chartType->setValue(e.attribute("report:chart-type").toInt());
    m_chartSubType->setValue(e.attribute("report:chart-sub-type").toInt());
    m_threeD->setValue(e.attribute("report:three-dimensions"));

    m_colorScheme->setValue(e.attribute("report:chart-color-scheme"));
    m_aa->setValue(e.attribute("report:antialiased"));
    m_xTitle->setValue(e.attribute("report:title-x-axis"));
    m_yTitle->setValue(e.attribute("report:title-y-axis"));
    m_backgroundColor->setValue(e.attribute("report:background-color"));
    m_displayLegend->setValue(e.attribute("report:display-legend"));
    m_linkMaster->setValue(e.attribute("report:link-master"));
    m_linkChild->setValue(e.attribute("report:link-child"));

    parseReportRect(e, &m_pos, &m_size);

}


KRChartData::~KRChartData()
{
}

void KRChartData::createProperties()
{
    m_chartWidget = 0;
    m_set = new KoProperty::Set(0, "Chart");

    QStringList strings;
    QList<QVariant> keys;
    QStringList stringkeys;

    m_dataSource = new KoProperty::Property("data-source", QStringList(), QStringList(), "", "Data Source");

    m_dataSource->setOption("extraValueAllowed", "true");

    m_font = new KoProperty::Property("Font", KGlobalSettings::generalFont(), "Font", "Field Font");

    keys << 1 << 2 << 3 << 4 << 5;
    strings << i18n("Bar") << i18n("Line") << i18n("Pie") << i18n("Ring") << i18n("Polar");
    KoProperty::Property::ListData *typeData = new KoProperty::Property::ListData(keys, strings);
    m_chartType = new KoProperty::Property("chart-type", typeData, 1, "Chart Type");

    keys.clear();
    strings.clear();
    keys << 0 << 1 << 2 << 3;
    strings << i18n("Normal") << i18n("Stacked") << i18n("Percent") << i18n("Rows");

    KoProperty::Property::ListData *subData = new KoProperty::Property::ListData(keys, strings);

    m_chartSubType = new KoProperty::Property("chart-sub-type", subData, 0, "Chart Sub Type");

    keys.clear();
    strings.clear();
    stringkeys << "default" << "rainbow" << "subdued";
    strings << i18n("Default") << i18n("Rainbow") << i18n("Subdued");
    m_colorScheme = new KoProperty::Property("chart-color-scheme", stringkeys, strings, "default", i18n("Color Scheme"));

    m_threeD = new KoProperty::Property("three-dimensions", false, "3D", "3D");
    m_aa = new KoProperty::Property("antialiased", false, "Antialiased", "Antialiased");

    m_xTitle = new KoProperty::Property("title-x-axis", "", "X Axis Title", "X Axis Title");
    m_yTitle = new KoProperty::Property("title-y-axis", "", "Y Axis Title", "Y Axis Title");

    m_displayLegend = new KoProperty::Property("display-legend", true, "Display Legend", "Display Legend");

    m_backgroundColor = new KoProperty::Property("background-color", Qt::white, "Background Color", "Background Color");

    m_linkMaster = new KoProperty::Property("link-master", "", "Link Master", i18n("Fields from master data source"));
    m_linkChild = new KoProperty::Property("link-child", "", "Link Child", i18n("Fields from child data source"));

    m_set->addProperty(m_name);
    m_set->addProperty(m_dataSource);

    m_set->addProperty(m_pos.property());
    m_set->addProperty(m_size.property());
    m_set->addProperty(m_chartType);
    m_set->addProperty(m_chartSubType);
    m_set->addProperty(m_font);
    m_set->addProperty(m_colorScheme);
    m_set->addProperty(m_threeD);
    m_set->addProperty(m_aa);
    m_set->addProperty(m_xTitle);
    m_set->addProperty(m_yTitle);
    m_set->addProperty(m_backgroundColor);
    m_set->addProperty(m_displayLegend);
    m_set->addProperty(m_linkMaster);
    m_set->addProperty(m_linkChild);

    set3D(false);
    setAA(false);
    setColorScheme("default");
}

void KRChartData::set3D(bool td)
{
    if (m_chartWidget && m_chartWidget->barDiagram()) {
        KDChart::BarDiagram *bar = m_chartWidget->barDiagram();
        bar->setPen(QPen(Qt::black));

        KDChart::ThreeDBarAttributes threed = bar->threeDBarAttributes();
        threed.setEnabled(td);
        threed.setDepth(10);
        threed.setAngle(15);
        threed.setUseShadowColors(true);
        bar->setThreeDBarAttributes(threed);
    }

}
void KRChartData::setAA(bool aa)
{
    if (m_chartWidget && m_chartWidget->diagram()) {
        m_chartWidget->diagram()->setAntiAliasing(aa);
    }
}

void KRChartData::setColorScheme(const QString &cs)
{
    if (m_chartWidget && m_chartWidget->diagram()) {
        if (cs == "rainbow") {
            m_chartWidget->diagram()->useRainbowColors();
        } else if (cs == "subdued") {
            m_chartWidget->diagram()->useSubduedColors();
        } else {
            m_chartWidget->diagram()->useDefaultColors();
        }
    }
}

void KRChartData::setConnection(KoReportData *c)
{
    m_reportData = c;
}

void KRChartData::populateData()
{
    QVector<datalist> data;
    QStringList labels;

    QStringList fn;


    delete m_chartWidget;
    m_chartWidget = 0;

    if (m_reportData) {
        QString src = m_dataSource->value().toString();

        if (!src.isEmpty()) {
            KoReportData *curs = m_reportData->data(src);
            if (curs && curs->open()) {
                fn = curs->fieldNames();

                //resize the data lists to match the number of columns
                int cols = fn.count() - 1;
                data.resize(cols);

                m_chartWidget = new KDChart::Widget();
                //_chartWidget->setStyle ( new QMotifStyle() );

                m_chartWidget->setType((KDChart::Widget::ChartType) m_chartType->value().toInt());
                m_chartWidget->setSubType((KDChart::Widget::SubType) m_chartSubType->value().toInt());
                set3D(m_threeD->value().toBool());
                setAA(m_aa->value().toBool());
                setColorScheme(m_colorScheme->value().toString());
                setBackgroundColor(m_backgroundColor->value().value<QColor>());
                curs->moveFirst();
                bool status = true;
                do {
                    labels << curs->value(0).toString();
                    for (int i = 1; i <= cols; ++i) {
                        data[i - 1] << curs->value(i).toDouble();
                        kDebug() << data[i-1];
                    }
                } while (curs->moveNext());

                kDebug() << labels;
                if (data.size() > 0) {
                    kDebug() << data[0];
                }
                for (int i = 1; i <= cols; ++i) {
                    m_chartWidget->setDataset(i - 1, data[i - 1], fn[i]);
                }

                setLegend(m_displayLegend->value().toBool());

                //Add the axis
                setAxis(m_xTitle->value().toString(), m_yTitle->value().toString());

                //Add the bottom labels
                if (m_chartWidget->barDiagram() || m_chartWidget->lineDiagram()) {
                    KDChart::AbstractCartesianDiagram *dia = dynamic_cast<KDChart::AbstractCartesianDiagram*>(m_chartWidget->diagram());

                    foreach(KDChart::CartesianAxis* axis, dia->axes()) {
                        if (axis->position() == KDChart::CartesianAxis::Bottom) {
                            axis->setLabels(labels);
                        }
                    }
                }
            } else {
                kDebug() << "Unable to open data set";
            }
            delete curs;
        } else {
            kDebug() << "No source set";
        }
    } else {
        kDebug() << "No connection!";
    }
}

QStringList KRChartData::masterFields()
{
    return m_linkMaster->value().toString().split(",");
}

void KRChartData::setLinkData(QString fld, QVariant val)
{
    kDebug() << "Field: " << fld << "is" << val;
    m_links[fld] = val;
}

//!TODO
#if 0
QStringList KRChartData::fieldNames(const QString &stmt)
{
    KexiDB::Parser *pars;

    pars = new KexiDB::Parser(m_reportData);

    pars->setOperation(KexiDB::Parser::OP_Select);
    pars->parse(stmt);

    KexiDB::QuerySchema *qs = pars->select();

    if (qs) {
        kDebug() << "Parsed OK " << qs->fieldCount();
        KexiDB::Field::List fl = qs->fields();
        QStringList fn;

        for (unsigned int i = 0; i < qs->fieldCount(); ++i) {
            fn << qs->field(i)->name();
        }
        return fn;
    } else {
        kDebug() << "Unable to parse statement";
        return QStringList();
    }

}

QStringList KRChartData::fieldNamesHackUntilImprovedParser(const QString &stmt)
{
    QStringList fn;
    QString ds = m_dataSource->value().toString();
    KexiDB::Field::List fl;
    QString s;

    if (m_reportData && m_reportData->tableSchema(ds)) {
        kDebug() << ds << "is a table";
        fn = m_reportData->tableSchema(ds)->names();
    } else if (m_reportData && m_reportData->querySchema(ds)) {
        kDebug() << ds << "is a query";
        fn = m_reportData->querySchema(ds)->names();
    } else {
        QString s = stmt.mid(6, stmt.indexOf("from", 0, Qt::CaseInsensitive) - 6).simplified();
        kDebug() << s;
        fn = s.split(",");
    }
    return fn;
}

#endif

void KRChartData::setAxis(const QString& xa, const QString &ya)
{
    Q_ASSERT(m_chartWidget);

    if (m_chartWidget->barDiagram() || m_chartWidget->lineDiagram()) {
        KDChart::AbstractCartesianDiagram *dia = dynamic_cast<KDChart::AbstractCartesianDiagram*>(m_chartWidget->diagram());
        KDChart::CartesianAxis *xAxis = 0;
        KDChart::CartesianAxis *yAxis = 0;

        //delete existing axis
        foreach(KDChart::CartesianAxis* axis, dia->axes()) {
            if (axis->position() == KDChart::CartesianAxis::Bottom) {
                xAxis = axis;
            }
            if (axis->position() == KDChart::CartesianAxis::Left) {
                yAxis = axis;
            }
        }

        if (!xAxis) {
            xAxis =  new KDChart::CartesianAxis(dynamic_cast<KDChart::AbstractCartesianDiagram*>(m_chartWidget->diagram()));
            xAxis->setPosition(KDChart::CartesianAxis::Bottom);
            dia->addAxis(xAxis);
        }

        if (!yAxis) {
            yAxis = new KDChart::CartesianAxis(dynamic_cast<KDChart::AbstractCartesianDiagram*>(m_chartWidget->diagram()));
            yAxis->setPosition(KDChart::CartesianAxis::Left);
            dia->addAxis(yAxis);
        }

        xAxis->setTitleText(xa);
        yAxis->setTitleText(ya);
    }
}

//!TODO
#if 0
KexiDB::Cursor *KRChartData::dataSet()
{
    QString ds = m_dataSource->value().toString();
    KexiDB::Cursor *c = 0;
    KexiDB::QuerySchema *qs;

    //Determin the type of the source data and create a queryschema
    if (m_reportData && m_reportData->tableSchema(ds)) {
        kDebug() << ds << "is a table";
        qs = new KexiDB::QuerySchema(*(m_reportData->tableSchema(ds)));
    } else if (m_reportData && m_reportData->querySchema(ds)) {
        kDebug() << ds << "is a query";
        qs = m_reportData->querySchema(ds);
    } else {
        kDebug() << ds << "is a statement";
        KexiDB::Parser *pars;
        pars = new KexiDB::Parser(m_reportData);
        pars->setOperation(KexiDB::Parser::OP_Select);
        pars->parse(ds);
        qs = pars->select();
    }

    //Now go through each master field value, and add the correspanding child field
    //as a where condition on the query
    if (qs) {
        QStringList childFields = m_linkChild->value().toString().split(",");
        QStringList masterFields = m_linkMaster->value().toString().split(",");

        for (int i = 0; i < childFields.size(); ++i) {
            KexiDB::Field *f = qs->findTableField(childFields[i]);
            //Only add the condition if we found the child field, and we have data for the master field
            if (f && m_links.contains(masterFields[i])) {
                qs->addToWhereExpression(f, m_links[masterFields[i]]);
            }
        }

        c = m_reportData->executeQuery(*qs, 1);
    }
    return c;
}
#endif

void KRChartData::setBackgroundColor(const QColor&)
{
    //Set the background color
    KDChart::Chart *cht = m_chartWidget->diagram()->coordinatePlane()->parent();

    KDChart::BackgroundAttributes ba;

    ba.setVisible(true);
    ba.setBrush(m_backgroundColor->value().value<QColor>());
    cht->setBackgroundAttributes(ba);
}

void KRChartData::setLegend(bool le)
{
    //Add the legend
    if (m_chartWidget) {
        if (le) {
//!TODO            QStringList fn = fieldNamesHackUntilImprovedParser(m_dataSource->value().toString());
            QStringList fn = m_reportData->fieldNames();

            m_chartWidget->addLegend(KDChart::Position::East);
            m_chartWidget->legend()->setOrientation(Qt::Horizontal);
            m_chartWidget->legend()->setTitleText("Legend");
            for (uint i = 1; i < (uint)fn.count(); ++i) {
                m_chartWidget->legend()->setText(i - 1, fn[i]);
            }

            m_chartWidget->legend()->setShowLines(true);
        } else {
            if (m_chartWidget->legend()) {
                m_chartWidget->takeLegend(m_chartWidget->legend());
            }
        }
    }
}

// RTTI
int KRChartData::type() const
{
    return RTTI;
}
int KRChartData::RTTI = KRObjectData::EntityChart;
KRChartData * KRChartData::toChart()
{
    return this;
}
