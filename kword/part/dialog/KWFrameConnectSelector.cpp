/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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

#include "KWFrameConnectSelector.h"
#include "KWDocument.h"
#include "frame/KWTextFrameSet.h"
#include "frame/KWTextFrame.h"

#include <QHeaderView>

KWFrameConnectSelector::KWFrameConnectSelector(FrameConfigSharedState *state)
    : m_state(state)
{
    m_state->addUser();
    widget.setupUi(this);

    connect (widget.m_framesList, SIGNAL( itemClicked(QTreeWidgetItem*, int) ),
            this, SLOT( frameSetSelected(QTreeWidgetItem*) ));
    connect (widget.m_frameSetName, SIGNAL( textChanged(const QString &) ),
            this, SLOT( nameChanged(const QString &)) );
}

KWFrameConnectSelector::~KWFrameConnectSelector() {
    m_state->removeUser();
}

bool KWFrameConnectSelector::open(KWFrame *frame) {
    KWTextFrame *textFrame = dynamic_cast<KWTextFrame*> (frame);
    if(textFrame == 0)
        return false;
    widget.m_framesList->clear();

    foreach(KWFrameSet *fs, m_state->document()->frameSets()) {
        KWTextFrameSet *textFs = dynamic_cast<KWTextFrameSet*> (fs);
        if(textFs == 0 || textFs->textFrameSetType() != KWord::OtherTextFrameSet)
            continue;
        m_frameSets.append(textFs);
        QTreeWidgetItem *row = new QTreeWidgetItem(widget.m_framesList);
        row->setText(0, textFs->name());
        if(frame->frameSet() == fs)
            widget.m_framesList->setCurrentItem(row);
        m_items.append(row);
    }

    if(textFrame->frameSet()) { // already has a frameset
        KWTextFrameSet *textFs = static_cast<KWTextFrameSet*> (textFrame->frameSet());
        if(textFs->textFrameSetType() != KWord::OtherTextFrameSet)
            return false; // can't alter frameSet of this auto-generated frame!

        if(textFs->frameCount() == 1) { // don't allow us to remove the last frame of an FS
            widget.m_newRadio->setEnabled(false);
            widget.m_frameSetName->setEnabled(false);
            widget.m_frameSetName->setText(textFs->name());
        }
        widget.m_existingRadio->setChecked(true);
    }
    else if(m_frameSets.count() == 0) { // no framesets on document
        QTreeWidgetItem *helpText = new QTreeWidgetItem(widget.m_framesList);
        helpText->setText(0, i18n("No framesets in document"));
        widget.m_framesList->setEnabled(false);
        widget.m_existingRadio->setEnabled(false);
        widget.m_newRadio->setChecked(true);
    }
    else {
        widget.m_newRadio->setChecked(true);
    }
    if(widget.m_frameSetName->text().isEmpty())
        widget.m_frameSetName->setText(m_state->document()->uniqueFrameSetName(i18n("frameset")));
    return true;
}

void KWFrameConnectSelector::frameSetSelected(QTreeWidgetItem* item) {
    widget.m_existingRadio->setChecked(true);
}

void KWFrameConnectSelector::nameChanged(const QString &text) {
    widget.m_newRadio->setChecked(true);
    foreach(QTreeWidgetItem *item, widget.m_framesList->selectedItems())
        widget.m_framesList->setItemSelected(item, false);
    foreach(QTreeWidgetItem *item, m_items) {
        if(item->text(0) == text) {
            widget.m_framesList->setCurrentItem(item);
            return;
        }
    }
}

void KWFrameConnectSelector::save() {
    Q_ASSERT(m_frameSets.count() == m_items.count());
    KWFrameSet *oldFS = m_state->frame()->frameSet();
    if(widget.m_newRadio->isChecked()) {
        KWTextFrameSet *newFS = new KWTextFrameSet();
        newFS->setName(widget.m_frameSetName->text());
        m_state->frame()->setFrameSet(newFS);
        m_state->document()->addFrameSet(newFS);
    }
    else { // attach to (different) FS
        QTreeWidgetItem *selected = widget.m_framesList->currentItem();
        Q_ASSERT(selected);
        int index = m_items.indexOf(selected);
        Q_ASSERT(index >= 0);
        KWFrameSet *newFS = m_frameSets[index];
        if(oldFS != newFS)
            m_state->frame()->setFrameSet(newFS);
    }
    if(oldFS && oldFS->frameCount() == 0) {
        // TODO
    }
}

void KWFrameConnectSelector::open(KoShape *shape) {
    KWFrame *frame = new KWTextFrame(shape, 0);
    m_state->setFrame(frame);
    open(frame);
}

KAction *KWFrameConnectSelector::createAction() {
    // TODO
    return 0;
}

#include "KWFrameConnectSelector.moc"
