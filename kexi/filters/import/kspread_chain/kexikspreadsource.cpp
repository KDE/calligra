#include "kexikspreadsource.h"
#include <kdatastream.h>
#include <qtable.h>
#include <qfile.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <klineedit.h>
#include <qcombobox.h>
#include <kcombobox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <kparts/componentfactory.h>
#include <koDocument.h>
#include <koView.h>
#include <qobjectlist.h>
#include <dcopobject.h>
#include <dcopref.h>
#include <qregexp.h>
#include <math.h>

static int util_decodeColumnLabelText( const QString &_col )
{
    int col = 0;
    int offset='a'-'A';
    int counterColumn = 0;
    for ( uint i=0; i < _col.length(); i++ )
    {
        counterColumn = (int) pow(26.0 , static_cast<int>(_col.length() - i - 1));
        if( _col[i] >= 'A' && _col[i] <= 'Z' )
            col += counterColumn * ( _col[i].latin1() - 'A' + 1);  // okay here (Werner)
        else if( _col[i] >= 'a' && _col[i] <= 'z' )
            col += counterColumn * ( _col[i].latin1() - 'A' - offset + 1 );
        else
            kdDebug(36001) << "util_decodeColumnLabelText: Wrong characters in label text for col:'" << _col << "'" << endl;
    }
    return col;
}


static QString util_encodeColumnLabelText( int column )
{
    int tmp;

    /* we start with zero */
    tmp = column - 1;

    if (tmp < 26) /* A-Z */
        return QString("%1").arg((char) ('A' + tmp));

    tmp -= 26;
    if (tmp < 26*26) /* AA-ZZ */
        return QString("%1%2").arg( (char) ('A' + tmp / 26) )
                              .arg( (char) ('A' + tmp % 26) );

    tmp -= 26*26;
    if (tmp < 26 * 26 * 26 ) /* AAA-ZZZ */
        return QString("%1%2%3").arg( (char) ('A' + tmp / (26 * 26)) )
                                .arg( (char) ('A' + (tmp / 26) % 26 ) )
                                .arg( (char) ('A' + tmp % 26) );

    tmp -= 26*26*26;
    if (tmp < 26 * 26 * 26 * 26) /* AAAA-ZZZZ */
        return QString("%1%2%3%4").arg( (char) ('A' + (tmp / (26 * 26 * 26 )      ) ))
                                  .arg( (char) ('A' + (tmp / (26 * 26      ) % 26 ) ))
                                  .arg( (char) ('A' + (tmp / (26           ) % 26 ) ))
                                  .arg( (char) ('A' + (tmp                   % 26 ) ));

    /* limit is currently 26^4 + 26^3 + 26^2 + 26^1 = 475254 */
    kdDebug(36001) << "invalid column\n";
    return QString("@@@");
}



KexiKSpreadSource::KexiKSpreadSource(QWidget *parent):KexiKSpreadSourceBase(parent),KexiTableImportSourceIface() {
		QHBoxLayout *l=new QHBoxLayout(kspreadFrame),m_headerArea(0,0,0,0),m_dataArea(0,0,0,0),m_headerTable(),m_dataTable();
		l->setAutoAdd(true);

		QString partName=QString("KexiKSpreadImportDoc%1").arg(0);

		KParts::ReadOnlyPart *rop=KParts::ComponentFactory::createPartInstanceFromLibrary<KParts::ReadOnlyPart>(QFile::encodeName("libkspreadpart"),
        	kspreadFrame,"KSpreadImport_View",this,partName.utf8());

		if (rop) {
			 kdDebug()<<"Trying to make a KoDocument out of the ReadOnlyPart"<<endl;
			 m_kspread=static_cast<KoDocument*>(rop->qt_cast("KoDocument"));
			 if (m_kspread==0) kdDebug()<<"ERROR ***********************************"<<endl;
			 else {
				QByteArray param;
				QByteArray data;
				QCString retType;
			        if (!m_kspread->dcopObject()->process("map()",param,retType,data)) {
		         	       kdDebug()<<"Error, can't get DCOPRef to the kspread document map (1)"<<endl;
		        	} else {
				        if (retType!="DCOPRef") {
			                	kdDebug()<<"Error, can't get DCOPRef to the kspread document map (2)"<<endl;
			        	} else {
					        QDataStream streamIn(data,IO_ReadOnly);
					        streamIn>>m_documentMap;
					}
				}
			}
		}
		l->activate();
		connect(syncHeader,SIGNAL(clicked()),this,SLOT(syncHeaderSection()));
		connect(syncDataArea,SIGNAL(clicked()),this,SLOT(syncDataSection()));
		headerRowColumnRange->installEventFilter(this);
		dataRange->installEventFilter(this);
}


