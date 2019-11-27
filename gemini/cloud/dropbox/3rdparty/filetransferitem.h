/*

    Copyright 2011 Cuong Le <metacuong@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef FILETRANSFERITEM_H
#define FILETRANSFERITEM_H

#include <QObject>
#include "listmodel.h"

/*
            filename: "chieu khong em mat ho buon tenh mai lang thang.gif"
            dropbox_path "/Folder 01/Folder 02
            completed: false
            is_download : false
            progressing: 0
            in_queue : false //in queue
            is_finished : true
            date: "11/21/2011 10:11 AM"
  */

class FileTransferItem : public ListItem
{
    Q_OBJECT
    Q_PROPERTY(qint8 progressing
                 READ progressing
                 WRITE setProgressing
                 )
public:
    enum Roles {
      SizeRole = ListItem::SizeRole,
      FileNameRole = ListItem::SectionRole+1,
      Dropbox_pathRole,
      CompletedRole,
      Is_downloadRole,
      ProgressingRole,
      In_queueRole,
      Is_finishedRole,
      DateRole,
      Is_CancelledRolse
    };

    explicit FileTransferItem(QString filename, QString sizestr, QString dropbox_path, bool is_download, QObject *parent = 0):
        ListItem(parent),
        m_filename(filename),
        m_dropbox_path(dropbox_path),
        m_completed(false),
        m_is_download(is_download),
        m_progressing(0),
        m_in_queue(true),
        m_is_finished(false),
        m_date(""),
        m_is_cancelled(false),
        m_size(sizestr)
    {}
    FileTransferItem(QObject *parent = 0): ListItem(parent) {}

    QVariant data(int role) const override;
    QHash<int, QByteArray> roleNames() const;

    inline QString id() const override { return m_filename; }

    inline QString filename() const { return m_filename;}
    inline QString size() const { return m_size;}
    inline QString dropbox_path() const { return m_dropbox_path;}
    inline bool completed() const { return m_completed;}
    inline bool is_download() const { return m_is_download;}
    inline qint8 progressing() const { return m_progressing;}
    inline bool in_queue() const { return m_in_queue;}
    inline bool is_finished() const { return m_is_finished;}
    inline QString date() const { return m_date;}
    inline bool is_cancelled() const { return m_is_cancelled;}

    void setCompleted(const bool &v){m_completed = v; emit dataChanged();}
    Q_INVOKABLE void setProgressing(const qint8 &v){m_progressing = v; emit dataChanged();}
    void setIn_queue(const bool &v){m_in_queue = v; emit dataChanged();}
    void setIs_finished(const bool &v){m_is_finished = v; emit dataChanged();}
    void setDatetime(const QString &v){m_date=v;emit dataChanged();}
    void setCancelled(const bool &v){m_is_cancelled=v;emit dataChanged();}


private:
    QString m_filename;
    QString m_dropbox_path;
    bool m_completed;
    bool m_is_download; // true : downloading a file | false : uploading a file
    qint8 m_progressing;
    bool m_in_queue;
    bool m_is_finished;
    QString m_date;
    bool m_is_cancelled;
    QString m_size;
};

#endif // FILETRANSFERITEM_H
