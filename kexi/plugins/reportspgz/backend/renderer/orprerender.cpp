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

#include "orprerender.h"
#include "renderobjects.h"
#include "orutils.h"
#include "barcodes.h"
//#include "graph.h" //TODO Use kdchart or kochart

#include <QPrinter>
#include <QFontMetrics>
#include <QPainter>

#include <parsexmlutils.h>
#include <labelsizeinfo.h>
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

//
// ORPreRenderPrivate
// This class is the private class that houses all the internal
// variables so we can provide a cleaner interface to the user
// without presenting to them things that they don't need to see
// and may change over time.
//
class ORPreRenderPrivate : public QObject
{
	Q_OBJECT
	public:
		ORPreRenderPrivate();
		virtual ~ORPreRenderPrivate();

		bool _valid;
		QDomDocument _docReport;

		KexiDB::Connection *_conn;

		ORODocument* _document;
		OROPage*     _page;
		KRReportData* _reportData;

		qreal _yOffset;      // how far down the current page are we
		qreal _topMargin;    // value stored in the correct units
		qreal _bottomMargin; // -- same as above --
		qreal _leftMargin;   // -- same as above --
		qreal _rightMargin;  // -- same as above --
		qreal _maxHeight;    // -- same as above --
		qreal _maxWidth;     // -- same as above --
		int _pageCounter;    // what page are we currently on?
		int _recordCount;

		orQuery* _query;
		QList<OROTextBox*> _postProcText;

		QMap<ORDataData,qreal> _subtotPageCheckPoints;
		QMap<ORDataData,qreal> * _subtotContextMap;
		KRDetailSectionData * _subtotContextDetail;
		bool _subtotContextPageFooter;

		bool populateData ( const ORDataData &, orData & );

		void createNewPage();
		qreal finishCurPage ( bool = false );
		qreal finishCurPageSize ( bool = false );

		void renderDetailSection ( KRDetailSectionData & );
		qreal renderSection ( const KRSectionData & );
		qreal renderSectionSize ( const KRSectionData & );

		qreal getNearestSubTotalCheckPoint ( const ORDataData & );
		
		///Scripting Stuff
		KRScriptHandler *_handler;
		void initEngine();
	
	signals:
		void enteredGroup(const QString&, const QVariant&);
		void exitedGroup(const QString&, const QVariant&);
		void renderingSection (KRSectionData*, OROPage*, QPointF);
};

ORPreRenderPrivate::ORPreRenderPrivate()
{
	_valid = false;
	_reportData = 0;
	_document = 0;
	_page = 0;
	_yOffset = 0.0;
	_topMargin = _bottomMargin = 0.0;
	_leftMargin = _rightMargin = 0.0;
	_pageCounter = 0;
	_maxHeight = _maxWidth = 0.0;
	_query = 0;
	_subtotContextMap = 0;
//	_subtotContextDetail = 0;
	_subtotContextPageFooter = false;
	
	
}

ORPreRenderPrivate::~ORPreRenderPrivate()
{
	if ( _reportData != 0 )
	{
		delete _reportData;
		_reportData = 0;
	}

	_postProcText.clear();
}

bool ORPreRenderPrivate::populateData ( const ORDataData & dataSource, orData &dataTarget )
{

	dataTarget.setQuery ( _query );
	dataTarget.setField ( dataSource.column );
	return true;

}

void ORPreRenderPrivate::createNewPage()
{
	if ( _pageCounter > 0 )
		finishCurPage();

	
	
	//TODO Totals
	// get the checkpoints for the page footers
	QMapIterator<ORDataData,qreal> it ( _subtotPageCheckPoints );
	while ( it.hasNext() )
	{
		it.next();
//		qreal d = 0.0;
		ORDataData data = it.key();
//		XSqlQuery * xqry = getQuerySource ( data.query )->getQuery();
//		if ( xqry )
//			d = xqry->getFieldTotal ( data.column );
//		_subtotPageCheckPoints.insert ( it.key(), d );
	}

	_pageCounter++;

	//Update the page count script value
	_handler->setPageNumber(_pageCounter);
	
	_page = new OROPage ( 0 );
	_document->addPage ( _page );

	bool lastPage = false;

	_yOffset = _topMargin;

	if ( _pageCounter == 1 && _reportData->pghead_first != 0 )
		renderSection ( * ( _reportData->pghead_first ) );
	else if ( lastPage == true && _reportData->pghead_last != 0 )
		renderSection ( * ( _reportData->pghead_last ) );
	else if ( ( _pageCounter % 2 ) == 1 && _reportData->pghead_odd != 0 )
		renderSection ( * ( _reportData->pghead_odd ) );
	else if ( ( _pageCounter % 2 ) == 0 && _reportData->pghead_even != 0 )
		renderSection ( * ( _reportData->pghead_even ) );
	else if ( _reportData->pghead_any != 0 )
		renderSection ( * ( _reportData->pghead_any ) );
}

