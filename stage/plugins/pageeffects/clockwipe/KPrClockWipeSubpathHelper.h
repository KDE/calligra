/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRCLOCKWIPESUBPATHHELPER_H
#define KPRCLOCKWIPESUBPATHHELPER_H

#include <QRect>

class QPainterPath;

class KPrClockWipeSubpathHelper
{
public:
    KPrClockWipeSubpathHelper();
    virtual ~KPrClockWipeSubpathHelper();

    static void addSubpathForCircularArc(QPainterPath *clipPath, QRect &boundingRect, double startAngle, double endAngle);
};

#endif // KPRCLOCKWIPESUBPATHHELPER_H
