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
		QHBoxLayout *l=new QHBoxLayout(kspreadFrame),m_headerArea(0,0,0,0),m_dataArea(0,0,0,0);
		l->setAutoAdd(true);

		QString partName=QString("KexiKSpreadImportDoc%1").arg(0);

		KParts::ReadOnlyPart *rop=KParts::ComponentFactory::createPartInstanceFromLibrary<KParts::ReadOnlyPart>(QFile::encodeName("libkspreadpart"),
        	kspreadFrame,"KSpreadImport_View",this,partName.utf8());

		if (rop) {
			 kdDebug()<<"Trying to make a KoDocument out of the ReadOnlyPart"<<endl;
			 m_kspread=static_cast<KoDocument*>(rop->qt_cast("KoDocument"));
			 if (m_kspread==0) kdDebug()<<"ERROR ***********************************"<<endl;
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

void KexiKSpreadSource::showRange(const QString& range) {
	if (!m_kspread) return;
	if (range.isEmpty()) return;

	//check for valid input
	QRegExp rangeDescription=QRegExp("\\s*\\w+\\s*\\[\\s*[a-zA-Z]+[0-9]+\\s*\\:\\s*[a-zA-Z]+[0-9]+\\s*\\]\\s*");
	if (!rangeDescription.exactMatch(range)) {
		kdDebug()<<range<<" is not a valid KexiKSpreadSource range value"<<endl;
		return;
	}

	//retrieve the table name;
	QRegExp tableNameExpr=QRegExp("\\s*\\w+\\s*\\[");
	tableNameExpr.search(range);
	QString tableName=tableNameExpr.capturedTexts()[0];
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

	showRange(tableName,startCol,startRow,endCol,endRow);
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
#warning "IMPORTANT: SANITY CHECKS NEED TO BE ALLOWED BY  THE FRAMEWORK"
	KexiDBTable tbl("KSpreadSOURCE");
	if (!m_kspread) return tbl;

	m_kspread->dcopObject();
/*
	if (useHeader->checked()) {
		if (m_headerArea.width()<m_headerArea.height()) {
			int start=m_headerArea.top();
			int end=m_headerArea.top()+m_headerArea.height();
			for (int i=start;i<end;i++) {
						
			}
		} else {
		}
	} else {
	}
*/
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
	m_dataPos=0;
//	return (preview->numRows()>0);
}

bool KexiKSpreadSource::nextTableRow() {
	m_dataPos++;
//	return (m_dataPos<preview->numRows());
}

QVariant KexiKSpreadSource::tableValue(int field) {
//	return QVariant(preview->text(m_dataPos,field));
}
