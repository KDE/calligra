/* This file is part of the KDE project
   Copyright (C) 2011-2014 Jarosław Staniek <staniek@kde.org>

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

#include "KexiProjectItemDelegate.h"
#include "KexiProjectModel.h"
#include "KexiProjectModelItem.h"

#include <kexiutils/identifier.h>
#include <kexiutils/utils.h>

#include <QLineEdit>

static int paddingBeforeGroupItem(const QFontMetrics& fm)
{
    return fm.lineSpacing() / 2;
}

class KexiProjectItemDelegate::Private
{
public:
    Private() {}
};

KexiProjectItemDelegate::KexiProjectItemDelegate(QObject *parent)
  : QStyledItemDelegate(parent), d(new Private)
{
}

KexiProjectItemDelegate::~KexiProjectItemDelegate()
{
    delete d;
}

void KexiProjectItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    const KexiProjectModel* model = qobject_cast<const KexiProjectModel*>(index.model());
    QPersistentModelIndex highlighted = model->itemWithSearchHighlight();
    QStyleOptionViewItem newOption(option);
    if (highlighted.isValid() && highlighted == index) {
        newOption.state |= QStyle::State_MouseOver;
    }
    KexiProjectModelItem *item = static_cast<KexiProjectModelItem*>(index.internalPointer());
    if (!item->partItem()) { // this is a group item
        if (item->childCount() == 0) {
            return;
        }
        newOption.palette = KexiUtils::paletteWithDimmedColor(newOption.palette,
                                                              QPalette::Active, QPalette::Text);
        newOption.palette.setColor(QPalette::Disabled, QPalette::Text,
                                   newOption.palette.color(QPalette::Active, QPalette::Text));
        newOption.displayAlignment = Qt::AlignLeft | Qt::AlignBottom;
        newOption.state &= (0xffffffff ^ QStyle::State_MouseOver);
        newOption.rect.setBottom(newOption.rect.bottom() - 3);
    }
    QStyledItemDelegate::paint(painter, newOption, index);
}

QWidget* KexiProjectItemDelegate::createEditor(
    QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *editor = QStyledItemDelegate::createEditor(parent, option, index);
    if (qobject_cast<QLineEdit*>(editor)) { // sanity check
        KexiUtils::IdentifierValidator *validator = new KexiUtils::IdentifierValidator(editor);
        validator->setLowerCaseForced(true);
        qobject_cast<QLineEdit*>(editor)->setValidator(validator);
    }
    return editor;
}

QSize KexiProjectItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    QSize s = QStyledItemDelegate::sizeHint(option, index);
    KexiProjectModelItem *item = static_cast<KexiProjectModelItem*>(index.internalPointer());
    if (!item->partItem()) { // this is a group item: add padding before
        if (item->childCount() == 0) {
            return QSize(0, 0);
        }
        s.setHeight(s.height() + paddingBeforeGroupItem(option.fontMetrics));
    }
    return s;
}
