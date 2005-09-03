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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef __kivio_canvas_h__
#define __kivio_canvas_h__

#include <qpainter.h>
#include <qbitmap.h>
#include <qlineedit.h>
#include <qwidget.h>
#include <qpoint.h>
#include <qrect.h>
#include <qstring.h>
#include <qptrlist.h>

#include <koQueryTrader.h>
#include <koPoint.h>
#include <koRect.h>
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

namespace Kivio {
  class Object;
};

#define YBORDER_WIDTH 50
#define XBORDER_HEIGHT 20

class KIVIO_EXPORT KivioCanvas : public QWidget
{ Q_OBJECT
    friend class KivioView;
  public:
    KivioCanvas( QWidget*, KivioView*, KivioDoc*, QScrollBar*, QScrollBar* );
    ~KivioCanvas();
    /**
    * Called from @ref KivioView to complete the construction. Has to
    * be called before any other method on this object may be invoced.
    */
    int xOffset() { return m_iXOffset; }
    int yOffset() { return m_iYOffset; }

    const KivioPage* activePage() const;
    KivioPage* activePage();
    KivioPage* findPage( const QString& _name );

    KivioView* view()const  { return m_pView; }
    KivioDoc* doc()const { return m_pDoc; }

    QSize actualSize();

    virtual bool event(QEvent*);
    virtual bool eventFilter(QObject*, QEvent*);

    enum RectType { Insert, Rubber };
    void startRectDraw( const QPoint &p, RectType t );
    void continueRectDraw( const QPoint &p, RectType t );
    void endRectDraw();
    QRect rect()const { return currRect; }

    void drawSelectedStencilsXOR();
    void drawStencilXOR( KivioStencil * );

    KoPoint snapToGrid(KoPoint);
    KoPoint snapToGuides(KoPoint, bool &, bool &);
    KoPoint snapToGridAndGuides(KoPoint);
    double snapToGridX(double);
    double snapToGridY(double);

    KoPoint mapFromScreen( const QPoint& );
    QPoint mapToScreen( KoPoint );

    void beginUnclippedSpawnerPainter();
    void endUnclippedSpawnerPainter();

    void eraseGuides();
    void paintGuides(bool=true);

    void setViewCenterPoint(const KoPoint &);

    KoRect visibleArea();
    void setVisibleArea(KoRect, int margin = 0);
    void setVisibleAreaByWidth(KoRect, int margin = 0);
    void setVisibleAreaByHeight(KoRect, int margin = 0);

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

    void updateGuides();

    void updateScrollBars();

    void startPasteMoving();

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

    virtual void keyReleaseEvent( QKeyEvent * );

    void beginUnclippedPainter();
    void endUnclippedPainter();

    void paintSelectedXOR();

    void updateGuidesCursor();

    void continuePasteMoving(const QPoint &pos);
    void endPasteMoving();

  protected slots:
    void borderTimerTimeout();
    void guideLinesTimerTimeout();

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
    QValueList<Kivio::Object*> m_dragObjectList;

    QTimer* m_borderTimer;
    QTimer* m_guideLinesTimer;

    bool delegateThisEvent;
    QCursor* storedCursor;
    KivioGuideLineData* pressGuideline;
    QPoint lastPoint;

    bool m_pasteMoving;
    QPtrList<KoRect> m_lstOldGeometry;
    KoPoint m_origPoint;
};

#endif
