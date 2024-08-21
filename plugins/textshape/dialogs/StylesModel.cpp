/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2011-2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "StylesModel.h"

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>
#include <KoStyleThumbnailer.h>

#include <QCollator>
#include <QImage>
#include <QList>
#include <QSharedPointer>

#include <KLocalizedString>
#include <QDebug>

#include <algorithm>

StylesModel::StylesModel(KoStyleManager *manager, AbstractStylesModel::Type modelType, QObject *parent)
    : AbstractStylesModel(parent)
    , m_styleManager(nullptr)
    , m_currentParagraphStyle(nullptr)
    , m_defaultCharacterStyle(nullptr)
    , m_provideStyleNone(false)
{
    m_modelType = modelType;
    setStyleManager(manager);
    // Create a default characterStyle for the preview of "None" character style
    if (m_modelType == StylesModel::CharacterStyle) {
        m_defaultCharacterStyle = new KoCharacterStyle();
        m_defaultCharacterStyle->setStyleId(NoneStyleId);
        m_defaultCharacterStyle->setName(i18n("None"));
        m_defaultCharacterStyle->setFontPointSize(12);

        m_provideStyleNone = true;
    }
}

StylesModel::~StylesModel()
{
    delete m_currentParagraphStyle;
    delete m_defaultCharacterStyle;
}

QModelIndex StylesModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column != 0)
        return QModelIndex();

    if (!parent.isValid()) {
        if (row >= m_styleList.count())
            return QModelIndex();
        return createIndex(row, column, m_styleList[row]);
    }
    return QModelIndex();
}

QModelIndex StylesModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int StylesModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_styleList.count();
    return 0;
}

int StylesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant StylesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int id = (int)index.internalId();
    switch (role) {
    case Qt::DisplayRole: {
        return QVariant();
    }
    case Qt::DecorationRole: {
        if (!m_styleThumbnailer) {
            return QPixmap();
        }
        if (m_modelType == StylesModel::ParagraphStyle) {
            KoParagraphStyle *paragStyle = m_styleManager->paragraphStyle(id);
            if (paragStyle) {
                return m_styleThumbnailer->thumbnail(paragStyle);
            }
            if (!paragStyle && m_draftParStyleList.contains(id)) {
                return m_styleThumbnailer->thumbnail(m_draftParStyleList[id]);
            }
        } else {
            KoCharacterStyle *usedStyle = nullptr;
            if (id == NoneStyleId) {
                usedStyle = static_cast<KoCharacterStyle *>(m_currentParagraphStyle);
                if (!usedStyle) {
                    usedStyle = m_defaultCharacterStyle;
                }
                usedStyle->setName(i18n("None"));
                if (usedStyle->styleId() >= 0) { // if the styleId is NoneStyleId, we are using the default character style
                    usedStyle->setStyleId(-usedStyle->styleId()); // this style is not managed by the styleManager but its styleId will be used in the thumbnail
                                                                  // cache as part of the key.
                }
                return m_styleThumbnailer->thumbnail(usedStyle);
            } else {
                usedStyle = m_styleManager->characterStyle(id);
                if (usedStyle) {
                    return m_styleThumbnailer->thumbnail(usedStyle, m_currentParagraphStyle);
                }
                if (!usedStyle && m_draftCharStyleList.contains(id)) {
                    return m_styleThumbnailer->thumbnail(m_draftCharStyleList[id]);
                }
            }
        }
        break;
    }
    case Qt::SizeHintRole: {
        return QVariant(QSize(250, 48));
    }
    default:
        break;
    };
    return QVariant();
}

