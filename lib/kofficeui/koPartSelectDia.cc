/* This file is part of the KDE libraries
    Copyright (C) 1998 Torben Weis <weis@kde.org>

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

#include <koQueryTypes.h>

#include "koPartSelectDia.h"

#include <klocale.h>
#include <kapp.h>
#include <qpainter.h>
#include <qpixmap.h>

/****************************************************
 *
 * KoBeListBoxItem
 *
 ****************************************************/

/**
 * This class is used to display icons and text in 
 * a QListBox.
 */
class KoBeListBoxItem : public QListBoxItem
{

public:

  KoBeListBoxItem( const char *s, const QPixmap p ) : QListBoxItem(), pm(p)
  { setText( s ); }

protected:

  virtual void paint( QPainter * );
  virtual int height( const QListBox * ) const;
  virtual int width( const QListBox * ) const;
  virtual const QPixmap *pixmap() const { return &pm; }

private:

  QPixmap pm;
};

void KoBeListBoxItem::paint( QPainter *p )
{
  p->drawPixmap( 3, 0, pm );
  QFontMetrics fm = p->fontMetrics();
  int yPos;
  // vertical text position 
  if ( pm.height() < fm.height() )
    yPos = fm.ascent() + fm.leading()/2;
  else 
    yPos = pm.height()/2 - fm.height()/2 + fm.ascent();
  p->drawText( pm.width() + 5, yPos, text() );
}

int KoBeListBoxItem::height(const QListBox *lb ) const
{  
  return QMAX( pm.height(), lb->fontMetrics().lineSpacing() + 1 );
}

int KoBeListBoxItem::width(const QListBox *lb ) const
{
  return pm.width() + lb->fontMetrics().width( text() ) + 6;
}

/****************************************************
 *
 * KoPartSelectDia
 *
 ****************************************************/

KoPartSelectDia::KoPartSelectDia( QWidget* parent, const char* name ) : DlgPartSelectData( parent, name )
{
  setCaption( i18n( "Insert Object" ) );  

  // Query for documents
  m_lstEntries = KoDocumentEntry::query();

  // Show the listbox
  QListBox *list = listBox();
  QValueList<KoDocumentEntry>::Iterator it = m_lstEntries.begin();
  for( ; it != m_lstEntries.end(); ++it )
  {
    list->insertItem( new KoBeListBoxItem( (*it).name, (*it).miniIcon ) );    
  }

  connect( ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

KoDocumentEntry KoPartSelectDia::result()
{
  return m_lstEntries[ listBox()->currentItem() ];
}

/*======================= show dialog ============================*/
KoDocumentEntry KoPartSelectDia::selectPart()
{
  KoDocumentEntry e;

  KoPartSelectDia *dlg = new KoPartSelectDia( 0, "PartSelect" );

  if (dlg->exec() == QDialog::Accepted)
  {
    e = dlg->result();
  }

  delete dlg;

  return e;
}

KoPartSelectDia::~KoPartSelectDia()
{
}

#include "koPartSelectDia.moc"
