/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2013 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "StyleManager.h"

#include "StylesManagerModel.h"
#include "StylesSortFilterProxyModel.h"

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>
#include <KoStyleThumbnailer.h>

#include <QInputDialog>
#include <QListView>
#include <QMessageBox>
#include <QModelIndex>
#include <QTabWidget>

#include <QDebug>

StyleManager::StyleManager(QWidget *parent)
    : QSplitter(parent)
    , m_styleManager(nullptr)
    , m_paragraphStylesModel(new StylesManagerModel(this))
    , m_characterStylesModel(new StylesManagerModel(this))
    , m_paragraphProxyModel(new StylesSortFilterProxyModel(this))
    , m_characterProxyModel(new StylesSortFilterProxyModel(this))
    , m_thumbnailer(new KoStyleThumbnailer())
    , m_unappliedStyleChanges(false)
{
    widget.setupUi(this);
    widget.tabs->tabBar()->setExpanding(true);
    widget.bNew->setToolTip(i18n("Create a new style inheriting the current style"));

    widget.buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);

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
    // m_paragraphProxyModel->setSortRole(Qt::DisplayRole);
    // m_characterProxyModel->setSortRole(Qt::DisplayRole);
    widget.paragraphStylesListView->setModel(m_paragraphProxyModel);
    widget.characterStylesListView->setModel(m_characterProxyModel);

    connect(widget.paragraphStylesListView, &QAbstractItemView::clicked, this, &StyleManager::slotParagraphStyleSelected);
    connect(widget.characterStylesListView, &QAbstractItemView::clicked, this, &StyleManager::slotCharacterStyleSelected);

    connect(widget.bNew, &QAbstractButton::pressed, this, &StyleManager::buttonNewPressed);
    // connect(widget.bDelete, SIGNAL(pressed()), this, SLOT(buttonDeletePressed()));
    widget.bDelete->setVisible(false); // TODO make it visible when we can safely delete styles

    connect(widget.tabs, &QTabWidget::currentChanged, this, &StyleManager::tabChanged);

    connect(widget.paragraphStylePage, &ParagraphGeneral::styleChanged, this, &StyleManager::currentParagraphStyleChanged);
    connect(widget.characterStylePage, &CharacterGeneral::styleChanged, this, &StyleManager::currentCharacterStyleChanged);
    connect(widget.paragraphStylePage, &ParagraphGeneral::nameChanged, this, &StyleManager::currentParagraphNameChanged);
    connect(widget.characterStylePage, &CharacterGeneral::nameChanged, this, &StyleManager::currentCharacterNameChanged);
}

StyleManager::~StyleManager()
{
    qDeleteAll(m_modifiedParagraphStyles.keys());
    qDeleteAll(m_modifiedCharacterStyles.keys());
}

