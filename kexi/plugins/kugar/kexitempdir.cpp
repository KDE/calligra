/*
 *
 *  This file is part of the KDE libraries
 *  Copyright (c) 2003 Joseph Wenninger <jowenn@kde.org>
 *
 * $Id: $
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include <config.h>

#include <sys/types.h>

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef HAVE_TEST
#include <test.h>
#endif
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#ifndef _PATH_TMP
#define _PATH_TMP "/tmp"
#endif

#include <qdatetime.h>
#include <qdir.h>

#include "kglobal.h"
#include "kapplication.h"
#include "kinstance.h"
#include "kexitempdir.h"
#include "kstandarddirs.h"
#include "kprocess.h"
#include <kdebug.h>

KexiTempDir::KexiTempDir(QString directoryPrefix, int mode)
{
   bAutoDelete = false;
   bExisting = false;
   if (directoryPrefix.isEmpty())
   {
      directoryPrefix = locateLocal("tmp", KGlobal::instance()->instanceName());
   }
   (void) create(directoryPrefix , mode);
}

bool
KexiTempDir::create(const QString &directoryPrefix, int mode)
{
   // make sure the random seed is randomized
   (void) KApplication::random();

   QCString nme = QFile::encodeName(directoryPrefix) + "XXXXXX";
   char *realName;
   if((realName=mkdtemp(nme.data())) == 0)
   {
       // Recreate it for the warning, mkdtemps emptied it
       QCString nme = QFile::encodeName(directoryPrefix) + "XXXXXX";
       qWarning("KexiTempDir: Error trying to create %s: %s", nme.data(), strerror(errno));
       mError = errno;
       mTmpName = QString::null;
       return false;
   }

   // got a return value != 0
   QCString realNameStr(realName);
   mTmpName = QFile::decodeName(realNameStr)+"/";
   kdDebug()<<"KexiTempDir: Temporary directory created :"<<realNameStr<<endl;
   kdDebug()<<"KexiTempDir: Temporary directory created :"<<mTmpName<<endl;
   mode_t tmp = 0;
   mode_t umsk = umask(tmp);
   umask(umsk);
   chmod(nme, mode&(~umsk));

   // Success!
   bExisting = true;

   // Set uid/gid (neccesary for SUID programs)
   chown(nme, getuid(), getgid());
   return true;
}

KexiTempDir::~KexiTempDir()
{
   if (bAutoDelete)
      unlink();
}

int
KexiTempDir::status() const
{
   return mError;
}

QString
KexiTempDir::name() const
{
   return mTmpName;
}

bool
KexiTempDir::existing() const
{
   return bExisting;
}

QDir *
KexiTempDir::qDir()
{
   if (bExisting) return new QDir(mTmpName);
   return 0;
}

void
KexiTempDir::unlink()
{
   if (!bExisting) return;
   QString rmstr("/bin/rm -rf ");
   rmstr += KProcess::quote(mTmpName);
   ::system( QFile::encodeName(rmstr) );

   bExisting=false;
   mError=0;
}


