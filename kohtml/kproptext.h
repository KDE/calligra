// kproptext.h


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




#if !defined KPROPTEXT_H
#define KPROPTEXT_H

#include <qlabel.h>
#include <qlined.h>
#include <kconfig.h>

/** This class provides a label with a line edit.
  * The widget is resized when the font changes.
  */
class KPropText : public QLabel
{
	Q_OBJECT
	
public:
	/** Constructor. 'text' is the text of the label.
	  * 'perc' is the percentage of space reserved for the line edit widget.
	  */
	KPropText( QWidget *parent, const char *text, int perc, const char *key=0, const char *group=0, KConfig *config=0, const char *name=0 );

	/** Destructor. deletes the line edit widget. */
	virtual ~KPropText();
	
	/** Return the line edit widget */
	QLineEdit *getLineEdit();

	/** Set the contents of the line edit widget.
	  * If no arguments are given, defaults to empty text.
	  */
	void setContents( const char * text=0 );

	/** Returns the contents of the line edit widget. */
	const char *getContents();

	/** set the group to modify */
	void setGroup( const char *group );

	/** Returns the group to be modified */
	const char *getGroup();

	/** set the key to modify */
	void setKey( const char * );

	/** returns the Key to be modified */
	const char *getKey();

	/** Set the KConfig object to operate upon. */
	void setKConfig( KConfig * );

	/** Returns the current KConfig object */
	KConfig *getKConfig();

public slots:
	/** Write the config key value. . */
	virtual void setConfig();

	/** Read the config key value into the widget. 
	  * . 
	  */
	virtual void getConfig();

protected:
	/** Handles resize events for the line edit widget. */
	virtual void resizeEvent( QResizeEvent *rev );

	/** This method is called after a font change.
	  * It will call sizeUpdate() to resize the widget to contain
	  * the new font.
	  */
	virtual void fontChange( const QFont &oldfont );

	/** Sets the minimum size to fit the font. */
	void sizeUpdate();

	QLineEdit *lineEdit;
	int percentage;
	KConfig *ConfigObject;
	QString Group, Key;
};

#endif //KPROPTEXT_H

