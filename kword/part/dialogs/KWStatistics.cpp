/* This file is part of the KDE project
 * Copyright (C) 2005, 2007 Thomas Zander <zander@kde.org>
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

#include "KWStatistics.h"
#include "KWord.h"
#include "KWDocument.h"
#include "frames/KWFrameSet.h"
#include "frames/KWTextFrameSet.h"

#include <KoCanvasResourceProvider.h>
#include <KoExecutePolicy.h>
#include <KoAction.h>

#include <QTextDocument>
#include <QTextBlock>
#include <QTextLayout>

KWStatistics::KWStatistics(KoCanvasResourceProvider *provider, KWDocument* document, QWidget *parent)
    : QWidget(parent),
    m_resourceProvider(provider),
    m_action(new KoAction(this)),
    m_document(document)
{
    widget.setupUi(this);
    m_action->setExecutePolicy(KoExecutePolicy::onlyLastPolicy);

    connect(m_resourceProvider, SIGNAL(sigResourceChanged(int, const QVariant &)), this, SLOT(updateResource(int)));
    connect(m_action, SIGNAL(triggered(const QVariant&)), this, SLOT(updateData()), Qt::DirectConnection);
    connect(m_action, SIGNAL(updateUi(const QVariant&)), this, SLOT(updateDataUi()), Qt::DirectConnection);
    connect(widget.footEndNotes, SIGNAL(toggled(bool)), m_action, SLOT(execute()));

    m_action->execute();
}

void KWStatistics::updateResource(int which) {
    if(which == KWord::CurrentPageCount  || which == KWord::CurrentFrameSetCount ||
            which == KWord::CurrentPictureCount || which == KWord::CurrentTableCount)
        m_action->execute();
}

void KWStatistics::updateData() {
    m_charsWithSpace = 0;
    m_charsWithoutSpace = 0;
    m_words = 0;
    m_sentences = 0;
    m_lines = 0;
    m_syllables = 0;
    m_paragraphs = 0;
    foreach(KWFrameSet *fs, m_document->frameSets()) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*> (fs);
        if(tfs == 0) continue;
        if(! (widget.footEndNotes->isChecked() || (tfs->textFrameSetType() == KWord::MainTextFrameSet ||
                    tfs->textFrameSetType() == KWord::OtherTextFrameSet)))
            continue;
        QTextDocument *doc = tfs->document();
        QTextBlock block = doc->begin();
        while(block.isValid()) {
            m_paragraphs += 1;
            m_charsWithSpace += block.length();
            m_charsWithoutSpace += block.length() - block.text().count(QRegExp("\\s"));
            if(block.layout())
                m_lines += block.layout()->lineCount();
            // TODO dig code out of the old text framework that counted words, sentences and syllables
            block = block.next();
        }
    }
}

void KWStatistics::updateDataUi() {
    // tab 1
    widget.pages->setText(
            KGlobal::locale()->formatNumber( m_resourceProvider->intResource(KWord::CurrentPageCount), 0 ) );
    widget.frames->setText(
            KGlobal::locale()->formatNumber( m_resourceProvider->intResource(KWord::CurrentFrameSetCount), 0 ) );
    widget.pictures->setText(
            KGlobal::locale()->formatNumber( m_resourceProvider->intResource(KWord::CurrentPictureCount), 0 ) );
    widget.tables->setText(
            KGlobal::locale()->formatNumber( m_resourceProvider->intResource(KWord::CurrentTableCount), 0 ) );

    // tab 2
    widget.words->setText(KGlobal::locale()->formatNumber( m_words, 0 ));
    widget.sentences->setText(KGlobal::locale()->formatNumber( m_sentences, 0 ));
    widget.syllables->setText(KGlobal::locale()->formatNumber( m_syllables, 0 ));
    widget.lines->setText(KGlobal::locale()->formatNumber( m_lines, 0 ));
    widget.characters->setText(KGlobal::locale()->formatNumber( m_charsWithoutSpace, 0 ));
    widget.characters2->setText(KGlobal::locale()->formatNumber( m_charsWithSpace, 0 ));

    // calculate Flesch reading ease score:
    float flesch_score = 0;
    if( m_words > 0 && m_sentences > 0 )
        flesch_score = 206.835 - (1.015 * (m_words / m_sentences)) - (84.6 * m_syllables / m_words);
    QString flesch = KGlobal::locale()->formatNumber( flesch_score );
    if( m_words < 200 ) // a kind of warning if too few words:
        flesch = i18n("approximately %1", flesch );
    widget.flesch->setText(flesch);
}

#include <KWStatistics.moc>
