/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qpushbutton.h>
#include <kurlrequester.h>

#include "kexidbwidgets.h"

#include "kexiwidgetprovider.h"
#include "formeditor/widgetcontainer.h"

KexiWidgetProvider::KexiWidgetProvider()
 : KFormEditor::WidgetProvider()
{
}

QWidget*
KexiWidgetProvider::create(QString key, KFormEditor::WidgetContainer *parent, const char *name)
{
	if(key == "KexiDBLineEdit")
	{
		return new KexiDBLineEdit(parent, name);
	}
	else if(key == "KexiDBLabel")
	{
		return new KexiDBLabel(parent, name);
	}
	else if(key == "QPushButton")
	{
		return new QPushButton(parent, name);
	}
	else if(key == "KURLRequest")
	{
		return new KURLRequester(parent, name);
	}

	return 0;
}

QStringList
KexiWidgetProvider::widgets()
{
	QStringList widgets;
	widgets.append("KexiDBLineEdit");
	widgets.append("KexiDBLabel");

	widgets.append("QPushButton");
	widgets.append("KURLRequest");

	return widgets;
}

