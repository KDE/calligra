#include "kexicsvsource.h"
#include <qtable.h>
#include <qfile.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <qcombobox.h>
#include <qcheckbox.h>

KexiCSVSource::KexiCSVSource(QWidget *parent):KexiCSVSourceBase(parent),KexiTableImportSourceIface() {

	connect(delimiter, SIGNAL(activated(const QString &)), this, SLOT(reparse(const QString &)));
	connect(delimiter, SIGNAL(textChanged(const QString &)), this, SLOT(reparse(const QString &)));
	connect(nameInRow, SIGNAL(clicked()), this, SLOT(reparse()));
	connect(preview, SIGNAL(currentChanged(int, int)), this, SLOT(colChanged(int, int)));

}


KexiCSVSource::~KexiCSVSource() {
}

bool KexiCSVSource::setFile(const QString &fileName) {
	m_file=fileName;
	return parseFile(m_file);
}

bool
KexiCSVSource::parseFile(const QString &file)
{


	QFile f(file);
	if(!f.exists() || !f.open(IO_ReadOnly))
	{
		KMessageBox::sorry(0, i18n("Coudn't open file"), i18n("CSV import"));
		return false;
	}

	QTextStream stream(&f);
	QString line;
	int row=0;
	int offset=0;
	while(!stream.eof())
	{
		line = stream.readLine();
		QStringList columns = QStringList::split(delimiter->currentText(), line, true);

		if(preview->numCols() <= columns.count())
		{
			int no = columns.count() - preview->numCols();
			int cols = preview->numCols();
			preview->insertColumns(cols, no);
			for(int i=cols; i < no; i++)
			{
				preview->horizontalHeader()->setLabel(i, "Field_" + QString::number(i));
			}
		}

		preview->insertRows(row-offset);

		int col=0;
		for(QStringList::Iterator it = columns.begin(); it != columns.end(); ++it)
		{
			if((row == 0) && nameInRow->isChecked())
			{
				preview->horizontalHeader()->setLabel(col, *it);
				if(col == 0) {
					preview->removeRow(row);
					offset=1;
				}
			}
			else
			{
				preview->setText(row-offset, col, *it);
			}
			col++;
		}

		row++;
	}

	return true;
}


void
KexiCSVSource::reparse()
{
	QTable *t = preview;
	for(int c=t->numCols(); t->numCols() > 0; c--)
	{
		t->removeColumn(c);
	}

	for(int r=t->numRows(); t->numRows() > 0; r--)
	{
		t->removeRow(r);
	}

	kdDebug() << "KexiCSVSource::reparse(): clean done!" << endl;
	parseFile(m_file);
}

void
KexiCSVSource::reparse(const QString &)
{
	reparse();
}

int KexiCSVSource::fieldCount() {
	return preview->numCols();
}

KexiDBTable KexiCSVSource::tableStructure() {
	KexiDBTable tbl("CSVSOURCE");

	int cols=preview->numCols();
	QHeader *hdr=preview->horizontalHeader();
	for (int i=0;i<cols;i++) {
		tbl.addField(KexiDBField(hdr->label(i),KexiDBField::SQLVarchar));
	}
	return tbl;
}

bool KexiCSVSource::firstTableRow() {
	m_dataPos=0;
	return (preview->numRows()>0);
}

bool KexiCSVSource::nextTableRow() {
	m_dataPos++;
	return (m_dataPos<preview->numRows());
}

QVariant KexiCSVSource::tableValue(int field) {
	return QVariant(preview->text(m_dataPos,field));
}
