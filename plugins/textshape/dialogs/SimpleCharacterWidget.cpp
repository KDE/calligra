/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2008, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009-2010 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2011-2012 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SimpleCharacterWidget.h"
#include "../commands/ChangeListCommand.h"
#include "DockerStylesComboModel.h"
#include "StylesDelegate.h"
#include "StylesModel.h"
#include "TextTool.h"
#include <KoStyleThumbnailer.h>

#include <KoCharacterStyle.h>
#include <KoInlineTextObjectManager.h>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>
#include <KoTextBlockData.h>
#include <KoTextDocumentLayout.h>
#include <KoZoomHandler.h>
#include <QAction>
#include <kselectaction.h>

#include <QDebug>

#include <QComboBox>
#include <QTextLayout>

SimpleCharacterWidget::SimpleCharacterWidget(TextTool *tool, QWidget *parent)
    : QWidget(parent)
    , m_styleManager(nullptr)
    , m_blockSignals(false)
    , m_comboboxHasBidiItems(false)
    , m_tool(tool)
    , m_thumbnailer(new KoStyleThumbnailer())
    , m_stylesModel(new StylesModel(nullptr, StylesModel::CharacterStyle))
    , m_sortedStylesModel(new DockerStylesComboModel())
    , m_stylesDelegate(nullptr)
{
    widget.setupUi(this);
    widget.bold->setDefaultAction(tool->action("format_bold"));
    widget.italic->setDefaultAction(tool->action("format_italic"));
    widget.strikeOut->setDefaultAction(tool->action("format_strike"));
    widget.underline->setDefaultAction(tool->action("format_underline"));
    widget.textColor->setDefaultAction(tool->action("format_textcolor"));
    widget.backgroundColor->setDefaultAction(tool->action("format_backgroundcolor"));
    widget.superscript->setDefaultAction(tool->action("format_super"));
    widget.subscript->setDefaultAction(tool->action("format_sub"));
    widget.moreOptions->setText("...");
    widget.moreOptions->setToolTip(i18n("Change font format"));
    connect(widget.moreOptions, &QAbstractButton::clicked, tool->action("format_font"), &QAction::trigger);

    connect(widget.bold, &QAbstractButton::clicked, this, &SimpleCharacterWidget::doneWithFocus);
    connect(widget.italic, &QAbstractButton::clicked, this, &SimpleCharacterWidget::doneWithFocus);
    connect(widget.strikeOut, &QAbstractButton::clicked, this, &SimpleCharacterWidget::doneWithFocus);
    connect(widget.underline, &QAbstractButton::clicked, this, &SimpleCharacterWidget::doneWithFocus);
    connect(widget.textColor, &QAbstractButton::clicked, this, &SimpleCharacterWidget::doneWithFocus);
    connect(widget.backgroundColor, &QAbstractButton::clicked, this, &SimpleCharacterWidget::doneWithFocus);
    connect(widget.superscript, &QAbstractButton::clicked, this, &SimpleCharacterWidget::doneWithFocus);
    connect(widget.subscript, &QAbstractButton::clicked, this, &SimpleCharacterWidget::doneWithFocus);

    QWidgetAction *fontFamilyAction = qobject_cast<QWidgetAction *>(tool->action("format_fontfamily"));
    QComboBox *family = fontFamilyAction ? qobject_cast<QComboBox *>(fontFamilyAction->requestWidget(this)) : 0;
    if (family) { // kdelibs 4.1 didn't return anything here.
        widget.fontsFrame->addWidget(family, 0, 0);
        connect(family, &QComboBox::activated, this, &SimpleCharacterWidget::doneWithFocus);
        connect(family, &QComboBox::activated, this, &SimpleCharacterWidget::fontFamilyActivated);
    }
    QWidgetAction *fontSizeAction = qobject_cast<QWidgetAction *>(tool->action("format_fontsize"));
    QComboBox *size = fontSizeAction ? qobject_cast<QComboBox *>(fontSizeAction->requestWidget(this)) : 0;
    if (size) { // kdelibs 4.1 didn't return anything here.
        widget.fontsFrame->addWidget(size, 0, 1);
        connect(size, &QComboBox::activated, this, &SimpleCharacterWidget::doneWithFocus);
        connect(size, &QComboBox::activated, this, &SimpleCharacterWidget::fontSizeActivated);
        QDoubleValidator *validator = new QDoubleValidator(2, 999, 1, size);
        size->setValidator(validator);
    }

    widget.fontsFrame->setColumnStretch(0, 1);

    m_stylesModel->setStyleThumbnailer(m_thumbnailer);
    widget.characterStyleCombo->setStylesModel(m_sortedStylesModel);
    connect(widget.characterStyleCombo,
            QOverload<const QModelIndex &>::of(&StylesCombo::selected),
            this,
            QOverload<const QModelIndex &>::of(&SimpleCharacterWidget::styleSelected));
    connect(widget.characterStyleCombo, &StylesCombo::newStyleRequested, this, &SimpleCharacterWidget::newStyleRequested);
    connect(widget.characterStyleCombo, &StylesCombo::newStyleRequested, this, &SimpleCharacterWidget::doneWithFocus);
    connect(widget.characterStyleCombo, &StylesCombo::showStyleManager, this, &SimpleCharacterWidget::slotShowStyleManager);

    m_sortedStylesModel->setStylesModel(m_stylesModel);
}

