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

#include "DockerStylesComboModel.h"

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>
#include <KoStyleThumbnailer.h>

#include <KLocale>
#include <KStringHandler>

#include <KDebug>

DockerStylesComboModel::DockerStylesComboModel(QObject *parent) :
    StylesFilteredModelBase(parent)
    , m_styleManager(0)
    , m_currentParagraphStyle(0)
    , m_defaultCharacterStyle(0)
{
}

DockerStylesComboModel::~DockerStylesComboModel()
{
    if (m_currentParagraphStyle) {
        delete m_currentParagraphStyle;
        m_currentParagraphStyle = 0;
    }
    if (m_defaultCharacterStyle) {
        delete m_defaultCharacterStyle;
        m_defaultCharacterStyle = 0;
    }
}

Qt::ItemFlags DockerStylesComboModel::flags(const QModelIndex &index) const
{
    if (index.internalId() == UsedStyleId || index.internalId() == UnusedStyleId) {
        return (Qt::NoItemFlags);
    }
    return (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

QModelIndex DockerStylesComboModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column != 0)
        return QModelIndex();

    if (!parent.isValid()) {
        if (row >= m_proxyToSource.count()) {
            return QModelIndex();
        }
        return createIndex(row, column, (m_proxyToSource.at(row) >= 0)?int(m_sourceModel->index(m_proxyToSource.at(row), 0, QModelIndex()).internalId()):m_proxyToSource.at(row));
    }
    return QModelIndex();
}

QVariant DockerStylesComboModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role){
    case AbstractStylesModel::isTitleRole: {
        if (index.internalId() == UsedStyleId || index.internalId() == UnusedStyleId) {
            return true;
        }
    }
    case Qt::DisplayRole: {
        if (index.internalId() == UsedStyleId) {
            return i18n("Used Styles");
        }
        if (index.internalId() == UnusedStyleId) {
            return i18n("Unused Styles");
        }
        return QVariant();
    }
    case Qt::DecorationRole: {
        if (m_sourceModel->stylesType() == AbstractStylesModel::CharacterStyle && index.internalId() == CharacterStyleNoneId) {
            KoCharacterStyle *usedStyle = static_cast<KoCharacterStyle*>(m_currentParagraphStyle);
            if (!usedStyle) {
                usedStyle = m_defaultCharacterStyle;
            }
            usedStyle->setName(i18n("None"));
            return m_styleThumbnailer->thumbnail(usedStyle, m_currentParagraphStyle, data(index, Qt::SizeHintRole).toSize());
        }
        else {
            return m_sourceModel->data(m_sourceModel->index(m_proxyToSource.at(index.row()), 0, QModelIndex()), role);
        }
        break;
    }
    case Qt::SizeHintRole: {
        return QVariant(QSize(250, 48));
    }
    default: break;
    };
    return QVariant();
}

QModelIndex DockerStylesComboModel::indexForCharacterStyle(const KoCharacterStyle &style) const
{
    if (&style) {
        QModelIndex sourceIndex(m_sourceModel->indexForCharacterStyle(style));

        if (!sourceIndex.isValid() || (m_sourceToProxy.at(sourceIndex.row()) < 0)) {
            return QModelIndex();
        }
        return createIndex(m_sourceToProxy.at(sourceIndex.row()), 0, int(sourceIndex.internalId()));
    }
    else {
        if (m_sourceModel->stylesType() == AbstractStylesModel::CharacterStyle) {
            return createIndex((m_proxyToSource.indexOf(CharacterStyleNoneId)), 0, CharacterStyleNoneId);
        }
        return QModelIndex();
    }
}

QImage DockerStylesComboModel::stylePreview(const QModelIndex &index, QSize size)
{
    if (!index.isValid()) {
        return QImage();
    }
    if (index.internalId() == CharacterStyleNoneId) {
        KoCharacterStyle *usedStyle = static_cast<KoCharacterStyle*>(m_currentParagraphStyle);
        if (!usedStyle) {
            usedStyle = m_defaultCharacterStyle;
        }
        usedStyle->setName(i18n("None"));
        if (usedStyle->styleId() >= 0) {
            usedStyle->setStyleId(-usedStyle->styleId()); //this style is not managed by the styleManager but its styleId will be used in the thumbnail cache as part of the key.
        }
        return m_styleThumbnailer->thumbnail(usedStyle, m_currentParagraphStyle, size);
    }
    else {
        return m_sourceModel->stylePreview(m_sourceModel->index(m_proxyToSource.at(index.row()), 0), size);
    }
}

