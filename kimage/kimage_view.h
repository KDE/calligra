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

class KImageView;
class KImageDoc;
class KImageShell;

#include <koFrame.h>
#include <koView.h>
#include <opMenu.h>
#include <opToolBar.h>
#include <openparts_ui.h>

#include <qpixmap.h>
#include <qwidget.h>

#include "kimage.h"

/**
 */
class KImageView : public QWidget,
		   virtual public KoViewIf,
		   virtual public KImage::View_skel
{
    Q_OBJECT
public:
    KImageView( QWidget *_parent, const char *_name, KImageDoc *_doc );
    ~KImageView();

    KImageDoc* doc() { return m_pDoc; }

    // IDL
    void fitToView();
    void fitWithProportions();
    void originalSize();
    void editImage();
    void pageLayout();
    void importImage();
    void exportImage();
    void infoImage();
    void centered();
    void rotateRight();
    void rotateLeft();
    void flipVertical();
    void flipHorizontal();
    void backgroundColor();
    void zoomFactor();
    void zoomIn10();
    void zoomOut10();
    void zoomDouble();
    void zoomHalf();
    void zoomMax();
    void zoomMaxAspect();
    void preferences();
    void runGimp();
    void runXV();
    void runCommand();

    virtual void cleanUp();

    CORBA::Boolean printDlg();
  
public slots:
    // Document signals
    void slotUpdateView();

protected:
  // C++
  virtual void init();
  
  // IDL
  virtual bool event( const char* _event, const CORBA::Any& _value );

  // C++
  virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
  virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );
  
  virtual void newView();
  virtual void helpUsing();

  virtual void resizeEvent( QResizeEvent *_ev );
  virtual void paintEvent( QPaintEvent *_ev );

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
    
  KImageDoc *m_pDoc;  

  QPixmap m_pixmap;
  int m_drawMode;
  int m_centerMode;
  
  int m_iXOffset;
  int m_iYOffset;
};

#endif
