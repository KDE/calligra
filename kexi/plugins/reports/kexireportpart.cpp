/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include <kdebug.h>
#include <kgenericfactory.h>

#include "kexiviewbase.h"
#include "keximainwindow.h"
#include "kexiproject.h"
#include <kexipartitem.h>
#include <kexidialogbase.h>

#include <kexidb/connection.h>
#include <kexidb/fieldlist.h>
#include <kexidb/field.h>

#include <form.h>
#include <formIO.h>
#include <formmanager.h>

#include "kexireportview.h"
#include "kexireportpart.h"

KexiReportPart::KexiReportPart(QObject *parent, const char *name, const QStringList &l)
 : KexiPart::Part(parent, name, l)
{
	kexipluginsdbg << "KexiReportPart::KexiReportPart()" << endl;
	m_names["instance"] = i18n("Report");
	m_supportedViewModes = Kexi::DataViewMode | Kexi::DesignViewMode;

/* @todo add configuration for supported factory groups */
	QStringList supportedFactoryGroups;
	supportedFactoryGroups += "kexi-report";
	m_manager = new KFormDesigner::FormManager(this, supportedFactoryGroups, "report_form_manager");
}

KexiReportPart::~KexiReportPart()
{
}

void
KexiReportPart::initPartActions()
{
}

void
KexiReportPart::initInstanceActions()
{
	m_manager->createActions(actionCollectionForMode(Kexi::DesignViewMode));
}

KexiDialogTempData*
KexiReportPart::createTempData(KexiDialogBase* dialog)
{
	return new KexiReportPart::TempData(dialog);
}

KexiViewBase*
KexiReportPart::createView(QWidget *parent, KexiDialogBase* dialog,
	KexiPart::Item &item, int)
{
	kexipluginsdbg << "KexiReportPart::createView()" << endl;
	KexiMainWindow *win = dialog->mainWin();
	if (!win || !win->project() || !win->project()->dbConnection())
		return 0;

	KexiReportView *view = new KexiReportView(win, parent, item.name().latin1(),
		win->project()->dbConnection() );

	return view;
}

//---------------

KexiReportPart::TempData::TempData(QObject* parent)
 : KexiDialogTempData(parent)
{
}

KexiReportPart::TempData::~TempData()
{
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_report, KGenericFactory<KexiReportPart>("kexihandler_report") )

#include "kexireportpart.moc"

