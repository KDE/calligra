/*
 * Copyright (c) 2010 Adam Celarek <kdedev at xibo dot at>
 * Copyright (C) 2012 Cyrille Berger <cberger@cberger.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 sure
 * USA
 */

#include "kis_color_selector_interface.h"
#include <KoCanvasResourceManager.h>

KisColorSelectorInterface::KisColorSelectorInterface()
        : m_blipDisplay(true), m_colorUpdateAllowed(true)
{
}

KisColorSelectorInterface::~KisColorSelectorInterface()
{
}

void KisColorSelectorInterface::setDisplayBlip(bool disp)
{
    m_blipDisplay = disp;
}

bool KisColorSelectorInterface::displayBlip() const
{
    return m_blipDisplay;
}

void KisColorSelectorInterface::commitColor(const KoColor& color, ColorRole role)
{
    KoCanvasResourceManager* canvasResourceManager = resourceManager();
    qDebug() << "commitColor " << canvasResourceManager;
    if (!canvasResourceManager)
        return;

    m_colorUpdateAllowed=false;

    if (role==Foreground)
        canvasResourceManager->setForegroundColor(color);
    else
        canvasResourceManager->setBackgroundColor(color);

    m_colorUpdateAllowed=true;
}

inline qreal distance(const QColor& c1, const QColor& c2)
{
    qreal dr = c1.redF()-c2.redF();
    qreal dg = c1.greenF()-c2.greenF();
    qreal db = c1.blueF()-c2.blueF();

    return sqrt(dr*dr+dg*dg+db*db);
}

inline bool inRange(qreal m) {
    if(m>=0. && m<=1.) return true;
    else return false;
}

inline bool modify(QColor* estimate, const QColor& target, const QColor& result)
{
    qreal r = estimate->redF() - (result.redF() - target.redF());
    qreal g = estimate->greenF() - (result.greenF() - target.greenF());
    qreal b = estimate->blueF() - (result.blueF() - target.blueF());

    if(inRange(r) && inRange(g) && inRange(b)) {
        estimate->setRgbF(r, g, b);
        return true;
    }
    else {
        return false;
    }
}

QColor KisColorSelectorInterface::findGeneratingColor(const KoColor& ref) const
{
//    kDebug() << "starting search for generating colour";
    KoColor converter(colorSpace());
    QColor currentEstimate;
    ref.toQColor(&currentEstimate);
//    kDebug() << "currentEstimate: " << currentEstimate;

    QColor currentResult;
    converter.fromQColor(currentEstimate);
    converter.toQColor(&currentResult);
//    kDebug() << "currentResult: " << currentResult;


    QColor target;
    ref.toQColor(&target);
//    kDebug() << "target: " << target;

    bool estimateValid=true;
    int iterationCounter=0;

//    kDebug() << "current distance = " << distance(target, currentResult);
    while(distance(target, currentResult)>0.001 && estimateValid && iterationCounter<100) {
        estimateValid = modify(&currentEstimate, target, currentResult);
        converter.fromQColor(currentEstimate);
        converter.toQColor(&currentResult);
//        kDebug() << "current distance = " << distance(target, currentResult);

        iterationCounter++;
    }

//    kDebug() << "end search for generating colour";

    return currentEstimate;
}
