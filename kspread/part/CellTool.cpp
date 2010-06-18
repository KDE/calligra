/* This file is part of the KDE project
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Robert Knight <robertknight@gmail.com>
   Copyright 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 1999-2004 David Faure <faure@kde.org>
   Copyright 2004-2005 Meni Livne <livne@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Hamish Rodda <rodda@kde.org>
   Copyright 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright 2003 Lukas Tinkl <lukas@kde.org>
   Copyright 2000-2002 Werner Trobin <trobin@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Daniel Naber <daniel.naber@t-online.de>
   Copyright 1999-2000 Torben Weis <weis@kde.org>
   Copyright 1999-2000 Stephan Kulow <coolo@kde.org>
   Copyright 2000 Bernd Wuebben <wuebben@kde.org>
   Copyright 2000 Wilco Greven <greven@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "CellTool.h"

#include <QPainter>

#include <KAction>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <krun.h>
#include <KSelectAction>

#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoViewConverter.h>

#include "Cell.h"
#include "Canvas.h"
#include "Canvas_p.h"
#include "Doc.h"
#include "Limits.h"
#include "PrintSettings.h"
#include "Selection.h"
#include "Sheet.h"
#include "StyleManager.h"
#include "Util.h"
#include "View.h"

#include "commands/AutoFillCommand.h"
#include "commands/DefinePrintRangeCommand.h"

#include "ui/CellView.h"
#include "ui/Editors.h"
#include "ui/SheetView.h"

using namespace KSpread;

class CellTool::Private
{
public:
    Canvas* canvas;
};

CellTool::CellTool(KoCanvasBase* canvas)
        : CellToolBase(canvas)
        , d(new Private)
{
    d->canvas = static_cast<Canvas*>(canvas);

    KAction* action = 0;

    // -- misc actions --

    action = new KAction(i18n("Define Print Range"), this);
    addAction("definePrintRange", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(definePrintRange()));
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

void CellTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    canvas()->shapeManager()->selection()->deselectAll();
    CellToolBase::activate(toolActivation, shapes);
}

KSpread::Selection* CellTool::selection()
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

SheetView* CellTool::sheetView(const Sheet* sheet) const
{
    return d->canvas->view()->sheetView(sheet);
}

void CellTool::definePrintRange()
{
    DefinePrintRangeCommand* command = new DefinePrintRangeCommand();
    command->setSheet(selection()->activeSheet());
    command->add(*selection());
    d->canvas->view()->doc()->addCommand(command);
}

#include "CellTool.moc"
