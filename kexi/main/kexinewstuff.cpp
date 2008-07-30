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
  , "http://download.kde.org/khotnewstuff/kexi-providers.xml"
  , parent)
{
  // Prevent GHNS to deny downloading a second time. If GHNS
  // fails to download something, it still marks the thing as
  // successfully downloaded and therefore we arn't able to
  // download it again :-/
  KGlobal::config()->deleteGroup("KNewStuffStatus");
}

KexiNewStuff::~KexiNewStuff()
{
}

bool
KexiNewStuff::install(const QString &fileName)
{
  kDebug() << "KexiNewStuff::install(): " << fileName << endl;

  KTar archive( fileName );
  if ( !archive.open( QIODevice::ReadOnly ) ) {
    kDebug() << QString("KexiNewStuff::install: Failed to open archivefile \"%1\"").arg(fileName) << endl;
    return false;
  }
  const KArchiveDirectory *archiveDir = archive.directory();
  const QString destDir = KFileDialog::getExistingDirectory(
    "kfiledialog:///DownloadExampleDatabases", parentWidget(),
    i18n("Choose Directory Where to Install Example Database"));
  if (destDir.isEmpty()) {
    kDebug() << QString("KexiNewStuff::install: Destination-directory is empty.") << endl;
    return false;
  }
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