qreal ORPreRenderPrivate::finishCurPageSize ( bool lastPage )
{
	qreal retval = 0.0;

	_subtotContextPageFooter = true;
	if ( lastPage && _reportData->pgfoot_last != 0 )
		retval = renderSectionSize ( * ( _reportData->pgfoot_last ) );
	else if ( _pageCounter == 1 && _reportData->pgfoot_first )
		retval = renderSectionSize ( * ( _reportData->pgfoot_first ) );
	else if ( ( _pageCounter % 2 ) == 1 && _reportData->pgfoot_odd )
		retval = renderSectionSize ( * ( _reportData->pgfoot_odd ) );
	else if ( ( _pageCounter % 2 ) == 0 && _reportData->pgfoot_even )
		retval = renderSectionSize ( * ( _reportData->pgfoot_even ) );
	else if ( _reportData->pgfoot_any != 0 )
		retval = renderSectionSize ( * ( _reportData->pgfoot_any ) );
	_subtotContextPageFooter = false;

	kDebug() << retval << endl;
	return retval;
}

qreal ORPreRenderPrivate::finishCurPage ( bool lastPage )
{
	
	qreal offset = _maxHeight - _bottomMargin;
	qreal retval = 0.0;

	kDebug () << offset << endl;
	_subtotContextPageFooter = true;
	if ( lastPage && _reportData->pgfoot_last != 0 )
	{
		kDebug() << "Last Footer" << endl; 
		_yOffset = offset - renderSectionSize ( * ( _reportData->pgfoot_last ) );
		retval = renderSection ( * ( _reportData->pgfoot_last ) );
	}
	else if ( _pageCounter == 1 && _reportData->pgfoot_first )
	{
		kDebug() << "First Footer" << endl; 
		_yOffset = offset - renderSectionSize ( * ( _reportData->pgfoot_first ) );
		retval = renderSection ( * ( _reportData->pgfoot_first ) );
	}
	else if ( ( _pageCounter % 2 ) == 1 && _reportData->pgfoot_odd )
	{
		kDebug() << "Odd Footer" << endl; 
		_yOffset = offset - renderSectionSize ( * ( _reportData->pgfoot_odd ) );
		retval = renderSection ( * ( _reportData->pgfoot_odd ) );
	}
	else if ( ( _pageCounter % 2 ) == 0 && _reportData->pgfoot_even )
	{
		kDebug() << "Even Footer" << endl; 
		_yOffset = offset - renderSectionSize ( * ( _reportData->pgfoot_even ) );
		retval = renderSection ( * ( _reportData->pgfoot_even ) );
	}
	else if ( _reportData->pgfoot_any != 0 )
	{
		kDebug() << "Any Footer" << endl; 
		_yOffset = offset - renderSectionSize ( * ( _reportData->pgfoot_any ) );
		retval = renderSection ( * ( _reportData->pgfoot_any ) );
	}
	_subtotContextPageFooter = false;

	return retval;
}

