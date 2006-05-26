/*
*
* Copyright (C) 2005  Fredrik Edemar
*                     f_edemar@linux.se
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*
*/

// Qt includes
#include <QLayout>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <QLabel>

// KDE includes
#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>
#include <QToolTip>

// local includes
#include "KPrPrinterDlg.h"

 KPrPrinterDlg::KPrPrinterDlg( QWidget *parent, const char *name )
  : KPrintDialogPage( parent, name )
{
  setTitle( i18n( "KPresenter Options" ) );
  Q3GridLayout *layout = new Q3GridLayout( this, 2, 1, 11, 6 );
  txtRows = new KIntNumInput(this );
  txtRows->setMinValue(1);
  txtRows->setMaxValue(5);
  txtRows->setValue(1);
  txtColumns = new KIntNumInput(this );
  txtColumns->setMinValue(1);
  txtColumns->setMaxValue(5);
  txtColumns->setValue(1);
  connect( txtRows, SIGNAL( valueChanged( int ) ), this, SLOT( txtRows_valueChanged( int ) ) );
  connect( txtColumns, SIGNAL( valueChanged( int ) ), this, SLOT( txtColumns_valueChanged( int ) ) );

  QLabel *caption = new QLabel(i18n("Slides in the pages:"), this );
  caption->setToolTip( i18n("Choose how many rows and columns with slides you want to have on all pages") );
  layout->addMultiCellWidget( caption, 0, 0, 0, 1 );
  Q3VBoxLayout *l2 = new Q3VBoxLayout( 0, 0, 6 );
  l2->addWidget( new QLabel(i18n("Rows: "), this) );
  l2->addWidget( new QLabel(i18n("Columns: "), this) );
  layout->addLayout( l2, 1, 0 );

  Q3VBoxLayout *l3 = new Q3VBoxLayout( 0, 0, 6 );
  l3->addWidget( txtRows );
  l3->addWidget( txtColumns );
  layout->addLayout( l3, 1, 1 );

  drawBorder = new QCheckBox(i18n("Draw border around the slides"), this );
  drawBorder->setChecked( true );
  drawBorder->setEnabled( false );
  layout->addMultiCellWidget( drawBorder, 2, 2, 0, 1 );
}

void KPrPrinterDlg::getOptions( QMap<QString, QString>& opts, bool )
{
  opts["kde-kpresenter-printrows"] = QString::number(txtRows->value());
  opts["kde-kpresenter-printcolumns"] = QString::number(txtColumns->value());
  opts["kde-kpresenter-printslideborders"] = QString::number(drawBorder->isEnabled() && drawBorder->isChecked());
}

void KPrPrinterDlg::setOptions( const QMap<QString, QString>& opts )
{
  if ( opts["kde-kpresenter-printrows"].isEmpty() )
    txtRows->setValue(1);
  else
    txtRows->setValue((opts["kde-kpresenter-printrows"]).toInt());
  if ( opts["kde-kpresenter-printcolumns"].isEmpty() )
    txtColumns->setValue(1);
  else
    txtColumns->setValue((opts["kde-kpresenter-printcolumns"]).toInt());

  if ( opts["kde-kpresenter-printslideborders"].isEmpty() )
    drawBorder->setChecked(true);
  else
    drawBorder->setChecked((opts["kde-kpresenter-printslideborders"]).toInt());
}

bool KPrPrinterDlg::isValid( const QString& )
{
  return true;
}

void KPrPrinterDlg::txtRows_valueChanged( int new_value)
{
  if ( new_value == 1 && txtColumns->value() == 1 )
    drawBorder->setEnabled( false );
  else
    drawBorder->setEnabled( true );
}
void KPrPrinterDlg::txtColumns_valueChanged( int new_value )
{
  if ( new_value == 1 && txtRows->value() == 1 )
    drawBorder->setEnabled( false );
  else
    drawBorder->setEnabled( true );
}

#include "KPrPrinterDlg.moc"
