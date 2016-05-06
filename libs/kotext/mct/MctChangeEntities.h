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

enum ChangeAction {
    NOCHANGE,       /// there is no change
    CHANGED,        /// something changed
    ADDED,          /// new value (which wasnt used before, in the old format)
    REMOVED         /// deleted value (new format does not contains it)
};
using PropertyMap = QMap<int, QPair<QString, ChangeAction>>;

class MctTable : public virtual MctNode , public MctCell
{
public:
    MctTable(QString cellName = "", QString tableName = "", MctCell *cellInfo = NULL);
    ~MctTable();
};

class MctParagraphBreak : public virtual MctNode
{
public:
    MctParagraphBreak();
    ~MctParagraphBreak();
};

class MctParagraphBreakInTable : public MctParagraphBreak , public MctTable
{
public:
    MctParagraphBreakInTable(QString cellName = "", QString tableName = "", MctCell *cellInfo = NULL);
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
    MctDelParagraphBreakInTable(QString cellName = "", QString tableName = "", MctCell *cellInfo = NULL);
    ~MctDelParagraphBreakInTable();
};

class MctStringChange : public virtual MctNode
{
public:
    MctStringChange(QString string = "");
    ~MctStringChange();
    QString getString() const;
    void setString(QString string);
private:
    QString string;
};

class MctStringChangeInTable : public MctStringChange , public MctTable
{
public:
    MctStringChangeInTable(QString string = "", QString cellName = "", QString tableName = "", MctCell *cellInfo = NULL);
    ~MctStringChangeInTable();
};

// FIXME: ::com::sun::star::beans::PropertyChangeEvent in the python legacy code is similar struct
class ChangeEvent
{
public:
    ChangeEvent(QTextFormat o, QTextFormat n);
    PropertyMap * getChanges();    // getter
    int getType();
    void calcDiff(int type);

    QTextFormat getOldFormat();
    QTextFormat getNewFormat();

    void setOldFormat(QTextFormat format);
    void setNewFormat(QTextFormat format);

    static void ensureProperties(QTextFormat &format1, QTextFormat format2);
    static QTextFormat getNewValuesForFormat(QTextFormat current, QTextFormat base);
    static QTextFormat getOldValuesForFormat(QTextFormat current, QTextFormat base);
    static void printProperties(QTextFormat format);

private:

    QTextFormat oldFormat;
    QTextFormat newFormat;
    PropertyMap * changedProperties;
};

class MctStylePropertyChange : public virtual MctNode
{
public:
    MctStylePropertyChange(ChangeEventList *changeList);
    ~MctStylePropertyChange();

    QTextFormat *getOldFormat() const;
    void setOldFormat(QTextFormat *value);

    ChangeEventList * getParagraphPropChanges();
    ChangeEventList * getTextPropChanges();
    ChangeEventList * getListPropChanges();
    ChangeEventList * getOtherPropChanges();
    // TODO: setters?

private:
    // bool hasNonUnoChangeEvent();

    void sortPropertyChanges(ChangeEventList *propChanges);

    ChangeEventList *textPropChanges;       // QTextCharFormat : QTextFormat !
    ChangeEventList *paragraphPropChanges;  // QTextBlockFormat : QTextFormat !
    ChangeEventList *listPropChanges;       // QTextListFormat : QTextFormat !
    ChangeEventList *otherPropChanges;      // QTextFormat



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

// Általános szövegmódosítások ősosztálya
class MctChangedTexObjectBase : public virtual MctNode
{
public:
    MctChangedTexObjectBase(QString name, MctPropertyBase *objectProperties = NULL);
    virtual ~MctChangedTexObjectBase();

