/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
 * Copyright (C) 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 * Copyright (C) 2012 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
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

#include "CharacterGeneral.h"
#include "CharacterHighlighting.h"
#include "LanguageTab.h"
#include "FontDecorations.h"
#include "FormattingPreview.h"

#include "StylesCombo.h"
#include "StylesModel.h"
#include "ValidParentStylesProxyModel.h"

#include <KoParagraphStyle.h>
#include <KoStyleThumbnailer.h>
#include <KoStyleManager.h>
#include <KoCharacterStyle.h>

#include "kdebug.h"

CharacterGeneral::CharacterGeneral(QWidget *parent)
        : QWidget(parent),
        m_paragraphStyleModel(new StylesModel(0,StylesModel::ParagraphStyle)),
        m_validParentStylesModel(new ValidParentStylesProxyModel()),
        m_style(0),
        m_styleManager(0),
        m_thumbnail(new KoStyleThumbnailer()),
        m_characterStyleModel(new StylesModel(0, StylesModel::CharacterStyle))
//        , m_characterInheritedStyleModel(new StylesModel(0, StylesModel::CharacterStyle))
{
    widget.setupUi(this);
    // we dont have next style for character styles
    widget.nextStyle->setVisible(false);
    m_characterStyleModel->setStyleThumbnailer(m_thumbnail);
    widget.nextStyle->setStylesModel(m_characterStyleModel);
    widget.label_2->setVisible(false);
    //

    // inherited style model
    widget.inheritStyle->showEditIcon(false);
    widget.inheritStyle->setStyleIsOriginal(true);
    //for character General
//    m_characterInheritedStyleModel->setStyleThumbnailer(m_thumbnail);
//    m_validParentStylesModel->setStyleThumbnailer(m_thumbnail);
//    m_validParentStylesModel->setStylesModel(m_characterStyleModel);
//    widget.inheritStyle->setStylesModel(m_validParentStylesModel);

    m_characterHighlighting = new CharacterHighlighting(true, this);
    connect(m_characterHighlighting, SIGNAL(charStyleChanged()), this, SIGNAL(styleChanged()));
    connect(m_characterHighlighting, SIGNAL(charStyleChanged()), this, SLOT(setPreviewCharacterStyle()));

    m_languageTab = new LanguageTab(true, this);

    widget.tabs->addTab(m_characterHighlighting, i18n("Font"));

    m_languageTab->setVisible(false);

    connect(widget.name, SIGNAL(textChanged(const QString &)), this, SIGNAL(nameChanged(const QString&)));
}

void CharacterGeneral::hideStyleName(bool hide)
{
    if (hide) {
        disconnect(widget.name, SIGNAL(textChanged(const QString &)), this, SIGNAL(nameChanged(const QString&)));
        widget.tabs->removeTab(0);
        m_nameHidden = true;
    }
}

void CharacterGeneral::setStyle(KoCharacterStyle *style)
{
    kDebug() << "in characterGeneral updateStyle. model: " << m_validParentStylesModel;
    m_style = style;
    if (m_style == 0)
        return;
    blockSignals(true);
/*
    //filter m_inherited style model

    //
    //m_characterInheritedStyleModel->clearStyleModel(); This Line has problem, a weird problem.
    //

    foreach(KoCharacterStyle *s, m_styleManager->characterStyles()) {
        KoCharacterStyle *parent = s;
        bool ok = true;
        while (ok && parent) {
            ok = parent->styleId() != m_style->styleId();

            parent = parent->parentStyle();
        }
        if (! ok) continue; // can't inherit from itself, even indirectly.
        m_characterInheritedStyleModel->addCharacterStyle(s);
        if (s == m_style->parent())
             widget.inheritStyle->setCurrentIndex(m_characterInheritedStyleModel->indexForCharacterStyle(*m_style).row());
    }
*/
    m_validParentStylesModel->setCurrentChildStyleId(style->styleId());
//    if(!m_style->parentStyle())
//        widget.inheritStyle->setCurrentIndex(-1);

    if (!m_nameHidden)
        widget.name->setText(style->name());

    m_characterHighlighting->setDisplay(style);
    //m_languageTab->setDisplay(style);

    widget.preview->setCharacterStyle(style);

    if (m_styleManager) {
        KoCharacterStyle *parentStyle = style->parentStyle();
        if (parentStyle) {
//            widget.inheritStyle->setCurrentIndex(m_validParentStylesModel->indexForCharacterStyle(*parentStyle).row());
        }
    }

    blockSignals(false);
}

void CharacterGeneral::save(KoCharacterStyle *style)
{
    KoCharacterStyle *savingStyle;
    if (style == 0) {
        if (m_style == 0)
            return;
        else
            savingStyle = m_style;
    }
    else
        savingStyle = style;

    m_characterHighlighting->save(savingStyle);
    //m_languageTab->save(savingStyle);
    savingStyle->setName(widget.name->text());
/*    if (widget.inheritStyle->currentIndex() != -1) {
        KoCharacterStyle *parent = m_styleManager->characterStyle(m_validParentStylesModel->index(widget.inheritStyle->currentIndex(), 0, QModelIndex()).internalId());
        savingStyle->setParentStyle(parent);
    }
*/
    if (m_style == savingStyle) {
        emit styleAltered(savingStyle);
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
    m_characterStyleModel->setStyleManager(m_styleManager);
    m_paragraphStyleModel->setStyleManager(m_styleManager);
    m_validParentStylesModel->setStyleManager(m_styleManager);
}

void CharacterGeneral::updateNextStyleCombo(KoParagraphStyle *style)
{
    widget.nextStyle->setCurrentIndex(m_paragraphStyleModel->indexForParagraphStyle(*style).row());
    m_paragraphStyleModel->setCurrentParagraphStyle(style->styleId());
}

int CharacterGeneral::nextStyleId()
{
    return m_styleManager->paragraphStyle(m_paragraphStyleModel->index(widget.nextStyle->currentIndex()).internalId())->styleId();
}

#include <CharacterGeneral.moc>
