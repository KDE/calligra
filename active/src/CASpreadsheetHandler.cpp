/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "CASpreadsheetHandler.h"
#include "CADocumentController.h"
#include "CACanvasController.h"

#include <sheets/Sheet.h>
#include <sheets/Map.h>
#include <sheets/DocBase.h>
#include <sheets/part/CanvasItem.h>
#include <sheets/part/Doc.h>
#include <sheets/part/Find.h>
#include <sheets/ui/SheetView.h>

#include <KoPart.h>
#include <KoToolManager.h>
#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>

#include <KMimeType>
#include <KMimeTypeTrader>

#include <QColor>

class CASpreadsheetHandler::Private
{
public:
    Private()
    {
        currentSheetNum = 0;
        findText = 0;
        sheet = 0;
        canvasItem = 0;
        sheetView = 0;
        matchFound = false;
	matchPerSheet = 0;
	searchSheetNumber = 0;
    }
    int currentSheetNum;
    int matchPerSheet;
    int searchSheetNumber;
    bool matchFound;
    QString searchString;
    Calligra::Sheets::Find* findText;
    QList<QTextDocument*> texts;
    Calligra::Sheets::Doc* document;
    Calligra::Sheets::CanvasItem* canvasItem;
    Calligra::Sheets::Sheet* sheet;
    Calligra::Sheets::SheetView *sheetView;
};

CASpreadsheetHandler::CASpreadsheetHandler (CADocumentController* documentController)
    : CAAbstractDocumentHandler (documentController)
    , d (new Private())
{
    d->findText = new Calligra::Sheets::Find(this);
    connect (d->findText, SIGNAL(updateCanvas()), SLOT(updateCanvas()));
    connect (d->findText, SIGNAL(matchFound(KoFindMatch)), SLOT(findMatchFound(KoFindMatch)));
    connect (d->findText, SIGNAL(noMatchFound()), SLOT(findNoMatchFound()));
}

KoZoomMode::Mode CASpreadsheetHandler::preferredZoomMode() const
{
    return KoZoomMode::ZOOM_WIDTH;
}

CASpreadsheetHandler::~CASpreadsheetHandler()
{
    delete d;
}

KoDocument* CASpreadsheetHandler::document()
{
    return d->document;
}

bool CASpreadsheetHandler::openDocument (const QString& uri)
{
    QString error;
    QString mimetype = KMimeType::findByPath (uri)->name();
    KoPart *part = KMimeTypeTrader::createInstanceFromQuery<KoPart>(mimetype,
                      QLatin1String("CalligraPart"), 0, QString(), QVariantList(), &error);

    if (!part) {
        kDebug() << "Doc can't be openend" << error;
        return false;
    }

    d->document = static_cast<Calligra::Sheets::Doc*> (part->document());
    d->document->openUrl (KUrl (uri));

    setCanvas (dynamic_cast<KoCanvasBase*> (part->canvasItem()));
    KoToolManager::instance()->addController (documentController()->canvasController());
    d->canvasItem = dynamic_cast<Calligra::Sheets::CanvasItem*> (canvas());

    if (!d->canvasItem) {
        kDebug() << "Failed to fetch a canvas item";
        return false;
    }

    KoZoomHandler* zoomHandler = new KoZoomHandler();
    documentController()->canvasController()->setZoomHandler (zoomHandler);
    KoZoomController* zoomController = documentController()->canvasController()->zoomController();
    zoomController->setZoom (KoZoomMode::ZOOM_CONSTANT, 1.0);

    documentController()->canvasController()->setCanvasMode (KoCanvasController::Spreadsheet);

    if (d->canvasItem) {
        // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
        connect (documentController()->canvasController()->canvasControllerProxyObject(),
                 SIGNAL(moveDocumentOffset(QPoint)), d->canvasItem, SLOT(setDocumentOffset(QPoint)));
        // whenever the size of the document viewed in the canvas changes, inform the zoom controller
        connect (d->canvasItem, SIGNAL(documentSizeChanged(QSize)), this, SLOT(tellZoomControllerToSetDocumentSize(QSize)));
        d->canvasItem->update();
    }

    connect (documentController()->canvasController(), SIGNAL(needsCanvasResize(QSizeF)), SLOT(resizeCanvas(QSizeF)));
    connect (documentController()->canvasController(), SIGNAL(needCanvasUpdate()), SLOT(updateCanvas()));

    updateCanvas();
    documentController()->canvasController()->zoomToFit();
    emit sheetCountChanged();

    return true;
}

