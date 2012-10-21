/* This file is part of the KDE project
 * Copyright (C) 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
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

#include "StylesManagerStylesModel.h"

#include "AbstractStylesModel.h"

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>
#include <KoStyleThumbnailer.h>

#include <KLocale>

#include <QMap>
#include <QSize>
#include <QVariant>

#include <KDebug>


StylesManagerStylesModel::StylesManagerStylesModel(QObject *parent) :
    StylesFilteredModelBase(parent),
    m_styleManager(0),
    m_currentSelectedStyle(0)
{
}

int StylesManagerStylesModel::rowCount(const QModelIndex &parent) const
{
    return m_modifiedStyles.count() + m_proxyToSource.count();
}

QModelIndex StylesManagerStylesModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || row >= (m_modifiedStyles.count() + m_proxyToSource.count()) || column != 0) {
        return QModelIndex();
    }

    if (!parent.isValid()) {
        if (row < m_modifiedStyles.count()) {
            return createIndex(row, 0, m_modifiedStyles.keys().at(row));
        }
        else {
            return createIndex(row, 0, (int)(m_sourceModel->index(m_proxyToSource.at(row - m_modifiedStyles.count()), 0, parent).internalId()));
        }
    }
    return QModelIndex();
}

QVariant StylesManagerStylesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case AbstractStylesModel::isModifiedStyle: {
        if (m_modifiedStyles.contains(index.internalId())) {
            return true;
        }
        else {
            return false;
        }
    }
    case Qt::DisplayRole: {
        return QVariant();
    }
    case Qt::DecorationRole: {
        if (m_modifiedStyles.contains(index.internalId())) {
            if (m_modifiedStyles.value(index.internalId())->styleType() == KoCharacterStyle::ParagraphStyle) {
                return m_styleThumbnailer->thumbnail(dynamic_cast<KoParagraphStyle*>(m_modifiedStyles.value(index.internalId())), QSize(250, 48));
            }
        }
        return m_sourceModel->data(m_sourceModel->index(m_proxyToSource.at(index.row() - m_modifiedStyles.count()), 0, QModelIndex()), role);
    }
    case Qt::SizeHintRole: {
        return QSize(250, 48);
    }
    default: break;
    }
    return QVariant();
}

void StylesManagerStylesModel::setStyleManager(KoStyleManager *sm)
{
    Q_ASSERT(sm);
    if (!sm) {
        return;
    }

    m_styleManager = sm;
}

KoCharacterStyle* StylesManagerStylesModel::currentSelectedStyle()
{
    return m_currentSelectedStyle;
}

void StylesManagerStylesModel::slotModifiedStyle()
{
    if (!m_modifiedStyles.contains(m_currentSelectedStyle->styleId())) {
        beginResetModel();
        if (m_currentSelectedStyle->styleType() == KoCharacterStyle::ParagraphStyle) {
            KoParagraphStyle *paragStyle = dynamic_cast<KoParagraphStyle*>(m_currentSelectedStyle);
            if (paragStyle) {
                m_currentSelectedStyle = paragStyle->clone();
                m_modifiedStyles.insert(m_currentSelectedStyle->styleId(), m_currentSelectedStyle);
            }
        }
        else {
            m_currentSelectedStyle = m_currentSelectedStyle->clone();
            m_modifiedStyles.insert(m_currentSelectedStyle->styleId(), m_currentSelectedStyle);
        }
        createMapping();
        endResetModel();
    }
}

void StylesManagerStylesModel::slotSetCurrentIndex(QModelIndex index)
{
    if (m_modifiedStyles.contains(index.internalId())) {
        m_currentSelectedStyle = m_modifiedStyles.value(index.internalId());
    }
    else {
        if (!(m_currentSelectedStyle = m_styleManager->paragraphStyle(index.internalId()))) {
            m_currentSelectedStyle = m_styleManager->characterStyle(index.internalId());
        }
    }
}

void StylesManagerStylesModel::createMapping()
{
    Q_ASSERT(m_sourceModel);
    Q_ASSERT(m_styleManager);
    if (!m_sourceModel || !m_styleManager) {
        return;
    }

    m_sourceToProxy.clear();
    m_proxyToSource.clear();

    for (int i = 0; i < m_sourceModel->rowCount(QModelIndex()); ++i) {
        QModelIndex index = m_sourceModel->index(i, 0, QModelIndex());
        int id = (int)index.internalId();
        if (!m_modifiedStyles.contains(id)) {
            m_proxyToSource.append(i);
        }
    }
    m_sourceToProxy.fill(-1, m_sourceModel->rowCount((QModelIndex())));
    for (int i = 0; i < m_proxyToSource.count(); ++i) {
        m_sourceToProxy[m_proxyToSource.at(i)] = i;
    }
}
