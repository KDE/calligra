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

#ifndef wudoc_h
#define wudoc_h

#include "WUView.h"

#include <KWDocument.h>


/******************************************************************/
/* Class: WUDocument                                           */
/******************************************************************/


class WUDocument : public KWDocument
{
    Q_OBJECT

public:

    WUDocument( QWidget *parentWidget = 0, QObject* parent = 0, bool singleViewMode = false );
    ~WUDocument();

    virtual bool initDoc(InitDocFlags flags, QWidget* parentWidget);

public slots:


protected slots:


protected:
    /// Reimplemented from KWDocument
    KoView* createViewInstance( QWidget* parent, const char* name );


private:
};


#endif
