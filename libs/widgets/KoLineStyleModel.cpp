/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoLineStyleModel_p.h"

#include <QPen>

KoLineStyleModel::KoLineStyleModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_hasTempStyle(false)
{
    // add standard dash patterns
    for (int i = Qt::NoPen; i < Qt::CustomDashLine; i++) {
        QPen pen(static_cast<Qt::PenStyle>(i));
        m_styles << pen.dashPattern();
    }
}

int KoLineStyleModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_styles.count() + (m_hasTempStyle ? 1 : 0);
}

QVariant KoLineStyleModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case Qt::DecorationRole: {
        QPen pen(Qt::black);
        pen.setWidth(2);
        if (index.row() < Qt::CustomDashLine)
            pen.setStyle(static_cast<Qt::PenStyle>(index.row()));
        else if (index.row() < m_styles.count())
            pen.setDashPattern(m_styles[index.row()]);
        else if (m_hasTempStyle)
            pen.setDashPattern(m_tempStyle);
        else
            pen.setStyle(Qt::NoPen);

        return QVariant(pen);
    }
    case Qt::SizeHintRole:
        return QSize(100, 15);
    default:
        return QVariant();
    }
}

bool KoLineStyleModel::addCustomStyle(const QVector<qreal> &style)
{
    if (m_styles.contains(style))
        return false;

    m_styles.append(style);
    return true;
}

int KoLineStyleModel::setLineStyle(Qt::PenStyle style, const QVector<qreal> &dashes)
{
    // check if we select a standard or custom style
    if (style < Qt::CustomDashLine) {
        // a standard style
        beginResetModel();
        m_hasTempStyle = false;
        endResetModel();
        return style;
    } else if (style == Qt::CustomDashLine) {
        // a custom style -> check if already added
        int index = m_styles.indexOf(dashes, Qt::CustomDashLine);
        if (index < 0) {
            // not already added -> add temporarily
            beginResetModel();
            m_tempStyle = dashes;
            m_hasTempStyle = true;
            endResetModel();
            return m_styles.count();
        } else {
            // already added -> return index
            beginResetModel();
            m_hasTempStyle = false;
            endResetModel();
            return index;
        }
    }
    return -1;
}