void ORPreRenderPrivate::renderDetailSection ( KRDetailSectionData & detailData )
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
				
				_subtotContextMap = & ( grp->_subtotCheckPoints );
				
				//Tell interested parties we're about to render a header
				kDebug() << "EMIT1" << endl;
				emit(enteredGroup(keys[i], keyValues[i]));
				
				if ( grp->head )
					renderSection ( * ( grp->head ) );
				_subtotContextMap = 0;
			}

			do
			{
				int l = curs->at();
				
				kDebug() << "At:" << l << "Y:" << _yOffset << endl;
				
				if ( renderSectionSize ( * ( detailData.detail ) ) + finishCurPageSize ( ( l+1 == _recordCount ) ) + _bottomMargin + _yOffset >= _maxHeight )
				{
					if ( l > 0 )
						curs->movePrev();
					createNewPage();
					if ( l > 0 )
						curs->moveNext();
				}
				
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
							bool do_break = false;
							for ( i = cnt - 1; i >= pos; i-- )
							{
								if ( do_break )
									createNewPage();
								do_break = false;
								grp = detailData.groupList[i];
								_subtotContextMap = & ( grp->_subtotCheckPoints );
								if ( grp->foot )
								{
									if ( renderSectionSize ( * ( grp->foot ) ) + finishCurPageSize() + _bottomMargin + _yOffset >= _maxHeight )
										createNewPage();
									renderSection ( * ( grp->foot ) );
								}
								_subtotContextMap = 0;
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
								if ( ORDetailGroupSectionData::BreakAfterGroupFoot == grp->pagebreak )
									do_break = true;
							}
							// step ahead to where we should be and print the needed headers
							// if all is good
							status = curs->moveNext();
							if ( do_break )
								createNewPage();
							if ( status == true )
							{
								for ( i = pos; i < cnt; i++ )
								{
									grp = detailData.groupList[i];
									_subtotContextMap = & ( grp->_subtotCheckPoints );
									if ( grp->head )
									{
										if ( renderSectionSize ( * ( grp->head ) ) + finishCurPageSize() + _bottomMargin + _yOffset >= _maxHeight )
										{
										curs->movePrev();
											createNewPage();
										curs->moveNext();
										}
										
										
										renderSection ( * ( grp->head ) );
									}
									_subtotContextMap = 0;
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
					_subtotContextMap = & ( grp->_subtotCheckPoints );
					if ( grp->foot )
					{
						if ( renderSectionSize ( * ( grp->foot ) ) + finishCurPageSize() + _bottomMargin + _yOffset >= _maxHeight )
							createNewPage();
						renderSection ( * ( grp->foot ) );
						emit(exitedGroup(keys[i], keyValues[i]));
					}
					_subtotContextMap = 0;
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
		if ( KRDetailSectionData::BreakAtEnd == detailData.pagebreak )
			createNewPage();
	}
}

qreal ORPreRenderPrivate::renderSectionSize ( const KRSectionData & sectionData )
{
	qreal intHeight = POINT_TO_INCH ( sectionData.height() ) * KoGlobal::dpiY();

	if ( sectionData.objects().count() == 0 )
		return intHeight;

	QList<KRObjectData*> objects = sectionData.objects();
	KRObjectData * elemThis;
	foreach(KRObjectData *ob, objects)
	{
		elemThis = ob;
		//++it;
		// TODO: See if this can be simplified anymore than it already is.
		//       All we need to know is how much strech we are going to get.
		if ( elemThis->type() == KRObjectData::EntityText )
		{
			orData       dataThis;
			KRTextData * t = elemThis->toText();

			populateData ( t->data(), dataThis );

			//dataThis.setQuery();
			dataThis.setField ( t->_controlSource->value().toString() );
			QPointF pos = t->_pos.toScene();
			QSizeF size = t->_size.toScene();
			pos /= 100.0;
			pos += QPointF ( _leftMargin, _yOffset );
			size /= 100.0;

			QRectF trf ( pos, size );

			QString qstrValue;
			qreal   intStretch      = trf.top() - _yOffset;
			qreal   intRectHeight   = trf.height();

			QFont f = t->_font->value().value<QFont>();

			qstrValue = dataThis.getValue();
			if ( qstrValue.length() )
			{
				int pos = 0;
				int idx;
				QChar separator;
				QRegExp re ( "\\s" );
				QPrinter prnt ( QPrinter::HighResolution );
				QFontMetrics fm ( f, &prnt );

				int   intRectWidth    = ( int ) ( trf.width() * prnt.resolution() ) - 10;

				while ( qstrValue.length() )
				{
					idx = re.indexIn ( qstrValue, pos );
					if ( idx == -1 )
					{
						idx = qstrValue.length();
						separator = QChar ( '\n' );
					}
					else
						separator = qstrValue.at ( idx );

					if ( fm.boundingRect ( qstrValue.left ( idx ) ).width() < intRectWidth || pos == 0 )
					{
						pos = idx + 1;
						if ( separator == '\n' )
						{
							qstrValue = qstrValue.mid ( idx+1,qstrValue.length() );
							pos = 0;

							intStretch += intRectHeight;
						}
					}
					else
					{
						qstrValue = qstrValue.mid ( pos,qstrValue.length() );
						pos = 0;

						intStretch += intRectHeight;
					}
				}

				intStretch += ( t->bpad / 100.0 );

				if ( intStretch > intHeight )
					intHeight = intStretch;
			}
		}
	}

	return intHeight;
}

qreal ORPreRenderPrivate::renderSection ( const KRSectionData & sectionData )
{
	qreal intHeight = POINT_TO_INCH ( sectionData.height() ) * KoGlobal::dpiY();
	kDebug() << "Name: " << sectionData.name() << " Height: " << intHeight << "Objects: " << sectionData.objects().count() << endl;

	_handler->populateEngineParameters(_query->getQuery());
	
	emit (renderingSection(const_cast<KRSectionData*>(&sectionData), _page, QPointF ( _leftMargin, _yOffset )));
	
	//Render section background
	ORORect* bg = new ORORect();
	bg->setPen(QPen(Qt::NoPen));
	bg->setBrush(sectionData.bgColor());
	qreal w = _page->document()->pageOptions().widthPx() - _page->document()->pageOptions().getMarginRight() - _leftMargin;
	
	bg->setRect(QRectF(_leftMargin, _yOffset, w,intHeight));
	_page->addPrimitive(bg, true);
	
	QList<KRObjectData*> objects = sectionData.objects();
	KRObjectData * elemThis;
	foreach(KRObjectData *ob, objects)
	{
		elemThis = ob;
		if ( elemThis->type() == KRObjectData::EntityLabel )
		{
			KRLabelData * l = elemThis->toLabel();
			QPointF pos = l->_pos.toScene();
			//QSizeF size = l->_size.toScene();
			
			pos += QPointF ( _leftMargin, _yOffset );

			OROTextBox * tb = new OROTextBox();
			tb->setPosition ( pos );
			tb->setSize ( l->_size.toScene() );
			tb->setFont ( l->font() );
			tb->setText ( l->text() );
			tb->setFlags ( l->textFlags() );
			tb->setTextStyle(l->textStyle());
			tb->setLineStyle(l->lineStyle());
			_page->addPrimitive ( tb );
		}
		else if ( elemThis->type() == KRObjectData::EntityField )
		{
			orData       dataThis;
			KRFieldData* f = elemThis->toField();

			QPointF pos = f->_pos.toScene();
			QSizeF size = f->_size.toScene();
			pos += QPointF ( _leftMargin, _yOffset );

			OROTextBox * tb = new OROTextBox();
			tb->setPosition ( pos );
			tb->setSize ( size );
			tb->setFont ( f->font() );
			tb->setFlags ( f->textFlags() );
			tb->setTextStyle(f->textStyle());
			tb->setLineStyle(f->lineStyle());
			
			QString str = QString::null;
	
			QString cs = f->_controlSource->value().toString();
			if (cs.left(1) == "=")
			{
				if (!cs.contains("PageTotal"))
				{
					str = _handler->evaluate(f->entityName()).toString();
				}
				else
				{
					str = cs.mid(1);
					_postProcText.append(tb);
				}
			}
			else
			{
				QString qry = "Data Source";
				QString clm = f->_controlSource->value().toString();
				
				populateData ( f->data(), dataThis );
				str = dataThis.getValue();
			}
			tb->setText ( str );
			_page->addPrimitive ( tb );
		
			
		}
		else if ( elemThis->type() == KRObjectData::EntityText )
		{
			orData       dataThis;
			KRTextData * t = elemThis->toText();
			
			populateData ( t->data(), dataThis );

			QPointF pos = t->_pos.toScene();
			QSizeF size = t->_size.toScene();
			pos += QPointF ( _leftMargin, _yOffset );

			QRectF trf ( pos, size );

			QString qstrValue;
			int     intLineCounter  = 0;
			qreal   intStretch      = trf.top() - _yOffset;
			qreal   intBaseTop      = trf.top();
			qreal   intRectHeight   = trf.height();

			QFont f = t->font();

			qstrValue = dataThis.getValue();
			if ( qstrValue.length() )
			{
				QRectF rect = trf;

				int pos = 0;
				int idx;
				QChar separator;
				QRegExp re ( "\\s" );
				QPrinter prnt ( QPrinter::HighResolution );
				QFontMetrics fm ( f, &prnt );
				
				int   intRectWidth    = ( int ) ( trf.width() * prnt.resolution() ) - 10;

				while ( qstrValue.length() )
				{
					idx = re.indexIn ( qstrValue, pos );
					if ( idx == -1 )
					{
						idx = qstrValue.length();
						separator = QChar ( '\n' );
					}
					else
						separator = qstrValue.at ( idx );

					if ( fm.boundingRect ( qstrValue.left ( idx ) ).width() < intRectWidth || pos == 0 )
					{
						pos = idx + 1;
						if ( separator == '\n' )
						{
							QString line = qstrValue.left ( idx );
							qstrValue = qstrValue.mid ( idx+1,qstrValue.length() );
							pos = 0;

							rect.setTop ( intBaseTop + ( intLineCounter * intRectHeight ) );
							rect.setBottom ( rect.top() + intRectHeight );

							OROTextBox * tb = new OROTextBox();
							tb->setPosition ( rect.topLeft() );
							tb->setSize ( rect.size() );
							tb->setFont ( t->font() );
							tb->setText ( line );
							tb->setFlags ( t->textFlags() );
							tb->setTextStyle(t->textStyle());
							tb->setLineStyle(t->lineStyle());
							_page->addPrimitive ( tb );


							intStretch += intRectHeight;
							intLineCounter++;
						}
					}
					else
					{
						QString line = qstrValue.left ( pos - 1 );
						qstrValue = qstrValue.mid ( pos,qstrValue.length() );
						pos = 0;

						rect.setTop ( intBaseTop + ( intLineCounter * intRectHeight ) );
						rect.setBottom ( rect.top() + intRectHeight );

						OROTextBox * tb = new OROTextBox();
						tb->setPosition ( rect.topLeft() );
						tb->setSize ( rect.size() );
						tb->setFont ( t->font() );
						tb->setText ( line );
						tb->setFlags ( t->textFlags() );
						tb->setTextStyle(t->textStyle());
						tb->setLineStyle(t->lineStyle());
						_page->addPrimitive ( tb );

						intStretch += intRectHeight;
						intLineCounter++;
					}
				}

				intStretch += ( t->bpad / 100.0 );

				if ( intStretch > intHeight )
					intHeight = intStretch;
			}
		}
		else if ( elemThis->type() == KRObjectData::EntityLine )
		{
			KRLineData * l = elemThis->toLine();
			OROLine * ln = new OROLine();
			QPointF s = l->_start.toScene();
			QPointF e = l->_end.toScene();
			QPointF offset ( _leftMargin, _yOffset );
			s += offset;
			e += offset;

			ln->setStartPoint ( s );
			ln->setEndPoint ( e );
			ln->setLineStyle ( l->lineStyle() );
			_page->addPrimitive ( ln );
		}
		else if ( elemThis->type() == KRObjectData::EntityBarcode )
		{
			KRBarcodeData * bc = elemThis->toBarcode();
			orData       dataThis;

			QPointF pos = bc->_pos.toScene();
			QSizeF size = bc->_size.toScene();
			pos += QPointF ( _leftMargin, _yOffset );

			QRectF rect = QRectF ( pos, size );

			populateData ( bc->data(), dataThis );

			QString fmt = bc->_format->value().toString();
			int align = bc->alignment();
			if ( fmt == "3of9" )
				render3of9 ( _page, rect,dataThis.getValue(), align );
			else if ( fmt == "3of9+" )
				renderExtended3of9 ( _page, rect,dataThis.getValue(), align );
			else if ( fmt == "128" )
				renderCode128 ( _page, rect, dataThis.getValue(), align );
			else if ( fmt == "ean13" )
				renderCodeEAN13 ( _page, rect, dataThis.getValue(), align );
			else if ( fmt == "ean8" )
				renderCodeEAN8 ( _page, rect, dataThis.getValue(), align );
			else if ( fmt == "upc-a" )
				renderCodeUPCA ( _page, rect, dataThis.getValue(), align );
			else if ( fmt == "upc-e" )
				renderCodeUPCE ( _page, rect, dataThis.getValue(), align );
			else
			{
				//logMessage("Encountered unknown barcode format: %s",(const char*)bc->format);
			}
		}
		else if ( elemThis->type() == KRObjectData::EntityImage )
		{
			KRImageData * im = elemThis->toImage();
			QString uudata;
			QByteArray imgdata;
			if ( !im->isInline() )
			{
				orData dataThis;
				populateData ( im->data(), dataThis );
				imgdata = dataThis.getRawValue();
			}
			else
			{
				uudata = im->inlineImageData();
				imgdata = KCodecs::base64Decode ( uudata.toLatin1() );
			}
			
			QImage img;
			img.loadFromData ( imgdata );

			OROImage * id = new OROImage();
			id->setImage ( img );
			if ( im->mode().toLower() == "stretch" )
			{
				id->setScaled ( true );
				id->setAspectRatioMode ( Qt::KeepAspectRatio );
				id->setTransformationMode ( Qt::SmoothTransformation );
			}
			QPointF pos = im->_pos.toScene();
			QSizeF size = im->_size.toScene();
			
			pos += QPointF ( _leftMargin, _yOffset );
			
			id->setPosition ( pos );
			id->setSize ( size );
			_page->addPrimitive ( id );
		}
		else if ( elemThis->type() == KRObjectData::EntityChart )
		{
			KRChartData * ch = elemThis->toChart();
			ch->setConnection(_conn);
			ch->populateData();
			OROImage * id = new OROImage();
			ch->widget()->setFixedSize(ch->_size.toScene().toSize());
			id->setImage ( QPixmap::grabWidget ( ch->widget() ).toImage() );
			QPointF pos = ch->_pos.toScene();
			QSizeF size = ch->_size.toScene();
			
			pos += QPointF ( _leftMargin, _yOffset );
			
			id->setPosition ( pos );
			id->setSize ( size );
			_page->addPrimitive ( id );
		}
		else
		{
			kDebug() << "Encountered an unknown element while rendering a section.";
		}
	}

	_yOffset += intHeight;

	return intHeight;
}

qreal ORPreRenderPrivate::getNearestSubTotalCheckPoint ( const ORDataData & d )
{
	// use the various contexts setup to determine what we should be
	// doing and try and locate the nearest subtotal check point value
	// and return that value... if we are unable to locate one then we
	// will just return 0.0 which will case the final value to be a
	// running total

	if ( _subtotContextPageFooter )
	{
		// first check to see if it's a page footer context
		// as that can happen from anywhere at any time.

		// TODO Totals
		if ( _subtotPageCheckPoints.contains ( d ) )
			return _subtotPageCheckPoints[d];

	}
	else if ( _subtotContextMap != 0 )
	{
		// next if an explicit map is set then we are probably
		// rendering a group head/foot now so we will use the
		// available their.. if it's not then we made a mistake

		if ( _subtotContextMap->contains ( d ) )
			return ( *_subtotContextMap ) [d];

	}
	else if ( _subtotContextDetail != 0 )
	{
		// finally if we are in a detail section then we will simply
		// traverse that details sections groups from the most inner
		// to the most outer and use the first check point we find

		// in actuallity we search from the outer most group to the
		// inner most group and just take the last value found which
		// would be the inner most group

		qreal dbl = 0.0;
		ORDetailGroupSectionData * grp = 0;
		for ( int i = 0; i < ( int ) _subtotContextDetail->groupList.count(); i++ )
		{
			grp = _subtotContextDetail->groupList[i];
			if ( grp->_subtotCheckPoints.contains ( d ) )
				dbl = grp->_subtotCheckPoints[d];
		}
		return dbl;

	}

	return 0.0;
}

void ORPreRenderPrivate::initEngine()
{
	_handler = new KRScriptHandler(_query->getQuery(), _reportData);
	
	connect(this, SIGNAL(enteredGroup(const QString&, const QVariant&)), _handler, SLOT(slotEnteredGroup(const QString&, const QVariant&)));
	
	connect(this, SIGNAL(exitedGroup(const QString&, const QVariant&)), _handler, SLOT(slotExitedGroup(const QString&, const QVariant&)));
	
	connect(this, SIGNAL(renderingSection(KRSectionData*, OROPage*, QPointF)), _handler, SLOT(slotEnteredSection(KRSectionData*, OROPage*, QPointF)));
}



//
// ORPreRender
//
ORPreRender::ORPreRender ( KexiDB::Connection *c )
{
	_internal = new ORPreRenderPrivate();
	setDatabase ( c );
}

ORPreRender::ORPreRender ( const QString & pDocument, KexiDB::Connection *c )
{
	_internal = new ORPreRenderPrivate();
	setDatabase ( c );
	setDom ( pDocument );
}

ORPreRender::~ORPreRender()
{
}

ORODocument* ORPreRender::generate()
{
	kDebug() << endl;
	if ( _internal == 0 || !_internal->_valid || _internal->_reportData == 0 )
		return 0;

	// Do this check now so we don't have to undo alot of work later if it fails
	LabelSizeInfo label;
	if ( _internal->_reportData->page.getPageSize() == "Labels" )
	{
		label = LabelSizeInfo::getByName ( _internal->_reportData->page.getLabelType() );
		if ( label.isNull() )
			return 0;
	}

	kDebug() << "Creating Document" << endl;
	_internal->_document = new ORODocument ( _internal->_reportData->title );

	_internal->_pageCounter  = 0;
	_internal->_yOffset      = 0.0;

	kDebug() << "Calculating Margins" << endl;
	if ( !label.isNull() )
	{
		if ( _internal->_reportData->page.isPortrait() )
		{
			_internal->_topMargin = ( label.startY() / 100.0 );
			_internal->_bottomMargin = 0;
			_internal->_rightMargin = 0;
			_internal->_leftMargin = ( label.startX() / 100.0 );
		}
		else
		{
			_internal->_topMargin = ( label.startX() / 100.0 );
			_internal->_bottomMargin = 0;
			_internal->_rightMargin = 0;
			_internal->_leftMargin = ( label.startY() / 100.0 );
		}
	}
	else
	{
		_internal->_topMargin    = _internal->_reportData->page.getMarginTop();
		_internal->_bottomMargin = _internal->_reportData->page.getMarginBottom();
		_internal->_rightMargin  = _internal->_reportData->page.getMarginRight();
		_internal->_leftMargin   = _internal->_reportData->page.getMarginLeft();
		kDebug() << "Margins:" << _internal->_topMargin << _internal->_bottomMargin << _internal->_rightMargin << _internal->_leftMargin << endl;
	}

	kDebug() << "Calculating Page Size" << endl;
	ReportPageOptions rpo ( _internal->_reportData->page );
	// This should reflect the information of the report page size
	if ( _internal->_reportData->page.getPageSize() == "Custom" )
	{
		_internal->_maxWidth = _internal->_reportData->page.getCustomWidth();
		_internal->_maxHeight = _internal->_reportData->page.getCustomHeight();
	}
	else
	{
		if ( !label.isNull() )
		{
			_internal->_maxWidth = label.width();
			_internal->_maxHeight =label.height();
			rpo.setPageSize ( label.paper() );
		}
		else
		{
			// lookup the correct size information for the specified size paper
			_internal->_maxWidth = KoPageFormat::width ( KoPageFormat::formatFromString ( _internal->_reportData->page.getPageSize() ), KoPageFormat::Portrait );
			_internal->_maxHeight = KoPageFormat::height ( KoPageFormat::formatFromString ( _internal->_reportData->page.getPageSize() ), KoPageFormat::Portrait );
		
			KoUnit pageUnit(KoUnit::Millimeter);
			_internal->_maxWidth = KoUnit::toInch(pageUnit.fromUserValue(_internal->_maxWidth)) * KoGlobal::dpiX();
			_internal->_maxHeight = KoUnit::toInch(pageUnit.fromUserValue(_internal->_maxHeight)) * KoGlobal::dpiY();
		}
	}

	if ( !_internal->_reportData->page.isPortrait() )
	{
		qreal tmp = _internal->_maxWidth;
		_internal->_maxWidth = _internal->_maxHeight;
		_internal->_maxHeight = tmp;
	}

	kDebug() << "Page Size:" << _internal->_maxWidth << _internal->_maxHeight << endl;
	
	_internal->_document->setPageOptions ( rpo );

//	QuerySource * qs = 0;

	_internal->_query = ( new orQuery ( "Data Source", _internal->_reportData->query, true, _internal->_conn ) );

//TODO field totals
//	_internal->_subtotPageCheckPoints.clear();
//	for ( int i = 0; i < _internal->_reportData->trackTotal.count(); i++ )
//	{
//		_internal->_subtotPageCheckPoints.insert ( _internal->_reportData->trackTotal[i], 0 );
//		XSqlQuery * xqry = _internal->getQuerySource ( _internal->_reportData->trackTotal[i].query )->getQuery();
//		if ( xqry )
//			xqry->trackFieldTotal ( _internal->_reportData->trackTotal[i].column );
//	}

	_internal->initEngine();
	_internal->createNewPage();
	if ( !label.isNull() )
	{
// Label Print Run
		int row = 0;
		int col = 0;

		// remember the initial margin setting as we will be modifying
		// the value and restoring it as we move around
		qreal margin = _internal->_leftMargin;

		_internal->_yOffset = _internal->_topMargin;

		qreal w = ( label.width() / 100.0 );
		qreal wg = ( label.xGap() / 100.0 );
		qreal h = ( label.height() / 100.0 );
		qreal hg = ( label.yGap() / 100.0 );
		int numCols = label.columns();
		int numRows = label.rows();
		qreal tmp;

		// flip the value around if we are printing landscape
		if ( !_internal->_reportData->page.isPortrait() )
		{
			w = ( label.height() / 100.0 );
			wg = ( label.yGap() / 100.0 );
			h = ( label.width() / 100.0 );
			hg = ( label.xGap() / 100.0 );
			numCols = label.rows();
			numRows = label.columns();
		}

		KRDetailSectionData * detailData = _internal->_reportData->detailsection;
		if ( detailData->detail != 0 )
		{
			orQuery *orqThis = _internal->_query;
			KexiDB::Cursor *query;

			if ( ( orqThis != 0 ) && ! ( ( query = orqThis->getQuery() )->eof() ) )
			{
				query->moveFirst();
				do
				{
					tmp = _internal->_yOffset; // store the value as renderSection changes it
					_internal->renderSection ( * ( detailData->detail ) );
					_internal->_yOffset = tmp; // restore the value that renderSection modified

					col ++;
					_internal->_leftMargin += w + wg;
					if ( col >= numCols )
					{
						_internal->_leftMargin = margin; // reset back to original value
						col = 0;
						row ++;
						_internal->_yOffset += h + hg;
						if ( row >= numRows )
						{
							_internal->_yOffset = _internal->_topMargin;
							row = 0;
							_internal->createNewPage();
						}
					}
				}
				while ( query->moveNext() );
			}
		}
		
	}
	else
	{
// Normal Print Run
		if ( _internal->_reportData->rpthead != 0 )
		{
			_internal->renderSection ( * ( _internal->_reportData->rpthead ) );
		}
		
	
		_internal->renderDetailSection ( * ( _internal->_reportData->detailsection ) );
		
		if ( _internal->_reportData->rptfoot != 0 )
		{
			if ( _internal->renderSectionSize ( * ( _internal->_reportData->rptfoot ) ) + _internal->finishCurPageSize ( true ) + _internal->_bottomMargin + _internal->_yOffset >= _internal->_maxHeight )
			{
				_internal->createNewPage();
			}
			_internal->renderSection ( * ( _internal->_reportData->rptfoot ) );
		}
	}
	_internal->finishCurPage ( true );

	// _postProcText contains those text boxes that need to be updated
	// with information that wasn't available at the time it was added to the document
	_internal->_handler->setPageTotal(_internal->_document->pages());
	
	for ( int i = 0; i < _internal->_postProcText.size(); i++ )
	{
		OROTextBox * tb = _internal->_postProcText.at ( i );
		
		_internal->_handler->setPageNumber(tb->page()->page() + 1);
		
		//tb->setText(_internal->_scriptEngine->evaluate(tb->text()).toString());
	}

	_internal->_handler->displayErrors();
	
	delete _internal->_handler;
	delete _internal->_query;
	_internal->_postProcText.clear();

	ORODocument * pDoc = _internal->_document;
	_internal->_document = 0;
	return pDoc;
}

void ORPreRender::setDatabase ( KexiDB::Connection *c )
{
	if ( _internal != 0 )
		_internal->_conn = c;
}

KexiDB::Connection* ORPreRender::database() const
{
	if ( _internal != 0 )
		return _internal->_conn;
	return 0;
}

bool ORPreRender::setDom ( const QString & docReport )
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

bool ORPreRender::isValid() const
{
	if ( _internal != 0 && _internal->_valid )
		return true;
	return false;
}

#include <orprerenderprivate.moc>