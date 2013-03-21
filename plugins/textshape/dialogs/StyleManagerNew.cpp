/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#include "StyleManagerNew.h"

#include "StylesModelNew.h"
#include "StylesSortFilterProxyModel.h"

#include <KoStyleManager.h>
#include <KoStyleThumbnailer.h>
#include <KoParagraphStyle.h>
#include <KoCharacterStyle.h>

#include <QListView>
#include <QModelIndex>
#include <QTabWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QInputDialog>

#include <KDebug>

StyleManagerNew::StyleManagerNew(QWidget *parent)
        : QWidget(parent)
         ,m_styleManager(0)
        , m_paragraphStylesModel(new StylesModelNew(this))
        , m_characterStylesModel(new StylesModelNew(this))
        , m_paragraphProxyModel(new StylesSortFilterProxyModel(this))
        , m_characterProxyModel(new StylesSortFilterProxyModel(this))
        , m_thumbnailer(new KoStyleThumbnailer())
//        , m_blockSignals(false)
//        , m_blockStyleChangeSignals(false)
        , m_unappliedStyleChanges(false)
//        , m_currentStyleChanged(false)
{
    widget.setupUi(this);
    layout()->setMargin(0);
    widget.bNew->setToolTip(i18n("Create a new style inheriting the current style"));

    // Force "Base" background of the style listviews to white, so the background
    // is consistent with the one of the preview area. Also the usual document text colors
    // are dark, because made for a white paper background, so with a dark UI
    // color scheme they are hardly seen.
    // TODO: update to background color of currently selected/focused shape/page
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, QColor(Qt::white));
    widget.paragraphStylesListView->setPalette(palette);
    widget.characterStylesListView->setPalette(palette);

    m_paragraphStylesModel->setStyleThumbnailer(m_thumbnailer);
    m_characterStylesModel->setStyleThumbnailer(m_thumbnailer);
    m_paragraphProxyModel->setDynamicSortFilter(true);
    m_characterProxyModel->setDynamicSortFilter(true);
    m_paragraphProxyModel->invalidate();
    m_characterProxyModel->invalidate();
    m_paragraphProxyModel->setSourceModel(m_paragraphStylesModel);
    m_characterProxyModel->setSourceModel(m_characterStylesModel);
    m_paragraphProxyModel->sort(0);
    m_characterProxyModel->sort(0);
    //m_paragraphProxyModel->setSortRole(Qt::DisplayRole);
    //m_characterProxyModel->setSortRole(Qt::DisplayRole);
    widget.paragraphStylesListView->setModel(m_paragraphProxyModel);
    widget.characterStylesListView->setModel(m_characterProxyModel);

    connect(widget.paragraphStylesListView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(slotParagraphStyleSelected(QModelIndex)));
    connect(widget.characterStylesListView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(slotCharacterStyleSelected(QModelIndex)));

    connect(widget.bNew, SIGNAL(pressed()), this, SLOT(buttonNewPressed()));
    //connect(widget.bDelete, SIGNAL(pressed()), this, SLOT(buttonDeletePressed()));
    widget.bDelete->setVisible(false); // TODO make it visible when we can safely delete styles

    connect(widget.tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

    connect(widget.paragraphStylePage, SIGNAL(styleChanged()), this, SLOT(currentParagraphStyleChanged()));
    connect(widget.characterStylePage, SIGNAL(styleChanged()), this, SLOT(currentCharacterStyleChanged()));
    connect(widget.paragraphStylePage, SIGNAL(nameChanged(QString)), this, SLOT(currentParagraphNameChanged(QString)));
    connect(widget.characterStylePage, SIGNAL(nameChanged(QString)), this, SLOT(currentCharacterNameChanged(QString)));
}

StyleManagerNew::~StyleManagerNew()
{
    qDeleteAll(m_modifiedParagraphStyles.keys());
    qDeleteAll(m_modifiedCharacterStyles.keys());
}

