/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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


#include <kdebug.h>
#include <kgenericfactory.h>

#include <keximainwindow.h>
#include <kexiproject.h>

#include "kexiquerydesigner.h"
#include "kexiquerypart.h"

KexiQueryPart::KexiQueryPart(QObject *parent, const char *name, const QStringList &l)
 : KexiPart::Part(parent, name, l)
{
	m_names["instance"] = i18n("Query");
}

KexiQueryPart::~KexiQueryPart()
{
}

KexiDialogBase*
KexiQueryPart::createInstance(KexiMainWindow *win, const KexiPart::Item &item, bool)
{
	KexiQueryDesigner *d = new KexiQueryDesigner(win, item);
	return d;
}

/*QString
KexiQueryPart::instanceName() const
{
	return i18n("Query");
}*/

bool KexiQueryPart::remove(KexiMainWindow *, const KexiPart::Item &)
{
	//TODO
	return false;
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_query, KGenericFactory<KexiQueryPart> )


#include "kexiquerypart.moc"

