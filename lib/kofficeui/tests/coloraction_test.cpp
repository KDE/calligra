/* This file is part of the KDE project
   Copyright (C) 1999 by Dirk A. Mueller <dmuell@gmx.net>
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>

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

#include <qlayout.h>
#include <qvgroupbox.h>
#include <qpopupmenu.h>
#include <qmenubar.h>

#include <kdebug.h>
#include <kapplication.h>
#include <kotoolbutton.h>
#include <coloraction_test.h>


TopLevel::TopLevel( QWidget* parent, const char* name) : QMainWindow( parent, name )
{
    setCaption( QString::fromLatin1( "KColorAction test application" ) );

    QWidget *w = new QWidget( this );
    setCentralWidget( w );

    QBoxLayout* l = new QHBoxLayout( w, 10 );
    QGroupBox* b1 = new QVGroupBox( QString::fromLatin1( "KoColorPanel 1" ), w );
    panel = new KoColorPanel( b1, "panel1" );
    panel->insertDefaultColors();
    l->addWidget( b1 );

    b1 = new QVGroupBox( QString::fromLatin1( "KoColorPanel 2" ), w );

    ( void ) new KoColorPanel( b1, "panel2" );
    l->addWidget( b1 );

    QPopupMenu* file = new QPopupMenu( this );
    menuBar()->insertItem( "&File", file );

    QPopupMenu* sub = new QPopupMenu( this );
    KoColorPanel* p = new KoColorPanel( sub );
    p->insertDefaultColors();
    sub->insertItem( p );
    sub->insertSeparator();
    sub->insertItem( new KoColorPanel( sub ) );
    sub->insertSeparator();
    sub->insertItem( "Test", this, SLOT( test() ), CTRL+Key_T );

    file->insertItem( "&Foo", sub );
    file->insertItem( "&Quit", qApp, SLOT( closeAllWindows() ), CTRL+Key_Q );

}

void TopLevel::test()
{
    kdDebug() << "test" << endl;
    //panel->insertDefaultColors();
    panel->insertColor( qRgb( 20, 20, 20 ) );
}

int main( int argc, char ** argv )
{
    KApplication *a = new KApplication ( argc, argv, "KColorAction Test" );
    TopLevel *toplevel = new TopLevel( 0, "coloractiontest" );
    a->setMainWidget( toplevel );
    toplevel->show();
    a->exec();
}

#include <coloraction_test.moc>
