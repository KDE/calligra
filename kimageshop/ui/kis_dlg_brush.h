/*
 *  brushdialog.h - part of KImageShop
 *
 *  A floating tabdialog showing a brushwidget and (later) some more properties
 *
 *  Copyright (c) 1999 Carsten Pfeiffer <pfeiffer@kde.org>
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

#ifndef BRUSHDIALOG_H
#define BRUSHDIALOG_H

#include "kfloatingtabdialog.h"

class BrushesWidget;

class BrushDialog : public KFloatingTabDialog
{
  Q_OBJECT

public:
  BrushDialog( QWidget* parent=0, const char* name=0, WFlags flags=0 );
  ~BrushDialog();
  
  const BrushesWidget *brushChooser() 	const 	{ return brushesTab; }
  
private:
  BrushesWidget *brushesTab;

};


#endif // BRUSHDIALOG_H
