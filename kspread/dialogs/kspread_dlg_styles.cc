/* This file is part of the KDE project
   Copyright (C) 2003 Laurent Montel <montel@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>

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

#include <q3header.h>
#include <qlayout.h>
#include <qmap.h>
//Added by qt3to4:
#include <QVBoxLayout>

#include <kcombobox.h>
#include <kdebug.h>
#include <k3listview.h>
#include <klocale.h>

#include "kspread_canvas.h"
#include "kspread_cell.h"
#include "kspread_dlg_layout.h"
#include "kspread_sheet.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "kspread_view.h"

#include "kspread_dlg_styles.h"

using namespace KSpread;

StyleWidget::StyleWidget( QWidget * parent, const char * name, Qt::WFlags fl )
  : QWidget( parent, name, fl )
{
  QVBoxLayout * layout = new QVBoxLayout( this );
  layout->setMargin(11);
  layout->setSpacing(6);

  m_styleList = new K3ListView( this);
  m_styleList->addColumn( i18n( "Styles" ) );
  m_styleList->setResizeMode( K3ListView::AllColumns );
  layout->addWidget( m_styleList );

  m_displayBox = new KComboBox( false, this );
  m_displayBox->setObjectName( "m_displayBox" );
  layout->addWidget( m_displayBox );

  m_styleList->header()->setLabel( 0, i18n( "Styles" ) );
  m_displayBox->clear();
  m_displayBox->insertItem(0, i18n( "All Styles" ) );
  m_displayBox->insertItem(1, i18n( "Applied Styles" ) );
  m_displayBox->insertItem(2, i18n( "Custom Styles" ) );
  m_displayBox->insertItem(3, i18n( "Hierarchical" ) );
  connect( m_styleList, SIGNAL(doubleClicked ( Q3ListViewItem *)),this, SIGNAL( modifyStyle()));
  resize( QSize(446, 384).expandedTo(minimumSizeHint()) );
}

StyleWidget::~StyleWidget()
{
}



StyleDlg::StyleDlg( View * parent, StyleManager * manager,
                                  const char * name )
  : KDialogBase( KDialogBase::Tabbed, Qt::Dialog, parent, name, true, "",
                 KDialogBase::Ok | KDialogBase::User1 | KDialogBase::User2 | KDialogBase::User3 | KDialogBase::Close,
                 KDialogBase::Ok, false, KGuiItem( i18n( "&New..." ) ), KGuiItem( i18n( "&Modify..." ) ), KGuiItem( i18n( "&Delete" ) ) ),
    m_view( parent ),
    m_styleManager( manager ),
    m_dlg( new StyleWidget( this ) )
{
  setWindowTitle( i18n( "Style Manager" ) );
  setButtonBoxOrientation( Qt::Vertical );
  setMainWidget( m_dlg );

  slotDisplayMode( 0 );
  enableButton( KDialogBase::User1, true );
  enableButton( KDialogBase::User2, true );
  enableButton( KDialogBase::User3, false );

  connect( m_dlg->m_styleList, SIGNAL( selectionChanged( Q3ListViewItem * ) ),
           this, SLOT( slotSelectionChanged( Q3ListViewItem * ) ) );
  connect( m_dlg->m_displayBox, SIGNAL( activated( int ) ), this, SLOT( slotDisplayMode( int ) ) );
  connect( this, SIGNAL( user3Clicked() ), this, SLOT( slotUser3() ) );
  connect( m_dlg, SIGNAL( modifyStyle() ), this, SLOT( slotUser2()));
}

StyleDlg::~StyleDlg()
{
}

void StyleDlg::fillComboBox()
{
  class Map : public QMap<CustomStyle *, K3ListViewItem *> {};
  Map entries;

  entries.clear();
  entries[m_styleManager->defaultStyle()] = new K3ListViewItem( m_dlg->m_styleList, i18n( "Default" ) );

  StyleManager::Styles::const_iterator iter = m_styleManager->m_styles.begin();
  StyleManager::Styles::const_iterator end  = m_styleManager->m_styles.end();

  while ( entries.count() != m_styleManager->m_styles.count() + 1 )
  {
    if ( entries.find( iter.value() ) == entries.end() )
    {
      if ( iter.value()->parent() == 0 )
        entries[iter.value()] = new K3ListViewItem( m_dlg->m_styleList, iter.value()->name() );
      else
      {
        Map::const_iterator i = entries.find( iter.value()->parent() );
        if ( i != entries.end() )
          entries[iter.value()] = new K3ListViewItem( i.value(), iter.value()->name() );
      }
    }

    ++iter;
    if ( iter == end )
      iter = m_styleManager->m_styles.begin();
  }
  entries.clear();
}

void StyleDlg::slotDisplayMode( int mode )
{
  m_dlg->m_styleList->clear();

  if ( mode != 3 )
    m_dlg->m_styleList->setRootIsDecorated( false );
  else
  {
    m_dlg->m_styleList->setRootIsDecorated( true );
    fillComboBox();
    return;
  }

  if ( mode != 2 )
    new K3ListViewItem( m_dlg->m_styleList, i18n( "Default" ) );

  StyleManager::Styles::iterator iter = m_styleManager->m_styles.begin();
  StyleManager::Styles::iterator end  = m_styleManager->m_styles.end();

  while ( iter != end )
  {
    CustomStyle * styleData = iter.value();
    if ( !styleData || styleData->name().isEmpty() )
    {
      ++iter;
      continue;
    }

    if ( mode == 2 )
    {
      if ( styleData->type() == Style::CUSTOM )
        new K3ListViewItem( m_dlg->m_styleList, styleData->name() );
    }
    else if ( mode == 1 )
    {
      if ( styleData->usage() > 0 )
        new K3ListViewItem( m_dlg->m_styleList, styleData->name() );
    }
    else
      new K3ListViewItem( m_dlg->m_styleList, styleData->name() );

    ++iter;
  }
}

void StyleDlg::slotOk()
{
  K3ListViewItem * item = (K3ListViewItem *) m_dlg->m_styleList->currentItem();

  if ( !item )
  {
    accept();
    return;
  }

  CustomStyle * s = 0;

  QString name( item->text( 0 ) );
  if ( name == i18n( "Default" ) )
    s = m_styleManager->defaultStyle();
  else
    s = m_styleManager->style( name );

  if ( !s )
  {
    accept();
    return;
  }

  if ( m_view )
  {
    Sheet * sheet = m_view->activeSheet();

    if ( sheet )
    {
      m_view->doc()->emitBeginOperation( false );
      sheet->setSelectionStyle( m_view->selectionInfo(), s );
    }
  }

  m_view->slotUpdateView( m_view->activeSheet() );
  accept();
}

void StyleDlg::slotUser1()
{
  CustomStyle * s = 0;

  K3ListViewItem * item = (K3ListViewItem *) m_dlg->m_styleList->currentItem();

  if ( item )
  {
    QString name( item->text( 0 ) );
    if ( name == i18n( "Default" ) )
      s = m_styleManager->defaultStyle();
    else
      s = m_styleManager->style( name );
  }
  else
    s = m_styleManager->defaultStyle();

  int i = 1;
  QString newName( i18n( "style%1" ).arg( m_styleManager->count() + i ) );
  while ( m_styleManager->style( newName ) != 0 )
  {
    ++i;
    newName = i18n( "style%1" ).arg( m_styleManager->count() + i );
  }

  CustomStyle * style = new CustomStyle( newName, s );
  style->setType( Style::TENTATIVE );

  CellFormatDialog dlg( m_view, style, m_styleManager, m_view->doc() );

  if ( style->type() == Style::TENTATIVE )
  {
    delete style;
    return;
  }

  m_styleManager->m_styles[ style->name() ] = style;

  slotDisplayMode( m_dlg->m_displayBox->currentIndex() );
}

void StyleDlg::slotUser2()
{
  K3ListViewItem * item = (K3ListViewItem *) m_dlg->m_styleList->currentItem();

  if ( !item )
    return;

  CustomStyle * s = 0;

  QString name( item->text( 0 ) );
  if ( name == i18n( "Default" ) )
    s = m_styleManager->defaultStyle();
  else
    s = m_styleManager->style( name );

  if ( !s )
    return;

  CellFormatDialog dlg( m_view, s, m_styleManager, m_view->doc() );
  slotDisplayMode( m_dlg->m_displayBox->currentIndex() );
}

void StyleDlg::slotUser3()
{
  K3ListViewItem * item = (K3ListViewItem *) m_dlg->m_styleList->currentItem();

  if ( !item )
    return;

  CustomStyle * s = 0;

  QString name( item->text( 0 ) );
  if ( name == i18n( "Default" ) )
    s = m_styleManager->defaultStyle();
  else
    s = m_styleManager->style( name );

  if ( !s )
    return;

  if ( s->type() != Style::CUSTOM )
    return;

  s->setType( Style::AUTO );
  m_styleManager->takeStyle( s );

  slotDisplayMode( m_dlg->m_displayBox->currentIndex() );
}

void StyleDlg::slotSelectionChanged( Q3ListViewItem * item )
{
  if ( !item )
    return;

  CustomStyle* style = 0;
  QString name( item->text( 0 ) );
  if ( name == i18n( "Default" ) )
    style = m_styleManager->defaultStyle();
  else
    style = m_styleManager->style( name );
  if ( !style )
  {
    enableButton( KDialogBase::User3, false );
    return;
  }

  if ( style->type() == Style::BUILTIN )
    enableButton( KDialogBase::User3, false );
  else
    enableButton( KDialogBase::User3, true );
}


#include "kspread_dlg_styles.moc"