    void setObjectName(QString name);
    MctPropertyBase* getObjectProperties() const;
    void setObjectProperties(MctPropertyBase* objectProperties);
protected:
    MctPropertyBase *objectProperties;
};

class MctAddedTextGraphicObject : public MctChangedTexObjectBase
{
public:
    MctAddedTextGraphicObject(QString name, MctEmbObjProperties* embObjProps = NULL);
    ~MctAddedTextGraphicObject();
};

class MctRemovedTextGraphicObject : public MctChangedTexObjectBase
{
public:
    MctRemovedTextGraphicObject(QString name, MctEmbObjProperties* embObjProps = NULL);
    ~MctRemovedTextGraphicObject();
};

class MctAddedTextGraphicObjectInTable : public MctAddedTextGraphicObject , public MctTable
{
public:
    MctAddedTextGraphicObjectInTable(QString name, MctEmbObjProperties* embObjProps = NULL, QString cellName = "", QString tableName = "", MctCell *cellInfo = NULL);
    ~MctAddedTextGraphicObjectInTable();
};

class MctRemovedTextGraphicObjectInTable : public MctRemovedTextGraphicObject , public MctTable
{
public:
    MctRemovedTextGraphicObjectInTable(QString name, MctEmbObjProperties* embObjProps = NULL, QString cellName = "", QString tableName = "", MctCell *cellInfo = NULL);
    ~MctRemovedTextGraphicObjectInTable();
};

class MctAddedEmbeddedObject : public virtual MctNode
{
public:
    MctAddedEmbeddedObject(QString name);
    ~MctAddedEmbeddedObject();
};

class MctRemovedEmbeddedObject : public virtual MctNode
{
public:
    MctRemovedEmbeddedObject();
    ~MctRemovedEmbeddedObject();
};

class MctAddedTextTable : public MctChangedTexObjectBase
{
public:    
    MctAddedTextTable(QString name, MctTableProperties* tableProps = NULL);
    MctAddedTextTable(int row, int col, QTextTableFormat format);
    ~MctAddedTextTable();

    int getRowCount();
    int getColCount();
    QTextTableFormat getTableFormat();
    void setRowCount(int row);
    void setColCount(int col);
    void setTableFormat(QTextTableFormat format);

protected:
    int row;
    int col;
    QTextTableFormat tableFormat;
};

class MctRemovedTextTable : public MctChangedTexObjectBase
{
public:
    MctRemovedTextTable(QString name, MctTableProperties* tableProps = NULL);
    //MctRemovedTextTable(int row, int col, QTextTableFormat format);
    ~MctRemovedTextTable();
};

class MctAddedTextTableInTable : public MctAddedTextTable , public MctTable
{
public:
    MctAddedTextTableInTable(QString name, MctTableProperties* tableProps = NULL, QString cellName = "", QString tableName = "", MctCell *cellInfo = NULL);
    //MctAddedTextTableInTable(int row, int col, QTextTableFormat format, MctCell *cellInfo = NULL);
    ~MctAddedTextTableInTable();
};

class MctRemovedTextTableInTable : public MctRemovedTextTable , public MctTable
{
public:
    MctRemovedTextTableInTable(QString name, MctTableProperties* tableProps = NULL, QString cellName = "", QString tableName = "", MctCell *cellInfo = NULL);
    ~MctRemovedTextTableInTable();
};

class MctStylePropertyChangeInTable : public MctStylePropertyChange , public MctTable
{
public:
    MctStylePropertyChangeInTable(ChangeEventList *propChanges, QString cellName = "", QString tableName = "", MctCell *cellInfo = NULL);
    ~MctStylePropertyChangeInTable();
};

class MctRowChangeInTable : public virtual MctNode
{
public:
    MctRowChangeInTable(int startRow, int rowCount, QString tableName);
    ~MctRowChangeInTable();
    int getStartRow() const;
    void setStartRow(int n);
    int getRowCount() const;
    void setRowCount(int n);
    QString getTableName() const;
    void setTableName(QString n);
private:
    int startRow;
    int rowCount;
    QString tableName;
};

class MctColChangeInTable : public virtual MctNode
{
public:
    MctColChangeInTable(int startCol, int colCount, QString tableName);
    ~MctColChangeInTable();   
    int getStartCol() const;
    void setStartCol(int n);
    int getColCount() const;
    void setColCount(int n);
    QString getTableName() const;
    void setTableName(QString n);
private:
    int startCol;
    int colCount;
    QString tableName;
};

class MctAddedRowInTable : public MctRowChangeInTable
{
public:
    MctAddedRowInTable(int startRow, int rowCount, QString tableName);
    ~MctAddedRowInTable();
};

class MctRemovedRowInTable : public MctRowChangeInTable
{
public:
    MctRemovedRowInTable(int startRow, int rowCount, QString tableName);
    ~MctRemovedRowInTable();
};

class MctAddedColInTable : public MctColChangeInTable
{
public:
    MctAddedColInTable(int startCol, int colCount, QString tableName);
    ~MctAddedColInTable();
};

class MctRemovedColInTable : public MctColChangeInTable
{
public:
    MctRemovedColInTable(int startCol, int colCount, QString tableName);
    ~MctRemovedColInTable();
};

#endif // MCTCHANGEENTITIES_H
