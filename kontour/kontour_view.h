/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef __kontour_view_h__
#define __kontour_view_h__

#include <koView.h>

#include <qcolor.h>

#include "kontour_doc.h"
#include "units.h"

class KontourDocument;
class GDocument;
class Canvas;
class Ruler;
class TabBar;
class QSplitter;
class QLabel;
class QHBoxLayout;
class PluginManager;
class ToolController;
class KToggleAction;
class KSelectAction;
class KPopupMenu;
class SelectTool;
class RectTool;
class OvalTool;
class ZoomTool;
class QDockArea;
class QButton;
class QScrollBar;
class LayerPanel;
class KoColor;
class KoColorChooser;
class KontourView : public KoView
{
Q_OBJECT
public:
  KontourView(QWidget *parent, const char *name = 0, KontourDocument *doc = 0);
  ~KontourView();

  virtual bool eventFilter(QObject *o, QEvent *e);

  GDocument *activeDocument() const {return mDoc->document(); }

  Canvas *canvas() const {return mCanvas; }

  ToolController *toolController() const {return tcontroller; }

  MeasurementUnit unit() const {return mUnit; }
  void unit(MeasurementUnit u);

  QColor workSpaceColor() const {return mWorkSpaceColor; }
  void workSpaceColor(QColor c);

  void setStatus(QString s);

protected:
    virtual void updateReadWrite( bool readwrite );

    void customEvent(QCustomEvent *e);

    virtual void setupPrinter( KPrinter &printer );
    virtual void print( KPrinter &printer );



private:
  void setupActions();
  void initActions();
  void setupCanvas();
  void setupPanels();
  void setupTools();
  void readConfig();
  void readConfigAfter();
  void writeConfig();
  void updateStyles();

private slots:
  void popupForSelection();
  void popupForRulers();

  void changeOutlineColor(const KoColor &c);
  void changePaintColor(const KoColor &c);
  void changeSelection();

  void slotZoomFactorChanged();

  void slotCopy();
  void slotPaste();
  void slotCut();
  void slotDuplicate();
  void slotDelete();
  void slotSelectAll();
  void slotZoomIn();
  void slotZoomOut();
  void slotViewZoom(const QString &);
  void slotOutline();
  void slotNormal();
  void slotShowRuler(bool);
  void slotShowGrid(bool);
  void slotShowHelplines(bool);
  void slotAlignToGrid(bool);
  void slotAlignToHelplines(bool);
  void slotToFront();
  void slotToBack();
  void slotForwardOne();
  void slotBackOne();
  void slotGroup();
  void slotUngroup();
  void slotStyles(const QString &s);
  void slotAddStyle();
  void slotDeleteStyle();
  void slotDistribute();
  void slotConvertToPath();
  void slotBlend();
  void slotOptions();

private:
  KontourDocument *mDoc;

  /* Settings */
  MeasurementUnit mUnit;
  QColor mWorkSpaceColor;

  // GUI
  QSplitter *mSplitView;
  QDockArea *mRightDock;
  
  QButton *mTabBarFirst;
  QButton *mTabBarLeft;
  QButton *mTabBarRight;
  QButton *mTabBarLast;

  QScrollBar *hBar;
  QScrollBar *vBar;
  Canvas *mCanvas;
  Ruler *hRuler;
  Ruler *vRuler;
  TabBar *mTabBar;
  QHBoxLayout *tabLayout;

  KPopupMenu *objMenu;
  KPopupMenu *rulerMenu;

  /* Status bar */
  
  QLabel *mSBState;
  
  /* Panels */
  LayerPanel *mLayerPanel;
  KoColorChooser *mPaintPanel;
  KoColorChooser *mOutlinePanel;

  // Actions
  KAction *m_copy;
  KAction *m_paste;
  KAction *m_cut;
  KAction *m_duplicate;
  KAction *m_delete;
  KAction *m_selectAll;

  KAction *m_zoomIn;
  KAction *m_zoomOut;
  KSelectAction *m_viewZoom;

  KToggleAction *m_normal;
  KToggleAction *m_outline;

  KToggleAction *m_showRuler;
  KToggleAction *m_showGrid;
  KToggleAction *m_showHelplines;

  KToggleAction *m_snapToGrid;
  KToggleAction *m_alignToHelplines;

  KAction *m_toFront;
  KAction *m_toBack;
  KAction *m_forwardOne;
  KAction *m_backOne;

  KSelectAction *m_styles;
  KAction *m_addStyle;
  KAction *m_deleteStyle;

  KAction *m_convertToPath;

  KAction *m_options;

  /* Tools */
  ToolController *tcontroller;

  SelectTool *mSelectTool;
  RectTool *mRectTool;
  OvalTool *mOvalTool;
  ZoomTool *mZoomTool;

  /* */
  bool mShowRulers;
  QWidget *mParent;
  bool mShowGUI;
  KAction *m_distribute;
};

#endif
