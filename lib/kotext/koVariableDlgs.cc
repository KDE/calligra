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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koVariableDlgs.h"
#include "koVariableDlgs.moc"

#include <klocale.h>
#include <kbuttonbox.h>

#include <qcombobox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qheader.h>
#include <qlineedit.h>

/******************************************************************
 *
 * Class: KoVariableNameDia
 *
 ******************************************************************/

KoVariableNameDia::KoVariableNameDia( QWidget *parent )
    : KDialogBase( parent, "", TRUE,i18n( "Entry Name" ),Ok|Cancel )
{
    init();
}


KoVariableNameDia::KoVariableNameDia( QWidget *parent, const QPtrList<KoVariable>& vars )
    : KDialogBase( parent, "", TRUE, i18n( "Variable Name" ), Ok|Cancel )
{

    init();
    enableButtonOK(false);
    QPtrListIterator<KoVariable> it( vars );
     for ( ; it.current() ; ++it ) {
        KoVariable *var = it.current();
        if ( var->type() == VT_CUSTOM )
            names->insertItem( ( (KoCustomVariable*) var )->name(), -1 );
    }

}

void KoVariableNameDia::init()
{
    back = makeVBoxMainWidget();

    QHBox *row1 = new QHBox( back );
    row1->setSpacing( 5 );

    QLabel *l = new QLabel( i18n( "Name:" ), row1 );
    l->setFixedSize( l->sizeHint() );
    names = new QComboBox( TRUE, row1 );
    names->setFocus();

    connect( names, SIGNAL( textChanged ( const QString & )),
             this, SLOT( textChanged ( const QString & )));
    connect( this, SIGNAL( okClicked() ),
             this, SLOT( accept() ) );
    connect( this, SIGNAL( cancelClicked() ),
             this, SLOT( reject() ) );
    enableButtonOK( !names->currentText().isEmpty() );
    resize( 350, 100 );
}

QString KoVariableNameDia::getName() const
{
    return names->currentText();
}

void KoVariableNameDia::textChanged ( const QString &_text )
{
    enableButtonOK(!_text.isEmpty());
}

/******************************************************************
 *
 * Class: KoCustomVariablesListItem
 *
 ******************************************************************/

KoCustomVariablesListItem::KoCustomVariablesListItem( QListView *parent )
    : QListViewItem( parent )
{
    editWidget = new QLineEdit( listView()->viewport() );
    listView()->addChild( editWidget );
}

void KoCustomVariablesListItem::setup()
{
    setHeight( QMAX( listView()->fontMetrics().height(),
                     editWidget->sizeHint().height() ) );
    if ( listView()->columnWidth( 1 ) < editWidget->sizeHint().width() )
        listView()->setColumnWidth( 1, editWidget->sizeHint().width() );
}

void KoCustomVariablesListItem::update()
{
    editWidget->resize( listView()->header()->cellSize( 1 ), height() );
    listView()->moveChild( editWidget, listView()->header()->cellPos( 1 ),
                           listView()->itemPos( this ) + listView()->contentsY() );
    editWidget->show();
}

void KoCustomVariablesListItem::setVariable( KoCustomVariable *v )
{
    var = v;
    editWidget->setText( var->value() );
    setText( 0, v->name() );
}

KoCustomVariable *KoCustomVariablesListItem::getVariable() const
{
    return var;

}

void KoCustomVariablesListItem::applyValue()
{
    var->setValue( editWidget->text() );
}

/******************************************************************
 *
 * Class: KoCustomVariablesList
 *
 ******************************************************************/

KoCustomVariablesList::KoCustomVariablesList( QWidget *parent )
    : QListView( parent )
{
    header()->setMovingEnabled( FALSE );
    addColumn( i18n( "Variable" ) );
    addColumn( i18n( "Value" ) );
    connect( header(), SIGNAL( sizeChange( int, int, int ) ),
             this, SLOT( columnSizeChange( int, int, int ) ) );
    connect( header(), SIGNAL( sectionClicked( int ) ),
             this, SLOT( sectionClicked( int ) ) );
    setColumnWidthMode( 0, Manual );
    setColumnWidthMode( 1, Manual );

    setSorting( -1 );
}

void KoCustomVariablesList::setValues()
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it )
        ( (KoCustomVariablesListItem *)it.current() )->applyValue();
}

void KoCustomVariablesList::columnSizeChange( int c, int, int )
{
    if ( c == 0 || c == 1 )
        updateItems();
}

void KoCustomVariablesList::sectionClicked( int )
{
    updateItems();
}

void KoCustomVariablesList::updateItems()
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it )
        ( (KoCustomVariablesListItem*)it.current() )->update();
}

/******************************************************************
 *
 * Class: KoCustomVariablesDia
 *
 ******************************************************************/

KoCustomVariablesDia::KoCustomVariablesDia( QWidget *parent, const QPtrList<KoVariable> &variables )
    : QDialog( parent, "", TRUE )
{
    setCaption( i18n( "Variable Value Editor" ) );

    back = new QVBox( this );
    back->setSpacing( 5 );
    back->setMargin( 5 );

    list = new KoCustomVariablesList( back );

    QStringList lst;
    QPtrListIterator<KoVariable> it( variables );
    for ( ; it.current() ; ++it ) {
        KoVariable *var = it.current();
        if ( var->type() == VT_CUSTOM ) {
            KoCustomVariable *v = (KoCustomVariable*)var;
            if ( !lst.contains( v->name() ) ) {
                lst.append( v->name() );
                KoCustomVariablesListItem *item = new KoCustomVariablesListItem( list );
                item->setVariable( v );
            }
        }
    }

    KButtonBox *bb = new KButtonBox( back );
    bb->addStretch();
    QPushButton *ok = bb->addButton( i18n( "&OK"  ) );
    ok->setDefault( TRUE );
    QPushButton *cancel = bb->addButton( i18n( "&Cancel"  ) );
    bb->layout();

    connect( ok, SIGNAL( clicked() ),
             this, SLOT( slotOk() ) );
    connect( cancel, SIGNAL( clicked() ),
             this, SLOT( reject() ) );
    ok->setEnabled(lst.count()>0);

    resize( 600, 400 );
    list->updateItems();
}

void KoCustomVariablesDia::resizeEvent( QResizeEvent *e )
{
    QDialog::resizeEvent( e );
    back->resize( size() );
}

void KoCustomVariablesDia::slotOk()
{
    list->setValues();
    accept();
}
