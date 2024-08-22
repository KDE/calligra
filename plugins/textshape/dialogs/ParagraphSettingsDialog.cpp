/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ParagraphSettingsDialog.h"

#include "../TextTool.h"
#include "ParagraphGeneral.h"

#include <KoList.h>
#include <KoListLevelProperties.h>
#include <KoParagraphStyle.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <commands/ParagraphFormattingCommand.h>

#include <QTextBlock>
#include <QTimer>

ParagraphSettingsDialog::ParagraphSettingsDialog(TextTool *tool, KoTextEditor *editor, QWidget *parent)
    : KoDialog(parent)
    , m_tool(tool)
    , m_editor(editor)
    , m_styleChanged(false)
{
    setCaption(i18n("Paragraph Format"));
    setModal(true);
    setButtons(Ok | Cancel | Apply);
    setDefaultButton(Ok);

    m_paragraphGeneral = new ParagraphGeneral;
    m_paragraphGeneral->hideStyleName(true);
    setMainWidget(m_paragraphGeneral);

    connect(this, &KoDialog::applyClicked, this, &ParagraphSettingsDialog::slotApply);
    connect(this, &KoDialog::okClicked, this, &ParagraphSettingsDialog::slotOk);
    initTabs();

    // Do this after initTabs so it doesn't cause signals prematurely
    connect(m_paragraphGeneral, &ParagraphGeneral::styleChanged, this, [this]() {
        styleChanged();
    });
}

ParagraphSettingsDialog::~ParagraphSettingsDialog() = default;

void ParagraphSettingsDialog::initTabs()
{
    KoParagraphStyle *style = KoParagraphStyle::fromBlock(m_editor->block());
    m_paragraphGeneral->setStyle(style, KoList::level(m_editor->block()), true);
}

void ParagraphSettingsDialog::styleChanged(bool state)
{
    m_styleChanged = state;
}

void ParagraphSettingsDialog::slotOk()
{
    slotApply();
    KoDialog::accept();
}

void ParagraphSettingsDialog::slotApply()
{
    if (!m_styleChanged)
        return;

    KoParagraphStyle chosenStyle;
    m_paragraphGeneral->save(&chosenStyle);

    QTextCharFormat cformat;
    QTextBlockFormat format;
    chosenStyle.KoCharacterStyle::applyStyle(cformat);
    chosenStyle.applyStyle(format);

    KoListLevelProperties llp;
    if (chosenStyle.listStyle()) {
        llp = chosenStyle.listStyle()->levelProperties(chosenStyle.listStyle()->listLevels().first());
    } else {
        llp.setLabelType(KoListStyle::None);
    }

    m_editor->applyDirectFormatting(cformat, format, llp);

    m_styleChanged = false;
}

void ParagraphSettingsDialog::setUnit(const KoUnit &unit)
{
    m_paragraphGeneral->setUnit(unit);
}

void ParagraphSettingsDialog::setImageCollection(KoImageCollection *imageCollection)
{
    m_paragraphGeneral->setImageCollection(imageCollection);
}
