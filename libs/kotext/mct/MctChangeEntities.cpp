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

#include "MctChangeEntities.h"

#include "MctEmbObjProperties.h"
#include "MctStaticData.h"
#include "MctTableProperties.h"

#include <QDebug>

#include <KoCharacterStyle.h>

MctTable::MctTable(QString cellName, QString tableName, MctCell *cellInfo)
    : MctNode()
{
    if (cellInfo)
        MctCell(cellInfo->cellName(), cellInfo->tableName());
    else
        MctCell(cellName, tableName);
}

MctTable::~MctTable()
{

}

MctParagraphBreak::MctParagraphBreak()
    : MctNode()
{

}

MctParagraphBreak::~MctParagraphBreak()
{

}

MctParagraphBreakInTable::MctParagraphBreakInTable(QString cellName, QString tableName, MctCell *cellInfo)
    : MctParagraphBreak()
    , MctTable(cellName, tableName, cellInfo)
{

}

MctParagraphBreakInTable::~MctParagraphBreakInTable()
{

}

MctDelParagraphBreak::MctDelParagraphBreak()
    : MctNode()
{

}

MctDelParagraphBreak::~MctDelParagraphBreak()
{

}

MctDelParagraphBreakInTable::MctDelParagraphBreakInTable(QString cellName, QString tableName, MctCell *cellInfo)
    : MctDelParagraphBreak()
    , MctTable(cellName, tableName, cellInfo)
{

}

MctDelParagraphBreakInTable::~MctDelParagraphBreakInTable()
{

}

MctStringChange::MctStringChange(QString string)
    : MctNode()
    , string(string)
{

}

MctStringChange::~MctStringChange()
{

}

QString MctStringChange::getString() const
{
    return this->string;
}

void MctStringChange::setString(QString string)
{
    this->string = string;
}

MctStringChangeInTable::MctStringChangeInTable(QString string, QString cellName, QString tableName, MctCell *cellInfo)
    : MctStringChange(string)
    , MctTable(cellName, tableName, cellInfo)
{

}

MctStringChangeInTable::~MctStringChangeInTable()
{

}

MctStylePropertyChange::MctStylePropertyChange(ChangeEventList *changeList)
{
    this->textPropChanges = new ChangeEventList();
    this->paragraphPropChanges = new ChangeEventList();
    this->listPropChanges = new ChangeEventList();
    this->otherPropChanges = new ChangeEventList();
    if(changeList != NULL) {
        this->sortPropertyChanges(changeList);
    }
}

MctStylePropertyChange::~MctStylePropertyChange()
{
    delete paragraphPropChanges;
    delete textPropChanges;
    delete listPropChanges;
    delete otherPropChanges;
}

ChangeEventList *MctStylePropertyChange::getTextPropChanges()
{
    return textPropChanges;
}

ChangeEventList *MctStylePropertyChange::getParagraphPropChanges()
{
    return paragraphPropChanges;
}

ChangeEventList *MctStylePropertyChange::getListPropChanges()
{
    return listPropChanges;
}

ChangeEventList *MctStylePropertyChange::getOtherPropChanges()
{
    return otherPropChanges;
}

/**
 * @brief MctStylePropertyChange::sortPropertyChanges Sort the different change types into theire own container
 * @param propChanges
 */
void MctStylePropertyChange::sortPropertyChanges(ChangeEventList *propChanges)
{
    for (auto it = propChanges->begin(); it != propChanges->end(); ++it) {

        int type = (*it)->getType();

        if ( type == QTextFormat::CharFormat ) {
            this->textPropChanges->append(*it);
        } else if ( type == QTextFormat::BlockFormat ) {
            this->paragraphPropChanges->append(*it);
        } else if ( type == QTextFormat::ListFormat ) {
            this->listPropChanges->append(*it);
        } else {
            this->otherPropChanges->append(*it);
        }
    }
}

MctAddedTextFrame::MctAddedTextFrame()
    : MctNode()
{

}

MctAddedTextFrame::~MctAddedTextFrame()
{

}

MctRemovedTextFrame::MctRemovedTextFrame()
    : MctNode()
{

}

MctRemovedTextFrame::~MctRemovedTextFrame()
{

}

MctChangedTexObjectBase::MctChangedTexObjectBase(QString name, MctPropertyBase *objectProperties)
    : MctNode()    
    , objectProperties(objectProperties)
{
    setName(name);
    this->setObjectName(name);
}

