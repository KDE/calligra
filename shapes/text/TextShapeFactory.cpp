/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
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
#include "TextShapeFactory.h"
#include "TextShape.h"

#include <klocale.h>

#include <KoProperties.h>
#include <KoShapeGeometry.h>
#include <KoShape.h>

#include <KoTextShapeData.h>

TextShapeFactory::TextShapeFactory(QObject *parent)
    : KoShapeFactory(parent, TextShape_SHAPEID, i18n("Text"))
{
    setToolTip(i18n("A Shape That Shows Text"));

    KoShapeTemplate t;
    t.name = i18n("Text");
    t.icon = "text";
    t.toolTip = i18n("Text Shape");
    KoProperties *props = new KoProperties();
    t.properties = props;
    props->setProperty("demo", true);
    addTemplate(t);
}

KoShape *TextShapeFactory::createDefaultShape() const {
    TextShape *text = new TextShape();
    return text;
}

KoShape *TextShapeFactory::createShape(const KoProperties * params) const {
    TextShape *shape = new TextShape();
shape->resize(QSizeF(300, 200));
    shape->setDemoText( params->boolProperty("demo") );
shape->addConnectionPoint(QPointF(0, 0));
shape->addConnectionPoint(QPointF(150, 100));
shape->addConnectionPoint(QPointF(0, 200));
shape->addConnectionPoint(QPointF(300, 200));
    return shape;
}

#if 0
QList<KoShapeConfigWidgetBase*> TextShapeFactory::createShapeOptionPanels() {
    QList<KoShapeConfigWidgetBase*> answer;
    answer.append(new KoShapeGeometry());
    return answer;
}
#endif

#include "TextShapeFactory.moc"
