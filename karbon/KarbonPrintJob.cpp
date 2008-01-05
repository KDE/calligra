/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#include "KarbonPrintJob.h"
#include "KarbonView.h"
#include "KarbonCanvas.h"

#include <KoShapeManager.h>

KarbonPrintJob::KarbonPrintJob(KarbonView *view)
    : KoPrintingDialog(view),
    m_view(view)
{
    setShapeManager( m_view->canvasWidget()->shapeManager() );
    printer().setFromTo(1, 1);
}

void KarbonPrintJob::preparePage(int)
{
    // if we have any custom tabs, here is where can can read them out and do our thing.

    //TODO  clip to document
}

QList<KoShape*> KarbonPrintJob::shapesOnPage(int)
{
    return shapeManager()->shapes();
}

QList<QWidget*> KarbonPrintJob::createOptionWidgets() const
{
    return QList<QWidget*>();
}
