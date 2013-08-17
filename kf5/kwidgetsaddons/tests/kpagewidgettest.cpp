/*
    This file is part of the KDE Libraries

    Copyright (C) 2006 Tobias Koenig (tokoe@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB. If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include "kpagewidgetmodel.h"

#include "kpagewidgettest.h"

KPageWidgetTest::KPageWidgetTest( QWidget *parent )
  : QWidget( parent )
{
  QGridLayout *layout = new QGridLayout( this );

  mWidget = new KPageWidget( this );
  layout->addWidget( mWidget, 0, 0, 7, 1 );

  connect( mWidget, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)),
           this, SLOT(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)) );
  connect( mWidget, SIGNAL(pageToggled(KPageWidgetItem*,bool)),
           this, SLOT(pageToggled(KPageWidgetItem*,bool)) );

  int rowCount = 0;
  QPushButton *button = new QPushButton( QStringLiteral("Auto"), this );
  layout->addWidget( button, rowCount, 1 );
  connect( button, SIGNAL(clicked()), this, SLOT(setAutoFace()) );
  rowCount++;

  button = new QPushButton( QStringLiteral( "Plain"), this );
  layout->addWidget( button, rowCount, 1 );
  connect( button, SIGNAL(clicked()), this, SLOT(setPlainFace()) );
  rowCount++;

  button = new QPushButton( QStringLiteral( "List"), this );
  layout->addWidget( button, rowCount, 1 );
  connect( button, SIGNAL(clicked()), this, SLOT(setListFace()) );
  rowCount++;

  button = new QPushButton( QStringLiteral( "Tree"), this );
  layout->addWidget( button, rowCount, 1 );
  connect( button, SIGNAL(clicked()), this, SLOT(setTreeFace()) );
  rowCount++;

  button = new QPushButton( QStringLiteral( "Tabbed"), this );
  layout->addWidget( button, rowCount, 1 );
  connect( button, SIGNAL(clicked()), this, SLOT(setTabbedFace()) );
  rowCount++;

  button = new QPushButton( QStringLiteral( "Add Page"), this );
  layout->addWidget( button, rowCount, 1 );
  connect( button, SIGNAL(clicked()), this, SLOT(addPage()) );
  rowCount++;

  button = new QPushButton( QStringLiteral( "Add Sub Page"), this );
  layout->addWidget( button, rowCount, 1 );
  connect( button, SIGNAL(clicked()), this, SLOT(addSubPage()) );
  rowCount++;

  button = new QPushButton( QStringLiteral( "Insert Page"), this );
  layout->addWidget( button, rowCount, 1 );
  connect( button, SIGNAL(clicked()), this, SLOT(insertPage()) );
  rowCount++;

  button = new QPushButton( QStringLiteral( "Delete Page"), this );
  layout->addWidget( button, rowCount, 1 );
  connect( button, SIGNAL(clicked()), this, SLOT(deletePage()) );
  rowCount++;

  KPageWidgetItem *item = mWidget->addPage( new QPushButton( QStringLiteral( "folder" )), QStringLiteral("folder") );
  item->setIcon( QIcon::fromTheme( QStringLiteral("folder") ) );
  item = mWidget->addSubPage( item, new QPushButton( QStringLiteral( "subfolder" ) ), QStringLiteral("subfolder") );
  item->setIcon( QIcon::fromTheme( QStringLiteral("folder") ) );
  item = mWidget->addPage( new QLabel( QStringLiteral("second folder") ), QStringLiteral("second folder") );
  item->setIcon( QIcon::fromTheme( QStringLiteral("folder") ) );
}

KPageWidgetTest::~KPageWidgetTest()
{
}

void KPageWidgetTest::setAutoFace()
{
  mWidget->setFaceType( KPageWidget::Auto );
}

void KPageWidgetTest::setPlainFace()
{
  mWidget->setFaceType( KPageWidget::Plain );
}

void KPageWidgetTest::setListFace()
{
  mWidget->setFaceType( KPageWidget::List );
}

void KPageWidgetTest::setTreeFace()
{
  mWidget->setFaceType( KPageWidget::Tree );
}

void KPageWidgetTest::setTabbedFace()
{
  mWidget->setFaceType( KPageWidget::Tabbed );
}

void KPageWidgetTest::addPage()
{
  static int counter = 0;

  const QString title = QString( QStringLiteral( "dynamic folder %1") ).arg( QString::number( counter ) );
  KPageWidgetItem *item = mWidget->addPage( new QPushButton( title ) , title );
  item->setIcon( QIcon::fromTheme( QStringLiteral("folder") ) );
  item->setHeader( QString( QStringLiteral( "Header Test No. %1") ).arg( QString::number( counter ) ) );
  item->setCheckable( true );

  counter++;
}

void KPageWidgetTest::addSubPage()
{
  static int counter = 0;

  KPageWidgetItem *item = mWidget->currentPage();
  if ( !item )
    return;

  const QString title = QString( QStringLiteral( "subfolder %1") ).arg( QString::number( counter ) );
  item = mWidget->addSubPage( item, new QLabel( title ) , title );
  item->setIcon( QIcon::fromTheme( QStringLiteral( "folder" ) ) );

  counter++;
}

void KPageWidgetTest::insertPage()
{
  static int counter = 0;

  KPageWidgetItem *item = mWidget->currentPage();
  if ( !item )
    return;

  const QString title = QString( QStringLiteral( "before folder %1") ).arg( QString::number( counter ) );
  item = mWidget->insertPage( item, new QLabel( title ) , title );
  item->setIcon( QIcon::fromTheme( QStringLiteral( "folder" ) ) );

  counter++;
}

void KPageWidgetTest::deletePage()
{
  KPageWidgetItem *item = mWidget->currentPage();
  if ( item )
    mWidget->removePage( item );
}

void KPageWidgetTest::currentPageChanged( KPageWidgetItem *current, KPageWidgetItem *before )
{
  if ( current )
    qDebug( "Current item: %s", qPrintable( current->name() ) );
  else
    qDebug( "No current item" );

  if ( before )
    qDebug( "Item before: %s", qPrintable( before->name() ) );
  else
    qDebug( "No item before" );
}

void KPageWidgetTest::pageToggled( KPageWidgetItem *item, bool checked )
{
  qDebug( "Item %s changed check state to: %s", qPrintable( item->name() ), checked ? "checked" : "unchecked" );
}

int main( int argc, char **argv )
{
  QApplication::setApplicationName( QStringLiteral("KPageWidgetTest") );

  QApplication app(argc, argv);

  KPageWidgetTest testWidget( 0 );
  testWidget.show();

  return app.exec();
}

