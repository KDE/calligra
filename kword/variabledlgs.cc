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

#include "variabledlgs.h"
#include "variabledlgs.moc"

#include <klocale.h>
#include <kbuttonbox.h>

#include <qcombobox.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qheader.h>
#include <qlineedit.h>
#include <qstringlist.h>

/******************************************************************
 *
 * Class: KWVariableNameDia
 *
 ******************************************************************/

/*================================================================*/
KWVariableNameDia::KWVariableNameDia( QWidget *parent, QList<KWVariable> *vars )
    : QDialog( parent, "", TRUE ), variables( vars )
{
    if ( variables )
        setCaption( i18n( "Variable Name" ) );
    else
        setCaption( i18n( "Entry Name" ) );

    back = new QVBox( this );
    back->setSpacing( 5 );
    back->setMargin( 5 );

    QHBox *row1 = new QHBox( back );
    row1->setSpacing( 5 );

    QLabel *l = new QLabel( i18n( "Name:" ), row1 );
    l->setFixedSize( l->sizeHint() );
    names = new QComboBox( TRUE, row1 );
    names->setFocus();

    if ( variables ) {
        KWVariable *var = 0;
        for ( var = variables->first(); var; var = variables->next() ) {
            if ( var->getType() == VT_CUSTOM )
                names->insertItem( ( (KWCustomVariable*) var )->getName(), -1 );
        }
    }

    KButtonBox *bb = new KButtonBox( back );
    bb->addStretch();
    QPushButton *ok = bb->addButton( i18n( "&OK"  ) );
    ok->setDefault( TRUE );
    QPushButton *cancel = bb->addButton( i18n( "&Cancel"  ) );
    bb->layout();

    connect( ok, SIGNAL( clicked() ),
             this, SLOT( accept() ) );
    connect( cancel, SIGNAL( clicked() ),
             this, SLOT( reject() ) );

    resize( 350, 100 );
}

/*================================================================*/
QString KWVariableNameDia::getName() const
{
    return names->currentText();
}

/*================================================================*/
void KWVariableNameDia::resizeEvent( QResizeEvent *e )
{
    QDialog::resizeEvent( e );
    back->resize( size() );
}

/******************************************************************
 *
 * Class: KWVariableValueListItem
 *
 ******************************************************************/

/*================================================================*/
KWVariableValueListItem::KWVariableValueListItem( QListView *parent )
    : QListViewItem( parent )
{
    editWidget = new QLineEdit( listView()->viewport() );
    listView()->addChild( editWidget );
}

/*================================================================*/
void KWVariableValueListItem::setup()
{
    setHeight( QMAX( listView()->fontMetrics().height(),
                     editWidget->sizeHint().height() ) );
    if ( listView()->columnWidth( 1 ) < editWidget->sizeHint().width() )
        listView()->setColumnWidth( 1, editWidget->sizeHint().width() );
}

/*================================================================*/
void KWVariableValueListItem::update()
{
    editWidget->resize( listView()->header()->cellSize( 1 ), height() );
    listView()->moveChild( editWidget, listView()->header()->cellPos( 1 ),
                           listView()->itemPos( this ) + listView()->contentsY() );
    editWidget->show();
}

/*================================================================*/
void KWVariableValueListItem::setVariable( KWCustomVariable *v )
{
    var = v;
    editWidget->setText( var->getValue() );
    setText( 0, v->getName() );
}

/*================================================================*/
KWCustomVariable *KWVariableValueListItem::getVariable() const
{
    return var;

}

/*================================================================*/
void KWVariableValueListItem::applyValue()
{
    var->setValue( editWidget->text() );
}

/******************************************************************
 *
 * Class: KWVariableValueList
 *
 ******************************************************************/

/*================================================================*/
KWVariableValueList::KWVariableValueList( QWidget *parent )
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

/*================================================================*/
void KWVariableValueList::setValues()
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it )
        ( (KWVariableValueListItem *)it.current() )->applyValue();
}

/*================================================================*/
void KWVariableValueList::columnSizeChange( int c, int, int )
{
    if ( c == 0 || c == 1 )
        updateItems();
}

/*================================================================*/
void KWVariableValueList::sectionClicked( int )
{
    updateItems();
}

/*================================================================*/
void KWVariableValueList::updateItems()
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it )
        ( (KWVariableValueListItem*)it.current() )->update();
}

/******************************************************************
 *
 * Class: KWVariableValueDia
 *
 ******************************************************************/

/*================================================================*/
KWVariableValueDia::KWVariableValueDia( QWidget *parent, QList<KWVariable> *vars )
    : QDialog( parent, "", TRUE ), variables( vars )
{
    setCaption( i18n( "Variable Value Editor" ) );

    back = new QVBox( this );
    back->setSpacing( 5 );
    back->setMargin( 5 );

    list = new KWVariableValueList( back );

    QStringList lst;
    KWVariable *var = 0;
    for ( var = variables->first(); var; var = variables->next() ) {
        if ( var->getType() == VT_CUSTOM ) {
            KWCustomVariable *v = (KWCustomVariable*)var;
            if ( !lst.contains( v->getName() ) ) {
                lst.append( v->getName() );
                KWVariableValueListItem *item = new KWVariableValueListItem( list );
                item->setVariable( v );
            }
        }
    }

    resize( 600, 400 );
    list->updateItems();
    list->updateItems();
}

/*================================================================*/
void KWVariableValueDia::resizeEvent( QResizeEvent *e )
{
    QDialog::resizeEvent( e );
    back->resize( size() );
}

/*================================================================*/
void KWVariableValueDia::closeEvent( QCloseEvent *e )
{
    list->setValues();
    QDialog::closeEvent( e );
}
