/* This file is part of the KDE project
 * Copyright (C) 2007 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
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
#include "MusicStyle.h"
using namespace MusicCore;

MusicStyle::MusicStyle() : m_font("Emmentaler")
{
    m_font.setPixelSize(35);
    m_staffLinePen.setWidthF(0.8);
    m_staffLinePen.setCapStyle(Qt::RoundCap);
    m_staffLinePen.setColor(Qt::black);
    m_stemPen.setWidthF(1.2);
    m_stemPen.setCapStyle(Qt::RoundCap);
    m_stemPen.setColor(Qt::black);
}

MusicStyle::~MusicStyle()
{
}

QPen MusicStyle::staffLinePen()
{
    return m_staffLinePen;
}

QPen MusicStyle::stemPen()
{
    return m_stemPen;
}


void MusicStyle::renderNoteHead(QPainter& painter, double x, double y, Chord::Duration duration)
{
    painter.setPen(QPen(Qt::SolidLine));
    painter.setFont(m_font);
    QPointF p(x, y);
    switch (duration) {
        case Chord::HundredTwentyEighth:
        case Chord::SixtyFourth:
        case Chord::ThirtySecond:
        case Chord::Sixteenth:
        case Chord::Eighth:
        case Chord::Quarter:
            painter.drawText(p, QString(0xE125));
            break;
        case Chord::Half:
            painter.drawText(p, QString(0xE124));
            break;
        case Chord::Whole:
            painter.drawText(p, QString(0xE123));
            break;
        case Chord::Breve:
            painter.drawText(p, QString(0xE122));
            break;
    }
}

void MusicStyle::renderRest(QPainter& painter, double x, double y, Chord::Duration duration)
{
    painter.setPen(QPen(Qt::SolidLine));
    painter.setFont(m_font);
    QPointF p(x, y);
    switch (duration) {
        case Chord::HundredTwentyEighth:
            painter.drawText(p, QString(0xE10D));
            break;
        case Chord::SixtyFourth:
            painter.drawText(p, QString(0xE10C));
            break;
        case Chord::ThirtySecond:
            painter.drawText(p, QString(0xE10B));
            break;
        case Chord::Sixteenth:
            painter.drawText(p, QString(0xE10A));
            break;
        case Chord::Eighth:
            painter.drawText(p, QString(0xE109));
            break;
        case Chord::Quarter:
            painter.drawText(p, QString(0xE107));
            break;
        case Chord::Half:
            painter.drawText(p, QString(0xE101));
            break;
        case Chord::Whole:
            painter.drawText(p, QString(0xE100));
            break;
        case Chord::Breve:
            painter.drawText(p, QString(0xE106));
            break;
    }
}

void MusicStyle::renderClef(QPainter& painter, double x, double y, Clef::ClefShape shape)
{
    painter.setPen(QPen(Qt::SolidLine));
    painter.setFont(m_font);
    QPointF p(x, y);
    switch (shape) {
        case Clef::GClef:
            painter.drawText(p, QString(0xE195));
            break;
        case Clef::FClef:
            painter.drawText(p, QString(0xE193));
            break;
        case Clef::CClef:
            painter.drawText(p, QString(0xE191));
            break;
    }
}

void MusicStyle::renderAccidental(QPainter& painter, double x, double y, int accidental)
{
    painter.setPen(QPen(Qt::SolidLine));
    painter.setFont(m_font);
    QPointF p(x, y);
    switch (accidental) {
        case 0:
            painter.drawText(p, QString(0xE111));
            break;
        case 1:
            painter.drawText(p, QString(0xE10E));
            break;
        case 2:
            painter.drawText(p, QString(0xE114));
            break;
        case -1:
            painter.drawText(p, QString(0xE112));
            break;
        case -2:
            painter.drawText(p, QString(0xE114));
            break;
    }
}
#include "kdebug.h"
void MusicStyle::renderTimeSignatureNumber(QPainter& painter, double x, double y, double w, int number)
{
    painter.setPen(QPen(Qt::SolidLine));
    painter.setFont(m_font);
    QFontMetricsF m(m_font);
    QString txt = QString::number(number);

    painter.drawText(QPointF(x + (w - m.width(txt))/2, y), txt);
}
