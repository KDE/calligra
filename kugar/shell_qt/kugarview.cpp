/**************************************************************************
						  KugarView.cpp  -  description
							 -------------------
	begin				 : 2003-03-13 11:12:40
	copyright			 : (C) 2003 by Joris Marcillac
	email				 : joris@marcillac.org
**************************************************************************/

/**************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*	This program is distributed in the hope that it will be useful,	  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of	  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU *
*	Library General Public License for more details.		  *
*                                                                         *
*	You should have received a copy of the GNU Library General Public *
*	License along with this library; if not, write to the Free	  *
*	Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,	  *
*	MA 02111-1307, USA						  *
*                                                                         *
**************************************************************************/
#include <qwidget.h>
#include "kugarview.h"
#include <mreportviewer.h>

class KugarViewPrivate : public MReportViewer, public QShared
{
public:

	KugarViewPrivate(QWidget *parent=0, const char *name=0)
		:MReportViewer( parent, name )
	{
	}

	virtual ~KugarViewPrivate()
	{
	}
};

//    Constructs an empty report.
KugarView::KugarView(QWidget *parent, const char *name)
{
    d = new KugarViewPrivate( parent, name );
	Q_CHECK_PTR(d);
}
//	Constructs a copy of \a other.
KugarView::KugarView( const KugarView& other )
    : d( other.d )
{
    d->ref();
}

//    Sets the report equal to \a other.
KugarView& KugarView::operator=( const KugarView& other )
{
    other.d->ref();
    deref();
    d = other.d;
    return *this;
}

void KugarView::deref()
{
    if ( d->deref() ) {
	delete d;
	d = 0;
    }
}

KugarView::~KugarView()
{
    deref();
}

bool KugarView::setReportData(const QString& data)
{
	return d->setReportData( data );
}

bool KugarView::setReportData(QIODevice* dev)
{
	return d->setReportData( dev );
}

bool KugarView::setReportTemplate(const QString &tpl)
{
	return d->setReportTemplate( tpl );
}

bool KugarView::setReportTemplate(QIODevice *dev)
{
	return d->setReportTemplate( dev );
}

bool KugarView::renderReport()
{
	return d->renderReport();
}

void KugarView::clearReport()
{
	d->clearReport();
}

void KugarView::printReport()
{
	d->printReport();
}

void KugarView::show()
{
	d->show();
}


