/*

 $Id$

 KCalc

 Copyright (C) Bernd Johannes Wuebben
               wuebben@math.cornell.edu
	       wuebben@kde.org

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

 */

#include "configdlg.h"

#include <klocale.h>
#include <kdialog.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QLabel>
#include <QGridLayout>
#include <QFrame>

// Undefine HAVE_LONG_DOUBLE for Beta 4 since RedHat 5.0 comes with a borken
// glibc

#ifdef HAVE_LONG_DOUBLE
#undef HAVE_LONG_DOUBLE
#endif

ConfigDlg::ConfigDlg(QWidget *parent, const char *name, DefStruct *defstruct)
  : QDialog(parent, name)
{
  defst = defstruct;

  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( KDialog::marginHint() );
  lay1->setSpacing( KDialog::spacingHint() );

  box = new Q3GroupBox(0, Qt::Vertical, i18n("Defaults"), this, "box");
  box->layout()->setSpacing(KDialog::spacingHint());
  box->layout()->setMargin(KDialog::marginHint());

  QGridLayout *grid1 = new QGridLayout(box->layout());
  label1 = new QLabel(box);
  label1->setText(i18n("Foreground color:"));
  grid1->addWidget(label1,0,0);

  button1 = new KColorButton( box );
  button1->setObjectName( "button1" );
  grid1->addWidget(button1,0,1);
  button1->setColor( defst->forecolor );

  connect(button1 , SIGNAL( changed( const QColor & ) ),
             this, SLOT( set_fore_color( const QColor & ) ) );

  label2 = new QLabel(box);
  grid1->addWidget(label2,1,0);
  label2->setText(i18n("Background color:"));

  button2 = new KColorButton( box );
  button2->setObjectName( "button2" );
  grid1->addWidget(button2,1,1);
  button2->setColor( defst->backcolor );

  connect(button2 , SIGNAL( changed( const QColor & ) ),
             this, SLOT( set_background_color( const QColor & ) ) );


  //  connect(button2,SIGNAL(clicked()),this,SLOT(set_background_color()));


  label5 = new QLabel(box);
  grid1->addWidget(label5,2,0);
  label5->setText(i18n("Precision:"));

  int maxprec;
#ifdef HAVE_LONG_DOUBLE
  maxprec = 16 ;
#else
  maxprec = 12 ;
#endif

  precspin = new QSpinBox( box );
  precspin->setRange( 0, maxprec );
  grid1->addWidget(precspin,2,1);

  if( defst->precision <= maxprec)
    precspin->setValue(defst->precision);
  else
    precspin->setValue(maxprec);


  cb = new QCheckBox(box);
  grid1->addWidget(cb,3,0);
  cb->setText(i18n("Set fixed precision at:"));
  if(defst->fixed)
    cb->setChecked(true);

  int fixprec;
#ifdef HAVE_LONG_DOUBLE
  fixprec = 14 ;
#else
  fixprec = 10 ;
#endif

  precspin2 = new QSpinBox( box );
  precspin2->setRange(0,fixprec);
  grid1->addWidget(precspin2,3,1);

  if( defst->fixedprecision <= fixprec)
    precspin2->setValue(defst->fixedprecision);
  else
    precspin2->setValue(fixprec);



  cb2 = new QCheckBox(box);
  grid1->addWidget(cb2,4,0);
  cb2->setText(i18n("Beep on error"));
  if(defst->beep)
    cb2->setChecked(true);


  stylegroup = new Q3ButtonGroup(box,"stylegroup");
  grid1->addWidget(stylegroup,5,7,0,1);
  stylegroup->setFrameStyle(QFrame::NoFrame);

  QGridLayout *grid2 = new QGridLayout(stylegroup);
  grid2->setMargin(KDialog::marginHint());
  grid2->setSpacing(KDialog::spacingHint());

  trigstyle = new QRadioButton(i18n("Trigonometry mode"),stylegroup,"trigstyle");
  grid2->addWidget(trigstyle,0,0);
  trigstyle->adjustSize();
  trigstyle->setChecked(defst->style == 0 );

  statstyle = new QRadioButton(i18n("Statistical mode"),stylegroup,"Stats");
  grid2->addWidget(statstyle,1,0);
  statstyle->adjustSize();
  statstyle->setChecked(defst->style == 1 );

  sheetstyle = new QRadioButton(i18n("Sheet mode"),stylegroup,"Sheet");
  grid2->addWidget(sheetstyle,2,0);
  sheetstyle->adjustSize();

  sheetstyle->setChecked(defst->style == 2 );
  button3 = new QPushButton(stylegroup);
  grid2->addWidget(button3,0,1);
  button3->setText(i18n("Help"));

  connect(button3,SIGNAL(clicked()),this,SLOT(help()));

  lay1->addWidget(box);
  connect(parent,SIGNAL(applyButtonPressed()),SLOT(okButton()));
}

void ConfigDlg::help()
{
}

void ConfigDlg::okButton()
{
  defst->precision = precspin->value();
  defst->fixedprecision = precspin2->value();
  defst->fixed = cb->isChecked();
  defst->beep = cb2->isChecked();


  if( trigstyle->isChecked())
    defst->style = 0;
  else if ( statstyle->isChecked() )
    defst->style = 1;
  else
    defst->style = 2;
}

void ConfigDlg::cancelbutton()
{
  reject();
}

void ConfigDlg::set_fore_color(const QColor &_color)
{
        defst->forecolor=_color;
}

void ConfigDlg::set_background_color( const QColor &_color )
{
        defst->backcolor=_color;
}

#include "configdlg.moc"
