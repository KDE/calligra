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

#include "DivineProportionTool.h"

//   #include <KoCanvasBase.h>
//   #include <KoSelection.h>
//   #include <KoShapeManager.h>
#include <KoPointerEvent.h>
//
//   #include <kdebug.h>
//   #include <KStandardShortcut>
//   #include <KAction>
//   #include <KStandardAction>
//   #include <QAbstractTextDocumentLayout>
//   #include <QAction>
//   #include <QBuffer>
//   #include <QTextBlock>
//   #include <QTabWidget>
//   #include <QTextLayout>
//   #include <QKeyEvent>
//   #include <QUndoCommand>
//   #include <QPointer>
//   #include <QMenu>
//   #include <QClipboard>

DivineProportionTool::DivineProportionTool(KoCanvasBase *canvas)
    : KoTool(canvas)
{
}

DivineProportionTool::~DivineProportionTool() {
}

void DivineProportionTool::paint( QPainter &painter, KoViewConverter &converter) {
}

void DivineProportionTool::mousePressEvent( KoPointerEvent *event ) {
    event->ignore();
}

void DivineProportionTool::mouseMoveEvent( KoPointerEvent *event ) {
    event->ignore();
}

void DivineProportionTool::mouseReleaseEvent( KoPointerEvent *event ) {
    event->ignore();
}

void DivineProportionTool::activate (bool temporary) {
}

void DivineProportionTool::deactivate() {
}


#include "DivineProportionTool.moc"
