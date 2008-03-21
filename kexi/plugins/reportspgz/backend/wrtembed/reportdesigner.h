/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
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
#ifndef REPORTDESIGNER_H
#define REPORTDESIGNER_H

#include <qwidget.h>
#include <qstring.h>

#include <qcolor.h>
#include <qmap.h>
#include <QVBoxLayout>
#include <QCloseEvent>

#include "parsexmlutils.h"

#include <kexidb/connection.h>
#include <koproperty/set.h>
#include <koproperty/property.h>
#include <kdebug.h>

#include "../../pgzkexireportpart2_export.h"

class ReportGridOptions;
class QDomDocument;
class QGraphicsScene;
class KoRuler;
class KoZoomHandler;
class QGridLayout;
class ReportSectionDetail;
class ReportSection;
class KoUnit;
class ReportScene;
class QGraphicsSceneMouseEvent;
class ReportSceneView;
class ReportWriterSectionData;
class KexiView;

namespace KoProperty
{
	class Editor;
}

//
// Class ReportWindow
//     The ReportWindow is the Window used to display a document in
// the ReportWriterWindow MDI.
//
class PGZKEXIREPORTPART2_LIB_EXPORT ReportDesigner : public QWidget
{
		Q_OBJECT
	public:
		enum Section
		{
			PageHeadFirst = 1,
			PageHeadOdd,
			PageHeadEven,
   			PageHeadLast,
			PageHeadAny,
   			ReportHead,
   			ReportFoot,
      			PageFootFirst,
		      	PageFootOdd,
			PageFootEven,
			PageFootLast,
			PageFootAny
		};
		
		ReportDesigner ( QWidget *, KexiDB::Connection * );
		ReportDesigner ( QWidget *, KexiDB::Connection *, const QString& );
		~ReportDesigner();

		ReportSection* getSection ( ReportDesigner::Section );
		void removeSection ( ReportDesigner::Section );
		void insertSection ( ReportDesigner::Section );
		
		ReportSectionDetail* detailSection() {return detail;}
		void setDetail ( ReportSectionDetail *rsd );
		void deleteDetail();

		void setReportTitle ( const QString & );
		void setReportDataSource ( const QString & );
		void setGridOptions ( bool, int );
		QString reportTitle();
		QString reportDataSource();

		QDomDocument document();

		bool isModified();

		void setConn ( KexiDB::Connection *c ) {conn = c;}
		KexiDB::Connection *theConn() {return conn;}
		bool isConnected() {return conn &&  conn->isConnected();}
		
		QStringList queryList();
		QStringList fieldList();
		
		int pageWidthPx() const;

		QGraphicsScene* activeScene();
		void setActiveScene ( QGraphicsScene* a );
		KoProperty::Set* propertySet() {kDebug() << endl; return set;}
		
		virtual QSize sizeHint() const;

		KoZoomHandler* zoomHandler();
		
		KoUnit pageUnit() const;

		void sectionMousePressEvent ( ReportScene *, QGraphicsSceneMouseEvent * e );
		void sectionMouseReleaseEvent ( ReportSceneView *, QMouseEvent * e );

		void changeSet ( KoProperty::Set * );
		KoProperty::Set* itemPropertySet() {kDebug() << endl; return _itmset;}

		void setModified ( bool = true );
		
		void showScriptEditor();
	public slots:
		
		void slotEditDelete();
		void slotEditCut();
		void slotEditCopy();
		void slotEditPaste();
		void slotEditPaste ( QGraphicsScene *, const QPointF & );

		void slotItemLabel();
		void slotItemField();
		void slotItemText();
		void slotItemLine();
		void slotItemBarcode();
		void slotItemImage();
		void slotItemGraph();
		
		void slotSectionEditor();

		void slotRaiseSelected();
		void slotLowerSelected();
		
	protected:
		virtual void closeEvent ( QCloseEvent * e );

		ReportSection * reportHead;
		ReportSection * pageHeadFirst;
		ReportSection * pageHeadOdd;
		ReportSection * pageHeadEven;
		ReportSection * pageHeadLast;
		ReportSection * pageHeadAny;

		ReportSection * pageFootFirst;
		ReportSection * pageFootOdd;
		ReportSection * pageFootEven;
		ReportSection * pageFootLast;
		ReportSection * pageFootAny;
		ReportSection * reportFoot;
		ReportSectionDetail * detail;

	private:
		class Private;
		Private * const d;
		
		void init();
		bool _modified; // true if this document has been modified, false otherwise
		KexiDB::Connection *conn;
		QStringList pageFormats();

		virtual void resizeEvent ( QResizeEvent * event );

		//Properties
		void createProperties();
		KoProperty::Set* set;
		KoProperty::Set* _itmset;
		KoProperty::Property* _title;
		KoProperty::Property* _dataSource;
		KoProperty::Property* _pageSize;
		KoProperty::Property* _orientation;
		KoProperty::Property* _unit;
		KoProperty::Property* _customHeight;
		KoProperty::Property* _customWidth;
		KoProperty::Property* _leftMargin;
		KoProperty::Property* _rightMargin;
		KoProperty::Property* _topMargin;
		KoProperty::Property* _bottomMargin;
		KoProperty::Property* _showGrid;
		KoProperty::Property* _gridDivisions;
		KoProperty::Property* _gridSnap;
		KoProperty::Property* _labelType;

		QString editorText(const QString&);
		QString _script;
		
		ReportWriterSectionData * sectionData;
		unsigned int selectionCount();

	private slots:
		void slotPropertyChanged ( KoProperty::Set &s, KoProperty::Property &p );
		void slotPageButton_Pressed();

	signals:
		void pagePropertyChanged ( KoProperty::Set &s );
		void propertySetChanged();
		void dirty();
};

#endif

