/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2010-2011 Jarosław Staniek <staniek@kde.org>
 * Copyright (C) 2011 Shantanu Tushar <jhahoneyk@gmail.com>
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


#include "CanvasController.h"

#include <KoDocument.h>
#include <KMimeType>
#include <KMimeTypeTrader>
#include <KoView.h>
#include <KoCanvasBase.h>
#include <KWCanvasItem.h>
#include <KDebug>
#include <KoZoomController.h>
#include <KoZoomHandler.h>
#include <KActionCollection>
#include <KoToolManager.h>
#include <tables/part/CanvasItem.h>
#include <KoPACanvasItem.h>
#include <KoPAView.h>
#include <KoPADocument.h>
#include <KoPAViewBase.h>
#include <stage/part/KPrDocument.h>
#include <tables/Sheet.h>
#include <tables/Map.h>
#include <tables/DocBase.h>
#include "PAView.h"
#include <KoPAPage.h>
#include <flow/part/FlowDocument.h>
#include <part/Doc.h>

#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtGui/QGraphicsWidget>
#include <QtCore/QSettings>
#include <QtCore/QFileInfo>

/*!
* extensions
*/
const QString EXT_PPS("pps");
const QString EXT_PPSX("ppsx");
const QString EXT_PPT("ppt");
const QString EXT_PPTX("pptx");
const QString EXT_ODP("odp");
const QString EXT_DOC("doc");
const QString EXT_DOCX("docx");
const QString EXT_ODT("odt");
const QString EXT_TXT("txt");
const QString EXT_RTF("rtf");
const QString EXT_ODS("ods");
const QString EXT_XLS("xls");
const QString EXT_XLSX("xlsx");

CanvasController::CanvasController(QDeclarativeItem* parent)
    : QDeclarativeItem(parent), KoCanvasController(0), m_zoomHandler(0), m_zoomController(0), 
      m_canvasItem(0), m_currentPoint(QPoint(0,0)), m_documentType(CADocumentInfo::Undefined),
      m_documentSize(QSizeF(0,0)), m_doc(0), m_currentSlideNum(-1), m_paView(0), m_loadProgress(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setClip(true);
    loadSettings();
}

