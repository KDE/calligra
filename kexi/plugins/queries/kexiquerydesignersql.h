/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIQUERYDESIGNERSQL_H
#define KEXIQUERYDESIGNERSQL_H

#include <kexiviewbase.h>

class KexiQueryDesigner;
class KexiQueryDesignerSQLEditor;
class KexiQueryDesignerSQLHistory;
class KexiQueryDocument;
class KexiSectionHeader;

/*! The KexiQueryDesignerSQLView class is a view containing SQL text editor 
 and SQL history widget splitted vertically. */

class KEXI_HAND_QUERY_EXPORT KexiQueryDesignerSQLView : public KexiViewBase
{
	Q_OBJECT

	public:
		KexiQueryDesignerSQLView(KexiMainWindow *mainWin, QWidget *parent, const char *name = 0);
		~KexiQueryDesignerSQLView();

		QString getQuery();
		KexiQueryDesignerSQLEditor *editor() { return m_editor; }

	protected:
		virtual bool beforeSwitchTo(int mode, bool &cancelled, bool &dontStore);
		virtual bool afterSwitchFrom(int mode, bool &cancelled);

	signals:
		void queryShortcut();

	private:
		KexiQueryDesignerSQLEditor *m_editor;
		KexiQueryDesignerSQLHistory *m_history;
		KexiSectionHeader *m_head;
};

#endif
