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
#ifndef __kivio_gui_h__
#define __kivio_gui_h__

class KivioView;
class KivioCanvas;
class KivioPage;
class KivioDoc;
class KivioRuler;
class KivioPainter;
class KivioPaperLayout;
class KivioParagraphAction;
class KivioShell;
class KivioStackBar;
class KivioStencilSpawner;
class KivioStencilSpawnerSet;
class KivioTabBar;
class ZoomAction;
class TKSelectColorAction;
class ToolController;
class StencilBarDockManager;
class ToolDockBase;
class ToolDockManager;

class KivioBirdEyePanel;
class KivioLayerPanel;
class KivioStencilGeometryPanel;
class KivioViewManagerPanel;

class KoDocumentEntry;

class KAction;
class KSelectAction;
class KFontAction;
class KFontSizeAction;
class KToggleAction;

class KSelectColorAction;
class KActionMenu;


class LineEndsAction;

class TKUFloatSpinBoxAction;
class TKSizeAction;
class TKPositionAction;
class TKUnitsAction;

#include <qdom.h>
#include <qlist.h>
#include <qframe.h>
#include <qbutton.h>
#include <qpoint.h>
#include <qstringlist.h>

#include <koView.h>
#include <koDocument.h>
#include <koDataTool.h>

class KivioView : public KoView
{ Q_OBJECT
friend KivioCanvas;
public:
  KivioView( QWidget *_parent, const char *_name, KivioDoc *_doc );
  ~KivioView();

  ToolController* toolsController() { return m_pTools; }
  KivioCanvas* canvasWidget() const { return m_pCanvas; }
  KivioDoc* doc() { return m_pDoc; }

  void addPage( KivioPage* );
  void removePage( KivioPage* );
  void setActivePage( KivioPage* );
  KivioPage* activePage();

  void setActiveSpawnerSet( KivioStencilSpawnerSet* );
  KivioStencilSpawnerSet *activeSpawnerSet();

  KivioTabBar* tabBar() { return  m_pTabBar;}
  ToolDockManager* toolDockManager() { return m_pToolDock; }

  virtual void setupPrinter(QPrinter&);
  virtual void print(QPrinter&);

  void paintContent( KivioPainter& painter, const QRect& rect, bool transparent );

  virtual QWidget* canvas();
  virtual int canvasXOffset() const;
  virtual int canvasYOffset() const;

  bool isSnapGuides() { return m_bSnapGuides; }
  bool isShowGuides() { return m_bShowGuides; }
  bool isShowRulers() { return m_bShowRulers; }
  bool isShowPageBorders() { return m_bShowPageBorders; }
  bool isShowPageMargins() { return m_bShowPageMargins; }

  virtual int leftBorder() const;
  virtual int rightBorder() const;
  virtual int topBorder() const;
  virtual int bottomBorder() const;


  // Returns the current interface color/lineWidth settings
  QColor fgColor();
  QColor bgColor();
  int lineWidth();

protected:
  virtual void customEvent( QCustomEvent* );

  void createGeometryDock();
  void createViewManagerDock();
  void createLayerDock();
  void createBirdEyeDock();

public slots:
  void paperLayoutDlg();

  void togglePageBorders(bool);
  void togglePageMargins(bool);
  void toggleShowRulers(bool);
  void toggleShowGrid(bool);
  void toggleSnapGrid(bool);
  void toggleShowGuides(bool);
  void toggleSnapGuides(bool);

  void toggleStencilGeometry(bool);
  void toggleViewManager(bool);
  void toggleLayersPanel(bool);
  void toggleBirdEyePanel(bool);

  void insertPage();
  void removePage();
  void hidePage();
  void showPage();
  void viewZoom(int);

  void groupStencils();
  void ungroupStencils();

  void selectAllStencils();
  void unselectAllStencils();

  void bringStencilToFront();
  void sendStencilToBack();

  void addStencilFromSpawner( KivioStencilSpawner * );

  void changePage( const QString& _name );

  void viewGUIActivated(bool);

  void updateToolBars();

  void cutStencil();
  void copyStencil();
  void pasteStencil();

  void alignStencilsDlg();
  void optionsDialog();

protected slots:
  void slotAddPage( KivioPage *_page );
  void slotPageRenamed( KivioPage* page, const QString& old_name );
  void slotUpdateView( KivioPage *_page );

  void setFGColor();
  void setBGColor();
  void setTextColor();

  void setFontFamily( const QString & );
  void setFontSize( const QString & );

  void setLineWidth();

  void toggleFontBold(bool);
  void toggleFontItalics(bool);
  void toggleFontUnderline(bool);

  void setHParaAlign( int );
  void setVParaAlign( int );

  void slotSetStartArrow( int );
  void slotSetEndArrow( int );

  void slotSetStartArrowSize();
  void slotSetEndArrowSize();

  void slotChangeStencilPosition(float, float);
  void slotChangeStencilSize(float, float);

  void canvasZoomChanged(float);

  void addSpawnerToStackBar( KivioStencilSpawnerSet * );
  void addStencilSet( const QString& );

protected:
  void setupActions();
  void initActions();

  virtual void updateReadWrite( bool readwrite );

  QButton* newIconButton( const char* file, bool kbutton = false, QWidget* parent = 0 );

private:
  KivioCanvas *m_pCanvas;
  QButton *m_pTabBarFirst;
  QButton *m_pTabBarLeft;
  QButton *m_pTabBarRight;
  QButton *m_pTabBarLast;
  KivioTabBar *m_pTabBar;

  KAction* m_paperLayout;
  KAction* m_insertPage;
  KAction* m_removePage;
  KAction* m_hidePage;
  KAction* m_showPage;
  ZoomAction* m_viewZoom;
  TKUnitsAction* m_unitAct;

  TKSelectColorAction *m_setFGColor;
  TKSelectColorAction *m_setBGColor;

  KFontAction *m_setFontFamily;
  KFontSizeAction *m_setFontSize;
  KToggleAction *m_setBold;
  KToggleAction *m_setItalics;
  KToggleAction *m_setUnderline;
  TKSelectColorAction *m_setTextColor;
  KivioParagraphAction *m_setHTextAlignment;
  KivioParagraphAction *m_setVTextAlignment;

  LineEndsAction *m_setStartArrow;
  LineEndsAction *m_setEndArrow;

  TKUFloatSpinBoxAction *m_setLineWidth;

  TKSizeAction *m_setEndArrowSize;
  TKSizeAction *m_setStartArrowSize;


  QStringList m_lineWidthList;

  KivioDoc* m_pDoc;
  KivioPage* m_pActivePage;
  KivioStencilSpawnerSet* m_pActiveSpawnerSet;

  ToolController* m_pTools;

  StencilBarDockManager* m_pDockManager;
  ToolDockManager* m_pToolDock;

  KivioRuler* vRuler;
  KivioRuler* hRuler;

  KivioStencilGeometryPanel* m_pStencilGeometryPanel;
  KivioViewManagerPanel* m_pViewManagerPanel;
  KivioLayerPanel* m_pLayersPanel;
  KivioBirdEyePanel* m_pBirdEyePanel;

  bool m_bShowPageBorders;
  bool m_bShowPageMargins;
  bool m_bShowRulers;
  bool m_bSnapGuides;
  bool m_bShowGuides;
};

#endif