void CanvasController::openDocument(const QString& path)
{
    QString error;
    QString mimetype = KMimeType::findByPath(path)->name();
    m_doc = KMimeTypeTrader::createPartInstanceFromQuery<KoDocument>(mimetype, 0, 0, QString(),
            QVariantList(), &error);
    if (!m_doc) {
        kDebug() << "Doc can't be openend" << error;
        return;
    }

    QString fname(path);
    QString ext = KMimeType::extractKnownExtension(fname);

    if (!ext.isEmpty()) {
        fname.chop(ext.length() + 1);
    } else {
        kDebug() << "Extension detection failed. This is bad.";
    }

    if (isPresentationDocumentExtension(ext)) {
        m_documentType = CADocumentInfo::Presentation;
        emit documentTypeChanged();

        KPrDocument *prDocument = static_cast<KPrDocument*>(m_doc);
        prDocument->openUrl(KUrl(path));

        m_canvasItem = dynamic_cast<KoCanvasBase*>(prDocument->canvasItem());
        if (!m_canvasItem) {
            kDebug() << "Failed to fetch a canvas item";
            return;
        }

        KoToolManager::instance()->addController(this);
        KoPACanvasItem *paCanvasItem = dynamic_cast<KoPACanvasItem*>(m_canvasItem);

        if (paCanvasItem) {
            m_paView = new PAView(this, dynamic_cast<KoPACanvasBase*>(m_canvasItem), prDocument);
            paCanvasItem->setView(m_paView);

            m_zoomController = m_paView->zoomController();
            m_zoomHandler = static_cast<KoZoomHandler*>(paCanvasItem->viewConverter());

            m_currentSlideNum = -1;
            nextSlide();

            // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
            connect(proxyObject, SIGNAL(moveDocumentOffset(const QPoint&)), paCanvasItem, SLOT(slotSetDocumentOffset(QPoint)));
            // whenever the size of the document viewed in the canvas changes, inform the zoom controller
            connect(paCanvasItem, SIGNAL(documentSize(QSize)), this, SLOT(tellZoomControllerToSetDocumentSize(QSize)));
            connect(paCanvasItem, SIGNAL(documentSize(QSize)), proxyObject, SLOT(updateDocumentSize(QSize)));
            paCanvasItem->update();
        }

        setCanvas(m_canvasItem);
    } else if (isSpreadsheetDocumentExtension(ext)) {
        m_documentType = CADocumentInfo::Spreadsheet;
        emit documentTypeChanged();

        Calligra::Tables::Doc *tablesDoc = static_cast<Calligra::Tables::Doc*>(m_doc);
        tablesDoc->openUrl(KUrl(path));

        m_canvasItem = dynamic_cast<KoCanvasBase*>(m_doc->canvasItem());
        if (!m_canvasItem) {
            kDebug() << "Failed to fetch a canvas item";
            return;
        }

        KoToolManager::instance()->addController(this);
        Calligra::Tables::CanvasItem *canvasItem = dynamic_cast<Calligra::Tables::CanvasItem*>(m_canvasItem);

        m_zoomHandler = new KoZoomHandler();
        m_zoomController = new KoZoomController(this, m_zoomHandler, tablesDoc->actionCollection());
        m_zoomController->setZoom(KoZoomMode::ZOOM_CONSTANT, 1.0);

        setCanvasMode(KoCanvasController::Spreadsheet);

        if (canvasItem) {
            // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
            connect(proxyObject, SIGNAL(moveDocumentOffset(const QPoint&)), canvasItem, SLOT(setDocumentOffset(QPoint)));
            // whenever the size of the document viewed in the canvas changes, inform the zoom controller
            connect(canvasItem, SIGNAL(documentSizeChanged(QSize)), this, SLOT(tellZoomControllerToSetDocumentSize(QSize)));
            //connect(canvasItem, SIGNAL(documentSizeChanged(QSize)),
            //proxyObject, SLOT(updateDocumentSize(QSize)));
            canvasItem->update();
        }

        setCanvas(m_canvasItem);
    } else {
        m_documentType = CADocumentInfo::TextDocument;
        emit documentTypeChanged();

        kDebug() << "Trying to open the document";
        KWDocument *kwDoc = static_cast<KWDocument*>(m_doc);
        kwDoc->openUrl(KUrl(path));

        m_canvasItem = dynamic_cast<KoCanvasBase*>(m_doc->canvasItem());
        if (!m_canvasItem) {
            kDebug() << "Failed to fetch a canvas item";
            return;
        }

        kDebug() << "Will now attempt to typecast";
        KoToolManager::instance()->addController(this);
        KWCanvasItem *canvasItem = dynamic_cast<KWCanvasItem*>(m_canvasItem);

        if (!canvasItem) {
            kDebug() << "Failed to get KWCanvasItem";
        }

        m_zoomHandler = static_cast<KoZoomHandler*>(m_canvasItem->viewConverter());
        m_zoomController = new KoZoomController(this, m_zoomHandler, m_doc->actionCollection());
        m_currentTextDocPage = kwDoc->pageManager()->begin();
        m_zoomController->setPageSize(m_currentTextDocPage.rect().size());
        m_zoomController->setZoom(KoZoomMode::ZOOM_CONSTANT, 1.0);

        if (canvasItem) {
            canvasItem->updateSize();

            // whenever the size of the document viewed in the canvas changes, inform the zoom controller
            connect(canvasItem, SIGNAL(documentSize(QSizeF)), m_zoomController, SLOT(setDocumentSize(QSizeF)));
            // update the canvas whenever we scroll, the canvas controller must emit this signal on scrolling/panning
            connect(proxyObject, SIGNAL(moveDocumentOffset(const QPoint&)), canvasItem, SLOT(setDocumentOffset(QPoint)));
            canvasItem->updateSize();
        }

        setCanvas(m_canvasItem);
    }

    kDebug() << "Requesting tool activation";
    KoToolManager::instance()->requestToolActivation(this);
    //FIXME: doesn't work, no emits
    connect(m_doc, SIGNAL(sigProgress(int)), SLOT(processLoadProgress(int)));

    kDebug() << "Trying to add to recent files";

    bool recentFileAlreadyExists = false;
    foreach(CADocumentInfo *docInfo, m_recentFiles) {
        if (docInfo->path() == path) {
            recentFileAlreadyExists = true;
            break;
        }
    }
    if (!recentFileAlreadyExists) {
        m_recentFiles << new CADocumentInfo(m_documentType, QFileInfo(path).fileName(), path);
    }

    emit sheetCountChanged();
    emit documentLoaded();
    kDebug() << "Everything done loading";
}

