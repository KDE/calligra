/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __ko_frame_h__
#define __ko_frame_h__

#include "koffice.h"
#include <opFrame.h>
#include <komBase.h>

#include <qcursor.h>
#include <qpicture.h>

class KoFrame;
class KoViewIf;

/**
 * Used internally to represent the resize handles
 * of the frame.
 */
class KoFrameResize : public QWidget
{
  Q_OBJECT
public:
  KoFrameResize( KoFrame *_part, const QCursor &cursor, int _pos );
  ~KoFrameResize();

protected:
  void mousePressEvent( QMouseEvent *_ev );
  void mouseMoveEvent( QMouseEvent *_ev );
  void mouseReleaseEvent( QMouseEvent *_ev );

  int position;
  int xPress;
  int yPress;

  KoFrame *m_pFrame;
};

/**
 * Used internally to represent the move handles of
 * the frame.
 */
class KoFrameMove : public QWidget
{
  Q_OBJECT
public:
  KoFrameMove( KoFrame *_part, const QCursor &cursor, int _pos );
  ~KoFrameMove();

protected:
  void mousePressEvent( QMouseEvent *_ev );
  void mouseMoveEvent( QMouseEvent *_ev );
  void mouseReleaseEvent( QMouseEvent *_ev );

  int position;
  int xPress;
  int yPress;
  int xInit;
  int yInit;

  KoFrame *m_pFrame;
};

/**
 * The KoFrame can embed another KOffice Component. In detail it
 * embeds a @ref KoView. That means that you can have one embedded
 * document but many KoFrames where each KoFrame embeds one KoView
 * of the document.
 *
 * @author Torben Weis
 */
class KoFrame : public OPFrame,
		virtual public KOMBase,
		virtual public KOffice::Frame_skel
{
  friend KoFrameMove;
  friend KoFrameResize;

  Q_OBJECT
public:
  // C++
  /**
   * A frame is just a usual Qt widget with some magic inside :-)
   */
  KoFrame( QWidget *_parent = 0L, const char *_name = 0L );
  /**
   * The Destructor calls @ref #detach automatically. After that the controls 'destroy'
   * function is called.
   */
  ~KoFrame();

  // IDL
  virtual void viewChangedState( bool is_marked, bool has_focus );

  // IDL
  virtual KOffice::View_ptr view();

  // C++
  /**
   * @return true if the function was successfull. The window
   *         will be swallowed once this widget is redrawn.
   *         You can force this by a call to @ref QWidget::update.
   */
  virtual bool attachView( KOffice::View_ptr _view );
  /**
   * Use this function if the KoView and the KoFrame run in
   * the same process. Reparenting and all this stuff is
   * skipped in this case.
   *
   * @return true if the function was successfull.
   *
   * @see OPFrame::attachLocal
   */
  virtual bool attachLocalView( KoViewIf* _view );
  virtual void attach( QPicture * );
  /**
   * Forget about the embedded view and free all related
   * resources.
   */
  virtual void detach();

  // C++
  /**
   * Tells how many pixels the part needs on the
   * left when it gets keyboard focus. This space
   * is needed for extra rulers and stuff.
   */
  virtual unsigned long int leftGUISize();
  /**
   * Tells how many pixels the part needs on the
   * right when it gets keyboard focus. This space
   * is needed for extra rulers and stuff.
   */
  virtual unsigned long int rightGUISize();
  /**
   * Tells how many pixels the part needs on the
   * top when it gets keyboard focus. This space
   * is needed for extra rulers and stuff.
   */
  virtual unsigned long int topGUISize();
  /**
   * Tells how many pixels the part needs on the
   * bottom when it gets keyboard focus. This space
   * is needed for extra rulers and stuff.
   */
  virtual unsigned long int bottomGUISize();

  // C++
  virtual QRect partGeometry();
  virtual void setPartGeometry( const QRect& );

signals:
  void sig_geometryStart( KoFrame* );
  void sig_geometryEnd( KoFrame* );
  void sig_moveStart( KoFrame* );
  void sig_moveEnd( KoFrame* );
  void sig_popupMenu(  KoFrame*, const QPoint& _point );
  // void sig_attachPart( KoFrame* );

protected:
  virtual void paintEvent( QPaintEvent * );
  virtual void resizeEvent( QResizeEvent * );
  virtual void mousePressEvent( QMouseEvent * );

  virtual void showBorders( bool _mode );
  virtual void setResizeMode( bool _m );
  virtual void showGUI( bool _mode );

  virtual void popupMenu( const QPoint& _point );
  virtual void geometryStart();
  virtual void geometryEnd();
  virtual void moveStart();
  virtual void moveEnd();

  bool m_bShowBorders;
  /**
   * Indicates wether we show the boxes that allow the user to resize this part.
   */
  bool m_bResizeMode;
  bool m_bShowGUI;

  KOffice::View_var m_vView;
  OpenParts::Window m_wView;
  KoViewIf *m_pView;

  KoFrameResize *m_pResizeWin1;
  KoFrameResize *m_pResizeWin2;
  KoFrameResize *m_pResizeWin3;
  KoFrameResize *m_pResizeWin4;
  KoFrameResize *m_pResizeWin5;
  KoFrameResize *m_pResizeWin6;
  KoFrameResize *m_pResizeWin7;
  KoFrameResize *m_pResizeWin8;

  KoFrameMove *m_pMoveWin1;
  KoFrameMove *m_pMoveWin2;
  KoFrameMove *m_pMoveWin3;
  KoFrameMove *m_pMoveWin4;

  QPicture* m_pPicture;
};

#endif
