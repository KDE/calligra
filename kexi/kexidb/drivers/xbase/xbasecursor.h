/* This file is part of the KDE project
   Copyright (C) 2008 Sharan Rao <sharanrao@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _XBASECURSOR_H_
#define _XBASECURSOR_H_

#include <kexidb/cursor.h>
#include <kexidb/connection.h>

namespace KexiDB {

class xBaseCursorData;

class xBaseCursor: public Cursor {
  public:
        	xBaseCursor(Connection* conn, Cursor* internalCursor, const QString& statement = QString(), 
      uint cursor_options = NoOptions );
    xBaseCursor(Connection* conn, Cursor* internalCursor, QuerySchema& query, uint options = 	NoOptions );
    virtual ~xBaseCursor();

    virtual bool drv_open();
    virtual bool drv_close();
    virtual void drv_getNextRecord();
    virtual QVariant value(uint);

    virtual void drv_clearServerResult();
    virtual void drv_appendCurrentRecordToBuffer();
    virtual void drv_bufferMovePointerNext();
    virtual void drv_bufferMovePointerPrev();
    virtual void drv_bufferMovePointerTo(qint64 to);
    virtual const char** rowData() const;
    virtual bool drv_storeCurrentRow(RecordData &data) const;
    
    virtual int serverResult();
    virtual QString serverResultName();
    virtual QString serverErrorMsg();

  protected:
    xBaseCursorData *d;
  
  private:
    void init();


};

}

#endif