void CanvasController::setVastScrolling(qreal factor)
{
    kDebug() << factor;
}

void CanvasController::setZoomWithWheel(bool zoom)
{
    kDebug() << zoom;
}

void CanvasController::updateDocumentSize(const QSize& sz, bool recalculateCenter)
{
    m_documentSize = sz;
    emit docHeightChanged();
    emit docWidthChanged();

    qDebug() << sz << recalculateCenter;
}

void CanvasController::setScrollBarValue(const QPoint& value)
{
    kDebug() << value;
}

QPoint CanvasController::scrollBarValue() const
{
    return QPoint();
}

void CanvasController::pan(const QPoint& distance)
{
    kDebug() << distance;
}

QPoint CanvasController::preferredCenter() const
{
    return QPoint();
}

void CanvasController::setPreferredCenter(const QPoint& viewPoint)
{
    kDebug() << viewPoint;
}

void CanvasController::recenterPreferred()
{
    kDebug() << "BLEH";
}

void CanvasController::zoomTo(const QRect& rect)
{
    kDebug() << rect;
}

void CanvasController::zoomBy(const QPoint& center, qreal zoom)
{
    kDebug() << center << zoom;
}

void CanvasController::zoomOut(const QPoint& center)
{
    kDebug() << center;
}

void CanvasController::zoomIn(const QPoint& center)
{
    kDebug() << center;
}

void CanvasController::ensureVisible(KoShape* shape)
{
    kDebug() << shape;
}

void CanvasController::ensureVisible(const QRectF& rect, bool smooth)
{
    kDebug() << rect << smooth;
}

int CanvasController::canvasOffsetY() const
{
    ////kDebug() << "ASKING";
    return 0;
}

int CanvasController::canvasOffsetX() const
{
    return 0;
}

int CanvasController::visibleWidth() const
{
    ////kDebug() << "ASKING";
    return 0;
}

int CanvasController::visibleHeight() const
{
    //kDebug() << "ASKING";
    return 0;
}

KoCanvasBase* CanvasController::canvas() const
{
    ////kDebug() << "ASKING";
    return m_canvasItem;
}

void CanvasController::setCanvas(KoCanvasBase* canvas)
{
    QGraphicsWidget *widget = canvas->canvasItem();
    widget->setParentItem(this);
    canvas->setCanvasController(this);
    widget->setVisible(true);

    zoomToFit();
}

void CanvasController::setDrawShadow(bool drawShadow)
{
    //kDebug() << "ASKING";
    kDebug() << drawShadow;
}

QSize CanvasController::viewportSize() const
{
    //kDebug() << "ASKING";
    return QSize();
}

void CanvasController::scrollContentsBy(int dx, int dy)
{
    kDebug() << dx << dy;
}

void CanvasController::scrollDown()
{
    return;
    m_currentPoint.ry()+=50;
    proxyObject->emitMoveDocumentOffset(m_currentPoint);
    dynamic_cast<KWCanvasItem*>(m_canvasItem)->update();
}

void CanvasController::scrollUp()
{
    return;
    m_currentPoint.ry()-=50;
    proxyObject->emitMoveDocumentOffset(m_currentPoint);
    dynamic_cast<KWCanvasItem*>(m_canvasItem)->update();

}

bool CanvasController::isPresentationDocumentExtension(const QString& extension) const
{
    return 0 == QString::compare(extension, EXT_ODP, Qt::CaseInsensitive)
           ||  0 == QString::compare(extension, EXT_PPS, Qt::CaseInsensitive)
           ||  0 == QString::compare(extension, EXT_PPSX, Qt::CaseInsensitive)
           ||  0 == QString::compare(extension, EXT_PPT, Qt::CaseInsensitive)
           ||  0 == QString::compare(extension, EXT_PPTX, Qt::CaseInsensitive);
}

