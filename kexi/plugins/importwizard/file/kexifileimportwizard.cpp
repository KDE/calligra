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

K_EXPORT_COMPONENT_FACTORY( kexifileimport, KGenericFactory<KexiFileImportWizard> )
