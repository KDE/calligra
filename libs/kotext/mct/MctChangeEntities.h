/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef MCTCHANGEENTITIES_H
#define MCTCHANGEENTITIES_H

#include "MctCell.h"
#include "MctNode.h"

#include <QMap>
#include <QTextBlockFormat>
#include <QTextCharFormat>

class MctPropertyBase;
class MctEmbObjProperties;
class MctTableProperties;

class ChangeEvent;
using ChangeEventList = QList<ChangeEvent*>;

/**
 * Change entities are used for specify all possible change type.
 * They are added to MctChange as an MctNode pointer.
 */

enum ChangeAction {
    NOCHANGE,       /// there is no change
    CHANGED,        /// something changed
    ADDED,          /// new value (which wasnt used before, in the old format)
    REMOVED         /// deleted value (new format does not contains it)
};
using PropertyMap = QMap<int, QPair<QString, ChangeAction>>;



/// Base class for Table related changes
class MctTable : public virtual MctNode , public MctCell
{
public:
    MctTable(const QString &m_cellName = "", const QString &m_tableName = "", MctCell *cellInfo = NULL);
    ~MctTable();
};

/// Base class for Paragraph related changes
class MctParagraphBreak : public virtual MctNode
{
public:
    MctParagraphBreak();
    ~MctParagraphBreak();
};

class MctParagraphBreakInTable : public MctParagraphBreak , public MctTable
{
public:
    MctParagraphBreakInTable(const QString &m_cellName = "", const QString &m_tableName = "", MctCell *cellInfo = NULL);
    ~MctParagraphBreakInTable();
};

class MctDelParagraphBreak : public virtual MctNode
{
public:
    MctDelParagraphBreak();
    ~MctDelParagraphBreak();
};

class MctDelParagraphBreakInTable : public MctDelParagraphBreak , public MctTable
{
public:
    MctDelParagraphBreakInTable(const QString &m_cellName = "", const QString &m_tableName = "", MctCell *cellInfo = NULL);
    ~MctDelParagraphBreakInTable();
};

/// Base class for String related changes
class MctStringChange : public virtual MctNode
{
public:
    MctStringChange(const QString &string = "");
    ~MctStringChange();
    QString getString() const;
    void setString(const QString &string);
private:
    QString m_string;
};

class MctStringChangeInTable : public MctStringChange , public MctTable
{
public:
    MctStringChangeInTable(const QString &string = "", const QString &cellName = "", const QString &tableName = "", MctCell *cellInfo = NULL);
    ~MctStringChangeInTable();
};

// FIXME: ::com::sun::star::beans::PropertyChangeEvent in the python legacy code is similar struct
class ChangeEvent
{
public:
    ChangeEvent(const QTextFormat &o, const QTextFormat &n);

    /**
     * getter of old type id
     *
     * @return type of old text format
     */
    int type();

    /**
     * compare old and new text format
     *
     * The different proprties will be inserted into m_changedProperties map.
     * @param type format type selector @todo refactor to use enum class instead of int
     */
    void calcDiff(int type);

    /// getter
    PropertyMap * changes();
    /// getter
    QTextFormat oldFormat();
    /// setter
    void setOldFormat(const QTextFormat &format);
    /// getter
    QTextFormat newFormat();
    /// setter
    void setNewFormat(const QTextFormat &format);

    // static functions!
    /**
     * copy properties
     *
     * @param format1 output formatting props
     * @param format2 input formatting props
     */
    static void ensureProperties(QTextFormat &format1, const QTextFormat &format2);

    /**
     *
     * @param current
     * @param base
     * @return
     * @todo understand this function
     */
    static QTextFormat getNewValuesForFormat(const QTextFormat& current, const QTextFormat &base);

    /**
     *
     * @param current
     * @param base
     * @return
     * @todo understand this function
     */
    static QTextFormat getOldValuesForFormat(const QTextFormat &current, const QTextFormat &base);