void StyleManager::setStyleManager(KoStyleManager *sm)
{
    Q_ASSERT(sm);
    m_styleManager = sm;

    widget.paragraphStylePage->setStyleManager(m_styleManager); // also updates style combos
    widget.characterStylePage->setStyleManager(m_styleManager); // also updates style combos
    widget.tabs->setCurrentIndex(widget.tabs->indexOf(widget.paragraphStylesListView));
    connect(sm, &KoStyleManager::paragraphStyleAdded, this, &StyleManager::addParagraphStyle);
    connect(sm, &KoStyleManager::characterStyleAdded, this, &StyleManager::addCharacterStyle);
    connect(sm, &KoStyleManager::paragraphStyleRemoved, this, &StyleManager::removeParagraphStyle);
    connect(sm, &KoStyleManager::characterStyleRemoved, this, &StyleManager::removeCharacterStyle);

    QList<KoCharacterStyle *> styles;
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

void StyleManager::setParagraphStyle(KoParagraphStyle *style)
{
    widget.characterStylePage->save();
    widget.paragraphStylePage->save();
    KoParagraphStyle *localStyle = nullptr;

    if (style) {
        QMap<KoParagraphStyle *, KoParagraphStyle *>::ConstIterator it = m_modifiedParagraphStyles.constFind(style);
        if (it == m_modifiedParagraphStyles.constEnd()) {
            localStyle = style->clone();
            m_modifiedParagraphStyles.insert(localStyle, style);
            m_paragraphStylesModel->replaceStyle(style, localStyle);
        } else {
            localStyle = dynamic_cast<KoParagraphStyle *>(it.key());
        }

        widget.paragraphStylesListView->setCurrentIndex(m_paragraphProxyModel->mapFromSource(m_paragraphStylesModel->styleIndex(localStyle)));
    }
    widget.paragraphStylePage->setStyle(localStyle, 0, false);
    widget.stackedWidget->setCurrentWidget(widget.paragraphStylePage);
    widget.tabs->setCurrentIndex(widget.tabs->indexOf(widget.paragraphStylesListView));
    widget.paragraphStylesListView->setEnabled(style != nullptr);
}

void StyleManager::setCharacterStyle(KoCharacterStyle *style, bool canDelete)
{
    Q_UNUSED(canDelete);
    widget.paragraphStylePage->save();
    widget.characterStylePage->save();
    KoCharacterStyle *localStyle = nullptr;

    if (style) {
        QMap<KoCharacterStyle *, KoCharacterStyle *>::ConstIterator it = m_modifiedCharacterStyles.constFind(style);
        if (it == m_modifiedCharacterStyles.constEnd()) {
            localStyle = style->clone();
            m_modifiedCharacterStyles.insert(localStyle, style);
            m_characterStylesModel->replaceStyle(style, localStyle);
        } else {
            localStyle = it.key();
        }
        widget.characterStylesListView->setCurrentIndex(m_characterProxyModel->mapFromSource(m_characterStylesModel->styleIndex(localStyle)));
    }
    widget.characterStylePage->setStyle(localStyle, false);
    widget.stackedWidget->setCurrentWidget(widget.characterStylePage);
    widget.tabs->setCurrentIndex(widget.tabs->indexOf(widget.characterStylesListView));
    widget.characterStylePage->setEnabled(style != nullptr);
    //   widget.bDelete->setEnabled(canDelete);
}

void StyleManager::setUnit(const KoUnit &unit)
{
    widget.paragraphStylePage->setUnit(unit);
}

void StyleManager::save()
{
    if (!m_unappliedStyleChanges) {
        return;
    }
    widget.paragraphStylePage->save();
    widget.characterStylePage->save();
    widget.paragraphStylePage->setStyle(nullptr, 0, false);
    widget.characterStylePage->setStyle(nullptr, false);

    m_styleManager->beginEdit();

    for (QMap<KoParagraphStyle *, KoParagraphStyle *>::ConstIterator it(m_modifiedParagraphStyles.constBegin()); it != m_modifiedParagraphStyles.constEnd();
         ++it) {
        if (it.value() == 0) {
            m_styleManager->add(it.key());
        } else {
            KoParagraphStyle *altered = it.key();
            m_styleManager->alteredStyle(altered);
            m_paragraphStylesModel->replaceStyle(altered, it.value());
            delete altered;
        }
    }
    m_modifiedParagraphStyles.clear();

    for (QMap<KoCharacterStyle *, KoCharacterStyle *>::ConstIterator it(m_modifiedCharacterStyles.constBegin()); it != m_modifiedCharacterStyles.constEnd();
         ++it) {
        if (it.value() == 0) {
            m_styleManager->add(it.key());
        } else {
            KoCharacterStyle *altered = it.key();
            m_styleManager->alteredStyle(altered);
            m_characterStylesModel->replaceStyle(altered, it.value());
            delete altered;
        }
    }
    m_modifiedCharacterStyles.clear();

    m_styleManager->endEdit();

    // set the paragraph and character style new so it has a cloned style to work on and we don't change the actual style.
    KoParagraphStyle *paragraphStyle = dynamic_cast<KoParagraphStyle *>(
        m_paragraphProxyModel->data(widget.paragraphStylesListView->currentIndex(), StylesManagerModel::StylePointer).value<KoCharacterStyle *>());
    if (paragraphStyle) {
        setParagraphStyle(paragraphStyle);
    }

    KoCharacterStyle *characterStyle =
        m_characterProxyModel->data(widget.characterStylesListView->currentIndex(), StylesManagerModel::StylePointer).value<KoCharacterStyle *>();
    if (characterStyle) {
        setCharacterStyle(characterStyle);
    }

    setUnappliedStyleChanges(false);
}

void StyleManager::currentParagraphStyleChanged()
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle *>(
        m_paragraphProxyModel->data(widget.paragraphStylesListView->currentIndex(), StylesManagerModel::StylePointer).value<KoCharacterStyle *>());
    if (style) {
        widget.paragraphStylePage->save();
        m_paragraphStylesModel->updateStyle(style);
        setUnappliedStyleChanges(true);
    }
}

