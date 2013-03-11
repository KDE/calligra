#ifndef FAKE_KFILE_H
#define FAKE_KFILE_H

#include <QFileInfo>
#include <QDebug>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>

#define KDE_fstat       ::fstat
#define KDE_lseek       ::lseek
#define KDE_fseek       ::fseek
#define KDE_ftell       ::ftell
#define KDE_fgetpos     ::fgetpos
#define KDE_fsetpos     ::fsetpos
#define KDE_readdir     ::readdir
#define KDE_sendfile        ::sendfile
#define KDE_struct_stat     struct stat
#define KDE_struct_dirent   struct dirent

namespace KDE {

    static int lstat(const QString &fileName, KDE_struct_stat *stat)
    {
        qDebug() << Q_FUNC_INFO << "TODO" << fileName;
        return -1;
    }

}

#endif
 
