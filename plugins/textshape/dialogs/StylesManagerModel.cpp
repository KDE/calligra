/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "StylesManagerModel.h"

#include "TextShapeDebug.h"

#include <KoCharacterStyle.h>
#include <KoStyleThumbnailer.h>

StylesManagerModel::StylesManagerModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_styleThumbnailer(nullptr)
{
}

QVariant StylesManagerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    if (row < 0 || row >= m_styles.size()) {
        return QVariant();
    }
    // debugTextShape << row << role;

    QVariant retval;
    switch (role) {
    case Qt::DisplayRole:
        retval = m_styles[row]->name();
        break;
    case Qt::DecorationRole:
        if (!m_styleThumbnailer) {
            retval = QPixmap();
        } else {
            retval = m_styleThumbnailer->thumbnail(m_styles[row]);
        }
        break;
    case StylePointer:
        retval = QVariant::fromValue(m_styles[row]);
        break;
    case Qt::SizeHintRole:
        retval = QVariant(QSize(250, 48));
        break;
    default:
        break;
    };
    return retval;
}

int StylesManagerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_styles.size();
}

void StylesManagerModel::setStyleThumbnailer(KoStyleThumbnailer *thumbnailer)
{
    m_styleThumbnailer = thumbnailer;
}

void StylesManagerModel::setStyles(const QList<KoCharacterStyle *> &styles)
{
    beginResetModel();
    m_styles = styles;
    endResetModel();
}

void StylesManagerModel::addStyle(KoCharacterStyle *style)
{
    if (m_styles.indexOf(style) == -1) {
        beginInsertRows(QModelIndex(), m_styles.size(), m_styles.size());
        m_styles.append(style);
        endInsertRows();
    }
}

void StylesManagerModel::removeStyle(KoCharacterStyle *style)
{
    int row = m_styles.indexOf(style);
    Q_ASSERT(row != -1);
    if (row != -1) {
        beginRemoveRows(QModelIndex(), row, row);
        m_styles.removeAt(row);
        endRemoveRows();
    }
}

void StylesManagerModel::replaceStyle(KoCharacterStyle *oldStyle, KoCharacterStyle *newStyle)
{
    debugTextShape << oldStyle << "->" << newStyle;
    int row = m_styles.indexOf(oldStyle);
    Q_ASSERT(row != -1);
    if (row != -1) {
        m_styles[row] = newStyle;
        QModelIndex index = this->index(row);
        Q_EMIT dataChanged(index, index);
    }
}

void StylesManagerModel::updateStyle(KoCharacterStyle *style)
{
    int row = m_styles.indexOf(style);
    Q_ASSERT(row != -1);
    if (row != -1) {
        debugTextShape << style << style->name();
        m_styleThumbnailer->removeFromCache(style);
        QModelIndex index = this->index(row);
        Q_EMIT dataChanged(index, index);
    }
}

QModelIndex StylesManagerModel::styleIndex(KoCharacterStyle *style)
{
    QModelIndex index;
    int row = m_styles.indexOf(style);
    if (row != -1) {
        index = this->index(row);
    }
    return index;
}
