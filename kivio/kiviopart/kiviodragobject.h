/* This file is part of the KDE project
   Copyright (C) 2003-2005 Peter Simonsson <psn@linux.se>

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

#ifndef KIVIODRAGOBJECT_H
#define KIVIODRAGOBJECT_H

#include <qdragobject.h>
#include <qvaluelist.h>

namespace Kivio {
class Object;
}

class KivioDragObject : public QDragObject
{
  Q_OBJECT
  public:
    KivioDragObject(QWidget* dragSource = 0, const char* name = 0);
    ~KivioDragObject();

    const char* format(int i) const;
    QByteArray encodedData(const char* mimetype) const;
    bool canDecode(QMimeSource*);
    bool decode(QMimeSource* e, QValueList<Kivio::Object*>& objectList);
    void addObject(Kivio::Object* object);

  protected:
    QByteArray kivioEncoded() const;
    QByteArray imageEncoded(const char* mimetype) const;

  private:
    enum { NumEncodeFormats = 3 };
    QCString m_encodeMimeList[NumEncodeFormats];
    QValueList<Kivio::Object*> m_objectList;
};

#endif
