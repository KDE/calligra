/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2005 Laurent Montel <montel@kde.org>
             (C) 1998-1999 Torben Weis <weis@kde.org>
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

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QLayout>
#include <q3buttongroup.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>

#include <knumvalidator.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include "Canvas.h"
#include "Doc.h"
#include "Localization.h"
#include "Selection.h"
#include "Sheet.h"
#include "View.h"

// commands
#include "commands/Manipulator.h"

#include "ValidityDialog.h"

using namespace KSpread;

DlgValidity::DlgValidity(View* parent,const char* name , const QRect &_marker )
  :KPageDialog(parent)

{
  setFaceType( Tabbed );
  setCaption( i18n("Validity") );
  setObjectName( name );
  setModal( true );
  setButtons( User2|User1|Cancel );
  setDefaultButton( User1 );
  setButtonGuiItem( User1, KStandardGuiItem::ok() );
  setButtonGuiItem( User2, KGuiItem( i18n("Clear &All") ) );

  m_pView=parent;
  marker=_marker;
  QFrame *page1 = new QFrame();
  addPage(page1, i18n("&Values"));
  QVBoxLayout *lay1 = new QVBoxLayout( page1 );
  lay1->setMargin(KDialog::marginHint());
  lay1->setSpacing(KDialog::spacingHint());

  QGroupBox* tmpQButtonGroup = new QGroupBox( i18n("Validity Criteria"), page1 );
/*  tmpQButtonGroup->layout()->setSpacing(KDialog::spacingHint());
  tmpQButtonGroup->layout()->setMargin(KDialog::marginHint());*/
  QGridLayout *grid1 = new QGridLayout(tmpQButtonGroup);

  QLabel *tmpQLabel = new QLabel( tmpQButtonGroup );
  tmpQLabel->setText(i18n("Allow:" ));
  grid1->addWidget(tmpQLabel,0,0);

  chooseType=new QComboBox(tmpQButtonGroup);
  grid1->addWidget(chooseType,0,1);
  QStringList listType;
  listType+=i18n("All");
  listType+=i18n("Number");
  listType+=i18n("Integer");
  listType+=i18n("Text");
  listType+=i18n("Date");
  listType+=i18n("Time");
  listType+=i18n("Text Length");
  listType+=i18n("List");
  chooseType->insertItems( 0,listType);
  chooseType->setCurrentIndex(0);

  allowEmptyCell = new QCheckBox( i18n( "Allow blanks" ), tmpQButtonGroup );
  grid1->addWidget(allowEmptyCell,1,0);

  tmpQLabel = new QLabel( tmpQButtonGroup );
  tmpQLabel->setText(i18n("Data:" ));
  grid1->addWidget(tmpQLabel,2,0);

  choose=new QComboBox(tmpQButtonGroup);
  grid1->addWidget(choose,2,1);
  QStringList list;
  list+=i18n("equal to");
  list+=i18n("greater than");
  list+=i18n("less than");
  list+=i18n("equal to or greater than");
  list+=i18n("equal to or less than");
  list+=i18n("between");
  list+=i18n("different from");
  list+=i18n("different to");
  choose->insertItems( 0,list);
  choose->setCurrentIndex(0);

  edit1 = new QLabel( tmpQButtonGroup );
  edit1->setText(i18n("Minimum:" ));
  grid1->addWidget(edit1,3,0);

  val_min=new QLineEdit(tmpQButtonGroup);
  grid1->addWidget(val_min,3,1);
  val_min->setValidator( new KFloatValidator( val_min ) );

  edit2 = new QLabel( tmpQButtonGroup );
  edit2->setText(i18n("Maximum:" ));
  grid1->addWidget(edit2,4,0);

  val_max=new QLineEdit(tmpQButtonGroup);
  grid1->addWidget(val_max,4,1);
  val_max->setValidator( new KFloatValidator( val_max ) );
  lay1->addWidget(tmpQButtonGroup);

  //Apply minimum width of column1 to avoid horizontal move when changing option
  //A bit ugly to apply text always, but I couldn't get a label->QFontMetrix.boundingRect("text").width()
  //to give mew the correct results - Philipp
  edit2->setText( i18n( "Date:" ) );
  grid1->addItem(new QSpacerItem( edit2->width(), 0), 0, 0 );
  edit2->setText( i18n( "Date minimum:" ) );
  grid1->addItem(new QSpacerItem( edit2->width(), 0), 0, 0 );
  edit2->setText( i18n( "Date maximum:" ) );
  grid1->addItem(new QSpacerItem( edit2->width(), 0), 0, 0 );
  edit2->setText( i18n( "Time:" ) );
  grid1->addItem(new QSpacerItem( edit2->width(), 0), 0, 0 );
  edit2->setText( i18n( "Time minimum:" ) );
  grid1->addItem(new QSpacerItem( edit2->width(), 0), 0, 0 );
  edit2->setText( i18n( "Time maximum:" ) );
  grid1->addItem(new QSpacerItem( edit2->width(), 0), 0, 0 );
  edit2->setText( i18n( "Minimum:" ) );
  grid1->addItem(new QSpacerItem( edit2->width(), 0), 0, 0 );
  edit2->setText( i18n( "Maximum:" ) );
  grid1->addItem(new QSpacerItem( edit2->width(), 0), 0, 0 );
  edit2->setText( i18n( "Number:" ) );
  grid1->addItem(new QSpacerItem( edit2->width(), 0), 0, 0 );

  validityList = new QTextEdit( tmpQButtonGroup );
  grid1->addWidget(validityList,2, 4,1, 1);

  validityLabelList = new QLabel( tmpQButtonGroup );
  validityLabelList->setText(i18n("Entries:" ));
  grid1->addWidget(validityLabelList,2,0);

  QFrame *page2 = new QFrame();
  addPage(page2, i18n("&Error Alert"));

  lay1 = new QVBoxLayout( page2 );
  lay1->setMargin(KDialog::marginHint());
  lay1->setSpacing(KDialog::spacingHint());

  tmpQButtonGroup = new Q3ButtonGroup( 0, Qt::Vertical, i18n("Contents"), page2, "ButtonGroup_2" );
  tmpQButtonGroup->layout()->setSpacing(KDialog::spacingHint());
  tmpQButtonGroup->layout()->setMargin(KDialog::marginHint());
  QGridLayout *grid2 = new QGridLayout(tmpQButtonGroup);

  displayMessage = new QCheckBox(i18n( "Show error message when invalid values are entered" ),tmpQButtonGroup );
  displayMessage->setChecked( true );
  grid2->addWidget(displayMessage,0, 0,0, 1);

  tmpQLabel = new QLabel( tmpQButtonGroup );
  tmpQLabel->setText(i18n("Action:" ));
  grid2->addWidget(tmpQLabel,1,0);

  chooseAction=new QComboBox(tmpQButtonGroup);
  grid2->addWidget(chooseAction,1,1);
  QStringList list2;
  list2+=i18n("Stop");
  list2+=i18n("Warning");
  list2+=i18n("Information");
  chooseAction->insertItems( 0,list2);
  chooseAction->setCurrentIndex(0);
  tmpQLabel = new QLabel( tmpQButtonGroup );
  tmpQLabel->setText(i18n("Title:" ));
  grid2->addWidget(tmpQLabel,2,0);

  title=new QLineEdit(  tmpQButtonGroup);
  grid2->addWidget(title,2,1);

  tmpQLabel = new QLabel( tmpQButtonGroup );
  tmpQLabel->setText(i18n("Message:" ));
  grid2->addWidget(tmpQLabel,3,0);

  message =new QTextEdit( tmpQButtonGroup);
  grid2->addWidget(message,3, 4,1, 1);
  lay1->addWidget(tmpQButtonGroup);

  QFrame *page3 = new QFrame();
  addPage(page3, i18n("Input Help"));
  lay1 = new QVBoxLayout( page3 );
  lay1->setMargin(KDialog::marginHint());
  lay1->setSpacing(KDialog::spacingHint());

  tmpQButtonGroup = new Q3ButtonGroup( 0, Qt::Vertical, i18n("Contents"), page3, "ButtonGroup_2" );
  tmpQButtonGroup->layout()->setSpacing(KDialog::spacingHint());
  tmpQButtonGroup->layout()->setMargin(KDialog::marginHint());

  QGridLayout *grid3 = new QGridLayout(tmpQButtonGroup);

  displayHelp = new QCheckBox(i18n( "Show input help when cell is selected" ),tmpQButtonGroup );
  displayMessage->setChecked( false );
  grid3->addWidget(displayHelp,0, 0,0, 1);

  tmpQLabel = new QLabel( tmpQButtonGroup );
  tmpQLabel->setText(i18n("Title:" ));
  grid3->addWidget(tmpQLabel,2,0);

  titleHelp=new QLineEdit(  tmpQButtonGroup);
  grid3->addWidget(titleHelp,2,1);

  tmpQLabel = new QLabel( tmpQButtonGroup );
  tmpQLabel->setText(i18n("Message:" ));
  grid3->addWidget(tmpQLabel,3,0);

  messageHelp =new QTextEdit( tmpQButtonGroup);
  grid3->addWidget(messageHelp,3, 4,1, 1);
  lay1->addWidget(tmpQButtonGroup);

  connect(choose,SIGNAL(activated(int )),this,SLOT(changeIndexCond(int)));
  connect(chooseType,SIGNAL(activated(int )),this,SLOT(changeIndexType(int)));
  connect( this, SIGNAL(user1Clicked()), SLOT(OkPressed()) );
  connect( this, SIGNAL(user2Clicked()), SLOT(clearAllPressed()) );
  init();
}