KexiKSpreadSource::~KexiKSpreadSource() {
}

void KexiKSpreadSource::syncHeaderSection() {
	headerRowColumnRange->setText(getRange());
//	showRange(headerRowColumnRange->text());
}

void KexiKSpreadSource::syncDataSection() {
	dataRange->setText(getRange());
}


bool KexiKSpreadSource::eventFilter ( QObject * watched, QEvent * e ) {

	if (e->type()==QEvent::FocusIn) {
		showRange(dynamic_cast<QLineEdit*>(watched)->text());
	}
	return false;
}


QRect KexiKSpreadSource::rangeFromString(const QString& range,QString &tableName) {
	if (range.isEmpty()) return QRect();

	//check for valid input
	QRegExp rangeDescription=QRegExp("\\s*\\w+\\s*\\[\\s*[a-zA-Z]+[0-9]+\\s*\\:\\s*[a-zA-Z]+[0-9]+\\s*\\]\\s*");
	if (!rangeDescription.exactMatch(range)) {
		kdDebug()<<range<<" is not a valid KexiKSpreadSource range value"<<endl;
		return QRect();
	}

	//retrieve the table name;
	QRegExp tableNameExpr=QRegExp("\\s*\\w+\\s*\\[");
	tableNameExpr.search(range);
	tableName=tableNameExpr.capturedTexts()[0];
	tableName.truncate(tableName.length()-1);
	tableName=tableName.stripWhiteSpace();

	//retrieve the range begining
	//col
	QRegExp startColExpr=QRegExp("\\[\\s*[a-zA-Z]+");
	startColExpr.search(range);
	QString startColName=startColExpr.capturedTexts()[0];
	startColName=startColName.right(startColName.length()-1);
	startColName=startColName.stripWhiteSpace().upper();
	int startCol=util_decodeColumnLabelText(startColName);
	//row
	QRegExp startLineExpr=QRegExp("[0-9]+\\s*\\:");
	startLineExpr.search(range);
	QString startRowName=startLineExpr.capturedTexts()[0];
	startRowName.truncate(startRowName.length()-1);
	startRowName=startRowName.stripWhiteSpace();
	int startRow=startRowName.toInt();

	//retrieving the range ending
	//col
	QRegExp endColExpr=QRegExp("\\:\\s*[a-zA-Z]+");
	endColExpr.search(range);
	QString endColName=endColExpr.capturedTexts()[0];
	endColName=endColName.right(endColName.length()-1).stripWhiteSpace().upper();
	int endCol=util_decodeColumnLabelText(endColName);
	//row
	QRegExp endRowExpr=QRegExp("[0-9]+\\s*\\]");
	endRowExpr.search(range);
	QString endRowName=endRowExpr.capturedTexts()[0];
	endRowName.truncate(endRowName.length()-1);
	endRowName.stripWhiteSpace();
	int endRow=endRowName.toInt();

	QRect r;
	r.setLeft(startCol);
	r.setRight(endCol);
	r.setTop(startRow);
	r.setBottom(endRow);
	return r;

}

void KexiKSpreadSource::showRange(const QString& range) {
	if (!m_kspread) return;
	QString tableName;
	QRect r=rangeFromString(range,tableName);
	if (!r.isValid()) return;
	showRange(tableName,r.left(),r.top(),r.right(),r.bottom());
}

void KexiKSpreadSource::showRange(const QString& tableName, int startCol, int startRow, int endCol, int endRow) {
	// get view dcop reference
        QByteArray param;
        QByteArray data;
        QCString retType;
        QDataStream streamOut(param,IO_WriteOnly);
        streamOut<<((int)0);
        if (!m_kspread->dcopObject()->process("view(int)",param,retType,data)) {
                kdDebug()<<"Error, can't get DCOPRef to view object (1)"<<endl;
                return ;
        }

        if (retType!="DCOPRef") {
                kdDebug()<<"Error, can't get DCOPRef to view object (2)"<<endl;
                return ;
        }
        QDataStream streamIn(data,IO_ReadOnly);
        DCOPRef viewRef;
        streamIn>>viewRef;

	// make sure the needed table is shown
        DCOPReply res1=viewRef.call("showTable",tableName);
        if (!res1.isValid()) {
                kdDebug()<<"Error, can't show table"<<endl;
                return ;
        }

	if (!((bool)res1)) {
                kdDebug()<<"Error, can't show table, table unknown"<<endl;
		return;
	}
	kdDebug()<<"Table is now visible"<<endl;

	QRect selRec=QRect(startCol,startRow,1+endCol-startCol,1+endRow-startRow);
        res1=viewRef.call("setSelection",selRec);
        if (!res1.isValid()) {
		kdDebug()<<"Error while setting kspread selection"<<endl;
                return ;
        }


//        DCOPRef tableRef=res1;
//        QString tableName=tableRef.call("name");


}

