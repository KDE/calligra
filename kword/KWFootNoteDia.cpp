/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "KWFootNoteDia.h"
#include "KWFootNoteDia.moc"

#include <q3buttongroup.h>
#include <kvbox.h>
#include <QRadioButton>
//Added by qt3to4:
#include <Q3GridLayout>

#include <klocale.h>
#include <QLayout>
#include <QLineEdit>
#include "KWConfigFootNoteDia.h"


/******************************************************************/
/* Class: KWFootNoteDia                                           */
/******************************************************************/

KWFootNoteDia::KWFootNoteDia( NoteType _noteType, KWFootNoteVariable::Numbering _numberingType, const QString & _manualString, QWidget *parent, KWDocument *_doc, const char *name )
    : KDialogBase( parent, name, true, QString::null, Ok|Cancel|User1, Ok, true )
{
    m_doc =_doc;
    //setButtonOK(i18n("&Insert"));

    setCaption( i18n("Insert Footnote/Endnote") );

    KVBox *page = makeVBoxMainWidget();

    Q3ButtonGroup *grp = new Q3ButtonGroup( i18n("Numbering"), page );
    Q3GridLayout *grid = new Q3GridLayout( grp, 9, 4, KDialog::marginHint(), KDialog::spacingHint());
    m_rbAuto = new QRadioButton( i18n("&Automatic"), grp );
    m_rbManual= new QRadioButton( i18n("&Manual"), grp );

    grp->setExclusive( true );
    int fHeight = grp->fontMetrics().height();
    grid->addRowSpacing( 0, fHeight/2 ); // groupbox title
    grid->addWidget( m_rbAuto, 1, 0);
    grid->addWidget( m_rbManual, 2, 0);
    if ( _numberingType == KWFootNoteVariable::Auto )
        m_rbAuto->setChecked( true );
    else
        m_rbManual->setChecked( true );
    m_footLine = new QLineEdit( grp);
    m_footLine->setText( _manualString );
    connect( m_footLine, SIGNAL( textChanged ( const QString & )), this, SLOT(footLineChanged( const QString & )));
    connect( grp, SIGNAL( clicked ( int ) ), this, SLOT(footNoteTypeChanged()));
    grid->addWidget( m_footLine, 2, 1);


    grp = new Q3ButtonGroup( 4, Qt::Vertical, page );
    m_rbFootNote = new QRadioButton( i18n("&Footnote"), grp );
    m_rbEndNote = new QRadioButton( i18n("&Endnote"), grp );

    grp->setExclusive( true );
    grp->insert( m_rbFootNote );
    grp->insert( m_rbEndNote );
    if (_noteType == FootNote )
        m_rbFootNote->setChecked( true );
    else
        m_rbEndNote->setChecked( true );

    footNoteTypeChanged();
    setButtonText( KDialogBase::User1, i18n("C&onfigure...") );
    connect( this, SIGNAL( user1Clicked() ), this, SLOT(slotConfigurate()));
}

void KWFootNoteDia::footNoteTypeChanged()
{
    if ( m_rbManual->isChecked())
    {
        enableButtonOK( !m_footLine->text().isEmpty() );
        m_footLine->setFocus();
    }
    else
    {
        enableButtonOK(true);
        setFocus();
    }
}

void KWFootNoteDia::footLineChanged( const QString &text )
{
    m_rbManual->setChecked( true );
    if ( text.isEmpty() || footNoteAlreadyExists(text) )
        enableButtonOK( false );
    else
        enableButtonOK( true );
}

NoteType KWFootNoteDia::noteType() const
{
    return m_rbFootNote->isChecked() ? FootNote : EndNote;
}

KWFootNoteVariable::Numbering KWFootNoteDia::numberingType()const
{
    return m_rbAuto->isChecked() ? KWFootNoteVariable::Auto : KWFootNoteVariable::Manual;
}

QString KWFootNoteDia::manualString()const
{
    return  m_rbAuto->isChecked() ? QString::null : m_footLine->text();
}

void KWFootNoteDia::slotConfigurate()
{
    KWConfigFootNoteDia *dia = new KWConfigFootNoteDia( this, "configfootnote", m_doc );
    dia->exec();
    delete dia;
}

bool KWFootNoteDia::footNoteAlreadyExists( const QString & text )
{
    return manualFootNotes.contains( text );
}

void KWFootNoteDia::appendManualFootNote( const QString & text )
{
    manualFootNotes.append( text );
}