void DlgValidity::displayOrNotListOfValidity( bool _displayList)
{
    if ( _displayList )
    {
        validityList->show();
        validityLabelList->show();
        edit1->hide();
        val_min->hide();
        edit2->hide();
        val_max->hide();
    }
    else
    {
        validityList->hide();
        validityLabelList->hide();
        edit1->show();
        val_min->show();
        edit2->show();
        val_max->show();
    }
}

void DlgValidity::changeIndexType(int _index)
{
    bool activate = ( _index!=0 );
    message->setEnabled(activate);
    title->setEnabled(activate);
    chooseAction->setEnabled( activate );
    displayHelp->setEnabled(activate);
    messageHelp->setEnabled(activate);
    titleHelp->setEnabled(activate);
    if ( _index == 7 )
        displayOrNotListOfValidity( true );
    else
        displayOrNotListOfValidity( false );

    switch(_index)
    {
    case 0:
        edit1->setText("");
        edit2->setText("");
        val_max->setEnabled(false);
        val_min->setEnabled(false);
        edit1->setEnabled(false);
        edit2->setEnabled(false);
        choose->setEnabled(false);
        break;
    case 1:
        val_min->setEnabled(true);
        edit1->setEnabled(true);
        choose->setEnabled(true);
        val_min->setValidator( new KFloatValidator( val_min ) );
        val_max->setValidator( new KFloatValidator( val_max ) );
        if( choose->currentIndex()<=4)
        {
            edit1->setText(i18n("Number:"));
            edit2->setText("");
            edit2->setEnabled(false);
            val_max->setEnabled(false);
        }
        else
        {
            edit1->setText(i18n("Minimum:" ));
            edit2->setText(i18n("Maximum:" ));
            edit2->setEnabled(true);
            val_max->setEnabled(true);
        }
        break;
    case 2:
    case 6:
        val_min->setEnabled(true);
        edit1->setEnabled(true);
        choose->setEnabled(true);
        val_min->setValidator( new KIntValidator( val_min ) );
        val_max->setValidator( new KIntValidator( val_max ) );
        if( choose->currentIndex()<=4)
        {
            edit1->setText(i18n("Number:"));
            edit2->setText("");
            edit2->setEnabled(false);
            val_max->setEnabled(false);
        }
        else
        {
            edit1->setText(i18n("Minimum:" ));
            edit2->setText(i18n("Maximum:" ));
            edit2->setEnabled(true);
            val_max->setEnabled(true);
        }
        break;

    case 3:
        edit1->setText("");
        edit2->setText("");
        val_max->setEnabled(false);
        val_min->setEnabled(false);
        choose->setEnabled(false);
        edit1->setEnabled(false);
        edit2->setEnabled(false);
        break;
    case 4:
        edit1->setText(i18n("Date:"));
        edit2->setText("");
        val_min->setEnabled(true);
        edit1->setEnabled(true);
        choose->setEnabled(true);

        val_min->setValidator(0);
        val_max->setValidator(0);
        if( choose->currentIndex()<=4)
        {
            edit1->setText(i18n("Date:"));
            edit2->setText("");
            edit2->setEnabled(false);
            val_max->setEnabled(false);
        }
        else
        {
            edit1->setText(i18n("Date minimum:"));
            edit2->setText(i18n("Date maximum:"));
            edit2->setEnabled(true);
            val_max->setEnabled(true);
        }
        break;
    case 5:
        val_min->setEnabled(true);
        edit1->setEnabled(true);
        choose->setEnabled(true);
        val_min->setValidator(0);
        val_max->setValidator(0);
        if( choose->currentIndex()<=4)
        {
            edit1->setText(i18n("Time:"));
            edit2->setText("");
            edit2->setEnabled(false);
            val_max->setEnabled(false);
        }
        else
        {
            edit1->setText(i18n("Time minimum:"));
            edit2->setText(i18n("Time maximum:"));
            edit2->setEnabled(true);
            val_max->setEnabled(true);
        }
        break;
    }
    if ( width() < sizeHint().width() )
        resize( sizeHint() );
}

