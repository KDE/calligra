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
#include <QMotifStyle>
#include <kdebug.h>

#include <KDChartChart>
#include <KDChartBackgroundAttributes>

typedef QVector<double> datalist;

KRChartData::KRChartData(QDomNode & element)
{
    _conn = 0;
    createProperties();

    QDomNodeList nl = element.childNodes();

    QString n;
    QDomNode node;

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        kDebug() << node.nodeName() << node.firstChild().nodeValue();
        if (n == "data") {
            QDomNodeList dnl = node.childNodes();
            for (int di = 0; di < dnl.count(); di++) {
                //TODO link child, master
                node = dnl.item(di);
                n = node.nodeName();
                if (n == "datasource") {
                    _dataSource->setValue(node.firstChild().nodeValue());
                } else {
                    kDebug() << "while parsing field data encountered and unknown element: " << n;
                }
            }
        } else if (n == "name") {
            _name->setValue(node.firstChild().nodeValue());
        } else if (n == "zvalue") {
            Z = node.firstChild().nodeValue().toDouble();
        } else if (n == "type") {
            _chartType->setValue(node.firstChild().nodeValue().toInt());
        } else if (n == "subtype") {
            _chartSubType->setValue(node.firstChild().nodeValue().toInt());
        } else if (n == "threed") {
            _threeD->setValue(node.firstChild().nodeValue() == "true" ? true : false);
        } else if (n == "colorscheme") {
            _colorScheme->setValue(node.firstChild().nodeValue());
        } else if (n == "antialiased") {
            _aa->setValue(node.firstChild().nodeValue() == "true" ? true : false);
        } else if (n == "xtitle") {
            _xTitle->setValue(node.firstChild().nodeValue());
        } else if (n == "ytitle") {
            _yTitle->setValue(node.firstChild().nodeValue());
        } else if (n == "rect") {
            QRectF r;
            parseReportRect(node.toElement(), r);
            _pos.setPointPos(r.topLeft());
            _size.setPointSize(r.size());
        } else if (n == "backgroundcolor") {
            _bgColor->setValue(QColor(node.firstChild().nodeValue()));

        } else if (n == "displaylegend") {
            _displayLegend->setValue(node.firstChild().nodeValue() == "true" ? true : false);

        } else if (n == "linestyle") {
            ORLineStyleData ls;
//   if (parseReportLineStyleData( node.toElement(), ls ))
//   {
//    _lnWeight->setValue(ls.weight);
//    _lnColor->setValue(ls.lnColor);
//    _lnStyle->setValue(ls.style);
//   }
        } else if (n =="linkmaster") {
            _linkMaster->setValue(node.firstChild().nodeValue());
        } else if (n =="linkchild") {
            _linkChild->setValue(node.firstChild().nodeValue());
        } else {
            kDebug() << "while parsing field element encountered unknow element: " << n;
        }
    }

}


KRChartData::~KRChartData()
{
}

void KRChartData::createProperties()
{
    _chartWidget = 0;
    _set = new KoProperty::Set(0, "Chart");

    QStringList strings;
    QList<QVariant> keys;
    QStringList stringkeys;

    _dataSource = new KoProperty::Property("DataSource", QStringList(), QStringList(), "", "Data Source");

    _dataSource->setOption("extraValueAllowed", "true");

    _font = new KoProperty::Property("Font", KGlobalSettings::generalFont(), "Font", "Field Font");

    keys << 1 << 2 << 3 << 4 << 5;
    strings << i18n("Bar") << i18n("Line") << i18n("Pie") << i18n("Ring") << i18n("Polar");
    KoProperty::Property::ListData *typeData = new KoProperty::Property::ListData(keys, strings);
    _chartType = new KoProperty::Property("Type", typeData, 1, "Chart Type");

    keys.clear();
    strings.clear();
    keys << 0 << 1 << 2 << 3;
    strings << i18n("Normal") << i18n("Stacked") << i18n("Percent") << i18n("Rows");

    KoProperty::Property::ListData *subData = new KoProperty::Property::ListData(keys, strings);

    _chartSubType = new KoProperty::Property("Sub Type", subData, 0, "Chart Sub Type");

    keys.clear();
    strings.clear();
    stringkeys << "default" << "rainbow" << "subdued";
    strings << i18n("Default") << i18n("Rainbow") << i18n("Subdued");
    _colorScheme = new KoProperty::Property("ColorScheme", stringkeys, strings, "default", i18n("Color Scheme"));

    _threeD = new KoProperty::Property("ThreeD", false, "3D", "3D");
    _aa = new KoProperty::Property("Antialiased", false, "Antialiased", "Antialiased");

    _xTitle = new KoProperty::Property("XTitle", "", "X Axis Title", "X Axis Title");
    _yTitle = new KoProperty::Property("YTitle", "", "Y Axis Title", "Y Axis Title");

    _displayLegend = new KoProperty::Property("DisplayLegend", true, "Display Legend", "Display Legend");

    _bgColor = new KoProperty::Property("BackgroundColor", Qt::white, "Background Color", "Background Color");

    _linkMaster = new KoProperty::Property("LinkMaster", "", "Link Master", i18n("Fields from master data source"));
    _linkChild = new KoProperty::Property("LinkChild", "", "Link Child", i18n("Fields from child data source"));

    _set->addProperty(_name);
    _set->addProperty(_dataSource);

    _set->addProperty(_pos.property());
    _set->addProperty(_size.property());
    _set->addProperty(_chartType);
    _set->addProperty(_chartSubType);
    _set->addProperty(_font);
    _set->addProperty(_colorScheme);
    _set->addProperty(_threeD);
    _set->addProperty(_aa);
    _set->addProperty(_xTitle);
    _set->addProperty(_yTitle);
    _set->addProperty(_bgColor);
    _set->addProperty(_displayLegend);
    _set->addProperty ( _linkMaster );
    _set->addProperty ( _linkChild );

    set3D(false);
    setAA(false);
    setColorScheme("default");
}

