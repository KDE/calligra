/* This file is part of the KDE project
   Copyright (C) 1999,2000,2001 Montel Laurent <lmontel@mandrakesoft.com>

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



#include "koPrintPreview.h"
#include <kdialogbase.h>
#include <klibloader.h>
#include <qwidget.h>
#include <klocale.h>
#include <kparts/mainwindow.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <qgrid.h>
#include <qfile.h>
#include <ktrader.h>
#include <krun.h>
#include <kparts/mainwindow.h>

#include <unistd.h>

void KoPrintPreview::preview(QWidget* parent, const char* name, const QString & tmpFile )
{
    KTrader::OfferList offers = KTrader::self()->query("application/postscript", "'KParts/ReadOnlyPart' in ServiceTypes");
    //QWidget *contents = new QWidget(&dialog);
    //QGridLayout *grid1 = new QGridLayout(contents,10,3,15,7); 

    // Try to find a postscript component first
    KLibFactory *factory = 0;
    KParts::ReadOnlyPart *m_pPartPreview=0;
    KTrader::OfferList::Iterator it(offers.begin());
    for( ; it != offers.end(); ++it)
	{
	  KService::Ptr ptr = (*it);
	  factory = KLibLoader::self()->factory( ptr->library().latin1() );
	  if (factory)
	    {
              KDialogBase dialog( parent, "preview", true, i18n("Preview"), KDialogBase::Ok);
	      m_pPartPreview = static_cast<KParts::ReadOnlyPart *>(factory->create(&dialog, ptr->name().latin1(), "KParts::ReadOnlyPart"));
	      //grid1->addMultiCellWidget(m_pPartPreview->widget(),0,8,0,2);
	      
	      m_pPartPreview->openURL( tmpFile );
	      dialog.setMainWidget(m_pPartPreview->widget());
	      dialog.resize(500,300);
	      dialog.exec();
	      
              unlink( QFile::encodeName(tmpFile) );
	      return;
	    }
	}
    // No component worked, or the query returned none -> run separate application
    KRun::runURL(tmpFile,"application/postscript");
    // Note: the temp file won't be deleted :(
}

