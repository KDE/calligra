/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2006 Robert Knight <robertknight@gmail.com>
   SPDX-FileCopyrightText: 2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 1999-2002, 2004 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002-2005 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 1999-2004 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2004-2005 Meni Livne <livne@kde.org>
   SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2003 Hamish Rodda <rodda@kde.org>
   SPDX-FileCopyrightText: 2003 Joseph Wenninger <jowenn@kde.org>
   SPDX-FileCopyrightText: 2003 Lukas Tinkl <lukas@kde.org>
   SPDX-FileCopyrightText: 2000-2002 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 2002 Harri Porten <porten@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2002 Daniel Naber <daniel.naber@t-online.de>
   SPDX-FileCopyrightText: 1999-2000 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 1999-2000 Stephan Kulow <coolo@kde.org>
   SPDX-FileCopyrightText: 2000 Bernd Wuebben <wuebben@kde.org>
   SPDX-FileCopyrightText: 2000 Wilco Greven <greven@kde.org>
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org
   SPDX-FileCopyrightText: 1999 Michael Reiher <michael.reiher@gmx.de>
   SPDX-FileCopyrightText: 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "CellTool.h"
#include "CanvasBase.h"
#include "Doc.h"
#include "commands/DefinePrintRangeCommand.h"

#include <KLocalizedString>

#include <KoCanvasController.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoViewConverter.h>

#include "engine/calligra_sheets_limits.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN CellTool::Private
{
public:
    CanvasBase *canvas;
};

CellTool::CellTool(KoCanvasBase *canvas)
    : CellToolBase(canvas)
    , d(new Private)
{
    d->canvas = static_cast<CanvasBase *>(canvas);

    QAction *action = nullptr;

    // -- misc actions --

    action = new QAction(i18n("Define Print Range"), this);
    addAction("definePrintRange", action);
    connect(action, &QAction::triggered, this, &CellTool::definePrintRange);
    action->setToolTip(i18n("Define the print range in the current sheet"));
}

CellTool::~CellTool()
{
    delete d;
}

void CellTool::paint(QPainter &painter, const KoViewConverter &viewConverter)
{
    KoShape::applyConversion(painter, viewConverter);
    const double xOffset = viewConverter.viewToDocumentX(canvas()->canvasController()->canvasOffsetX());
    const double yOffset = viewConverter.viewToDocumentY(canvas()->canvasController()->canvasOffsetY());
    // The visible area in document coordinates:
    const QRectF paintRect = QRectF(QPointF(-xOffset, -yOffset), size());

    /* paint the selection */
    paintReferenceSelection(painter, paintRect);
    paintSelection(painter, paintRect);
}

void CellTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes)
{
    canvas()->shapeManager()->selection()->deselectAll();
    CellToolBase::activate(toolActivation, shapes);
}

Calligra::Sheets::Selection *CellTool::selection()
{
    return d->canvas->selection();
}

QPointF CellTool::offset() const
{
    return QPointF(0.0, 0.0);
}

QSizeF CellTool::size() const
{
    return canvas()->viewConverter()->viewToDocument(d->canvas->size());
}

QPointF CellTool::canvasOffset() const
{
    return d->canvas->offset();
}

int CellTool::maxCol() const
{
    return KS_colMax;
}

int CellTool::maxRow() const
{
    return KS_rowMax;
}

SheetView *CellTool::sheetView(Sheet *sheet) const
{
    return d->canvas->sheetView(sheet);
}

void CellTool::definePrintRange()
{
    DefinePrintRangeCommand *command = new DefinePrintRangeCommand();
    command->setSheet(selection()->activeSheet());
    command->add(*selection());
    d->canvas->doc()->addCommand(command);
}
