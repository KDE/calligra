#ifndef __ko_frame_h__
#define __ko_frame_h__

#include "koffice.h"
#include <opFrame.h>
#include <komBase.h>

#include <qcursor.h>
#include <qpicture.h>

class KoFrame;

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

class KoFrame : public OPFrame,
		virtual public KOMBase,
		virtual public KOffice::Frame_skel
{
  friend KoFrameMove;
  friend KoFrameResize;
  
  Q_OBJECT
public:
  // C++
  KoFrame( QWidget *_parent = 0L, const char *_name = 0L );
  /**
   * The Destructor calls @ref #detach automatically. After that the controls 'destroy'
   * function is called.
   */
  ~KoFrame();

  // IDL
  virtual void viewChangedState( CORBA::Boolean is_marked, CORBA::Boolean has_focus );

  // IDL
  virtual KOffice::View_ptr view();
  
  // C++
  /**
   * @return true if the function was successfull. The window
   *         will be swallowed once this widget is redrawn.
   *         You can force this by a call to @ref QWidget::update.
   */
  virtual bool attachView( KOffice::View_ptr _view );
  virtual void attach( QPicture * );
  virtual void detach();
  
  // C++
  virtual CORBA::ULong leftGUISize();
  virtual CORBA::ULong rightGUISize();
  virtual CORBA::ULong topGUISize();
  virtual CORBA::ULong bottomGUISize();

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


