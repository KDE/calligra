/* This file is part of the KDE project
   Copyright (C) 2001 Eva Brucherseifer <eva@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qtextcodec.h>

#include <kapplication.h>
#include <kglobal.h>
#include <klocale.h>
#include <kurlrequester.h>

#include <exportdialog.h>
#include <exportwidget.h>

ExportDialog::ExportDialog( QWidget *parent, const char *name )
  : KDialogBase( parent, name, true, i18n("Export Sheet to HTML"), Ok|Cancel, No, true ), m_mainwidget( new ExportWidget( this ) )
{
  kapp->restoreOverrideCursor();

  connect( m_mainwidget->mCustomButton, SIGNAL( toggled( bool ) ),
           m_mainwidget->mCustomURL, SLOT( setEnabled( bool ) ) );

  m_mainwidget->mEncodingBox->insertItem( i18n( "Recommended: UTF-8" ) );
  m_mainwidget->mEncodingBox->insertItem( i18n( "Locale (%1)" ).arg( KGlobal::locale()->codecForEncoding()->name() ) );

  m_mainwidget->mCustomURL->setMode( KFile::ExistingOnly );

  setMainWidget( m_mainwidget );
}

ExportDialog::~ExportDialog()
{
  kapp->setOverrideCursor(Qt::waitCursor);
}

QTextCodec *ExportDialog::encoding() const
{
  if( m_mainwidget->mEncodingBox->currentItem() == 1 ) // locale selected
    return KGlobal::locale()->codecForEncoding();

  return QTextCodec::codecForName( "utf8" ); // utf8 is default
}

bool ExportDialog::useBorders() const
{
  return m_mainwidget->mUseBorders->isChecked();
}

QString ExportDialog::customStyleURL() const
{
  QString url = m_mainwidget->mCustomURL->url();
  if( m_mainwidget->mCustomButton->isChecked() && KURL( url ).isValid() )
    return url;

  return QString::null;
}

#include <exportdialog.moc>
