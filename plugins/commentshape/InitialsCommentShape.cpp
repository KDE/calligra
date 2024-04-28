/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "InitialsCommentShape.h"
#include "Globals.h"

#include <QPainter>

InitialsCommentShape::InitialsCommentShape()
    : KoShape()
    , m_active(true)
{
}

InitialsCommentShape::~InitialsCommentShape()
{
}

void InitialsCommentShape::saveOdf(KoShapeSavingContext & /*context*/) const
{
}

bool InitialsCommentShape::loadOdf(const KoXmlElement & /*element*/, KoShapeLoadingContext & /*context*/)
{
    return false;
}

void InitialsCommentShape::paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &)
{
    applyConversion(painter, converter);

    QLinearGradient gradient(initialsBoxPoint, QPointF(0, initialsBoxSize.height()));
    qreal lighterPos = 0.0;
    qreal darkerPos = 0.0;
    if (!m_active) {
        darkerPos = 1.0;
    } else {
        lighterPos = 1.0;
    }
    gradient.setColorAt(lighterPos, QColor(Qt::yellow));
    gradient.setColorAt(darkerPos, QColor(254, 201, 7));
    const QBrush brush(gradient);
    painter.setBrush(brush);

    painter.setPen(QPen(Qt::black, 0));

    painter.drawRect(QRectF(initialsBoxPoint, initialsBoxSize));

    painter.drawText(QRectF(initialsBoxPoint, initialsBoxSize), Qt::AlignCenter, m_initials);
}

void InitialsCommentShape::setInitials(const QString &initials)
{
    m_initials = initials;
}

QString InitialsCommentShape::initials()
{
    return m_initials;
}

bool InitialsCommentShape::isActive() const
{
    return m_active;
}

void InitialsCommentShape::setActive(bool activate)
{
    m_active = activate;
}

void InitialsCommentShape::toogleActive()
{
    setActive(!m_active);
    update();
}
