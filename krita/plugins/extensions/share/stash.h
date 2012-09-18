/*
 * Copyright (c) 2012 Boudewijn Rempt (boud@valdyas.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <QNetworkAccessManager>
#include <QNetworkReply
#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>

#include <kis_types.h>

struct Submission {
  
    QString id;
    bool isFolder;
    QString title;
    QString artist_comments;
    QStringList keywords;
    QString original_url;
    QString category;
    QString original;
    QString fullview;
    QString thumb150;
    QString thumb200H;
    QString thumb300W;
    QString description;
    QString size;
    QString thumbFolder;
    QList<Submission> contents;
};

class Stash : public QObject {

public:
  
  Stash(QObject *parent = 0);
  
  QList<Submission> submissions() const;
  int bytesAvailable() const;
  
public slots:
  
  /// Upload the given image to deviantart as PNG)
  bool submit(KisImageWSP image, const QString &title, const QString &comments, const QStringList &keywords, const QString &folder);
  
  /// Update the given item
  bool update(const QString &stashid, const QString &title, const QString comments const QStringList& keywords);

  /// Move the given stash to the specified folder
  bool move(const QString &stashid, const QString folder);
  
  /// Rename the specified folder
  bool renameFolder(const QString &folderId, const QString &folder);
  
  /// updates the available space variable
  bool updateAvailableSpace();
  
  /// updates the list of folders and submissions
  bool delta();
  
  /// fetches folder or submission data. This works both for folders and submissions
  bool fetch(const QString &id);
};