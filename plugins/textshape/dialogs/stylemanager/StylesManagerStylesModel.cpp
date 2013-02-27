/* This file is part of the KDE project
 * Copyright (C) 2013 Pierre Stirnweiss <pstirnweiss@googlemail.org>
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

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>
#include <KoStyleThumbnailer.h>

#include <KLocale>

#include <QImage>

#include <KDebug>

StylesManagerStylesModel::StylesManagerStylesModel(QObject *parent) :
    StylesFilteredModelBase(parent)
  , m_styleManager(0)
  , m_styleIdCounter(1)
{
}

StylesManagerStylesModel::~StylesManagerStylesModel()
{

}

Qt::ItemFlags StylesManagerStylesModel::flags(const QModelIndex &index) const
{
    if (index.internalId() == NewStyleId || index.internalId() == ExistingStyleId) {
        return (Qt::NoItemFlags);
    }
    if (m_proxyToSource.at(index.row()) < 0) { //negative Ids which are not the titles (handled above) are new styles
        return (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    return m_sourceModel->flags(m_sourceModel->index(m_proxyToSource.at(index.row()), 0, QModelIndex()));
}

QModelIndex StylesManagerStylesModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column != 0)
        return QModelIndex();

    if (!parent.isValid()) {
        if (row >= m_proxyToSource.count()) {
            return QModelIndex();
        }
        //m_proxyToSource stores a mix of internalIds (negative, specific to this model) and row number in the source model
        return createIndex(row, column, (m_proxyToSource.at(row) >= 0)?int(m_sourceModel->index(m_proxyToSource.at(row), 0, QModelIndex()).internalId()):m_proxyToSource.at(row));
    }
    return QModelIndex();
}

QVariant StylesManagerStylesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role){
    case AbstractStylesModel::isTitleRole: {
        if (index.internalId() == NewStyleId || index.internalId() == ExistingStyleId) {
            return true;
        }
        break;
    }
    case AbstractStylesModel::ParagraphStylePointer: {
        if (m_sourceModel->stylesType() == AbstractStylesModel::ParagraphStyle) {
            if (index.internalId() == NewStyleId || index.internalId() == ExistingStyleId) {
                return 0;
            }
            if (m_proxyToSource.at(index.row()) < 0) {
                QVariant variant;
                variant.setValue<void*>(m_stylesMap.value(index.internalId()));
                return variant;
            }
            KoParagraphStyle *style = static_cast<KoParagraphStyle*>(m_sourceModel->data(m_sourceModel->index(m_proxyToSource.at(index.row()), 0, QModelIndex()), role).value<void*>());
            if (style) {
                if (m_stylesMap.contains(style->styleId())) {
                    style = dynamic_cast<KoParagraphStyle*>(m_stylesMap.value(style->styleId()));
                }
            }
            QVariant variant;
            variant.setValue<void*>(style);
            return variant;
        }
        return 0;
        break;
    }
    case AbstractStylesModel::CharacterStylePointer: {
        if (m_sourceModel->stylesType() == AbstractStylesModel::CharacterStyle) {
            if (index.internalId() == NewStyleId || index.internalId() == ExistingStyleId) {
                return 0;
            }
            if (m_proxyToSource.at(index.row()) < 0) {
                QVariant variant;
                variant.setValue<void*>(m_stylesMap.value(index.internalId()));
                return variant;
            }
            KoCharacterStyle *style = static_cast<KoCharacterStyle*>(m_sourceModel->data(m_sourceModel->index(m_proxyToSource.at(index.row()), 0, QModelIndex()), role).value<void*>());
            if (style) {
                if (m_stylesMap.contains(style->styleId())) {
                    style = m_stylesMap.value(style->styleId());
                }
            }
            QVariant variant;
            variant.setValue<void*>(style);
            return variant;
        }
        return 0;
        break;
    }
    case Qt::DisplayRole: {
        if (index.internalId() == NewStyleId) {
            return i18n("New Styles");
        }
        if (index.internalId() == ExistingStyleId) {
            return i18n("Existing Styles");
        }
        break;
    }
    case Qt::DecorationRole: {
        if (index.internalId() >= 0) {
            if (m_sourceModel->stylesType() == AbstractStylesModel::CharacterStyle) {
                KoCharacterStyle *style = static_cast<KoCharacterStyle*>(m_sourceModel->data(m_sourceModel->index(m_proxyToSource.at(index.row()), 0, QModelIndex()), AbstractStylesModel::CharacterStylePointer).value<void*>());
                if (style) {
                    if (m_stylesMap.contains(style->styleId())) {
                        style = m_stylesMap.value(style->styleId());
                        if (style) {
                            return m_styleThumbnailer->thumbnail(style, 0, data(index, Qt::SizeHintRole).toSize(), true);
                        }
                        return QVariant();
                    }
                }
            }
            else if (m_sourceModel->stylesType() == AbstractStylesModel::ParagraphStyle) {
                KoParagraphStyle *style = static_cast<KoParagraphStyle*>(m_sourceModel->data(m_sourceModel->index(m_proxyToSource.at(index.row()), 0, QModelIndex()), AbstractStylesModel::ParagraphStylePointer).value<void*>());
                if (style) {
                    if (m_stylesMap.contains(style->styleId())) {
                        style = dynamic_cast<KoParagraphStyle*>(m_stylesMap.value(style->styleId()));
                        if (style) {
                            return m_styleThumbnailer->thumbnail(style, data(index, Qt::SizeHintRole).toSize(), true);
                        }
                        return QVariant();
                    }
                }

            }
            return m_sourceModel->data(m_sourceModel->index(m_proxyToSource.at(index.row()), 0, QModelIndex()), role);
        }
        else {
            if (m_sourceModel->stylesType() == AbstractStylesModel::ParagraphStyle) {
                KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_stylesMap.value(index.internalId()));
                if (style) {
                    return m_styleThumbnailer->thumbnail(style, data(index, Qt::SizeHintRole).toSize(), true);
                }
                else {
                    return QVariant();
                }
            }
            else {
                KoCharacterStyle * style = m_stylesMap.value(index.internalId());
                if (style) {
                    return m_styleThumbnailer->thumbnail(style, 0, data(index, Qt::SizeHintRole).toSize(), true);
                }
                else {
                    return QVariant();
                }
            }
        }
        break;
    }
    case Qt::SizeHintRole: {
        return QVariant(QSize(250, 48));
    }
    default: break;
    };
    if (index.internalId() >= 0) {
        return m_sourceModel->data(m_sourceModel->index(m_proxyToSource.at(index.row()), 0, QModelIndex()), role);
    }
    return QVariant();
}

QModelIndex StylesManagerStylesModel::indexForCharacterStyle(const KoCharacterStyle &style) const
{
    if (&style) {
        if (style.styleId() < 0) {
            return createIndex(m_proxyToSource.indexOf(style.styleId()), 0, style.styleId());
        }
        QModelIndex sourceIndex(m_sourceModel->indexForCharacterStyle(style));

        if (!sourceIndex.isValid() || (m_sourceToProxy.at(sourceIndex.row()) < 0)) {
            return QModelIndex();
        }
        return createIndex(m_sourceToProxy.at(sourceIndex.row()), 0, int(sourceIndex.internalId()));
    }
    return QModelIndex();
}

QImage StylesManagerStylesModel::stylePreview(const QModelIndex &index, QSize size)
{
    if (!index.isValid()) {
        return QImage();
    }
    /// TODO create preview for new/unsaved styles
    return m_sourceModel->stylePreview(m_sourceModel->index(m_proxyToSource.at(index.row()), 0), size);
}

void StylesManagerStylesModel::setStyleManager(KoStyleManager *sm)
{
    Q_ASSERT(sm);
    m_styleManager = sm;
}

KoCharacterStyle* StylesManagerStylesModel::slotCreateNewStyle(const QModelIndex &index)
{
    if (m_sourceModel->stylesType() == AbstractStylesModel::CharacterStyle) {
        KoCharacterStyle *selectedStyle = static_cast<KoCharacterStyle*>(index.data(AbstractStylesModel::CharacterStylePointer).value<void*>());
        KoCharacterStyle *newStyle;
        if (selectedStyle) {
            newStyle = selectedStyle->clone();
        }
        else {
            newStyle = new KoCharacterStyle();
        }
        newStyle->setStyleId(-m_styleIdCounter);
        newStyle->setName(i18n("Style") + "_" + QString::number(m_styleIdCounter));
        m_stylesMap.insert(-m_styleIdCounter, newStyle);
        m_newStylesId.append(-m_styleIdCounter++);
        beginResetModel();
        createMapping();
        endResetModel();

        return newStyle;
    }
    else {
        KoParagraphStyle *selectedStyle = static_cast<KoParagraphStyle*>(index.data(AbstractStylesModel::ParagraphStylePointer).value<void*>());
        KoParagraphStyle *newStyle;
        if (selectedStyle) {
            newStyle = selectedStyle->clone();
        }
        else {
            newStyle = new KoParagraphStyle();
        }
        newStyle->setStyleId(-m_styleIdCounter);
        newStyle->setName(i18n("Style") + "_" + QString::number(m_styleIdCounter));
        m_stylesMap.insert(-m_styleIdCounter, newStyle);
        m_newStylesId.append(-m_styleIdCounter++);
        beginResetModel();
        createMapping();
        endResetModel();

        return newStyle;
    }
    return 0;
}

KoCharacterStyle* StylesManagerStylesModel::unsavedStyle(const QModelIndex &index)
{
    if (index.internalId() == NewStyleId || index.internalId() == ExistingStyleId) {
        return 0;
    }
    if (m_proxyToSource.at(index.row()) < 0) {
        if (m_stylesMap.contains(m_proxyToSource.at(index.row()))) {
            return m_stylesMap.value(m_proxyToSource.at(index.row()));
        }
        return 0;
    }
    if (m_sourceModel->stylesType() == AbstractStylesModel::CharacterStyle) {
        KoCharacterStyle *style = static_cast<KoCharacterStyle*>(data(index, AbstractStylesModel::CharacterStylePointer).value<void*>());
        if (!style) {
            return 0;
        }
        if (m_stylesMap.contains(style->styleId())) {
            return m_stylesMap.value(style->styleId());
        }
        else {
            style = style->clone();
            m_stylesMap.insert(style->styleId(), style);
            return style;
        }

    }
    else if (m_sourceModel->stylesType() == AbstractStylesModel::ParagraphStyle) {
        KoParagraphStyle *style = static_cast<KoParagraphStyle*>(data(index, AbstractStylesModel::ParagraphStylePointer).value<void*>());
        if (!style) {
            return 0;
        }
        if (m_stylesMap.contains(style->styleId())) {
            return m_stylesMap.value(style->styleId());
        }
        else {
            style = style->clone();
            m_stylesMap.insert(style->styleId(), style);
            return style;
        }
    }
    return 0;
}

void StylesManagerStylesModel::saveStyle(const QModelIndex &index)
{
    Q_ASSERT(m_styleManager);
    Q_ASSERT(m_sourceModel);
    if (!m_sourceModel || !m_styleManager) {
        return;
    }

    if (index.internalId() == NewStyleId || index.internalId() == ExistingStyleId) {
        return;
    }
    if (m_proxyToSource.at(index.row()) < 0) {
        if (m_sourceModel->stylesType() == AbstractStylesModel::CharacterStyle) {
            KoCharacterStyle *style = m_stylesMap.value(m_proxyToSource.at(index.row()));
            m_stylesMap.remove(m_proxyToSource.at(index.row()));
            m_newStylesId.removeAll(m_proxyToSource.at(index.row()));
            m_styleManager->add(style);
        }
        else if (m_sourceModel->stylesType() == AbstractStylesModel::ParagraphStyle) {
            KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_stylesMap.value(m_proxyToSource.at(index.row())));
            if (style) {
                m_stylesMap.remove(m_proxyToSource.at(index.row()));
                m_newStylesId.removeAll(m_proxyToSource.at(index.row()));
                m_styleManager->add(style);
            }
        }
    }
    else {
        if (m_sourceModel->stylesType() == AbstractStylesModel::CharacterStyle) {
            KoCharacterStyle *originalStyle = static_cast<KoCharacterStyle*>(m_sourceModel->data(m_sourceModel->index(m_proxyToSource.at(index.row()), 0, QModelIndex()), AbstractStylesModel::CharacterStylePointer).value<void*>());
            if (originalStyle) {
                beginResetModel();
                originalStyle->copyProperties(m_stylesMap.value(originalStyle->styleId()));
                m_stylesMap.remove(originalStyle->styleId());
                endResetModel();
            }
        }
        else if (m_sourceModel->stylesType() == AbstractStylesModel::ParagraphStyle) {
            KoParagraphStyle *originalStyle = static_cast<KoParagraphStyle*>(m_sourceModel->data(m_sourceModel->index(m_proxyToSource.at(index.row()), 0, QModelIndex()), AbstractStylesModel::ParagraphStylePointer).value<void*>());
            if (originalStyle) {
                KoParagraphStyle *unsavedStyle = dynamic_cast<KoParagraphStyle*>(m_stylesMap.value(originalStyle->styleId()));
                if (unsavedStyle) {
                    beginResetModel();
                    originalStyle->copyProperties(unsavedStyle);
                    m_stylesMap.remove(originalStyle->styleId());
                    endResetModel();
                }
            }
        }
    }
}

void StylesManagerStylesModel::createMapping()
{
    Q_ASSERT(m_sourceModel);
    if (!m_sourceModel) {
        return;
    }

    m_proxyToSource.clear();
    m_sourceToProxy.clear();

    if (!m_newStylesId.isEmpty()) {
        m_proxyToSource << NewStyleId << m_newStylesId.toVector();
    }

    if (m_sourceModel->rowCount()) {
        m_proxyToSource << ExistingStyleId;
        for(int i = 0; i < m_sourceModel->rowCount(QModelIndex()); ++i) {
            m_proxyToSource.append(i);
        }
    }
    m_sourceToProxy.fill(-1, m_sourceModel->rowCount(QModelIndex()));
    for (int i = 0; i < m_proxyToSource.count(); ++i) {
        if (m_proxyToSource.at(i) >= 0) { //we do not need to map to the titles
            m_sourceToProxy[m_proxyToSource.at(i)] = i;
        }
    }
    kDebug() << "m_proxyToSource: " << m_proxyToSource;
}
