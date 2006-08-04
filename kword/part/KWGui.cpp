/* This file is part of the KDE project
 * Copyright (C) 2001 David Faure <faure@kde.org>
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
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
 * Boston, MA 02110-1301, USA
 */

#include "KWGui.h"
#include "KWView.h"
#include "KWDocument.h"
#include "KWCanvas.h"

#include "KoCanvasController.h"
#include "KoToolManager.h"
#include "KoShapeSelector.h"

#include <QGridLayout>

KWGui::KWGui( const QString& viewMode, KWView *parent )
  : QWidget( parent)
{
    QGridLayout *gridLayout = new QGridLayout( this );
    gridLayout->setMargin(0);
    gridLayout->setSpacing(0);
    m_canvas = new KWCanvas( viewMode, static_cast<KWDocument*>(parent->koDocument()), parent, this );
    m_canvasController = new KoCanvasController(this);
    m_canvasController->setCanvas(m_canvas);
    KoToolManager::instance()->addControllers(m_canvasController, static_cast<KWDocument*>(parent->koDocument()));

KoShapeSelector *selector = new KoShapeSelector(0, m_canvasController, ".*");
selector->resize(QSize(100, 200));
selector->show();

    gridLayout->addWidget( m_canvasController, 0, 0 );
}
KWGui::~KWGui() {
    KoToolManager::instance()->removeCanvasController(m_canvasController);
}

int KWGui::visibleWidth() const {
    return m_canvasController->visibleWidth();
}

int KWGui::visibleHeight() const {
    return m_canvasController->visibleHeight();
}

bool KWGui::verticalScrollBarVisible() {
    return m_canvasController->verticalScrollBar() &&
        m_canvasController->verticalScrollBar()->isVisible();
}

#include "KWGui.moc"
