/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXI_UTILS_H
#define KEXI_UTILS_H

#include <qstring.h>
#include "kexi_global.h"

//! displays information that feature "feature_name" is not availabe in the current application version
#define KEXI_UNFINISHED(feature_name) KMessageBox::sorry(0, \
	i18n("%1 for version %2 of %3 application.") \
	.arg(QString(feature_name).isEmpty() ? i18n("This function is not available") : i18n("\"%1\" function is not available").arg(QString(feature_name).replace("&",""))) \
	.arg(KEXI_VERSION_STRING).arg(KEXI_APP_NAME) )

//! like above - for use inside KExiActionProxy subclass - reuses feature name from shared action's text
#define KEXI_UNFINISHED_SHARED_ACTION(action_name) \
	KEXI_UNFINISHED(sharedAction(action_name) ? sharedAction(action_name)->text() : QString::null)

namespace Kexi
{
	inline bool isIdentifier(const QString& s) {
		uint i;
		for (i=0; i<s.length(); i++) {
			QChar c = s.at(i).lower();
			if (!(c=='_' || c>='a' && c<='z' || i>0 && c>='0' && c<='9'))
				break;
		}
		return i>0 && i==s.length();
	}
}

#endif