void KRChartData::set3D(bool td)
{
    if (_chartWidget && _chartWidget->barDiagram()) {
        KDChart::BarDiagram *bar = _chartWidget->barDiagram();
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
    if (_chartWidget && _chartWidget->diagram()) {
        _chartWidget->diagram()->setAntiAliasing(aa);
    }
}

void KRChartData::setColorScheme(const QString &cs)
{
    if (_chartWidget && _chartWidget->diagram()) {
        if (cs == "rainbow") {
            _chartWidget->diagram()->useRainbowColors();
        } else if (cs == "subdued") {
            _chartWidget->diagram()->useSubduedColors();
        } else {
            _chartWidget->diagram()->useDefaultColors();
        }
    }
}

void KRChartData::setConnection(KexiDB::Connection *c)
{
    _conn = c;
}

void KRChartData::populateData()
{
    KexiDB::Cursor* curs = 0;
    QVector<datalist> data;
    QStringList labels;

    QStringList fn;

    delete _chartWidget;
    _chartWidget = 0;

    if (_conn) {
        curs = dataSet();

        if (curs) {
            fn = fieldNamesHackUntilImprovedParser(_dataSource->value().toString());

            //resize the data lists to match the number of columns
            int cols = fn.count() - 1;
            data.resize(cols);

            _chartWidget = new KDChart::Widget();
            //_chartWidget->setStyle ( new QMotifStyle() );

            _chartWidget->setType((KDChart::Widget::ChartType) _chartType->value().toInt());
            _chartWidget->setSubType((KDChart::Widget::SubType) _chartSubType->value().toInt());
            set3D(_threeD->value().toBool());
            setAA(_aa->value().toBool());
            setColorScheme(_colorScheme->value().toString());
            setBackgroundColor(_bgColor->value().value<QColor>());
            curs->moveFirst();
            while (!curs->eof()) {
                labels << curs->value(0).toString();

                for (int i = 1; i <= cols; ++i) {
                    data[i - 1] << curs->value(i).toDouble();
                }
                curs->moveNext();
            }
            kDebug() << labels;
            if (data.size() > 0) {
                kDebug() << data[0];
            }
            for (int i = 1; i <= cols; ++i) {
                _chartWidget->setDataset(i - 1, data[i - 1], fn[i]);
            }

            setLegend(_displayLegend->value().toBool());



            //Add the axis
            setAxis(_xTitle->value().toString(), _yTitle->value().toString());

            //Add the bottom labels
            if (_chartWidget->barDiagram() || _chartWidget->lineDiagram()) {
                KDChart::AbstractCartesianDiagram *dia = dynamic_cast<KDChart::AbstractCartesianDiagram*>(_chartWidget->diagram());

                foreach(KDChart::CartesianAxis* axis, dia->axes()) {
                    if (axis->position() == KDChart::CartesianAxis::Bottom) {
                        axis->setLabels(labels);
                    }
                }
            }


        } else {
            kDebug() << "Cursor was invalid: " << _dataSource->value().toString();
        }
    } else {
        kDebug() << "No connection!";
    }
}

QStringList KRChartData::masterFields()
{
    return _linkMaster->value().toString().split(",");
}

void KRChartData::setLinkData(QString fld, QVariant val)
{
    kDebug() << "Field: " << fld << "is" << val;
    _links[fld] = val;
}

QStringList KRChartData::fieldNames(const QString &stmt)
{
    KexiDB::Parser *pars;

    pars = new KexiDB::Parser(_conn);

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
    QString ds = _dataSource->value().toString();
    KexiDB::Field::List fl;
    QString s;

    if (_conn && _conn->tableSchema(ds)) {
        kDebug() << ds << "is a table";
        fn = _conn->tableSchema(ds)->names();
    } else if (_conn && _conn->querySchema(ds)) {
        kDebug() << ds << "is a query";
        fn = _conn->querySchema(ds)->names();
    } else {
        QString s = stmt.mid(6, stmt.indexOf("from", 0, Qt::CaseInsensitive) - 6).simplified();
        kDebug() << s;
        fn = s.split(",");
    }
    return fn;
}


void KRChartData::setAxis(const QString& xa, const QString &ya)
{
    Q_ASSERT(_chartWidget);

    if (_chartWidget->barDiagram() || _chartWidget->lineDiagram()) {
        KDChart::AbstractCartesianDiagram *dia = dynamic_cast<KDChart::AbstractCartesianDiagram*>(_chartWidget->diagram());
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
            xAxis =  new KDChart::CartesianAxis(dynamic_cast<KDChart::AbstractCartesianDiagram*>(_chartWidget->diagram()));
            xAxis->setPosition(KDChart::CartesianAxis::Bottom);
            dia->addAxis(xAxis);
        }

        if (!yAxis) {
            yAxis = new KDChart::CartesianAxis(dynamic_cast<KDChart::AbstractCartesianDiagram*>(_chartWidget->diagram()));
            yAxis->setPosition(KDChart::CartesianAxis::Left);
            dia->addAxis(yAxis);
        }

        xAxis->setTitleText(xa);
        yAxis->setTitleText(ya);
    }
}

