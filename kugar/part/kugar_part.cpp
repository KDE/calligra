// Copyright (c) 2000 Phil Thompson <phil@river-bank.demon.co.uk>
//
// This file contains the implementation of the Kugar KPart.


#include <kapplication.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kinstance.h>
#include <kaboutdata.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>
#include <qfile.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <qwidget.h>
#include <kfiledialog.h>
#include <kdebug.h>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "kugar.h"

#include "kugar_part.h"
#include "kugar_view.h"
#include "kugar_factory.h"

KugarPart::KugarPart( QWidget *parentWidget, const char *widgetName, QObject* parent ,
                const char* name , bool singleViewMode):KoDocument(parentWidget,widgetName,parent,name,singleViewMode)
{
	setInstance(KugarFactory::global(),false);
	m_reportEngine=new MReportEngine();
        connect(m_reportEngine,SIGNAL(preferedTemplate(const QString &)),
                     SLOT(slotPreferedTemplate(const QString &)));
}

KugarPart::~KugarPart()
{
	m_reportEngine->removeRef();
//	closeURL();
}

bool KugarPart::loadXML( QIODevice *file, const QDomDocument & doc)
{
	bool ok=true;
        if (file)
        {
		file->reset();
		m_reportData=QString(file->readAll());

		if (m_reportData.length()!=0)
		{
			ok=m_reportEngine->setReportData(m_reportData);
//			ok=m_reportEngine->setReportData(doc);
			m_reportEngine->renderReport();
			kdDebug()<<"KugarPart::loadXML: report data set"<<endl;
			if (ok)
			{
				kdDebug()<<m_reportData<<endl;
				QPtrList<KoView> vs= views();
				if (vs.count())
				{
					for (KoView *v=vs.first();v;v=vs.next())
					{
						ok=static_cast<KugarView*>(v->qt_cast("KugarView"))->renderReport();
						if (!ok) break;
					}		
				}
                        }
			if (!ok) KMessageBox::sorry(0,i18n("Invalid data file %1").arg(m_file));
		}
		else
		{
			ok=false;
			KMessageBox::sorry(0,i18n("The zero sized data file %1  can't be rendered").arg(m_file));
		}

        }
        else
	{
		ok=false;
                KMessageBox::sorry(0,i18n("Unable to open data file: %1").arg(m_file));
	}

        return ok;	
}

bool KugarPart::initDoc()
{
        QString filename;
	bool ok=false;
	KURL url=KFileDialog::getOpenURL(QString::null,
                           "*.kud|Kugar Data File");
	if (url.isValid())
	{
		ok=openURL(url);
	}
        return ok;

    // If nothing is loaded, do initialize here
    return TRUE;
}

KoView* KugarPart::createViewInstance( QWidget* parent, const char* name )
{
    KugarView *v=new KugarView( this, parent, name );
    v->renderReport();
    return v;
}


void KugarPart::slotPreferedTemplate(const QString &tpl)
{
//        KURL url(m_forcedUserTemplate.isEmpty()?tpl:m_forcedUserTemplate);
	KURL url(tpl);
        QString localtpl;
        bool isTemp = false;

        if (url.isMalformed())
        {
                if (tpl.find('/') >= 0)
                        localtpl = tpl;
                else
                        localtpl = kapp -> dirs() -> findResource("data","kugar/templates/" + tpl);
        }
        else
        {
                if (KIO::NetAccess::download(url,localtpl))
                        isTemp = true;
                else
                        KMessageBox::sorry(0,i18n("Unable to download template file: %1").arg(url.prettyURL()));
        }

        if (!localtpl.isNull())
        {
                QFile f(localtpl);

                if (f.open(IO_ReadOnly))
                {
                        if (!m_reportEngine -> setReportTemplate(&f))
                                KMessageBox::sorry(0,i18n("Invalid template file: %1").arg(localtpl));

                        f.close();
                }
                else
                        KMessageBox::sorry(0,i18n("Unable to open template file: %1").arg(localtpl));

                if (isTemp)
                        KIO::NetAccess::removeTempFile(localtpl);
        }
}

#include "kugar_part.moc"
