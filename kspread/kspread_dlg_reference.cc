/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999, 2000, 2001  Montel Laurent <lmontel@mandrakesoft.com>

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



#include "kspread_dlg_reference.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_util.h"
#include "kspread_map.h"

#include <qvariant.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>

#include <kbuttonbox.h>
#include <kmessagebox.h>
#include <kdebug.h>

KSpreadreference::KSpreadreference( KSpreadView* parent, const char* name )
  : QDialog( parent, name,TRUE )
{
  m_pView = parent;
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );
  m_list = new QListBox(this);
  lay1->addWidget( m_list );

  setCaption( i18n("Area Name") );

  m_rangeName = new QLabel(this);
  lay1->addWidget(m_rangeName);

  m_pRemove = new QPushButton(i18n("&Remove..."), this);
  lay1->addWidget( m_pRemove );
  //m_pRemove->setEnabled(false);
  KButtonBox *bb = new KButtonBox( this );
  //  bb->addStretch();
  m_pEdit = bb->addButton( i18n("&Edit") );
  m_pOk = bb->addButton( i18n("&OK") );
  m_pOk->setDefault( TRUE );
  m_pCancel = bb->addButton( i18n( "&Cancel" ) );
  bb->layout();
  lay1->addWidget( bb );

  QString text;
  QStringList tableName;
  QPtrListIterator<KSpreadTable> it2 ( m_pView->doc()->map()->tableList() );
  for( ; it2.current(); ++it2 )
  {
      tableName.append( it2.current()->tableName());
  }

  QValueList<Reference>::Iterator it;
  QValueList<Reference> area = m_pView->doc()->listArea();
  for ( it = area.begin(); it != area.end(); ++it )
  {
    text = (*it).ref_name;
    if ( tableName.contains((*it).table_name ))
        m_list->insertItem(text);
  }

  if ( !m_list->count() )
  {
    m_pOk->setEnabled( false );
    m_pRemove->setEnabled( false );
    m_pEdit->setEnabled( false );
  }

  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pCancel, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
  connect( m_pEdit, SIGNAL( clicked() ), this, SLOT( slotEdit() ) );
  connect( m_pRemove, SIGNAL( clicked() ), this, SLOT( slotRemove() ) );
  connect( m_list, SIGNAL(doubleClicked(QListBoxItem *)), this,
           SLOT(slotDoubleClicked(QListBoxItem *)));
  connect( m_list, SIGNAL(highlighted ( QListBoxItem * ) ), this,
           SLOT(slotHighlighted(QListBoxItem * )));
  m_rangeName->setText(i18n("Area: %1").arg(""));

  resize( 250, 200 );
}

void KSpreadreference::displayAreaValues(QString const & areaName)
{
  QString tmpName;
  QValueList<Reference>::Iterator it;
  QValueList<Reference> area( m_pView->doc()->listArea() );
  for ( it = area.begin(); it != area.end(); ++it )
  {
    if ((*it).ref_name == areaName)
    {
      if (!m_pView->doc()->map()->findTable( (*it).table_name))
        kdDebug(36001) << "(*it).table_name '" << (*it).table_name
                       << "' not found!*********" << endl;
      else
        tmpName = util_rangeName(m_pView->doc()->map()->findTable( (*it).table_name),
                                 (*it).rect);
      break;
    }
  }

  tmpName = i18n("Area: %1").arg(tmpName);
  m_rangeName->setText(tmpName);
}

void KSpreadreference::slotHighlighted(QListBoxItem * )
{
  QString tmp = m_list->text(m_list->currentItem());
  displayAreaValues(tmp);
}

void KSpreadreference::slotDoubleClicked(QListBoxItem *)
{
  slotOk();
}

void KSpreadreference::slotRemove()
{
  if (m_list->currentItem() == -1)
    return;

  int ret = KMessageBox::warningYesNo( this, i18n("Do you really want to remove this area name?"));
  if (ret == 4) // reponse = No
    return;

  QString textRemove;
  if ( m_list->currentItem() != -1)
  {
    QString textRemove = m_list->text(m_list->currentItem());
    m_pView->doc()->removeArea(textRemove );

    /*
      m_list->clear();
      QString text;
      QValueList<Reference>::Iterator it;
      QValueList<Reference> area=m_pView->doc()->listArea();
      for ( it = area.begin(); it != area.end(); ++it )
      {
      text=(*it).ref_name;
      m_list->insertItem(text);
      }
    */

    m_list->removeItem(m_list->currentItem());

    KSpreadTable *tbl;

    for ( tbl = m_pView->doc()->map()->firstTable(); tbl != 0L; tbl = m_pView->doc()->map()->nextTable() )
    {
      tbl->refreshRemoveAreaName(textRemove);
    }
  }

  if ( !m_list->count() )
  {
    m_pOk->setEnabled( false );
    m_pRemove->setEnabled( false );
    m_pEdit->setEnabled( false );
  }
}

void KSpreadreference::slotEdit()
{
  QString name(m_list->text(m_list->currentItem()));
  if ( name.length() == 0 )
    return;
  KSpreadEditAreaName editDlg( m_pView, "EditArea", name );
  editDlg.exec();

  m_rangeName->setText(i18n("Area: %1").arg(""));  
  QString tmp = m_list->text(m_list->currentItem());
  if (tmp.length() > 0)
  {
    displayAreaValues( tmp );
  }
  else
  {
  }    
}

