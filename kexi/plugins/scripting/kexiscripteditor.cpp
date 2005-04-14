/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include "kexiscripteditor.h"

#include <kdebug.h>

#include <kexidialogbase.h>
#include <kexidb/connection.h>

#ifdef KTEXTEDIT_BASED_SQL_EDITOR
#else
# include <ktexteditor/highlightinginterface.h>
#endif

KexiScriptEditor::KexiScriptEditor(KexiMainWindow *mainWin, QWidget *parent, const char *name)
 : KexiEditor(mainWin, parent, name)
{
	// Load script code
	QString data;
	loadDataBlock(data);
	setText(data);

	// TEMP: let's use python highlighting for now
	KTextEditor::HighlightingInterface *hl = KTextEditor::highlightingInterface(document());
	for(uint i=0; i < hl->hlModeCount(); i++)  {
//		kdDebug() << "hlmode("<<i<<"): " << hl->hlModeName(i) << endl;
		if (hl->hlModeName(i).contains("python", false))  {
			hl->setHlMode(i);
			break;
		}
	}

	/// \todo plug actions here
	connect(this, SIGNAL(textChanged()), this, SLOT(slotDirty()));
}

KexiDB::SchemaData*
KexiScriptEditor::storeNewData(const KexiDB::SchemaData& sdata, bool &cancel)
{
	KexiDB::SchemaData *s = KexiViewBase::storeNewData(sdata, cancel);
	kexipluginsdbg << "KexiScriptEditor::storeNewData(): new id:" << s->id() << endl;

	if (!s || cancel) {
		delete s;
		return 0;
	}
	if (!storeData()) {
		//failure: remove object's schema data to avoid garbage
		KexiDB::Connection *conn = parentDialog()->mainWin()->project()->dbConnection();
		conn->removeObject( s->id() );
		delete s;
		return 0;
	}
	return s;
}

tristate
KexiScriptEditor::storeData()
{
	kexipluginsdbg << "KexiScriptEditor::storeData(): " << parentDialog()->partItem()->name() << " [" << parentDialog()->id() << "]" << endl;
	return storeDataBlock(text());
}

void
KexiScriptEditor::slotDirty()
{
	KexiViewBase::setDirty(true);
}


#include "kexiscripteditor.moc"

