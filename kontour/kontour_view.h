/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@linux.ru.net)

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
class QDockArea;
class QSplitter;
class QLabel;
class QHBoxLayout;
class KToggleAction;
class KSelectAction;
class KPopupMenu;
class QButton;
class QScrollBar;
class DCOPObject;
class KoColor;

namespace Kontour {
class GDocument;
class Canvas;
class Ruler;
class TabBar;
class ToolController;
class SelectTool;
class EditPointTool;
class ZoomTool;
class PathTool;
class RectTool;
class OvalTool;
class PolygonTool;
class TextTool;
class InsertImageTool;
class LayerPanel;
class PaintPanel;
class OutlinePanel;
class TransformPanel;
class AlignmentPanel;
class Command;
class GStyle;
};
using namespace Kontour;

class KontourView : public KoView
{
Q_OBJECT
public:
  KontourView(QWidget *parent, const char *name = 0, KontourDocument *doc = 0);
  virtual ~KontourView();
  virtual DCOPObject* dcopObject();

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


signals:
  void changedStyle(const GStyle &);

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

public slots:
  void popupForSelection();
  void popupForRulers();
  void changeSelection();

  void slotZoomFactorChanged();
  void slotCoordsChanged(double x, double y);

  void slotCopy();
  void slotPaste();
  void slotCut();
  void slotDuplicate();
  void slotDelete();
  void slotSelectAll();
  void slotDeselectAll();
  void slotZoomIn();
  void slotZoomOut();
  void slotViewZoom(const QString &);
  void slotOutline();
  void slotNormal();
  void slotShowRuler(bool b);
  void slotShowGrid(bool b);
  void slotShowHelplines(bool b);
  void slotAlignToGrid(bool b);
  void slotAlignToHelplines(bool b);
  void slotToFront();
  void slotToBack();
  void slotForwardOne();
  void slotBackOne();
  void slotGroup();
  void slotUngroup();
  void slotConvertToPath();
  void slotStyles(const QString &s);
  void slotAddStyle();
  void slotDeleteStyle();
  void slotDashEdit();
  void slotShowLayerPanel(bool b);
  void slotShowOutlinePanel(bool b);
  void slotShowPaintPanel(bool b);
  void slotShowTransformationPanel(bool b);
  void slotOptions();

private:
  KontourDocument     *mDoc;

  // Settings
  MeasurementUnit      mUnit;
  QColor               mWorkSpaceColor;

  // GUI
  QButton             *mTabBarFirst;
  QButton             *mTabBarLeft;
  QButton             *mTabBarRight;
  QButton             *mTabBarLast;

  QScrollBar          *hBar;
  QScrollBar          *vBar;
  Canvas              *mCanvas;
  Ruler               *hRuler;
  Ruler               *vRuler;
  TabBar              *mTabBar;
  QHBoxLayout         *tabLayout;

  KPopupMenu          *objMenu;
  KPopupMenu          *rulerMenu;

  // Status bar
  QLabel              *mSBCoords;
  QLabel              *mSBState;

  // Panels
  LayerPanel          *mLayerPanel;
  PaintPanel          *mPaintPanel;
  OutlinePanel        *mOutlinePanel;
  TransformPanel      *mTransformPanel;
  AlignmentPanel      *mAlignmentPanel;

  // Actions
  KAction             *m_copy;
  KAction             *m_paste;
  KAction             *m_cut;
  KAction             *m_duplicate;
  KAction             *m_delete;
  KAction             *m_selectAll;
  KAction             *m_deselectAll;

  KAction             *m_zoomIn;
  KAction             *m_zoomOut;
  KSelectAction       *m_viewZoom;

  KToggleAction       *m_normal;
  KToggleAction       *m_outline;

  KToggleAction       *m_showRuler;
  KToggleAction       *m_showGrid;
  KToggleAction       *m_showHelplines;

  KToggleAction       *m_snapToGrid;
  KToggleAction       *m_alignToHelplines;

  KAction             *m_toFront;
  KAction             *m_toBack;
  KAction             *m_forwardOne;
  KAction             *m_backOne;
  KAction             *m_group;
  KAction             *m_ungroup;
  KAction             *m_convertToPath;

  KSelectAction       *m_styles;
  KAction             *m_addStyle;
  KAction             *m_deleteStyle;
  KAction             *m_dashEdit;

  KToggleAction       *m_showLayerPanel;
  KToggleAction       *m_showOutlinePanel;
  KToggleAction       *m_showPaintPanel;
  KToggleAction       *m_showTransformationPanel;
  KAction             *m_options;

  // Tools
  ToolController *tcontroller;

  SelectTool          *mSelectTool;
  EditPointTool       *mEditPointTool;
  ZoomTool            *mZoomTool;
  PathTool            *mPathTool;
  RectTool            *mRectTool;
  OvalTool            *mOvalTool;
  PolygonTool         *mPolygonTool;
  TextTool            *mTextTool;
  InsertImageTool     *mInsertImageTool;

  //
  bool mShowRulers;
  bool mShowGUI;

  DCOPObject *mDCOP;
};

#endif