void KSpreadreference::slotOk()
{
  QString text;
  if (m_list->currentItem() != -1)
  {
    int index = m_list->currentItem();
    text = m_list->text(index);
    QValueList<Reference> area = m_pView->doc()->listArea();

    if (m_pView->activeTable()->tableName() != area[ index ].table_name)
    {
      KSpreadTable *table = m_pView->doc()->map()->findTable(area[ index ].table_name);
      if (table)
        m_pView->setActiveTable(table);
    }

    m_pView->canvasWidget()->gotoLocation(KSpreadPoint( m_pView->activeTable()->tableName()
                                                        + "!"+util_cellName(area[ index ].rect.left(), area[ index ].rect.top()  ), m_pView->doc()->map() ) );
    m_pView->selectionInfo()->setSelection(area[ index ].rect ,
                                           m_pView->activeTable() );
  }
  accept();
}

void KSpreadreference::slotCancel()
{
  reject();
}



KSpreadEditAreaName::KSpreadEditAreaName( KSpreadView * parent,
                                          const char * name,
                                          QString const & areaname )
  : QDialog( parent, name )
{
  m_pView = parent;

  resize( 350, 142 );
  setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5,
                              (QSizePolicy::SizeType)4, 0, 0,
                              sizePolicy().hasHeightForWidth() ) );
  setCaption( i18n( "Edit Area" ) );
  setSizeGripEnabled( TRUE );
  QGridLayout * KSpreadEditAreaNameLayout
    = new QGridLayout( this, 1, 1, 11, 6, "KSpreadEditAreaNameLayout");

  QHBoxLayout * Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1");
  QSpacerItem * spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding,
                                          QSizePolicy::Minimum );
  Layout1->addItem( spacer );

  m_buttonOk = new QPushButton( this, "m_buttonOk" );
  m_buttonOk->setText( i18n( "&OK" ) );
  m_buttonOk->setAccel( 0 );
  m_buttonOk->setAutoDefault( TRUE );
  m_buttonOk->setDefault( TRUE );
  Layout1->addWidget( m_buttonOk );

  m_buttonCancel = new QPushButton( this, "buttonCancel" );
  m_buttonCancel->setText( i18n( "&Cancel" ) );
  m_buttonCancel->setAccel( 0 );
  m_buttonCancel->setAutoDefault( TRUE );
  Layout1->addWidget( m_buttonCancel );

  KSpreadEditAreaNameLayout->addMultiCellLayout( Layout1, 3, 3, 0, 1 );

  QLabel * TextLabel4 = new QLabel( this, "TextLabel4" );
  TextLabel4->setText( i18n( "Cells:" ) );

  KSpreadEditAreaNameLayout->addWidget( TextLabel4, 2, 0 );

  m_area = new QLineEdit( this, "m_area" );

  KSpreadEditAreaNameLayout->addWidget( m_area, 2, 1 );

  QLabel * TextLabel1 = new QLabel( this, "TextLabel1" );
  TextLabel1->setText( i18n( "Sheet:" ) );

  KSpreadEditAreaNameLayout->addWidget( TextLabel1, 1, 0 );

  m_sheets = new QComboBox( FALSE, this, "m_sheets" );

  KSpreadEditAreaNameLayout->addWidget( m_sheets, 1, 1 );

  QLabel * TextLabel2 = new QLabel( this, "TextLabel2" );
  TextLabel2->setText( i18n( "Area name:" ) );

  KSpreadEditAreaNameLayout->addWidget( TextLabel2, 0, 0 );

  m_areaName = new QLabel( this, "m_areaName" );
  m_areaName->setText( areaname );

  KSpreadEditAreaNameLayout->addWidget( m_areaName, 0, 1 );

  QPtrList<KSpreadTable> tableList = m_pView->doc()->map()->tableList();
  for (unsigned int c = 0; c < tableList.count(); ++c)
  {
    KSpreadTable * t = tableList.at(c);
    if (!t)
      continue;
    m_sheets->insertItem( t->tableName() );
  }

  QString tmpName;
  QValueList<Reference>::Iterator it;
  QValueList<Reference> area(m_pView->doc()->listArea());
  for ( it = area.begin(); it != area.end(); ++it )
  {
    if ((*it).ref_name == areaname)
    {
      if (!m_pView->doc()->map()->findTable( (*it).table_name))
        kdDebug(36001) << "(*it).table_name '" << (*it).table_name
                       << "' not found!*********" << endl;
      else
        tmpName = util_rangeName( (*it).rect );
      break;
    }
  }

  m_sheets->setCurrentText( (*it).table_name );
  m_area->setText( tmpName );

  // signals and slots connections
  connect( m_buttonOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

KSpreadEditAreaName::~KSpreadEditAreaName()
{
}

void KSpreadEditAreaName::slotOk()
{
  KSpreadRange range( m_area->text() );

  if ( !range.isValid() )
  {
    KSpreadPoint point( m_area->text() );
    if ( !point.isValid() )
      return;

    m_area->setText( m_area->text() + ":" + m_area->text() );

    range = KSpreadRange( m_area->text() );
  }

  m_pView->doc()->removeArea( m_areaName->text() );
  m_pView->doc()->addAreaName(range.range, m_areaName->text(), m_sheets->currentText() );

  KSpreadTable *tbl;

  for ( tbl = m_pView->doc()->map()->firstTable(); tbl != 0L; tbl = m_pView->doc()->map()->nextTable() )
  {
    tbl->refreshChangeAreaName( m_areaName->text() );
  }

  accept();
}

#include "kspread_dlg_reference.moc"
