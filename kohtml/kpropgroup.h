// kpropgroup.h


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


#if !defined KPROPGROUP_H
#define KPROPGROUP_H

#include <qlayout.h>
#include <qgroupbox.h>
#include <qlayout.h>

/** This class provides a box to hold (kprop)widgets.
 * You can add widgets which will be added to layout controll.
 */
class KPropGroup : public QGroupBox
{
	Q_OBJECT
	
public:
	/** Contructor of the box. The 'text' is the title of the box. */ 
	KPropGroup( QWidget *parent=0, const char *title="", int orientation=0, int border=-1, int autoborder=-1, const char *name=0 );

	/** Destructor. */
	virtual ~KPropGroup();
	
	/** Add a widget to the box. */
	void addWidget( QWidget *widget );

protected:
	/** If the font changes for this widget, this method is called.	  */ 
	virtual void fontChange( const QFont &oldfont );

	QVBoxLayout *layout;
	
};

#endif //KPROPGROUP_H

