/* Swinder - Portable library for spreadsheet
   Copyright (C) 2009,2010 Sebastian Sauer <sebsauer@kdab.com>

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

#include <Charting.h>

#include "formulas.h"
#include "records.h"
#include "format.h"
#include "value.h"
#include "cell.h"
#include "sheet.h"
#include "workbook.h"

#include <string>
#include <iostream>
#include <vector>

#include <QtCore/QList>
#include <QtCore/QRect>
#include <QtCore/QString>

namespace MSO
{
    class OfficeArtDgContainer;
}

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
        OfficeArt = 0x001E
    };

    Object(Type t, unsigned long id);
    virtual ~Object();

    /// Returns the object type.
    Type type() const {
        return m_type;
    }
    /// Returns the unique object identifier.
    unsigned long id() const {
        return m_id;
    }

    /// Location of the object.
    unsigned long m_colL, m_dxL, m_rwT, m_dyT, m_colR, m_dxR, m_rwB, m_dyB;

    /// Apply the drawing-container to the object.
    virtual bool applyDrawing(const MSO::OfficeArtDgContainer &container);

protected:
    const Type m_type;
    unsigned long m_id;
};

/**
 * Picture objects are used to store bitmap, enhanced metafiles or
 * other kind of images.
 */
class PictureObject : public Object
{
public:
    PictureObject(unsigned long id) : Object(Picture, id), m_offset(0), m_size(0) {}
    virtual ~PictureObject() {}

    /// Enumeration of possible image types.
    enum Type { EnhancedMetafile, Bitmap, Unspecified };
    /// Returns the type of the image.
    Type type() const {
        return m_type;
    }
    /// Sets the type of the image.
    void setType(const Type &t) {
        m_type = t;
    }

    /// Returns the offset of the picture in the control stream.
    uint controlStreamOffset() {
        return m_offset;
    }
    /// Returns the size of the picture in the control stream.
    uint controlStreamSize() {
        return m_size;
    }
    /// Sets the offset and size of the picture in the control stream.
    void setControlStream(uint offset, uint size) {
        m_offset = offset;
        m_size = size;
    }

    /**
     * Returns the filename of the embedded storage. This can be empty if the
     * picture is not located in an embedded but e.g. in a control stream.
     * If not empty then thís is usually a concatenation of "MBD" and a eight
     * byte hexadecimal representation.
     */
    std::string embeddedStorage() const {
        return m_storage;
    }
    /// Set the filename of the embedded storage.
    void setEmbeddedStorage(const std::string &filename) {
        m_storage = filename;
    }

    /**
     * Returns the path and filename of the picture-file in the KoStore.
     * This can be something like "Pictures/TheUniqueIdentifierOfThePicture.jpg" for example.
     */
    QString fileName() const {
        return m_filename;
    }
    void setFileName(const QString& name) {
        m_filename = name;
    }
    
private:
    Type m_type;
    uint m_offset, m_size;
    std::string m_storage;
    QString m_filename;
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
    QString note() const {
        return m_note;
    }
    /// Set the comment.
    void setNote(const QString &n) {
        m_note = n;
    }
private:
    QString m_note;
};

/**
 * A charting object.
 */
class ChartObject : public Object
{
public:
    Charting::Chart* m_chart;

    explicit ChartObject(unsigned long id) : Object(Chart, id), m_chart(new Charting::Chart) {}
    virtual ~ChartObject() { delete m_chart; }
    bool operator==(const ChartObject &other) const { return this == &other; }
    bool operator!=(const ChartObject &other) const { return ! (*this == other); }
};

/**
 * A OfficeArt object.
 */
class OfficeArtObject
{
public:
    explicit OfficeArtObject(const MSO::OfficeArtSpContainer& object);
    ~OfficeArtObject();
    MSO::OfficeArtSpContainer object() const;
    void setText(const QString& text);
    QString text() const;
    bool operator==(const OfficeArtObject& other) const { return this == &other; }
    bool operator!=(const OfficeArtObject& other) const { return !(*this == other); }
private:
    MSO::OfficeArtSpContainer m_object;
    QString m_text;
};

} // namespace Swinder

#endif // SWINDER_OBJECTS_H