void StyleManagerNew::setStyleManager(KoStyleManager *sm)
{
    Q_ASSERT(sm);
    m_styleManager = sm;

    widget.paragraphStylePage->setStyleManager(m_styleManager); //also updates style combos
    widget.characterStylePage->setStyleManager(m_styleManager); //also updates style combos
    widget.tabs->setCurrentIndex(widget.tabs->indexOf(widget.paragraphStylesListView));
    connect(sm, SIGNAL(styleAdded(KoParagraphStyle*)), this, SLOT(addParagraphStyle(KoParagraphStyle*)));
    connect(sm, SIGNAL(styleAdded(KoCharacterStyle*)), this, SLOT(addCharacterStyle(KoCharacterStyle*)));
    connect(sm, SIGNAL(styleRemoved(KoParagraphStyle*)), this, SLOT(removeParagraphStyle(KoParagraphStyle*)));
    connect(sm, SIGNAL(styleRemoved(KoCharacterStyle*)), this, SLOT(removeCharacterStyle(KoCharacterStyle*)));

    QList<KoCharacterStyle*> styles;
    QList<KoParagraphStyle *> paragraphStyles = m_styleManager->paragraphStyles();
    KoParagraphStyle *defaultParagraphStyle = m_styleManager->defaultParagraphStyle();
    foreach (KoParagraphStyle *style, paragraphStyles) {
        if (style != defaultParagraphStyle) {
            styles.append(style);
        }
    }
    m_paragraphStylesModel->setStyles(styles);
    styles = m_styleManager->characterStyles();
    styles.removeOne(m_styleManager->defaultCharacterStyle());
    m_characterStylesModel->setStyles(styles);
    if (!paragraphStyles.isEmpty()) {
        widget.paragraphStylesListView->setCurrentIndex(m_paragraphProxyModel->mapFromSource(m_paragraphStylesModel->index(0)));
    }
    if (!styles.isEmpty()) {
        widget.characterStylesListView->setCurrentIndex(m_characterProxyModel->mapFromSource(m_characterStylesModel->index(0)));
    }

    tabChanged(0);
}

void StyleManagerNew::setParagraphStyle(KoParagraphStyle *style)
{
    qDebug() << Q_FUNC_INFO << style;
    widget.characterStylePage->save();
    //widget.characterStylePage->setStyle(0); // TODO check if that is needed
    widget.paragraphStylePage->save();
    KoParagraphStyle *localStyle = 0;

    if (style) {
        QMap<KoParagraphStyle*, KoParagraphStyle*>::iterator it = m_modifiedParagraphStyles.find(style);
        if (it == m_modifiedParagraphStyles.end()) {
            localStyle = style->clone();
            m_modifiedParagraphStyles.insert(localStyle, style);
            m_paragraphStylesModel->replaceStyle(style, localStyle);
        }
        else {
            localStyle = dynamic_cast<KoParagraphStyle *>(it.key());
        }

        widget.paragraphStylesListView->setCurrentIndex(m_paragraphProxyModel->mapFromSource(m_paragraphStylesModel->styleIndex(localStyle)));
    }
    widget.paragraphStylePage->setStyle(localStyle);
    widget.stackedWidget->setCurrentWidget(widget.paragraphStylePage);
    widget.tabs->setCurrentIndex(widget.tabs->indexOf(widget.paragraphStylesListView));
    widget.paragraphStylesListView->setEnabled(style != 0);
}

void StyleManagerNew::setCharacterStyle(KoCharacterStyle *style, bool canDelete)
{
    qDebug() << Q_FUNC_INFO << style;
    Q_UNUSED(canDelete);
    widget.paragraphStylePage->save();
    //widget.paragraphStylePage->setStyle(0); // TODO check if that is needed
    widget.characterStylePage->save();
    KoCharacterStyle *localStyle = 0;

    if (style) {
        QMap<KoCharacterStyle*, KoCharacterStyle*>::iterator it = m_modifiedCharacterStyles.find(style);
        if (it == m_modifiedCharacterStyles.end()) {
            localStyle = style->clone();
            m_modifiedCharacterStyles.insert(localStyle, style);
            m_characterStylesModel->replaceStyle(style, localStyle);
        }
        else {
            localStyle = it.key();
        }
        widget.characterStylesListView->setCurrentIndex(m_characterProxyModel->mapFromSource(m_characterStylesModel->styleIndex(localStyle)));
    }
    widget.characterStylePage->setStyle(localStyle);
    widget.stackedWidget->setCurrentWidget(widget.characterStylePage);
    widget.tabs->setCurrentIndex(widget.tabs->indexOf(widget.characterStylesListView));
    widget.characterStylePage->setEnabled(style != 0);
 //   widget.bDelete->setEnabled(canDelete);
}