void DlgValidity::changeIndexCond(int _index)
{
  switch(_index)
  {
   case 0:
   case 1:
   case 2:
   case 3:
   case 4:
    val_max->setEnabled(false);
    if(chooseType->currentIndex()==1 ||chooseType->currentIndex()==2
       ||chooseType->currentIndex()==6)
      edit1->setText(i18n("Number:"));
    else if( chooseType->currentIndex()==3)
      edit1->setText("");
    else if( chooseType->currentIndex()==4)
      edit1->setText(i18n("Date:"));
    else if( chooseType->currentIndex()==5)
      edit1->setText(i18n("Time:"));
    edit2->setText("");
    edit2->setEnabled(false);
    break;
   case 5:
   case 6:
    val_max->setEnabled(true);
    edit2->setEnabled(true);
    edit1->setEnabled(true);
    if(chooseType->currentIndex()==1 || chooseType->currentIndex()==2
       || chooseType->currentIndex()==6)
    {
      edit1->setText(i18n("Minimum:" ));
      edit2->setText(i18n("Maximum:" ));
    }
    else if(chooseType->currentIndex()==3)
    {
      edit1->setText("");
      edit2->setText("");
    }
    else if(chooseType->currentIndex()==4)
    {
      edit1->setText(i18n("Date minimum:"));
      edit2->setText(i18n("Date maximum:"));
    }
    else if(chooseType->currentIndex()==5)
    {
      edit1->setText(i18n("Time minimum:"));
      edit2->setText(i18n("Time maximum:"));
    }
    break;
  }
}

