
/***************************************************************

     Requires the Qt and KDE widget libraries, available at no cost at
     http://www.troll.no and http://www.kde.org respectively

     Copyright (C) 1997, 1998 Fester Zigterman ( fzr@dds.nl )

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


***************************************************************/

#include "kpropgroup.h"
#include "kpropgroup.moc"
#include <kapp.h>

KPropGroup::KPropGroup( QWidget *parent, const char *title, int orientation, int border, int autoborder, const char *name )
	: QGroupBox( title, parent,name )
{
	int _border = border?(fontMetrics().height() ):border;
	int _autoborder = autoborder?4:autoborder;
	setFontPropagation( QWidget::SameFont );
	layout = new QVBoxLayout( this, _border, _autoborder );
	
}

KPropGroup::~KPropGroup()
{}

void KPropGroup::fontChange( const QFont & )
{
	debug( "Font Change!\n");
}

void KPropGroup::addWidget( QWidget *widget )
{
	layout->addWidget( widget );
}
