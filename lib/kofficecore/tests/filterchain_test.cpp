/* This file is part of the KDE project
   Copyright (C) 2001 Werner Trobin <trobin@kde.org>

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

#include <koFilterChain.h>
#include <kinstance.h>
#include <kdebug.h>

int main( int /*argc*/, char **/*argv*/ )
{
    KInstance instance( "filterchain_test" );  // we need an instance when using the trader
    KOffice::Graph g( "application/x-kword" );
    g.dump();
    KOffice::Graph h( "application/msword" );
    h.dump();
    return 0;
}
