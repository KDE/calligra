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
  virtual bool isMarked();
  virtual void setMarked( bool _marked );

  // IDL
  virtual bool printDlg();
  virtual void setMainWindow( OpenParts::MainWindow_ptr _main );

  // IDL
  void setFocus( bool _focus );

  // IDL
  virtual unsigned long int leftGUISize() { return 0; }
  virtual unsigned long int rightGUISize() { return 0; }
  virtual unsigned long int topGUISize() { return 0; }
  virtual unsigned long int bottomGUISize() { return 0; }

protected:
  bool m_bMarked;

  KOffice::View::Mode m_eMode;
  KOffice::MainWindow_var m_vKoMainWindow;
};

#endif
