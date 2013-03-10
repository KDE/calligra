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

#include "CAPresentationHandler.h"
#include "CAPAView.h"
#include "CADocumentController.h"
#include "CACanvasController.h"
#include "CAPADocumentModel.h"

#include <stage/part/KPrDocument.h>

#include <KoPACanvasItem.h>
#include <KoPAPageBase.h>
#include <KoCanvasController.h>
#include <KoSelection.h>

#include <KoPart.h>
#include <KoToolManager.h>
#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoFindText.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoFindBase.h>

#include <KDebug>
#include <KMimeType>
#include <KMimeTypeTrader>

#include <QSize>
#include <QTimer>
#include <QTextDocument>

class CAPresentationHandler::Private
{
public:
    Private()
    {
        currentSlideNum = -1;
        paDocumentModel = 0;
        findText = 0;
        countMatchesPerSlide = 0;
        searchSlideNumber = 0;
        matchFound = false;
    }

    QString searchString;
    KPrDocument* document;
    CAPAView* paView;
    int currentSlideNum;
    int countMatchesPerSlide;
    int searchSlideNumber;
    bool matchFound;
    QList<QTextDocument*> texts;
    KoFindText* findText;
    QTimer slideshowTimer;
    QList<KoPAPageBase*> slideShow;
    CAPADocumentModel *paDocumentModel;
};

CAPresentationHandler::CAPresentationHandler (CADocumentController* documentController)
    : CAAbstractDocumentHandler (documentController)
    , d (new Private())
{
    QList<QTextDocument*> texts;
    d->findText = new KoFindText(this);
    connect(&d->slideshowTimer, SIGNAL(timeout()), SLOT(advanceSlideshow()));
    connect (d->findText, SIGNAL(updateCanvas()), SLOT(updateCanvas()));
    connect (d->findText, SIGNAL(matchFound(KoFindMatch)), SLOT(findMatchFound(KoFindMatch)));
    connect (d->findText, SIGNAL(noMatchFound()), SLOT(findNoMatchFound()));
}

CAPresentationHandler::~CAPresentationHandler()
{
    delete d;
}

KoZoomMode::Mode CAPresentationHandler::preferredZoomMode() const
{
    return KoZoomMode::ZOOM_PAGE;
}

KoDocument* CAPresentationHandler::document()
{
    return d->document;
}

bool CAPresentationHandler::openDocument (const QString& uri)
{
    QString error;
    QString mimetype = KMimeType::findByPath (uri)->name();
    KoPart *part = KMimeTypeTrader::createInstanceFromQuery<KoPart>(mimetype,
                      QLatin1String("CalligraPart"), 0, QString(), QVariantList(), &error);

    if (!part) {
        kDebug() << "Doc can't be openend" << error;
        return false;
    }

    d->document = qobject_cast<KPrDocument*> (part->document());
    d->document->openUrl (KUrl (uri));

    KoCanvasBase* paCanvas = dynamic_cast<KoCanvasBase*> (part->canvasItem());
    KoPACanvasItem* paCanvasItem = dynamic_cast<KoPACanvasItem*> (paCanvas);
    if (!paCanvasItem) {
        kDebug() << "Failed to fetch a canvas item";
        return false;
    }

    if (paCanvasItem) {
        d->paView = new CAPAView (documentController()->canvasController(), dynamic_cast<KoPACanvasBase*> (paCanvas),
                                d->document);
        paCanvasItem->setView (d->paView);

        documentController()->canvasController()->setZoomHandler (static_cast<KoZoomHandler*> (paCanvasItem->viewConverter()));
        d->paView->connectToZoomController();

        // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
        connect (documentController()->canvasController()->canvasControllerProxyObject(),
                 SIGNAL(moveDocumentOffset(QPoint)), paCanvasItem, SLOT(slotSetDocumentOffset(QPoint)));
        // whenever the size of the document viewed in the canvas changes, inform the zoom controller
        connect (paCanvasItem, SIGNAL(documentSize(QSize)), this, SLOT(tellZoomControllerToSetDocumentSize(QSize)));

        paCanvasItem->update();
    }

    setCanvas (paCanvas);
    KoToolManager::instance()->addController (documentController()->canvasController());

    connect(documentController()->canvasController(), SIGNAL(needsCanvasResize(QSizeF)), SLOT(resizeCanvas(QSizeF)));
    connect (documentController()->canvasController(), SIGNAL(needCanvasUpdate()), SLOT(updateCanvas()));

    d->paDocumentModel = new CAPADocumentModel(this, d->document);
    emit totalNumberOfSlidesChanged();
    QTimer::singleShot(0, this, SLOT(nextSlide()));

    return true;
}