MctChangedTexObjectBase::~MctChangedTexObjectBase()
{
    delete objectProperties;
}

void MctChangedTexObjectBase::setObjectName(QString name)
{
    if (this->objectProperties == NULL)
        return;
    else
        this->objectProperties->setPropertyName(name);
}

MctPropertyBase * MctChangedTexObjectBase::getObjectProperties() const
{
    return this->objectProperties;
}

void MctChangedTexObjectBase::setObjectProperties(MctPropertyBase* objectProperties)
{
    this->objectProperties  = objectProperties;
}

MctAddedTextGraphicObject::MctAddedTextGraphicObject(QString name, MctEmbObjProperties* embObjProps)
    : MctChangedTexObjectBase(name, embObjProps)
{

}

MctAddedTextGraphicObject::~MctAddedTextGraphicObject()
{

}

MctRemovedTextGraphicObject::MctRemovedTextGraphicObject(QString name, MctEmbObjProperties* embObjProps)
    : MctChangedTexObjectBase(name, embObjProps)
{

}

MctRemovedTextGraphicObject::~MctRemovedTextGraphicObject()
{

}

MctAddedTextGraphicObjectInTable::MctAddedTextGraphicObjectInTable(QString name, MctEmbObjProperties* embObjProps, QString cellName, QString tableName, MctCell *cellInfo)
    : MctAddedTextGraphicObject(name, embObjProps)
    , MctTable(cellName, tableName, cellInfo)
{

}

MctAddedTextGraphicObjectInTable::~MctAddedTextGraphicObjectInTable()
{

}

MctRemovedTextGraphicObjectInTable::MctRemovedTextGraphicObjectInTable(QString name, MctEmbObjProperties* embObjProps, QString cellName, QString tableName, MctCell *cellInfo)
    : MctRemovedTextGraphicObject(name, embObjProps)
    , MctTable(cellName, tableName, cellInfo)
{

}

MctRemovedTextGraphicObjectInTable::~MctRemovedTextGraphicObjectInTable()
{

}

MctAddedEmbeddedObject::MctAddedEmbeddedObject(QString name)
    : MctNode()
{
    this->name = name;
}

MctAddedEmbeddedObject::~MctAddedEmbeddedObject()
{

}

MctRemovedEmbeddedObject::MctRemovedEmbeddedObject()
    : MctNode()
{

}

MctRemovedEmbeddedObject::~MctRemovedEmbeddedObject()
{

}

MctAddedTextTable::MctAddedTextTable(QString name, MctTableProperties *tableProps)
    : MctChangedTexObjectBase(name, tableProps)
{

}

MctAddedTextTable::MctAddedTextTable(int row, int col, QTextTableFormat format)
    : MctChangedTexObjectBase("")
    , row(row), col(col), tableFormat(format)
{
}

int MctAddedTextTable::getRowCount()
{
    return row;
}

int MctAddedTextTable::getColCount()
{
    return col;
}

QTextTableFormat MctAddedTextTable::getTableFormat()
{
    return tableFormat;
}

void MctAddedTextTable::setRowCount(int row)
{
    this->row = row;
}

void MctAddedTextTable::setColCount(int col)
{
    this->col = col;
}

void MctAddedTextTable::setTableFormat(QTextTableFormat format)
{
    this->tableFormat = format;
}

MctAddedTextTable::~MctAddedTextTable()
{

}

MctRemovedTextTable::MctRemovedTextTable(QString name, MctTableProperties* tableProps)
    :  MctChangedTexObjectBase(name, tableProps)
{

}

/*MctRemovedTextTable::MctRemovedTextTable(int row, int col, QTextTableFormat format)
    :  MctChangedTexObjectBase(""), MctAddedTextTable(row, col, format)
{

}*/

MctRemovedTextTable::~MctRemovedTextTable()
{

}

MctAddedTextTableInTable::MctAddedTextTableInTable(QString name, MctTableProperties *tableProps, QString cellName, QString tableName, MctCell *cellInfo)
    : MctAddedTextTable(name, tableProps)
    , MctTable(cellName, tableName, cellInfo)
{

}

MctAddedTextTableInTable::~MctAddedTextTableInTable()
{

}

MctRemovedTextTableInTable::MctRemovedTextTableInTable(QString name, MctTableProperties *tableProps, QString cellName, QString tableName, MctCell *cellInfo)
    : MctRemovedTextTable(name, tableProps)
    , MctTable(cellName, tableName, cellInfo)
{

}