SimpleCharacterWidget::~SimpleCharacterWidget()
{
    // the model is set on the comboBox which takes ownership
    delete m_thumbnailer;
}

void SimpleCharacterWidget::setStyleManager(KoStyleManager *sm)
{
    Q_ASSERT(sm);
    if (!sm || m_styleManager == sm) {
        return;
    }
    if (m_styleManager) {
        disconnect(m_styleManager, &KoStyleManager::characterStyleApplied, this, &SimpleCharacterWidget::slotCharacterStyleApplied);
    }
    m_styleManager = sm;
    // we want to disconnect this before setting the stylemanager. Populating the model apparently selects the first inserted item. We don't want this to
    // actually set a new style.
    disconnect(widget.characterStyleCombo,
               QOverload<const QModelIndex &>::of(&StylesCombo::selected),
               this,
               QOverload<const QModelIndex &>::of(&SimpleCharacterWidget::styleSelected));
    m_stylesModel->setStyleManager(sm);
    m_sortedStylesModel->setStyleManager(sm);
    connect(widget.characterStyleCombo,
            QOverload<const QModelIndex &>::of(&StylesCombo::selected),
            this,
            QOverload<const QModelIndex &>::of(&SimpleCharacterWidget::styleSelected));
    connect(m_styleManager, &KoStyleManager::characterStyleApplied, this, &SimpleCharacterWidget::slotCharacterStyleApplied);
}

void SimpleCharacterWidget::setInitialUsedStyles(QVector<int> list)
{
    m_sortedStylesModel->setInitialUsedStyles(list);
}

void SimpleCharacterWidget::setCurrentFormat(const QTextCharFormat &format, const QTextCharFormat &refBlockCharFormat)
{
    if (!m_styleManager || format == m_currentCharFormat) {
        return;
    }
    m_currentCharFormat = format;

    KoCharacterStyle *style(m_styleManager->characterStyle(m_currentCharFormat.intProperty(KoCharacterStyle::StyleId)));
    bool useParagraphStyle = false;
    if (!style) {
        style = static_cast<KoCharacterStyle *>(m_styleManager->paragraphStyle(m_currentCharFormat.intProperty(KoParagraphStyle::StyleId)));
        useParagraphStyle = true;
    }
    if (style) {
        bool unchanged = true;
        QTextCharFormat comparisonFormat = refBlockCharFormat;
        style->applyStyle(comparisonFormat);
        // Here we are making quite a few assumptions:
        // i. we can set the "ensured" properties on a blank charFormat. These corresponds to Qt default. We are not creating false positive (ie. different
        // styles showing as identical). ii. a property whose toBool returns as false is identical to an unset property (this is done through the
        // clearUnsetProperties method)
        style->ensureMinimalProperties(comparisonFormat);
        style->ensureMinimalProperties(m_currentCharFormat);
        clearUnsetProperties(comparisonFormat);
        clearUnsetProperties(m_currentCharFormat);
        if (m_currentCharFormat.properties().count() != comparisonFormat.properties().count()) {
            unchanged = false;
        } else {
            foreach (int property, m_currentCharFormat.properties().keys()) {
                if (m_currentCharFormat.property(property) != comparisonFormat.property(property)) {
                    unchanged = false;
                }
            }
        }
        disconnect(widget.characterStyleCombo,
                   QOverload<const QModelIndex &>::of(&StylesCombo::selected),
                   this,
                   QOverload<const QModelIndex &>::of(&SimpleCharacterWidget::styleSelected));
        // TODO, this is very brittle index 1 is because index 0 is the title. The proper solution to that would be for the "None" style to have a styleId which
        // does not get applied on the text, but can be used in the ui
        widget.characterStyleCombo->setCurrentIndex((useParagraphStyle) ? 1 : m_sortedStylesModel->indexOf(style).row());
        widget.characterStyleCombo->setStyleIsOriginal(unchanged);
        widget.characterStyleCombo->slotUpdatePreview();
        connect(widget.characterStyleCombo,
                QOverload<const QModelIndex &>::of(&StylesCombo::selected),
                this,
                QOverload<const QModelIndex &>::of(&SimpleCharacterWidget::styleSelected));
    }
}

