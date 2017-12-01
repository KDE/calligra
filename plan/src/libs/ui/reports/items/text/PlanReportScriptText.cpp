/* This file is part of the KDE project
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2016 by Dag Andersen <danders@get2net.dk>
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
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "PlanReportScriptText.h"

#include <QFile>
#include <QTextStream>

namespace Scripting
{

Text::Text(PlanReportItemText* t)
{
    m_text = t;
}


Text::~Text()
{
}

QString Text::source() const
{
    return m_text->itemDataSource();
}

void Text::setSource(const QString& s)
{
    m_text->m_controlSource->setValue(s);
}

int Text::horizontalAlignment() const
{
    const QString a = m_text->m_horizontalAlignment->value().toString().toLower();

    if (a == QLatin1String("left")) {
        return -1;
    }
    if (a == QLatin1String("center")) {
        return 0;
    }
    if (a == QLatin1String("right")) {
        return 1;
    }
    return -1;
}
void Text::setHorizonalAlignment(int a)
{
    switch (a) {
    case -1:
        m_text->m_horizontalAlignment->setValue(QLatin1String("left"));
        break;
    case 0:
        m_text->m_horizontalAlignment->setValue(QLatin1String("center"));
        break;
    case 1:
        m_text->m_horizontalAlignment->setValue(QLatin1String("right"));
        break;
    default:
        m_text->m_horizontalAlignment->setValue(QLatin1String("left"));
        break;
    }
}

int Text::verticalAlignment() const
{
    const QString a = m_text->m_horizontalAlignment->value().toString().toLower();

    if (a == QLatin1String("top")) {
        return -1;
    }
    if (a == QLatin1String("middle")) {
        return 0;
    }
    if (a == QLatin1String("bottom")) {
        return 1;
    }
    return -1;
}
void Text::setVerticalAlignment(int a)
{
    switch (a) {
    case -1:
        m_text->m_verticalAlignment->setValue(QLatin1String("top"));
        break;
    case 0:
        m_text->m_verticalAlignment->setValue(QLatin1String("middle"));
        break;
    case 1:
        m_text->m_verticalAlignment->setValue(QLatin1String("bottom"));
        break;
    default:
        m_text->m_verticalAlignment->setValue(QLatin1String("middle"));
        break;
    }
}

QColor Text::backgroundColor() const
{
    return m_text->m_backgroundColor->value().value<QColor>();
}
void Text::setBackgroundColor(const QColor& c)
{
    m_text->m_backgroundColor->setValue(QColor(c));
}

QColor Text::foregroundColor() const
{
    return m_text->m_foregroundColor->value().value<QColor>();
}
void Text::setForegroundColor(const QColor& c)
{
    m_text->m_foregroundColor->setValue(QColor(c));
}

int Text::backgroundOpacity() const
{
    return m_text->m_backgroundOpacity->value().toInt();
}
void Text::setBackgroundOpacity(int o)
{
    m_text->m_backgroundOpacity->setValue(o);
}

QColor Text::lineColor() const
{
    return m_text->m_lineColor->value().value<QColor>();
}
void Text::setLineColor(const QColor& c)
{
    m_text->m_lineColor->setValue(QColor(c));
}

int Text::lineWeight() const
{
    return m_text->m_lineWeight->value().toInt();
}
void Text::setLineWeight(int w)
{
    m_text->m_lineWeight->setValue(w);
}

int Text::lineStyle() const
{
    return m_text->m_lineStyle->value().toInt();
}
void Text::setLineStyle(int s)
{
    if (s < 0 || s > 5) {
        s = 1;
    }
    m_text->m_lineStyle->setValue(s);
}

QPointF Text::position() const
{
    return m_text->m_pos.toPoint();
}
void Text::setPosition(const QPointF& p)
{
    m_text->m_pos.setPointPos(p);
}

QSizeF Text::size() const
{
    return m_text->m_size.toPoint();
}
void Text::setSize(const QSizeF& s)
{
    m_text->m_size.setPointSize(s);
}

void Text::loadFromFile(const QString &fn)
{
    QFile file(fn);
    //kreportpluginDebug() << "Loading from" << fn;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_text->m_controlSource->setValue(tr("$Unable to read %1").arg(fn));
        return;
    }
    QTextStream in(&file);
    QString data = in.readAll();
    /*
    while (!in.atEnd()) {
      QString line = in.readLine();
      process_line(line);
    }*/
    m_text->m_controlSource->setValue(QVariant(QLatin1String("$") + data));
}

}
