/*
 * This file is part of the KDE project
 *
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


#ifndef CANVASCONTROLLER_H
#define CANVASCONTROLLER_H

#include "KoCanvasController.h"
#include <QDeclarativeItem>

class KoDocument;
class KoCanvasBase;
class KoZoomController;
class KoZoomHandler;

class CanvasController : public QDeclarativeItem, KoCanvasController
{
    Q_OBJECT

    Q_PROPERTY(int sheetCount READ sheetCount NOTIFY sheetCountChanged)
    Q_PROPERTY(qreal docHeight READ docHeight NOTIFY docHeightChanged)
    Q_PROPERTY(qreal docWidth READ docWidth NOTIFY docWidthChanged)
    Q_PROPERTY(int cameraX READ cameraX WRITE setCameraX NOTIFY cameraXChanged)
    Q_PROPERTY(int cameraY READ cameraY WRITE setCameraY NOTIFY cameraYChanged)
    Q_PROPERTY(DocumentType documentType READ documentType NOTIFY documentTypeChanged)
    Q_ENUMS(DocumentType)

public:
    enum DocumentType { Undefined, TextDocument, Spreadsheet, Presentation };

    explicit CanvasController(QDeclarativeItem *parent = 0);
    virtual ~CanvasController();
    virtual void setVastScrolling(qreal factor);
    virtual void setZoomWithWheel(bool zoom);
    virtual void updateDocumentSize(const QSize& sz, bool recalculateCenter);
    virtual void setScrollBarValue(const QPoint& value);
    virtual QPoint scrollBarValue() const;
    virtual void pan(const QPoint& distance);
    virtual QPoint preferredCenter() const;
    virtual void setPreferredCenter(const QPoint& viewPoint);
    virtual void recenterPreferred();
    virtual void zoomTo(const QRect& rect);
    virtual void zoomBy(const QPoint& center, qreal zoom);
    virtual void zoomOut(const QPoint& center);
    virtual void zoomIn(const QPoint& center);
    virtual void ensureVisible(KoShape* shape);
    virtual void ensureVisible(const QRectF& rect, bool smooth = false);
    virtual int canvasOffsetY() const;
    virtual int canvasOffsetX() const;
    virtual int visibleWidth() const;
    virtual int visibleHeight() const;
    virtual KoCanvasBase* canvas() const;
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void setDrawShadow(bool drawShadow);
    virtual QSize viewportSize() const;
    virtual void scrollContentsBy(int dx, int dy);

    int sheetCount() const;
    DocumentType documentType() const;
    qreal docWidth() const;
    qreal docHeight() const;
    int cameraX() const;
    int cameraY() const;
    void setCameraX(int cameraX);
    void setCameraY(int cameraY);

public slots:
    void openDocument(const QString &path);
    void scrollDown();
    void scrollUp();
    void tellZoomControllerToSetDocumentSize(QSize size);
    void centerToCamera();
    void nextSheet();
    void previousSheet();

private:
    KoZoomHandler *m_zoomHandler;
    KoZoomController *m_zoomController;
    KoCanvasBase *m_canvas;
    QPoint m_currentPoint;
    DocumentType m_documentType;
    QSizeF m_documentViewSize;
    KoDocument *m_doc;
    QStringList m_recentFiles;

    void loadSettings();
    void saveSettings();

protected:
    bool isPresentationDocumentExtension(const QString& extension) const;
    bool isSpreadsheetDocumentExtension(const QString& extension) const;

signals:
    void sheetCountChanged();
    void docHeightChanged();
    void docWidthChanged();
    void cameraXChanged();
    void cameraYChanged();
    void documentTypeChanged();
};

#endif // CANVASCONTROLLER_H