void StyleManagerNew::setUnit(const KoUnit &unit)
{
    widget.paragraphStylePage->setUnit(unit);
}

void StyleManagerNew::save()
{
    qDebug() << Q_FUNC_INFO << "YYY" << m_modifiedParagraphStyles.size() << m_modifiedCharacterStyles.size();
    if (!m_unappliedStyleChanges) {
        return;
    }
    widget.paragraphStylePage->save();
    widget.characterStylePage->save();
    widget.paragraphStylePage->setStyle(0);
    widget.characterStylePage->setStyle(0);

    m_styleManager->beginEdit();

    for (QMap<KoParagraphStyle*, KoParagraphStyle*>::iterator it(m_modifiedParagraphStyles.begin()); it != m_modifiedParagraphStyles.end(); ++it) {
        if (it.value() == 0) {
            m_styleManager->add(it.key());
        }
        else {
            KoParagraphStyle *altered = it.key();
            qDebug() << "YYY 2" << altered << it.key() << it.value();
            m_styleManager->alteredStyle(altered);
            m_paragraphStylesModel->replaceStyle(altered, it.value());
            delete altered;
        }
    }
    m_modifiedParagraphStyles.clear();

    for (QMap<KoCharacterStyle*, KoCharacterStyle*>::iterator it(m_modifiedCharacterStyles.begin()); it != m_modifiedCharacterStyles.end(); ++it) {
        if (it.value() == 0) {
            m_styleManager->add(it.key());
        }
        else {
            KoCharacterStyle *altered = it.key();
            m_styleManager->alteredStyle(altered);
            m_characterStylesModel->replaceStyle(altered, it.value());
            delete altered;
        }
    }
    m_modifiedCharacterStyles.clear();

    m_styleManager->endEdit();

    // set the paragraph and character style new so it has a cloned style to work on and we don't change the actual style.
    KoParagraphStyle *paragraphStyle = dynamic_cast<KoParagraphStyle*>(m_paragraphProxyModel->data(widget.paragraphStylesListView->currentIndex(),
                                                                       StylesModelNew::StylePointer).value<KoCharacterStyle*>());
    if (paragraphStyle) {
        setParagraphStyle(paragraphStyle);
    }

    KoCharacterStyle *characterStyle = m_characterProxyModel->data(widget.characterStylesListView->currentIndex(),
                                                                   StylesModelNew::StylePointer).value<KoCharacterStyle*>();
    if (characterStyle) {
        setCharacterStyle(characterStyle);
    }

    m_unappliedStyleChanges = false;
}

void StyleManagerNew::currentParagraphStyleChanged()
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphProxyModel->data(widget.paragraphStylesListView->currentIndex(), StylesModelNew::StylePointer).value<KoCharacterStyle*>());
    qDebug() << Q_FUNC_INFO << style;
    if (style) {
        widget.paragraphStylePage->save();
        m_paragraphStylesModel->updateStyle(style);
        m_unappliedStyleChanges = true;
    }
}

void StyleManagerNew::currentParagraphNameChanged(const QString &name)
{
    KoCharacterStyle *style = m_paragraphProxyModel->data(widget.paragraphStylesListView->currentIndex(), StylesModelNew::StylePointer).value<KoCharacterStyle*>();
    qDebug() << Q_FUNC_INFO << style << name;
    if (style) {
        style->setName(name);
        currentParagraphStyleChanged();
    }
}

