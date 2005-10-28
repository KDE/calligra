/* This file is part of the KDE project
   Copyright (C) 2005 Inge Wallin <inge@lysator.liu.se>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef wuview_h
#define wuview_h

#include <KWView.h>


/******************************************************************/
/* Class: WUView                                                  */
/******************************************************************/

class WUView : public KWView
{
    Q_OBJECT

public:
    WUView( KWViewMode* viewMode, QWidget *_parent, const char *_name, KWDocument *_doc );
    virtual ~WUView();

   virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

public slots:


protected slots:


private:

};


#endif
