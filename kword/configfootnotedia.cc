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

KWConfigFootNoteDia::KWConfigFootNoteDia( QWidget *parent, const char *name, KWDocument *_doc )
    : KDialogBase(Tabbed, QString::null, Ok | Cancel , Ok, parent, name, true)
{
    setCaption( i18n("Configure Endnote/Footnote") );
    m_doc = _doc;

    setupTab1();
    setupTab2();
    setupTab3();
}

void KWConfigFootNoteDia::setupTab1()
{
    QVBox * page = addVBoxPage( i18n( "Footnotes" ) );
    m_footNoteConfig = new KoCounterStyleWidget( false, true, page );
    //m_footNoteConfig->numTypeChanged( KoParagCounter::NUM_LIST );
    m_footNoteConfig->setCounter (static_cast<KWVariableSettings*>(m_doc->getVariableCollection()->variableSetting())->footNoteCounter() );
}

void KWConfigFootNoteDia::setupTab2()
{
    QVBox * page = addVBoxPage( i18n( "Endnotes" ) );
    m_endNoteConfig = new KoCounterStyleWidget( false, true/*onlyStyleLetter*/, page );
    //m_endNoteConfig->numTypeChanged( KoParagCounter::NUM_LIST );
    m_endNoteConfig->setCounter (static_cast<KWVariableSettings*>(m_doc->getVariableCollection()->variableSetting())->endNoteCounter() );
}


void KWConfigFootNoteDia::setupTab3()
{
    QVBox * page = addVBoxPage( i18n( "Separator Line" ) );
    QVButtonGroup *bgSeparatorPosition = new QVButtonGroup( i18n( "Separator Line Position" ), page );
    rbPosLeft = new QRadioButton( i18n("Left"), bgSeparatorPosition);
    rbPosCentered = new QRadioButton( i18n("Centered"), bgSeparatorPosition);
    rbPosRight = new QRadioButton( i18n("Right"), bgSeparatorPosition);
    QVButtonGroup *bgSeparatorLength = new QVButtonGroup( i18n( "Separator Line Length" ), page );

    QLabel *lab = new QLabel(i18n("Length:"), bgSeparatorLength);

    spLength = new KIntNumInput( 1, bgSeparatorLength );
    spLength->setRange( 1, 100, 1,false );
    spLength->setValue( m_doc->footNoteSeparatorLineLength());
    spLength->setSuffix(i18n(" %"));
    switch( m_doc->footNoteSeparatorLinePosition() )
    {
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

    QVButtonGroup *bgSeparatorWidth = new QVButtonGroup( i18n( "Separator Line Width" ), page );

    lab = new QLabel(i18n("Width(pt):"), bgSeparatorWidth);
    spWidth = new KDoubleNumInput( 1, bgSeparatorWidth );
    spWidth->setRange( 0, 5, 0.5 ,false );
    spWidth->setValue( m_doc->footNoteSeparatorLineWidth());

    QVButtonGroup *bgSeparatorType = new QVButtonGroup( i18n( "Separator Line Type" ), page );

    lab = new QLabel(i18n("Type of Line:"), bgSeparatorType);
    m_cbLineType = new QComboBox( bgSeparatorType );
    QStringList lst;
    lst <<i18n("Solid");
    lst <<i18n("Dash Line");
    lst <<i18n("Dot Line");
    lst <<i18n("Dash Dot Line");
    lst <<i18n("Dash Dot Dot Line");
    m_cbLineType->insertStringList( lst );
    m_cbLineType->setCurrentItem( static_cast<int>(m_doc->footNoteSeparatorLineType()));
}


void KWConfigFootNoteDia::slotOk()
{
    KMacroCommand * macro = 0L;
    KCommand *cmd = 0L;
    KoParagCounter counter =static_cast<KWVariableSettings*>(m_doc->getVariableCollection()->variableSetting())->footNoteCounter();
    if (counter != m_footNoteConfig->counter() )
    {
        macro = new KMacroCommand(i18n("Change End-/Footnote Variable Settings"));
        cmd= new KWChangeFootEndNoteSettingsCommand( i18n("Change End-/FootNote Variable Settings") , counter, m_footNoteConfig->counter() ,true ,m_doc);
        macro->addCommand(cmd );
    }
    counter = static_cast<KWVariableSettings*>(m_doc->getVariableCollection()->variableSetting())->endNoteCounter();
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
            macro = new KMacroCommand(i18n("Change Foot Note Line Separator Settings"));
        cmd = new KWChangeFootNoteLineSeparatorParametersCommand( i18n("Change Foot Note Line Separator Settings") , m_doc->footNoteSeparatorLinePosition(), tmp, m_doc->footNoteSeparatorLineLength(), val,m_doc->footNoteSeparatorLineWidth(), width, m_doc->footNoteSeparatorLineType(), type, m_doc);
        macro->addCommand( cmd );
    }

   if ( macro )
    {
        macro->execute();
        m_doc->addCommand( macro );
    }

    KDialogBase::slotOk();
}
