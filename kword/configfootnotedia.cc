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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kwdoc.h"
#include "configfootnotedia.h"
#include "configfootnotedia.moc"

#include <klocale.h>
#include <qradiobutton.h>
#include <knuminput.h>
#include <qvbuttongroup.h>
#include <qlabel.h>
#include <qvbox.h>
#include <koParagDia.h>
#include <kwvariable.h>
#include "kwcommand.h"
#include <qcombobox.h>

//Laurent disable it because for the moment
//end note is not implemented
//#define END_NOTE

KWConfigFootNoteDia::KWConfigFootNoteDia( QWidget *parent, const char *name, KWDocument *_doc )
    : KDialogBase(Tabbed, QString::null, Ok | Cancel , Ok, parent, name, true)
{
    setCaption( i18n("Configure Endnote/Footnote") );
    m_doc = _doc;

    resize( 510, 310 );
    setupTab1();
#ifdef END_NOTE
    setupTab2();
#endif
    setupTab3();
}

void KWConfigFootNoteDia::setupTab1()
{
    QVBox * page = addVBoxPage( i18n( "Footnotes" ) );
    m_footNoteConfig = new KoCounterStyleWidget( false, true, false, page );
    //m_footNoteConfig->numTypeChanged( KoParagCounter::NUM_LIST );
    m_footNoteConfig->setCounter (static_cast<KWVariableSettings*>(m_doc->getVariableCollection()->variableSetting())->footNoteCounter() );
}

void KWConfigFootNoteDia::setupTab2()
{
    QVBox * page = addVBoxPage( i18n( "Endnotes" ) );
    m_endNoteConfig = new KoCounterStyleWidget( false, true/*onlyStyleLetter*/,false, page );
    //m_endNoteConfig->numTypeChanged( KoParagCounter::NUM_LIST );
    m_endNoteConfig->setCounter (static_cast<KWVariableSettings*>(m_doc->getVariableCollection()->variableSetting())->endNoteCounter() );
}


void KWConfigFootNoteDia::setupTab3()
{
    QFrame * page = addPage( i18n( "Separator Line" ) );
    QVBoxLayout *pageLayout= new QVBoxLayout( page, 11, 6);

    // use this when the message freeze is lifted.
    //QButtonGroup *positionGroupBox = new QButtonGroup( i18n( "Position"), page );
    QButtonGroup *positionGroupBox = new QButtonGroup( i18n( "Separator Line Position"), page );
    positionGroupBox->setColumnLayout(0, Qt::Vertical );
    QVBoxLayout *positionLayout = new QVBoxLayout( positionGroupBox->layout() );
    positionLayout->setAlignment( Qt::AlignTop );

    // i18n context information necessary for gender considerations (e.g. in Polish)
    rbPosLeft = new QRadioButton( i18n("Position", "Left"), positionGroupBox, "rbPosLeft" );
    positionLayout->addWidget( rbPosLeft );

    rbPosCentered = new QRadioButton( i18n("Position", "Centered"), positionGroupBox, "rbPosCentered" );
    positionLayout->addWidget( rbPosCentered );

    rbPosRight = new QRadioButton( i18n("Position", "Right"), positionGroupBox, "rbPosRight" );
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

    QGridLayout *layout = new QGridLayout( 0, 1, 1, 0, 6);

    spWidth = new KDoubleNumInput( 1, page );
    spWidth->setRange( 0, 5, 0.5 ,false );
    spWidth->setValue( m_doc->footNoteSeparatorLineWidth());
    layout->addWidget( spWidth, 1, 1 );

    spLength = new KIntNumInput( page, "spLength" );
    spLength->setRange( 1, 100, 1,false );
    spLength->setValue( m_doc->footNoteSeparatorLineLength());
    spLength->setSuffix(i18n(" %"));
    layout->addWidget( spLength, 0, 1 );

    // use this when the message freeze is lifted.
    //QLabel *lSize = new QLabel( i18n("arg is a unit such as pt", "&Width (%1)").arg("pt"), page );
    //lSize->setBuddy( spWidth );
    QLabel *lSize = new QLabel( i18n("Separator line width (%1):").arg(QString("pt")), page );
    layout->addWidget( lSize, 1, 0 );

    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout->addMultiCell( spacer, 0, 1, 2, 2 );

    // use this when the message freeze is lifted.
    //QLabel *lWidth = new QLabel( i18n("&Size on page:"), page, "lWidth" );
    //lWidth->setBuddy( spLength );
    QLabel *lWidth = new QLabel( i18n("Separator line length:"), page, "lWidth" );
    layout->addWidget( lWidth, 0, 0 );
    pageLayout->addLayout( layout );

    QHBoxLayout *styleLayout = new QHBoxLayout( 0, 0, 6, "styleLayout");

    // use this when the message freeze is lifted.
    //QLabel *styleLabel = new QLabel( i18n("Style:"), page );
    QLabel *styleLabel = new QLabel( i18n("Separator line type:"), page );
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
    KoParagCounter counter =static_cast<KWVariableSettings*>(m_doc->getVariableCollection()->variableSetting())->footNoteCounter();
    if (counter != m_footNoteConfig->counter() )
    {
        macro = new KMacroCommand(i18n("Change End-/Footnote Variable Settings"));
        cmd= new KWChangeFootEndNoteSettingsCommand( i18n("Change End-/Footnote Variable Settings") , counter, m_footNoteConfig->counter() ,true ,m_doc);
        macro->addCommand(cmd );
    }
#ifdef END_NOTE
    counter = static_cast<KWVariableSettings*>(m_doc->getVariableCollection()->variableSetting())->endNoteCounter();
    if (counter != m_endNoteConfig->counter() )
    {
        if ( !macro )
            macro = new KMacroCommand(i18n("Change End-/Footnote Variable Settings"));
        cmd= new KWChangeFootEndNoteSettingsCommand( i18n("Change End-/Footnote Variable Settings") , counter, m_endNoteConfig->counter() ,false ,m_doc);
        macro->addCommand(cmd );
    }
#endif

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