MctRemovedTextTableInTable::~MctRemovedTextTableInTable()
{

}

MctStylePropertyChangeInTable::MctStylePropertyChangeInTable(ChangeEventList *propChanges, QString cellName, QString tableName, MctCell *cellInfo)
    : MctStylePropertyChange(propChanges)
    , MctTable(cellName, tableName, cellInfo)
{

}

MctStylePropertyChangeInTable::~MctStylePropertyChangeInTable()
{

}

MctRowChangeInTable::MctRowChangeInTable(int startRow, int rowCount, QString tableName)
    : MctNode()
    , startRow(startRow)
    , rowCount(rowCount)
    , tableName(tableName)
{

}

MctRowChangeInTable::~MctRowChangeInTable()
{

}

int MctRowChangeInTable::getStartRow() const
{
    return this->startRow;
}

void MctRowChangeInTable::setStartRow(int n)
{
    this->startRow = n;
}

int MctRowChangeInTable::getRowCount() const
{
    return this->rowCount;
}

void MctRowChangeInTable::setRowCount(int n)
{
    this->rowCount = n;
}

QString MctRowChangeInTable::getTableName() const
{
    return this->tableName;
}

void MctRowChangeInTable::setTableName(QString n)
{
    this->tableName = n;
}

MctColChangeInTable::MctColChangeInTable(int startCol, int colCount, QString tableName)
    : MctNode()
    , startCol(startCol)
    , colCount(colCount)
    , tableName(tableName)
{

}

MctColChangeInTable::~MctColChangeInTable()
{

}

int MctColChangeInTable::getStartCol() const
{
    return this->startCol;
}

void MctColChangeInTable::setStartCol(int n)
{
    this->startCol = n;
}

int MctColChangeInTable::getColCount() const
{
    return this->colCount;
}

void MctColChangeInTable::setColCount(int n)
{
    this->colCount = n;
}

QString MctColChangeInTable::getTableName() const
{
    return this->tableName;
}

void MctColChangeInTable::setTableName(QString n)
{
    this->tableName = n;
}

MctAddedRowInTable::MctAddedRowInTable(int startRow, int rowCount, QString tableName)
    : MctRowChangeInTable(startRow, rowCount, tableName)
{

}

MctAddedRowInTable::~MctAddedRowInTable()
{

}

MctRemovedRowInTable::MctRemovedRowInTable(int startRow, int rowCount, QString tableName)
    : MctRowChangeInTable(startRow, rowCount, tableName)
{

}

MctRemovedRowInTable::~MctRemovedRowInTable()
{

}

MctAddedColInTable::MctAddedColInTable(int startCol, int colCount, QString tableName)
    : MctColChangeInTable(startCol, colCount, tableName)
{

}

MctAddedColInTable::~MctAddedColInTable()
{

}

MctRemovedColInTable::MctRemovedColInTable(int startCol, int colCount, QString tableName)
    : MctColChangeInTable(startCol, colCount, tableName)
{

}

MctRemovedColInTable::~MctRemovedColInTable()
{

}


ChangeEvent::ChangeEvent(QTextFormat o, QTextFormat n)
{
    if ( o.type() != n.type() ) {
        qCritical() << "Different format type in a change event!";
        return;
    } else {
#if 0   // FIXME: legacy code, remove
        if (o.isBlockFormat()) {
            oldFormat = o.toBlockFormat();
            newFormat = n.toBlockFormat();
        } else if (o.isCharFormat()) {
            oldFormat = o.toCharFormat();
            newFormat = n.toCharFormat();
        } else if (o.isFrameFormat()) {
            qWarning() << "Unsupported format type";
        } else if (o.isImageFormat()) {
            qWarning() << "Unsupported format type";
        } else if (o.isListFormat()) {
            qWarning() << "Unsupported format type";
        } else if (o.isTableCellFormat()) {
            qWarning() << "Unsupported format type";
        } else if (o.isTableFormat()) {
            qWarning() << "Unsupported format type";
        } else {
            qCritical() << "Unknow format type";
        }
#endif
        oldFormat = o;
        newFormat = n;
        changedProperties = new PropertyMap();  // Need to call calcDiff() before using it!
    }

}

PropertyMap *ChangeEvent::getChanges()
{
    return changedProperties;
}

int ChangeEvent::getType()
{
    return oldFormat.type();
}

