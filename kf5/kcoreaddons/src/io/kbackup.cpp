/* kate: tab-indents off; replace-tabs on; tab-width 4; remove-trailing-space on; encoding utf-8;*/
/*
  This file is part of the KDE libraries
  Copyright 1999 Waldo Bastian <bastian@kde.org>
  Copyright 2006 Allen Winter <winter@kde.org>
  Copyright 2006 Gregory S. Hayes <syncomm@kde.org>
  Copyright 2006 Jaison Lee <lee.jaison@gmail.com>
  Copyright 2011 Romain Perier <bambi@ubuntu.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License version 2 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QProcess>

#include <qstandardpaths.h>

#include "kbackup.h"

namespace KBackup
{

bool backupFile( const QString& qFilename, const QString& backupDir )
{
    // get backup type from config, by default use "simple"
    // get extension from config, by default use "~"
    // get max number of backups from config, by default set to 10
#pragma message("KDE5 TODO: Remove KConfig correctly")
#if 0
    KConfigGroup g(KSharedConfig::openConfig(), "Backups"); // look in the Backups section
    QString type = g.readEntry( "Type", "simple" );
    QString extension = g.readEntry( "Extension", "~" );
    QString message = g.readEntry( "Message", "Automated KDE Commit" );
    int maxnum = g.readEntry( "MaxBackups", 10 );
    if ( type.toLower() == QLatin1String("numbered") ) {
        return( numberedBackupFile( qFilename, backupDir, extension, maxnum ) );
    } else if ( type.toLower() == QLatin1String("rcs") ) {
        return( rcsBackupFile( qFilename, backupDir, message ) );
    } else {
        return( simpleBackupFile( qFilename, backupDir, extension ) );
    }
#endif
    return( simpleBackupFile( qFilename, backupDir, QLatin1String("~") ) );
}

bool simpleBackupFile( const QString& qFilename,
                                  const QString& backupDir,
                                  const QString& backupExtension )
{
    QString backupFileName = qFilename + backupExtension;

    if ( !backupDir.isEmpty() ) {
        QFileInfo fileInfo ( qFilename );
        backupFileName = backupDir + QLatin1Char('/') + fileInfo.fileName() + backupExtension;
    }

//    qDebug() << "KBackup copying " << qFilename << " to " << backupFileName;
    QFile::remove(backupFileName);
    return QFile::copy(qFilename, backupFileName);
}

bool rcsBackupFile( const QString& qFilename,
                               const QString& backupDir,
                               const QString& backupMessage )
{
    QFileInfo fileInfo ( qFilename );

    QString qBackupFilename;
    if ( backupDir.isEmpty() ) {
        qBackupFilename = qFilename;
    } else {
        qBackupFilename = backupDir + fileInfo.fileName();
    }
    qBackupFilename += QString::fromLatin1( ",v" );

    // If backupDir is specified, copy qFilename to the
    // backupDir and perform the commit there, unlinking
    // backupDir/qFilename when finished.
    if ( !backupDir.isEmpty() )
    {
        if ( !QFile::copy(qFilename, backupDir + fileInfo.fileName()) ) {
            return false;
        }
        fileInfo.setFile(backupDir + QLatin1Char('/') + fileInfo.fileName());
    }

    const QString cipath = QStandardPaths::findExecutable(QString::fromLatin1("ci"));
    const QString copath = QStandardPaths::findExecutable(QString::fromLatin1("co"));
    const QString rcspath = QStandardPaths::findExecutable(QString::fromLatin1("rcs"));
    if ( cipath.isEmpty() || copath.isEmpty() || rcspath.isEmpty() )
        return false;

    // Check in the file unlocked with 'ci'
    QProcess ci;
    if ( !backupDir.isEmpty() )
        ci.setWorkingDirectory( backupDir );
    ci.start( cipath, QStringList() << QString::fromLatin1("-u") << fileInfo.filePath() );
    if ( !ci.waitForStarted() )
        return false;
    ci.write( backupMessage.toLatin1() );
    ci.write(".");
    ci.closeWriteChannel();
    if( !ci.waitForFinished() )
        return false;

    // Use 'rcs' to unset strict locking
    QProcess rcs;
    if ( !backupDir.isEmpty() )
        rcs.setWorkingDirectory( backupDir );
    rcs.start( rcspath, QStringList() << QString::fromLatin1("-U") << qBackupFilename );
    if ( !rcs.waitForFinished() )
        return false;

    // Use 'co' to checkout the current revision and restore permissions
    QProcess co;
    if ( !backupDir.isEmpty() )
        co.setWorkingDirectory( backupDir );
    co.start( copath, QStringList() << qBackupFilename );
    if ( !co.waitForFinished() )
        return false;

    if ( !backupDir.isEmpty() ) {
        return QFile::remove( fileInfo.filePath() );
    } else {
        return true;
    }
}

bool numberedBackupFile( const QString& qFilename,
                                    const QString& backupDir,
                                    const QString& backupExtension,
                                    const uint maxBackups )
{
    QFileInfo fileInfo ( qFilename );

    // The backup file name template.
    QString sTemplate;
    if ( backupDir.isEmpty() ) {
        sTemplate = qFilename + QLatin1String(".%1") + backupExtension;
    } else {
        sTemplate = backupDir + QLatin1Char('/') + fileInfo.fileName() + QLatin1String(".%1") + backupExtension;
    }

    // First, search backupDir for numbered backup files to remove.
    // Remove all with number 'maxBackups' and greater.
    QDir d = backupDir.isEmpty() ? fileInfo.dir() : backupDir;
    d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    const QStringList nameFilters = QStringList( fileInfo.fileName() + QLatin1String(".*") + backupExtension );
    d.setNameFilters( nameFilters );
    d.setSorting( QDir::Name );

    uint maxBackupFound = 0;
    Q_FOREACH ( const QFileInfo &fi, d.entryInfoList() ) {
        if ( fi.fileName().endsWith( backupExtension ) ) {
            // sTemp holds the file name, without the ending backupExtension
            QString sTemp = fi.fileName();
            sTemp.truncate( fi.fileName().length()-backupExtension.length() );
            // compute the backup number
            int idex = sTemp.lastIndexOf( QLatin1Char('.') );
            if ( idex > 0 ) {
                bool ok;
                uint num = sTemp.mid( idex+1 ).toUInt( &ok );
                if ( ok ) {
                    if ( num >= maxBackups ) {
                        QFile::remove( fi.filePath() );
                    } else {
                        maxBackupFound = qMax( maxBackupFound, num );
                    }
                }
            }
        }
    }

    // Next, rename max-1 to max, max-2 to max-1, etc.
    QString to=sTemplate.arg( maxBackupFound+1 );
    for ( int i=maxBackupFound; i>0; i-- ) {
        QString from = sTemplate.arg( i );
//        qDebug() << "KBackup renaming " << from << " to " << to;
        QFile::rename( from, to );
        to = from;
    }

    // Finally create most recent backup by copying the file to backup number 1.
//    qDebug() << "KBackup copying " << qFilename << " to " << sTemplate.arg(1);
    return QFile::copy(qFilename, sTemplate.arg(1));
}

  
}