KexiDB::Cursor *KRChartData::dataSet()
{
    QString ds = _dataSource->value().toString();
    KexiDB::Cursor *c = 0;
    KexiDB::QuerySchema *qs;

    //Determin the type of the source data and create a queryschema
    if (_conn && _conn->tableSchema(ds)) {
        kDebug() << ds << "is a table";
        qs = new KexiDB::QuerySchema(*(_conn->tableSchema(ds)));
    } else if (_conn && _conn->querySchema(ds)) {
        kDebug() << ds << "is a query";
        qs = _conn->querySchema(ds);
    } else {
        kDebug() << ds << "is a statement";
        KexiDB::Parser *pars;
        pars = new KexiDB::Parser(_conn);
        pars->setOperation(KexiDB::Parser::OP_Select);
        pars->parse(ds);
        qs = pars->select();
    }

    //Now go through each master field value, and add the correspanding child field
    //as a where condition on the query
    if (qs) {
        QStringList childFields = _linkChild->value().toString().split(",");
        QStringList masterFields = _linkMaster->value().toString().split(",");

        for(int i = 0; i < childFields.size(); ++i){
            KexiDB::Field *f = qs->findTableField(childFields[i]);
            //Only add the condition if we found the child field, and we have data for the master field
            if (f && _links.contains(masterFields[i])) {
                qs->addToWhereExpression(f, _links[masterFields[i]]);
            }
        }

        c = _conn->executeQuery(*qs, 1);
    }
    return c;
}
void KRChartData::setBackgroundColor(const QColor&)
{
    //Set the background color
    KDChart::Chart *cht = _chartWidget->diagram()->coordinatePlane()->parent();

    KDChart::BackgroundAttributes ba;

    ba.setVisible(true);
    ba.setBrush(_bgColor->value().value<QColor>());
    cht->setBackgroundAttributes(ba);
}

void KRChartData::setLegend(bool le)
{
    //Add the legend
    if (_chartWidget) {
        if (le) {
            QStringList fn = fieldNamesHackUntilImprovedParser(_dataSource->value().toString());

            _chartWidget->addLegend(KDChart::Position::East);
            _chartWidget->legend()->setOrientation(Qt::Horizontal);
            _chartWidget->legend()->setTitleText("Legend");
            for (uint i = 1; i < (uint)fn.count(); ++i) {
                _chartWidget->legend()->setText(i - 1, fn[i]);
            }

            _chartWidget->legend()->setShowLines(true);
        } else {
            if (_chartWidget->legend()) {
                _chartWidget->takeLegend(_chartWidget->legend());
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
