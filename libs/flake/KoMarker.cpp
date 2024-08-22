/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoMarker.h"

#include "KoOdfWorkaround.h"
#include "KoPathShape.h"
#include "KoPathShapeLoader.h"
#include "KoShapeLoadingContext.h"
#include "KoShapeSavingContext.h"
#include <KoGenStyle.h>
#include <KoGenStyles.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>

#include <QPainterPath>
#include <QString>
#include <QUrl>

class Q_DECL_HIDDEN KoMarker::Private
{
public:
    Private() = default;

    QString name;
    QString d;
    QPainterPath path;
    QRect viewBox;
};

KoMarker::KoMarker()
    : d(new Private())
{
}

KoMarker::~KoMarker()
{
    delete d;
}

bool KoMarker::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    Q_UNUSED(context);
    // A shape uses draw:marker-end="Arrow" draw:marker-end-width="0.686cm" draw:marker-end-center="true" which marker and how the marker is used

    //<draw:marker draw:name="Arrow" svg:viewBox="0 0 20 30" svg:d="m10 0-10 30h20z"/>
    //<draw:marker draw:name="Arrowheads_20_1" draw:display-name="Arrowheads 1" svg:viewBox="0 0 10 10" svg:d="m0 0h10v10h-10z"/>

    d->d = element.attributeNS(KoXmlNS::svg, "d");
    if (d->d.isEmpty()) {
        return false;
    }

#ifndef NWORKAROUND_ODF_BUGS
    KoOdfWorkaround::fixMarkerPath(d->d);
#endif

    KoPathShape pathShape;
    KoPathShapeLoader loader(&pathShape);
    loader.parseSvg(d->d, true);

    d->path = pathShape.outline();
    d->viewBox = KoPathShape::loadOdfViewbox(element);

    QString displayName(element.attributeNS(KoXmlNS::draw, "display-name"));
    if (displayName.isEmpty()) {
        displayName = element.attributeNS(KoXmlNS::draw, "name");
    }
    d->name = displayName;
    return true;
}

QString KoMarker::saveOdf(KoShapeSavingContext &context) const
{
    KoGenStyle style(KoGenStyle::MarkerStyle);
    style.addAttribute("draw:display-name", d->name);
    style.addAttribute("svg:d", d->d);
    const QString viewBox = QString::fromLatin1("%1 %2 %3 %4").arg(d->viewBox.x()).arg(d->viewBox.y()).arg(d->viewBox.width()).arg(d->viewBox.height());
    style.addAttribute(QLatin1String("svg:viewBox"), viewBox);
    QString name = QString(QUrl::toPercentEncoding(d->name, "", " ")).replace('%', '_');
    return context.mainStyles().insert(style, name, KoGenStyles::DontAddNumberToName);
}

QString KoMarker::name() const
{
    return d->name;
}

QPainterPath KoMarker::path(qreal width) const
{
    if (!d->viewBox.isValid() || width == 0) {
        return QPainterPath();
    }

    // TODO: currently the <min-x>, <min-y> properties of viewbox are ignored, why? OOo-compat?
    qreal height = width * d->viewBox.height() / d->viewBox.width();

    QTransform transform;
    transform.scale(width / d->viewBox.width(), height / d->viewBox.height());
    return transform.map(d->path);
}

bool KoMarker::operator==(const KoMarker &other) const
{
    return (d->d == other.d->d && d->viewBox == other.d->viewBox);
}
