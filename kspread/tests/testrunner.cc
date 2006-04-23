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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "testrunner.h"

#include <QApplication>
#include <QGridLayout>
#include <QHash>
#include <QLabel>
#include <QTextEdit>

#include <kcombobox.h>
#include <kdialogbase.h>
#include <kpushbutton.h>

#include "tester.h"
#include "value_tester.h"
#include "formula_tester.h"
//#include "stylecluster_tester.h"

namespace KSpread
{

class TestRunner::Private
{
public:
  QHash<QString, Tester*> testers;
  KComboBox* testType;
  KPushButton* runButton;
  QTextEdit* logView;
};

}

using namespace KSpread;


TestRunner::TestRunner():
  KDialogBase( KDialogBase::Plain, "Internal Tests", KDialogBase::Close,
  KDialogBase::Close )
{
  d = new Private;

  QFrame* mainWidget = plainPage();
  QGridLayout* layout = new QGridLayout( mainWidget );
  layout->setMargin(KDialog::marginHint());
  layout->setSpacing(KDialog::spacingHint());
  setMinimumSize( 360, 230 );

  QLabel* typeLabel = new QLabel( "Type of Test:", mainWidget );
  layout->addWidget( typeLabel, 0, 0 );

  d->testType = new KComboBox( mainWidget );
  layout->addWidget( d->testType, 0, 1 );

  QSpacerItem* spacerItem = new QSpacerItem( 10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum );
  layout->addItem( spacerItem, 0, 2 );

  d->runButton = new KPushButton( "Run", mainWidget );
  layout->addWidget( d->runButton, 0, 3 );

  d->logView = new QTextEdit( mainWidget );
  layout->addWidget( d->logView, 2, 2, 0, 3 );
  d->logView->setTextFormat( Qt::LogText );

  QObject::connect( d->runButton, SIGNAL( clicked() ), this, SLOT( runTest() ) );

  // add all tests here !!
  addTester( new ValueTester() );
  // addTester( new StyleClusterTester() );
  addTester( new FormulaParserTester() );
  addTester( new FormulaEvalTester() );
  addTester( new FormulaOasisConversionTester() );
}

TestRunner::~TestRunner()
{
  QHash<QString, Tester*>::Iterator it( d->testers.begin() );
  QHash<QString, Tester*>::Iterator end( d->testers.end() );
  for( ; it != end; ++it )
    delete it.value();
  delete d;
}

void TestRunner::addTester( Tester* tester )
{
  if( !tester ) return;
  d->testers.insert( tester->name(), tester );
  d->testType->insertItem( d->testType->count(), tester->name() );
}

void TestRunner::runTest()
{
  QString testName = d->testType->currentText();
  Tester* tester = d->testers[ testName ];
  if( tester )
  {
    d->logView->clear();
    d->logView->append( QString("Test: %1").arg( testName ) );

    QApplication::setOverrideCursor(Qt::WaitCursor);
    tester->run();
    QApplication::restoreOverrideCursor();

    QStringList errorList = tester->errors();
    if( tester->failed() )
    {
      d->logView->append( QString( "%1 tests, <b>%2 failed.</b>").arg( tester->count() ).
        arg(  tester->failed() ) );
      for( int k = 0; k < errorList.count(); k++ )
        d->logView->append( errorList[k] );
    }
    else
      d->logView->append( QString( "%1 tests, everything is OK. ").arg( tester->count() ) );

    d->logView->append( "Test finished." );
  }
}

#include "testrunner.moc"
