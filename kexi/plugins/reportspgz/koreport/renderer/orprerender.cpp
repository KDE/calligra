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
#include "koreportdata.h"
#include "barcodes.h"
#include <kdeversion.h>

#include <QPrinter>
#include <QFontMetrics>
#include <QPainter>

#include <parsexmlutils.h>
#include <labelsizeinfo.h>
#include <kcodecs.h>
#include <KoPageFormat.h>
#include <kdebug.h>
#include <KoDpi.h>

#include <krobjectdata.h>
#include <krtextdata.h>
#include <krbarcodedata.h>
#include <krfielddata.h>
#include <krimagedata.h>
#include <krlabeldata.h>
#include <krlinedata.h>
#include <krchartdata.h>
#include <krcheckdata.h>

#include "scripting/krscripthandler.h"
#include <krreportdata.h>
#include <krdetailsectiondata.h>
#include <QResizeEvent>
#include <QApplication>

#include <KDChartAbstractDiagram>
#include <KDChartAbstractCoordinatePlane>
#include <KDChartChart>

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

    KoReportData* _kodata;
    QList<OROTextBox*> _postProcText;

    void createNewPage();
    qreal finishCurPage(bool = false);
    qreal finishCurPageSize(bool = false);

    void renderDetailSection(KRDetailSectionData &);
    qreal renderSection(const KRSectionData &);
    qreal renderSectionSize(const KRSectionData &);

    qreal getNearestSubTotalCheckPoint(const ORDataData &);
    
    ///Scripting Stuff
    KRScriptHandler *_handler;
    void initEngine();

signals:
    void enteredGroup(const QString&, const QVariant&);
    void exitedGroup(const QString&, const QVariant&);
    void renderingSection(KRSectionData*, OROPage*, QPointF);
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
    _kodata = 0;
    _conn = 0;
}

ORPreRenderPrivate::~ORPreRenderPrivate()
{
    if (_reportData != 0) {
        delete _reportData;
        _reportData = 0;
    }

    _postProcText.clear();
}

void ORPreRenderPrivate::createNewPage()
{
    if (_pageCounter > 0)
        finishCurPage();

    _pageCounter++;

    //Update the page count script value
    _handler->setPageNumber(_pageCounter);
    _handler->newPage();

    _page = new OROPage(0);
    _document->addPage(_page);

    //TODO calculate past page
    bool lastPage = false;

    _yOffset = _topMargin;

    if (_pageCounter == 1 && _reportData->pghead_first != 0)
        renderSection(* (_reportData->pghead_first));
    else if (lastPage == true && _reportData->pghead_last != 0)
        renderSection(* (_reportData->pghead_last));
    else if ((_pageCounter % 2) == 1 && _reportData->pghead_odd != 0)
        renderSection(* (_reportData->pghead_odd));
    else if ((_pageCounter % 2) == 0 && _reportData->pghead_even != 0)
        renderSection(* (_reportData->pghead_even));
    else if (_reportData->pghead_any != 0)
        renderSection(* (_reportData->pghead_any));
}

qreal ORPreRenderPrivate::finishCurPageSize(bool lastPage)
{
    qreal retval = 0.0;

    if (lastPage && _reportData->pgfoot_last != 0)
        retval = renderSectionSize(* (_reportData->pgfoot_last));
    else if (_pageCounter == 1 && _reportData->pgfoot_first)
        retval = renderSectionSize(* (_reportData->pgfoot_first));
    else if ((_pageCounter % 2) == 1 && _reportData->pgfoot_odd)
        retval = renderSectionSize(* (_reportData->pgfoot_odd));
    else if ((_pageCounter % 2) == 0 && _reportData->pgfoot_even)
        retval = renderSectionSize(* (_reportData->pgfoot_even));
    else if (_reportData->pgfoot_any != 0)
        retval = renderSectionSize(* (_reportData->pgfoot_any));

    kDebug() << retval;
    return retval;
}