void SimpleCharacterWidget::clearUnsetProperties(QTextFormat &format)
{
    foreach (int property, format.properties().keys()) {
        if (!format.property(property).toBool()) {
            format.clearProperty(property);
        }
    }
}

void SimpleCharacterWidget::fontFamilyActivated(int index)
{
    /**
     * Hack:
     *
     * Selecting a font that is already selected in the combobox
     * will not trigger the action, so we help it on the way by
     * manually triggering it here if that happens.
     */
    if (index == m_lastFontFamilyIndex) {
        KSelectAction *action = qobject_cast<KSelectAction *>(m_tool->action("format_fontfamily"));
        if (action->currentAction())
            action->currentAction()->trigger();
    }
    m_lastFontFamilyIndex = index;
}

void SimpleCharacterWidget::fontSizeActivated(int index)
{
    /**
     * Hack:
     *
     * Selecting a font size that is already selected in the
     * combobox will not trigger the action, so we help it on
     * the way by manually triggering it here if that happens.
     */
    if (index == m_lastFontSizeIndex) {
        KSelectAction *action = qobject_cast<KSelectAction *>(m_tool->action("format_fontsize"));
        action->currentAction()->trigger();
    }
    m_lastFontSizeIndex = index;
}

void SimpleCharacterWidget::setCurrentBlockFormat(const QTextBlockFormat &format)
{
    if (format == m_currentBlockFormat)
        return;
    m_currentBlockFormat = format;

    m_stylesModel->setCurrentParagraphStyle(format.intProperty(KoParagraphStyle::StyleId));
    disconnect(widget.characterStyleCombo,
               QOverload<const QModelIndex &>::of(&StylesCombo::selected),
               this,
               QOverload<const QModelIndex &>::of(&SimpleCharacterWidget::styleSelected));
    widget.characterStyleCombo->slotUpdatePreview();
    connect(widget.characterStyleCombo,
            QOverload<const QModelIndex &>::of(&StylesCombo::selected),
            this,
            QOverload<const QModelIndex &>::of(&SimpleCharacterWidget::styleSelected));
}

void SimpleCharacterWidget::styleSelected(int index)
{
    KoCharacterStyle *charStyle = m_styleManager->characterStyle(m_sortedStylesModel->index(index, 0, QModelIndex()).internalId());

    // if the selected item correspond to a null characterStyle, send the null pointer. the tool should set the characterStyle as per paragraph
    Q_EMIT characterStyleSelected(charStyle);
    Q_EMIT doneWithFocus();
}

void SimpleCharacterWidget::styleSelected(const QModelIndex &index)
{
    if (!index.isValid()) {
        Q_EMIT doneWithFocus();
        return;
    }
    KoCharacterStyle *charStyle = m_styleManager->characterStyle(index.internalId());

    // if the selected item correspond to a null characterStyle, send the null pointer. the tool should set the characterStyle as per paragraph
    Q_EMIT characterStyleSelected(charStyle);
    Q_EMIT doneWithFocus();
}

void SimpleCharacterWidget::slotShowStyleManager(int index)
{
    int styleId = m_sortedStylesModel->index(index, 0, QModelIndex()).internalId();
    Q_EMIT showStyleManager(styleId);
    Q_EMIT doneWithFocus();
}

void SimpleCharacterWidget::slotCharacterStyleApplied(const KoCharacterStyle *style)
{
    m_sortedStylesModel->styleApplied(style);
}
