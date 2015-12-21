/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "HyperlinkStrategy.h"

#include "Selection.h"
#include "Sheet.h"
#include "Util.h"
#include "CellToolBase.h"

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoToolBase.h>

#include <kmessagebox.h>
#include <krun.h>

#include <QMimeDatabase>
#include <QTimer>
#include <QUrl>

using namespace Calligra::Sheets;

class HyperlinkStrategy::Private
{
public:
    QPointF lastPoint;
    QRectF textRect;
    QString url;
};

HyperlinkStrategy::HyperlinkStrategy(CellToolBase *cellTool,
                                     const QPointF& documentPos, Qt::KeyboardModifiers modifiers,
                                     const QString& url, const QRectF& textRect)
        : AbstractSelectionStrategy(cellTool, documentPos, modifiers)
        , d(new Private)
{
    d->lastPoint = documentPos;
    d->textRect = textRect;
    d->textRect.moveTo(d->lastPoint); // turn textRect which is relative to the cell to an absolute coordinate
    d->url = url;
}

HyperlinkStrategy::~HyperlinkStrategy()
{
    delete d;
}

void HyperlinkStrategy::handleMouseMove(const QPointF& documentPos, Qt::KeyboardModifiers modifiers)
{
    const QPointF position = documentPos - cellTool()->offset();
    d->lastPoint = position;
    if (d->textRect.contains(position))
        return;
    AbstractSelectionStrategy::handleMouseMove(documentPos, modifiers);
}

void HyperlinkStrategy::finishInteraction(Qt::KeyboardModifiers modifiers)
{
    if (!d->textRect.contains(d->lastPoint)) {
        return;
    }
    Q_UNUSED(modifiers)
    selection()->activeSheet()->showStatusMessage(i18n("Link %1 activated", d->url));

    const QUrl url(d->url);
    if (!url.isValid() || url.isRelative()) {
        const Region region(d->url, selection()->activeSheet()->map(), selection()->activeSheet());
        if (region.isValid()) {
            if (region.firstSheet() != selection()->activeSheet()) {
                selection()->emitVisibleSheetRequested(region.firstSheet());
            }
            selection()->initialize(region);

            if (!region.firstRange().isNull()) {
                const Cell cell = Cell(region.firstSheet(), region.firstRange().topLeft());
            }
        }
    } else {
        const QString type = QMimeDatabase().mimeTypeForUrl(url).name();
        if (!Util::localReferenceAnchor(d->url)) {
            const bool executable = KRun::isExecutableFile(url, type);
            if (executable) {
                const QString question = i18n("This link points to the program or script '%1'.\n"
                                              "Malicious programs can harm your computer. "
                                              "Are you sure that you want to run this program?", d->url);
                // this will also start local programs, so adding a "don't warn again"
                // checkbox will probably be too dangerous
                const int answer = KMessageBox::warningYesNo(tool()->canvas()->canvasWidget(), question,
                                   i18n("Open Link?"));
                if (answer != KMessageBox::Yes) {
                    return;
                }
            }
            new KRun(url, tool()->canvas()->canvasWidget(), 0);
        }
    }

    tool()->repaintDecorations();
}
