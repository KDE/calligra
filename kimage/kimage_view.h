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

#ifndef __kimage_gui_h__
#define __kimage_gui_h__

#include <qpixmap.h>
#include <qwidget.h>

#include <kprocess.h>

#include <opMenu.h>
#include <opToolBar.h>
#include <openparts_ui.h>

#include <koFrame.h>
#include <koView.h>

#include "kimage.h"

class KImageDoc;

class KImageView : public QWidget,
		   virtual public KoViewIf,
		   virtual public KImage::View_skel
{
  Q_OBJECT

public:
  KImageView( QWidget* _parent, const char* _name, KImageDoc* _doc );
  ~KImageView();
  KImageDoc* doc();

  void editEditImage();
  void editImportImage();
  void editExportImage();
  void editPageLayout();
  void editPreferences();

  void viewZoomFactor();
  void viewFitToView();
  void viewFitWithProportions();
  void viewOriginalSize();
  void viewCentered();
  void viewInfoImage();
  void viewBackgroundColor();

  void transformRotateRight();
  void transformRotateLeft();
  void transformRotateAngle();
  void transformFlipVertical();
  void transformFlipHorizontal();
  void transformZoomFactor();
  void transformZoomIn10();
  void transformZoomOut10();
  void transformZoomDouble();
  void transformZoomHalf();
  void transformZoomMax();
  void transformZoomMaxAspect();

  virtual void cleanUp();
  bool printDlg();

public slots:
  // Document signals
  void slotUpdateView();

protected:
  enum DrawMode { OriginalSize, FitToView, FitWithProps, ZoomFactor };

  virtual void init();
  virtual bool event( const QCString & _event, const CORBA::Any& _value );
  virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
  virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );
  virtual void newView();
  virtual void helpUsing();
  virtual void resizeEvent( QResizeEvent* _ev );
  virtual void paintEvent( QPaintEvent* _ev );
  QString tmpFilename();
  void executeCommand( KProcess& proc );

  // edit toolbar
  OpenPartsUI::ToolBar_var m_vToolBarEdit;
  long int m_idButtonEdit_Lines;
  long int m_idButtonEdit_Areas;
  long int m_idButtonEdit_Bars;
  long int m_idButtonEdit_Cakes;

  // edit menu
  OpenPartsUI::Menu_var m_vMenuEdit;
  long int m_idMenuEdit_Undo;
  long int m_idMenuEdit_Redo;
  long int m_idMenuEdit_Edit;
  long int m_idMenuEdit_Import;
  long int m_idMenuEdit_Export;
  long int m_idMenuEdit_Page;
  long int m_idMenuEdit_Preferences;

  // view menu
  OpenPartsUI::Menu_var m_vMenuView;
  long int m_idMenuView_ZoomFactor;
  long int m_idMenuView_FitToView;
  long int m_idMenuView_FitWithProps;
  long int m_idMenuView_Original;
  long int m_idMenuView_Center;
  long int m_idMenuView_Info;
  long int m_idMenuView_BackgroundColor;

  // transform menu
  OpenPartsUI::Menu_var m_vMenuTransform;
  long int m_idMenuTransform_RotateRight;
  long int m_idMenuTransform_RotateLeft;
  long int m_idMenuTransform_RotateAngle;
  long int m_idMenuTransform_FlipVertical;
  long int m_idMenuTransform_FlipHorizontal;
  long int m_idMenuTransform_ZoomFactor;
  long int m_idMenuTransform_ZoomIn10;
  long int m_idMenuTransform_ZoomOut10;
  long int m_idMenuTransform_ZoomDouble;
  long int m_idMenuTransform_ZoomHalf;
  long int m_idMenuTransform_ZoomMax;
  long int m_idMenuTransform_ZoomMaxAspect;

  // help menu
  OpenPartsUI::Menu_var m_vMenuHelp;
  long int m_idMenuHelp_About;
  long int m_idMenuHelp_Using;

private:
  QPoint m_zoomFactor;
  KImageDoc* m_pDoc;
  QPixmap m_pixmap;
  DrawMode m_drawMode;
  int m_centerMode;
};

#endif
