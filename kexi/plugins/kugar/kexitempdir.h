/* 
   This file is part of the KDE libraries
   Copyright (c) 2003 Joseph Wenninger <jowenn@kde.org>
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef _KEXITEMPDIR_H_
#define _KEXITEMPDIR_H_

#include <qstring.h>
#include <stdio.h>
#include <errno.h>

class QDir;
class KexiTempDirPrivate;

/**
 * The KTempDir class creates a unique directory for temporary use.
 *
 * This is especially useful if you need to create a directory in a world
 * writable directory like /tmp without being vulnerable to so called
 * symlink attacks.
 *
 * KDE applications, however, shouldn't create files or directories in /tmp in the first 
 * place but use the "tmp" resource instead. The standard KTempDir
 * constructor will do that by default.
 *
 * To create a temporary directory that starts with a certain name
 * in the "tmp" resource, one should use:
 * KTempDir(locateLocal("tmp", prefix));
 *
 * KTempFile does not create any missing directories, but locateLocal() does.
 *
 * See also @ref KStandardDirs
 *
 * @author Joseph Wenninger <jowenn@kde.org>
 */
class KexiTempDir
{
public:
   /**
    * Creates a temporary directory with the name:
    *  <directoryPrefix><six letters>
    *
    * The default @p directoryPrefix is "$KDEHOME/tmp-$HOST/appname"
    * @param directoryPrefix the prefix of the file name, or QString::null
    *        for the default value
    **/
   KexiTempDir(QString directoryPrefix=QString::null, 
             int mode = 0700 );


   /**
    * The destructor deletes the directory and it's contents if autoDelete is enabled
    **/
   ~KexiTempDir();

   /**
    * Turn automatic deletion on or off.
    * Automatic deletion is off by default.
    * @param autoDelete true to turn automatic deletion on
    **/
   void setAutoDelete(bool autoDelete) { bAutoDelete = autoDelete; }

   /**
    * Returns the status of the directory creation  based on errno. (see errno.h) 
    * 0 means OK.
    *
    * You should check the status after object creation to check 
    * whether a directory could be created in the first place.
    *
    * @return the errno status, 0 means ok
    **/
   int status() const;

   /**
    * Returns the full path and name of the directory.
    * @return The name of the file, or QString::null if creating the
    *         directory has failed or the directory has been unlinked
    **/
   QString name() const;
   
   
   /**
    * Returns the @ref QDir* of the temporary directory.
    * @return QDir directory information of the directory or 0 if their is no managed directory
    * The caller has to free the pointer open for writing to the 
    **/
   QDir *qDir();

   /**
    * Deletes the directory recursively
    **/
   void unlink();   

   /**
    * @return true if a temporary directory has successfully been created and not been unlinked yet
    */
   bool existing() const;
protected:

   bool create(const QString &directoryPrefix,  int mode);

   void setError(int error) { mError = error; }
private:
   int mError;
   QString mTmpName;
   bool bExisting;
   bool bAutoDelete;

   KexiTempDirPrivate *d;
};

#endif
