/* Swinder - Portable library for spreadsheet
   SPDX-FileCopyrightText: 2009, 2010 Sebastian Sauer <sebsauer@kdab.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SWINDER_OBJECTS_H
#define SWINDER_OBJECTS_H

#include <Charting.h>

#include "cell.h"
#include "excel.h"
#include "format.h"
#include "formulas.h"
#include "records.h"
#include "sheet.h"
#include "value.h"
#include "workbook.h"

#include <iostream>
#include <string>
#include <vector>

#include <QList>
#include <QRect>
#include <QString>

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
    Type type() const
    {
        return m_type;
    }
    /// Returns the unique object identifier.
    unsigned long id() const
    {
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
 * Note objects used to store comments attached to a cell or revision.
 */
class NoteObject : public Object
{
public:
    NoteObject(unsigned long id)
        : Object(Note, id)
    {
    }
    ~NoteObject() override = default;
    /// Returns the comment.
    QString note() const
    {
        return m_note;
    }
    /// Set the comment.
    void setNote(const QString &n)
    {
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
    std::unique_ptr<KoChart::Chart> m_chart;

    explicit ChartObject(unsigned long id)
        : Object(Chart, id)
        , m_chart(std::make_unique<KoChart::Chart>())
    {
    }
    ~ChartObject() override = default;

    bool operator==(const ChartObject &other) const
    {
        return this == &other;
    }
    bool operator!=(const ChartObject &other) const
    {
        return !(*this == other);
    }

    ChartObject(const ChartObject &co) = delete;
    ChartObject &operator=(const ChartObject &co) = delete;
};

/**
 * A OfficeArt object.
 */
class OfficeArtObject
{
public:
    explicit OfficeArtObject(const MSO::OfficeArtSpContainer &object, quint32 index = 0);
    ~OfficeArtObject();
    MSO::OfficeArtSpContainer object() const;
    void setText(const TxORecord &text);
    TxORecord text() const;
    void setIndex(quint32 index);
    quint32 index() const;
    bool operator==(const OfficeArtObject &other) const
    {
        return this == &other;
    }
    bool operator!=(const OfficeArtObject &other) const
    {
        return !(*this == other);
    }

private:
    MSO::OfficeArtSpContainer m_object;
    TxORecord m_text;
    quint32 m_index;
};

} // namespace Swinder

#endif // SWINDER_OBJECTS_H
