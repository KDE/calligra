/*
 *  kimageshop_view.h - part of KImageShop
 *
 *  Copyright (c) 1999 The KImageShop team (see file AUTHORS)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __kimageshop_view_h__
#define __kimageshop_view_h__

#include <qpixmap.h>
#include <qwidget.h>

#include <kprocess.h>
 
#include <opMenu.h>
#include <opToolBar.h>
#include <openparts_ui.h>

#include <koFrame.h>
#include <koView.h>

#include "kimageshop.h"

class KImageShopDoc;

class KImageShopView : public QWidget,
		       virtual public KoViewIf,
		       virtual public KImageShop::View_skel
{
  Q_OBJECT
    
 public:
  KImageShopView( QWidget* _parent, const char* _name, KImageShopDoc* _doc );
  ~KImageShopView();
  
  KImageShopDoc* doc();
  
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

  OpenPartsUI::ToolBar_var m_vToolBarEdit;
  OpenPartsUI::Menu_var m_vMenuEdit;
  OpenPartsUI::Menu_var m_vMenuView;
  OpenPartsUI::Menu_var m_vMenuTransform;
  OpenPartsUI::Menu_var m_vMenuPlugIns;
  OpenPartsUI::Menu_var m_vMenuFilter;
  OpenPartsUI::Menu_var m_vMenuExtras;
  
private:
  KImageShopDoc* m_pDoc; 
  QPixmap m_pixmap;
};

#endif