void DlgValidity::init()
{
  Validity validity = Cell( m_pView->activeSheet(), marker.topLeft() ).validity();
  if ( !validity.isEmpty() )
  {
    message->setPlainText(validity.message());
    title->setText(validity.title());
    QString tmp;
    switch( validity.restriction())
    {
     case Validity::None:
      chooseType->setCurrentIndex(0);
      break;
     case Validity::Number:
      chooseType->setCurrentIndex(1);
      if(validity.condition() >=5 )
        val_max->setText(tmp.setNum(validity.maximumValue()));
      val_min->setText(tmp.setNum(validity.minimumValue()));
      break;
     case Validity::Integer:
      chooseType->setCurrentIndex(2);
      if(validity.condition() >=5 )
        val_max->setText(tmp.setNum(validity.maximumValue()));
      val_min->setText(tmp.setNum(validity.minimumValue()));
      break;
     case Validity::TextLength:
      chooseType->setCurrentIndex(6);
      if(validity.condition() >=5 )
        val_max->setText(tmp.setNum(validity.maximumValue()));
      val_min->setText(tmp.setNum(validity.minimumValue()));
      break;
     case Validity::Text:
      chooseType->setCurrentIndex(3);
      break;
     case Validity::Date:
      chooseType->setCurrentIndex(4);
      val_min->setText(m_pView->doc()->locale()->formatDate(validity.minimumDate(),true));
      if(validity.condition() >=5 )
        val_max->setText(m_pView->doc()->locale()->formatDate(validity.maximumDate(),true));
      break;
     case Validity::Time:
      chooseType->setCurrentIndex(5);
      val_min->setText(m_pView->doc()->locale()->formatTime(validity.minimumTime(),true));
      if(validity.condition() >=5 )
        val_max->setText(m_pView->doc()->locale()->formatTime(validity.maximumTime(),true));
      break;
     case Validity::List:
     {
         chooseType->setCurrentIndex(7);
         QStringList lst =validity.validityList();
         QString tmp;
         for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
             tmp +=( *it ) + '\n';
         }
         validityList->setText( tmp );
     }
      break;
     default :
      chooseType->setCurrentIndex(0);
      break;
    }
    switch (validity.action())
    {
      case Validity::Stop:
      chooseAction->setCurrentIndex(0);
      break;
      case Validity::Warning:
      chooseAction->setCurrentIndex(1);
      break;
      case Validity::Information:
      chooseAction->setCurrentIndex(2);
      break;
     default :
      chooseAction->setCurrentIndex(0);
      break;
    }
    switch ( validity.condition() )
    {
      case Conditional::Equal:
      choose->setCurrentIndex(0);
      break;
      case Conditional::Superior:
      choose->setCurrentIndex(1);
      break;
      case Conditional::Inferior:
      choose->setCurrentIndex(2);
      break;
      case Conditional::SuperiorEqual:
      choose->setCurrentIndex(3);
      break;
      case Conditional::InferiorEqual:
      choose->setCurrentIndex(4);
      break;
      case Conditional::Between:
      choose->setCurrentIndex(5);
      break;
      case Conditional::Different:
      choose->setCurrentIndex(6);
      break;
      case Conditional::DifferentTo:
      choose->setCurrentIndex(7);
      break;
     default :
      choose->setCurrentIndex(0);
      break;
    }
    displayMessage->setChecked( validity.displayMessage() );
    allowEmptyCell->setChecked( validity.allowEmptyCell() );
    titleHelp->setText( validity.titleInfo() );
    messageHelp->setPlainText( validity.messageInfo() );
    displayHelp->setChecked( validity.displayValidationInformation() );
  }
  changeIndexType(chooseType->currentIndex()) ;
  changeIndexCond(choose->currentIndex()) ;
}

