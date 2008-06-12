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

#include "krhtmlrender.h"
#include "renderobjects.h"
#include "orutils.h"
#include "barcodes.h"

#include <QPrinter>
#include <QFontMetrics>
#include <QPainter>

#include <parsexmlutils.h>
//#include <labelsizeinfo.h>
#include <kcodecs.h>
#include <KoPageFormat.h>
#include <kdebug.h>

#include <krobjectdata.h>
#include <krtextdata.h>
#include <krbarcodedata.h>
#include <krfielddata.h>
#include <krimagedata.h>
#include <krlabeldata.h>
#include <krlinedata.h>
#include <krchartdata.h>

#include "scripting/krscripthandler.h"
#include <krreportdata.h>
#include <krdetailsectiondata.h>
#include <QResizeEvent>
#include <QApplication>

#include <KDChartAbstractDiagram>
#include <KDChartAbstractCoordinatePlane>
#include <KDChartChart>

//
// KRHtmlRenderPrivate
// This class is the private class that houses all the internal
// variables so we can provide a cleaner interface to the user
// without presenting to them things that they don't need to see
// and may change over time.
//
class KRHtmlRenderPrivate : public QObject
{
	Q_OBJECT
	public:
		KRHtmlRenderPrivate();
		virtual ~KRHtmlRenderPrivate();

		bool _valid;
		QDomDocument _docReport;

		KexiDB::Connection *_conn;

		QString html;
		KRReportData* _reportData;

		int _recordCount;

		orQuery* _query;
	
		KRDetailSectionData * _subtotContextDetail;
		bool _subtotContextPageFooter;

		bool populateData ( const ORDataData &, orData & );

		void renderDetailSection ( KRDetailSectionData & );
		qreal renderSection ( const KRSectionData & );
	
		///Scripting Stuff
		KRScriptHandler *_handler;
		void initEngine();

	signals:
		void enteredGroup(const QString&, const QVariant&);
		void exitedGroup(const QString&, const QVariant&);
		void renderingSection (KRSectionData*, OROPage*, QPointF);
};

KRHtmlRenderPrivate::KRHtmlRenderPrivate()
{
	_valid = false;
	_reportData = 0;

	_query = 0;
	
	_subtotContextPageFooter = false;
	
	
}

KRHtmlRenderPrivate::~KRHtmlRenderPrivate()
{
	if ( _reportData != 0 )
	{
		delete _reportData;
		_reportData = 0;
	}
}

bool KRHtmlRenderPrivate::populateData ( const ORDataData & dataSource, orData &dataTarget )
{

	dataTarget.setQuery ( _query );
	dataTarget.setField ( dataSource.column );
	return true;

}

