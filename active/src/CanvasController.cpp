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
#include <flow/part/FlowPage.h>
#include <part/Doc.h>

#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtGui/QGraphicsWidget>
#include <QtCore/QSettings>
#include <QtCore/QFileInfo>
#include <KoFindText.h>
#include <KoShapeManager.h>
#include <KoResourceManager_p.h>

CanvasController::CanvasController(QDeclarativeItem* parent)
    : QDeclarativeItem(parent), KoCanvasController(0), m_zoomHandler(0), m_zoomController(0), 
      m_canvas (0), m_currentPoint(QPoint(0,0)), m_documentType(CADocumentInfo::Undefined),
      m_documentSize(QSizeF(0,0)), m_doc(0), m_currentSlideNum(-1), m_paView(0), m_loadProgress(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setClip(true);
    loadSettings();
}

void CanvasController::setVastScrolling(qreal factor)
{

}

void CanvasController::setZoomWithWheel(bool zoom)
{

}

void CanvasController::updateDocumentSize(const QSize& sz, bool recalculateCenter)
{
    m_documentSize = sz;
    emit docHeightChanged();
    emit docWidthChanged();
}

void CanvasController::setScrollBarValue(const QPoint& value)
{

}

QPoint CanvasController::scrollBarValue() const
{
    return QPoint();
}

void CanvasController::pan(const QPoint& distance)
{

}

QPoint CanvasController::preferredCenter() const
{
    return QPoint();
}

void CanvasController::setPreferredCenter(const QPoint& viewPoint)
{

}

void CanvasController::recenterPreferred()
{
}

void CanvasController::zoomTo(const QRect& rect)
{

}

void CanvasController::zoomBy(const QPoint& center, qreal zoom)
{

}

void CanvasController::zoomOut(const QPoint& center)
{

}

void CanvasController::zoomIn(const QPoint& center)
{

}

void CanvasController::ensureVisible(KoShape* shape)
{
    setCameraX(shape->position().x());
    setCameraY(shape->position().y());
}

void CanvasController::ensureVisible(const QRectF& rect, bool smooth)
{
    kDebug() << rect << smooth;
}

int CanvasController::canvasOffsetY() const
{
    return 0;
}

int CanvasController::canvasOffsetX() const
{
    return 0;
}

int CanvasController::visibleWidth() const
{
    return 0;
}

int CanvasController::visibleHeight() const
{
    return 0;
}

KoCanvasBase* CanvasController::canvas() const
{
    return m_canvas;
}

void CanvasController::setCanvas(KoCanvasBase* canvas)
{
    QGraphicsWidget *widget = canvas->canvasItem();
    widget->setParentItem(this);
    canvas->setCanvasController(this);
    widget->setVisible(true);
    m_canvas = canvas;

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
    dynamic_cast<KWCanvasItem*>(m_canvas)->update();
}

void CanvasController::scrollUp()
{
    return;
    m_currentPoint.ry()-=50;
    proxyObject->emitMoveDocumentOffset(m_currentPoint);
    dynamic_cast<KWCanvasItem*>(m_canvas)->update();

}

int CanvasController::sheetCount() const
{
    if (m_canvas && m_documentType == CADocumentInfo::Spreadsheet) {
        Calligra::Tables::CanvasItem *canvas = dynamic_cast<Calligra::Tables::CanvasItem*>(m_canvas);
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

void CanvasController::zoomToFit()
{
    QSizeF canvasSize(width(), height());

    switch (documentType()) {
    case CADocumentInfo::Presentation: {
        QSizeF pageSize = m_paView->activePage()->boundingRect().size();
        QGraphicsWidget *canvasItem = m_canvas->canvasItem();
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
    m_canvas->canvasItem()->setGeometry(0,0,width(),height());
    break;
    case CADocumentInfo::Spreadsheet:
    default:
        m_canvas->canvasItem()->setGeometry(0,0,width(),height());
    }

    emit docHeightChanged();
    emit docWidthChanged();
}

void CanvasController::updateCanvasItem()
{
    if (m_canvas) {
        switch (m_documentType) {
            case CADocumentInfo::TextDocument:
            dynamic_cast<KWCanvasItem*>(m_canvas)->update();
            break;
        case CADocumentInfo::Spreadsheet:
            dynamic_cast<Calligra::Tables::CanvasItem*>(m_canvas)->update();
            updateDocumentSizeForActiveSheet();
            break;
        case CADocumentInfo::Presentation:
            dynamic_cast<KoPACanvasItem*>(m_canvas)->update();
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

CanvasController* CanvasController::canvasController()
{
    return this;
}

void CanvasController::updateDocumentSizeForActiveSheet()
{
    if (m_documentType != CADocumentInfo::Spreadsheet)
        return;
    Calligra::Tables::Sheet *sheet = dynamic_cast<Calligra::Tables::CanvasItem*>(m_canvas)->activeSheet();
    updateDocumentSize(sheet->cellCoordinatesToDocument(sheet->usedArea(false)).toRect().size(), false);
}

void CanvasController::geometryChanged (const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (m_canvas) {
        QGraphicsWidget *widget = m_canvas->canvasItem();
        widget->setParentItem(this);
        widget->setVisible(true);
        widget->setGeometry(newGeometry);

        zoomToFit();
    }
    QDeclarativeItem::geometryChanged (newGeometry, oldGeometry);
}

#include "CanvasController.moc"
