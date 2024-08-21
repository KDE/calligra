/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWFrameConnectSelector.h"
#include "KWDocument.h"
#include "Words.h"
#include "WordsDebug.h"
#include "frames/KWTextFrameSet.h"

KWFrameConnectSelector::KWFrameConnectSelector(FrameConfigSharedState *state)
    : m_state(state)
    , m_shape(nullptr)
{
    widget.setupUi(this);

    connect(widget.framesList, &QTreeWidget::itemClicked, this, &KWFrameConnectSelector::frameSetSelected);
    connect(widget.existingRadio, &QAbstractButton::clicked, this, &KWFrameConnectSelector::existingRadioClicked);
    connect(widget.frameSetName, &QLineEdit::textChanged, this, &KWFrameConnectSelector::nameChanged);
}

bool KWFrameConnectSelector::canOpen(KoShape *shape)
{
    if (shape->shapeId() != TextShape_SHAPEID) {
        // We should only go further if it's a textshape, ok if it is not in KWTextFrameSet
        return false;
    }

    if (KWFrameSet::from(shape)) { // already has a frameset
        KWTextFrameSet *textFs = static_cast<KWTextFrameSet *>(KWFrameSet::from(shape));
        if (textFs->textFrameSetType() != Words::OtherTextFrameSet)
            return false; // can't alter shapesequence of this auto-generated shape
    }

    return true;
}

void KWFrameConnectSelector::existingRadioClicked(bool on)
{
    // make sure there is a selected frameset
    if (on && !widget.framesList->currentItem() && widget.framesList->model()->rowCount() > 0) {
        QModelIndex curr = widget.framesList->model()->index(0, 0);
        widget.framesList->setCurrentIndex(curr);
        widget.framesList->selectionModel()->select(curr, QItemSelectionModel::Select);
    }
    debugWords << Q_FUNC_INFO << on << widget.framesList->currentItem();
}

void KWFrameConnectSelector::frameSetSelected()
{
    widget.existingRadio->setChecked(true);
}

void KWFrameConnectSelector::nameChanged(const QString &text)
{
    widget.newRadio->setChecked(true);
    foreach (QTreeWidgetItem *item, widget.framesList->selectedItems())
        item->setSelected(false);
    foreach (QTreeWidgetItem *item, m_items) {
        if (item->text(0) == text) {
            widget.framesList->setCurrentItem(item);
            return;
        }
    }
}

void KWFrameConnectSelector::save()
{
    Q_ASSERT(m_frameSets.count() == m_items.count());
    KWFrameSet *oldFS = KWFrameSet::from(m_shape);
    if (widget.newRadio->isChecked()) {
        KWTextFrameSet *newFS = new KWTextFrameSet(m_state->document());
        newFS->setName(widget.frameSetName->text());
        new KWFrame(m_shape, newFS);
        m_state->document()->addFrameSet(newFS);
    } else { // attach to (different) FS
        QTreeWidgetItem *selected = widget.framesList->currentItem();
        Q_ASSERT(selected);
        int index = m_items.indexOf(selected);
        Q_ASSERT(index >= 0);
        KWFrameSet *newFS = m_frameSets[index];
        if (oldFS != newFS) {
            new KWFrame(m_shape, newFS);
        }
    }
    m_state->removeUser();
}

void KWFrameConnectSelector::open(KoShape *shape)
{
    m_state->addUser();
    m_shape = shape;
    widget.framesList->clear();

    if (widget.frameSetName->text().isEmpty())
        widget.frameSetName->setText(m_state->document()->uniqueFrameSetName(i18n("frameset")));

    foreach (KWFrameSet *fs, m_state->document()->frameSets()) {
        KWTextFrameSet *textFs = dynamic_cast<KWTextFrameSet *>(fs);
        if (textFs == nullptr || textFs->textFrameSetType() != Words::OtherTextFrameSet)
            continue;
        m_frameSets.append(textFs);
        QTreeWidgetItem *row = new QTreeWidgetItem(widget.framesList);
        row->setText(0, textFs->name());
        if (KWFrameSet::from(m_shape) == fs)
            widget.framesList->setCurrentItem(row);
        m_items.append(row);
    }

    if (KWFrameSet::from(shape)) { // already has a frameset
        KWTextFrameSet *textFs = static_cast<KWTextFrameSet *>(KWFrameSet::from(shape));

        if (textFs->shapeCount() == 1) { // don't allow us to remove the last frame of an FS
            widget.newRadio->setEnabled(false);
            widget.frameSetName->setEnabled(false);
            widget.frameSetName->setText(textFs->name());
        }
        widget.existingRadio->setChecked(true);
    } else if (m_frameSets.count() == 0) { // no framesets on document
        QTreeWidgetItem *helpText = new QTreeWidgetItem(widget.framesList);
        helpText->setText(0, i18n("No framesets in document"));
        widget.framesList->setEnabled(false);
        widget.existingRadio->setEnabled(false);
        widget.newRadio->setChecked(true);
    } else {
        widget.newRadio->setChecked(true);
    }
}