void StyleManagerNew::currentCharacterStyleChanged()
{
    KoCharacterStyle *style = m_characterProxyModel->data(widget.characterStylesListView->currentIndex(), StylesModelNew::StylePointer).value<KoCharacterStyle*>();
    qDebug() << Q_FUNC_INFO << style;
    if (style) {
        widget.characterStylePage->save();
        m_characterStylesModel->updateStyle(style);
        m_unappliedStyleChanges = true;
    }
}

void StyleManagerNew::currentCharacterNameChanged(const QString &name)
{
    KoCharacterStyle *style = m_characterProxyModel->data(widget.characterStylesListView->currentIndex(), StylesModelNew::StylePointer).value<KoCharacterStyle*>();
    qDebug() << Q_FUNC_INFO << style << name;
    if (style) {
        style->setName(name);
        currentCharacterStyleChanged();
    }
}


void StyleManagerNew::addParagraphStyle(KoParagraphStyle *style)
{
    widget.paragraphStylePage->setStyleManager(m_styleManager); //updates style combos
    m_paragraphStylesModel->addStyle(style);
    setParagraphStyle(style);
    m_unappliedStyleChanges = true;
}

void StyleManagerNew::addCharacterStyle(KoCharacterStyle *style)
{
    widget.characterStylePage->setStyleManager(m_styleManager); //updates style combos
    m_characterStylesModel->addStyle(style);
    setCharacterStyle(style);
    m_unappliedStyleChanges = true;
}

void StyleManagerNew::removeParagraphStyle(KoParagraphStyle* style)
{
    if (m_modifiedParagraphStyles.contains(style)) {
        m_modifiedParagraphStyles.remove(style);
        m_paragraphStylesModel->removeStyle(style);
    }
    widget.paragraphStylePage->setStyleManager(m_styleManager); //updates style combos
}

void StyleManagerNew::removeCharacterStyle(KoCharacterStyle* style)
{
    if (m_modifiedCharacterStyles.contains(style)) {
        m_modifiedCharacterStyles.remove(style);
        m_characterStylesModel->removeStyle(style);
    }
    widget.characterStylePage->setStyleManager(m_styleManager); //updates style combos
}

void StyleManagerNew::slotParagraphStyleSelected(QModelIndex index)
{
#if 0
    if (!checkUniqueStyleName()) {
        widget.paragraphStylesListView->setCurrentIndex(m_paragraphStylesModel->indexForParagraphStyle(*m_selectedParagStyle));
        widget.paragraphStylePage->selectName();
        return;
    }
#endif
    KoParagraphStyle *paragraphStyle = dynamic_cast<KoParagraphStyle*>(m_paragraphProxyModel->data(index, StylesModelNew::StylePointer).value<KoCharacterStyle*>());
    if (paragraphStyle) {
        setParagraphStyle(paragraphStyle);
        return;
    }
}

void StyleManagerNew::slotCharacterStyleSelected(QModelIndex index)
{
#if 0
    if (!checkUniqueStyleName()) {
        widget.characterStylesListView->setCurrentIndex(m_characterStylesModel->indexForCharacterStyle(*m_selectedCharStyle));
        widget.characterStylePage->selectName();
        return;
    }
#endif
    KoCharacterStyle *characterStyle = m_characterProxyModel->data(index, StylesModelNew::StylePointer).value<KoCharacterStyle*>();
    if (characterStyle) {
        setCharacterStyle(characterStyle, false);
        return;
    }
}

