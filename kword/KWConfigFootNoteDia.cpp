/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KWDocument.h"
#include "KWConfigFootNoteDia.h"
#include "KWConfigFootNoteDia.moc"
#include "KWVariable.h"
#include "KWCommand.h"
#include "KWFrameSet.h"

#include <KoParagDia.h>
#include <KoUnitWidgets.h>

#include <klocale.h>
#include <knuminput.h>

#include <QRadioButton>

#include <QLabel>
#include <kvbox.h>
#include <QLayout>
#include <QComboBox>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3Frame>


KWConfigFootNoteDia::KWConfigFootNoteDia( QWidget *parent, const char *name, KWDocument *doc )
    : KPageDialog(parent)
{
    setButtons( Ok | Cancel );
    setDefaultButton( Ok );
    setFaceType( KPageDialog::Tabbed );
    setCaption( i18n("Configure Endnote/Footnote") );
    m_doc = doc;

    resize( 510, 310 );
    setupTab1();
    setupTab2();
    setupTab3();
}

void KWConfigFootNoteDia::setupTab1()
{
    KVBox * page = new KVBox();
    addPage( page, i18n( "Footnotes" ) );
    m_footNoteConfig = new KoCounterStyleWidget( false, true, false, page );
    //m_footNoteConfig->numTypeChanged( KoParagCounter::NUM_LIST );
    m_footNoteConfig->setCounter (static_cast<KWVariableSettings*>(m_doc->variableCollection()->variableSetting())->footNoteCounter() );
}

void KWConfigFootNoteDia::setupTab2()
{
    KVBox * page = new KVBox();
    addPage(page, i18n( "Endnotes" ) );
    m_endNoteConfig = new KoCounterStyleWidget( false, true/*onlyStyleLetter*/,false, page );
    //m_endNoteConfig->numTypeChanged( KoParagCounter::NUM_LIST );
    m_endNoteConfig->setCounter (static_cast<KWVariableSettings*>(m_doc->variableCollection()->variableSetting())->endNoteCounter() );
}


void KWConfigFootNoteDia::setupTab3()
{
    QFrame * page = new QFrame();
    addPage( page, i18n( "Separator Line" ) );
    QString comment= i18n ("Configure the separator line. The separator line "
        "is drawn directly above the frame for the footnotes.");
    page->setWhatsThis( comment);
    Q3VBoxLayout *pageLayout= new Q3VBoxLayout( page, 0, KDialog::spacingHint());

    QGroupBox *positionGroupBox = new QGroupBox( i18n( "Position"), page );
    comment= i18n ("The separator can be positioned horizontally by picking one "
        "of the three alignments.");
    positionGroupBox->setWhatsThis( comment);
    QVBoxLayout *positionLayout = new QVBoxLayout( positionGroupBox );
    positionLayout->setAlignment( Qt::AlignTop );

    // i18n context information necessary for gender considerations (e.g. in Polish)
    rbPosLeft = new QRadioButton( i18nc("Position", "Left"), positionGroupBox );
    rbPosLeft->setObjectName( "rbPosLeft" );
    positionLayout->addWidget( rbPosLeft );

    rbPosCentered = new QRadioButton( i18nc("Position", "Centered"), positionGroupBox );
    rbPosCentered->setObjectName( "rbPosCentered" );
    positionLayout->addWidget( rbPosCentered );

    rbPosRight = new QRadioButton( i18nc("Position", "Right"), positionGroupBox );
    rbPosRight->setObjectName( "rbPosRight" );
    positionLayout->addWidget( rbPosRight );
    pageLayout->addWidget( positionGroupBox );
    switch( m_doc->footNoteSeparatorLinePosition() ) {
        case SLP_LEFT:
            rbPosLeft->setChecked( true);
            break;
        case SLP_CENTERED:
            rbPosCentered->setChecked( true);
            break;
        case SLP_RIGHT:
            rbPosRight->setChecked( true);
            break;
    }

    Q3GridLayout *layout = new Q3GridLayout( 0, 1, 1, 0, KDialog::spacingHint());

    spWidth = new KoUnitDoubleSpinBox(page, 0, 5, 0.5, m_doc->footNoteSeparatorLineWidth(), m_doc->unit(), 1);

    layout->addWidget( spWidth, 1, 1 );

    spLength = new KIntNumInput( page);
	spLength->setObjectName( "spLength" );
    spLength->setRange( 1, 100, 1,false );
    spLength->setValue( m_doc->footNoteSeparatorLineLength());
    spLength->setSuffix(i18n(" %"));
    layout->addWidget( spLength, 0, 1 );

    QLabel *lSize = new QLabel( i18n( "&Width:"), page );
    lSize->setBuddy( spWidth );
    comment= i18n ("The width is the thickness of the separator line, set to 0 for no separator line.");
    lSize->setWhatsThis( comment);
    spWidth->setWhatsThis( comment);
    layout->addWidget( lSize, 1, 0 );

    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout->addMultiCell( spacer, 0, 1, 2, 2 );

    QLabel *lWidth = new QLabel( i18n("&Size on page:"), page, "lWidth" );
    lWidth->setBuddy( spLength );
    comment= i18n ("The size of the separator line can be defined as a percentage of the width of the page.");
    spLength->setWhatsThis( comment);
    lWidth->setWhatsThis( comment);
    layout->addWidget( lWidth, 0, 0 );
    pageLayout->addLayout( layout );

    Q3HBoxLayout *styleLayout = new Q3HBoxLayout( 0, 0, KDialog::spacingHint(), "styleLayout");

    QLabel *styleLabel = new QLabel( i18n("Style:"), page );
    styleLayout->addWidget( styleLabel );

    m_cbLineType = new QComboBox( page );
    QStringList lst;
    lst <<i18n("Solid");
    lst <<i18n("Dash Line");
    lst <<i18n("Dot Line");
    lst <<i18n("Dash Dot Line");
    lst <<i18n("Dash Dot Dot Line");
    m_cbLineType->insertStringList( lst );
    m_cbLineType->setCurrentItem( static_cast<int>(m_doc->footNoteSeparatorLineType()));
    comment= i18n ("The separator line can be drawn as a solid line or as a line which has a "
        "pattern; the pattern can be set in the style-type.");
    styleLabel->setWhatsThis( comment);
    m_cbLineType->setWhatsThis( comment);
    styleLayout->addWidget( m_cbLineType );

    QSpacerItem* spacer_2 = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    styleLayout->addItem( spacer_2 );
    pageLayout->addLayout( styleLayout );
    QSpacerItem* spacer_3 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
    pageLayout->addItem( spacer_3 );
}


