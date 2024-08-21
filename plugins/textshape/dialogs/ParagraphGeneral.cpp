/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 * SPDX-FileCopyrightText: 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ParagraphGeneral.h"
#include "ParagraphBulletsNumbers.h"
#include "ParagraphDecorations.h"
#include "ParagraphDropCaps.h"
#include "ParagraphIndentSpacing.h"
#include "ParagraphLayout.h"
#include "StylesModel.h"

#include <KoParagraphStyle.h>
#include <KoStyleManager.h>
#include <KoStyleThumbnailer.h>

ParagraphGeneral::ParagraphGeneral(QWidget *parent)
    : CharacterGeneral(parent)
    , m_nameHidden(false)
    , m_style(nullptr)
    , m_styleManager(nullptr)
    , m_thumbnail(new KoStyleThumbnailer())
    , m_paragraphInheritedStyleModel(new StylesModel(nullptr, StylesModel::ParagraphStyle))
{
    // Disable for now
    // include in TOC
    widget.inToc->setVisible(false);
    //
    widget.nextStyle->setVisible(true);
    widget.label_2->setVisible(true);

    m_paragraphInheritedStyleModel->setStyleThumbnailer(m_thumbnail);
    widget.inheritStyle->setStylesModel(m_paragraphInheritedStyleModel);

    m_paragraphIndentSpacing = new ParagraphIndentSpacing(this);
    widget.tabs->addTab(m_paragraphIndentSpacing, i18n("Indent/Spacing"));

    connect(m_paragraphIndentSpacing, &ParagraphIndentSpacing::parStyleChanged, this, &CharacterGeneral::styleChanged);

    m_paragraphLayout = new ParagraphLayout(this);
    widget.tabs->addTab(m_paragraphLayout, i18n("General Layout"));

    connect(m_paragraphLayout, &ParagraphLayout::parStyleChanged, this, &CharacterGeneral::styleChanged);

    m_paragraphBulletsNumbers = new ParagraphBulletsNumbers(this);
    widget.tabs->addTab(m_paragraphBulletsNumbers, i18n("Bullets/Numbers"));

    connect(m_paragraphBulletsNumbers, &ParagraphBulletsNumbers::parStyleChanged, this, &CharacterGeneral::styleChanged);

    m_paragraphDecorations = new ParagraphDecorations(this);
    widget.tabs->addTab(m_paragraphDecorations, i18n("Decorations"));

    connect(m_paragraphDecorations, &ParagraphDecorations::parStyleChanged, this, &CharacterGeneral::styleChanged);

    m_paragraphDropCaps = new ParagraphDropCaps(this);
    widget.tabs->addTab(m_paragraphDropCaps, i18n("Drop Caps"));

    connect(m_paragraphDropCaps, &ParagraphDropCaps::parStyleChanged, this, &CharacterGeneral::styleChanged);

    widget.preview->setText(QString(
        "Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat."));

    connect(widget.name, &QLineEdit::textChanged, this, QOverload<const QString &>::of(&ParagraphGeneral::nameChanged));
    connect(widget.nextStyle, QOverload<int>::of(&StylesCombo::currentIndexChanged), this, &ParagraphGeneral::styleChanged);

    connect(this, &CharacterGeneral::styleChanged, this, &ParagraphGeneral::setPreviewParagraphStyle);
}

void ParagraphGeneral::hideStyleName(bool hide)
{
    if (hide) {
        disconnect(widget.name, &QLineEdit::textChanged, this, QOverload<const QString &>::of(&ParagraphGeneral::nameChanged));
        widget.tabs->removeTab(0);
        m_nameHidden = true;
    }
}

void ParagraphGeneral::selectName()
{
    widget.tabs->setCurrentIndex(widget.tabs->indexOf(widget.generalTab));
    widget.name->selectAll();
    widget.name->setFocus(Qt::OtherFocusReason);
}

