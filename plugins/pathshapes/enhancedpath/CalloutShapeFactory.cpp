/* This file is part of the KDE project
 * Copyright (C) 2018 Dag Andersen <danders@get2net.dk>
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#include "CalloutShapeFactory.h"
#include "CalloutShape.h"

#include <KoShapeStroke.h>
#include <KoProperties.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoColorBackground.h>
#include <KoShapeLoadingContext.h>

#include <KoIcon.h>

#include <klocalizedstring.h>

#include <QString>

#include <math.h>

CalloutShapeFactory::CalloutShapeFactory()
    : KoShapeFactoryBase(CalloutShapeId, i18n("A callout shape"))
{
    setToolTip(i18n("A callout shape"));
    setIconName(koIconName("enhancedpath"));
    setXmlElementNames(KoXmlNS::draw, QStringList("custom-shape"));
    setLoadingPriority(5);

    addCallout();
}

KoShape *CalloutShapeFactory::createDefaultShape(KoDocumentResourceManager *) const
{
    Q_ASSERT(!templates().isEmpty());
    return createShape(templates().first().properties);
}

KoShape *CalloutShapeFactory::createShape(const KoProperties *params, KoDocumentResourceManager *) const
{
    Q_ASSERT(params->contains("viewBox"));
    QVariant viewboxData;
    params->property("viewBox", viewboxData);

    CalloutShape *shape = new CalloutShape(viewboxData.toRect());

    shape->setShapeId(KoPathShapeId);
    shape->setStroke(new KoShapeStroke(1.0));
    shape->setType(params->stringProperty("type"));
    shape->addModifiers(params->stringProperty("modifiers"));

    ListType handles = params->property("handles").toList();
    foreach (const QVariant &v, handles) {
        shape->addHandle(v.toMap());
    }

    ComplexType formulae = params->property("formulae").toMap();
    ComplexType::const_iterator formula = formulae.constBegin();
    ComplexType::const_iterator lastFormula = formulae.constEnd();
    for (; formula != lastFormula; ++formula) {
        shape->addFormula(formula.key(), formula.value().toString());
    }
    QStringList commands = params->property("commands").toStringList();
    foreach (const QString &cmd, commands) {
        shape->addCommand(cmd);
    }

    QVariant color;
    if (params->property("background", color)) {
        shape->setBackground(QSharedPointer<KoColorBackground>(new KoColorBackground(color.value<QColor>())));
    }
    QSizeF size = shape->size();
    if (size.width() > size.height()) {
        shape->setSize(QSizeF(100, 100 * size.height() / size.width()));
    } else {
        shape->setSize(QSizeF(100 * size.width() / size.height(), 100));
    }
    return shape;
}

KoProperties* CalloutShapeFactory::dataToProperties(
    const QString &modifiers, const QStringList &commands,
    const ListType &handles, const ComplexType & formulae) const
{
    KoProperties *props = new KoProperties();
    props->setProperty("modifiers", modifiers);
    props->setProperty("commands", commands);
    props->setProperty("handles", handles);
    props->setProperty("formulae", formulae);
    props->setProperty("background", QVariant::fromValue<QColor>(QColor(Qt::red)));

    return props;
}

void CalloutShapeFactory::addCallout()
{
    QString modifiers("4250 45000");

    QStringList commands;
    commands.append("M 3590 0");
    commands.append("X 0 3590");
    commands.append("L ?f2 ?f3 0 8970 0 12630 ?f4 ?f5 0 18010");
    commands.append("Y 3590 21600");
    commands.append("L ?f6 ?f7 8970 21600 12630 21600 ?f8 ?f9 18010 21600");
    commands.append("X 21600 18010");
    commands.append("L ?f10 ?f11 21600 12630 21600 8970 ?f12 ?f13 21600 3590");
    commands.append("Y 18010 0");
    commands.append("L ?f14 ?f15 12630 0 8970 0 ?f16 ?f17");
    commands.append("Z");
    commands.append("N");

    ComplexType formulae;
    formulae["f0"] = "$0 -10800";
    formulae["f1"] = "$1 -10800";
    formulae["f2"] = "if(?f18 ,$0 ,0)";
    formulae["f3"] = "if(?f18 ,$1 ,6280)";
    formulae["f4"] = "if(?f23 ,$0 ,0)";
    formulae["f5"] = "if(?f23 ,$1 ,15320)";
    formulae["f6"] = "if(?f26 ,$0 ,6280)";
    formulae["f7"] = "if(?f26 ,$1 ,21600)";
    formulae["f8"] = "if(?f29 ,$0 ,15320)";
    formulae["f9"] = "if(?f29 ,$1 ,21600)";
    formulae["f10"] = "if(?f32 ,$0 ,21600)";
    formulae["f11"] = "if(?f32 ,$1 ,15320)";
    formulae["f12"] = "if(?f34 ,$0 ,21600)";
    formulae["f13"] = "if(?f34 ,$1 ,6280)";
    formulae["f14"] = "if(?f36 ,$0 ,15320)";
    formulae["f15"] = "if(?f36 ,$1 ,0)";
    formulae["f16"] = "if(?f38 ,$0 ,6280)";
    formulae["f17"] = "if(?f38 ,$1 ,0)";
    formulae["f18"] = "if($0 ,-1,?f19)";
    formulae["f19"] = "if(?f1 ,-1,?f22)";
    formulae["f20"] = "abs(?f0)";
    formulae["f21"] = "abs(?f1)";
    formulae["f22"] = "?f20 -?f21";
    formulae["f23"] = "if($0 ,-1,?f24)";
    formulae["f24"] = "if(?f1 ,?f22 ,-1)";
    formulae["f25"] = "$1 -21600";
    formulae["f26"] = "if(?f25 ,?f27 ,-1)";
    formulae["f27"] = "if(?f0 ,-1,?f28)";
    formulae["f28"] = "?f21 -?f20";
    formulae["f29"] = "if(?f25 ,?f30 ,-1)";
    formulae["f30"] = "if(?f0 ,?f28 ,-1)";
    formulae["f31"] = "$0 -21600";
    formulae["f32"] = "if(?f31 ,?f33 ,-1)";
    formulae["f33"] = "if(?f1 ,?f22 ,-1)";
    formulae["f34"] = "if(?f31 ,?f35 ,-1)";
    formulae["f35"] = "if(?f1 ,-1,?f22)";
    formulae["f36"] = "if($1 ,-1,?f37)";
    formulae["f37"] = "if(?f0 ,?f28 ,-1)";
    formulae["f38"] = "if($1 ,-1,?f39)";
    formulae["f39"] = "if(?f0 ,-1,?f28)";
    formulae["f40"] = "$0";
    formulae["f41"] = "$1";

    ListType handles;
    ComplexType handle;
    handle["draw:handle-position"] = "$0 $1";
    handles.append(QVariant(handle));

    KoShapeTemplate t;
    t.id = KoPathShapeId;
    t.templateId = "rectangular";
    t.name = i18n("Rectangular Callout");
    t.family = "funny";
    t.toolTip = i18n("A rectangular callout");
    t.iconName = koIconName("callout-shape");
    KoProperties* properties = dataToProperties(modifiers, commands, handles, formulae);
    properties->setProperty("viewBox", QRect(0, 0, 21600, 21600));
    properties->setProperty("type", "rectangular-callout");
    t.properties = properties;

    addTemplate(t);
}



bool CalloutShapeFactory::supports(const KoXmlElement & e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(context);
    if (e.localName() == "custom-shape" && e.namespaceURI() == KoXmlNS::draw) {
        KoXmlElement x = KoXml::namedItemNS(e, KoXmlNS::draw, "enhanced-geometry");
        if (x.isNull()) {
            return false;
        }
        if (x.attributeNS(KoXmlNS::draw, "type").contains("callout")) {
            return true;
        }
    }
    return false;
}
