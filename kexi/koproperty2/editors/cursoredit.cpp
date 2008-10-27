/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2008 Jarosław Staniek <staniek@kde.org>

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

#include "cursoredit.h"
#include "Property.h"

#include <QMap>
#include <QVariant>
#include <QCursor>

#include <KLocale>
#include <KDebug>
#include <KGlobal>

class CursorListData : public KoProperty::Property::ListData
{
public:
    CursorListData() : KoProperty::Property::ListData(keysInternal(), stringsInternal())
    {
    }

    Qt::CursorShape indexToShape(int index) const
    {
        if (index < 0 || index >= keys.count())
            return Qt::ArrowCursor;
        return (Qt::CursorShape)(keys[index].toInt());
/*        if (index >= 1 && index <= 10)
            return (Qt::CursorShape)(index - 1);
        if (index == 0)
            return Qt::BlankCursor;
        if (index >= 11 && index <= 18)
            return (Qt::CursorShape)index;
        return Qt::ArrowCursor;*/
    }

    int shapeToIndex(Qt::CursorShape _shape) const
    {
        int index = 0;
        foreach (const QVariant& shape, keys) {
            if (shape.toInt() == _shape)
                return index;
            index++;
        }
        return 0;
    }
private:
    static QList<QVariant> keysInternal() {
        QList<QVariant> keys;
        keys
        << Qt::BlankCursor
        << Qt::ArrowCursor
        << Qt::UpArrowCursor
        << Qt::CrossCursor
        << Qt::WaitCursor
        << Qt::IBeamCursor
        << Qt::SizeVerCursor
        << Qt::SizeHorCursor
        << Qt::SizeBDiagCursor
        << Qt::SizeFDiagCursor
        << Qt::SizeAllCursor
        << Qt::SplitVCursor
        << Qt::SplitHCursor
        << Qt::PointingHandCursor
        << Qt::ForbiddenCursor
        << Qt::WhatsThisCursor
        << Qt::BusyCursor
        << Qt::OpenHandCursor
        << Qt::ClosedHandCursor;
        return keys;
    }

    static QStringList stringsInternal() {
        QStringList strings;
        strings << i18nc("Mouse Cursor Shape", "No Cursor") //0
        << i18nc("Mouse Cursor Shape", "Arrow") //1
        << i18nc("Mouse Cursor Shape", "Up Arrow") //2
        << i18nc("Mouse Cursor Shape", "Cross") //3
        << i18nc("Mouse Cursor Shape", "Waiting") //4
        << i18nc("Mouse Cursor Shape", "I") //5
        << i18nc("Mouse Cursor Shape", "Size Vertical") //6
        << i18nc("Mouse Cursor Shape", "Size Horizontal") //7
        << i18nc("Mouse Cursor Shape", "Size Slash") //8
        << i18nc("Mouse Cursor Shape", "Size Backslash") //9
        << i18nc("Mouse Cursor Shape", "Size All") //10
        << i18nc("Mouse Cursor Shape", "Split Vertical") //11
        << i18nc("Mouse Cursor Shape", "Split Horizontal") //12
        << i18nc("Mouse Cursor Shape", "Pointing Hand") //13
        << i18nc("Mouse Cursor Shape", "Forbidden") //14
        << i18nc("Mouse Cursor Shape", "What's This?") //15
        << i18nc("Mouse Cursor Shape", "Busy") //16
        << i18nc("Mouse Cursor Shape", "Open Hand") //17
        << i18nc("Mouse Cursor Shape", "Closed Hand"); //18
        return strings;
    }
};

K_GLOBAL_STATIC(CursorListData, s_cursorListData)

CursorEdit::CursorEdit(QWidget *parent)
        : ComboBox(s_cursorListData, ComboBox::Options(), parent)
{
 //   if (property)
 //       property->setListData(new Property::ListData(*s_cursorListData));
}

CursorEdit::~CursorEdit()
{
}

QCursor CursorEdit::cursorValue() const
{
    return QCursor( s_cursorListData->indexToShape(ComboBox::value().toInt()) );
}

void CursorEdit::setCursorValue(const QCursor &value)
{
    ComboBox::setValue( s_cursorListData->shapeToIndex( value.shape() ) );
}

/*
void CursorEdit::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
    ComboBox::drawViewer(p, cg, r, value.value<QCursor>().shape());
}*/

/*
void CursorEdit::setProperty(Property *prop)
{
    if (prop && prop != property())
        prop->setListData(new Property::ListData(*m_cursorListData));
    ComboBox::setProperty(prop);
}*/

//---------------

CursorDelegate::CursorDelegate()
{
//    options.removeBorders = false;
}

QWidget * CursorDelegate::createEditor( int type, QWidget *parent, 
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    return new CursorEdit(parent);
}

void CursorDelegate::paint( QPainter * painter, 
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    painter->save();
//    const EditorDataModel *editorModel = dynamic_cast<const EditorDataModel*>(index.model());
//    Property *prop = editorModel->propertyForItem(index);
    int comboIndex = s_cursorListData->shapeToIndex( index.data(Qt::EditRole).value<QCursor>().shape() );
    painter->drawText(option.rect, Qt::AlignVCenter | Qt::AlignLeft, 
        s_cursorListData->names[ comboIndex ] );
    painter->restore();
}

#include "cursoredit.moc"
