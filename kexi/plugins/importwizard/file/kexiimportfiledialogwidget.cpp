/* This file is part of the KDE project
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>

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

#include "kexiimportfiledialogwidget.h"
#include "kexiimportfiledialogwidget.moc"
#include <kio/netaccess.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>
#include "filters/kexifiltermanager.h"
#include "kexifileimportwizard.h"
#include <kurlcombobox.h>
#include <kpushbutton.h>

KexiImportFileDialogWidget::KexiImportFileDialogWidget(KexiFilterManager *filterManager,KexiFileImportWizard *wiz,const QString& startDir,const QString& filter,
	QWidget *parent,const char *name,bool modal):  KFileDialog (startDir, filter, parent,
		name, modal),m_load(false),m_filterManager(filterManager),m_wiz(wiz) {
}

KURL KexiImportFileDialogWidget::currentURL() {
	setResult( QDialog::Accepted ); // selectedURL tests for it
	return KFileDialog::selectedURL();
}

        // Return true if the current URL exists, show msg box if not
bool KexiImportFileDialogWidget::checkURL() {
	bool ok = true;
	KURL url = currentURL();
        ok = KIO::NetAccess::exists(url,topLevelWidget()); 
        if ( !ok ) {
		ok=false;
		KMessageBox::error( this, i18n( "The file %1 doesn't exist." ).arg( url.path() ) );
	}
	return ok;
}

void KexiImportFileDialogWidget::accept() {
	kdDebug()<<"KexiImportFileDialogWidget::accep()"<<endl;
	if ( checkURL() ) {
		if (m_load) {
			m_load=false;
			loadPlugin(true);
		} else {
			emit nextPage();
		}
	}
	else	if (m_load) {
		m_load=false;
		loadPlugin(false);
	}
	

}

void KexiImportFileDialogWidget::reject() {
                KFileDialog::reject();
		emit canceled();
        }



void KexiImportFileDialogWidget::setMimePluginMap(const QMap<QString,QString>  map) {
	m_map=map;
}

void KexiImportFileDialogWidget::loadPlugin(bool load) {

        if (!load) {
                emit filterHasBeenLoaded(0);
                return;
        }

        KURL url(selectedURL());

        if ((!url.isValid()) || (url.isEmpty())) {
                kdDebug()<<"INVALID URL"<<endl;
                //show an error message
                emit filterHasBeenLoaded(0);
                return;
        }
        KMimeType::Ptr im=KMimeType::findByURL(url);
        if (!m_map.contains(im->name()))  {
                kdDebug()<<"Unsupported mimetype"<<endl;
                //display an error
                emit filterHasBeenLoaded(0);
                return;
        }

        KexiFilter *filter=m_filterManager->loadFilter(m_map[im->name()],m_wiz);
        emit filterHasBeenLoaded(filter,url);
}

void KexiImportFileDialogWidget::setLoad(bool load) {
	m_load=load;
}


void KexiImportFileDialogWidget::initiateLoading() {
	kdDebug()<<"void KexiImportFileDialogWidget::initiateLoading"<<endl;
	if (locationEdit->currentText().isEmpty()) {
		m_load=false;
		emit filterHasBeenLoaded(0);
	} else {
		m_load=true;
		okButton()->animateClick();
	}
}