void KWConfigFootNoteDia::slotOk()
{
    KMacroCommand * macro = 0L;
    KCommand *cmd = 0L;
    KoParagCounter counter =static_cast<KWVariableSettings*>(m_doc->variableCollection()->variableSetting())->footNoteCounter();
    if (counter != m_footNoteConfig->counter() )
    {
        macro = new KMacroCommand(i18n("Change End-/Footnote Variable Settings"));
        cmd= new KWChangeFootEndNoteSettingsCommand( i18n("Change End-/Footnote Variable Settings") , counter, m_footNoteConfig->counter() ,true ,m_doc);
        macro->addCommand(cmd );
    }
    counter = static_cast<KWVariableSettings*>(m_doc->variableCollection()->variableSetting())->endNoteCounter();
    if (counter != m_endNoteConfig->counter() )
    {
        if ( !macro )
            macro = new KMacroCommand(i18n("Change End-/Footnote Variable Settings"));
        cmd= new KWChangeFootEndNoteSettingsCommand( i18n("Change End-/Footnote Variable Settings") , counter, m_endNoteConfig->counter() ,false ,m_doc);
        macro->addCommand(cmd );
    }

    int val =spLength->value();
    double width = spWidth->value();
    SeparatorLinePos tmp = SLP_LEFT;
    SeparatorLineLineType type = static_cast<SeparatorLineLineType>(m_cbLineType->currentItem());
    if ( rbPosRight->isChecked())
        tmp = SLP_RIGHT;
    else if ( rbPosCentered->isChecked())
        tmp = SLP_CENTERED;
    else if ( rbPosLeft->isChecked())
        tmp = SLP_LEFT;

    if ( (val != m_doc->footNoteSeparatorLineLength())||
         tmp != m_doc->footNoteSeparatorLinePosition()||
         width!= m_doc->footNoteSeparatorLineWidth() ||
         type != m_doc->footNoteSeparatorLineType())
    {
        if ( !macro )
            macro = new KMacroCommand(i18n("Change Footnote Line Separator Settings"));
        cmd = new KWChangeFootNoteLineSeparatorParametersCommand( i18n("Change Footnote Line Separator Settings") , m_doc->footNoteSeparatorLinePosition(), tmp, m_doc->footNoteSeparatorLineLength(), val,m_doc->footNoteSeparatorLineWidth(), width, m_doc->footNoteSeparatorLineType(), type, m_doc);
        macro->addCommand( cmd );
    }

   if ( macro )
    {
        macro->execute();
        m_doc->addCommand( macro );
    }

    KDialogBase::slotOk();
}
