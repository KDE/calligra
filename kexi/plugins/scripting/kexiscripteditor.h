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

#ifndef KEXISCRIPTEDITOR_H
#define KEXISCRIPTEDITOR_H

#include "kexieditor.h"

/*! The KexiQueryDesignerSQLEditor class embeds text editor
 for entering query texts. */

class KexiScriptEditor : public KexiEditor
{
	Q_OBJECT

	public:
		KexiScriptEditor(KexiMainWindow *mainWin, QWidget *parent, const char *name = 0);
		virtual ~KexiScriptEditor() {;}

	protected:
		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);
		virtual tristate storeData();

	protected slots:
		void  slotDirty();

	private:
};

#endif