Qt::ItemFlags StylesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemFlags();
    return (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

void StylesModel::setCurrentParagraphStyle(int styleId)
{
    if (!m_styleManager || m_currentParagraphStyle == m_styleManager->paragraphStyle(styleId) || !m_styleManager->paragraphStyle(styleId)) {
        return; // TODO do we create a default paragraphStyle? use the styleManager default?
    }
    if (m_currentParagraphStyle) {
        delete m_currentParagraphStyle;
        m_currentParagraphStyle = nullptr;
    }
    m_currentParagraphStyle = m_styleManager->paragraphStyle(styleId)->clone();
}

void StylesModel::setProvideStyleNone(bool provide)
{
    if (m_modelType == StylesModel::CharacterStyle) {
        m_provideStyleNone = provide;
    }
}

QModelIndex StylesModel::indexOf(const KoCharacterStyle *style) const
{
    if (style) {
        return createIndex(m_styleList.indexOf(style->styleId()), 0, style->styleId());
    } else {
        return QModelIndex();
    }
}

QImage StylesModel::stylePreview(int row, const QSize &size)
{
    if (!m_styleManager || !m_styleThumbnailer) {
        return QImage();
    }
    if (m_modelType == StylesModel::ParagraphStyle) {
        KoParagraphStyle *usedStyle = nullptr;
        usedStyle = m_styleManager->paragraphStyle(index(row).internalId());
        if (usedStyle) {
            return m_styleThumbnailer->thumbnail(usedStyle, size);
        }
        if (!usedStyle && m_draftParStyleList.contains(index(row).internalId())) {
            return m_styleThumbnailer->thumbnail(m_draftParStyleList[index(row).internalId()], size);
        }
    } else {
        KoCharacterStyle *usedStyle = nullptr;
        if (index(row).internalId() == (quintptr)NoneStyleId) {
            usedStyle = static_cast<KoCharacterStyle *>(m_currentParagraphStyle);
            if (!usedStyle) {
                usedStyle = m_defaultCharacterStyle;
            }
            usedStyle->setName(i18n("None"));
            if (usedStyle->styleId() >= 0) {
                usedStyle->setStyleId(
                    -usedStyle
                         ->styleId()); // this style is not managed by the styleManager but its styleId will be used in the thumbnail cache as part of the key.
            }
            return m_styleThumbnailer->thumbnail(usedStyle, m_currentParagraphStyle, size);
        } else {
            usedStyle = m_styleManager->characterStyle(index(row).internalId());
            if (usedStyle) {
                return m_styleThumbnailer->thumbnail(usedStyle, m_currentParagraphStyle, size);
            }
            if (!usedStyle && m_draftCharStyleList.contains(index(row).internalId())) {
                return m_styleThumbnailer->thumbnail(m_draftCharStyleList[index(row).internalId()], m_currentParagraphStyle, size);
            }
        }
    }
    return QImage();
}
/*
QImage StylesModel::stylePreview(QModelIndex &index, const QSize &size)
{
    if (!m_styleManager || !m_styleThumbnailer) {
        return QImage();
    }
    if (m_modelType == StylesModel::ParagraphStyle) {
        KoParagraphStyle *usedStyle = 0;
        usedStyle = m_styleManager->paragraphStyle(index.internalId());
        if (usedStyle) {
            return m_styleThumbnailer->thumbnail(usedStyle, size);
        }
        if (!usedStyle && m_draftParStyleList.contains(index.internalId())) {
            return m_styleThumbnailer->thumbnail(m_draftParStyleList[index.internalId()], size);
        }
    }
    else {
        KoCharacterStyle *usedStyle = 0;
        if (index.internalId() == NoneStyleId) {
            usedStyle = static_cast<KoCharacterStyle*>(m_currentParagraphStyle);
            if (!usedStyle) {
                usedStyle = m_defaultCharacterStyle;
            }
            usedStyle->setName(i18n("None"));
            if (usedStyle->styleId() >= 0) {
                usedStyle->setStyleId(-usedStyle->styleId()); //this style is not managed by the styleManager but its styleId will be used in the thumbnail
cache as part of the key.
            }
            return m_styleThumbnailer->thumbnail(usedStyle, m_currentParagraphStyle, size);
        }
        else {
            usedStyle = m_styleManager->characterStyle(index.internalId());
            if (usedStyle) {
                return m_styleThumbnailer->thumbnail(usedStyle, m_currentParagraphStyle, size);
            }
            if (!usedStyle && m_draftCharStyleList.contains(index.internalId())) {
                return m_styleThumbnailer->thumbnail(m_draftCharStyleList[index.internalId()],m_currentParagraphStyle, size);
            }
        }
    }
    return QImage();
}
*/
void StylesModel::setStyleManager(KoStyleManager *sm)
{
    if (sm == m_styleManager)
        return;
    if (m_styleManager) {
        disconnect(sm, &KoStyleManager::paragraphStyleAdded, this, &StylesModel::addParagraphStyle);
        disconnect(sm, &KoStyleManager::characterStyleRemoved, this, &StylesModel::removeCharacterStyle);
        disconnect(sm, &KoStyleManager::paragraphStyleRemoved, this, &StylesModel::removeParagraphStyle);
        disconnect(sm, &KoStyleManager::characterStyleRemoved, this, &StylesModel::removeCharacterStyle);
    }
    m_styleManager = sm;
    if (m_styleManager == nullptr) {
        return;
    }

    if (m_modelType == StylesModel::ParagraphStyle) {
        updateParagraphStyles();
        connect(sm, &KoStyleManager::paragraphStyleAdded, this, &StylesModel::addParagraphStyle);
        connect(sm, &KoStyleManager::paragraphStyleRemoved, this, &StylesModel::removeParagraphStyle);
    } else {
        updateCharacterStyles();
        connect(sm, &KoStyleManager::characterStyleAdded, this, &StylesModel::addCharacterStyle);
        connect(sm, &KoStyleManager::characterStyleRemoved, this, &StylesModel::removeCharacterStyle);
    }
}

void StylesModel::setStyleThumbnailer(KoStyleThumbnailer *thumbnailer)
{
    m_styleThumbnailer = thumbnailer;
}

// called when the stylemanager adds a style
void StylesModel::addParagraphStyle(KoParagraphStyle *style)
{
    Q_ASSERT(style);
    QCollator collator;
    QList<int>::iterator begin = m_styleList.begin();
    int index = 0;
    for (; begin != m_styleList.end(); ++begin) {
        KoParagraphStyle *s = m_styleManager->paragraphStyle(*begin);
        if (!s && m_draftParStyleList.contains(*begin))
            s = m_draftParStyleList[*begin];
        // s should be found as the manager and the m_styleList should be in sync
        Q_ASSERT(s);
        if (collator.compare(style->name(), s->name()) < 0) {
            break;
        }
        ++index;
    }
    beginInsertRows(QModelIndex(), index, index);
    int styleId = style->styleId();
    m_styleList.insert(begin, styleId);
    connect(style, &KoParagraphStyle::nameChanged, this, [this, styleId] {
        updateName(styleId);
    });
    endInsertRows();
}

bool sortParagraphStyleByName(KoParagraphStyle *style1, KoParagraphStyle *style2)
{
    Q_ASSERT(style1);
    Q_ASSERT(style2);
    return QCollator().compare(style1->name(), style2->name()) < 0;
}

void StylesModel::updateParagraphStyles()
{
    Q_ASSERT(m_styleManager);

    beginResetModel();
    m_styleList.clear();

    QList<KoParagraphStyle *> styles = m_styleManager->paragraphStyles();
    std::sort(styles.begin(), styles.end(), sortParagraphStyleByName);

    foreach (KoParagraphStyle *style, styles) {
        if (style != m_styleManager->defaultParagraphStyle()) { // The default character style is not user selectable. It only provides individual property
                                                                // defaults and is not a style per say.
            int styleId = style->styleId();
            m_styleList.append(styleId);
            connect(style, &KoParagraphStyle::nameChanged, this, [this, styleId] {
                updateName(styleId);
            });
        }
    }

    endResetModel();
}

// called when the stylemanager adds a style
void StylesModel::addCharacterStyle(KoCharacterStyle *style)
{
    Q_ASSERT(style);
    // find the place where we need to insert the style
    QCollator collator;
    QList<int>::ConstIterator begin = m_styleList.constBegin();
    int index = 0;
    // the None style should also be the first one so only start after it
    if (begin != m_styleList.constEnd() && *begin == NoneStyleId) {
        ++begin;
        ++index;
    }
    for (; begin != m_styleList.constEnd(); ++begin) {
        KoCharacterStyle *s = m_styleManager->characterStyle(*begin);
        if (!s && m_draftCharStyleList.contains(*begin))
            s = m_draftCharStyleList[*begin];
        // s should be found as the manager and the m_styleList should be in sync
        Q_ASSERT(s);
        if (collator.compare(style->name(), s->name()) < 0) {
            break;
        }
        ++index;
    }
    beginInsertRows(QModelIndex(), index, index);
    m_styleList.insert(index, style->styleId());
    endInsertRows();
    int styleId = style->styleId();
    connect(style, &KoParagraphStyle::nameChanged, this, [this, styleId](const QString &) {
        updateName(styleId);
    });
}

bool sortCharacterStyleByName(KoCharacterStyle *style1, KoCharacterStyle *style2)
{
    Q_ASSERT(style1);
    Q_ASSERT(style2);
    return QCollator().compare(style1->name(), style2->name()) < 0;
}

void StylesModel::updateCharacterStyles()
{
    Q_ASSERT(m_styleManager);

    beginResetModel();
    m_styleList.clear();

    if (m_provideStyleNone && m_styleManager->paragraphStyles().count()) {
        m_styleList.append(NoneStyleId);
    }

    QList<KoCharacterStyle *> styles = m_styleManager->characterStyles();
    std::sort(styles.begin(), styles.end(), sortCharacterStyleByName);

    foreach (KoCharacterStyle *style, styles) {
        if (style != m_styleManager->defaultCharacterStyle()) { // The default character style is not user selectable. It only provides individual property
                                                                // defaults and is not a style per say.
            int styleId = style->styleId();
            m_styleList.append(styleId);
            connect(style, &KoParagraphStyle::nameChanged, this, [this, styleId] {
                updateName(styleId);
            });
        }
    }

    endResetModel();
}

// called when the stylemanager removes a style
void StylesModel::removeParagraphStyle(KoParagraphStyle *style)
{
    int row = m_styleList.indexOf(style->styleId());
    beginRemoveRows(QModelIndex(), row, row);
    disconnect(style, &KoParagraphStyle::nameChanged, this, nullptr);
    m_styleList.removeAt(row);
    endRemoveRows();
}

// called when the stylemanager removes a style
void StylesModel::removeCharacterStyle(KoCharacterStyle *style)
{
    int row = m_styleList.indexOf(style->styleId());
    beginRemoveRows(QModelIndex(), row, row);
    disconnect(style, &KoParagraphStyle::nameChanged, this, nullptr);
    m_styleList.removeAt(row);
    endRemoveRows();
}

void StylesModel::updateName(int styleId)
{
    // updating the name of a style can mean that the style needs to be moved inside the list to keep the sort order.
    QCollator collator;
    int oldIndex = m_styleList.indexOf(styleId);
    if (oldIndex >= 0) {
        int newIndex = 0;
        if (m_modelType == StylesModel::ParagraphStyle) {
            KoParagraphStyle *paragStyle = m_styleManager->paragraphStyle(styleId);
            if (!paragStyle && m_draftParStyleList.contains(styleId))
                paragStyle = m_draftParStyleList.value(styleId);
            if (paragStyle) {
                m_styleThumbnailer->removeFromCache(paragStyle);

                QList<int>::ConstIterator begin = m_styleList.constBegin();
                for (; begin != m_styleList.constEnd(); ++begin) {
                    // don't test again the same style
                    if (*begin == styleId) {
                        continue;
                    }
                    KoParagraphStyle *s = m_styleManager->paragraphStyle(*begin);
                    if (!s && m_draftParStyleList.contains(*begin))
                        s = m_draftParStyleList[*begin];
                    // s should be found as the manager and the m_styleList should be in sync
                    Q_ASSERT(s);
                    if (collator.compare(paragStyle->name(), s->name()) < 0) {
                        break;
                    }
                    ++newIndex;
                }
                if (oldIndex != newIndex) {
                    // beginMoveRows needs the index where it would be placed when it is still in the old position
                    // so add one when newIndex > oldIndex
                    beginMoveRows(QModelIndex(), oldIndex, oldIndex, QModelIndex(), newIndex > oldIndex ? newIndex + 1 : newIndex);
                    m_styleList.removeAt(oldIndex);
                    m_styleList.insert(newIndex, styleId);
                    endMoveRows();
                }
            }
        } else {
            KoCharacterStyle *characterStyle = m_styleManager->characterStyle(styleId);
            if (!characterStyle && m_draftCharStyleList.contains(styleId))
                characterStyle = m_draftCharStyleList[styleId];
            if (characterStyle) {
                m_styleThumbnailer->removeFromCache(characterStyle);

                QList<int>::ConstIterator begin = m_styleList.constBegin();
                if (begin != m_styleList.constEnd() && *begin == NoneStyleId) {
                    ++begin;
                    ++newIndex;
                }
                for (; begin != m_styleList.constEnd(); ++begin) {
                    // don't test again the same style
                    if (*begin == styleId) {
                        continue;
                    }
                    KoCharacterStyle *s = m_styleManager->characterStyle(*begin);
                    if (!s && m_draftCharStyleList.contains(*begin))
                        s = m_draftCharStyleList[*begin];
                    // s should be found as the manager and the m_styleList should be in sync
                    Q_ASSERT(s);
                    if (collator.compare(characterStyle->name(), s->name()) < 0) {
                        break;
                    }
                    ++newIndex;
                }
                if (oldIndex != newIndex) {
                    // beginMoveRows needs the index where it would be placed when it is still in the old position
                    // so add one when newIndex > oldIndex
                    beginMoveRows(QModelIndex(), oldIndex, oldIndex, QModelIndex(), newIndex > oldIndex ? newIndex + 1 : newIndex);
                    m_styleList.removeAt(oldIndex);
                    m_styleList.insert(newIndex, styleId);
                    endMoveRows();
                }
            }
        }
    }
}

QModelIndex StylesModel::firstStyleIndex()
{
    if (!m_styleList.count()) {
        return QModelIndex();
    }
    return createIndex(m_styleList.indexOf(m_styleList.at(0)), 0, m_styleList.at(0));
}

QList<int> StylesModel::StyleList()
{
    return m_styleList;
}

QHash<int, KoParagraphStyle *> StylesModel::draftParStyleList()
{
    return m_draftParStyleList;
}

QHash<int, KoCharacterStyle *> StylesModel::draftCharStyleList()
{
    return m_draftCharStyleList;
}

void StylesModel::addDraftParagraphStyle(KoParagraphStyle *style)
{
    style->setStyleId(-(m_draftParStyleList.count() + 1));
    m_draftParStyleList.insert(style->styleId(), style);
    addParagraphStyle(style);
}

void StylesModel::addDraftCharacterStyle(KoCharacterStyle *style)
{
    if (m_draftCharStyleList.count() == 0) // we have a character style "m_defaultCharacterStyle" with style id NoneStyleId in style model.
        style->setStyleId(-(m_draftCharStyleList.count() + 2));
    else
        style->setStyleId(-(m_draftCharStyleList.count() + 1));
    m_draftCharStyleList.insert(style->styleId(), style);
    addCharacterStyle(style);
}

void StylesModel::clearDraftStyles()
{
    for (KoParagraphStyle *style : m_draftParStyleList) {
        removeParagraphStyle(style);
    }
    m_draftParStyleList.clear();
    for (KoCharacterStyle *style : m_draftCharStyleList) {
        removeCharacterStyle(style);
    }
    m_draftCharStyleList.clear();
}

StylesModel::Type StylesModel::stylesType() const
{
    return m_modelType;
}