QStringList CAPresentationHandler::supportedMimetypes()
{
    QStringList supportedTypes;
    supportedTypes << "application/vnd.oasis.opendocument.presentation" << "application/vnd.ms-powerpoint";
    return supportedTypes;
}

void CAPresentationHandler::nextSlide()
{
    if (d->currentSlideNum == d->document->pageCount()-1)
        return;
    d->currentSlideNum++;
    emit currentSlideNumberChanged();

    gotoCurrentSlide();
    zoomToFit();
}

void CAPresentationHandler::previousSlide()
{
    if (d->currentSlideNum > 0)
    {
        d->currentSlideNum--;
        emit currentSlideNumberChanged();
    }

    gotoCurrentSlide();
    zoomToFit();
}

void CAPresentationHandler::gotoCurrentSlide()
{
    d->paView->doUpdateActivePage (d->document->pageByIndex (d->currentSlideNum, false));
    emit previousPageImageChanged();
    emit nextPageImageChanged();
}

void CAPresentationHandler::setTextData(int slideNumber) {
    d->texts.clear();
    KoFindText::findTextInShapes(d->document->pageByIndex(slideNumber,false)->shapes(), d->texts);
    d->findText->setDocuments(d->texts);
}

void CAPresentationHandler::zoomToFit()
{
    updateCanvas();
    setTextData(d->currentSlideNum);
}

void CAPresentationHandler::tellZoomControllerToSetDocumentSize (const QSize& size)
{
    documentController()->canvasController()->zoomController()->setDocumentSize (size);
}

void CAPresentationHandler::updateCanvas()
{
    dynamic_cast<KoPACanvasItem*> (canvas())->update();
}

QString CAPresentationHandler::documentTypeName()
{
    return "presentation";
}

void CAPresentationHandler::resizeCanvas (const QSizeF& canvasSize)
{
    QSizeF pageSize = d->paView->activePage()->boundingRect().size();
    QGraphicsWidget* canvasItem = canvas()->canvasItem();
    QSizeF newSize (pageSize);
    newSize.scale (canvasSize, Qt::KeepAspectRatio);

    if (canvasSize.width() < canvasSize.height()) {
        canvasItem->setGeometry (0, (canvasSize.height() - newSize.height()) / 2,
                                 newSize.width(), newSize.height());
        documentController()->canvasController()->zoomHandler()->setZoom (canvasSize.width() / pageSize.width() * 0.75);
    } else {
        canvasItem->setGeometry ( (canvasSize.width() - newSize.width()) / 2, 0,
                                  newSize.width(), newSize.height());
        documentController()->canvasController()->zoomHandler()->setZoom (canvasSize.height() / pageSize.height() * 0.75);
    }
}

QString CAPresentationHandler::searchString() const
{
    return d->searchString;
}

void CAPresentationHandler::setSearchString (const QString& searchString)
{
    d->searchString = searchString;
    d->findText->find(searchString);

    emit searchStringChanged();
}

void CAPresentationHandler::searchOtherSlides(SearchDirection direction) {
    //Reset the count
    d->countMatchesPerSlide = 0;
    if( direction == SearchForward) {
        d->searchSlideNumber = d->currentSlideNum + 1;
    } else if ( direction == SearchBackwards) {
        if( d->currentSlideNum != 0) {
            d->searchSlideNumber = d->currentSlideNum - 1;
        } else {
           return;
        }
    }
    while ((d->searchSlideNumber < totalNumberOfSlides()) && (d->searchSlideNumber >= 0)) {
        setTextData(d->searchSlideNumber);
        setSearchString(d->searchString);
        if(d->matchFound == true) {
            d->currentSlideNum = d->searchSlideNumber;
            setCurrentSlideNumber(d->currentSlideNum);
            setSearchString(d->searchString);
            if(direction == SearchBackwards) {
               d->countMatchesPerSlide = d->findText->matches().count() - 1;
               d->findText->findPrevious();
            }
            break;
        }
        if(direction == SearchForward) {
           d->searchSlideNumber++;
        } else if(direction == SearchBackwards) {
           d->searchSlideNumber--;
        }
    }
}

