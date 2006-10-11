/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#ifndef EXAMPLE_VIEW
#define EXAMPLE_VIEW

#include <KoView.h>
//Added by qt3to4:
#include <QPaintEvent>

class KAction;
class QPaintEvent;

class ExamplePart;

class ExampleView : public KoView
{
    Q_OBJECT
public:
    ExampleView( ExamplePart* part, QWidget* parent = 0 );

protected slots:
    void cut();
    void copy();

protected:
    void paintEvent( QPaintEvent* );

    virtual void updateReadWrite( bool readwrite );

private:
    //KAction* m_cut;
};

#endif
