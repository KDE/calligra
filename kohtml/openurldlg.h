/*

	Copyright (C) 1998 Simon Hausmann
                       <tronical@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
//
// $Id$
//
#ifndef __openurldlg_h__
#define __openurldlg_h__

#include <qdialog.h>
#include <klined.h>
#include <kurlcompletion.h>

class OpenURLDlg : public QDialog
{
  Q_OBJECT
public:
  OpenURLDlg();
  ~OpenURLDlg();

  QString url();

protected slots:
  void chooseFile();  
    
private:
   KLineEdit *urlEdit;
   KURLCompletion *completion;
};

#endif
