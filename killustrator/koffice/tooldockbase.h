/*
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
#ifndef TOOLDOCKBASE_H
#define TOOLDOCKBASE_H

#include <qwidget.h>
#include <qintdict.h>
#include <qframe.h>
#include <qtimer.h>

enum ToolDockPosition { ToolDockLeft, ToolDockRight, ToolDockTop, ToolDockBottom, ToolDockCenter };

class QGridLayout;
class StencilBarMoveManager;
class ToolDockBase;
class ToolDockManager;

class ToolDockBaseBorder : public QWidget
{ Q_OBJECT
public:
  enum Position { Left, Right, Top, Bottom, TopLeft, TopRight, BottomLeft, BottomRight };

  ToolDockBaseBorder( Position, ToolDockBase* parent, const char* name = 0 );
  ~ToolDockBaseBorder();

  Position position() { return pos; }

signals:
  void resizeStart();
  void resizeStop();

protected:
  void paintEvent( QPaintEvent* );
  void mousePressEvent( QMouseEvent* );
  void mouseReleaseEvent( QMouseEvent* );

private:
  Position pos;
};
/******************************************************************************/
class ToolDockButton : public QFrame
{ Q_OBJECT
public:
  ToolDockButton( QWidget* parent, const char* name = 0 );
  ~ToolDockButton();

  void setPixmap( const QPixmap& );
  void setToggled( bool );
  void setDown( bool );

signals:
  void clicked();
  void toogled(bool);

protected:
  void paintEvent( QPaintEvent* );
  void mousePressEvent( QMouseEvent* );
  void mouseReleaseEvent( QMouseEvent* );
  void enterEvent( QEvent* );
  void leaveEvent( QEvent* );

private:
  QPixmap* pixmap;
  bool tg;
  bool down;
  bool isin;
  bool mdown;
};
/******************************************************************************/
class ToolDockBaseCaption : public QWidget
{ Q_OBJECT
public:
  ToolDockBaseCaption( ToolDockPosition, ToolDockBase* parent, const char* name = 0 );
  ~ToolDockBaseCaption();

signals:
  void moveStart();
  void moveStop();

  void doClose();
  void doStick(bool);

public slots:
  void stick(bool);

protected slots:
  void slotClose();
  void slotStick(bool);

protected:
  void paintEvent( QPaintEvent* );
  void mousePressEvent( QMouseEvent* );
  void mouseMoveEvent( QMouseEvent* );
  void mouseReleaseEvent( QMouseEvent* );

private:
  bool m_bPressed;
  bool m_bMove;
  QPoint m_ppoint;
  ToolDockPosition position;
  ToolDockButton* closeButton;
  ToolDockButton* stickButton;
};
/******************************************************************************/
class ToolDockBaseCaptionManager : public QObject
{ Q_OBJECT
public:
  ToolDockBaseCaptionManager( ToolDockBase* parent, const char* name = 0 );
  ~ToolDockBaseCaptionManager();

  void setView( ToolDockPosition );
  ToolDockBaseCaption* captionWidget( ToolDockPosition );
  int captionHeight();

signals:
  void doClose();
  void doStick(bool);

protected slots:
  void slotClose();
  void slotStick(bool);

private:
  ToolDockBaseCaption* m_pLeft;
  ToolDockBaseCaption* m_pRight;
  ToolDockBaseCaption* m_pTop;
  ToolDockBaseCaption* m_pBottom;
};
/******************************************************************************/
class ToolDockSnap
{
public:
  void clear();
  void set(ToolDockPosition,QWidget*);
  QWidget* get(ToolDockPosition);

private:
  QIntDict<QWidget> snaps;
};
/******************************************************************************/
class ToolDockBase : public QWidget
{ Q_OBJECT
friend class ToolDockManager;
public:
  virtual ~ToolDockBase();

  void activate();

private:
  ToolDockBase( QWidget* parent, const QString& caption, const char* name = 0 );

  void setView( QWidget* );

public slots:
  void show();
  void hide();
  void makeVisible(bool);

signals:
  void visibleChange(bool);

protected slots:
  void beginResize();
  void stopResize();

  void beginMove();
  void stopMove();

  void positionChanged();
  void sizeChanged();

  void fixPosition(int& x, int& y, int& w, int& h);
  void fixSize(int& x, int& y, int& w, int& h);

  void slotStick(bool);
  bool isStick() { return stick; }

protected:
  void enterEvent( QEvent* );
  void mousePressEvent( QMouseEvent* );
  void resizeEvent( QResizeEvent* );
  void paintEvent( QPaintEvent* );

  void updateCaption();
  ToolDockPosition getCaptionPos( bool* = 0L );
  void mouseStatus(bool);

protected slots:
  void slotHideTimeOut();
  void slotHideProcessTimeOut();
  void hideProcessStop();
  void showProcessStop();

private:
  bool stick;
  QWidget* m_pView;
  QGridLayout* m_pBaseLayout;

  ToolDockBaseBorder* m_pBorderLeft;
  ToolDockBaseBorder* m_pBorderRight;
  ToolDockBaseBorder* m_pBorderTop;
  ToolDockBaseBorder* m_pBorderBottom;

  ToolDockBaseBorder* m_pBorderTopLeft;
  ToolDockBaseBorder* m_pBorderTopRight;
  ToolDockBaseBorder* m_pBorderBottomRight;
  ToolDockBaseBorder* m_pBorderBottomLeft;

  ToolDockBaseCaptionManager* m_pCaptionManager;

  StencilBarMoveManager* mrManager;
  ToolDockSnap snaps;

  QGridLayout* m_pLayout;

  int hdx;
  QPixmap* hpixmap;
  QSize hminsize;
  QSize hmaxsize;
  QSize hsize;
  QTimer hideTimer;
  QTimer hideProcessTimer;
  ToolDockPosition hideDirection;
  enum hstatus { hnone, hstarthide, hprocesshide, hdonehide, hprocessshow };
  hstatus hStatus;
};

#endif

