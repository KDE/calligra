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


#include "WUDocument.h"


/******************************************************************/
/* Class: WUDocument                                              */
/******************************************************************/


WUDocument::WUDocument(QWidget *parentWidget, const char *widname, QObject* parent, const char* name, bool singleViewMode )
    : KWDocument( parentWidget, widname, parent, name, singleViewMode )
{
}


WUDocument::~WUDocument()
{
}


bool WUDocument::initDoc(InitDocFlags /*flags*/, QWidget* /*parentWidget*/)
{
    initEmpty();

    return true;
}


KoView* WUDocument::createViewInstance( QWidget* parent, const char* name )
{
    return new WUView( viewModeType(), parent, name, this );
}



#include "WUDocument.moc"
