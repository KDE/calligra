/* Swinder - Portable library for spreadsheet
   Copyright (C) 2009 Sebastian Sauer <sebsauer@kdab.com>

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
   Boston, MA 02110-1301, USA
*/

#ifndef SWINDER_OBJECTS_H
#define SWINDER_OBJECTS_H

#include "ustring.h"

namespace Swinder
{

/**
 * Base class for all kind of objects.
 */
class Object
{
  public:

    /// The object type.
    enum Type {
      Group = 0x0000,
      Line = 0x0001,
      Rectangle = 0x0002,
      Oval = 0x0003,
      Arc = 0x0004,
      Chart = 0x0005,
      Text = 0x0006,
      Button = 0x0007,
      Picture = 0x0008,
      Polygon = 0x0009,
      Checkbox = 0x000B,
      RadioButton = 0x000C,
      EditBox = 0x000D,
      Label = 0x000E,
      DialogBox = 0x000F,
      SpinControl = 0x0010,
      Scrollbar = 0x0011,
      List = 0x0012,
      GroupBox = 0x0013,
      DropdownList = 0x0014,
      Note = 0x0019, ///< \a NoteObject
      OfficeArtObject = 0x001E
    };
    
    Object(Type t, unsigned long id): m_type(t), m_id(id) {}
    virtual ~Object() {}

    /// Returns the object type.
    Type type() const { return m_type; }
    /// Returns the unique object identifier.
    unsigned long id() const { return m_id; }

  private:
    const Type m_type;
    unsigned long m_id;
};

/**
 * Note objects used to store comments attached to a cell or revision.
 */
class NoteObject : public Object
{
  public:
    NoteObject(unsigned long id) : Object(Note, id) {}
    virtual ~NoteObject() {}
    /// Returns the comment.
    UString note() const { return m_note; }
    /// Set the comment.
    void setNote(const UString &n) { m_note = n; }
  private:
    UString m_note;
};

} // namespace Swinder

#endif // SWINDER_OBJECTS_H
