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

#ifndef __kformviewer_gui_h__
#define __kformviewer_gui_h__

#include <qscrollview.h>
#include <openparts_ui.h>
#include <koFrame.h>
#include <koView.h>

#include "kformviewer.h"

class KformViewerDoc;

class KformViewerView : public QScrollView,
                        virtual public KoViewIf,
                        virtual public KformViewer::View_skel
{
  Q_OBJECT

public:

  KformViewerView( QWidget* , const char*, KformViewerDoc* );
  ~KformViewerView();
  KformViewerDoc* doc();

  virtual void cleanUp();
  CORBA::Boolean printDlg();

public slots:

  void slotUpdateView();

protected:

  virtual void init();
  virtual bool event( const char* , const CORBA::Any& );
  virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr );
  virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr );
  virtual void newView();
  virtual void helpUsing();
  void resizeEvent( QResizeEvent* _event );

private:

  KformViewerDoc* m_pDoc;  
};

#endif
