/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2002 Ariya Hidayat <ariya@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999-2002 Laurent Montel <montel@kde.org>
   Copyright 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "ReferenceDialog.h"

// Qt
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QList>
#include <QPushButton>
#include <QVariant>
#include <QVBoxLayout>
//Added by qt3to4:
#include <Q3ListBox>

// KDE
#include <kdebug.h>
#include <kmessagebox.h>
#include <KStandardGuiItem>

// KSpread
#include "Canvas.h"
#include "Doc.h"
#include "Localization.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "Selection.h"
#include "Sheet.h"
#include "Util.h"
#include "View.h"

using namespace KSpread;

NamedAreaDialog::NamedAreaDialog(View* parent)
  : KDialog( parent )
{
  setButtons(KDialog::Ok | KDialog::Cancel | KDialog::User1 | KDialog::User2);
  setButtonsOrientation(Qt::Vertical);
  setButtonText(KDialog::User1, i18n("&Remove"));
  setButtonText(KDialog::User2, i18n("&Edit..."));
  setModal(true);
  setWindowTitle(i18n("Area Name"));

  m_pView = parent;

  QWidget* widget = new QWidget(this);
  setMainWidget(widget);

  QVBoxLayout *vboxLayout = new QVBoxLayout(widget);
  vboxLayout->setMargin(0);
  vboxLayout->setSpacing(KDialog::spacingHint());

  m_list = new Q3ListBox(this);
  vboxLayout->addWidget( m_list );

  m_rangeName = new QLabel(this);
  vboxLayout->addWidget(m_rangeName);

  QString text;
  QStringList sheetName;
  foreach ( Sheet* sheet, m_pView->doc()->map()->sheetList() )
  {
    sheetName.append( sheet->sheetName());
  }

  const QList<QString> namedAreas = m_pView->doc()->namedAreaManager()->areaNames();
  for (int i = 0; i < namedAreas.count(); ++i)
    m_list->insertItem(namedAreas[i]);

  if ( !m_list->count() )
  {
    enableButtonOk(false);
    enableButton(KDialog::User1, false);
    enableButton(KDialog::User2, false);
  }

  connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
  connect(this, SIGNAL(cancelClicked()), this, SLOT(slotCancel()));
  connect(this, SIGNAL(user1Clicked()), this, SLOT(slotRemove()));
  connect(this, SIGNAL(user2Clicked()), this, SLOT(slotEdit()));
  connect( m_list, SIGNAL(doubleClicked(Q3ListBoxItem *)), this,
           SLOT(slotDoubleClicked(Q3ListBoxItem *)));
  connect( m_list, SIGNAL(highlighted ( Q3ListBoxItem * ) ), this,
           SLOT(slotHighlighted(Q3ListBoxItem * )));
  m_rangeName->setText(i18n("Area: %1",QString("")));

  resize( 250, 200 );
}

void NamedAreaDialog::displayAreaValues(QString const & areaName)
{
  QString tmpName = m_pView->doc()->namedAreaManager()->namedArea(areaName).name();
  tmpName = i18n("Area: %1", tmpName);
  m_rangeName->setText(tmpName);
}

void NamedAreaDialog::slotHighlighted(Q3ListBoxItem * )
{
  QString tmp = m_list->text(m_list->currentItem());
  displayAreaValues(tmp);
}

void NamedAreaDialog::slotDoubleClicked(Q3ListBoxItem *)
{
  slotOk();
}

void NamedAreaDialog::slotRemove()
{
  if (m_list->currentItem() == -1)
    return;

  int ret = KMessageBox::warningContinueCancel( this, i18n("Do you really want to remove this area name?"),i18n("Remove Area"),KStandardGuiItem::del());
  if (ret == KMessageBox::Cancel)
    return;

  QString textRemove;
  if ( m_list->currentItem() != -1)
  {
    m_pView->doc()->emitBeginOperation( false );

    QString textRemove = m_list->text(m_list->currentItem());
    m_pView->doc()->namedAreaManager()->remove(textRemove);
    m_pView->doc()->setModified(true);
    /*
      m_list->clear();
      QString text;
      QList<Reference>::Iterator it;
      QList<Reference> area=m_pView->doc()->listArea();
      for ( it = area.begin(); it != area.end(); ++it )
      {
      text=(*it).ref_name;
      m_list->insertItem(text);
      }
    */

    m_list->removeItem(m_list->currentItem());

    m_pView->slotUpdateView( m_pView->activeSheet() );
  }

  if ( !m_list->count() )
  {
    enableButtonOk(false);
    enableButton(KDialog::User1, false);
    enableButton(KDialog::User2, false);
  }
}