bool CanvasController::isSpreadsheetDocumentExtension(const QString& extension) const
{
    return 0 == QString::compare(extension, EXT_ODS, Qt::CaseInsensitive)
           ||  0 == QString::compare(extension, EXT_XLS, Qt::CaseInsensitive)
           ||  0 == QString::compare(extension, EXT_XLSX, Qt::CaseInsensitive);
}

int CanvasController::sheetCount() const
{
    if (m_canvasItem && m_documentType == CADocumentInfo::Spreadsheet) {
        Calligra::Tables::CanvasItem *canvas = dynamic_cast<Calligra::Tables::CanvasItem*>(m_canvasItem);
        return canvas->activeSheet()->map()->count();
    } else {
        return 0;
    }
}

void CanvasController::tellZoomControllerToSetDocumentSize(QSize size)
{
    m_zoomController->setDocumentSize(size);
    setDocumentSize(size);
}

CADocumentInfo::DocumentType CanvasController::documentType() const
{
    return m_documentType;
}

qreal CanvasController::docHeight() const
{
    if (m_zoomHandler) {
        return m_documentSize.height()*m_zoomHandler->zoomFactorY();
    } else {
        return m_documentSize.height();
    }
}

qreal CanvasController::docWidth() const
{
    if (m_zoomHandler) {
        return m_documentSize.width()*m_zoomHandler->zoomFactorX();
    } else {
        return m_documentSize.width();
    }
}

int CanvasController::cameraX() const
{
    return m_currentPoint.x();
}

int CanvasController::cameraY() const
{
    return m_currentPoint.y();
}

void CanvasController::setCameraX(int cameraX)
{
    m_currentPoint.setX(cameraX);
    emit cameraXChanged();
    centerToCamera();
}

void CanvasController::setCameraY(int cameraY)
{
    m_currentPoint.setY(cameraY);
    emit cameraYChanged();
    centerToCamera();
}

void CanvasController::centerToCamera()
{
    if (proxyObject) {
        proxyObject->emitMoveDocumentOffset(m_currentPoint);
    }

    updateCanvasItem();
}


void CanvasController::nextSheet()
{
    Calligra::Tables::CanvasItem *canvasItem = dynamic_cast<Calligra::Tables::CanvasItem*>(m_canvasItem);
    if (!canvasItem)
        return;
    Calligra::Tables::Sheet *sheet = canvasItem->activeSheet();
    if (!sheet)
        return;
    Calligra::Tables::DocBase *kspreadDoc = qobject_cast<Calligra::Tables::DocBase*>(m_doc);
    if (!kspreadDoc)
        return;
    sheet = kspreadDoc->map()->nextSheet(sheet);
    if (!sheet)
        return;
    canvasItem->setActiveSheet(sheet);
    updateDocumentSize(sheet->cellCoordinatesToDocument(sheet->usedArea(false)).toRect().size(), false);
}

void CanvasController::previousSheet()
{
    Calligra::Tables::CanvasItem *canvasItem = dynamic_cast<Calligra::Tables::CanvasItem*>(m_canvasItem);
    if (!canvasItem)
        return;
    Calligra::Tables::Sheet *sheet = canvasItem->activeSheet();
    if (!sheet)
        return;
    Calligra::Tables::DocBase *kspreadDoc = dynamic_cast<Calligra::Tables::DocBase*>(m_doc);
    if (!kspreadDoc)
        return;
    sheet = kspreadDoc->map()->previousSheet(sheet);
    if (!sheet)
        return;
    canvasItem->setActiveSheet(sheet);
    updateDocumentSize(sheet->cellCoordinatesToDocument(sheet->usedArea(false)).toRect().size(), false);
}

void CanvasController::loadSettings()
{
    QSettings settings;
    foreach(QString string, settings.value("recentFiles").toStringList()) {
        m_recentFiles.append(CADocumentInfo::fromStringList(string.split(";")));
    }
}

void CanvasController::saveSettings()
{
    QSettings settings;
    QStringList list;
    foreach(CADocumentInfo *docInfo, m_recentFiles) {
        list << docInfo->toStringList().join(";");
    }
    settings.setValue("recentFiles", list);
}

CanvasController::~CanvasController()
{
    saveSettings();
}