void StyleManagerNew::buttonNewPressed()
{
    qDebug() << Q_FUNC_INFO << 1;
    if (!checkUniqueStyleName()) {
        if (widget.tabs->indexOf(widget.paragraphStylesListView) == widget.tabs->currentIndex()){
            widget.paragraphStylePage->selectName();
        } else {
            widget.characterStylePage->selectName();
        }
        return;
    }
    qDebug() << Q_FUNC_INFO << 2;
    if (widget.tabs->indexOf(widget.paragraphStylesListView) == widget.tabs->currentIndex()){
        KoParagraphStyle *newStyle = 0;
        KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphProxyModel->data(widget.paragraphStylesListView->currentIndex(),
                                                                                              StylesModelNew::StylePointer).value<KoCharacterStyle*>());
        if (style) {
            newStyle = style->clone();
        }
        else {
            newStyle = new KoParagraphStyle();
        }
        newStyle->setName(i18n("New Style"));
        m_modifiedParagraphStyles.insert(newStyle, 0);
        addParagraphStyle(newStyle);
        widget.paragraphStylePage->selectName();
    }
    else {
        KoCharacterStyle *newStyle = 0;
        KoCharacterStyle *style = m_characterProxyModel->data(widget.characterStylesListView->currentIndex(), StylesModelNew::StylePointer).value<KoCharacterStyle*>();
        if (style) {
            newStyle = style->clone();
        }
        else {
            newStyle = new KoCharacterStyle();
        }
        newStyle->setName(i18n("New Style"));
        //newStyle->setStyleId(-m_draftCharacterStyles.size());
        qDebug() << Q_FUNC_INFO << "XXX New Style" << newStyle << newStyle->styleId();
        m_modifiedCharacterStyles.insert(newStyle, 0);
        addCharacterStyle(newStyle);
        widget.characterStylePage->selectName();
    }
}

void StyleManagerNew::tabChanged(int index)
{
#if 0
    if (!checkUniqueStyleName()) {
        m_blockSignals = true;
        if (widget.tabs->indexOf(widget.paragraphStylesListView) == widget.tabs->currentIndex()){
            widget.tabs->setCurrentIndex(widget.tabs->indexOf(widget.characterStylesListView));
        } else {
            widget.tabs->setCurrentIndex(widget.tabs->indexOf(widget.paragraphStylesListView));
        }
        widget.characterStylePage->selectName();
        m_blockSignals = false;
        return;
    }
#endif
    if (widget.tabs->indexOf(widget.paragraphStylesListView) == index) {
        KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphProxyModel->data(widget.paragraphStylesListView->currentIndex(),
                                                                                              StylesModelNew::StylePointer).value<KoCharacterStyle*>());
        setParagraphStyle(style);
        widget.stackedWidget->setCurrentWidget(widget.paragraphStylePage);
    }
    else {
        KoCharacterStyle *style = m_characterProxyModel->data(widget.characterStylesListView->currentIndex(), StylesModelNew::StylePointer).value<KoCharacterStyle*>();
        setCharacterStyle(style);
        widget.stackedWidget->setCurrentWidget(widget.characterStylePage);
    }
}

bool StyleManagerNew::unappliedStyleChanges()
{
    return m_unappliedStyleChanges;
}

bool StyleManagerNew::checkUniqueStyleName()
{
#if 0
    // FIXME that does not work for altered styles that change the name
    if (m_selectedParagStyle) {
        QList<KoCharacterStyle *> stylesList = m_paragraphStylesModel->styleList();
        foreach(KoCharacterStyle *style, stylesList) {
            if (widget.paragraphStylePage->styleName() == style->name()) {
                if (style != m_selectedParagStyle) {
                    QMessageBox::critical(this, i18n("Warning"), i18n("Another style named '%1' already exist. Please choose another name.", style->name()));
                    return false;
                }
            }

        }
    }
    if (m_selectedCharStyle) {
        QList<KoCharacterStyle *> stylesList = m_characterStylesModel->styleList();
        foreach(KoCharacterStyle *style, stylesList) {
            qDebug() << Q_FUNC_INFO << "XXX" << style << m_selectedCharStyle;
            if (widget.characterStylePage->styleName() == style->name()) {
                if (style != m_selectedCharStyle) {
                    QMessageBox::critical(this, i18n("Warning"), i18n("Another style named '%1' already exist. Please choose another name.", style->name()));
                    widget.characterStylesListView->setCurrentIndex(m_characterStylesModel->indexForStyle(m_selectedCharStyle));
                    return false;
                }
            }

        }
    }

#endif
    return true;
}

/* TODO
    On new move focus to name text field.
*/

#include <StyleManagerNew.moc>