qreal ORPreRenderPrivate::finishCurPage(bool lastPage)
{

    qreal offset = _maxHeight - _bottomMargin;
    qreal retval = 0.0;

    kDebug() << offset;

    if (lastPage && _reportData->pgfoot_last != 0) {
        kDebug() << "Last Footer";
        _yOffset = offset - renderSectionSize(* (_reportData->pgfoot_last));
        retval = renderSection(* (_reportData->pgfoot_last));
    } else if (_pageCounter == 1 && _reportData->pgfoot_first) {
        kDebug() << "First Footer";
        _yOffset = offset - renderSectionSize(* (_reportData->pgfoot_first));
        retval = renderSection(* (_reportData->pgfoot_first));
    } else if ((_pageCounter % 2) == 1 && _reportData->pgfoot_odd) {
        kDebug() << "Odd Footer";
        _yOffset = offset - renderSectionSize(* (_reportData->pgfoot_odd));
        retval = renderSection(* (_reportData->pgfoot_odd));
    } else if ((_pageCounter % 2) == 0 && _reportData->pgfoot_even) {
        kDebug() << "Even Footer";
        _yOffset = offset - renderSectionSize(* (_reportData->pgfoot_even));
        retval = renderSection(* (_reportData->pgfoot_even));
    } else if (_reportData->pgfoot_any != 0) {
        kDebug() << "Any Footer";
        _yOffset = offset - renderSectionSize(* (_reportData->pgfoot_any));
        retval = renderSection(* (_reportData->pgfoot_any));
    }

    return retval;
}

void ORPreRenderPrivate::renderDetailSection(KRDetailSectionData & detailData)
{
    kDebug();

    if (detailData.m_detailSection != 0) {
        if (_kodata) {
	    //TODO init the engine earlier?
            _handler->setSource(_kodata->source());
        }
        if (_kodata/* && !curs->eof()*/) {
            QStringList keys;
            QStringList keyValues;
            bool    status;
            int i = 0, pos = 0, cnt = 0;
            ORDetailGroupSectionData * grp = 0;

            _kodata->moveFirst();
            _recordCount = _kodata->recordCount();

            kDebug() << "Record Count:" << _recordCount;

            for (i = 0; i < (int) detailData.m_groupList.count(); i++) {
                cnt++;
                grp = detailData.m_groupList[i];
                QMapIterator<ORDataData, qreal> it(grp->_subtotCheckPoints);
                while (it.hasNext()) {
                    it.next();
                    grp->_subtotCheckPoints.insert(it.key(), 0.0);
                }
                keys.append(grp->column);
                if (!keys[i].isEmpty())
                    keyValues.append(_kodata->value(_kodata->fieldNumber(keys[i])).toString());
                else
                    keyValues.append(QString());
      
                //Tell interested parties we're about to render a header
                kDebug() << "EMIT1";
                emit(enteredGroup(keys[i], keyValues[i]));

                if (grp->head)
                    renderSection(* (grp->head));
            }

            do {
		kDebug() << "...getting pos";
                long l = _kodata->at();

                kDebug() << "At:" << l << "Y:" << _yOffset;

                if (renderSectionSize(* (detailData.m_detailSection)) + finishCurPageSize((l + 1 == _recordCount)) + _bottomMargin + _yOffset >= _maxHeight) {
                    if (l > 0) {
			kDebug() << "...moving prev";
                        _kodata->movePrevious();
			kDebug() << "...creating new page";
			createNewPage();
			kDebug() << "...moving next";
                        _kodata->moveNext();
		    }
                }

                renderSection(* (detailData.m_detailSection));
		kDebug() << "...moving next";
		if (_kodata)
		  status = _kodata->moveNext();
		kDebug() << "...done";
		
                if (status == true && keys.count() > 0) {
                    // check to see where it is we need to start
                    pos = -1; // if it's still -1 by the time we are done then no keyValues changed
                    for (i = 0; i < keys.count(); i++) {
                        if (keyValues[i] != _kodata->value(_kodata->fieldNumber(keys[i])).toString()) {
                            pos = i;
                            break;
                        }
                    }

                    // don't bother if nothing has changed
                    if (pos != -1) {
                        // roll back the query and go ahead if all is good
                        status = _kodata->movePrevious();
                        if (status == true) {
                            // print the footers as needed
                            // any changes made in this for loop need to be duplicated
                            // below where the footers are finished.
                            bool do_break = false;
                            for (i = cnt - 1; i >= pos; i--) {
                                if (do_break)
                                    createNewPage();
                                do_break = false;
                                grp = detailData.m_groupList[i];
                              
                                if (grp->foot) {
                                    if (renderSectionSize(* (grp->foot)) + finishCurPageSize() + _bottomMargin + _yOffset >= _maxHeight)
                                        createNewPage();
                                    renderSection(* (grp->foot));
                                }

                                if (ORDetailGroupSectionData::BreakAfterGroupFoot == grp->pagebreak)
                                    do_break = true;
                            }
                            // step ahead to where we should be and print the needed headers
                            // if all is good
                            status = _kodata->moveNext();
                            if (do_break)
                                createNewPage();
                            if (status == true) {
                                for (i = pos; i < cnt; i++) {
                                    grp = detailData.m_groupList[i];
                            
                                    if (grp->head) {
                                        if (renderSectionSize(* (grp->head)) + finishCurPageSize() + _bottomMargin + _yOffset >= _maxHeight) {
                                            _kodata->movePrevious();
                                            createNewPage();
                                            _kodata->moveNext();
                                        }


                                        renderSection(* (grp->head));
                                    }
                               
                                    if (!keys[i].isEmpty())
                                        keyValues[i] = _kodata->value(_kodata->fieldNumber(keys[i])).toString();

                                    //Tell interested parties thak key values changed
                                    kDebug() << "EMIT2";
                                    emit(enteredGroup(keys[i], keyValues[i]));
                                }
                            }
                        }
                    }
                }
            } while (status == true);

            if (keys.size() > 0 && _kodata->movePrevious()) {
                // finish footers
                // duplicated changes from above here
                for (i = cnt - 1; i >= 0; i--) {
                    grp = detailData.m_groupList[i];

                    if (grp->foot) {
                        if (renderSectionSize(* (grp->foot)) + finishCurPageSize() + _bottomMargin + _yOffset >= _maxHeight)
                            createNewPage();
                        renderSection(* (grp->foot));
                        emit(exitedGroup(keys[i], keyValues[i]));
                    }
                }
            }
        }
        if (KRDetailSectionData::BreakAtEnd == detailData.m_pageBreak)
            createNewPage();
    }
}