void CanvasController::nextSlide()
{
    if (m_documentType != CADocumentInfo::Presentation)
        return;
    m_currentSlideNum++;
    KPrDocument *prDocument = static_cast<KPrDocument*>(m_doc);
    if (m_currentSlideNum >= prDocument->pageCount())
        m_currentSlideNum = prDocument->pageCount()-1;
    m_paView->doUpdateActivePage(prDocument->pageByIndex(m_currentSlideNum, false));
    zoomToFit();
}

void CanvasController::previousSlide()
{
    if (m_documentType != Presentation)
        return;
    m_currentSlideNum--;
    KPrDocument *prDocument = static_cast<KPrDocument*>(m_doc);
    if (m_currentSlideNum < 0)
        m_currentSlideNum = 0;
    m_paView->doUpdateActivePage(prDocument->pageByIndex(m_currentSlideNum, false));
    zoomToFit();
}

void CanvasController::zoomToFit()
{
    QSizeF canvasSize(width(), height());

    switch (documentType()) {
    case CADocumentInfo::Presentation: {
        QSizeF pageSize = m_paView->activePage()->boundingRect().size();
        QGraphicsWidget *canvasItem = m_canvasItem->canvasItem();
        QSizeF newSize(pageSize);
        newSize.scale(canvasSize, Qt::KeepAspectRatio);

        if (canvasSize.width() < canvasSize.height()) {
            canvasItem->setGeometry(0, (canvasSize.height()-newSize.height())/2,
                                    newSize.width(), newSize.height());
            m_zoomHandler->setZoom(canvasSize.width()/pageSize.width()*0.75);
        } else {
            canvasItem->setGeometry((canvasSize.width()-newSize.width())/2, 0,
                                    newSize.width(), newSize.height());
            m_zoomHandler->setZoom(canvasSize.height()/pageSize.height()*0.75);
        }

        break;
    }
    case CADocumentInfo::TextDocument: {
        KWDocument *doc = static_cast<KWDocument*>(m_doc);
        KWPage currentPage = doc->pageManager()->page(qreal(cameraY()));
        if (currentPage.isValid()) {
            m_zoomHandler->setZoom(canvasSize.width()/currentPage.width()*0.75);
        }
    }
    m_canvasItem->canvasItem()->setGeometry(0,0,width(),height());
    break;
    case CADocumentInfo::Spreadsheet:
    default:
        m_canvasItem->canvasItem()->setGeometry(0,0,width(),height());
    }

    emit docHeightChanged();
    emit docWidthChanged();
}

void CanvasController::updateCanvasItem()
{
    if (m_canvasItem) {
        switch (m_documentType) {
        case CADocumentInfo::TextDocument:
            dynamic_cast<KWCanvasItem*>(m_canvasItem)->update();
            break;
        case CADocumentInfo::Spreadsheet:
            dynamic_cast<Calligra::Tables::CanvasItem*>(m_canvasItem)->update();
            updateDocumentSizeForActiveSheet();
            break;
        case CADocumentInfo::Presentation:
            dynamic_cast<KoPACanvasItem*>(m_canvasItem)->update();
            break;
        }
    }
}

void CanvasController::processLoadProgress (int value)
{
    m_loadProgress = value;
    emit loadProgressChanged();
    if (value == 100) {
        switch (m_documentType) {
        case CADocumentInfo::Presentation:
            updateDocumentSizeForActiveSheet();
            break;
        }
    }
}

int CanvasController::loadProgress() const
{
    return m_loadProgress;
}

void CanvasController::updateDocumentSizeForActiveSheet()
{
    if (m_documentType != CADocumentInfo::Spreadsheet)
        return;
    Calligra::Tables::Sheet *sheet = dynamic_cast<Calligra::Tables::CanvasItem*>(m_canvasItem)->activeSheet();
    updateDocumentSize(sheet->cellCoordinatesToDocument(sheet->usedArea(false)).toRect().size(), false);
}

void CanvasController::geometryChanged (const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (m_canvasItem) {
        QGraphicsWidget *widget = m_canvasItem->canvasItem();
        widget->setParentItem(this);
        widget->setVisible(true);
        widget->setGeometry(newGeometry);

        zoomToFit();
    }
    QDeclarativeItem::geometryChanged (newGeometry, oldGeometry);
}

#include "CanvasController.moc"