void NamedAreaDialog::slotEdit()
{
  QString name(m_list->text(m_list->currentItem()));
  if ( name.isEmpty() )
    return;
  EditAreaName editDlg( m_pView, "EditArea", name );
  editDlg.exec();

  m_rangeName->setText(i18n("Area: %1",QString("")));
  QString tmp = m_list->text(m_list->currentItem());
  if (!tmp.isEmpty())
    displayAreaValues( tmp );
}

void NamedAreaDialog::slotOk()
{
  m_pView->doc()->emitBeginOperation( false );

  QString text;
  if (m_list->currentItem() != -1)
  {
    int index = m_list->currentItem();
    text = m_list->text(index);
    const QList<QString> areas = m_pView->doc()->namedAreaManager()->areaNames();
    Region region = m_pView->doc()->namedAreaManager()->namedArea(areas[index]);
    Sheet* sheet = m_pView->doc()->namedAreaManager()->sheet(areas[index]);

    if (m_pView->activeSheet() != sheet)
    {
      if (sheet)
        m_pView->setActiveSheet(sheet);
    }
    m_pView->selection()->initialize(region);
  }

  m_pView->slotUpdateView( m_pView->activeSheet() );
  accept();
}

void NamedAreaDialog::slotCancel()
{
  reject();
}



EditAreaName::EditAreaName( View * parent,
                                          const char * name,
                                          QString const & areaname )
  : KDialog( parent )
{
  setCaption( i18n("Edit Area") );
  setObjectName( name );
  setModal( true );
  setButtons( Ok|Cancel );

  m_pView = parent;

  resize( 350, 142 );
  setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5,
                              (QSizePolicy::SizeType)4 ) );

  QWidget *page = new QWidget();
  setMainWidget( page );

  QGridLayout * gridLayout = new QGridLayout( page);
  gridLayout->setMargin(KDialog::marginHint());
  gridLayout->setSpacing(KDialog::spacingHint());

  QLabel * textLabel4 = new QLabel( page );
  textLabel4->setText( i18n( "Cells:" ) );
  gridLayout->addWidget( textLabel4, 2, 0 );

  m_area = new QLineEdit( page );
  gridLayout->addWidget( m_area, 2, 1 );

  QLabel * textLabel1 = new QLabel( page );
  textLabel1->setText( i18n( "Sheet:" ) );
  gridLayout->addWidget( textLabel1, 1, 0 );

  m_sheets = new QComboBox( page );
  gridLayout->addWidget( m_sheets, 1, 1 );

  QLabel * textLabel2 = new QLabel( page );
  textLabel2->setText( i18n( "Area name:" ) );
  gridLayout->addWidget( textLabel2, 0, 0 );

  m_areaName = new QLabel( page );
  m_areaName->setText( areaname );
  gridLayout->addWidget( m_areaName, 0, 1 );

  Sheet* sheet = m_pView->doc()->namedAreaManager()->sheet(areaname);
  const QString tmpName = m_pView->doc()->namedAreaManager()->namedArea(areaname).name(sheet);

  int currentIndex = 0;
  QList<Sheet*> sheetList = m_pView->doc()->map()->sheetList();
  for (int c = 0; c < sheetList.count(); ++c)
  {
    Sheet * t = sheetList.at(c);
    if (!t)
      continue;
    if (t == sheet)
        currentIndex = c;
    m_sheets->insertItem( c, t->sheetName() );
  }

  m_sheets->setCurrentIndex(currentIndex);
  m_area->setText( tmpName );
  connect(this,SIGNAL(okClicked()),this,SLOT(slotOk()));
}

EditAreaName::~EditAreaName()
{
}

void EditAreaName::slotOk()
{
    Sheet* sheet = m_pView->doc()->map()->sheet(m_sheets->currentIndex());
    Region region(m_area->text(), m_pView->doc()->map(), sheet);
    if (!region.isValid())
        return;

    m_pView->doc()->emitBeginOperation( false );

    m_pView->doc()->namedAreaManager()->remove(m_areaName->text());
    m_pView->doc()->namedAreaManager()->insert(sheet, region.firstRange(), m_areaName->text());

    foreach (Sheet* sheet, m_pView->doc()->map()->sheetList())
        sheet->refreshChangeAreaName(m_areaName->text());

    m_pView->slotUpdateView(m_pView->activeSheet());
    accept();
}

#include "ReferenceDialog.moc"