qreal ORPreRenderPrivate::renderSectionSize(const KRSectionData & sectionData)
{
    qreal intHeight = POINT_TO_INCH(sectionData.height()) * KoDpi::dpiY();

    if (sectionData.objects().count() == 0)
        return intHeight;

    QList<KRObjectData*> objects = sectionData.objects();
    KRObjectData * elemThis;
    foreach(KRObjectData *ob, objects) {
        elemThis = ob;
        //++it;
        // TODO: See if this can be simplified anymore than it already is.
        //       All we need to know is how much stretch we are going to get.
        if (elemThis->type() == KRObjectData::EntityText) {
            KRTextData * t = elemThis->toText();

            QPointF pos = t->m_pos.toScene();
            QSizeF size = t->m_size.toScene();
            pos += QPointF(_leftMargin, _yOffset);

            QRectF trf(pos, size);

            QString qstrValue;
            qreal   intStretch      = trf.top() - _yOffset;
            qreal   intRectHeight   = trf.height();

            QFont f = t->m_font->value().value<QFont>();

            qstrValue = _kodata->value(t->m_controlSource->value().toString()).toString();
            if (qstrValue.length()) {
                int pos = 0;
                int idx;
                QChar separator;
                QRegExp re("\\s");
                QPrinter prnt(QPrinter::HighResolution);
                QFontMetrics fm(f, &prnt);

                int   intRectWidth    = (int)((t->m_size.toPoint().width() / 72) * prnt.resolution());

                while (qstrValue.length()) {
                    idx = re.indexIn(qstrValue, pos);
                    if (idx == -1) {
                        idx = qstrValue.length();
                        separator = QChar('\n');
                    } else
                        separator = qstrValue.at(idx);

                    if (fm.boundingRect(qstrValue.left(idx)).width() < intRectWidth || pos == 0) {
                        pos = idx + 1;
                        if (separator == '\n') {
                            qstrValue = qstrValue.mid(idx + 1, qstrValue.length());
                            pos = 0;

                            intStretch += intRectHeight;
                        }
                    } else {
                        qstrValue = qstrValue.mid(pos, qstrValue.length());
                        pos = 0;

                        intStretch += intRectHeight;
                    }
                }

                intStretch += (t->bpad / 100.0);

                if (intStretch > intHeight)
                    intHeight = intStretch;
            }
        }
    }

    return intHeight;
}

