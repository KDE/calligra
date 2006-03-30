/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KexiDBTextWidgetInterface_H
#define KexiDBTextWidgetInterface_H

#include <widget/utils/kexidisplayutils.h>
//Added by qt3to4:
#include <Q3Frame>
#include <QEvent>

namespace KexiDB {
	class QueryColumnInfo;
}
class Q3Frame;

//! Interface for a few text editor's features
class KEXIFORMUTILS_EXPORT KexiDBTextWidgetInterface
{
	public:
		KexiDBTextWidgetInterface()
		 : m_autonumberDisplayParameters(0)
		{
		}
		~KexiDBTextWidgetInterface() {
			delete m_autonumberDisplayParameters;
		}
		void setColumnInfo(KexiDB::QueryColumnInfo* cinfo, QWidget *w);
		void paintEvent( Q3Frame *w, bool textIsEmpty, int alignment, bool hasFocus );
		void event( QEvent * e, QWidget *w, bool textIsEmpty );

		//! parameters for displaying autonumber sign
		KexiDisplayUtils::DisplayParameters *m_autonumberDisplayParameters;
};

#endif
