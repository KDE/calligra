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

#include "xpm/blank_cursor.xpm"
#include "xpm/arrow_cursor.xpm"
#include "xpm/bdiag_cursor.xpm"
#include "xpm/busy_cursor.xpm"
#include "xpm/closedhand_cursor.xpm"
#include "xpm/cross_cursor.xpm"
#include "xpm/fdiag_cursor.xpm"
#include "xpm/forbidden_cursor.xpm"
#include "xpm/hand_cursor.xpm"
#include "xpm/ibeam_cursor.xpm"
#include "xpm/openhand_cursor.xpm"
#include "xpm/sizeall_cursor.xpm"
#include "xpm/sizehor_cursor.xpm"
#include "xpm/sizever_cursor.xpm"
#include "xpm/splith_cursor.xpm"
#include "xpm/splitv_cursor.xpm"
#include "xpm/uparrow_cursor.xpm"
#include "xpm/wait_cursor.xpm"
#include "xpm/whatsthis_cursor.xpm"

#include <QMap>
#include <QVariant>
#include <QCursor>
#include <QBitmap>

#include <KLocale>
#include <KDebug>
#include <KGlobal>
#include <KIconLoader>

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

    QPixmap pixmapForIndex(int index, const QPalette& pal, bool transparentBackground = false) const
    {
        if (index < 0 || index > 18)
            index = 0;
        QPixmap xpm(m_xpms[index]);
        if (transparentBackground)
            return xpm;
        QPixmap px(xpm.size());
        QColor bg( pal.color(QPalette::Base) ); // paint bg with to avoid invisible black-on-black painting
        bg.setAlpha(127);
        px.fill(bg);
        QPainter p(&px);
        p.drawPixmap(0, 0, xpm);
        return px;
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
        strings << i18nc("Mouse Cursor Shape", "No cursor") //0
        << i18nc("Mouse Cursor Shape", "Arrow") //1
        << i18nc("Mouse Cursor Shape", "Up arrow") //2
        << i18nc("Mouse Cursor Shape", "Cross") //3
        << i18nc("Mouse Cursor Shape", "Waiting") //4
        << i18nc("Mouse Cursor Shape", "Text cursor") //5
        << i18nc("Mouse Cursor Shape", "Size vertical") //6
        << i18nc("Mouse Cursor Shape", "Size horizontal") //7
        << i18nc("Mouse Cursor Shape", "Size slash") //8
        << i18nc("Mouse Cursor Shape", "Size backslash") //9
        << i18nc("Mouse Cursor Shape", "Size all") //10
        << i18nc("Mouse Cursor Shape", "Split vertical") //11
        << i18nc("Mouse Cursor Shape", "Split horizontal") //12
        << i18nc("Mouse Cursor Shape", "Pointing hand") //13
        << i18nc("Mouse Cursor Shape", "Forbidden") //14
        << i18nc("Mouse Cursor Shape", "What's this?") //15
        << i18nc("Mouse Cursor Shape", "Busy") //16
        << i18nc("Mouse Cursor Shape", "Open hand") //17
        << i18nc("Mouse Cursor Shape", "Closed hand"); //18
        return strings;
    }
    static char ** m_xpms[];
};

char ** CursorListData::m_xpms[] =
{
    blank_cursor_xpm,
    arrow_cursor_xpm,
    uparrow_cursor_xpm,
    cross_cursor_xpm,
    wait_cursor_xpm,
    ibeam_cursor_xpm,
    sizever_cursor_xpm,
    sizehor_cursor_xpm,
    bdiag_cursor_xpm,
    fdiag_cursor_xpm,
    sizeall_cursor_xpm,
    splitv_cursor_xpm,
    splith_cursor_xpm,
    hand_cursor_xpm,
    forbidden_cursor_xpm,
    whatsthis_cursor_xpm,
    busy_cursor_xpm,
    openhand_cursor_xpm,
    closedhand_cursor_xpm
};

K_GLOBAL_STATIC(CursorListData, s_cursorListData)

//----------------------
class CursorIconProvider : public ComboBox::Options::IconProviderInterface
{
public:
    CursorIconProvider(QWidget* parent) : m_parent(parent) {}
    virtual QIcon icon(int index) const
    {
          return s_cursorListData->pixmapForIndex(index, m_parent->palette());
    }
    virtual IconProviderInterface* clone() const
    {
        return new CursorIconProvider(m_parent);
    }
    QPointer<QWidget> m_parent;
};

//----------------------

ComboBox::Options initComboBoxOptions( QWidget* parent )
{
    ComboBox::Options options;
    options.iconProvider = new CursorIconProvider(parent);
    return options;
}

CursorEdit::CursorEdit(QWidget *parent)
        : ComboBox(s_cursorListData, initComboBoxOptions( this ), parent)
{
 //   if (property)
 //       property->setListData(new Property::ListData(*s_cursorListData));
}

CursorEdit::~CursorEdit()
{
}

QCursor CursorEdit::cursorValue() const
{
    return QCursor( (Qt::CursorShape)ComboBox::value().toInt() );
//    return QCursor( s_cursorListData->indexToShape(ComboBox::value().toInt()) );
}

void CursorEdit::setCursorValue(const QCursor &value)
{
//    ComboBox::setValue( s_cursorListData->shapeToIndex( value.shape() ) );
    ComboBox::setValue( value.shape() );
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
    options.removeBorders = false;
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
    const QPixmap pm( s_cursorListData->pixmapForIndex(comboIndex, option.palette)
        .scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation) );
    QPoint pmPoint(option.rect.topLeft());
    pmPoint.setX(pmPoint.x() + 2);
    painter->drawPixmap(pmPoint, pm);
    QRect r(option.rect);
    r.setLeft(2 + r.left() + 1 + pm.width());
    painter->drawText(r, Qt::AlignVCenter | Qt::AlignLeft, 
        s_cursorListData->names[ comboIndex ] );
    painter->restore();
}

#include "cursoredit.moc"
