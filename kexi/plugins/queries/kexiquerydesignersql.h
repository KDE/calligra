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
#include <kexiquerypart.h>

class KexiQueryDesignerSQLEditor;
class KexiQueryDesignerSQLViewPrivate;

//! The KexiQueryDesignerSQLView class for editing Queries in text mode.
/*! It is a view containing SQL text editor 
 and SQL history/status widget splitted vertically. 
 Depending on user's will, the widget can be in "sql history" 
 mode or in "sql status" mode. */
class KEXI_HAND_QUERY_EXPORT KexiQueryDesignerSQLView : public KexiViewBase
{
	Q_OBJECT

	public:
		KexiQueryDesignerSQLView(KexiMainWindow *mainWin, QWidget *parent, const char *name = 0);
		virtual ~KexiQueryDesignerSQLView();

		QString sqlText() const;
		KexiQueryDesignerSQLEditor *editor() const;

		virtual bool eventFilter ( QObject *o, QEvent *e );

	protected:
		KexiQueryPart::TempData * tempData() const;

		virtual bool beforeSwitchTo(int mode, bool &cancelled, bool &dontStore);
		virtual bool afterSwitchFrom(int mode, bool &cancelled);
		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);

		void setStatusOk();
		void setStatusError(const QString& msg);
		void setStatusEmpty();
		void setStatusText(const QString& text);

		virtual void updateActions(bool activated);

	protected slots:
		/*! Performs query checking (by text parsing). \return true and sets d->parsedQuery 
		 to the new query schema object on success. */
		bool slotCheckQuery();
		void slotUpdateMode();
		void slotTextChanged();
//		void slotHistoryHeaderButtonClicked(const QString& buttonIdentifier);
		void slotSelectQuery();

	signals:
		void queryShortcut();

	private:
		KexiQueryDesignerSQLViewPrivate *d;
};

#endif
