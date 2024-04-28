/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001, 2002 Montel Laurent <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
   SPDX-FileCopyrightText: 2008 Pierre Stirnweiss <pstirnweiss@googlemail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FontDia.h"
#include "CharacterHighlighting.h"
#include "FontDecorations.h"

#include "CharacterGeneral.h"

#include "FormattingPreview.h"

#include <KoTextEditor.h>

#include <KLocalizedString>

#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFragment>

FontDia::FontDia(KoTextEditor *editor, QWidget *parent)
    : KoDialog(parent)
    , m_editor(editor)
    , m_styleChanged(false)
{
    m_initialFormat = m_editor->charFormat();

    setCaption(i18n("Select Font"));
    setModal(true);
    setButtons(Ok | Cancel | Reset | Apply);
    setDefaultButton(Ok);

    m_characterGeneral = new CharacterGeneral(this);
    m_characterGeneral->hideStyleName(true);
    setMainWidget(m_characterGeneral);

    connect(this, &KoDialog::applyClicked, this, &FontDia::slotApply);
    connect(this, &KoDialog::okClicked, this, &FontDia::slotOk);
    connect(this, &KoDialog::resetClicked, this, &FontDia::slotReset);
    initTabs();

    // Do this after initTabs so it doesn't cause signals prematurely
    connect(m_characterGeneral, &CharacterGeneral::styleChanged, this, [this]() {
        styleChanged();
    });
}

void FontDia::initTabs()
{
    KoCharacterStyle style(m_initialFormat);
    m_characterGeneral->setStyle(&style, true);
}

void FontDia::styleChanged(bool state)
{
    m_styleChanged = state;
}

void FontDia::slotApply()
{
    if (!m_styleChanged)
        return;

    m_editor->beginEditBlock(kundo2_i18n("Font"));
    KoCharacterStyle chosenStyle;
    m_characterGeneral->save(&chosenStyle);
    QTextCharFormat cformat;
    chosenStyle.applyStyle(cformat);
    m_editor->mergeAutoStyle(cformat);
    m_editor->endEditBlock();

    m_styleChanged = false;
}

void FontDia::slotOk()
{
    slotApply();
    KoDialog::accept();
}

void FontDia::slotReset()
{
    initTabs();
    slotApply(); // ### Should reset() apply?
}
