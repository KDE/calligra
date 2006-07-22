/* This file is part of the KDE project
   Copyright (C) 2002 Heinrich Kuettler <heinrich.kuettler@gmx.de>

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

#include <qlistbox.h>
#include <qpainter.h>

#include <kapplication.h>
#include <kcombobox.h>
#include <kglobalsettings.h>
#include <ktoolbar.h>
#include <kdebug.h>

#include "symbolaction.h"

/*
 * The items for the SymbolCombos. *
 */

KFORMULA_NAMESPACE_BEGIN

class SymbolComboItem : public QListBoxItem
{
public:
    SymbolComboItem( const QString&, const QFont& , QChar, QComboBox* combo );
    virtual ~SymbolComboItem();

    virtual int width( const QListBox* ) const;
    virtual int height( const QListBox* ) const;

protected:
    virtual void paint( QPainter *p );

private:
    QComboBox *m_combo;
    QString m_name;
    QFont m_font;
    QChar m_symbol;

    static int widest;
};

int SymbolComboItem::widest = 0;

SymbolComboItem::SymbolComboItem( const QString &name, const QFont& font, 
                                  QChar symbol, QComboBox *combo )
    : QListBoxItem( combo->listBox() ),
      m_combo( combo ),
      m_name( name ),
      m_font( font ),
      m_symbol( symbol )
{
    setText( name );
    int charWidth = QFontMetrics( m_font ).width( QChar( m_symbol ) );
    widest = QMAX( widest, charWidth );
}

SymbolComboItem::~SymbolComboItem()
{
}

int SymbolComboItem::width( const QListBox * /*lb*/ ) const
{
    return widest + QFontMetrics( KGlobalSettings::generalFont() ).width( text() ) + 12;
}

int SymbolComboItem::height( const QListBox * /*lb*/ ) const
{
    int generalHeight = QFontMetrics( KGlobalSettings::generalFont() ).lineSpacing();
    int fontHeight = QFontMetrics( m_font ).lineSpacing();
    return QMAX( generalHeight, fontHeight ) + 2;
}

void SymbolComboItem::paint( QPainter *p )
{
    p->setFont( m_font );
    QFontMetrics fm( p->fontMetrics() );
    p->drawText( 3, fm.ascent() + fm.leading() / 2,
                 QString( "%1" ).arg( QChar( m_symbol ) ) );

    p->setFont( KGlobalSettings::generalFont() );
    fm = p->fontMetrics();
    p->drawText( widest + 6, height( m_combo->listBox() ) / 2 + fm.strikeOutPos(), m_name );
}

/*
 * The symbol action *
 */
SymbolAction::SymbolAction( QObject* parent, const char* name )
    : KSelectAction( parent, name )
{
    setEditable( FALSE );
}

SymbolAction::SymbolAction( const QString& text, const KShortcut& cut,
                            const QObject* receiver, const char* slot,
                            QObject* parent, const char* name )
    : KSelectAction( text, cut, receiver, slot, parent, name )
{
    setEditable( FALSE );
}

int SymbolAction::plug( QWidget* w, int index )
{
    if (kapp && !kapp->authorizeKAction(name()))
        return -1;
    if ( w->inherits( "KToolBar" ) )
    {
        KToolBar* bar = static_cast<KToolBar*>( w );
        int id_ = KAction::getToolButtonID();
        KComboBox *cb = new KComboBox( bar );
        connect( cb, SIGNAL( activated( const QString & ) ),
                 SLOT( slotActivated( const QString & ) ) );
        cb->setEnabled( isEnabled() );
        bar->insertWidget( id_, comboWidth(), cb, index );
        cb->setMinimumWidth( cb->sizeHint().width() );

        addContainer( bar, id_ );

        connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

        updateItems( containerCount() - 1 );

        return containerCount() - 1;
    }
    else return KSelectAction::plug( w, index );
}

void SymbolAction::setSymbols( const QStringList &names,
                               const QFont& font,
                               const QMemArray<QChar>& chars )
{
    m_chars = chars;
    m_font = font;
    setItems( names );

    int len = containerCount();
    for ( int i = 0; i < len; ++i )
        updateItems( i );
}

void SymbolAction::updateItems( int id )
{
    QWidget *w = container( id );
    if ( w->inherits( "KToolBar" ) ) {
        QWidget *r = static_cast<KToolBar*>( w )->getWidget( itemId( id ) );
        if ( r->inherits( "QComboBox" ) ) {
            QComboBox *cb = static_cast<QComboBox*>( r );
            cb->clear();

            for( uint i = 0; i < items().count(); ++i ) {
                new SymbolComboItem( *items().at( i ), m_font, m_chars.at( i ), cb );
            }
            cb->setMinimumWidth( cb->sizeHint().width() );
        }
    }
}

KFORMULA_NAMESPACE_END
