/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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

#include <koQueryTrader.h>

#include "kivio_point.h"
#include "kivio_rect.h"
#include "kivio_intra_stencil_data.h"
#include "tkunits.h"

class KivioView;
class KivioCanvas;
class KivioPage;
class KivioDoc;
class KivioRuler;
class ToolController;
class KivioGuideLineData;

class KivioScreenPainter;
class KivioStencil;

class QPainter;
class QLabel;
class QPixmap;
class QScrollBar;
class QTimer;

#define YBORDER_WIDTH 50
#define XBORDER_HEIGHT 20


class KivioCanvas : public QWidget
{ Q_OBJECT
friend KivioView;
public:
  KivioCanvas( QWidget*, KivioView*, KivioDoc*, ToolController*, QScrollBar*, QScrollBar*, KivioRuler*, KivioRuler* );
  ~KivioCanvas();
  /**
   * Called from @ref KivioView to complete the construction. Has to
   * be called before any other method on this object may be invoced.
   */
  int xOffset() { return m_iXOffset; }
  int yOffset() { return m_iYOffset; }

  QPoint actualPaperOrigin();

  const KivioPage* activePage() const;
  KivioPage* activePage();
  KivioPage* findPage( const QString& _name );

  KivioView* view() { return m_pView; }
  KivioDoc* doc() { return m_pDoc; }

  float zoom();

  QSize actualSize();

  virtual bool event(QEvent*);
  virtual bool eventFilter(QObject*, QEvent*);

  enum RectType { Insert, Rubber };
  void startRectDraw( const QPoint &p, RectType t );
  void continueRectDraw( const QPoint &p, RectType t );
  void endRectDraw();
  QRect rect() { return currRect; }

  void startSpawnerDragDraw( const QPoint &p );
  void continueSpawnerDragDraw( const QPoint &p );
  void endSpawnerDragDraw();

  void drawSelectedStencilsXOR();
  void drawStencilXOR( KivioStencil * );

  TKPoint snapToGrid(TKPoint);
  TKPoint snapToGuides(TKPoint, bool &, bool &);
  TKPoint snapToGridAndGuides(TKPoint);
  float snapToGridX(float);
  float snapToGridY(float);

  TKPoint mapFromScreen( QPoint );
  QPoint mapToScreen( TKPoint );

  void beginUnclippedSpawnerPainter();
  void endUnclippedSpawnerPainter();

  void eraseGuides();
  void paintGuides(bool=true);

  void centerPage();
  void setViewCenterPoint(KivioPoint);

  KivioRect visibleArea();
  void setVisibleArea(KivioRect, int margin = 0);
  void setVisibleAreaByWidth(KivioRect, int margin = 0);
  void setVisibleAreaByHeight(KivioRect, int margin = 0);

signals:
  void zoomChanges(float);
  void visibleAreaChanged();

public slots:
  virtual void setUpdatesEnabled(bool);

  void toggleShowRulers(bool);
  void setZoom(float);
  void zoomIn(QPoint);
  void zoomOut(QPoint);

  void scrollDx(int dx);
  void scrollDy(int dy);

  void scrollV(int value);
  void scrollH(int value);

  void updateGuides();

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

  void updateScrollBars();
  void updateRulers( bool horiz, bool vert );
  TKSize actualPaperSizePt();
  TKSize actualGridFrequency();

  void beginUnclippedPainter();
  void endUnclippedPainter();

  void paintSelectedXOR();

  void updateGuidesCursor();

protected slots:
  void borderTimerTimeout();
  void guideLinesTimerTimeout();

private:
  KivioView* m_pView;
  KivioDoc* m_pDoc;

  ToolController* m_pToolsController;

  QScrollBar* m_pVertScrollBar;
  QScrollBar* m_pHorzScrollBar;

  KivioRuler* m_pVRuler;
  KivioRuler* m_pHRuler;

  int m_iXOffset;
  int m_iYOffset;

  QPixmap* m_buffer;

  int m_pScrollX;
  int m_pScrollY;
  QPointArray gridLines;

  float m_pZoom;

  bool oldRectValid;
  QRect currRect;
  QPoint rectAnchor;
  QPainter* unclippedPainter;
  QPoint sizePreviewPos;

  KivioScreenPainter* unclippedSpawnerPainter;
  KivioIntraStencilData m_dragStencilData;
  KivioStencil* m_pDragStencil;

  QTimer* m_borderTimer;
  QTimer* m_guideLinesTimer;

  bool delegateThisEvent;
  QCursor* storedCursor;
  KivioGuideLineData* pressGuideline;
  QPoint lastPoint;
};

#endif
