/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "VoiceElement.h"

namespace MusicCore
{

class VoiceElement::Private
{
public:
    Staff *staff;
    int length;
    qreal x;
    qreal y;
    qreal width;
    qreal height;
    VoiceBar *voiceBar;
    qreal beatline;
};

VoiceElement::VoiceElement(int length)
    : d(new Private)
{
    d->staff = nullptr;
    d->length = length;
    d->x = 0;
    d->y = 0;
    d->width = 0;
    d->height = 0;
    d->voiceBar = nullptr;
    d->beatline = 0;
}

VoiceElement::~VoiceElement()
{
    delete d;
}

Staff *VoiceElement::staff() const
{
    return d->staff;
}

void VoiceElement::setStaff(Staff *staff)
{
    d->staff = staff;
}

VoiceBar *VoiceElement::voiceBar() const
{
    return d->voiceBar;
}

void VoiceElement::setVoiceBar(VoiceBar *voiceBar)
{
    d->voiceBar = voiceBar;
}

qreal VoiceElement::x() const
{
    return d->x;
}

void VoiceElement::setX(qreal x)
{
    if (d->x == x)
        return;
    d->x = x;
    Q_EMIT xChanged(x);
}

qreal VoiceElement::y() const
{
    return d->y;
}

void VoiceElement::setY(qreal y)
{
    if (d->y == y)
        return;
    d->y = y;
    Q_EMIT yChanged(y);
}

qreal VoiceElement::width() const
{
    return d->width;
}

void VoiceElement::setWidth(qreal width)
{
    if (d->width == width)
        return;
    d->width = width;
    Q_EMIT widthChanged(width);
}

qreal VoiceElement::height() const
{
    return d->height;
}

void VoiceElement::setHeight(qreal height)
{
    if (d->height == height)
        return;
    d->height = height;
    Q_EMIT heightChanged(height);
}

int VoiceElement::length() const
{
    return d->length;
}

void VoiceElement::setLength(int length)
{
    if (d->length == length)
        return;
    d->length = length;
    Q_EMIT lengthChanged(length);
}

qreal VoiceElement::beatline() const
{
    return d->beatline;
}

void VoiceElement::setBeatline(qreal beatline)
{
    d->beatline = beatline;
}

} // namespace MusicCore
