/*
 *  Copyright (c) 2014 Shivaraman Aiyer<sra392@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_perspective_grid_managerng.h"

#include <cfloat>

#include <QList>
#include <QPointF>
#include <klocale.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>

#include "kis_perspective_gridng.h"
#include <QPainter>

struct KisPerspectiveGridNgManager::Private {
    QList<KisPerspectiveGridNg*> assistants;
    KToggleAction* toggleAssistant;
    void updateAction() {
        toggleAssistant->setEnabled(!assistants.isEmpty());
    }
};

KisPerspectiveGridNgManager::KisPerspectiveGridNgManager(KisView2* parent) :
        KisCanvasDecoration("perspectiveGridNgManager", i18n("Perspective GridNg"), parent),
        d(new Private)
{
    qDebug() << "Shiva --> KisPerspectiveGridNgManager";
}

KisPerspectiveGridNgManager::~KisPerspectiveGridNgManager()
{
    qDeleteAll(d->assistants.begin(), d->assistants.end());
    delete d;
}

void KisPerspectiveGridNgManager::addAssistant(KisPerspectiveGridNg* assistant)
{
    if (d->assistants.contains(assistant)) return;
    d->assistants.push_back(assistant);
    d->updateAction();
}

void KisPerspectiveGridNgManager::removeAssistant(KisPerspectiveGridNg* assistant)
{
    delete assistant;
    d->assistants.removeAll(assistant);
    d->updateAction();
}

void KisPerspectiveGridNgManager::removeAll()
{
    foreach (KisPerspectiveGridNg* assistant, d->assistants) {
        delete assistant;
    }
    d->assistants.clear();
    d->updateAction();
}

QPointF KisPerspectiveGridNgManager::adjustPosition(const QPointF& point, const QPointF& strokeBegin)
{
    if (d->assistants.empty()) return point;
    if (d->assistants.count() == 1) {
        QPointF newpoint = d->assistants.first()->adjustPosition(point, strokeBegin);
        // check for NaN
        if (newpoint.x() != newpoint.x()) return point;
        return newpoint;
    }
    QPointF best = point;
    double distance = DBL_MAX;
    foreach(KisPerspectiveGridNg* assistant, d->assistants) {
        QPointF pt = assistant->adjustPosition(point, strokeBegin);
        if (pt.x() != pt.x()) continue;
        double d = qAbs(pt.x() - point.x()) + qAbs(pt.y() - point.y());
        if (d < distance) {
            best = pt;
            distance = d;
        }
    }
    return best;
}

void KisPerspectiveGridNgManager::endStroke()
{
    foreach(KisPerspectiveGridNg* assistant, d->assistants) {
        assistant->endStroke();
    }
}

void KisPerspectiveGridNgManager::setup(KActionCollection * collection)
{
    d->toggleAssistant = new KToggleAction(i18n("Show Painting Assistants"), this);
    collection->addAction("view_toggle_painting_assistants", d->toggleAssistant);
    connect(d->toggleAssistant, SIGNAL(triggered()), this, SLOT(toggleVisibility()));

    d->toggleAssistant->setCheckedState(KGuiItem(i18n("Hide Painting Assistants")));
    d->toggleAssistant->setChecked(false);
    d->updateAction();
}

void KisPerspectiveGridNgManager::drawDecoration(QPainter& gc, const QRectF& updateRect, const KisCoordinatesConverter *converter,KisCanvas2* canvas)
{
    foreach(KisPerspectiveGridNg* assistant, d->assistants) {
        assistant->drawAssistant(gc, updateRect, converter,canvas);
    }
}

QList<KisPerspectiveGridNgHandleSP> KisPerspectiveGridNgManager::handles()
{
    QList<KisPerspectiveGridNgHandleSP> hs;
    foreach(KisPerspectiveGridNg* assistant, d->assistants) {
        foreach(KisPerspectiveGridNgHandleSP handle, assistant->handles()) {
            if (!hs.contains(handle)) {
                hs.push_back(handle);
            }
        }
        hs.push_back(assistant->vanishingPointX());
        hs.push_back(assistant->vanishingPointY());
        hs.push_back(assistant->vanishingPointZ());
//        foreach(const KisPerspectiveGridNgHandleSP handle, assistant->sideHandles()) {
//            if (!hs.contains(handle)) {
//                hs.push_back(handle);
//            }
//        }
    }
    return hs;
}

QList<KisPerspectiveGridNg*> KisPerspectiveGridNgManager::assistants()
{
    return d->assistants;
}

#include "kis_perspective_grid_managerng.moc"