void KRHtmlRenderPrivate::renderDetailSection ( KRDetailSectionData & detailData )
{
	kDebug() << endl;
	
	if ( detailData.detail != 0 )
	{
		KexiDB::Cursor *curs;
		_subtotContextDetail = &detailData;

		if (_query)
		{
			curs = _query->getQuery();
// 			//TODO init the engine earlier?
			_handler->setSource( _query->getSql());
		}
		if ( curs && !curs->eof() )
		{
			QStringList keys;
			QStringList keyValues;
			bool    status;
			int i = 0, pos = 0, cnt = 0;
			ORDetailGroupSectionData * grp = 0;

			curs->moveFirst();
			if (_query->schema().table() || _query->schema().query() )
			{
				_recordCount = KexiDB::rowCount ( _query->schema() );
			}
			else
			{
				_recordCount = 1;	
			}
			
			kDebug() << "Record Count:" << _recordCount << endl;
		
			for ( i = 0; i < ( int ) detailData.groupList.count(); i++ )
			{
				cnt++;
				grp = detailData.groupList[i];
				QMapIterator<ORDataData,qreal> it ( grp->_subtotCheckPoints );
				while ( it.hasNext() )
				{
					it.next();
					grp->_subtotCheckPoints.insert ( it.key(), 0.0 );
				}
				keys.append ( grp->column );
				if ( !keys[i].isEmpty() ) 
					keyValues.append ( curs->value ( _query->fieldNumber ( keys[i] ) ).toString() );
				else 
					keyValues.append ( QString() );
				
				//Tell interested parties we're about to render a header
				kDebug() << "EMIT1" << endl;
				emit(enteredGroup(keys[i], keyValues[i]));
				
				if ( grp->head )
					renderSection ( * ( grp->head ) );
			}

			do
			{
				int l = curs->at();
				
				kDebug() << "At:" << l << endl;
				
				renderSection ( * ( detailData.detail ) );

				status = curs->moveNext();
				if ( status == true && keys.count() > 0 )
				{
					// check to see where it is we need to start
					pos = -1; // if it's still -1 by the time we are done then no keyValues changed
					for ( i = 0; i < keys.count(); i++ )
					{
						if ( keyValues[i] != curs->value ( _query->fieldNumber ( keys[i] ) ).toString() )
						{
							pos = i;
							break;
						}
					}

					// don't bother if nothing has changed
					if ( pos != -1 )
					{
						// roll back the query and go ahead if all is good
						status = curs->movePrev();
						if ( status == true )
						{
							// print the footers as needed
							// any changes made in this for loop need to be duplicated
							// below where the footers are finished.

							for ( i = cnt - 1; i >= pos; i-- )
							{
								grp = detailData.groupList[i];
								
								if ( grp->foot )
								{
									renderSection ( * ( grp->foot ) );
								}
								
								// reset the sub-total values for this group
								QMapIterator<ORDataData,qreal> it ( grp->_subtotCheckPoints );
								while ( it.hasNext() )
								{
									it.next();
									qreal d = 0.0;
									ORDataData data = it.key();
									KexiDB::Cursor * xqry = _query->getQuery();
									if ( xqry )
									{
//TODO field totals								d = xqry->getFieldTotal ( data.column );
									}
									grp->_subtotCheckPoints.insert ( it.key(), d );
								}

							}
							// step ahead to where we should be and print the needed headers
							// if all is good
							status = curs->moveNext();
					
							if ( status == true )
							{
								for ( i = pos; i < cnt; i++ )
								{
									grp = detailData.groupList[i];
								
									if ( grp->head )
									{	
										renderSection ( * ( grp->head ) );
									}
							
									if ( !keys[i].isEmpty() )
										keyValues[i] = curs->value ( _query->fieldNumber ( keys[i] ) ).toString();
									
									//Tell interested parties thak key values changed
									kDebug() << "EMIT2" << endl;
									emit(enteredGroup(keys[i], keyValues[i]));
								}
							}
						}
					}
				}
			}
			while ( status == true );

			if ( keys.size() > 0 && curs->movePrev() )
			{
				// finish footers
				// duplicated changes from above here
				for ( i = cnt - 1; i >= 0; i-- )
				{
					grp = detailData.groupList[i];
					
					if ( grp->foot )
					{
						renderSection ( * ( grp->foot ) );
						emit(exitedGroup(keys[i], keyValues[i]));
					}
				
					// reset the sub-total values for this group
					QMapIterator<ORDataData,qreal> it ( grp->_subtotCheckPoints );
					while ( it.hasNext() )
					{
						it.next();
						qreal d = 0.0;
						ORDataData data = it.key();
						KexiDB::Cursor * xqry = _query->getQuery();
						if ( xqry )
						{
//TODO field totals							d = xqry->getFieldTotal ( data.column );
						}
						grp->_subtotCheckPoints.insert ( it.key(), d );
					}
				}
			}
		}
		_subtotContextDetail = 0;
	}

}


qreal KRHtmlRenderPrivate::renderSection ( const KRSectionData & sectionData )
{
	_handler->populateEngineParameters(_query->getQuery());
	
	emit (renderingSection(const_cast<KRSectionData*>(&sectionData), 0, QPointF ( 0, 0 )));
	
	//Render section background
	
	html += "<div class=\"" + sectionData.name() + "\" style=\"background-color: " + sectionData.bgColor().name() + "\">";
	
	QList<KRObjectData*> objects = sectionData.objects();
	KRObjectData * elemThis;
	foreach(KRObjectData *ob, objects)
	{
		html += "<div class=\"" + ob->entityName() + "\">";
		elemThis = ob;
		if ( elemThis->type() == KRObjectData::EntityLabel )
		{
			KRLabelData * l = elemThis->toLabel();
			html += l->text();
		}
		else if ( elemThis->type() == KRObjectData::EntityField )
		{
			orData       dataThis;
			KRFieldData* f = elemThis->toField();

			
			QString str = QString::null;
	
			QString cs = f->properties()->property("ControlSource").value().toString();
			if (cs.left(1) == "=")
			{
				if (!cs.contains("PageTotal"))
				{
					str = _handler->evaluate(f->entityName()).toString();
				}
				else
				{
					str = cs.mid(1);
					//TODO _postProcText.append(tb);
				}
			}
			else
			{
				//QString qry = "Data Source";
				QString clm = f->properties()->property("ControlSource").value().toString();
				
				populateData ( f->data(), dataThis );
				str = dataThis.getValue();
			}
			html += str;
		}
		else if ( elemThis->type() == KRObjectData::EntityText )
		{
			orData       dataThis;
			KRTextData * t = elemThis->toText();
			
			populateData ( t->data(), dataThis );

			html += dataThis.getValue();
		}
		else if ( elemThis->type() == KRObjectData::EntityLine )
		{
			//Add lines to html? i think not

		}
		else if ( elemThis->type() == KRObjectData::EntityBarcode )
		{
			//TODO create barcodes as images an save them
		}
		else if ( elemThis->type() == KRObjectData::EntityImage )
		{
			//TODO render images to files and add to html
		}
		else if ( elemThis->type() == KRObjectData::EntityChart )
		{
			//TODO render images to files and add to html
		}
		else
		{
			kDebug() << "Encountered an unknown element while rendering a section.";
		}
		html += "</div>";
	}
	html += "</div>\n";
}