void CAPresentationHandler::findNext() {
    d->countMatchesPerSlide++;
    d->findText->findNext();
    if((d->countMatchesPerSlide >= d->findText->matches().count()) or (d->findText->matches().count() == 0)) {
      searchOtherSlides(SearchForward);
    }
}

void CAPresentationHandler::findPrevious() {
    d->countMatchesPerSlide--;
    d->findText->findPrevious();
    if( d->countMatchesPerSlide < 0) {
       searchOtherSlides(SearchBackwards);
    }
}

void CAPresentationHandler::findMatchFound (const KoFindMatch& match)
{
    QTextCursor cursor = match.location().value<QTextCursor>();
    updateCanvas();

    canvas()->resourceManager()->setResource (KoText::CurrentTextAnchor, cursor.anchor());
    canvas()->resourceManager()->setResource (KoText::CurrentTextPosition, cursor.position());
    d->matchFound = true;
}

void CAPresentationHandler::findNoMatchFound()
{
    d->matchFound = false;
    kDebug() << "Match for " << searchString() << " not found";
}

QString CAPresentationHandler::topToolbarSource() const
{
    return "PresentationTopToolbar.qml";
}

QString CAPresentationHandler::leftToolbarSource() const
{
    return "PresentationLeftToolbar.qml";
}

QString CAPresentationHandler::rightToolbarSource() const
{
    return "PresentationRightToolbar.qml";
}

QString CAPresentationHandler::centerOverlaySource() const
{
    return "PresentationCenterOverlay.qml";
}

QString CAPresentationHandler::bottomToolbarSource() const
{
    return "FindToolbar.qml";
}

void CAPresentationHandler::setSlideshowDelay(int delay)
{
    d->slideshowTimer.setInterval(delay*1000);
}

int CAPresentationHandler::slideshowDelay() const
{
    return d->slideshowTimer.interval()/1000;
}

void CAPresentationHandler::startSlideshow()
{
    d->slideShow = d->document->slideShow();
    d->currentSlideNum = 0;
    d->paView->doUpdateActivePage(d->slideShow.at(d->currentSlideNum));

    d->slideshowTimer.start();
    emit slideshowStarted();
}

void CAPresentationHandler::stopSlideshow()
{
    d->slideshowTimer.stop();
    emit slideshowStopped();
}

void CAPresentationHandler::advanceSlideshow()
{
    d->paView->doUpdateActivePage(d->slideShow.at(++(d->currentSlideNum)));

    if (d->currentSlideNum == d->slideShow.count() - 1) {
        stopSlideshow();
    }
}

int CAPresentationHandler::currentSlideNumber() const
{
    return d->currentSlideNum + 1;
}

int CAPresentationHandler::totalNumberOfSlides() const
{
    return d->document->pageCount();
}

CAPADocumentModel* CAPresentationHandler::paDocumentModel() const
{
    return d->paDocumentModel;
}

void CAPresentationHandler::setCurrentSlideNumber(int number)
{
    d->currentSlideNum = number;
    gotoCurrentSlide();
    emit currentSlideNumberChanged();
}

QString CAPresentationHandler::nextPageImage() const
{
    return paDocumentModel()->data(paDocumentModel()->index(d->currentSlideNum+1, 0), CAPADocumentModel::SlideImageRole).toString();
}

QString CAPresentationHandler::previousPageImage() const
{
    return paDocumentModel()->data(paDocumentModel()->index(d->currentSlideNum-1, 0), CAPADocumentModel::SlideImageRole).toString();
}

void CAPresentationHandler::gotoNextPage()
{
    nextSlide();
}

void CAPresentationHandler::gotoPreviousPage()
{
    previousSlide();
}

CAAbstractDocumentHandler::FlickModes CAPresentationHandler::flickMode() const
{
    return FlickHorizontally;
}

#include "CAPresentationHandler.moc"
