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
#include <kmessagebox.h>

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
}

KugarPart::~KugarPart()
{
//	closeURL();
}

bool KugarPart::loadXML( QIODevice *file, const QDomDocument & )
{
	bool ok=true;
        if (file)
        {
		file->reset();
		m_reportData=QString(file->readAll());
		if (m_reportData.length()!=0)
		{
			kdDebug()<<m_reportData<<endl;
			QPtrList<KoView> vs= views();
			for (KoView *v=vs.first();v;v=vs.next())
			{
				if (!(static_cast<KugarView*>(v->qt_cast("KugarView"))->renderReport(m_reportData)))
				{
					ok=false;
					break;
				}
			}		
                        
			if (!ok) KMessageBox::sorry(0,i18n("Invalid data file: %1").arg(m_file));
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
    v->renderReport(m_reportData);
    return v;
}

