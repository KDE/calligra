#include "kexikspreadsource.h"
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
}


KexiKSpreadSource::~KexiKSpreadSource() {
}

void KexiKSpreadSource::syncHeaderSection() {
	headerRowColumnRange->setText(getRange());	
}

void KexiKSpreadSource::syncDataSection() {
	dataRange->setText(getRange());
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
	return QString ("%1!%2%3 : %4!%5%6").arg(tableName).arg(util_encodeColumnLabelText(sel.x()))
		.arg(sel.y()).arg(tableName).arg(util_encodeColumnLabelText(sel.x()+sel.width()-1))
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
