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

#ifndef __kimageshop_gui_h__
#define __kimageshop_gui_h__

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
