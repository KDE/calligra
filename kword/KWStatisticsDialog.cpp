/* This file is part of the KOffice project
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.

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

#include "KWStatisticsDialog.h"
#include "KWDocument.h"
#include "KWFrameSet.h"
#include <klocale.h>
#include <QTabWidget>
#include <q3vbox.h>
#include <QLabel>
#include <QLayout>
#include <q3progressdialog.h>
#include <QCheckBox>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <Q3Frame>
#include <Q3GroupBox>

KWStatisticsDialog::KWStatisticsDialog( QWidget *parent, KWDocument *document )
    : KDialog(parent)
{
    setCaption( i18n("Statistics") );
    setButtons( KDialog::Ok );
    setDefaultButton( KDialog::Ok );
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    Q3VBoxLayout *topLayout = new Q3VBoxLayout( page, 0, KDialog::spacingHint() );

    QTabWidget *tab = new QTabWidget( page );
    Q3Frame *pageAll = 0;
    Q3Frame *pageGeneral = 0;
    Q3Frame *pageSelected = 0;
    for (int i=0; i < 7; ++i) {
        m_resultLabelAll[i] = 0;
        m_resultLabelSelected[i] = 0;
        if ( i < 6 )
            m_resultGeneralLabel[i]=0;
    }
    m_doc = document;
    m_parent = parent;
    m_canceled = true;


    // add Tab "General"
    pageGeneral = new Q3Frame( this );
    tab->addTab( pageGeneral,  i18n( "General" ) );

    addBoxGeneral( pageGeneral, m_resultGeneralLabel );
    calcGeneral( m_resultGeneralLabel );

    // add Tab "All"
    pageAll = new Q3Frame( this );
    tab->addTab( pageAll,  i18n( "Text" ) );

    addBox( pageAll, m_resultLabelAll, true );

    m_canceled = true;
    pageSelected = new Q3Frame( this );
    tab->addTab( pageSelected,  i18n( "Selected Text" ) );
    // let's see if there's selected text
    bool b = docHasSelection();
    tab->setTabEnabled(pageSelected, b);
    if ( b ) {
        addBox( pageSelected, m_resultLabelSelected,  false);
        // assign results to 'selected' tab.
        if ( !calcStats( m_resultLabelSelected, true,true ) )
            return;
        if ( !calcStats( m_resultLabelAll, false,false ) )
            return;
        //showPage( 2 );
    } else {
        // assign results
        if ( !calcStats( m_resultLabelAll, false, false ) )
            return;
        //showPage( 1 );
    }
    topLayout->addWidget( tab );
    m_canceled = false;

}

void KWStatisticsDialog::slotRefreshValue(bool state)
{
    m_canceled = true;
    // let's see if there's selected text
    bool b = docHasSelection();
    if ( b )
    {
        if ( !calcStats( m_resultLabelSelected, true, true ) )
            return;
        if ( !calcStats( m_resultLabelAll, false, state ) )
            return;
    }
    else
    {
        // assign results
        if ( !calcStats( m_resultLabelAll, false, state ) )
            return;
    }
    m_canceled = false;
}

void KWStatisticsDialog::calcGeneral( QLabel **resultLabel )
{
    KLocale *locale = KGlobal::locale();

    resultLabel[0]->setText( locale->formatNumber( m_doc->pageCount(), 0) );
    int table =0;
    int picture = 0;
    int part = 0;
    int nbFrameset = 0;
    int nbFormula = 0;
    Q3PtrListIterator<KWFrameSet> framesetIt( m_doc->framesetsIterator() );
    for ( framesetIt.toFirst(); framesetIt.current(); ++framesetIt ) {
        KWFrameSet *frameSet = framesetIt.current();
        if ( frameSet && frameSet->isVisible())
        {
            if ( frameSet->type() == FT_TABLE)
                table++;
            else if ( frameSet->type() == FT_PICTURE)
                picture++;
            else if ( frameSet->type() == FT_PART )
                part++;
            else if ( frameSet->type() == FT_FORMULA )
                nbFormula++;
            nbFrameset++;
        }
    }

    resultLabel[1]->setText( locale->formatNumber( nbFrameset, 0 ) );
    resultLabel[2]->setText( locale->formatNumber( picture, 0 ) );
    resultLabel[3]->setText( locale->formatNumber( table, 0 ) );
    resultLabel[4]->setText( locale->formatNumber( part, 0 ) );
    resultLabel[5]->setText( locale->formatNumber( nbFormula, 0 ) );
}

bool KWStatisticsDialog::calcStats( QLabel **resultLabel, bool selection, bool useFootEndNote  )
{
    ulong charsWithSpace = 0L;
    ulong charsWithoutSpace = 0L;
    ulong words = 0L;
    ulong sentences = 0L;
    ulong lines = 0L;
    ulong syllables = 0L;

    // safety check result labels
    for (int i=0; i < 7; ++i) {
        if ( !resultLabel[i] ) {
            kDebug() << "Warning: KWStatisticsDiaolog::calcStats result table not initialized." << endl;
            return false;
        }
    }

    // count paragraphs for progress dialog:
    ulong paragraphs = 0L;
    Q3PtrListIterator<KWFrameSet> framesetIt( m_doc->framesetsIterator() );
    for ( framesetIt.toFirst(); framesetIt.current(); ++framesetIt ) {
        KWFrameSet *frameSet = framesetIt.current();
        if ( (frameSet->frameSetInfo() == KWFrameSet::FI_FOOTNOTE || frameSet->frameSetInfo() == KWFrameSet::FI_BODY) && frameSet->isVisible() )
        {
            if ( (useFootEndNote && frameSet->frameSetInfo() == KWFrameSet::FI_FOOTNOTE) ||
                    frameSet->frameSetInfo() == KWFrameSet::FI_BODY ) {
                paragraphs += frameSet->paragraphs();
            }

        }
    }
    Q3ProgressDialog progress( i18n( "Counting..." ), i18n( "Cancel" ), paragraphs, this, "count", true );
    progress.setMinimumDuration( 1000 );
    progress.setProgress( 0 );

    // do the actual counting
    for ( framesetIt.toFirst(); framesetIt.current(); ++framesetIt ) {
        KWFrameSet *frameSet = framesetIt.current();
        // Exclude headers and footers
        if ( (frameSet->frameSetInfo() == KWFrameSet::FI_FOOTNOTE || frameSet->frameSetInfo() == KWFrameSet::FI_BODY) && frameSet->isVisible() ) {
            if ( (useFootEndNote && frameSet->frameSetInfo() == KWFrameSet::FI_FOOTNOTE) || frameSet->frameSetInfo() == KWFrameSet::FI_BODY )
            {

                if( ! frameSet->statistics( &progress, charsWithSpace, charsWithoutSpace,
                                            words, sentences, syllables, lines, selection ) ) {
                    // someone pressed "Cancel"
                    return false;
                }
            }
        }
    }

    // assign results
    KLocale *locale = KGlobal::locale();
    resultLabel[0]->setText( locale->formatNumber( charsWithSpace, 0) );
    resultLabel[1]->setText( locale->formatNumber( charsWithoutSpace, 0 ) );
    resultLabel[2]->setText( locale->formatNumber( syllables, 0 ) );
    resultLabel[3]->setText( locale->formatNumber( words, 0 ) );
    resultLabel[4]->setText( locale->formatNumber( sentences, 0 ) );
    resultLabel[5]->setText( locale->formatNumber( lines, 0 ) );
    // add flesch
    double f = calcFlesch( sentences, words, syllables );
    QString flesch = locale->formatNumber( f , 1 );
    if( words < 200 ) {
        // a kind of warning if too few words:
        flesch = i18n("approximately %1", flesch );
    }
    resultLabel[6]->setText( flesch );
    return true;
}

double KWStatisticsDialog::calcFlesch( ulong sentences, ulong words, ulong syllables )
{
    // calculate Flesch reading ease score:
    float flesch_score = 0;
    if( words > 0 && sentences > 0 )
        flesch_score = 206.835 - (1.015 * (words / sentences)) - (84.6 * syllables / words);
    return flesch_score;
}

void KWStatisticsDialog::addBoxGeneral( Q3Frame *page, QLabel **resultLabel )
{
    // Layout Managers
    Q3VBoxLayout *topLayout = new Q3VBoxLayout( page, 0, 7 );
    Q3GroupBox *box = new Q3GroupBox( i18n( "Statistics" ), page );
    Q3GridLayout *grid = new Q3GridLayout( box, 9, 3, KDialog::marginHint(), KDialog::spacingHint() );
    grid->setRowStretch (9, 1);
    // margins
    int fHeight = box->fontMetrics().height();
    grid->setMargin( fHeight );
    grid->addColSpacing( 1, fHeight );
    grid->addRowSpacing( 0, fHeight );

    // insert labels
    QLabel *label1 = new QLabel( i18n( "Number of pages:" ), box );
    grid->addWidget( label1, 1, 0 );
    resultLabel[0] = new QLabel( "", box );
    grid->addWidget( resultLabel[0], 1, 2 );

    QLabel *label2 = new QLabel( i18n( "Number of frames:" ), box );
    grid->addWidget( label2, 2, 0 );
    resultLabel[1] = new QLabel( "", box );
    grid->addWidget( resultLabel[1], 2, 2 );

    QLabel *label3 = new QLabel( i18n( "Number of pictures:" ), box );
    grid->addWidget( label3, 3, 0 );
    resultLabel[2] = new QLabel( "", box );
    grid->addWidget( resultLabel[2], 3, 2 );


    QLabel *label4 = new QLabel( i18n( "Number of tables:" ), box );
    grid->addWidget( label4, 4, 0);
    resultLabel[3] = new QLabel( "", box );
    grid->addWidget( resultLabel[3], 4, 2 );

    QLabel *label5 = new QLabel( i18n( "Number of embedded objects:" ), box );
    grid->addWidget( label5, 5, 0);
    resultLabel[4] = new QLabel( "", box );
    grid->addWidget( resultLabel[4], 5, 2 );

    QLabel *label6 = new QLabel( i18n( "Number of formula frameset:" ), box );
    grid->addWidget( label6, 6, 0 );
    resultLabel[5] = new QLabel( "", box );
    grid->addWidget( resultLabel[5], 6, 2);

    topLayout->addWidget( box );
}

void KWStatisticsDialog::addBox( Q3Frame *page, QLabel **resultLabel, bool calcWithFootNoteCheckbox )
{
    // Layout Managers
    Q3VBoxLayout *topLayout = new Q3VBoxLayout( page, 0, 7 );
    if ( calcWithFootNoteCheckbox )
    {
        QWidget *w = new QWidget(page);
        topLayout->addWidget( w );
        Q3VBoxLayout *noteLayout = new Q3VBoxLayout( w, KDialog::marginHint(), 0 );
        QCheckBox *calcWithFootNote = new QCheckBox( i18n("&Include text from foot- and endnotes"), w);
        noteLayout->addWidget( calcWithFootNote );
        connect( calcWithFootNote, SIGNAL(toggled ( bool )), this, SLOT( slotRefreshValue(bool)));
    }


    Q3GroupBox *box = new Q3GroupBox( i18n( "Statistics" ), page );
    Q3GridLayout *grid = new Q3GridLayout( box, 9, 3, KDialog::marginHint(), KDialog::spacingHint() );
    grid->setRowStretch (9, 1);

    // margins
    int fHeight = box->fontMetrics().height();
    grid->setMargin( fHeight );
    grid->addColSpacing( 1, fHeight );
    grid->addRowSpacing( 0, fHeight );

    //maximum size for result column (don't know how to do this better..)
    QString init = i18n("approximately %1").arg( "00000000" );

    // insert labels
    QLabel *label1 = new QLabel( i18n( "Characters including spaces:" ), box );
    grid->addWidget( label1, 1, 0 );
    resultLabel[0] = new QLabel( "", box );
    grid->addWidget( resultLabel[0], 1, 2 );

    QLabel *label2 = new QLabel( i18n( "Characters without spaces:" ), box );
    grid->addWidget( label2, 2, 0 );
    resultLabel[1] = new QLabel( "", box );
    grid->addWidget( resultLabel[1], 2, 2 );

    QLabel *label3 = new QLabel( i18n( "Syllables:" ), box );
    grid->addWidget( label3, 3, 0 );
    resultLabel[2] = new QLabel( "", box );
    grid->addWidget( resultLabel[2], 3, 2 );

    QLabel *label4 = new QLabel( i18n( "Words:" ), box );
    grid->addWidget( label4, 4, 0 );
    resultLabel[3] = new QLabel( "", box );
    grid->addWidget( resultLabel[3], 4, 2 );

    QLabel *label5 = new QLabel( i18n( "Sentences:" ), box );
    grid->addWidget( label5, 5, 0 );
    resultLabel[4] = new QLabel( "", box );
    grid->addWidget( resultLabel[4], 5, 2 );

    QLabel *label6 = new QLabel( i18n( "Lines:" ), box );
    grid->addWidget( label6, 6, 0 );
    resultLabel[5] = new QLabel( "", box );
    grid->addWidget( resultLabel[5], 6, 2 );


    QLabel *label7 = new QLabel( i18n( "Flesch reading ease:" ), box );
    grid->addWidget( label7, 7, 0 );
    resultLabel[6] = new QLabel( init, box );
    grid->addWidget( resultLabel[6], 7, 2 );

    topLayout->addWidget( box );
}

bool KWStatisticsDialog::docHasSelection()const
{
    Q3PtrListIterator<KWFrameSet> fsIt( m_doc->framesetsIterator() );

    for ( ; fsIt.current(); ++fsIt ) {
        KWFrameSet *fs = fsIt.current();
        if ( fs->paragraphsSelected() ) {
            return true;
        }
    }
    return false;
}

#include "KWStatisticsDialog.moc"

