#ifndef TKCOLORACTION_H
#define TKCOLORACTION_H

#include "tkaction.h"

#include <kpopupmenu.h>

#include <qlayout.h>
#include <qcolor.h>
#include <qdict.h>

#include <qdom.h>

class TKColorPanel;

class TKColorPopupMenu : public KPopupMenu
{ Q_OBJECT
public:
  TKColorPopupMenu( QWidget* parent = 0, const char* name = 0 );
  ~TKColorPopupMenu();

public slots:
  void updateItemSize();
};
/****************************************************************************************/
class TKSelectColorAction : public TKAction
{ Q_OBJECT
public:
  enum Type {
    TextColor,
    LineColor,
    FillColor,
    Color
  };

  TKSelectColorAction( const QString& text, Type type, QObject* parent, const char* name, bool inMenu );
  TKSelectColorAction( const QString& text, Type type,
                       QObject* receiver, const char* slot,
                       QObject* parent, const char* name, bool inMenu );

  virtual ~TKSelectColorAction();

  QColor color() const { return m_pCurrentColor; }

  KPopupMenu* popupMenu() { return m_pMenu; }


public slots:
  void setCurrentColor( const QColor& );
  void setActiveColor( const QColor& );
  virtual void activate();

signals:
  void colorSelected( const QColor& );

protected slots:
  void selectColorDialog();
  void panelColorSelected( const QColor& );
  void panelReject();
  virtual void slotActivated();

protected:
  void init();
  virtual void initToolBarButton(TKToolBarButton*);
  void updatePixmap();
  void updatePixmap(TKToolBarButton*);

protected:
  TKColorPopupMenu* m_pMenu;
  TKColorPanel* m_pStandardColor;
  TKColorPanel* m_pRecentColor;
  int m_type;

  QColor m_pCurrentColor;

private:
  class TKSelectColorActionPrivate;
  TKSelectColorActionPrivate *d;
  bool m_inMenu;
};
/****************************************************************************************/
class TKColorPanelButton : public QFrame
{ Q_OBJECT
public:
  TKColorPanelButton( const QColor&, QWidget* parent, const char* name = 0 );
  ~TKColorPanelButton();

  void setActive( bool );

  QColor panelColor() { return m_Color; }

signals:
  void selected( const QColor& );

protected:
  virtual void paintEvent( QPaintEvent* );
  virtual void enterEvent( QEvent* );
  virtual void leaveEvent( QEvent* );
  virtual void mouseReleaseEvent( QMouseEvent* );

  QColor m_Color;
  bool m_bActive;

private:
  class TKColorPanelButtonPrivate;
  TKColorPanelButtonPrivate *d;
};
/****************************************************************************************/
class TKColorPanel : public QWidget
{ Q_OBJECT

public:
  TKColorPanel( QWidget* parent = 0L, const char* name = 0 );
  ~TKColorPanel();

  void setActiveColor( const QColor& );
  void setNumCols( int col );
  void clear();

public slots:
  void insertColor( const QColor& );
  void insertColor( const QColor&, const QString& );
  void selected( const QColor& );

signals:
  void colorSelected( const QColor& );
  void reject();
  void sizeChanged();

protected:
  void addToGrid( TKColorPanelButton* );
  void resetGrid();

  virtual void mouseReleaseEvent( QMouseEvent* );
  virtual void showEvent( QShowEvent *e );

  QGridLayout* m_pLayout;
  int m_iWidth;
  int m_iX;
  int m_iY;

  QColor m_activeColor;
  QDict<TKColorPanelButton> m_pColorDict;

private:
  void fillPanel();

  class TKColorPanelPrivate;
  TKColorPanelPrivate *d;
};

#endif