QString KexiKSpreadSource::getRange() {
	if (!m_kspread) return "";

	QByteArray param;
	QByteArray data;
	QCString retType;
	QDataStream streamOut(param,IO_WriteOnly);
	streamOut<<((int)0);
	if (!m_kspread->dcopObject()->process("view(int)",param,retType,data)) {
		kdDebug()<<"Error, can't get DCOPRef to view object (1)"<<endl;
		return "";
	}

	if (retType!="DCOPRef") {
		kdDebug()<<"Error, can't get DCOPRef to view object (2)"<<endl;
		return "";
	}
	QDataStream streamIn(data,IO_ReadOnly);
	DCOPRef viewRef;
	streamIn>>viewRef;

	DCOPReply res1=viewRef.call("table");
	if (!res1.isValid()) {
		kdDebug()<<"Error, can't get DCOPRef to view object (3)"<<endl;
		return "";
	}
	DCOPRef tableRef=res1;
	QString tableName=tableRef.call("name");

	DCOPReply res=viewRef.call("selection");
	if (!res.isValid()) {
		kdDebug()<<"Error, can't get selection"<<endl;
		return "";
	}

	QRect sel=res;
	return QString ("%1[%2%3 : %4%5]").arg(tableName).arg(util_encodeColumnLabelText(sel.x()))
		.arg(sel.y()).arg(util_encodeColumnLabelText(sel.x()+sel.width()-1))
		.arg(sel.y()+sel.height()-1);


}

bool KexiKSpreadSource::checkConsistency() {
	m_dataArea=rangeFromString(dataRange->text(),m_dataTable);
	if (!m_dataArea.isValid()){
		KMessageBox::sorry(this,i18n("You have not specified a valid cell range. This must contain the data you would like to import"));
		return false;
	}
	if (useHeader->isChecked()) {
		m_headerArea=rangeFromString(headerRowColumnRange->text(),m_headerTable);
		if (!m_headerArea.isValid()) {
			KMessageBox::sorry(this,i18n("You have chosen that a separate header range should be used. The specified range is invalid though. You have to fix that problem before you can proceed"));
			return false;
		}
	}

	m_recordIsRow=!(dataRowType->currentItem());
	kdDebug()<<"dataEndType->currentItem() == "<<dataEndType->currentItem()<<endl;
	switch (dataEndType->currentItem()) {
		case 0:m_dataEnd=RangeEnd;
			break;
		case 1:m_dataEnd=ShrinkEmpty;
			break;
		case 2:m_dataEnd=ExpandEmpty;
			break;
		default: m_dataEnd=RangeEnd;
	}
	kdDebug()<<"m_dataEnd=="<<m_dataEnd<<endl;
	return true;
}

bool KexiKSpreadSource::setFile(const QString &fileName) {
	m_file=fileName;
	if (m_kspread) {
		m_kspread->openURL(fileName);
		m_kspread->widget()->show();
	}


}


int KexiKSpreadSource::fieldCount() {
	if (useHeader->isChecked())
		return (m_headerArea.width()<m_headerArea.height())? m_headerArea.height():m_headerArea.width();
	else
		return 0;
//		return m_recordHorizontal?m_dataArea.width():m_dataArea.height();
}

