/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef __kivio_canvas_h__
#define __kivio_canvas_h__

#include <qpainter.h>
#include <qbitmap.h>
#include <QLineEdit>
#include <QWidget>
#include <QPoint>
#include <qrect.h>
#include <QString>
#include <qptrlist.h>

#include <KoQueryTrader.h>
#include <KoPoint.h>
#include <KoRect.h>
#include <KoGuides.h>
#include <koffice_export.h>
#include "kivio_intra_stencil_data.h"

class KivioView;
class KivioCanvas;
class KivioPage;
class KivioDoc;
class KivioRuler;
class KivioGuideLineData;

class KivioScreenPainter;
class KivioStencil;

class QPainter;
class QLabel;
class QPixmap;
class QScrollBar;
class QTimer;

class KoSize;
class KoRect;

#define YBORDER_WIDTH 50
#define XBORDER_HEIGHT 20

class KIVIO_EXPORT KivioCanvas : public QWidget
{
  Q_OBJECT
  friend class KivioView;
  public:
    KivioCanvas( QWidget*, KivioView*, KivioDoc*, QScrollBar*, QScrollBar* );
    ~KivioCanvas();

    int xOffset() const { return m_iXOffset; }
    int yOffset() const { return m_iYOffset; }

    const KivioPage* activePage() const;
    KivioPage* activePage();
    KivioPage* findPage( const QString& _name );

    KivioView* view()const  { return m_pView; }
    KivioDoc* doc()const { return m_pDoc; }

    QSize actualSize() const;

    virtual bool event(QEvent*);

    enum RectType { Insert, Rubber };
    void startRectDraw( const QPoint &p, RectType t );
    void continueRectDraw( const QPoint &p, RectType t );
    void endRectDraw();
    QRect rect()const { return currRect; }

    void startSpawnerDragDraw( const QPoint &p );
    void continueSpawnerDragDraw( const QPoint &p );
    void endSpawnerDragDraw();

    void drawSelectedStencilsXOR();
    void drawStencilXOR( KivioStencil * );

    KoPoint snapToGrid(const KoPoint&);
    KoPoint snapToGuides(const KoPoint&, bool &, bool &);
    KoPoint snapToGridAndGuides(const KoPoint&);

    KoPoint mapFromScreen(const QPoint&);
    QPoint mapToScreen(const KoPoint&);

    void beginUnclippedSpawnerPainter();
    void endUnclippedSpawnerPainter();

    void setViewCenterPoint(const KoPoint &);

    KoRect visibleArea();
    void setVisibleArea(KoRect, int margin = 0);
    void setVisibleAreaByWidth(KoRect, int margin = 0);
    void setVisibleAreaByHeight(KoRect, int margin = 0);

    KoGuides& guideLines() { return m_guides; }

    int pageOffsetX() const { return m_pageOffsetX; }
    int pageOffsetY() const { return m_pageOffsetY; }

    void setShowConnectorTargets(bool state) { m_showConnectorTargets = state; }
    bool showConnectorTargets() const { return m_showConnectorTargets; }

  signals:
    void visibleAreaChanged();

  public slots:
    virtual void setUpdatesEnabled(bool);

    void zoomIn(const QPoint&);
    void zoomOut(const QPoint&);

    void scrollDx(int dx);
    void scrollDy(int dy);

    void scrollV(int value);
    void scrollH(int value);

    void updateScrollBars();

    void startPasteMoving();

    void updateAutoGuideLines();

  protected:
    virtual void mousePressEvent( QMouseEvent* );
    virtual void mouseReleaseEvent( QMouseEvent* );
    virtual void mouseMoveEvent( QMouseEvent* );

    virtual void enterEvent( QEvent* );
    virtual void leaveEvent( QEvent* );

    virtual void resizeEvent( QResizeEvent* );
    virtual void paintEvent( QPaintEvent* );
    virtual void wheelEvent( QWheelEvent* );

    virtual void dragEnterEvent( QDragEnterEvent * );
    virtual void dragMoveEvent( QDragMoveEvent * );
    virtual void dragLeaveEvent( QDragLeaveEvent * );
    virtual void dropEvent( QDropEvent * );

    virtual void keyPressEvent( QKeyEvent * );

    void beginUnclippedPainter();
    void endUnclippedPainter();

    void paintSelectedXOR();

    void continuePasteMoving(const QPoint &pos);
    void endPasteMoving();

  protected slots:
    void borderTimerTimeout();

  private:
    KivioView* m_pView;
    KivioDoc* m_pDoc;

    QScrollBar* m_pVertScrollBar;
    QScrollBar* m_pHorzScrollBar;

    int m_iXOffset;
    int m_iYOffset;

    QPixmap* m_buffer;

    int m_pScrollX;
    int m_pScrollY;
    QPointArray gridLines;

    bool oldRectValid;
    QRect currRect;
    QPoint rectAnchor;
    QPainter* unclippedPainter;
    QPoint sizePreviewPos;

    KivioScreenPainter* unclippedSpawnerPainter;
    KivioIntraStencilData m_dragStencilData;
    KivioStencil* m_pDragStencil;

    QTimer* m_borderTimer;

    bool delegateThisEvent;
    QPoint lastPoint;

    bool m_pasteMoving;
    QPtrList<KoRect> m_lstOldGeometry;
    KoPoint m_origPoint;

    KoGuides m_guides;

    int m_pageOffsetX;
    int m_pageOffsetY;

    bool m_showConnectorTargets;
};

#endif
