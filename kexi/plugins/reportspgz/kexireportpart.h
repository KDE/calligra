/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)                  
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */


#ifndef _KEXIREPORTPART_H_
#define _KEXIREPORTPART_H_


#include <kexi_export.h>
#include <core/kexipart.h>
#include <core/KexiWindowData.h>

#include <QDomDocument>


/**
 * @short Application Main Window
 * @author Adam Pigg <adam@piggz.co.uk>
 * @version 0.1
 */
class KexiReportPart : public KexiPart::Part
{
	Q_OBJECT
	public:
		/**
		 * Default Constructor
		 */
		KexiReportPart(QObject *parent, const QStringList &l);

		/**
		 * Default Destructor
		 */
		virtual ~KexiReportPart();
		virtual KexiView* createView(QWidget *parent, KexiWindow* win, 
				KexiPart::Item &item, Kexi::ViewMode = Kexi::DataViewMode, QMap<QString,QString>* staticObjectArgs = 0);
		virtual KexiWindowData* createWindowData(KexiWindow* window);
		
		virtual void setupCustomPropertyPanelTabs(KTabWidget *tab);
		
		class TempData : public KexiWindowData
		{
			public:
				TempData(QObject* parent);
				QString document;
				/*! true, if \a document member has changed in previous view. Used on view switching.
				Check this flag to see if we should refresh data for DataViewMode. */
				bool reportSchemaChangedInPreviousView : 1;
				QString name;
		};
		
	private:
		QString loadReport(const QString&);
		class Private;
		Private* d;
};

#endif // _KEXIREPORTPART_H_