QTextFormat ChangeEvent::getOldFormat()
{
    return oldFormat;
}

QTextFormat ChangeEvent::getNewFormat()
{
    return newFormat;
}

void ChangeEvent::setOldFormat(QTextFormat format)
{
    oldFormat = format;
}

void ChangeEvent::setNewFormat(QTextFormat format)
{
    newFormat = format;
}

void ChangeEvent::ensureProperties(QTextFormat &format1, QTextFormat format2)
{
    const QMap<int, QVariant> props = format2.properties();
    QMap<int, QVariant>::const_iterator it = props.begin();
    while (it != props.end()) {
        if (!it.value().isNull() && !format1.hasProperty(it.key())) {
            format1.setProperty(it.key(), it.value());
        }
        ++it;
    }
}

QTextFormat ChangeEvent::getNewValuesForFormat(QTextFormat current, QTextFormat base)
{
    int type = MctStaticData::UnknownProperty;
    if(base.isCharFormat()) { type = MctStaticData::CharacterProperty;}
    else if(base.isBlockFormat()) { type = MctStaticData::ParagraphProperty;}
    else if(base.isListFormat()) { type = MctStaticData::ListProperty;}

    QTextFormat format(base.type());
    foreach (int key, base.properties().keys()) {
        if(current.property(key) != base.property(key) && key != KoCharacterStyle::ChangeTrackerId) {
            QString currentProp = current.property(key).toString();
            QString baseProp = base.property(key).toString();
            QString propName = MctStaticData::getTextPropetyString(key, type);
            if(key == QTextFormat::ForegroundBrush || key == QTextFormat::BackgroundBrush
               || key == QTextFormat::TextUnderlineColor || key == KoCharacterStyle::StrikeOutColor) {
                currentProp = current.brushProperty(key).color().name();
                baseProp = base.brushProperty(key).color().name();
            }

            if(currentProp == "") {
                QVariant defValue = "";
                if (MctStaticData::TEXT_PROPS_BOOLEAN.contains(propName)) {
                    defValue = false;
                } else if (MctStaticData::TEXT_PROPS_INT.contains(propName)) {
                    defValue = 0;
                    if(key == QTextFormat::BlockAlignment) {
                        defValue = 17;
                    }
                } else if (MctStaticData::TEXT_PROPS_DOUBLE.contains(propName)) {
                    defValue = 0.0;
                }
                format.setProperty(key, defValue);
            } else {
                if(key == QTextFormat::BackgroundBrush && !current.background().isOpaque()) {
                    format.setProperty(key, QBrush(QColor(Qt::black), Qt::NoBrush));
                } else {
                    format.setProperty(key, current.property(key));
                }
            }
            if(key == QTextFormat::FontWeight && (currentProp == "0" || currentProp == "")) {
                format.setProperty(key, 50);
            }
            QString prop = format.property(key).toString();
            if(key == QTextFormat::ForegroundBrush || key == QTextFormat::BackgroundBrush
               || key == QTextFormat::TextUnderlineColor || key == KoCharacterStyle::StrikeOutColor) {
                prop = format.brushProperty(key).color().name();
            }
            qDebug() << "Diff Property: " << MctStaticData::getTextPropetyString(key, type)  << " = " << prop;
        }
    }
    return format;
}

