/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 Harri Porten <porten@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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



#include "kspread_dlg_reference.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_util.h"
#include "kspread_map.h"
#include "kspread_view.h"
#include "kspread_sheet.h"
#include "selection.h"
#include "kspread_locale.h"

#include <qvariant.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>

#include <kbuttonbox.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kdeversion.h>

#include <kstdguiitem.h>

using namespace KSpread;

reference::reference( View* parent, const char* name )
  : QDialog( parent, name,TRUE )
{
  m_pView = parent;
  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( KDialog::marginHint() );
  lay1->setSpacing( KDialog::spacingHint() );
  m_list = new QListBox(this);
  lay1->addWidget( m_list );

  setCaption( i18n("Area Name") );

  m_rangeName = new QLabel(this);
  lay1->addWidget(m_rangeName);

  m_pRemove = new QPushButton(i18n("&Remove"), this);
  lay1->addWidget( m_pRemove );
  //m_pRemove->setEnabled(false);
  KButtonBox *bb = new KButtonBox( this );
  //  bb->addStretch();
  m_pEdit = bb->addButton( i18n("&Edit...") );
  m_pOk = bb->addButton( KStdGuiItem::ok() );
  m_pCancel = bb->addButton( KStdGuiItem::cancel() );
  m_pOk->setDefault( TRUE );
  bb->layout();
  lay1->addWidget( bb );

  QString text;
  QStringList sheetName;
  QPtrListIterator<Sheet> it2 ( m_pView->doc()->map()->sheetList() );
  for( ; it2.current(); ++it2 )
  {
      sheetName.append( it2.current()->sheetName());
  }

  QValueList<Reference>::Iterator it;
  QValueList<Reference> area = m_pView->doc()->listArea();
  for ( it = area.begin(); it != area.end(); ++it )
  {
    text = (*it).ref_name;
    if ( sheetName.contains((*it).sheet_name ))
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

void reference::displayAreaValues(QString const & areaName)
{
  QString tmpName;
  QValueList<Reference>::Iterator it;
  QValueList<Reference> area( m_pView->doc()->listArea() );
  for ( it = area.begin(); it != area.end(); ++it )
  {
    if ((*it).ref_name == areaName)
    {
      if (!m_pView->doc()->map()->findSheet( (*it).sheet_name))
        kdDebug(36001) << "(*it).table_name '" << (*it).sheet_name
                       << "' not found!*********" << endl;
      else
        tmpName = util_rangeName(m_pView->doc()->map()->findSheet( (*it).sheet_name),
                                 (*it).rect);
      break;
    }
  }

  tmpName = i18n("Area: %1").arg(tmpName);
  m_rangeName->setText(tmpName);
}

void reference::slotHighlighted(QListBoxItem * )
{
  QString tmp = m_list->text(m_list->currentItem());
  displayAreaValues(tmp);
}

void reference::slotDoubleClicked(QListBoxItem *)
{
  slotOk();
}

void reference::slotRemove()
{
  if (m_list->currentItem() == -1)
    return;

  int ret = KMessageBox::warningContinueCancel( this, i18n("Do you really want to remove this area name?"),i18n("Remove Area"),KStdGuiItem::del());
  if (ret == KMessageBox::Cancel)
    return;

  QString textRemove;
  if ( m_list->currentItem() != -1)
  {
    m_pView->doc()->emitBeginOperation( false );

    QString textRemove = m_list->text(m_list->currentItem());
    m_pView->doc()->removeArea(textRemove );
    m_pView->doc()->setModified(true);
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

    Sheet *tbl;

    for ( tbl = m_pView->doc()->map()->firstSheet(); tbl != 0L; tbl = m_pView->doc()->map()->nextSheet() )
    {
      tbl->refreshRemoveAreaName(textRemove);
    }

    m_pView->slotUpdateView( m_pView->activeSheet() );
  }

  if ( !m_list->count() )
  {
    m_pOk->setEnabled( false );
    m_pRemove->setEnabled( false );
    m_pEdit->setEnabled( false );
  }
}

void reference::slotEdit()
{
  QString name(m_list->text(m_list->currentItem()));
  if ( name.isEmpty() )
    return;
  EditAreaName editDlg( m_pView, "EditArea", name );
  editDlg.exec();

  m_rangeName->setText(i18n("Area: %1").arg(""));
  QString tmp = m_list->text(m_list->currentItem());
  if (!tmp.isEmpty())
    displayAreaValues( tmp );
}

void reference::slotOk()
{
  m_pView->doc()->emitBeginOperation( false );

  QString text;
  if (m_list->currentItem() != -1)
  {
    int index = m_list->currentItem();
    text = m_list->text(index);
    QValueList<Reference> area = m_pView->doc()->listArea();

    if (m_pView->activeSheet()->sheetName() != area[ index ].sheet_name)
    {
      Sheet *sheet = m_pView->doc()->map()->findSheet(area[ index ].sheet_name);
      if (sheet)
        m_pView->setActiveSheet(sheet);
    }

    Region region(m_pView, Cell::fullName(m_pView->activeSheet(), area[index].rect.left(), area[index].rect.top()));
    m_pView->selectionInfo()->initialize(region);
    m_pView->selectionInfo()->initialize(area[ index ].rect);//,                                           m_pView->activeSheet() );
  }

  m_pView->slotUpdateView( m_pView->activeSheet() );
  accept();
}

void reference::slotCancel()
{
  reject();
}



EditAreaName::EditAreaName( View * parent,
                                          const char * name,
                                          QString const & areaname )
    : KDialogBase( parent, name ,true,i18n( "Edit Area" ) , Ok|Cancel )
{
  m_pView = parent;

  resize( 350, 142 );
  setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5,
                              (QSizePolicy::SizeType)4, 0, 0,
                              sizePolicy().hasHeightForWidth() ) );

  QWidget * page = new QWidget( this );
  setMainWidget(page);

  QGridLayout * EditAreaNameLayout
    = new QGridLayout( page, 1, 1, 11, 6, "EditAreaNameLayout");

  QHBoxLayout * Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1");
  QSpacerItem * spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding,
                                          QSizePolicy::Minimum );
  Layout1->addItem( spacer );

  EditAreaNameLayout->addMultiCellLayout( Layout1, 3, 3, 0, 1 );

  QLabel * TextLabel4 = new QLabel( page, "TextLabel4" );
  TextLabel4->setText( i18n( "Cells:" ) );

  EditAreaNameLayout->addWidget( TextLabel4, 2, 0 );

  m_area = new QLineEdit( page, "m_area" );

  EditAreaNameLayout->addWidget( m_area, 2, 1 );

  QLabel * TextLabel1 = new QLabel( page, "TextLabel1" );
  TextLabel1->setText( i18n( "Sheet:" ) );

  EditAreaNameLayout->addWidget( TextLabel1, 1, 0 );

  m_sheets = new QComboBox( FALSE, page, "m_sheets" );

  EditAreaNameLayout->addWidget( m_sheets, 1, 1 );

  QLabel * TextLabel2 = new QLabel( page, "TextLabel2" );
  TextLabel2->setText( i18n( "Area name:" ) );

  EditAreaNameLayout->addWidget( TextLabel2, 0, 0 );

  m_areaName = new QLabel( page, "m_areaName" );
  m_areaName->setText( areaname );

  EditAreaNameLayout->addWidget( m_areaName, 0, 1 );

  QPtrList<Sheet> sheetList = m_pView->doc()->map()->sheetList();
  for (unsigned int c = 0; c < sheetList.count(); ++c)
  {
    Sheet * t = sheetList.at(c);
    if (!t)
      continue;
    m_sheets->insertItem( t->sheetName() );
  }

  QString tmpName;
  QValueList<Reference>::Iterator it;
  QValueList<Reference> area(m_pView->doc()->listArea());
  for ( it = area.begin(); it != area.end(); ++it )
  {
    if ((*it).ref_name == areaname)
    {
      if (!m_pView->doc()->map()->findSheet( (*it).sheet_name))
        kdDebug(36001) << "(*it).table_name '" << (*it).sheet_name
                       << "' not found!*********" << endl;
      else
        tmpName = util_rangeName( (*it).rect );
      break;
    }
  }

  m_sheets->setCurrentText( (*it).sheet_name );
  m_area->setText( tmpName );

}

EditAreaName::~EditAreaName()
{
}

void EditAreaName::slotOk()
{
  Range range( m_area->text() );

  if ( !range.isValid() )
  {
    Point point( m_area->text() );
    if ( !point.isValid() )
      return;

    m_area->setText( m_area->text() + ":" + m_area->text() );

    range = Range( m_area->text() );
  }

  m_pView->doc()->emitBeginOperation( false );

  m_pView->doc()->removeArea( m_areaName->text() );
  m_pView->doc()->addAreaName(range.range(), m_areaName->text(), m_sheets->currentText() );

  Sheet *sheet;

  for ( sheet = m_pView->doc()->map()->firstSheet(); sheet != 0L;
        sheet = m_pView->doc()->map()->nextSheet() )
  {
    sheet->refreshChangeAreaName( m_areaName->text() );
  }

  m_pView->slotUpdateView( m_pView->activeSheet() );
  accept();
}

#include "kspread_dlg_reference.moc"