void KRHtmlRenderPrivate::initEngine()
{
	_handler = new KRScriptHandler(_query->getQuery(), _reportData);
	
	connect(this, SIGNAL(enteredGroup(const QString&, const QVariant&)), _handler, SLOT(slotEnteredGroup(const QString&, const QVariant&)));
	
	connect(this, SIGNAL(exitedGroup(const QString&, const QVariant&)), _handler, SLOT(slotExitedGroup(const QString&, const QVariant&)));
	
	connect(this, SIGNAL(renderingSection(KRSectionData*, OROPage*, QPointF)), _handler, SLOT(slotEnteredSection(KRSectionData*, OROPage*, QPointF)));
}



//
// KRHtmlRender
//
KRHtmlRender::KRHtmlRender ( KexiDB::Connection *c )
{
	_internal = new KRHtmlRenderPrivate();
	setDatabase ( c );
}

KRHtmlRender::KRHtmlRender ( const QString & pDocument, KexiDB::Connection *c )
{
	_internal = new KRHtmlRenderPrivate();
	setDatabase ( c );
	setDom ( pDocument );
}

KRHtmlRender::~KRHtmlRender()
{
}

QString KRHtmlRender::generate()
{
	kDebug() << endl;
	if ( _internal == 0 || !_internal->_valid || _internal->_reportData == 0 )
		return 0;

	//_internal->_pageCounter  = 0;

	_internal->_query = ( new orQuery ( "Data Source", _internal->_reportData->query(), true, _internal->_conn ) );

	_internal->initEngine();

	_internal->html = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n<html>\n<head>\n<style type=\"text/css\">\n";
	
	QList<KRObjectData*> objects = _internal->_reportData->objects();
	foreach(KRObjectData *ob, objects)
	{
	    _internal->html += "." + ob->entityName() + "{\n";
	    _internal->html += "position: absolute;\n";
	    if (ob->type() == KRObjectData::EntityField)
	    {
	      _internal->html += "top: " + QString::number(dynamic_cast<KRFieldData*>(ob)->position().toPoint().y()) + "pt;\n";
	      _internal->html += "left: " + QString::number(dynamic_cast<KRFieldData*>(ob)->position().toPoint().x()) + "pt;\n";
	    }
	    _internal->html += "}\n";
	    
	}
	QList<KRSectionData*> sections = _internal->_reportData->sections();
	foreach(KRSectionData *sec, sections)
	{
	  _internal->html += "." + sec->name() + "{\n";
	  _internal->html += "position: relative;\n";
	    _internal->html += "top: 0pt;\n";
	    _internal->html += "left: 0pt;\n";
	    _internal->html += "height: " + QString::number((int)(sec->height())) + "pt;\n";
	  _internal->html += "}\n";
	  
	}
	
	_internal->html += "</style></head><body>";
	if ( _internal->_reportData->section(KRReportData::ReportHead) != 0 )
	{
		_internal->renderSection ( * ( _internal->_reportData->section(KRReportData::ReportHead) ) );
	}
	
	//For HTML export it makes sense to only render the PageHeadAny section once
	if ( _internal->_reportData->section(KRReportData::PageHeadAny) != 0 )
	{
		_internal->renderSection ( * ( _internal->_reportData->section(KRReportData::PageHeadAny) ) );
	}

	_internal->renderDetailSection ( * ( _internal->_reportData->detail() ) );
	
	//For HTML export it makes sense to only render the PageFootAny section once
	if ( _internal->_reportData->section(KRReportData::PageFootAny) != 0 )
	{
		_internal->renderSection ( * ( _internal->_reportData->section(KRReportData::PageFootAny) ) );
	}
	if ( _internal->_reportData->section(KRReportData::ReportFoot) != 0 )
	{
		_internal->renderSection ( * ( _internal->_reportData->section(KRReportData::ReportFoot) ) );
	}


	_internal->html += "</body></html>";
	_internal->_handler->displayErrors();
	
	delete _internal->_handler;
	delete _internal->_query;

	return _internal->html;
}

void KRHtmlRender::setDatabase ( KexiDB::Connection *c )
{
	if ( _internal != 0 )
		_internal->_conn = c;
}

KexiDB::Connection* KRHtmlRender::database() const
{
	if ( _internal != 0 )
		return _internal->_conn;
	return 0;
}

bool KRHtmlRender::setDom ( const QString & docReport )
{
	kDebug() << docReport << endl;
	if ( _internal != 0 )
	{
		if ( _internal->_reportData != 0 )
			delete _internal->_reportData;
		_internal->_valid = false;

		_internal->_docReport.setContent(docReport);
		_internal->_reportData = new KRReportData(_internal->_docReport.documentElement());
		//TODO KRReportData->isValid()
		_internal->_valid = true;
		
	}
	return isValid();
}

bool KRHtmlRender::isValid() const
{
	if ( _internal != 0 && _internal->_valid )
		return true;
	return false;
}

#include <krhtmlrenderprivate.moc>