qreal ORPreRenderPrivate::renderSection(const KRSectionData & sectionData)
{
    qreal intHeight = POINT_TO_INCH(sectionData.height()) * KoDpi::dpiY();
    kDebug() << "Name: " << sectionData.name() << " Height: " << intHeight << "Objects: " << sectionData.objects().count();

    //_handler->populateEngineParameters(_query->getQuery());

    emit(renderingSection(const_cast<KRSectionData*>(&sectionData), _page, QPointF(_leftMargin, _yOffset)));

    //Create a pre-rendered section for this section and add it to the document
    OROSection *sec = new OROSection(_document);
    sec->setHeight(sectionData.height());
    sec->setBackgroundColor(sectionData.bgColor());
    sec->setType(sectionData.type());
    _document->addSection(sec);

    //Render section background
    ORORect* bg = new ORORect();
    bg->setPen(QPen(Qt::NoPen));
    bg->setBrush(sectionData.bgColor());
    qreal w = _page->document()->pageOptions().widthPx() - _page->document()->pageOptions().getMarginRight() - _leftMargin;

    bg->setRect(QRectF(_leftMargin, _yOffset, w, intHeight));
    _page->addPrimitive(bg, true);

    QList<KRObjectData*> objects = sectionData.objects();
    KRObjectData * elemThis;
    foreach(KRObjectData *ob, objects) {
        elemThis = ob;
        if (elemThis->type() == KRObjectData::EntityLabel) {
            KRLabelData * l = elemThis->toLabel();
            QPointF pos = l->m_pos.toScene();
            //QSizeF size = l->_size.toScene();

            pos += QPointF(_leftMargin, _yOffset);

            OROTextBox * tb = new OROTextBox();
            tb->setPosition(pos);
            tb->setSize(l->m_size.toScene());
            tb->setFont(l->font());
            tb->setText(l->text());
            tb->setFlags(l->textFlags());
            tb->setTextStyle(l->textStyle());
            tb->setLineStyle(l->lineStyle());
            _page->addPrimitive(tb);

            OROTextBox *tb2 = dynamic_cast<OROTextBox*>(tb->clone());
            tb2->setPosition(l->m_pos.toPoint());
            sec->addPrimitive(tb2);
        } else if (elemThis->type() == KRObjectData::EntityField) {
            KRFieldData* f = elemThis->toField();

            QPointF pos = f->m_pos.toScene();
            QSizeF size = f->m_size.toScene();
            pos += QPointF(_leftMargin, _yOffset);

            OROTextBox * tb = new OROTextBox();
            tb->setPosition(pos);
            tb->setSize(size);
            tb->setFont(f->font());
            tb->setFlags(f->textFlags());
            tb->setTextStyle(f->textStyle());
            tb->setLineStyle(f->lineStyle());

            QString str;

            QString cs = f->m_controlSource->value().toString();
            if (cs.left(1) == "=") { //Everything after = is treated as code
                if (!cs.contains("PageTotal()")) {
#if KDE_IS_VERSION(4,2,88)
		      QVariant v = _handler->evaluate(cs.mid(1));
#else
		      QVariant v = _handler->evaluate(f->entityName());
#endif
                    
                    str = v.toString();
                } else {
#if KDE_IS_VERSION(4,2,88)
		      str = cs.mid(1);
#else
		      str = f->entityName();
#endif
                    _postProcText.append(tb);
                }
            } else if (cs.left(1) == "$") { //Everything past $ is treated as a string 
                str = cs.mid(1);
            } else {
                //QString qry = "Data Source";
                QString clm = f->m_controlSource->value().toString();

                //populateData(f->data(), dataThis);
                //str = dataThis.getValue();
		str = _kodata->value(clm).toString();
            }
            tb->setText(str);
            _page->addPrimitive(tb);

            OROTextBox *tb2 = dynamic_cast<OROTextBox*>(tb->clone());
            tb2->setPosition(f->m_pos.toPoint());
            sec->addPrimitive(tb2);


        } else if (elemThis->type() == KRObjectData::EntityText) {
	    QString qstrValue;
            KRTextData * t = elemThis->toText();
	    
	    QString cs = t->m_controlSource->value().toString();
	    
	    kDebug() << cs;
	    
	    if (cs.left(1) == "$") { //Everything past $ is treated as a string 
	      qstrValue = cs.mid(1);
	    } else {
	      qstrValue = _kodata->value(t->m_controlSource->value().toString()).toString();
	    }
	    
            QPointF pos = t->m_pos.toScene();
            QSizeF size = t->m_size.toScene();
            pos += QPointF(_leftMargin, _yOffset);
	    
            QRectF trf(pos, size);

            int     intLineCounter  = 0;
            qreal   intStretch      = trf.top() - _yOffset;
            qreal   intBaseTop      = trf.top();
            qreal   intRectHeight   = trf.height();

            QFont f = t->font();

            kDebug() << qstrValue;
	    
            if (qstrValue.length()) {
                QRectF rect = trf;

                int pos = 0;
                int idx;
                QChar separator;
                QRegExp re("\\s");
                QPrinter prnt(QPrinter::HighResolution);
                QFontMetrics fm(f, &prnt);

//                int   intRectWidth    = (int)(trf.width() * prnt.resolution()) - 10;
		int   intRectWidth    = (int)((t->m_size.toPoint().width() / 72) * prnt.resolution());
		 
                while (qstrValue.length()) {
                    idx = re.indexIn(qstrValue, pos);
                    if (idx == -1) {
                        idx = qstrValue.length();
                        separator = QChar('\n');
                    } else
                        separator = qstrValue.at(idx);

                    if (fm.boundingRect(qstrValue.left(idx)).width() < intRectWidth || pos == 0) {
                        pos = idx + 1;
                        if (separator == '\n') {
                            QString line = qstrValue.left(idx);
                            qstrValue = qstrValue.mid(idx + 1, qstrValue.length());
                            pos = 0;

                            rect.setTop(intBaseTop + (intLineCounter * intRectHeight));
                            rect.setBottom(rect.top() + intRectHeight);

                            OROTextBox * tb = new OROTextBox();
                            tb->setPosition(rect.topLeft());
                            tb->setSize(rect.size());
                            tb->setFont(t->font());
                            tb->setText(line);
                            tb->setFlags(t->textFlags());
                            tb->setTextStyle(t->textStyle());
                            tb->setLineStyle(t->lineStyle());
                            _page->addPrimitive(tb);

                            OROTextBox *tb2 = dynamic_cast<OROTextBox*>(tb->clone());
                            tb2->setPosition(t->m_pos.toPoint());
                            sec->addPrimitive(tb2);

                            intStretch += intRectHeight;
                            intLineCounter++;
                        }
                    } else {
                        QString line = qstrValue.left(pos - 1);
                        qstrValue = qstrValue.mid(pos, qstrValue.length());
                        pos = 0;

                        rect.setTop(intBaseTop + (intLineCounter * intRectHeight));
                        rect.setBottom(rect.top() + intRectHeight);

                        OROTextBox * tb = new OROTextBox();
                        tb->setPosition(rect.topLeft());
                        tb->setSize(rect.size());
                        tb->setFont(t->font());
                        tb->setText(line);
                        tb->setFlags(t->textFlags());
                        tb->setTextStyle(t->textStyle());
                        tb->setLineStyle(t->lineStyle());
                        _page->addPrimitive(tb);

                        intStretch += intRectHeight;
                        intLineCounter++;
                    }
                }

                intStretch += (t->bpad / 100.0);

                if (intStretch > intHeight)
                    intHeight = intStretch;
            }
        } else if (elemThis->type() == KRObjectData::EntityLine) {
            KRLineData * l = elemThis->toLine();
            OROLine * ln = new OROLine();
            QPointF s = l->m_start.toScene();
            QPointF e = l->m_end.toScene();
            QPointF offset(_leftMargin, _yOffset);
            s += offset;
            e += offset;

            ln->setStartPoint(s);
            ln->setEndPoint(e);
            ln->setLineStyle(l->lineStyle());
            _page->addPrimitive(ln);

            OROLine *l2 = dynamic_cast<OROLine*>(ln->clone());
            l2->setStartPoint(l->m_start.toPoint());
            l2->setEndPoint(l->m_end.toPoint());
            sec->addPrimitive(l2);
        } else if (elemThis->type() == KRObjectData::EntityBarcode) {
            KRBarcodeData * bc = elemThis->toBarcode();

            QPointF pos = bc->m_pos.toScene();
            QSizeF size = bc->m_size.toScene();
            pos += QPointF(_leftMargin, _yOffset);

            QRectF rect = QRectF(pos, size);

	    QString val = _kodata->value(bc->m_controlSource->value().toString()).toString();
            QString fmt = bc->m_format->value().toString();
            int align = bc->alignment();
            if (fmt == "3of9")
                render3of9(_page, rect, val, align);
            else if (fmt == "3of9+")
                renderExtended3of9(_page, rect, val, align);
            else if (fmt == "128")
                renderCode128(_page, rect, val, align);
            else if (fmt == "ean13")
                renderCodeEAN13(_page, rect, val, align);
            else if (fmt == "ean8")
                renderCodeEAN8(_page, rect, val, align);
            else if (fmt == "upc-a")
                renderCodeUPCA(_page, rect, val, align);
            else if (fmt == "upc-e")
                renderCodeUPCE(_page, rect, val, align);
            else {
                //logMessage("Encountered unknown barcode format: %s",(const char*)bc->format);
            }
        } else if (elemThis->type() == KRObjectData::EntityImage) {
            KRImageData * im = elemThis->toImage();
            QString uudata;
            QByteArray imgdata;
            if (!im->isInline()) {
//TODO                orData dataThis;
//                populateData(im->data(), dataThis);
//                imgdata = dataThis.getRawValue();
            } else {
                uudata = im->inlineImageData();
                imgdata = KCodecs::base64Decode(uudata.toLatin1());
            }

            QImage img;
            img.loadFromData(imgdata);
            OROImage * id = new OROImage();
            id->setImage(img);
            if (im->mode().toLower() == "stretch") {
                id->setScaled(true);
                id->setAspectRatioMode(Qt::KeepAspectRatio);
                id->setTransformationMode(Qt::SmoothTransformation);
            }
            QPointF pos = im->m_pos.toScene();
            QSizeF size = im->m_size.toScene();

            pos += QPointF(_leftMargin, _yOffset);

            id->setPosition(pos);
            id->setSize(size);
            _page->addPrimitive(id);

            OROImage *i2 = dynamic_cast<OROImage*>(id->clone());
            i2->setPosition(im->m_pos.toPoint());
            sec->addPrimitive(i2);
        } else if (elemThis->type() == KRObjectData::EntityChart) {
            KRChartData * ch = elemThis->toChart();
            ch->setConnection(_conn);

            QStringList masterFields = ch->masterFields();
            for (int i = 0; i < masterFields.size(); ++i){
                if (!masterFields[i].simplified().isEmpty()){
        //            ch->setLinkData(masterFields[i], _query->getQuery()->value(_query->fieldNumber(masterFields[i])));
                }
            }
            ch->populateData();
            if (ch->widget()) {
                OROPicture * id = new OROPicture();
                ch->widget()->setFixedSize(ch->m_size.toScene().toSize());

                QPainter p(id->picture());

                ch->widget()->diagram()->coordinatePlane()->parent()->paint(&p, QRect(QPoint(0, 0), ch->m_size.toScene().toSize()));

                QPointF pos = ch->m_pos.toScene();
                QSizeF size = ch->m_size.toScene();

                pos += QPointF(_leftMargin, _yOffset);

                id->setPosition(pos);
                id->setSize(size);
                _page->addPrimitive(id);

                OROPicture *p2 = dynamic_cast<OROPicture*>(id->clone());
                p2->setPosition(ch->m_pos.toPoint());
                sec->addPrimitive(p2);
            }
        } else if (elemThis->type() == KRObjectData::EntityCheck) {
            KRCheckData *cd = elemThis->toCheck();
            OROCheck *chk = new OROCheck();

            QPointF pos = cd->m_pos.toScene();
            QSizeF size = cd->m_size.toScene();
            pos += QPointF(_leftMargin, _yOffset);

            chk->setPosition(pos);
            chk->setSize(size);

            chk->setLineStyle(cd->lineStyle());
            chk->setForegroundColor(cd->m_foregroundColor->value().value<QColor>());
            chk->setCheckType(cd->m_checkStyle->value().toString());

            QString str;

            QString cs = cd->controlSource();
            kDebug() << "EntityCheck CS:" << cs;

            if (cs.left(1) == "=") {
#if KDE_IS_VERSION(4,2,88)
		      str = _handler->evaluate(cs.mid(1)).toString();
#else
		      str = _handler->evaluate(cd->entityName()).toString();
#endif  
            } else {
                QString clm = cd->m_controlSource->value().toString();
                str = _kodata->value(clm).toString();
            }

            bool v = false;

            str = str.toLower();

            kDebug() << "Check Value:" << str;
            if (str == "t" || str == "true" || str == "1")
                v = true;

            chk->setValue(v);

            _page->addPrimitive(chk);
            OROCheck *chk2 = dynamic_cast<OROCheck*>(chk->clone());
            chk2->setPosition(cd->m_pos.toPoint());
            sec->addPrimitive(chk2);
        } else {
            kDebug() << "Encountered an unknown element while rendering a section.";
        }
    }

    _yOffset += intHeight;

    kDebug() << _yOffset;
    return intHeight;
}

