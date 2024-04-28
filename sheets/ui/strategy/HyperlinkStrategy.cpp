/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "HyperlinkStrategy.h"

#include "../CellToolBase.h"
#include "../Selection.h"
#include "core/Sheet.h"
#include "engine/MapBase.h"
#include "engine/Util.h"

#include <KoCanvasBase.h>
#include <KoToolBase.h>

#include <KLocalizedString>
#include <KMessageBox>

#include <QMimeDatabase>

using namespace Calligra::Sheets;

class HyperlinkStrategy::Private
{
public:
    QPointF lastPoint;
    QRectF textRect;
    QString url;
};

HyperlinkStrategy::HyperlinkStrategy(CellToolBase *cellTool,
                                     const QPointF &documentPos,
                                     Qt::KeyboardModifiers modifiers,
                                     const QString &url,
                                     const QRectF &textRect)
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

void HyperlinkStrategy::handleMouseMove(const QPointF &documentPos, Qt::KeyboardModifiers modifiers)
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
        const Region region = selection()->activeSheet()->map()->regionFromName(d->url, selection()->activeSheet());
        if (region.isValid()) {
            Sheet *firstSheet = dynamic_cast<Sheet *>(region.firstSheet());
            if (firstSheet != selection()->activeSheet()) {
                selection()->emitVisibleSheetRequested(firstSheet);
            }
            selection()->initialize(region);
        }
    } else {
        const QString type = QMimeDatabase().mimeTypeForUrl(url).name();
        if (!Util::localReferenceAnchor(d->url)) {
            // const bool executable = KRun::isExecutableFile(url, type);
            // if (executable) {
            //     const QString question = i18n("This link points to the program or script '%1'.\n"
            //                                   "Malicious programs can harm your computer. "
            //                                   "Are you sure that you want to run this program?", d->url);
            //     // this will also start local programs, so adding a "don't warn again"
            //     // checkbox will probably be too dangerous
            //     const int answer = KMessageBox::warningTwoActions(tool()->canvas()->canvasWidget(), question,
            //                        i18n("Open Link?"), KStandardGuiItem::open(), KStandardGuiItem::cancel());
            //     if (answer != KMessageBox::PrimaryAction) {
            //         return;
            //     }
            // }
            // new KRun(url, tool()->canvas()->canvasWidget(), 0);
        }
    }

    tool()->repaintDecorations();
}
