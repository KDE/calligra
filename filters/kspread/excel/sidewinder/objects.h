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

#include <QRect>
#include <QList>

namespace Swinder
{

class DrawingObject
{
public:
    enum Property {
        pid = 0x0104, // identifier for pictures
        itxid = 0x0080, // identifier for text
        cxk = 0x0158, // where on the shape the connection points are
        fillColor = 0x0181, // foreground color of the fill
        fillBackColor = 0x0183, // background color of the fill
        fillCrMod = 0x0185, // background color of the fill for black-white displays
        fillStyleBooleanProperties = 0x01bf, // 32-bit field of Bollean properties for the fill style
        lineColor = 0x01c0, // foreground color of the line
        lineCrMod = 0x01c3, // foreground color of the line for black-white displays
        shadowColor = 0x0201, // primary color of the shadow
        shadowCrMod = 0x0203, // primary color of the shadow for black-white displays
        shadowStyleBooleanProperties = 0x023f, // 32-bit field of Bollean properties for shadows
        groupShapeBooleanProperties = 0x03bf, // 32-bit field of Bollean properties for a shape or a group
        lineStyleBooleanProperties = 0x01ff, // 32-bit field of Bollean properties for the line style
        pictureContrast = 0x7f,
        textBooleanProperties = 0x00bf,
        wzName = 0x0380
    };

    static const char* propertyName(Property p);
    
    std::map<unsigned long,unsigned long> m_properties;
    unsigned long m_colL, m_dxL, m_rwT, m_dyT, m_colR, m_dxR, m_rwB, m_dyB;
    bool m_gotClientData; // indicates that a OfficeArtClientData was received

    explicit DrawingObject();
    virtual ~DrawingObject();
    DrawingObject(const DrawingObject& other);
    void operator=(const DrawingObject& other);

protected:
    // read a OfficeArtRecordHeader struct.
    void readHeader(const unsigned char* data, unsigned *recVer = 0, unsigned *recInstance = 0, unsigned *recType = 0, unsigned long *recLen = 0);
    // read a drawing object (container or atom) and handle/dispatch according to the recType.
    unsigned long handleObject(unsigned size, const unsigned char* data, bool* recordHandled = 0);
};

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

    Object(Type t, unsigned long id): m_type(t), m_id(id), m_drawingObject(0) {}
    virtual ~Object() { delete m_drawingObject; }

    /// Returns the object type.
    Type type() const {
        return m_type;
    }
    /// Returns the unique object identifier.
    unsigned long id() const {
        return m_id;
    }

    // Each Object can have optional a DrawingObject assigned.
    DrawingObject* drawingObject() const { return m_drawingObject; }
    void setDrawingObject(DrawingObject* drawing) { m_drawingObject = drawing; }

private:
    const Type m_type;
    unsigned long m_id;
    DrawingObject* m_drawingObject;
};

/**
 * Picture objects used to store bitmap, enhanced metafiles or other
 * kind of images.
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
private:
    Type m_type;
    uint m_offset, m_size;
    std::string m_storage;
    
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
    UString note() const {
        return m_note;
    }
    /// Set the comment.
    void setNote(const UString &n) {
        m_note = n;
    }
private:
    UString m_note;
};

class ChartObject : public Object
{
public:
    class Value
    {
    public:
        enum DataId {
            SeriesLegendOrTrendlineName = 0x00, ///< Referenced data specifies the series, legend entry, or trendline name. Error bars name MUST be empty.
            HorizontalValues = 0x01, ///< Referenced data specifies the values or horizontal values on bubble and scatter chart groups of the series and error bars.
            VerticalValues = 0x02, ///< Referenced data specifies the categories or vertical values on bubble and scatter chart groups of the series and error bars.
            BubbleSizeValues = 0x03 ///< Referenced data specifies the bubble size values of the series.
        };
        DataId dataId;

        enum DataType {
            AutoGeneratedName = 0x00, ///< The data source is a category (3) name, series name or bubble size that was automatically generated.
            TextOrValue = 0x01, ///< The data source is the text or value as specified by the formula field.
            CellRange = 0x02 ///< The data source is the value from a range of cells in a sheet specified by the formula field.
        };
        DataType type;

        bool isUnlinkedFormat; ///< false=data uses the number formatting of the referenced data, true=data uses the custom formatting specified via m_numberFormat.
        unsigned numberFormat; ///< specifies the numnber format to use for the data.
        UString formula; ///< the optional formula. could be for example "[Sheet1.$D$2:$F$2]"

        Value(DataId dataId, DataType type, bool isUnlinkedFormat, unsigned numberFormat, const UString& formula) : dataId(dataId), type(type), isUnlinkedFormat(isUnlinkedFormat), numberFormat(numberFormat), formula(formula) {}
        virtual ~Value() {}
    };

    class Format
    {
    public:
        Format() {}
        virtual ~Format() {}
    };

    class PieFormat : public Format
    {
    public:
        int pcExplode; // from PieFormat
        PieFormat(int pcExplode) : Format(), pcExplode(pcExplode) {}
    };

    class ChartImpl
    {
    public:
        ChartImpl() {}
        virtual ~ChartImpl() {}
        virtual const char* name() const = 0;
    };

    class PieImpl : public ChartImpl
    {
    public:
        int anStart;
        int pcDonut;
        PieImpl(int anStart, int pcDonut) : ChartImpl(), anStart(anStart), pcDonut(pcDonut) {}
        virtual const char* name() const { return "circle"; }
    };

    class Series
    {
    public:
        /// the type of data in categories, or horizontal values on bubble and scatter chart groups, in the series. MUST be either 0x0001=numeric or 0x0003=text.
        uint dataTypeX;
        /// the count of categories (3), or horizontal values on bubble and scatter chart groups, in the series.
        uint countXValues;
        /// the count of values, or vertical values on bubble and scatter chart groups, in the series.
        uint countYValues;
        /// the count of bubble size values in the series.
        uint countBubbleSizeValues;
        /// Range that contains the values that should be visualized by the dataSeries.
        UString valuesCellRangeAddress;
        /// The referenced values used in the chart
        QMap<Value::DataId, Value*> datasetValue;
        /// The formatting for the referenced values
        QList<Format*> datasetFormat;
    };

    /// Optional total positioning. The need to be ignored if the chart is embedded into a sheet.
    int x, y, width, height;

    /// Margins around the chart object
    int leftMargin, topMargin, rightMargin, bottomMargin;

    /// List of series
    QList<Series*> series;

    /// Range of all referenced cells.
    QRect cellRangeAddress;
    /// Range that contains the vertical values (the categories) for the plot-area.
    UString verticalCellRangeAddress;

    /// The more concrete chart implementation like e.g. a PieImpl for a pie chart.
    ChartImpl *impl;

    explicit ChartObject(unsigned long id) : Object(Chart, id), x(-1), y(-1), width(-1), height(-1), leftMargin(0), topMargin(0), rightMargin(0), bottomMargin(0), impl(0) {}
    virtual ~ChartObject() {}
    bool operator==(const ChartObject &other) const { return this == &other; }
    bool operator!=(const ChartObject &other) const { return ! (*this == other); }
};

} // namespace Swinder

#endif // SWINDER_OBJECTS_H
