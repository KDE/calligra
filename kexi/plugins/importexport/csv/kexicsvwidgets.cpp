/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexicsvwidgets.h"

#include <klocale.h>

KexiCSVDelimiterComboBox::KexiCSVDelimiterComboBox( QWidget * parent )
 : KComboBox(parent, "KexiCSVDelimiterComboBox")
{
	insertItem( i18n("Comma \",\"") );
	insertItem( i18n( "Semicolon \";\"" ) );
	insertItem( i18n( "Tabulator" ) );
	insertItem( i18n( "Space \" \"" ) );
	insertItem( i18n( "Other" ) );
}

KexiCSVTextQuoteComboBox::KexiCSVTextQuoteComboBox( QWidget * parent )
 : KComboBox(parent, "KexiCSVTextQuoteComboBox")
{
	insertItem( i18n( "\"" ) );
	insertItem( i18n( "'" ) );
	insertItem( i18n( "None" ) );
}