QStringList CASpreadsheetHandler::supportedMimetypes()
{
    QStringList supportedTypes;
    supportedTypes << "application/vnd.oasis.opendocument.spreadsheet" << "application/vnd.ms-excel";
    return supportedTypes;
}

void CASpreadsheetHandler::tellZoomControllerToSetDocumentSize (QSize size)
{
    documentController()->canvasController()->zoomController()->setDocumentSize (size);
}

void CASpreadsheetHandler::updateCanvas()
{
    dynamic_cast<Calligra::Sheets::CanvasItem*> (canvas())->update();
    updateDocumentSizeForActiveSheet();
}

void CASpreadsheetHandler::updateDocumentSizeForActiveSheet()
{
    d->sheet = dynamic_cast<Calligra::Sheets::CanvasItem*> (canvas())->activeSheet();
    //FIXME 1.5 is a hack to "fix" the wrong values below. Why is it wrong?
    documentController()->canvasController()->updateDocumentSize (
        d->sheet->cellCoordinatesToDocument (d->sheet->usedArea (false)).toRect().size()*1.5, false);
}

QString CASpreadsheetHandler::documentTypeName()
{
    return "spreadsheet";
}

void CASpreadsheetHandler::nextSheet()
{
    d->canvasItem = dynamic_cast<Calligra::Sheets::CanvasItem*> (canvas());
    if (!d->canvasItem)
        return;
    d->sheet = d->canvasItem->activeSheet();
    if (!d->sheet)
        return;
    Calligra::Sheets::DocBase* kspreadDoc = qobject_cast<Calligra::Sheets::DocBase*> (document());
    if (!kspreadDoc)
        return;
    d->sheet = kspreadDoc->map()->nextSheet (d->sheet);
    if (!d->sheet)
        return;
    d->currentSheetNum++;
    emit currentSheetNumChanged();
    d->canvasItem->setActiveSheet (d->sheet);
    documentController()->canvasController()->updateDocumentSize (d->sheet->cellCoordinatesToDocument (d->sheet->usedArea (false)).toRect().size(), false);
    if(!d->searchString.isEmpty()) {
       setSearchString(d->searchString);
    }
}

void CASpreadsheetHandler::previousSheet()
{
    d->canvasItem = dynamic_cast<Calligra::Sheets::CanvasItem*> (canvas());
    if (!d->canvasItem)
        return;
    d->sheet = d->canvasItem->activeSheet();
    if (!d->sheet)
        return;
    Calligra::Sheets::DocBase* kspreadDoc = dynamic_cast<Calligra::Sheets::DocBase*> (document());
    if (!kspreadDoc)
        return;
    d->sheet = kspreadDoc->map()->previousSheet (d->sheet);
    if (!d->sheet)
        return;
    d->currentSheetNum--;
    emit currentSheetNumChanged();
    d->canvasItem->setActiveSheet (d->sheet);
    documentController()->canvasController()->updateDocumentSize (d->sheet->cellCoordinatesToDocument (d->sheet->usedArea (false)).toRect().size(), false);
}

void CASpreadsheetHandler::gotoSheet(int sheetNumber, SearchDirection direction)
{
    if(direction == SearchForward) {
       d->currentSheetNum = sheetNumber;
    } else if(direction == SearchBackwards) {
       d->currentSheetNum = sheetNumber;
    }
    emit currentSheetNumChanged();
    d->canvasItem->setActiveSheet(d->sheet);
    documentController()->canvasController()->updateDocumentSize (d->sheet->cellCoordinatesToDocument (d->sheet->usedArea (false)).toRect().size(), false);
}


QString CASpreadsheetHandler::searchString() const
{
    return d->searchString;
}

void CASpreadsheetHandler::setSearchString (const QString& searchString)
{
    d->searchString = searchString;

    d->sheetView = d->canvasItem->sheetView(d->sheet);
    d->canvasItem->setActiveSheet(d->sheet);
    d->findText->setCurrentSheet(d->sheet,d->sheetView);
    d->findText->find(searchString);

    emit searchStringChanged();
}