void DockerStylesComboModel::setInitialUsedStyles(QVector<int> usedStyles)
{
    Q_UNUSED(usedStyles);
    // This is not used yet. Let's revisit this later.

//    m_usedStyles << usedStyles;
//    beginResetModel();
//    createMapping();
//    endResetModel();
}

void DockerStylesComboModel::setStylesModel(AbstractStylesModel *sourceModel)
{
    if ((!m_sourceModel || !m_defaultCharacterStyle) && sourceModel->stylesType() == AbstractStylesModel::CharacterStyle) {
        m_defaultCharacterStyle = new KoCharacterStyle();
        m_defaultCharacterStyle->setStyleId(CharacterStyleNoneId);
        m_defaultCharacterStyle->setName(i18n("None"));
        m_defaultCharacterStyle->setFontPointSize(12);
    }
    StylesFilteredModelBase::setStylesModel(sourceModel);
}

void DockerStylesComboModel::setStyleManager(KoStyleManager *sm)
{
    Q_ASSERT(sm);
    Q_ASSERT(m_sourceModel);
    if(!sm || !m_sourceModel || m_styleManager == sm) {
        return;
    }
    m_styleManager = sm;
    m_usedStyles.clear();
    m_usedStylesId.clear();

    if (m_sourceModel->stylesType() == AbstractStylesModel::CharacterStyle) {
        KoCharacterStyle *compareStyle;
        foreach(int i, m_styleManager->usedCharacterStyles()) {
            if (!m_usedStylesId.contains(i)) {
                QVector<int>::iterator begin = m_usedStyles.begin();
                compareStyle = m_styleManager->characterStyle(i);
                for ( ; begin != m_usedStyles.end(); ++begin) {
                    KoCharacterStyle *s = m_styleManager->characterStyle(m_sourceModel->index(*begin, 0, QModelIndex()).internalId());
                    if (KStringHandler::naturalCompare(compareStyle->name(), s->name()) < 0) {
                        break;
                    }
                }
                m_usedStyles.insert(begin, m_sourceModel->indexForCharacterStyle(*compareStyle).row());
                m_usedStylesId.append(i);
            }
        }
    }
    else {
        KoParagraphStyle *compareStyle;
        foreach(int i, m_styleManager->usedParagraphStyles()) {
            if (!m_usedStylesId.contains(i)) {
                QVector<int>::iterator begin = m_usedStyles.begin();
                compareStyle = m_styleManager->paragraphStyle(i);
                for ( ; begin != m_usedStyles.end(); ++begin) {
                    KoParagraphStyle *s = m_styleManager->paragraphStyle(m_sourceModel->index(*begin, 0, QModelIndex()).internalId());
                    if (KStringHandler::naturalCompare(compareStyle->name(), s->name()) < 0) {
                        break;
                    }
                }
                m_usedStyles.insert(begin, m_sourceModel->indexForParagraphStyle(*compareStyle).row());
                m_usedStylesId.append(i);
            }
        }
    }
    createMapping();
}

void DockerStylesComboModel::setCurrentParagraphStyle(int styleId)
{
    if (!m_styleManager || !m_styleManager->paragraphStyle(styleId) || m_currentParagraphStyle == m_styleManager->paragraphStyle(styleId)) {
        return; //TODO do we create a default paragraphStyle? use the styleManager default?
    }
    if (m_currentParagraphStyle) {
        delete m_currentParagraphStyle;
        m_currentParagraphStyle = 0;
    }
    m_currentParagraphStyle = m_styleManager->paragraphStyle(styleId)->clone();
    m_currentParagraphStyle->setStyleId(-styleId);
}

