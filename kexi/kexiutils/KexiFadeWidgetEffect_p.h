/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) version 3.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef KEXIFADEWIDGETEFFECT_P_H
#define KEXIFADEWIDGETEFFECT_P_H

#include "KexiFadeWidgetEffect.h"

#include <QtCore/QTimeLine>
#include <QPixmap>

class KexiFadeWidgetEffectPrivate
{
public:
    KexiFadeWidgetEffectPrivate(QWidget *_destWidget);
    KexiFadeWidgetEffect *q;

    QPixmap transition(const QPixmap &from, const QPixmap &to, qreal amount) const;
    void finished();

    QTimeLine timeLine;
    QPixmap oldPixmap;
    QPixmap newPixmap;
    QWidget *destWidget;
    bool disabled;
    int defaultDuration;
};

#endif // KEXIFADEWIDGETEFFECT_P_H
