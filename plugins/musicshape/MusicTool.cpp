/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include <QPainter>
#include <QTabWidget>

#include "MusicDebug.h"
#include <KLocalizedString>

#include <KoCanvasBase.h>
#include <KoSelection.h>

#include "MusicShape.h"

#include "MusicTool.h"

#include "dialogs/PartsWidget.h"

MusicTool::MusicTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
    , m_musicshape(nullptr)
{
}

MusicTool::~MusicTool() = default;

void MusicTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes)
{
    Q_UNUSED(toolActivation);
    // debugMusic ;

    foreach (KoShape *shape, shapes) {
        m_musicshape = dynamic_cast<MusicShape *>(shape);
        if (m_musicshape)
            break;
    }
    if (!m_musicshape) {
        Q_EMIT done();
        return;
    }
    Q_EMIT shapeChanged(m_musicshape);
    useCursor(Qt::ArrowCursor);
}

void MusicTool::deactivate()
{
    // debugMusic<<"MusicTool::deactivate";
    m_musicshape = nullptr;
}

void MusicTool::paint(QPainter &painter, const KoViewConverter &viewConverter)
{
    Q_UNUSED(painter);
    Q_UNUSED(viewConverter);
}

void MusicTool::mousePressEvent(KoPointerEvent *)
{
}

void MusicTool::mouseMoveEvent(KoPointerEvent *)
{
}

void MusicTool::mouseReleaseEvent(KoPointerEvent *)
{
}

void MusicTool::addCommand(KUndo2Command *command)
{
    canvas()->addCommand(command);
}

QWidget *MusicTool::createOptionWidget()
{
    QTabWidget *widget = new QTabWidget();

    PartsWidget *pw = new PartsWidget(this, widget);
    widget->addTab(pw, i18n("Parts"));

    connect(this, &MusicTool::shapeChanged, pw, &PartsWidget::setShape);

    if (m_musicshape)
        pw->setShape(m_musicshape);

    return widget;
}

MusicShape *MusicTool::shape()
{
    return m_musicshape;
}
