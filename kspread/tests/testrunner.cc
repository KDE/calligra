/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "testrunner.h"

#include <qdict.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtextedit.h>

#include <kcombobox.h>
#include <kdialogbase.h>
#include <klocale.h>
#include <kpushbutton.h>

#include "tester.h"
#include "value_tester.h"

namespace KSpread
{

class TestRunner::Private
{
public:
  QDict<Tester> testers;
  KComboBox* testType;
  KPushButton* runButton;
  QTextEdit* logView;
};

};

using namespace KSpread;


TestRunner::TestRunner():
  KDialogBase( KDialogBase::Plain, i18n( "Internal Tests"), KDialogBase::Close, 
  KDialogBase::Close )
{
  d = new Private;
  
  QFrame* mainWidget = plainPage();
  QGridLayout* layout = new QGridLayout( mainWidget, 3, 4, marginHint(), spacingHint() );
  setMinimumSize( 360, 230 );
  
  QLabel* typeLabel = new QLabel( i18n("Type of Test:"), mainWidget );
  layout->addWidget( typeLabel, 0, 0 );
  
  d->testType = new KComboBox( mainWidget );
  layout->addWidget( d->testType, 0, 1 );
  
  QSpacerItem* spacerItem = new QSpacerItem( 10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum );
  layout->addItem( spacerItem, 0, 2 );
  
  d->runButton = new KPushButton( i18n("Run"), mainWidget );
  layout->addWidget( d->runButton, 0, 3 );
  
  d->logView = new QTextEdit( mainWidget );
  layout->addMultiCellWidget( d->logView, 2, 2, 0, 3 );
  d->logView->setTextFormat( Qt::LogText );
  
  QObject::connect( d->runButton, SIGNAL( clicked() ), this, SLOT( runTest() ) );
  
  // add all tests here !!
  addTester( new ValueTester() );
}

TestRunner::~TestRunner()
{
  QDictIterator<Tester> it( d->testers );
  for( ; it.current(); ++it ) delete it.current();
  delete d;
}

void TestRunner::addTester( Tester* tester )
{
  if( !tester ) return;
  d->testers.insert( tester->name(), tester );
  d->testType->insertItem( tester->name() );
}

void TestRunner::runTest()
{
  QString testName = d->testType->currentText();
  Tester* tester = d->testers.find( testName );
  if( tester )
  {
    d->logView->clear();
    d->logView->append( i18n("Test: %1").arg( testName ) );
    tester->run();
    QStringList errorList = tester->errors();
    if( tester->failed() )
    {
      d->logView->append( i18n( "%1 tests, <b>%2 failed.</b>" ).arg( tester->count() ).
        arg(  tester->failed() ) );
      for( unsigned k = 0; k < errorList.count(); k++ )
        d->logView->append( errorList[k] );
    }
    else
      d->logView->append( i18n( "%1 tests, everything is OK." ).arg( tester->count() ) );
    d->logView->append( i18n("Test finished.") );
  }
}