    /**
     * toString method for debugging
     * @param format text format
     */
    static void printProperties(const QTextFormat &format);

private:
    QTextFormat m_oldFormat;            ///< old text format
    QTextFormat m_newFormat;            ///< new text format
    PropertyMap * m_changedProperties;  ///< map of changed properties
};

/**
 * similar to \ref ChangeEvent
 *
 * @todo refactor this, merge with ChangeEvent for example
 */
class MctStylePropertyChange : public virtual MctNode
{
public:
    MctStylePropertyChange(ChangeEventList *changeList);
    ~MctStylePropertyChange();

    QTextFormat *getOldFormat() const;
    void setOldFormat(QTextFormat *value);
    /// getter of paragraph related style changes
    ChangeEventList * paragraphPropChanges();
    /// getter of text related style changes
    ChangeEventList * textPropChanges();
    /// getter of list related style changes
    ChangeEventList * listPropChanges();
    /// getter of other style changes
    ChangeEventList * otherPropChanges();    

private:
    // bool hasNonUnoChangeEvent();

    /**
     * sort the different change types into theire own container
     * @param propChanges style changes list
     */
    void sortPropertyChanges(ChangeEventList *propChanges);

    ChangeEventList *m_textPropChanges;       ///< QTextCharFormat : QTextFormat !
    ChangeEventList *m_paragraphPropChanges;  ///< QTextBlockFormat : QTextFormat !
    ChangeEventList *m_listPropChanges;       ///< QTextListFormat : QTextFormat !
    ChangeEventList *m_otherPropChanges;      ///< QTextFormat

    // list of complex uno services cant be constructed when graph is loaded from xml. (every item in the list contains dictionary with keys identical to ::com::sun::star::beans::PropertyChangeEvent)
    // QList< QMap<unsigned long int, PropertyChangeEvent*> > nonUnoPropertyChanges;
};

class MctAddedTextFrame : public virtual MctNode
{
public:
    MctAddedTextFrame();
    ~MctAddedTextFrame();
};

class MctRemovedTextFrame : public virtual MctNode
{
public:
    MctRemovedTextFrame();
    ~MctRemovedTextFrame();
};

/// Base of text object changes
class MctChangedTexObjectBase : public virtual MctNode
{
public:
    MctChangedTexObjectBase(const QString &name, MctPropertyBase *objectProperties = NULL);
    virtual ~MctChangedTexObjectBase();

    /// getter
    MctPropertyBase* objectProperties() const;
    /// setter
    void setObjectName(const QString &name);
    /// setter
    void setObjectProperties(MctPropertyBase* objectProperties);

protected:
    MctPropertyBase *m_objectProperties; ///< property container
};

class MctAddedTextGraphicObject : public MctChangedTexObjectBase
{
public:
    MctAddedTextGraphicObject(const QString &name, MctEmbObjProperties* embObjProps = NULL);
    ~MctAddedTextGraphicObject();
};

class MctRemovedTextGraphicObject : public MctChangedTexObjectBase
{
public:
    MctRemovedTextGraphicObject(const QString &name, MctEmbObjProperties* embObjProps = NULL);
    ~MctRemovedTextGraphicObject();
};

class MctAddedTextGraphicObjectInTable : public MctAddedTextGraphicObject , public MctTable
{
public:
    MctAddedTextGraphicObjectInTable(const QString &name, MctEmbObjProperties* embObjProps = NULL, const QString &m_cellName = "", const QString &m_tableName = "", MctCell *cellInfo = NULL);
    ~MctAddedTextGraphicObjectInTable();
};

class MctRemovedTextGraphicObjectInTable : public MctRemovedTextGraphicObject , public MctTable
{
public:
    MctRemovedTextGraphicObjectInTable(const QString &name, MctEmbObjProperties* embObjProps = NULL, const QString &m_cellName = "", const QString &m_tableName = "", MctCell *cellInfo = NULL);
    ~MctRemovedTextGraphicObjectInTable();
};