void StyleManager::currentParagraphNameChanged(const QString &name)
{
    KoCharacterStyle *style =
        m_paragraphProxyModel->data(widget.paragraphStylesListView->currentIndex(), StylesManagerModel::StylePointer).value<KoCharacterStyle *>();
    if (style) {
        style->setName(name);
        currentParagraphStyleChanged();
    }
}

void StyleManager::currentCharacterStyleChanged()
{
    KoCharacterStyle *style =
        m_characterProxyModel->data(widget.characterStylesListView->currentIndex(), StylesManagerModel::StylePointer).value<KoCharacterStyle *>();
    if (style) {
        widget.characterStylePage->save();
        m_characterStylesModel->updateStyle(style);
        setUnappliedStyleChanges(true);
    }
}

void StyleManager::currentCharacterNameChanged(const QString &name)
{
    KoCharacterStyle *style =
        m_characterProxyModel->data(widget.characterStylesListView->currentIndex(), StylesManagerModel::StylePointer).value<KoCharacterStyle *>();
    if (style) {
        style->setName(name);
        currentCharacterStyleChanged();
    }
}

void StyleManager::addParagraphStyle(KoParagraphStyle *style)
{
    widget.paragraphStylePage->setStyleManager(m_styleManager); // updates style combos
    m_paragraphStylesModel->addStyle(style);
    setParagraphStyle(style);
    setUnappliedStyleChanges(true);
}

void StyleManager::addCharacterStyle(KoCharacterStyle *style)
{
    widget.characterStylePage->setStyleManager(m_styleManager); // updates style combos
    m_characterStylesModel->addStyle(style);
    setCharacterStyle(style);
    setUnappliedStyleChanges(true);
}

void StyleManager::removeParagraphStyle(KoParagraphStyle *style)
{
    if (m_modifiedParagraphStyles.contains(style)) {
        m_modifiedParagraphStyles.remove(style);
        m_paragraphStylesModel->removeStyle(style);
    }
    widget.paragraphStylePage->setStyleManager(m_styleManager); // updates style combos
}

void StyleManager::removeCharacterStyle(KoCharacterStyle *style)
{
    if (m_modifiedCharacterStyles.contains(style)) {
        m_modifiedCharacterStyles.remove(style);
        m_characterStylesModel->removeStyle(style);
    }
    widget.characterStylePage->setStyleManager(m_styleManager); // updates style combos
}

void StyleManager::slotParagraphStyleSelected(const QModelIndex &index)
{
    if (checkUniqueStyleName()) {
        KoParagraphStyle *paragraphStyle =
            dynamic_cast<KoParagraphStyle *>(m_paragraphProxyModel->data(index, StylesManagerModel::StylePointer).value<KoCharacterStyle *>());
        if (paragraphStyle) {
            setParagraphStyle(paragraphStyle);
            return;
        }
    }
}

void StyleManager::slotCharacterStyleSelected(const QModelIndex &index)
{
    if (checkUniqueStyleName()) {
        KoCharacterStyle *characterStyle = m_characterProxyModel->data(index, StylesManagerModel::StylePointer).value<KoCharacterStyle *>();
        if (characterStyle) {
            setCharacterStyle(characterStyle, false);
            return;
        }
    }
}

void StyleManager::buttonNewPressed()
{
    if (checkUniqueStyleName()) {
        if (widget.tabs->indexOf(widget.paragraphStylesListView) == widget.tabs->currentIndex()) {
            KoParagraphStyle *newStyle = nullptr;
            KoParagraphStyle *style = dynamic_cast<KoParagraphStyle *>(
                m_paragraphProxyModel->data(widget.paragraphStylesListView->currentIndex(), StylesManagerModel::StylePointer).value<KoCharacterStyle *>());
            if (style) {
                newStyle = style->clone();
            } else {
                newStyle = new KoParagraphStyle();
            }
            newStyle->setName(i18n("New Style"));
            m_modifiedParagraphStyles.insert(newStyle, 0);
            addParagraphStyle(newStyle);
            widget.paragraphStylePage->selectName();
        } else {
            KoCharacterStyle *newStyle = nullptr;
            KoCharacterStyle *style =
                m_characterProxyModel->data(widget.characterStylesListView->currentIndex(), StylesManagerModel::StylePointer).value<KoCharacterStyle *>();
            if (style) {
                newStyle = style->clone();
            } else {
                newStyle = new KoCharacterStyle();
            }
            newStyle->setName(i18n("New Style"));
            m_modifiedCharacterStyles.insert(newStyle, 0);
            addCharacterStyle(newStyle);
            widget.characterStylePage->selectName();
        }
    }
}