QTextFormat ChangeEvent::getOldValuesForFormat(QTextFormat current, QTextFormat base)
{
    int type = MctStaticData::UnknownProperty;
    if(base.isCharFormat()) { type = MctStaticData::CharacterProperty;}
    else if(base.isBlockFormat()) { type = MctStaticData::ParagraphProperty;}
    else if(base.isListFormat()) { type = MctStaticData::ListProperty;}

    QTextFormat format(base.type());
    foreach (int key, current.properties().keys()) {
        if(current.property(key) != base.property(key)) {
            if(key == KoCharacterStyle::ChangeTrackerId) {
                continue;
            }
            QString propName = MctStaticData::getTextPropetyString(key, type);
            QString baseProp = base.property(key).toString();
            QString currentProp = current.property(key).toString();
            if(key == QTextFormat::FontWeight && baseProp == ""  && currentProp == "50") {
                continue;
            }
            if(key == QTextFormat::ForegroundBrush || key == QTextFormat::BackgroundBrush
               || key == QTextFormat::TextUnderlineColor || key == KoCharacterStyle::StrikeOutColor) {
                baseProp = base.brushProperty(key).color().name();
                currentProp = current.brushProperty(key).color().name();
            }
            if(baseProp == currentProp && (key == QTextFormat::BackgroundBrush || key == QTextFormat::ForegroundBrush
               || key == QTextFormat::TextUnderlineColor || key == KoCharacterStyle::StrikeOutColor)) {
                if(base.brushProperty(key).isOpaque() == current.brushProperty(key).isOpaque()) {
                    continue;
                }
            }

            if(baseProp == "") {
                if(currentProp == "" || currentProp == "0.0" || currentProp == "0"|| currentProp == "false" || currentProp == "#000000") {
                    continue;
                }
                QVariant defValue = "";
                if (MctStaticData::TEXT_PROPS_BOOLEAN.contains(propName)) {
                    defValue = false;
                } else if (MctStaticData::TEXT_PROPS_INT.contains(propName)) {
                    defValue = 0;
                    if(key == QTextFormat::BlockAlignment) {
                        defValue = 17;
                    }
                } else if (MctStaticData::TEXT_PROPS_DOUBLE.contains(propName)) {
                    defValue = 0.0;
                }
                format.setProperty(key, defValue);
            } else {
                if(key == QTextFormat::BackgroundBrush && !base.background().isOpaque()) {
                    format.setProperty(key, QBrush(QColor(Qt::black), Qt::NoBrush));
                } else {
                    format.setProperty(key, base.property(key));
                }
            }
            if(key == QTextFormat::FontWeight && (baseProp == "0" || baseProp == "")) {
                format.setProperty(key, 50);
            }
            qDebug() << "Diff property base: " << propName << " = " << baseProp;
            qDebug() << "Diff property current: " << propName << " = " << currentProp;
        }
    }
    return format;
}

void ChangeEvent::printProperties(QTextFormat format)
{
    qDebug() << "Format contains properties: ";
    int type = MctStaticData::UnknownProperty;
    if(format.isCharFormat()) { type = MctStaticData::CharacterProperty;}
    else if(format.isBlockFormat()) { type = MctStaticData::ParagraphProperty;}
    else if(format.isListFormat()) { type = MctStaticData::ListProperty;}
    foreach (int key, format.properties().keys()) {
        QString prop = format.property(key).toString();
        if(key == QTextFormat::ForegroundBrush || key == QTextFormat::BackgroundBrush
           || key == QTextFormat::TextUnderlineColor || key == KoCharacterStyle::StrikeOutColor) {
            prop = format.brushProperty(key).color().name();
        }
        qDebug() << "Property: " << MctStaticData::getTextPropetyString(key, type)  << " = " << prop << QString::number(key, 16);
    }
}

void ChangeEvent::calcDiff(int type)
{
    foreach (int key, oldFormat.properties().keys()) {
        if(key == KoCharacterStyle::ChangeTrackerId) {
            continue;
        }
        if(newFormat.property(key) != oldFormat.property(key)) {
            QString propName = MctStaticData::getTextPropetyString(key, type);
            MctStaticData::textPropMapTester(key);
            QString oldProp = oldFormat.property(key).toString();
            QString newProp = newFormat.property(key).toString();
            if(key == QTextFormat::ForegroundBrush || key == QTextFormat::BackgroundBrush) {
                oldProp = key == QTextFormat::BackgroundBrush && !oldFormat.background().isOpaque() ? "transparent" : oldFormat.brushProperty(key).color().name();
                newProp = newFormat.brushProperty(key).color().name();
            }

            if(oldProp == "" || oldProp == "false" || oldProp == "0" || oldProp == "0.0") {
                qDebug() << "Property ADDED: " << propName << QString::number(key, 16) << " = " << oldProp << " new value: " << newProp;
                changedProperties->insert(key, QPair<QString, ChangeAction>(oldProp, ChangeAction::ADDED));
            } else if(newProp == "" || newProp == "false" || newProp == "0" || newProp == "0.0") {
                qDebug() << "Property REMOVED: " << propName << QString::number(key, 16) << " = " << oldProp << " new value: " << newProp;
                changedProperties->insert(key, QPair<QString, ChangeAction>(oldProp, ChangeAction::REMOVED));
            } else {
                qDebug() << "Property CHANGED: " << propName << QString::number(key, 16) << " = " << oldProp << " new value: " << newProp;
                changedProperties->insert(key, QPair<QString, ChangeAction>(oldProp, ChangeAction::CHANGED));
            }
        }
    }
}
