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
 
#include <koFrame.h>
#include <koView.h>

#include <opMenu.h>
#include <opToolBar.h>
#include <openparts_ui.h>

#include "kimage.h"

class KImageDoc;
class KImageShell;

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
    
  void extrasRunGimp();
  void extrasRunXV();
  void extrasRunCommand();

  virtual void cleanUp();
  CORBA::Boolean printDlg();

public slots:
  // Document signals
  void slotUpdateView();

protected:
  virtual void init();
  virtual bool event( const char* _event, const CORBA::Any& _value );
  virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
  virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );
  virtual void newView();
  virtual void helpUsing();
  virtual void resizeEvent( QResizeEvent* _ev );
  virtual void paintEvent( QPaintEvent* _ev );
  QString tmpFilename();

  // edit toolbar
  OpenPartsUI::ToolBar_var m_vToolBarEdit;
  CORBA::Long m_idButtonEdit_Lines;
  CORBA::Long m_idButtonEdit_Areas;
  CORBA::Long m_idButtonEdit_Bars;
  CORBA::Long m_idButtonEdit_Cakes;

  // edit menu
  OpenPartsUI::Menu_var m_vMenuEdit;
  CORBA::Long m_idMenuEdit_Edit;
  CORBA::Long m_idMenuEdit_Import;
  CORBA::Long m_idMenuEdit_Export;
  CORBA::Long m_idMenuEdit_Page;
  CORBA::Long m_idMenuEdit_Preferences;

  // view menu
  OpenPartsUI::Menu_var m_vMenuView;
  CORBA::Long m_idMenuView_FitToView;
  CORBA::Long m_idMenuView_FitWithProps;
  CORBA::Long m_idMenuView_Original;
  CORBA::Long m_idMenuView_Center;
  CORBA::Long m_idMenuView_Info;
  CORBA::Long m_idMenuView_BackgroundColor;

  // transform menu
  OpenPartsUI::Menu_var m_vMenuTransform;
  CORBA::Long m_idMenuTransform_RotateRight;
  CORBA::Long m_idMenuTransform_RotateLeft;
  CORBA::Long m_idMenuTransform_RotateAngle;
  CORBA::Long m_idMenuTransform_FlipVertical;
  CORBA::Long m_idMenuTransform_FlipHorizontal;
  CORBA::Long m_idMenuTransform_ZoomFactor;
  CORBA::Long m_idMenuTransform_ZoomIn10;
  CORBA::Long m_idMenuTransform_ZoomOut10;
  CORBA::Long m_idMenuTransform_ZoomDouble;
  CORBA::Long m_idMenuTransform_ZoomHalf;
  CORBA::Long m_idMenuTransform_ZoomMax;
  CORBA::Long m_idMenuTransform_ZoomMaxAspect;

  // filter menu
  OpenPartsUI::Menu_var m_vMenuFilter;

  // plugins menu
  OpenPartsUI::Menu_var m_vMenuPlugIns;

  // extras menu
  OpenPartsUI::Menu_var m_vMenuExtras;
  CORBA::Long m_idMenuExtras_RunGimp;
  CORBA::Long m_idMenuExtras_RunXV;
  CORBA::Long m_idMenuExtras_RunCommand;

  // help menu
  OpenPartsUI::Menu_var m_vMenuHelp;
  CORBA::Long m_idMenuHelp_About;
  CORBA::Long m_idMenuHelp_Using;
    
private:
  KImageDoc* m_pDoc;  
  QPixmap m_pixmap;
  int m_drawMode;
  int m_centerMode;
};

#endif