void CASpreadsheetHandler::searchOtherSheets(SearchDirection direction) {
    int tempCurrentSheet = d->currentSheetNum;
    if(direction == SearchForward) {
       d->matchPerSheet = 0;
       if(d->searchSheetNumber < sheetCount()) {
           d->searchSheetNumber = currentSheetNumber();
       } else {
         return;
       }
    } else if(direction == SearchBackwards) {
       if(d->searchSheetNumber >= 0) {
          d->matchPerSheet = d->findText->matches().count() - 1;
          d->searchSheetNumber = d->currentSheetNum - 1;
       } else {
          return;
       }
    }

    Calligra::Sheets::DocBase* kspreadDoc = qobject_cast<Calligra::Sheets::DocBase*> (document());

    while((d->searchSheetNumber < sheetCount()) && (d->searchSheetNumber >= 0)) {
      d->sheet = kspreadDoc->map()->sheet(d->searchSheetNumber);
      d->sheet = kspreadDoc->map()->sheet(d->searchSheetNumber);
      setSearchString(d->searchString);
      if(d->matchFound == true) {
         gotoSheet(d->searchSheetNumber, direction);
         setSearchString(d->searchString);
         if(direction == SearchBackwards) {
            if(d->findText->matches().count() == 1) {
               d->matchPerSheet = 0;
            }
            if(d->searchSheetNumber == 0) {
               d->findText->findPrevious();
            }
         }
         break;
      }

      if(direction == SearchForward) {
         d->searchSheetNumber++;
      } else if(direction == SearchBackwards) {
         d->searchSheetNumber--;
      }
    }

    if(d->matchFound == false) {
       gotoSheet(tempCurrentSheet,direction);
       if(direction == SearchBackwards) {
          d->matchPerSheet = d->findText->matches().count();
          d->findText->findPrevious();
       } else if( direction == SearchForward) {
          d->matchPerSheet = 1;
       }
    }
}

void CASpreadsheetHandler::findNext() {
    d->matchPerSheet++;
    d->findText->findNext();
    if(d->matchPerSheet >= d->findText->matches().count()) {
       searchOtherSheets(SearchForward);
    }

}

void CASpreadsheetHandler::findPrevious() {
    d->matchPerSheet--;
    d->findText->findPrevious();
    if(d->matchPerSheet < 0) {
       searchOtherSheets(SearchBackwards);
    }
}

void CASpreadsheetHandler::findMatchFound (const KoFindMatch& match)
{
    QTextCursor cursor = match.location().value<QTextCursor>();
    updateCanvas();


    d->findText->setCurrentSheet(d->sheet,d->sheetView);
    d->sheetView->activeHighlight();

    d->sheetView->setHighlighted(match.location().value<Calligra::Sheets::Cell>().cellPosition(), true);

    d->canvasItem->resourceManager()->setResource(KoText::CurrentTextAnchor, cursor.anchor());
    d->canvasItem->resourceManager()->setResource(KoText::CurrentTextPosition, cursor.position());
    d->matchFound = true;
}

void CASpreadsheetHandler::findNoMatchFound()
{
    d->matchFound = false;
    kDebug() << "Match for " << searchString() << " not found";
}

void CASpreadsheetHandler::resizeCanvas (const QSizeF& canvasSize)
{
    canvas()->canvasItem()->setGeometry (QRectF (QPointF (0, 0), canvasSize));
}

int CASpreadsheetHandler::sheetCount() const
{
    return dynamic_cast<Calligra::Sheets::CanvasItem*> (canvas())->activeSheet()->map()->count();
}

QString CASpreadsheetHandler::leftToolbarSource() const
{
    return "SpreadsheetLeftToolbar.qml";
}

QString CASpreadsheetHandler::rightToolbarSource() const
{
    return "SpreadsheetRightToolbar.qml";
}

QString CASpreadsheetHandler::bottomToolbarSource() const
{
    return "FindToolbar.qml";
}

int CASpreadsheetHandler::currentSheetNumber() const
{
    return d->currentSheetNum + 1;
}

#include "CASpreadsheetHandler.moc"
