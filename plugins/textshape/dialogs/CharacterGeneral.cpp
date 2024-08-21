/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 * SPDX-FileCopyrightText: 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "CharacterGeneral.h"
#include "CharacterHighlighting.h"
#include "FontDecorations.h"
#include "FormattingPreview.h"
#include "LanguageTab.h"

#include "StylesCombo.h"
#include "StylesModel.h"

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>
#include <KoStyleThumbnailer.h>

#include <QDebug>

CharacterGeneral::CharacterGeneral(QWidget *parent)
    : QWidget(parent)
    , m_style(nullptr)
    , m_styleManager(nullptr)
    , m_thumbnail(new KoStyleThumbnailer())
    , m_paragraphStyleModel(new StylesModel(nullptr, StylesModel::ParagraphStyle))
    , m_characterInheritedStyleModel(new StylesModel(nullptr, StylesModel::CharacterStyle))
{
    widget.setupUi(this);
    widget.tabs->tabBar()->setExpanding(true);
    // we don't have next style for character styles
    widget.nextStyle->setVisible(false);
    widget.label_2->setVisible(false);
    //

    // paragraph style model
    widget.nextStyle->showEditIcon(false);
    widget.nextStyle->setStyleIsOriginal(true);
    m_paragraphStyleModel->setStyleThumbnailer(m_thumbnail);
    widget.nextStyle->setStylesModel(m_paragraphStyleModel);
    // inherited style model
    widget.inheritStyle->showEditIcon(false);
    widget.inheritStyle->setStyleIsOriginal(true);
    // for character General
    m_characterInheritedStyleModel->setStyleThumbnailer(m_thumbnail);
    widget.inheritStyle->setStylesModel(m_characterInheritedStyleModel);
    widget.inheritStyle->setEnabled(false);

    m_characterHighlighting = new CharacterHighlighting(this);
    connect(m_characterHighlighting, &CharacterHighlighting::charStyleChanged, this, &CharacterGeneral::styleChanged);
    connect(m_characterHighlighting, &CharacterHighlighting::charStyleChanged, this, &CharacterGeneral::setPreviewCharacterStyle);

    m_languageTab = new LanguageTab(true, this);

    widget.tabs->addTab(m_characterHighlighting, i18n("Font"));

    m_languageTab->setVisible(false);

    connect(widget.name, &QLineEdit::textChanged, this, &CharacterGeneral::nameChanged);
}

void CharacterGeneral::hideStyleName(bool hide)
{
    if (hide) {
        disconnect(widget.name, &QLineEdit::textChanged, this, &CharacterGeneral::nameChanged);
        widget.tabs->removeTab(0);
        m_nameHidden = true;
    }
}

void CharacterGeneral::setStyle(KoCharacterStyle *style, bool directFormattingMode)
{
    m_style = style;
    if (m_style == nullptr)
        return;
    blockSignals(true);

    if (!m_nameHidden)
        widget.name->setText(style->name());

    m_characterHighlighting->setDisplay(style, directFormattingMode);
    // m_languageTab->setDisplay(style);

    widget.preview->setCharacterStyle(style);

    if (m_styleManager) {
        KoCharacterStyle *parentStyle = style->parentStyle();
        if (parentStyle) {
            widget.inheritStyle->setCurrentIndex(m_characterInheritedStyleModel->indexOf(parentStyle).row());
        }
    }

    blockSignals(false);
}

void CharacterGeneral::save(KoCharacterStyle *style)
{
    KoCharacterStyle *savingStyle;
    if (style == nullptr) {
        if (m_style == nullptr)
            return;
        else
            savingStyle = m_style;
    } else
        savingStyle = style;

    m_characterHighlighting->save(savingStyle);
    // m_languageTab->save(savingStyle);
    savingStyle->setName(widget.name->text());

    if (m_style == savingStyle) {
        Q_EMIT styleAltered(savingStyle);
    }
}

void CharacterGeneral::switchToGeneralTab()
{
    widget.tabs->setCurrentIndex(0);
}

void CharacterGeneral::selectName()
{
    widget.tabs->setCurrentIndex(widget.tabs->indexOf(widget.generalTab));
    widget.name->selectAll();
    widget.name->setFocus(Qt::OtherFocusReason);
}

void CharacterGeneral::setPreviewCharacterStyle()
{
    KoCharacterStyle *charStyle = new KoCharacterStyle();
    save(charStyle);
    if (charStyle) {
        widget.preview->setCharacterStyle(charStyle);
    }

    delete charStyle;
}

QString CharacterGeneral::styleName() const
{
    return widget.name->text();
}

void CharacterGeneral::setStyleManager(KoStyleManager *sm)
{
    if (!sm)
        return;
    m_styleManager = sm;
    m_paragraphStyleModel->setStyleManager(m_styleManager);
    m_characterInheritedStyleModel->setStyleManager(m_styleManager);
}

void CharacterGeneral::updateNextStyleCombo(KoParagraphStyle *style)
{
    if (!style)
        return;

    widget.nextStyle->setCurrentIndex(m_paragraphStyleModel->indexOf(style).row());
    m_paragraphStyleModel->setCurrentParagraphStyle(style->styleId());
}

int CharacterGeneral::nextStyleId()
{
    if (!m_styleManager) {
        return 0;
    }
    int nextStyleIndex = widget.nextStyle->currentIndex();
    QModelIndex paragraphStyleIndex = m_paragraphStyleModel->index(nextStyleIndex);
    quint64 internalId = paragraphStyleIndex.internalId();
    KoParagraphStyle *paragraphStyle = m_styleManager->paragraphStyle(internalId);
    if (paragraphStyle) {
        return paragraphStyle->styleId();
    } else {
        return 0;
    }
}

KoCharacterStyle *CharacterGeneral::style() const
{
    return m_style;
}
