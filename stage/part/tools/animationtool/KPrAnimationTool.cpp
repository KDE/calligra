/* This file is part of the KDE project

   Copyright (C) 2008 C. Boemann <cbo@boemann.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrAnimationTool.h"

#include <klocale.h>

#include <QList>
#include <QLabel>
#include <QPainter>

#include <KoPointerEvent.h>
#include <KoPACanvas.h>
#include <KoViewConverter.h>
#include <KPrPageEffectDocker.h>
#include "KPrClickActionDocker.h"

KPrAnimationTool::KPrAnimationTool( KoCanvasBase *canvas )
    : KoToolBase( canvas )
{
}

KPrAnimationTool::~KPrAnimationTool()
{
}


void KPrAnimationTool::paint( QPainter &painter, const KoViewConverter &converter)
{
    Q_UNUSED( painter );
    Q_UNUSED( converter );
}


void KPrAnimationTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    useCursor(Qt::ArrowCursor);
    repaintDecorations();
    Q_UNUSED(toolActivation);
    Q_UNUSED(shapes);
}

void KPrAnimationTool::mousePressEvent( KoPointerEvent *event )
{
    event->ignore();
}

void KPrAnimationTool::mouseMoveEvent( KoPointerEvent *event )
{
    Q_UNUSED( event );
}

void KPrAnimationTool::mouseReleaseEvent( KoPointerEvent *event )
{
    Q_UNUSED( event );
}


QList<QWidget *> KPrAnimationTool::createOptionWidgets()
{
    KPrPageEffectDocker *effectWidget = new KPrPageEffectDocker( );
    effectWidget->setView((dynamic_cast<KoPACanvas *>(canvas()))->koPAView());

    KPrClickActionDocker *clickActionWidget = new KPrClickActionDocker();
    clickActionWidget->setView((dynamic_cast<KoPACanvas *>(canvas()))->koPAView());

    QList<QWidget *> widgets;
    effectWidget->setWindowTitle(i18n("Slide Transitions"));
    widgets.append(effectWidget);
    clickActionWidget->setWindowTitle(i18n("Shape Click Actions"));
    widgets.append(clickActionWidget);
    return widgets;
}

#include "KPrAnimationTool.moc"