void ORPreRenderPrivate::initEngine()
{
    _handler = new KRScriptHandler(_kodata, _reportData);

    connect(this, SIGNAL(enteredGroup(const QString&, const QVariant&)), _handler, SLOT(slotEnteredGroup(const QString&, const QVariant&)));

    connect(this, SIGNAL(exitedGroup(const QString&, const QVariant&)), _handler, SLOT(slotExitedGroup(const QString&, const QVariant&)));

    connect(this, SIGNAL(renderingSection(KRSectionData*, OROPage*, QPointF)), _handler, SLOT(slotEnteredSection(KRSectionData*, OROPage*, QPointF)));
}

//
// ORPreRender
//

ORPreRender::ORPreRender(const QString & pDocument)
{
    d = new ORPreRenderPrivate();
    setDom(pDocument);
}

ORPreRender::~ORPreRender()
{
}

void ORPreRender::setName(const QString &n)
{
    d->_reportData->setName(n);
}

ORODocument* ORPreRender::generate()
{
    kDebug();
    if (d == 0 || !d->_valid || d->_reportData == 0)
        return 0;

    // Do this check now so we don't have to undo a lot of work later if it fails
    LabelSizeInfo label;
    if (d->_reportData->page.getPageSize() == "Labels") {
        label = LabelSizeInfo::getByName(d->_reportData->page.getLabelType());
        if (label.isNull())
            return 0;
    }

    kDebug() << "Creating Document";
    d->_document = new ORODocument(d->_reportData->m_title);

    d->_pageCounter  = 0;
    d->_yOffset      = 0.0;

    kDebug() << "Calculating Margins";
    if (!label.isNull()) {
        if (d->_reportData->page.isPortrait()) {
            d->_topMargin = (label.startY() / 100.0);
            d->_bottomMargin = 0;
            d->_rightMargin = 0;
            d->_leftMargin = (label.startX() / 100.0);
        } else {
            d->_topMargin = (label.startX() / 100.0);
            d->_bottomMargin = 0;
            d->_rightMargin = 0;
            d->_leftMargin = (label.startY() / 100.0);
        }
    } else {
        d->_topMargin    = d->_reportData->page.getMarginTop();
        d->_bottomMargin = d->_reportData->page.getMarginBottom();
        d->_rightMargin  = d->_reportData->page.getMarginRight();
        d->_leftMargin   = d->_reportData->page.getMarginLeft();
        kDebug() << "Margins:" << d->_topMargin << d->_bottomMargin << d->_rightMargin << d->_leftMargin;
    }

    kDebug() << "Calculating Page Size";
    ReportPageOptions rpo(d->_reportData->page);
    // This should reflect the information of the report page size
    if (d->_reportData->page.getPageSize() == "Custom") {
        d->_maxWidth = d->_reportData->page.getCustomWidth();
        d->_maxHeight = d->_reportData->page.getCustomHeight();
    } else {
        if (!label.isNull()) {
            d->_maxWidth = label.width();
            d->_maxHeight = label.height();
            rpo.setPageSize(label.paper());
        } else {
            // lookup the correct size information for the specified size paper
            d->_maxWidth = KoPageFormat::width(KoPageFormat::formatFromString(d->_reportData->page.getPageSize()), KoPageFormat::Portrait);
            d->_maxHeight = KoPageFormat::height(KoPageFormat::formatFromString(d->_reportData->page.getPageSize()), KoPageFormat::Portrait);

            KoUnit pageUnit(KoUnit::Millimeter);
            d->_maxWidth = KoUnit::toInch(pageUnit.fromUserValue(d->_maxWidth)) * KoDpi::dpiX();
            d->_maxHeight = KoUnit::toInch(pageUnit.fromUserValue(d->_maxHeight)) * KoDpi::dpiY();
        }
    }

    if (!d->_reportData->page.isPortrait()) {
        qreal tmp = d->_maxWidth;
        d->_maxWidth = d->_maxHeight;
        d->_maxHeight = tmp;
    }

    kDebug() << "Page Size:" << d->_maxWidth << d->_maxHeight;

    d->_document->setPageOptions(rpo);
    d->_kodata->open();
    d->initEngine();

    //Loop through all abjects that have been registered, and register them with the script handler
    if (d->_handler) {
        QMapIterator<QString, QObject*> i(m_scriptObjects);
        while (i.hasNext()) {
            i.next();
            d->_handler->registerScriptObject(i.value(), i.key());
        }
    }
    
    //execute the script
    d->_handler->trigger();
    
    d->createNewPage();
    if (!label.isNull()) {
// Label Print Run
        int row = 0;
        int col = 0;

        // remember the initial margin setting as we will be modifying
        // the value and restoring it as we move around
        qreal margin = d->_leftMargin;

        d->_yOffset = d->_topMargin;

        qreal w = (label.width() / 100.0);
        qreal wg = (label.xGap() / 100.0);
        qreal h = (label.height() / 100.0);
        qreal hg = (label.yGap() / 100.0);
        int numCols = label.columns();
        int numRows = label.rows();
        qreal tmp;

        // flip the value around if we are printing landscape
        if (!d->_reportData->page.isPortrait()) {
            w = (label.height() / 100.0);
            wg = (label.yGap() / 100.0);
            h = (label.width() / 100.0);
            hg = (label.xGap() / 100.0);
            numCols = label.rows();
            numRows = label.columns();
        }

        KRDetailSectionData * detailData = d->_reportData->detailsection;
        if (detailData->m_detailSection != 0) {
            KoReportData *mydata = d->_kodata;

            if ((mydata != 0))/* && !((query = orqThis->getQuery())->eof()))*/ {
                mydata->moveFirst();
                do {
                    tmp = d->_yOffset; // store the value as renderSection changes it
                    d->renderSection(* (detailData->m_detailSection));
                    d->_yOffset = tmp; // restore the value that renderSection modified

                    col ++;
                    d->_leftMargin += w + wg;
                    if (col >= numCols) {
                        d->_leftMargin = margin; // reset back to original value
                        col = 0;
                        row ++;
                        d->_yOffset += h + hg;
                        if (row >= numRows) {
                            d->_yOffset = d->_topMargin;
                            row = 0;
                            d->createNewPage();
                        }
                    }
                } while (mydata->moveNext());
            }
        }

    } else {
// Normal Print Run
        if (d->_reportData->rpthead != 0) {
            d->renderSection(* (d->_reportData->rpthead));
        }

        if (d->_reportData->detailsection) {
            d->renderDetailSection(* (d->_reportData->detailsection));
        }

        if (d->_reportData->rptfoot != 0) {
            if (d->renderSectionSize(* (d->_reportData->rptfoot)) + d->finishCurPageSize(true) + d->_bottomMargin + d->_yOffset >= d->_maxHeight) {
                d->createNewPage();
            }
            d->renderSection(* (d->_reportData->rptfoot));
        }
    }
    d->finishCurPage(true);

    // _postProcText contains those text boxes that need to be updated
    // with information that wasn't available at the time it was added to the document
    d->_handler->setPageTotal(d->_document->pages());

    for (int i = 0; i < d->_postProcText.size(); i++) {
        OROTextBox * tb = d->_postProcText.at(i);

        d->_handler->setPageNumber(tb->page()->page() + 1);
        
        tb->setText( d->_handler->evaluate(tb->text()).toString());
    }

    d->_handler->displayErrors();

    d->_kodata->close();
    delete d->_handler;
    delete d->_kodata
;
    d->_postProcText.clear();

    ORODocument * pDoc = d->_document;
    d->_document = 0;
    return pDoc;
}

void ORPreRender::setSourceData(KoReportData *data)
{
    if (d != 0 && data != 0)
    {
        d->_kodata = data;
        d->_conn  = static_cast<KexiDB::Connection*>(data->connection());
    }
}

bool ORPreRender::setDom(const QString & docReport)
{
    kDebug() << docReport;
    if (d != 0) {
        if (d->_reportData != 0)
            delete d->_reportData;
        d->_valid = false;

        d->_docReport.setContent(docReport);
        d->_reportData = new KRReportData(d->_docReport.documentElement().firstChildElement( "koreport" ));
        d->_valid = d->_reportData->isValid();
    }
    return isValid();
}

bool ORPreRender::isValid() const
{
    if (d != 0 && d->_valid)
        return true;
    return false;
}

void ORPreRender::registerScriptObject(QObject* obj, const QString& name)
{
    kDebug() << name;
    m_scriptObjects[name] = obj;
}

#include <orprerenderprivate.moc>