void DockerStylesComboModel::styleApplied(const KoCharacterStyle *style)
{
    if (!m_usedStylesId.contains(style->styleId())) {
        m_usedStylesId.append(style->styleId());
        if (m_sourceModel->stylesType() == AbstractStylesModel::CharacterStyle) {
            QVector<int>::iterator begin = m_usedStyles.begin();
            for ( ; begin != m_usedStyles.end(); ++begin) {
                KoCharacterStyle *s = m_styleManager->characterStyle(m_sourceModel->index(*begin, 0, QModelIndex()).internalId());
                if (KStringHandler::naturalCompare(style->name(), s->name()) < 0) {
                    break;
                }
            }
            m_usedStyles.insert(begin, m_sourceModel->indexForCharacterStyle(*style).row());
        }
        else {
            QVector<int>::iterator begin = m_usedStyles.begin();
            for ( ; begin != m_usedStyles.end(); ++begin) {
                KoParagraphStyle *s = m_styleManager->paragraphStyle(m_sourceModel->index(*begin, 0, QModelIndex()).internalId());
                if (KStringHandler::naturalCompare(style->name(), s->name()) < 0) {
                    break;
                }
            }
            m_usedStyles.insert(begin, m_sourceModel->indexForCharacterStyle(*(style)).row());   // We use the ForCharacterStyle variant also for parag styles because the signal exist only in charStyle variant. TODO merge these functions in StylesModel. they use the styleId anyway.
        }
        beginResetModel();
        createMapping();
        endResetModel();
    }
}

void DockerStylesComboModel::createMapping()
{
    Q_ASSERT(m_sourceModel);
    if (!m_sourceModel || !m_styleManager) {
        return;
    }

    m_proxyToSource.clear();
    m_sourceToProxy.clear();
    m_unusedStyles.clear();

    for (int i = 0; i < m_sourceModel->rowCount(QModelIndex()); ++i) {
        QModelIndex index = m_sourceModel->index(i, 0, QModelIndex());
        int id = (int)index.internalId();
        if (!m_usedStylesId.contains(id)) {
            if (m_sourceModel->stylesType() == AbstractStylesModel::ParagraphStyle) {
                KoParagraphStyle *paragStyle = m_styleManager->paragraphStyle(id);
                if (paragStyle) {
                    if (!m_unusedStyles.empty()) {
                        QVector<int>::iterator begin = m_unusedStyles.begin();
                        for ( ; begin != m_unusedStyles.end(); ++begin) {
                            KoParagraphStyle *style = m_styleManager->paragraphStyle(m_sourceModel->index(*begin, 0, QModelIndex()).internalId());
                            if (KStringHandler::naturalCompare(paragStyle->name(), style->name()) < 0) {
                                break;
                            }
                        }
                        m_unusedStyles.insert(begin, i);
                    }
                    else {
                        m_unusedStyles.append(i);
                    }
                }
            }
            else {
                KoCharacterStyle *charStyle = m_styleManager->characterStyle(id);
                if (charStyle) {
                    if (!m_unusedStyles.empty()) {
                        QVector<int>::iterator begin = m_unusedStyles.begin();
                        for ( ; begin != m_unusedStyles.end(); ++begin) {
                            KoCharacterStyle *style = m_styleManager->characterStyle(m_sourceModel->index(*begin, 0, QModelIndex()).internalId());
                            if (KStringHandler::naturalCompare(charStyle->name(), style->name()) < 0) {
                                break;
                            }
                        }
                        m_unusedStyles.insert(begin, i);
                    }
                    else {
                        m_unusedStyles.append(i);
                    }
                }
            }
        }
    }
    if (!m_usedStyles.isEmpty() || m_sourceModel->stylesType() == AbstractStylesModel::CharacterStyle) {
        m_proxyToSource << UsedStyleId;
        if (m_sourceModel->stylesType() == AbstractStylesModel::CharacterStyle) {
            m_proxyToSource << CharacterStyleNoneId;
        }
        if (!m_usedStyles.isEmpty()) {
            m_proxyToSource << m_usedStyles;
        }
    }
    if (!m_unusedStyles.isEmpty()) {
        m_proxyToSource << UnusedStyleId << m_unusedStyles; //UsedStyleId and UnusedStyleId will be detected as title (in index method) and will be treated accordingly
    }
    m_sourceToProxy.fill(-1, m_sourceModel->rowCount((QModelIndex())));
    for (int i = 0; i < m_proxyToSource.count(); ++i) {
        if (m_proxyToSource.at(i) >= 0) { //we do not need to map to the titles
            m_sourceToProxy[m_proxyToSource.at(i)] = i;
        }
    }
}
