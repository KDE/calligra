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
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/

// Qt includes
#include <qlayout.h>

// KDE includes
#include <kdebug.h>
#include <kdialog.h>
#include <klocale.h>
#include <qtooltip.h>

// local includes
#include "printerDlg.h"

 KSpreadPrinterDlg::KSpreadPrinterDlg( QWidget *parent, const char *name )
  : KPrintDialogPage( parent, name )
{
  setTitle( i18n( "KSpread Options" ) );
  QVBoxLayout *layout = new QVBoxLayout( this );
  layout->setMargin( KDialog::marginHint() );
  layout->setSpacing( KDialog::spacingHint() );
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
  QToolTip::add( caption, i18n("Choose how many rows and columns with slides you want to have on every pages") );
  layout->addWidget( caption );
  layout->addWidget( new QLabel(i18n("Rows: "), this) );
  layout->addWidget( txtRows );
  layout->addWidget( new QLabel(i18n("Columns: "), this) );
  layout->addWidget( txtColumns );

  drawBorder = new QCheckBox(i18n("Draw border around the slides"), this );
  drawBorder->setChecked( true );
  drawBorder->setEnabled( false );
  layout->addWidget( drawBorder );

  layout->addStretch( 1 );
}

void KSpreadPrinterDlg::getOptions( QMap<QString, QString>& opts, bool )
{
  opts["kde-kspread-printrows"] = QString::number(txtRows->value());
  opts["kde-kspread-printcolumns"] = QString::number(txtColumns->value());
  opts["kde-kspread-printslideborders"] = QString::number(drawBorder->isEnabled() && drawBorder->isChecked());
}

void KSpreadPrinterDlg::setOptions( const QMap<QString, QString>& opts )
{
  if ( opts["kde-kspread-printrows"].isEmpty() )
    txtRows->setValue(1);
  else
    txtRows->setValue((opts["kde-kspread-printrows"]).toInt());
  if ( opts["kde-kspread-printcolumns"].isEmpty() )
    txtColumns->setValue(1);
  else
    txtColumns->setValue((opts["kde-kspread-printcolumns"]).toInt());

  if ( opts["kde-kspread-printslideborders"].isEmpty() )
    drawBorder->setChecked(true);
  else
    drawBorder->setChecked((opts["kde-kspread-printslideborders"]).toInt());
}

bool KSpreadPrinterDlg::isValid( const QString& )
{
  return true;
}

void KSpreadPrinterDlg::txtRows_valueChanged( int new_value)
{
  if ( new_value == 1 && txtColumns->value() == 1 )
    drawBorder->setEnabled( false );
  else
    drawBorder->setEnabled( true );
}
void KSpreadPrinterDlg::txtColumns_valueChanged( int new_value )
{
  if ( new_value == 1 && txtRows->value() == 1 )
    drawBorder->setEnabled( false );
  else
    drawBorder->setEnabled( true );
}

#include "printerDlg.moc"
