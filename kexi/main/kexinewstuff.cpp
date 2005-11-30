/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#ifdef HAVE_KNEWSTUFF

#include <kdebug.h>
#include <ktar.h>
#include <qdir.h>
#include <kaction.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kdeversion.h>

#include "kexinewstuff.h"

KexiNewStuff::KexiNewStuff(QWidget *parent)
 : KNewStuff( "kexi/template"
#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)
	, "http://download.kde.org/khotnewstuff/kexi-providers.xml"
#endif
	, parent)
{
}

KexiNewStuff::~KexiNewStuff()
{
}

bool
KexiNewStuff::install(const QString &fileName)
{
	kdDebug() << "KexiNewStuff::install(): " << fileName << endl;

	KTar archive( fileName );
	if ( !archive.open( IO_ReadOnly ) )
		return false;
	const KArchiveDirectory *archiveDir = archive.directory();
	const QString destDir = KFileDialog::getExistingDirectory(
		":downloadExampleDatabases", parentWidget(),
		i18n("Choose Directory Where to Install Example Database"));
	if (destDir.isEmpty())
		return false;
	archiveDir->copyTo(destDir);
	archive.close();

	return true;
}

bool
KexiNewStuff::createUploadFile(const QString &)
{
	return true;
}

#endif
