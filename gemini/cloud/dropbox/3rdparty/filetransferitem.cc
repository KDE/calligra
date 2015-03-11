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
#include "filetransferitem.h"

QHash<int, QByteArray> FileTransferItem::roleNames() const
{
  QHash<int, QByteArray> names;
  names[FileNameRole] = "filename";
  names[SizeRole] = "size";
  names[CompletedRole] = "completed";
  names[Is_downloadRole] = "is_download";
  names[ProgressingRole] = "progressing";
  names[In_queueRole] = "in_queue";
  names[Is_finishedRole] = "is_finished";
  names[DateRole] = "date";
  names[Dropbox_pathRole] = "dropbox_path";
  names[Is_CancelledRolse] = "is_cancelled";
  return names;
}

QVariant FileTransferItem::data(int role) const
{
  switch(role) {
  case FileNameRole: return filename();break;
  case SizeRole : return size();break;
  case CompletedRole: return completed();break;
  case Is_downloadRole: return is_download();break;
  case ProgressingRole: return progressing();break;
  case In_queueRole: return in_queue();break;
  case Is_finishedRole: return is_finished();break;
  case DateRole: return date();break;
  case Dropbox_pathRole: return dropbox_path();break;
  case Is_CancelledRolse: return is_cancelled();break;
    default:
        return QVariant();
  }
}