class MctAddedEmbeddedObject : public virtual MctNode
{
public:
    MctAddedEmbeddedObject(const QString &name);
    ~MctAddedEmbeddedObject();
};

class MctRemovedEmbeddedObject : public virtual MctNode
{
public:
    MctRemovedEmbeddedObject();
    ~MctRemovedEmbeddedObject();
};

/// Table related changes
class MctAddedTextTable : public MctChangedTexObjectBase
{
public:    
    MctAddedTextTable(const QString &name, MctTableProperties* tableProps = NULL);
    MctAddedTextTable(int row, int col, const QTextTableFormat &format);
    ~MctAddedTextTable();

    int rowCount(); // unused
    int colCount(); // unused
    QTextTableFormat getTableFormat();
    void setRowCount(int row);
    void setColCount(int col);
    void setTableFormat(const QTextTableFormat &format);

protected:
    int m_row;
    int m_col;
    QTextTableFormat m_tableFormat;
};

class MctRemovedTextTable : public MctChangedTexObjectBase
{
public:
    MctRemovedTextTable(const QString &name, MctTableProperties* tableProps = NULL);
    ~MctRemovedTextTable();
};

class MctAddedTextTableInTable : public MctAddedTextTable , public MctTable
{
public:
    MctAddedTextTableInTable(const QString &name, MctTableProperties* tableProps = NULL, const QString &cellName = "", const QString &tableName = "", MctCell *cellInfo = NULL);
    ~MctAddedTextTableInTable();
};

class MctRemovedTextTableInTable : public MctRemovedTextTable , public MctTable
{
public:
    MctRemovedTextTableInTable(const QString &name, MctTableProperties* tableProps = NULL, const QString &cellName = "", const QString &tableName = "", MctCell *cellInfo = NULL);
    ~MctRemovedTextTableInTable();
};

class MctStylePropertyChangeInTable : public MctStylePropertyChange , public MctTable
{
public:
    MctStylePropertyChangeInTable(ChangeEventList *propChanges, const QString &cellName = "", const QString &tableName = "", MctCell *cellInfo = NULL);
    ~MctStylePropertyChangeInTable();
};

class MctRowChangeInTable : public virtual MctNode
{
public:
    MctRowChangeInTable(int startRow, int rowCount, const QString &tableName);
    ~MctRowChangeInTable();
    int startRow() const;
    void setStartRow(int n);
    int rowCount() const;
    void setRowCount(int n);
    QString tableName() const;
    void setTableName(const QString &n);
private:
    int m_startRow;
    int m_rowCount;
    QString m_tableName;
};

class MctColChangeInTable : public virtual MctNode
{
public:
    MctColChangeInTable(int startCol, int colCount, const QString &tableName);
    ~MctColChangeInTable();   
    int startCol() const;
    void setStartCol(int n);
    int colCount() const;
    void setColCount(int n);
    QString tableName() const;
    void setTableName(const QString &n);
private:
    int m_startCol;
    int m_colCount;
    QString m_tableName;
};

class MctAddedRowInTable : public MctRowChangeInTable
{
public:
    MctAddedRowInTable(int m_startRow, int rowCount, const QString &m_tableName);
    ~MctAddedRowInTable();
};

class MctRemovedRowInTable : public MctRowChangeInTable
{
public:
    MctRemovedRowInTable(int m_startRow, int rowCount, const QString &m_tableName);
    ~MctRemovedRowInTable();
};

class MctAddedColInTable : public MctColChangeInTable
{
public:
    MctAddedColInTable(int m_startCol, int colCount, const QString &tableName);
    ~MctAddedColInTable();
};

class MctRemovedColInTable : public MctColChangeInTable
{
public:
    MctRemovedColInTable(int m_startCol, int colCount, const QString &tableName);
    ~MctRemovedColInTable();
};

#endif // MCTCHANGEENTITIES_H