KexiDBTable KexiKSpreadSource::tableStructure() {
	KexiDBTable tbl("KSpreadSOURCE");
	if (!m_kspread) return tbl;
	if (m_documentMap.isNull()) return tbl; //perhaps display an error dialog about an internal error or a mismatching kspread interface

	int start;
	int end;
	QString table;
	QRect area;
	if (useHeader->isChecked()) {
		table=m_headerTable;
		area=m_headerArea;
	} else {
		table=m_dataTable;
		area=m_dataArea;
		if (m_recordIsRow)
			area.setBottom(area.top());
		else
			area.setRight(area.left());
#warning fixme
	}

	if (area.width()<area.height()) {
		start=area.top();
		end=area.top()+area.height();
		int col=area.left();
		for (int i=start;i<end;i++)  {
			QString fieldName=kspreadCellAsString(table,col,i);
			if (fieldName.isEmpty()) fieldName=QString("Unknown_%1").arg(i);
			tbl.addField(KexiDBField(kspreadCellAsString(table,col,i),KexiDBField::SQLVarchar));
		}

	} else {
		start=area.left();
		end=area.left()+area.width();
		int row=area.top();
		for (int i=start;i<end;i++) {
			QString fieldName=kspreadCellAsString(table,i,row);
			if (fieldName.isEmpty()) fieldName=QString("Unknown_%1").arg(i);
			tbl.addField(KexiDBField(fieldName,KexiDBField::SQLVarchar));
		}

	}



	return tbl;
/*
	int cols=preview->numCols();
	QHeader *hdr=preview->horizontalHeader();
	for (int i=0;i<cols;i++) {
		tbl.addField(KexiDBField(hdr->label(i),KexiDBField::SQLVarchar));
	}
	return tbl;
*/
}

bool KexiKSpreadSource::firstTableRow() {
	if (m_recordIsRow)
		m_dataPos=m_dataArea.top()-1;
	else
		m_dataPos=m_dataArea.left()-1;
	return nextTableRow();
}

bool KexiKSpreadSource::nextTableRow() {
	m_dataPos++;

	kdDebug()<<"nextTableRow"<<endl;
	if ((m_dataEnd==RangeEnd) || (m_dataEnd==ShrinkEmpty)){
		kdDebug()<<"RangeEnd or ShrinkEmpty"<<endl;
		if (m_recordIsRow) {
			if (m_dataPos>m_dataArea.bottom()) return false;
			if (m_dataEnd==ShrinkEmpty) {
				kdDebug()<<"record is a row | shrink mode"<<endl;
				QRect rec=QRect(m_dataArea.left(),m_dataPos,m_dataArea.width(),1);
				return !kspreadNoContent(m_dataTable,rec);
			}

		} else {
			if (m_dataPos>m_dataArea.right()) return false;
			if (m_dataEnd==ShrinkEmpty) {
				QRect rec=QRect(m_dataPos,m_dataArea.top(),1,m_dataArea.height());
				return !kspreadNoContent(m_dataTable,rec);
			}
		}
		return true;
	}

	if (m_recordIsRow) {
		QRect rec=QRect(m_dataArea.left(),m_dataPos,m_dataArea.width(),1);
		return !kspreadNoContent(m_dataTable,rec);
	} else {
		QRect rec=QRect(m_dataPos,m_dataArea.top(),1,m_dataArea.height());
		return !kspreadNoContent(m_dataTable,rec);
	}
}

QVariant KexiKSpreadSource::tableValue(int field) {
//	return QVariant(preview->text(m_dataPos,field));
	if (m_recordIsRow)
		return kspreadCellAsString(m_dataTable,field+m_dataArea.left(),m_dataPos);
	else
		return kspreadCellAsString(m_dataTable,m_dataPos,field+m_dataArea.top());
}


QString KexiKSpreadSource::kspreadCellAsString(const QString& table, int x, int y) {
	kdDebug()<<"MapRefObj:"<<m_documentMap.obj()<<endl;
	kdDebug()<<"Table of interest:"<<table<<endl;
	kdDebug()<<"kpsreadCellAsString"<<endl;
	DCOPRef cellRef=DCOPRef(m_documentMap.app(),(QString::fromUtf8(m_documentMap.obj())+"/"+table+"/"+util_encodeColumnLabelText(x)+ QString::number(y)).utf8());
	QString value=cellRef.call("visibleContentAsString");
	return value;
}

bool KexiKSpreadSource::kspreadNoContent(const QString& table, QRect area) {
	kdDebug()<<"MapRefObj:"<<m_documentMap.obj()<<endl;
	kdDebug()<<"Table of interest:"<<table<<endl;
	kdDebug()<<"kspreadNoContent"<<endl;
	DCOPRef cellRef=DCOPRef(m_documentMap.app(),(QString::fromUtf8(m_documentMap.obj())+"/"+table).utf8());
	bool value=cellRef.call("areaHasNoContent(QRect)",area);
	return value;
}

#include "kexikspreadsource.moc"