void DlgValidity::clearAllPressed()
{
  val_min->setText("");
  val_max->setText("");
  message->setPlainText("");
  title->setText("");
  displayMessage->setChecked( true );
  allowEmptyCell->setChecked( false );
  choose->setCurrentIndex(0);
  chooseType->setCurrentIndex(0);
  chooseAction->setCurrentIndex(0);
  changeIndexType(0);
  changeIndexCond(0);
  messageHelp->setPlainText("" );
  titleHelp->setText( "" );
  validityList->setText( "" );
  displayHelp->setChecked( false );
}

void DlgValidity::OkPressed()
{
  Validity validity;
  if( chooseType->currentIndex()==1)
  {
    bool ok;
    val_min->text().toDouble(&ok);
    if(! ok)
    {
      KMessageBox::error( this , i18n("This is not a valid value."),i18n("Error"));
      val_min->setText("");
      return;
    }
    val_max->text().toDouble(&ok);
    if(! ok && choose->currentIndex() >=5 && choose->currentIndex()< 7)
    {
      KMessageBox::error( this , i18n("This is not a valid value."),i18n("Error"));
      val_max->setText("");
      return;
    }
  }
  else if( chooseType->currentIndex()==2 || chooseType->currentIndex()==6)
  {
    bool ok;
    val_min->text().toInt(&ok);
    if(! ok)
    {
      KMessageBox::error( this , i18n("This is not a valid value."),i18n("Error"));
      val_min->setText("");
      return;
    }
    val_max->text().toInt(&ok);
    if(! ok && choose->currentIndex() >=5 && choose->currentIndex()< 7)
    {
      KMessageBox::error( this , i18n("This is not a valid value."),i18n("Error"));
      val_max->setText("");
      return;
    }
  }
  else  if(  chooseType->currentIndex()==5)
  {
    if(! m_pView->doc()->locale()->readTime(val_min->text()).isValid())
    {
      KMessageBox::error( this , i18n("This is not a valid time."),i18n("Error"));
      val_min->setText("");
      return;
    }
    if(! m_pView->doc()->locale()->readTime(val_max->text()).isValid() && choose->currentIndex()  >=5)
    {
      KMessageBox::error( this , i18n("This is not a valid time."),i18n("Error"));
      val_max->setText("");
      return;
    }
  }
  else  if(  chooseType->currentIndex()==4)
  {
    if(! m_pView->doc()->locale()->readDate(val_min->text()).isValid())
    {
      KMessageBox::error( this , i18n("This is not a valid date."),i18n("Error"));
      val_min->setText("");
      return;
    }
    if(! m_pView->doc()->locale()->readDate(val_max->text()).isValid() && choose->currentIndex()  >=5 )
    {
      KMessageBox::error( this , i18n("This is not a valid date."),i18n("Error"));
      val_max->setText("");
      return;
    }
  }
  else if ( chooseType->currentIndex()==7 )
  {
      //Nothing
  }

  if( chooseType->currentIndex()==0)
  {//no validity
    validity.setRestriction( Validity::None );
    validity.setAction( Validity::Stop );
    validity.setCondition( Conditional::Equal );
    validity.setMessage( message->toPlainText() );
    validity.setTitle( title->text() );
    validity.setMinimumValue( 0 );
    validity.setMaximumValue( 0 );
    validity.setMinimumTime( QTime(0,0,0) );
    validity.setMaximumTime( QTime(0,0,0) );
    validity.setMinimumDate( QDate(0,0,0) );
    validity.setMaximumDate( QDate(0,0,0) );
  }
  else
  {
    switch( chooseType->currentIndex())
    {
     case 0:
      validity.setRestriction( Validity::None );
      break;
     case 1:
      validity.setRestriction( Validity::Number );
      break;
     case 2:
      validity.setRestriction( Validity::Integer );
      break;
     case 3:
      validity.setRestriction( Validity::Text );
      break;
     case 4:
      validity.setRestriction( Validity::Date );
      break;
     case 5:
      validity.setRestriction( Validity::Time );
      break;
     case 6:
      validity.setRestriction( Validity::TextLength );
      break;
     case 7:
      validity.setRestriction( Validity::List );
      break;

     default :
      break;
    }
    switch (chooseAction->currentIndex())
    {
     case 0:
       validity.setAction( Validity::Stop );
      break;
     case 1:
       validity.setAction( Validity::Warning );
      break;
     case 2:
       validity.setAction( Validity::Information );
      break;
     default :
      break;
    }
    switch ( choose->currentIndex())
    {
     case 0:
       validity.setCondition( Conditional::Equal );
      break;
     case 1:
       validity.setCondition( Conditional::Superior );
      break;
     case 2:
       validity.setCondition( Conditional::Inferior );
      break;
     case 3:
       validity.setCondition( Conditional::SuperiorEqual );
      break;
     case 4:
       validity.setCondition( Conditional::InferiorEqual );
      break;
     case 5:
       validity.setCondition( Conditional::Between );
      break;
     case 6:
       validity.setCondition( Conditional::Different );
      break;
     case 7:
       validity.setCondition( Conditional::DifferentTo );
      break;
     default :
      break;
    }
    validity.setMessage( message->toPlainText() );
    validity.setTitle( title->text() );
    validity.setMinimumValue( 0 );
    validity.setMaximumValue( 0 );
    validity.setMinimumTime( QTime(0,0,0) );
    validity.setMaximumTime( QTime(0,0,0) );
    validity.setMinimumDate( QDate(0,0,0) );
    validity.setMaximumDate( QDate(0,0,0) );

    if( chooseType->currentIndex()==1)
    {
      if(choose->currentIndex()  <5)
      {
        validity.setMinimumValue( val_min->text().toDouble() );
      }
      else
      {
        validity.setMinimumValue( qMin(val_min->text().toDouble(),val_max->text().toDouble()) );
        validity.setMaximumValue( qMax(val_max->text().toDouble(),val_min->text().toDouble()) );
      }
    }
    else if( chooseType->currentIndex()==2 || chooseType->currentIndex()==6)
    {
      if(choose->currentIndex()  <5)
      {
        validity.setMinimumValue( val_min->text().toInt() );
      }
      else
      {
        validity.setMinimumValue( qMin(val_min->text().toInt(),val_max->text().toInt()) );
        validity.setMaximumValue( qMax(val_max->text().toInt(),val_min->text().toInt()) );
      }
    }
    else  if(  chooseType->currentIndex()==4)
    {
      if(choose->currentIndex()  <5)
      {
        validity.setMinimumDate( m_pView->doc()->locale()->readDate(val_min->text()) );
      }
      else
      {
        if(m_pView->doc()->locale()->readDate(val_min->text())<m_pView->doc()->locale()->readDate(val_max->text()))
        {
          validity.setMinimumDate( m_pView->doc()->locale()->readDate(val_min->text()) );
          validity.setMaximumDate( m_pView->doc()->locale()->readDate(val_max->text()) );
        }
        else
        {
          validity.setMinimumDate( m_pView->doc()->locale()->readDate(val_max->text()) );
          validity.setMaximumDate( m_pView->doc()->locale()->readDate(val_min->text()) );
        }
      }
    }
    else  if(  chooseType->currentIndex()==5)
    {
      if(choose->currentIndex()  <5)
      {
        validity.setMinimumTime( m_pView->doc()->locale()->readTime(val_min->text()) );
      }
      else
      {
        if(m_pView->doc()->locale()->readTime(val_min->text())<m_pView->doc()->locale()->readTime(val_max->text()))
        {
          validity.setMaximumTime( m_pView->doc()->locale()->readTime(val_max->text()) );
          validity.setMinimumTime( m_pView->doc()->locale()->readTime(val_min->text()) );
        }
        else
        {
          validity.setMaximumTime( m_pView->doc()->locale()->readTime(val_min->text()) );
          validity.setMinimumTime( m_pView->doc()->locale()->readTime(val_max->text()) );
        }
      }
    }
    else if ( chooseType->currentIndex()==7 )
    {
      validity.setValidityList( validityList->toPlainText().split( '\n', QString::SkipEmptyParts ) );
    }
  }
  validity.setDisplayMessage( displayMessage->isChecked() );
  validity.setAllowEmptyCell( allowEmptyCell->isChecked() );
  validity.setDisplayValidationInformation( displayHelp->isChecked() );
  validity.setMessageInfo( messageHelp->toPlainText() );
  validity.setTitleInfo( titleHelp->text() );

  ValidityManipulator* manipulator = new ValidityManipulator();
  manipulator->setSheet( m_pView->activeSheet() );
  manipulator->setValidity( validity );
  manipulator->add( *m_pView->selection() );
  manipulator->execute();

  accept();
}

#include "ValidityDialog.moc"