void StyleManager::tabChanged(int index)
{
    int paragraphIndex = widget.tabs->indexOf(widget.paragraphStylesListView);
    if (!checkUniqueStyleName(paragraphIndex == index ? widget.tabs->indexOf(widget.characterStylesListView) : paragraphIndex)) {
        // this is needed to not call tab changed during the resetting of the tab as this leads to en endless recursion.
        disconnect(widget.tabs, &QTabWidget::currentChanged, this, &StyleManager::tabChanged);
        if (widget.tabs->indexOf(widget.paragraphStylesListView) == widget.tabs->currentIndex()) {
            widget.tabs->setCurrentIndex(widget.tabs->indexOf(widget.characterStylesListView));
        } else {
            widget.tabs->setCurrentIndex(widget.tabs->indexOf(widget.paragraphStylesListView));
        }
        connect(widget.tabs, &QTabWidget::currentChanged, this, &StyleManager::tabChanged);
    } else {
        if (paragraphIndex == index) {
            KoParagraphStyle *style = dynamic_cast<KoParagraphStyle *>(
                m_paragraphProxyModel->data(widget.paragraphStylesListView->currentIndex(), StylesManagerModel::StylePointer).value<KoCharacterStyle *>());
            setParagraphStyle(style);
            widget.stackedWidget->setCurrentWidget(widget.paragraphStylePage);
        } else {
            KoCharacterStyle *style =
                m_characterProxyModel->data(widget.characterStylesListView->currentIndex(), StylesManagerModel::StylePointer).value<KoCharacterStyle *>();
            setCharacterStyle(style);
            widget.stackedWidget->setCurrentWidget(widget.characterStylePage);
        }
    }
}

bool StyleManager::unappliedStyleChanges() const
{
    return m_unappliedStyleChanges;
}

void StyleManager::setUnappliedStyleChanges(bool unappliedStyleChanges)
{
    if (m_unappliedStyleChanges == unappliedStyleChanges) {
        return;
    }
    m_unappliedStyleChanges = unappliedStyleChanges;
    Q_EMIT unappliedStyleChangesChanged(m_unappliedStyleChanges);
}

bool StyleManager::checkUniqueStyleName()
{
    return checkUniqueStyleName(widget.tabs->currentIndex());
}

bool StyleManager::checkUniqueStyleName(int widgetIndex)
{
    QModelIndex index;
    QString styleName;
    QListView *listView;
    if (widget.tabs->indexOf(widget.paragraphStylesListView) == widgetIndex) {
        styleName = widget.paragraphStylePage->styleName();
        listView = widget.paragraphStylesListView;
        index = m_paragraphProxyModel->mapFromSource(m_paragraphStylesModel->styleIndex(widget.paragraphStylePage->style()));
    } else {
        styleName = widget.characterStylePage->styleName();
        index = m_characterProxyModel->mapFromSource(m_characterStylesModel->styleIndex(widget.characterStylePage->style()));
        listView = widget.characterStylesListView;
    }

    QModelIndexList stylesByName;
    if (index.isValid()) {
        stylesByName.append(m_paragraphProxyModel->match(m_paragraphProxyModel->index(0, 0),
                                                         Qt::DisplayRole,
                                                         QVariant(styleName),
                                                         -1,
                                                         Qt::MatchFlags(Qt::MatchExactly | Qt::MatchWrap)));
        stylesByName.append(m_characterProxyModel->match(m_characterProxyModel->index(0, 0),
                                                         Qt::DisplayRole,
                                                         QVariant(styleName),
                                                         -1,
                                                         Qt::MatchFlags(Qt::MatchExactly | Qt::MatchWrap)));
    }

    bool unique = stylesByName.size() <= 1;
    if (!unique) {
        QMessageBox::critical(this, i18n("Warning"), i18n("Another style named '%1' already exist. Please choose another name.", styleName));
        listView->setCurrentIndex(index);
        if (widget.tabs->indexOf(widget.paragraphStylesListView) == widgetIndex) {
            widget.paragraphStylePage->selectName();
        } else {
            widget.characterStylePage->selectName();
        }
    }
    return unique;
}

QDialogButtonBox *StyleManager::buttonBox() const
{
    return widget.buttonBox;
}
