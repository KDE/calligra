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

#include "folderitem.h"
#include <QStringList>

QHash<int, QByteArray> FolderItem::roleNames() const
{
  QHash<int, QByteArray> names;
  names[RevisionRole] = "revision";
  names[Thumb_existsRole] = "thumb_exists";
  names[BytesRole] = "bytes";
  names[ModifiedRole] = "modified";
  names[PathRole] = "path";
  names[Is_dirRole] = "is_dir";
  names[IconRole] = "icon";
  names[Mime_typeRole] = "mime_type";
  names[SizeRole] = "size";
  names[CheckedRole] = "checked";
  names[NameRole] = "name";
  names[SectionRole] = "section";
  return names;
}

QVariant FolderItem::data(int role) const
{
  switch(role) {
  case RevisionRole: return revision();break;
  case Thumb_existsRole: return thumb_exists();break;
  case BytesRole: return bytes();break;
  case ModifiedRole: return modified();break;
  case PathRole: return path();break;
  case Is_dirRole: return is_dir();break;
  case IconRole: return icon();break;
  case Mime_typeRole: return mime_type();break;
  case SizeRole: return size();break;
  case CheckedRole: return checked();break;
  case NameRole: return name();break;
  case SectionRole: return section();break;
    default:
        return QVariant();
  }
}

QString FolderItem::name() const {
    return "";
}

QString FolderItem::section() const {
    QStringList ala(path().split("/"));
    QString lab(ala.at(ala.length()-1));
    return lab.at(0);
}

QString FolderItem::xsection() {
    return section();
}

void FolderItem::setChecked(const bool &v) {
    if( m_checked != v) {
        m_checked=v;
        emit dataChanged(); // TODO is this needed?
        emit checkedChanged();
    }
}

void FolderItem::setRevision(const qreal &v) {
    if( m_revision != v) {
        m_revision=v;
        emit revisionChanged();
    }
}

void FolderItem::setThumb_exists(const bool &v) {
    if( m_thumb_exists != v) {
        m_thumb_exists=v;
        emit thumb_existsChanged();
    }
}

void FolderItem::setBytes(const qreal &v) {
    if( m_bytes != v) {
        m_bytes=v;
        emit bytesChanged();
    }
}

void FolderItem::setModified(const QString &v) {
    if( m_modified != v) {
        m_modified=v;
        emit modifiedChanged();
    }
}

void FolderItem::setPath(const QString &v) {
    if( m_path != v) {
        m_path=v;
        emit pathChanged();
    }
}

void FolderItem::setIs_dir(const bool &v) {
    if( m_is_dir != v) {
        m_is_dir=v;
        emit is_dirChanged();
    }
}

void FolderItem::setIcon(const QString &v) {
    if( m_icon != v) {
        m_icon=v;
        emit iconChanged();
    }
}

void FolderItem::setMime_type(const QString &v) {
    if( m_mime_type != v) {
        m_mime_type=v;
        emit mime_typeChanged();
    }
}

void FolderItem::setSize(const QString &v) {
    if( m_size != v) {
        m_size=v;
        emit sizeChanged();
    }
}













