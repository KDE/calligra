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

#ifndef __ko_view_h__
#define __ko_view_h__

#include <koffice.h>
#include <opView.h>

class KoDocument;

class KoViewIf : virtual public OPViewIf,
		 virtual public KOffice::View_skel
{
public:
  KoViewIf( KoDocument* _doc );
  ~KoViewIf();
  
  // IDL
  virtual KOffice::View::Mode mode();
  virtual void setMode( KOffice::View::Mode _mode );

  // IDL
  virtual CORBA::Boolean isMarked();
  virtual void setMarked( CORBA::Boolean _marked );

  // IDL
  virtual CORBA::Boolean printDlg();
  virtual void setMainWindow( OpenParts::MainWindow_ptr _main );
  
  // IDL
  void KoViewIf::setFocus( CORBA::Boolean _focus );

  // IDL
  virtual CORBA::ULong leftGUISize() { return 0; }
  virtual CORBA::ULong rightGUISize() { return 0; }
  virtual CORBA::ULong topGUISize() { return 0; }
  virtual CORBA::ULong bottomGUISize() { return 0; }
  
protected:
  CORBA::Boolean m_bMarked;
  
  KOffice::View::Mode m_eMode;
  KOffice::MainWindow_var m_vKoMainWindow;
};

#endif
