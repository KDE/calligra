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

#include "kexifileimportwizard.h"
#include "kexifileimportwizard.moc"
#include "kexiimportfiledialogwidget.h"

#include <qlayout.h>
#include <kgenericfactory.h>
#include <qobjectlist.h>
#include <kdebug.h>
#include <qpushbutton.h>
#include <kpushbutton.h>
#include <filters/kexifiltermanager.h>

KexiFileImportWizard::KexiFileImportWizard(QObject *parent,
	const char *name,const QStringList &list):KexiImportWizard(parent,name,list) {
}

KexiFileImportWizard::~KexiFileImportWizard() {
}


QWidget* KexiFileImportWizard::openWidget(QWidget *parent, const QString& dialogFilter) {
        QString dir = QString::null;
        QPoint point( 0, 0 );

        KexiImportFileDialogWidget *fileDialog=new KexiImportFileDialogWidget(filterManager(),this,dir, 
		QString::null, parent,"file dialog", false);

        fileDialog->reparent( parent , point );
        fileDialog->setFilter(dialogFilter);

        QObjectList *l = fileDialog->queryList( "QPushButton" );
        QObjectListIt it( *l );
        QObject *obj;
        while ( (obj = it.current()) != 0 ) {
                ++it;
                ((QPushButton*)obj)->hide();
        }
        delete l;

        fileDialog->setSizeGripEnabled ( FALSE );

/*        connect(m_fileDialog, SIGNAL(  okClicked() ),
            this, SLOT (  openFile() ));
        connect(m_fileDialog, SIGNAL(  nextPage() ),
            this, SLOT (  openFile() )); */
	
	return fileDialog;

}

KexiFilter *KexiFileImportWizard::openPageLeft(QWidget *widget, const QMap<QString,QString> mimePluginMap) {
        kdDebug()<<"KexiFileImportWizard"<<endl;
	KexiImportFileDialogWidget *fileDialog=static_cast<KexiImportFileDialogWidget*>
		(widget->qt_cast("KexiImportFileDialogWidget"));
	if (!fileDialog) return 0;

	fileDialog->setMimePluginMap(mimePluginMap);
	fileDialog->setLoad(true);
	fileDialog->initiateLoading();
}

K_EXPORT_COMPONENT_FACTORY( kexifileimport, KGenericFactory<KexiFileImportWizard>("kexifileimport") )