void ParagraphGeneral::setStyle(KoParagraphStyle *style, int level, bool directFormattingMode)
{
    m_style = style;
    if (m_style == nullptr)
        return;

    CharacterGeneral::setStyle(style, directFormattingMode);

    blockSignals(true);

    /*    widget.inheritStyle->clear();
        widget.inheritStyle->addItem(i18nc("Inherit style", "None"));
        widget.inheritStyle->setCurrentIndex(0);
        foreach(KoParagraphStyle *s, m_paragraphStyles) {
            KoParagraphStyle *parent = s;
            bool ok = true;
            while (ok && parent) {
                ok = parent->styleId() != style->styleId();
                parent = parent->parentStyle();
            }
            if (! ok) continue; // can't inherit from myself, even indirectly.

            widget.inheritStyle->addItem(s->name(), s->styleId());
            if (s == style->parent())
                widget.inheritStyle->setCurrentIndex(widget.inheritStyle->count() - 1);
        }
    */
    if (!m_nameHidden)
        widget.name->setText(style->name());

    if (m_styleManager) {
        CharacterGeneral::updateNextStyleCombo(m_styleManager->paragraphStyle(style->nextStyle()));
        KoParagraphStyle *parentStyle = style->parentStyle();
        if (parentStyle) {
            widget.inheritStyle->setCurrentIndex(m_paragraphInheritedStyleModel->indexOf(parentStyle).row());
            // m_paragraphInheritedStyleModel->setCurrentParagraphStyle(parentStyle->styleId());
        }
    }

    m_paragraphIndentSpacing->setDisplay(style, directFormattingMode);
    m_paragraphLayout->setDisplay(style, directFormattingMode);
    m_paragraphBulletsNumbers->setDisplay(style, level, directFormattingMode);
    m_paragraphDecorations->setDisplay(style, directFormattingMode);
    m_paragraphDropCaps->setDisplay(style, directFormattingMode);

    widget.preview->setParagraphStyle(style);

    blockSignals(false);
}

void ParagraphGeneral::setUnit(const KoUnit &unit)
{
    m_paragraphIndentSpacing->setUnit(unit);
    m_paragraphDropCaps->setUnit(unit);
}

void ParagraphGeneral::save(KoParagraphStyle *style)
{
    KoParagraphStyle *savingStyle;

    if (style == nullptr) {
        if (m_style == nullptr)
            return;
        else
            savingStyle = m_style;
    } else
        savingStyle = style;

    CharacterGeneral::save(style);

    m_paragraphIndentSpacing->save(savingStyle);
    m_paragraphLayout->save(savingStyle);
    m_paragraphBulletsNumbers->save(savingStyle);
    m_paragraphDecorations->save(savingStyle);
    m_paragraphDropCaps->save(savingStyle);
    savingStyle->setName(widget.name->text());
    if (int nextStyleId = CharacterGeneral::nextStyleId()) {
        savingStyle->setNextStyle(nextStyleId);
    }

    if (m_style == savingStyle) {
        Q_EMIT styleAltered(savingStyle);
    }
}

void ParagraphGeneral::switchToGeneralTab()
{
    widget.tabs->setCurrentIndex(0);
}

void ParagraphGeneral::setPreviewParagraphStyle()
{
    KoParagraphStyle *parStyle = new KoParagraphStyle();
    save(parStyle);
    if (parStyle) {
        widget.preview->setParagraphStyle(parStyle);
    }

    delete parStyle;
}

void ParagraphGeneral::setImageCollection(KoImageCollection *imageCollection)
{
    m_paragraphBulletsNumbers->setImageCollection(imageCollection);
}

QString ParagraphGeneral::styleName() const
{
    return widget.name->text();
}

void ParagraphGeneral::setStyleManager(KoStyleManager *sm)
{
    if (!sm)
        return;
    m_styleManager = sm;
    CharacterGeneral::setStyleManager(m_styleManager);
    m_paragraphInheritedStyleModel->setStyleManager(m_styleManager);
}

KoParagraphStyle *ParagraphGeneral::style() const
{
    return m_style;
}
