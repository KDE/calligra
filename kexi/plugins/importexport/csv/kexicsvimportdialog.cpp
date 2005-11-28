/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

   This is temporary solution for Kexi CSV support,
   based on kspread/dialogs/kspread_dlg_csv.cc

   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
			 (C) 2002-2003 Ariya Hidayat <ariya@kde.org>
			 (C) 2002	  Laurent Montel <montel@kde.org>
			 (C) 1999 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qclipboard.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmime.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtable.h>
#include <qlayout.h>
#include <qfiledialog.h>
#include <qpainter.h>
#include <qtextcodec.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kglobalsettings.h>
#include <kactivelabel.h>
#include <kiconloader.h>
#include <kcharsets.h>

#include <kexiutils/identifier.h>
#include <kexiutils/utils.h>
#include <core/kexi.h>
#include <core/kexiproject.h>
#include <core/kexipart.h>
#include <core/kexipartinfo.h>
#include <core/keximainwindow.h>
#include <core/kexiguimsghandler.h>
#include <kexidb/connection.h>
#include <kexidb/tableschema.h>
#include <kexidb/transaction.h>
#include <widget/kexicharencodingcombobox.h>

#include "kexicsvimportdialog.h"
#include "kexicsvimportoptionsdlg.h"
#include "kexicsvwidgets.h"

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#if 0
#include <kspread_cell.h>
#include <kspread_doc.h>
#include <kspread_sheet.h>
#include <kspread_undo.h>
#include <kspread_view.h>
#endif

#define _IMPORT_ICON "download_manager" /*todo: change to "file_import" or so*/
#define _TEXT_TYPE 0
#define _NUMBER_TYPE 1
#define _DATE_TYPE 2
#define _PK_FLAG 3

//extra:
#define _NO_TYPE_YET -1 //allows to accept a number of empty cells, before something non-empty
#define _FP_NUMBER_TYPE 255 //_NUMBER_TYPE variant

class KexiCSVImportDialogTable : public QTable
{
public:
	KexiCSVImportDialogTable( QWidget * parent = 0, const char * name = 0 )
	: QTable(parent, name) {
		f = font();
		f.setBold(true);
	}
	virtual void paintCell( QPainter * p, int row, int col, const QRect & cr, bool selected, const QColorGroup & cg ) {
		if (row==0)
			p->setFont(f);
		else
			p->setFont(font());
		QTable::paintCell(p, row, col, cr, selected, cg);
	}
	virtual void setColumnWidth( int col, int w ) {
		//make columns a bit wider
		QTable::setColumnWidth( col, w + 16 );
	}
	QFont f;
};

KexiCSVImportDialog::KexiCSVImportDialog( Mode mode, KexiMainWindow* mainWin, QWidget * parent, const char * name
//, QRect const & rect, Mode mode
)
 : KDialogBase( 
	KDialogBase::Plain, 
	i18n( "Importing CSV Data File" ),
	(mode==File ? User1 : (ButtonCode)0) |Ok|Cancel, 
	Ok,
	parent, 
	name ? name : "KexiCSVImportDialog", 
	true, 
	false,
	KGuiItem( i18n("&Options..."))
   ),
//	m_pView( parent ),
	m_mainWin(mainWin),
	m_cancelled( false ),
	m_adjustRows( 0 ),
	m_startline( 0 ),
	m_textquote( KEXICSV_DEFAULT_TEXT_QUOTE ),
	m_delimiter( KEXICSV_DEFAULT_DELIMITER ),
	m_mode(mode),
	m_prevSelectedCol(-1),
//	m_targetRect( rect ),
	m_columnsAdjusted(false),
	m_1stRowForFieldNamesDetected(false),
	m_firstFillTableCall(true),
	m_primaryKeyColumn(-1)
{
	setButtonOK(KGuiItem( i18n("&Import..."), _IMPORT_ICON));

	m_typeNames.resize(3);
	m_typeNames[0] = i18n("text");
	m_typeNames[1] = i18n("number");
	m_typeNames[2] = i18n("date");

	m_pkIcon = SmallIcon("key");

	m_uniquenessTest.setAutoDelete(true);

	setIcon(SmallIcon(_IMPORT_ICON));
	setSizeGripEnabled( TRUE );

	m_encoding = QString::fromLatin1(KGlobal::locale()->encoding());

//  QWidget* page = new QWidget( plainPage() );
  plainPage()->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred, 5, 5));
//  setMainWidget( page );
  //  MyDialogLayout = new QGridLayout( page, 4, 4, marginHint(), spacingHint(), "MyDialogLayout");
	MyDialogLayout = new QGridLayout( plainPage(), 6, 5, 
		KDialogBase::marginHint(), KDialogBase::spacingHint(), "MyDialogLayout");

	QHBoxLayout *hbox = new QHBoxLayout( plainPage() );
	QLabel *lbl = new QLabel(
		mode==File ? i18n("<b>Preview of data from file:</b>")
			: i18n("<b>Preview of data from clipboard:</b>"),
		plainPage());
	lbl->setMinimumWidth(130);
	lbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	lbl->setAlignment(Qt::AlignVCenter | Qt::AlignLeft | Qt::WordBreak);
	hbox->addWidget(lbl);
	QLabel *iconLbl = new QLabel(plainPage());
	iconLbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	KActiveLabel *fnameLbl = new KActiveLabel(plainPage());
	fnameLbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	fnameLbl->setAlignment(Qt::AlignVCenter | Qt::AlignLeft | Qt::WordBreak);
	hbox->addSpacing(5);
	hbox->addWidget(iconLbl);
	hbox->addWidget(fnameLbl);
	MyDialogLayout->addMultiCellLayout(hbox, 0, 0, 0, 4);

	QFrame* separator = new QFrame(plainPage());
	separator->setFrameShape(QFrame::HLine);
	separator->setFrameShadow(QFrame::Sunken);
	MyDialogLayout->addMultiCellWidget(separator, 1, 1, 0, 4);

///	MyDialogLayout->addItem( 
///		new QSpacerItem( 20, 111, QSizePolicy::Minimum, QSizePolicy::Expanding ), 2, 4 );

/*  // Limit the range
  int column = m_targetRect.left();
  KexiCell* lastCell = m_pView->activeTable()->getLastCellColumn( column );
  if( lastCell )
	if( m_targetRect.bottom() > lastCell->row() )
	  m_targetRect.setBottom( lastCell->row() );
*/

	// Delimiter: comma, semicolon, tab, space, other
	m_delimiterCombo = new KexiCSVDelimiterComboBox(plainPage());
//	m_delimiterCombo->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred );
	MyDialogLayout->addWidget( m_delimiterCombo, 3, 0 );

	m_delimiterLabel = new QLabel(m_delimiterCombo, i18n("Delimiter:"), plainPage());
	MyDialogLayout->addWidget( m_delimiterLabel, 2, 0 );

	m_delimiterEdit = new QLineEdit( plainPage(), "m_delimiterEdit" );
//  m_delimiterEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, m_delimiterEdit->sizePolicy().hasHeightForWidth() ) );
	m_delimiterEdit->setMaximumSize( QSize( 30, 32767 ) );
	m_delimiterEdit->setMaxLength(1);
	MyDialogLayout->addWidget( m_delimiterEdit, 4, 0 );

#if 0
  m_delimiterBox = new QButtonGroup( plainPage(), "m_delimiterBox" );
  m_delimiterBox->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred );
  m_delimiterBox->setTitle( i18n( "Delimiter" ) );
  m_delimiterBox->setColumnLayout(0, Qt::Vertical );
  m_delimiterBox->layout()->setSpacing( KDialog::spacingHint() );
  m_delimiterBox->layout()->setMargin( KDialog::marginHint() );
  m_delimiterBoxLayout = new QGridLayout( m_delimiterBox->layout() );
  m_delimiterBoxLayout->setAlignment( Qt::AlignTop );
  MyDialogLayout->addMultiCellWidget( m_delimiterBox, 1, 4, 0, 0 );

  m_radioComma = new QRadioButton( m_delimiterBox, "m_radioComma" );
  m_radioComma->setText( i18n( "Comma" ) );
  m_radioComma->setChecked( TRUE );
  m_delimiterBoxLayout->addWidget( m_radioComma, 0, 0 );

  m_radioSemicolon = new QRadioButton( m_delimiterBox, "m_radioSemicolon" );
  m_radioSemicolon->setText( i18n( "Semicolon" ) );
  m_delimiterBoxLayout->addWidget( m_radioSemicolon, 0, 1 );

  m_radioTab = new QRadioButton( m_delimiterBox, "m_radioTab" );
  m_radioTab->setText( i18n( "Tabulator" ) );
  m_delimiterBoxLayout->addWidget( m_radioTab, 1, 0 );

  m_radioSpace = new QRadioButton( m_delimiterBox, "m_radioSpace" );
  m_radioSpace->setText( i18n( "Space" ) );
  m_delimiterBoxLayout->addWidget( m_radioSpace, 1, 1 );

  m_radioOther = new QRadioButton( m_delimiterBox, "m_radioOther" );
  m_radioOther->setText( i18n( "Other delimiter", "Other" ) );
  m_delimiterBoxLayout->addWidget( m_radioOther, 2, 0 );
#endif //0

	// Format: number, text, currency,
	m_formatComboText = i18n( "Format for column %1:" );
	m_formatCombo = new KComboBox(plainPage(), "m_formatCombo");
	m_formatCombo->insertItem(i18n("Text"));
	m_formatCombo->insertItem(i18n("Number"));
	m_formatCombo->insertItem(i18n("Date"));
//	m_formatCombo->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred );
	MyDialogLayout->addWidget( m_formatCombo, 3, 1 );

	m_formatLabel = new QLabel(m_formatCombo, "", plainPage());
	m_formatLabel->setAlignment(Qt::AlignAuto | Qt::WordBreak);
	MyDialogLayout->addWidget( m_formatLabel, 2, 1 );

	m_primaryKeyField = new QCheckBox( i18n( "Primary key" ), plainPage(), "m_primaryKeyField" );
	MyDialogLayout->addWidget( m_primaryKeyField, 4, 1 );

#if 0
  m_formatBox = new QButtonGroup( plainPage(), "m_formatBox" );
  m_formatBox->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred );
//  m_formatBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, m_formatBox->sizePolicy().hasHeightForWidth() ) );
  m_formatBoxTitle = i18n( "Format for column %1" );
  m_formatBox->setColumnLayout(0, Qt::Vertical );
  m_formatBox->layout()->setSpacing( KDialog::spacingHint() );
  m_formatBox->layout()->setMargin( KDialog::marginHint() );
  m_formatBoxLayout = new QGridLayout( m_formatBox->layout(), 3, 2 );
  m_formatBoxLayout->setAlignment( Qt::AlignTop );
  MyDialogLayout->addMultiCellWidget( m_formatBox, 1, 4, 1, 1 );

  m_radioText = new QRadioButton( m_formatBox, "m_radioText" );
  m_radioText->setText( i18n( "Text" ) );
  m_radioText->setChecked( TRUE );
  m_formatBoxLayout->addWidget( m_radioText, 0, 0 );

  m_radioNumber = new QRadioButton( m_formatBox, "m_radioNumber" );
  m_radioNumber->setText( i18n( "Number" ) );
//js  m_formatBoxLayout->addMultiCellWidget( m_radioNumber, 1, 1, 0, 1 );
  m_formatBoxLayout->addWidget( m_radioNumber, 1, 0 );

//js  m_radioCurrency = new QRadioButton( m_formatBox, "m_radioCurrency" );
//js  m_radioCurrency->setText( i18n( "Currency" ) );
//js  m_formatBoxLayout->addMultiCellWidget( m_radioCurrency, 0, 0, 1, 2 );

  m_radioDate = new QRadioButton( m_formatBox, "m_radioDate" );
  m_radioDate->setText( i18n( "Date" ) );
//js  m_formatBoxLayout->addWidget( m_radioDate, 1, 2 );
  m_formatBoxLayout->addWidget( m_radioDate, 0, 1 );
#endif //0

  m_comboQuote = new KexiCSVTextQuoteComboBox( plainPage() );
//  m_comboQuote->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
////  m_comboQuote->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred );
//  m_comboQuote->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, m_comboQuote->sizePolicy().hasHeightForWidth() ) );
  MyDialogLayout->addWidget( m_comboQuote, 3, 2 );

  m_comboLine = new KComboBox( FALSE, plainPage(), "m_comboLine" );
  m_comboLine->insertItem( i18n( "1" ) );
////  m_comboLine->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred );
//  m_comboLine->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, m_comboLine->sizePolicy().hasHeightForWidth() ) );
  MyDialogLayout->addWidget( m_comboLine, 3, 3 );

//  QSpacerItem* spacer_2 = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Preferred );
//  MyDialogLayout->addItem( spacer_2, 4, 3 );

  TextLabel2 = new QLabel( m_comboQuote, i18n( "Text quote:" ), plainPage(), "TextLabel2" );
////  TextLabel2->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred );
//js  TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );
  TextLabel2->setAlignment(Qt::AlignAuto | Qt::WordBreak);
  MyDialogLayout->addWidget( TextLabel2, 2, 2 );

  TextLabel3 = new QLabel( m_comboLine, i18n( "Start at line:" ), plainPage(), "TextLabel3" );
////  TextLabel3->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred );
//  TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );
  TextLabel3->setAlignment(Qt::AlignAuto | Qt::WordBreak);
  MyDialogLayout->addWidget( TextLabel3, 2, 3 );

  m_ignoreDuplicates = new QCheckBox( plainPage(), "m_ignoreDuplicates" );
  m_ignoreDuplicates->setText( i18n( "Ignore duplicated delimiters" ) );
  MyDialogLayout->addMultiCellWidget( m_ignoreDuplicates, 4, 4, 2, 4 );

  m_1stRowForFieldNames = new QCheckBox( plainPage(), "m_1stRowForFieldNames" );
  m_1stRowForFieldNames->setText( i18n( "First row contains column names" ) );
  MyDialogLayout->addMultiCellWidget( m_1stRowForFieldNames, 5, 5, 2, 4 );

  m_table = new KexiCSVImportDialogTable( plainPage(), "m_table" );
  m_table->setSizePolicy( QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum, 2, 2) );
  //m_table->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, m_table->sizePolicy().hasHeightForWidth() ) );
  m_table->setNumRows( 0 );
  m_table->setNumCols( 0 );
  MyDialogLayout->addMultiCellWidget( m_table, 6, 6, 0, 4 );

//	delimiterClicked(0);

/** @todo reuse Clipboard too! */
/*
if ( m_mode == Clipboard )
  {
	setCaption( i18n( "Inserting From Clipboard" ) );
	QMimeSource * mime = QApplication::clipboard()->data();
	if ( !mime )
	{
	  KMessageBox::information( this, i18n("There is no data in the clipboard.") );
	  m_cancelled = true;
	  return;
	}

	if ( !mime->provides( "text/plain" ) )
	{
	  KMessageBox::information( this, i18n("There is no usable data in the clipboard.") );
	  m_cancelled = true;
	  return;
	}
	m_fileArray = QByteArray(mime->encodedData( "text/plain" ) );
  }
  else if ( mode == File )
  {*/
	m_dateRegExp1 = QRegExp("\\d{1,4}[/\\-\\.]\\d{1,2}[/\\-\\.]\\d{1,2}");
	m_dateRegExp2 = QRegExp("\\d{1,2}[/\\-\\.]\\d{1,2}[/\\-\\.]\\d{1,4}");
	m_fpNumberRegExp = QRegExp("\\d*[,\\.]\\d+");

	if (mode == File) {
		QStringList mimetypes;
		mimetypes << "text/x-csv" << "text/plain" << "all/allfiles";
#ifdef Q_WS_WIN
	//! @todo remove
		m_fname = QFileDialog::getOpenFileName( KGlobalSettings::documentPath(), 
			KexiUtils::fileDialogFilterStrings(mimetypes, false),
			this, "KexiCSVImportDialog", i18n("Open CSV Data File"));
#else
		m_fname = KFileDialog::getOpenFileName(":CSVImportDialog", mimetypes.join(" "), this);
#endif
		//cancel action !
		if ( m_fname.isEmpty() )
		{
			actionButton( Ok )->setEnabled( false );
			m_cancelled = true;
			if (parentWidget())
				parentWidget()->raise();
			return;
		}
	}
	else if (mode == Clipboard) {
		QCString type("plain");
		m_data = QApplication::clipboard()->text(type);
	}
	else {
		return;
	}

	if (!loadData())
		return;

	//try to detect delimiter
	int detectedDelimiter = mode==File ? 0/*comma*/ : 2/*tab*/; //<-- defaults
	for (uint i=0; i < QMIN(128, m_data.length()); i++) {
		const QChar c(m_data[i]);
		if (c=='\t') {
			detectedDelimiter = 2;
			break;
		}
		else if (c==',') {
			detectedDelimiter = 0;
			break;
		}
		else if (c==';') {
			detectedDelimiter = 1;
			break;
		}
	}

	delimiterChanged(detectedDelimiter); // this will cause fillTable()

	if (m_mode==File) {
		iconLbl->setPixmap(KMimeType::pixmapForURL(KURL::fromPathOrURL(m_fname), 0, KIcon::Desktop));
		fnameLbl->setText( "  " + QDir::convertSeparators(m_fname) );
	}
	else {
		iconLbl->setPixmap(DesktopIcon("editpaste"));
	}

	QSize s( sizeHint() );
	s.setHeight(QMAX(500,s.height()));
	resize(s);

	m_table->setSelectionMode(QTable::NoSelection);

	connect(m_formatCombo, SIGNAL(activated(int)),
	  this, SLOT(formatChanged(int)));
//	connect(m_delimiterBox, SIGNAL(clicked(int)),
//	  this, SLOT(delimiterClicked(int)));
	connect(m_delimiterCombo, SIGNAL(activated(int)),
	  this, SLOT(delimiterChanged(int)));
	connect(m_delimiterEdit, SIGNAL(returnPressed()),
	  this, SLOT(returnPressed()));
	connect(m_delimiterEdit, SIGNAL(textChanged ( const QString & )),
	  this, SLOT(textChanged ( const QString & ) ));
	connect(m_comboLine, SIGNAL(activated(const QString&)),
	  this, SLOT(lineSelected(const QString&)));
	connect(m_comboQuote, SIGNAL(activated(const QString&)),
	  this, SLOT(textquoteSelected(const QString&)));
	connect(m_table, SIGNAL(currentChanged(int, int)),
	  this, SLOT(currentCellChanged(int, int)));
	connect(m_table, SIGNAL(valueChanged(int,int)),
	  this, SLOT(cellValueChanged(int,int)));
	connect(m_ignoreDuplicates, SIGNAL(stateChanged(int)),
	  this, SLOT(ignoreDuplicatesChanged(int)));
	connect(m_1stRowForFieldNames, SIGNAL(stateChanged(int)),
	  this, SLOT(slot1stRowForFieldNamesChanged(int)));

	connect(this, SIGNAL(user1Clicked()), this, SLOT(optionsButtonClicked()));
//	connect(this, SIGNAL(user2Clicked()), this, SLOT(accept()));

	currentCellChanged(0, 0);
	updateGeometry();
	m_table->setFocus();
}

KexiCSVImportDialog::~KexiCSVImportDialog()
{
  // no need to delete child widgets, Qt does it all for us
}

bool KexiCSVImportDialog::loadData()
{
	if (m_mode!=File) //data already loaded, no encoding stuff needed
		return true;

	QFile in(m_fname);
	if (!in.open(IO_ReadOnly))
	{
		KMessageBox::sorry( this, i18n("Cannot open input file '%1'.").arg(m_fname) );
		in.close();
		actionButton( Ok )->setEnabled( false );
		m_cancelled = true;
		if (parentWidget())
			parentWidget()->raise();
		return false;
	}
	QTextStream inputStream(&in);
	if (m_encoding != QString::fromLatin1(KGlobal::locale()->encoding())) {
		QTextCodec *codec = KGlobal::charsets()->codecForName(m_encoding);
		if (codec)
			inputStream.setCodec(codec); //QTextCodec::codecForName("CP1250"));
	}

//	m_fileArray = QByteArray(in.size());
	m_data = inputStream.read();
//	in.readBlock(m_fileArray.data(), in.size());
	in.close();
	return true;
}

bool KexiCSVImportDialog::cancelled()
{
  return m_cancelled;
}

void KexiCSVImportDialog::fillTable()
{
	setEnabled(false);
	QPushButton *pb = actionButton(KDialogBase::Cancel);
	if (pb)
		pb->setEnabled(true); //allow to cancel
	KexiUtils::WaitCursor wait;

	if (m_table->numRows()>0) //to accept editor
		m_table->setCurrentCell(0,0);

	int row, column, maxColumn;
	bool lastCharDelimiter = false;
	bool ignoreDups = m_ignoreDuplicates->isChecked();
	enum { S_START, S_QUOTED_FIELD, S_MAYBE_END_OF_QUOTED_FIELD, S_END_OF_QUOTED_FIELD,
		 S_MAYBE_NORMAL_FIELD, S_NORMAL_FIELD } state = S_START;

	QChar x;
	QString field = "";

	for (row = 0; row < m_table->numRows(); ++row)
		for (column = 0; column < m_table->numCols(); ++column)
			m_table->clearCell(row, column);

	row = column = 1;
	maxColumn = 0;
	QTextStream inputStream(m_data, IO_ReadOnly);

	m_detectedTypes.clear();
	m_detectedTypes.resize(1024, _NO_TYPE_YET);//_TEXT_TYPE);
	m_uniquenessTest.clear();
	m_uniquenessTest.resize(1024);
	m_1stRowForFieldNamesDetected = true;

	while (!inputStream.atEnd())
	{
		inputStream >> x; // read one char

		if (x == '\r')
			inputStream >> x; // eat '\r', to handle DOS/LOSEDOWS files correctly

		switch (state)
		{
		case S_START :
			if (x == m_textquote)
			{
				state = S_QUOTED_FIELD;
			}
			else if (x == m_delimiter)
			{
				setText(row - m_startline, column, field);
				field = "";
				if ((ignoreDups == false) || (lastCharDelimiter == false))
					++column;
				lastCharDelimiter = true;
			}
			else if (x == '\n')
			{
				++row;
				maxColumn = QMAX( maxColumn, column );
				column = 1;
			}
			else
			{
				field += x;
				state = S_MAYBE_NORMAL_FIELD;
			}
			break;
		case S_QUOTED_FIELD :
			if (x == m_textquote)
			{
				state = S_MAYBE_END_OF_QUOTED_FIELD;
			}
			else if (x == '\n')
			{
				setText(row - m_startline, column, field);
				field = "";
				if (x == '\n')
				{
					++row;
					maxColumn = QMAX( maxColumn, column );
					column = 1;
				}
				else
				{
					if ((ignoreDups == false) || (lastCharDelimiter == false))
						++column;
					lastCharDelimiter = true;
				}
				state = S_START;
			}
			else
			{
				field += x;
			}
			break;
		case S_MAYBE_END_OF_QUOTED_FIELD :
			if (x == m_textquote)
			{
				field += x;
				state = S_QUOTED_FIELD;
			}
			else if (x == m_delimiter || x == '\n')
			{
				setText(row - m_startline, column, field);
				field = "";
				if (x == '\n')
				{
					++row;
					maxColumn = QMAX( maxColumn, column );
					column = 1;
				}
				else
				{
					if ((ignoreDups == false) || (lastCharDelimiter == false))
						++column;
					lastCharDelimiter = true;
				}
				state = S_START;
			}
			else
			{
				state = S_END_OF_QUOTED_FIELD;
			}
			break;
		case S_END_OF_QUOTED_FIELD :
			if (x == m_delimiter || x == '\n')
			{
				setText(row - m_startline, column, field);
				field = "";
				if (x == '\n')
				{
					++row;
					maxColumn = QMAX( maxColumn, column );
					column = 1;
				}
				else
				{
					if ((ignoreDups == false) || (lastCharDelimiter == false))
						++column;
					lastCharDelimiter = true;
				}
				state = S_START;
			}
			else
			{
				state = S_END_OF_QUOTED_FIELD;
			}
			break;
		case S_MAYBE_NORMAL_FIELD :
			if (x == m_textquote)
			{
				field = "";
				state = S_QUOTED_FIELD;
				break;
			}
		case S_NORMAL_FIELD :
			if (x == m_delimiter || x == '\n')
			{
				setText(row - m_startline, column, field);
				field = "";
				if (x == '\n')
				{
					++row;
					maxColumn = QMAX( maxColumn, column );
					column = 1;
				}
				else
				{
					if ((ignoreDups == false) || (lastCharDelimiter == false))
						++column;
					lastCharDelimiter = true;
				}
				state = S_START;
			}
			else
			{
				field += x;
			}
		}
		if (x != m_delimiter)
			lastCharDelimiter = false;

		if (m_firstFillTableCall && row==2 && !m_1stRowForFieldNames->isChecked() && m_1stRowForFieldNamesDetected) {
			//'1st row for field name' flag detected: reload table
			m_1stRowForFieldNamesDetected = false;
			m_table->setNumRows( 0 );
			m_1stRowForFieldNames->setChecked(true); //this will reload table
			m_firstFillTableCall = false; //this trick is allowed only once, on startup
			slot1stRowForFieldNamesChanged(1);
			return;
		}

		if (row % 128 == 0)
			qApp->processEvents();
	}

	m_1stRowForFieldNamesDetected = false;

	// file with only one line without '\n'
	if (field.length() > 0)
	{
		setText(row - m_startline, column, field);
		++row;
		field = "";
	}

	adjustRows( row - m_startline - (m_1stRowForFieldNames->isChecked()?1:0) );

	maxColumn = QMAX( maxColumn, column );
	m_table->setNumCols(maxColumn);

	for (column = 0; column < m_table->numCols(); ++column)
	{
//		QString header = m_table->horizontalHeader()->label(column);
//		if (header != i18n("Text") && header != i18n("Number") &&
//			header != i18n("Date") && header != i18n("Currency"))
//		const int detectedType = m_detectedTypes[column+1];
//		m_table->horizontalHeader()->setLabel(column, m_typeNames[ detectedType ]); //i18n("Text"));
		updateColumnText(column);
		if (!m_columnsAdjusted)
			m_table->adjustColumn(column);
	}
	m_columnsAdjusted = true;

	if (m_primaryKeyColumn>=0 && m_primaryKeyColumn<m_table->numCols()) {
		if (_NUMBER_TYPE != m_detectedTypes[ m_primaryKeyColumn ]) {
			m_primaryKeyColumn = -1;
		}
	}

	m_prevSelectedCol = -1;
	m_table->setCurrentCell(0,0);
	currentCellChanged(0, 0);
	if (m_primaryKeyColumn != -1)
		m_table->setPixmap(0, m_primaryKeyColumn, m_pkIcon);
	setEnabled(true);

	fillComboBox();
}

void KexiCSVImportDialog::updateColumnText(int col)
{
	QString colName;
	if (col<(int)m_columnNames.count() && (m_1stRowForFieldNames->isChecked() || m_changedColumnNames[col]))
		colName = m_columnNames[ col ];
	if (colName.isEmpty()) {
		colName = i18n("Column %1").arg(col+1); //will be changed to a valid identifier on import
		m_changedColumnNames[ col ] = false;
	}
	int detectedType = m_detectedTypes[col];
	if (detectedType==_FP_NUMBER_TYPE)
		detectedType=_NUMBER_TYPE; //we're simplifying that for now
	else if (detectedType==_NO_TYPE_YET) {
		m_detectedTypes[col]=_TEXT_TYPE; //entirely empty column
		detectedType=_TEXT_TYPE;
	}
	m_table->horizontalHeader()->setLabel(col, 
		i18n("Column %1").arg(col+1) + "  \n(" + m_typeNames[ detectedType ] + ")  ");
	m_table->setText(0, col, colName);
	m_table->horizontalHeader()->adjustHeaderSize();

	//check uniqueness
	QValueList<int> *list = m_uniquenessTest[col];
	if (m_primaryKeyColumn==-1 && list && !list->isEmpty()) {
		qHeapSort(*list);
		QValueList<int>::ConstIterator it=list->constBegin();
		int prevValue = *it;
		++it;
		for(; it!=list->constEnd() && prevValue!=(*it); ++it)
			prevValue=(*it);
		if (it!=list->constEnd()) {
			//duplicates:
			list->clear();
		}
		else {
			//a candidate for PK (autodetected)!
			if (-1==m_primaryKeyColumn) {
				m_primaryKeyColumn=col;
			}
		}
	}
	if (list) //not needed now: conserve memory
		list->clear();
}

void KexiCSVImportDialog::fillComboBox()
{
	m_comboLine->clear();
	const uint count = QMAX(0, m_table->numRows()-1+m_startline);
	for (uint row = 0; row < count; ++row)
		m_comboLine->insertItem(QString::number(row + 1), row);
	m_comboLine->setCurrentItem(m_startline);
}

void KexiCSVImportDialog::detectTypeAndUniqueness(int row, int col, const QString& text)
{
	int intValue;
	const int type = m_detectedTypes[col];
	if (row==1 || type!=_TEXT_TYPE) {
		bool found = false;
		//detect type because it's 1st row or all prev. rows were not text
		//-number?
		if (row==1 || type==_NUMBER_TYPE || type==_NO_TYPE_YET) {
			bool ok = text.isEmpty();//empty values allowed
			if (!ok)
				intValue = text.toInt(&ok);
			if (ok && (row==1 || type==_NO_TYPE_YET)) {
				m_detectedTypes[col]=_NUMBER_TYPE;
				found = true; //yes
			}
		}
		//-FP number?
		if (!found && (row==1 || type==_FP_NUMBER_TYPE || type==_NO_TYPE_YET)) {
			bool ok = text.isEmpty() || m_fpNumberRegExp.exactMatch(text);
			if (!ok)
				text.toInt(&ok);
			if (ok && (row==1 || type==_NO_TYPE_YET)) {
				m_detectedTypes[col]=_FP_NUMBER_TYPE;
				found = true; //yes
			}
		}
		//-date?
		if (!found && (row==1 || type==_DATE_TYPE || type==_NO_TYPE_YET)) {
			if ((row==1 || type==_NO_TYPE_YET)
				&& (text.isEmpty() || m_dateRegExp1.exactMatch(text) || m_dateRegExp2.exactMatch(text)))
			{
				m_detectedTypes[col]=_DATE_TYPE;
				found = true; //yes
			}
		}
		if (!found && type==_NO_TYPE_YET && !text.isEmpty()) {
			//eventually, a non-emptytext after a while
			m_detectedTypes[col]=_TEXT_TYPE;
			found = true; //yes
		}
		//default: text type (already set)
	}
	//check uniqueness for this value
	QValueList<int> *list = m_uniquenessTest[col];
	if (row==1 && (!list || !list->isEmpty()) && !text.isEmpty() && _NUMBER_TYPE == m_detectedTypes[col]) {
		if (!list) {
			list = new QValueList<int>();
			m_uniquenessTest.insert(col, list);
		}
		list->append( intValue );
	}
	else {
		//the value is empty or uniqueness test failed in the past
		if (list && !list->isEmpty())
			list->clear(); //indicate that uniqueness test failed
	}
}

void KexiCSVImportDialog::setText(int row, int col, const QString& text)
{
	if (m_table->numCols() < col) {
		m_table->setNumCols(col);
		if ((int)m_columnNames.size() < m_table->numCols()) {
			m_columnNames.resize(m_table->numCols()+10);
			m_changedColumnNames.resize(m_table->numCols()+10);
		}
	}

	if (m_1stRowForFieldNames->isChecked()) {
		if ((row+m_startline)==1) {//this is for column name
			if ((col-1) < (int)m_changedColumnNames.size() && false==m_changedColumnNames[col-1]) {
				//this column has no custom name entered by a user
				//-get the name from the data cell
				QString colName(text.simplifyWhiteSpace());
				if (!colName.isEmpty()) {
					if (colName.left(1)>="0" && colName.left(1)<="9")
						colName.prepend(i18n("Column")+" ");
					m_columnNames[ col-1 ] = colName;
				}
			}
			return;
		}
	}
	else {
		if ((row+m_startline)==1) {//this row is for column name
			if (m_1stRowForFieldNamesDetected && !m_1stRowForFieldNames->isChecked()) {
				QString f( text.simplifyWhiteSpace() );
				if (f.isEmpty() || !f[0].isLetter())
					m_1stRowForFieldNamesDetected = false; //this couldn't be a column name
			}
		}
		row++; //1st row was for column names
	}

	if (row < 2) // skipped by the user
		return;

	if (m_table->numRows() < row) {
		m_table->setNumRows(row+5000); /* We add 5000 at a time to limit recalculations */
		m_table->verticalHeader()->setLabel(0, i18n("Column name")+"   ");
		m_adjustRows=1;
	}

	m_table->setText(row - 1, col - 1, text);
	m_table->verticalHeader()->setLabel(row-1, QString::number(row-1));

	detectTypeAndUniqueness(row-1, col-1, text);
}

/*
 * Called after the first fillTable() when number of rows are unknown.
 */
void KexiCSVImportDialog::adjustRows(int iRows)
{
	if (m_adjustRows)
	{
		m_table->setNumRows( iRows );
		m_adjustRows=0;
	}
}

void KexiCSVImportDialog::returnPressed()
{
	if (m_delimiterCombo->currentItem() != 4)
		return;

	m_delimiter = m_delimiterEdit->text();
	fillTable();
}

void KexiCSVImportDialog::textChanged ( const QString & )
{
//	m_radioOther->setChecked ( true );
	delimiterChanged(4); // other
}

void KexiCSVImportDialog::formatChanged(int id)
{
	if (id==_PK_FLAG) {
		if (m_primaryKeyColumn>=0 && m_primaryKeyColumn<m_table->numCols()) {
			m_table->setPixmap(0, m_primaryKeyColumn, QPixmap());
		}
		if (m_primaryKeyField->isChecked()) {
			m_primaryKeyColumn = m_table->currentColumn();
			m_table->setPixmap(0, m_primaryKeyColumn, m_pkIcon);
		}
		else
			m_primaryKeyColumn = -1;
		return;
	}
	else {
		m_detectedTypes[m_table->currentColumn()]=id;
		m_primaryKeyField->setEnabled( _NUMBER_TYPE == id );
		m_primaryKeyField->setChecked( m_primaryKeyColumn == m_table->currentColumn() );
	}
	updateColumnText(m_table->currentColumn());
}

void KexiCSVImportDialog::delimiterChanged(int id)
{
	switch (id)
	{
	 case 0: // comma
	m_delimiter = ",";
	break;
	 case 1: // semicolon
	m_delimiter = ";";
	break;
	 case 2: // tab
	m_delimiter = "\t";
	break;
	 case 3: // space
	m_delimiter = " ";
	break;
	 case 4: // other
	m_delimiter = m_delimiterEdit->text();
	break;
	}
	m_delimiterEdit->setEnabled(id==4);
	m_columnsAdjusted = false;
	fillTable();
}

void KexiCSVImportDialog::textquoteSelected(const QString& mark)
{
	if (mark == i18n("none"))
	m_textquote = 0;
	else
	m_textquote = mark[0];

	fillTable();
}

void KexiCSVImportDialog::lineSelected(const QString& line)
{
	const int startline = line.toInt() - 1;
	if (m_startline == startline)
		return;
	m_startline = startline;
	m_adjustRows=1;
	fillTable();
}

void KexiCSVImportDialog::currentCellChanged(int, int col)
{
	if (m_prevSelectedCol==col)
		return;
	m_prevSelectedCol = col;
//	int id;
//	QString header = m_table->horizontalHeader()->label(col);

/*	if (header == i18n("Text"))
		id = 1;
	else if (header == i18n("Number"))
		id = 0;
	else if (header == i18n("Date"))
		id = 3;
	else
		id = 2;*/

	int type = m_detectedTypes[col];
	if (type==_FP_NUMBER_TYPE)
		type=_NUMBER_TYPE; //we're simplifying that for now

	m_formatCombo->setCurrentItem( type );
	m_formatLabel->setText( m_formatComboText.arg(col+1) );
	m_primaryKeyField->setEnabled( _NUMBER_TYPE == m_detectedTypes[col]);
	m_primaryKeyField->setChecked( m_primaryKeyColumn == col );
}

void KexiCSVImportDialog::cellValueChanged(int row,int col)
{
	if (row==0) {//column name has changed
		m_columnNames[ col ] = m_table->text(row, col);
		m_changedColumnNames.setBit( col );
	}
}

void KexiCSVImportDialog::accept()
{
//! @todo MOVE MOST OF THIS TO CORE/ (KexiProject?) after KexiDialogBase code is moved to non-gui place

	KexiGUIMessageHandler msg; //! @todo make it better integrated with main window

	const uint numRows( m_table->numRows() );
	if (numRows == 0)
		return; //impossible

	if (numRows == 1) {
		if (KMessageBox::No == KMessageBox::questionYesNo(this, i18n("Data set contains no rows. Do you want to import empty table?")))
			return;
	}

	KexiProject* project = m_mainWin->project();
	if (!project) {
		msg.showErrorMessage(i18n("No project available."));
		return;
	}
	KexiDB::Connection *conn = project->dbConnection();
	if (!conn) {
		msg.showErrorMessage(i18n("No database connection available."));
		return;
	}
	KexiPart::Part *part = Kexi::partManager().partForMimeType("kexi/table");
	if (!part) {
		msg.showErrorMessage(&Kexi::partManager());
		return;
	}

	//get suggested name based on the file name
	QString suggestedName;
	if (m_mode==File) {
		suggestedName = KURL::fromPathOrURL(m_fname).fileName();
		//remove extension
		if (!suggestedName.isEmpty()) {
			const int idx = suggestedName.findRev(".");
			if (idx!=-1)
				suggestedName = suggestedName.mid(0, idx ).simplifyWhiteSpace();
		}
	}

	//-new part item
	KexiPart::Item *partItem = project->createPartItem(part->info(), suggestedName);
	if (!partItem) {
//		msg.showErrorMessage(project);
		return;
	}

#define _ERR \
		project->deleteUnstoredItem(partItem); \
		return

	//-ask for table name/title
	// (THIS IS FROM KexiMainWindowImpl::saveObject())
	bool allowOverwriting = true;
	tristate res = m_mainWin->getNewObjectInfo( partItem, part, allowOverwriting );
	if (~res || !res) {
		//! @todo: err
		_ERR;
	}
	//(allowOverwriting is now set to true, if user accepts overwriting, 
	// and overwriting will be needed)

//	KexiDB::SchemaData sdata(part->info()->projectPartID());
//	sdata.setName( partItem->name() );

	//-create table schema (and thus schema object)
	//-assign information (THIS IS FROM KexiDialogBase::storeNewData())
	KexiDB::TableSchema *tableSchema = new KexiDB::TableSchema(partItem->name());
	tableSchema->setCaption( partItem->caption() );
	tableSchema->setDescription( partItem->description() );
	const uint numCols( m_table->numCols() );

	bool implicitPrimaryKeyAdded = false;
#if 0 //! @todo don't do this unless user wanted it
	if (m_primaryKeyColumn==-1) {
		//add implicit PK field
//! @todo make this field hidden (what about e.g. pgsql?)
		implicitPrimaryKeyAdded = true;

		QString fieldName("id");
		QString fieldCaption("Id");

		QStringList colnames;
		for (uint col = 0; col < numCols; col++)
			colnames.append( m_table->text(0, col).lower().simplifyWhiteSpace() );

		if (colnames.find(fieldName)!=colnames.end()) {
			int num = 1;
			while (colnames.find(fieldName+QString::number(num))!=colnames.end())
				num++;
			fieldName += QString::number(num);
			fieldCaption += QString::number(num);
		}
		KexiDB::Field *field = new KexiDB::Field(
			fieldName,
			KexiDB::Field::Integer,
			KexiDB::Field::NoConstraints,
			KexiDB::Field::NoOptions,
			0,0, //uint length=0, uint precision=0,
			QVariant(), //QVariant defaultValue=QVariant(),
			fieldCaption
		); //no description and width for now
		field->setPrimaryKey(true);
		field->setAutoIncrement(true);
		tableSchema->addField( field );
	}
#endif //implicitPrimaryKeyAdded

	for (uint col = 0; col < numCols; col++) {
		QString fieldCaption( m_table->text(0, col).simplifyWhiteSpace() );
		QString fieldName( KexiUtils::string2Identifier( fieldCaption ) );
		if (tableSchema->field(fieldName)) {
			QString fixedFieldName;
			uint i = 2; //"apple 2, apple 3, etc. if there're many "apple" names
			do {
				fixedFieldName = fieldName + "_" + QString::number(i);
				if (!tableSchema->field(fixedFieldName))
					break;
				i++;
			} while (true);
			fieldName = fixedFieldName;
			fieldCaption += (" " + QString::number(i));
		}
		const int detectedType = m_detectedTypes[col];
		KexiDB::Field::Type fieldType;
		if (detectedType==_DATE_TYPE)
			fieldType = KexiDB::Field::Date;
//! @todo what about time and date/time?
		else if (detectedType==_NUMBER_TYPE)
			fieldType = KexiDB::Field::Integer;
		else if (detectedType==_FP_NUMBER_TYPE)
			fieldType = KexiDB::Field::Double;
//! @todo what about time and float/double types and different integer subtypes?
		else //_TEXT_TYPE and the rest
			fieldType = KexiDB::Field::Text;
//! @todo what about long text?

		KexiDB::Field *field = new KexiDB::Field(
			fieldName,
			fieldType,
			KexiDB::Field::NoConstraints,
			KexiDB::Field::NoOptions,
			0,0, //uint length=0, uint precision=0,
			QVariant(), //QVariant defaultValue=QVariant(),
			fieldCaption
		); //no description and width for now
		if ((int)col == m_primaryKeyColumn) {
			field->setPrimaryKey(true);
			field->setAutoIncrement(true);
		}
		tableSchema->addField( field );
	}

	KexiDB::Transaction transaction = conn->beginTransaction();
	if (transaction.isNull()) {
		msg.showErrorMessage(conn);
		_ERR;
	}
	KexiDB::TransactionGuard tg(transaction);

	//-create physical table
	if (!conn->createTable(tableSchema, allowOverwriting)) {
		msg.showErrorMessage(conn);
		delete tableSchema;
		_ERR;
	}

#define _DROP_TABLE_AND_RETURN \
	project->deleteUnstoredItem(partItem); \
	conn->dropTable(tableSchema); /*alsoRemoveSchema*/ \
	return;
//	conn->removeTableSchemaInternal(tableSchema); 
//	return

/*already done in createTable()
	if (!conn->storeObjectSchemaData( *tableSchema, true )) {
		msg.showErrorMessage(conn);
		_ERR2;
	}*/

	//-fill table contents
	QValueList<QVariant> values;
	for (uint row = 1/*skip "types" row*/; row < numRows; row++) {
		if (implicitPrimaryKeyAdded)
			values.append( QVariant() ); //id will be autogenerated here
		for (uint col = 0; col < numCols; col++) {
			const int detectedType = m_detectedTypes[col];
			QString t( m_table->text(row,col) );
			if (detectedType==_NUMBER_TYPE)
				values.append( t.isEmpty() ? QVariant() : t.toInt() );
//! @todo what about time and float/double types and different integer subtypes?
			else if (detectedType==_FP_NUMBER_TYPE)
				values.append( t.isEmpty() ? QVariant() : t.toDouble() );
			else if (detectedType==_DATE_TYPE)
				//todo
				values.append( t.isEmpty() ? QVariant() : QDate::fromString(t, Qt::ISODate) );
//! @todo what about time and date/time?
			else //_TEXT_TYPE and the rest
				values.append( t );
		}
		if (!conn->insertRecord(*tableSchema, values)) {
			msg.showErrorMessage(conn);
			_DROP_TABLE_AND_RETURN;
		}
		values.clear();
	}

	if (!tg.commit()) {
		msg.showErrorMessage(conn);
		_DROP_TABLE_AND_RETURN;
	}

	//-now we can store the item
	partItem->setIdentifier( tableSchema->id() );
	project->addStoredItem( part->info(), partItem );

	QDialog::accept();
	KMessageBox::information(this, i18n("Data has been successfully imported to table \"%1\".").arg(tableSchema->name()));
	parentWidget()->raise();
}

int KexiCSVImportDialog::getHeader(int col)
{
	QString header = m_table->horizontalHeader()->label(col);

	if (header == i18n("Text"))
	return TEXT;
	else if (header == i18n("Number"))
	return NUMBER;
	else if (header == i18n("Currency"))
	return CURRENCY;
	else
	return DATE;
}

QString KexiCSVImportDialog::getText(int row, int col)
{
	return m_table->text(row, col);
}

void KexiCSVImportDialog::ignoreDuplicatesChanged(int)
{
	fillTable();
}

void KexiCSVImportDialog::slot1stRowForFieldNamesChanged(int)
{
	m_adjustRows=1;
	if (m_1stRowForFieldNames->isChecked() && m_startline>0 && m_startline>=(m_comboLine->count()-1))
		m_startline--;
	fillTable();
//	if (m_1stRowForFieldNames->isChecked())
//		m_comboLine->removeItem(m_comboLine->count()-1);
//	else
//		m_comboLine->insertItem(QString::number(m_comboLine->count()+1));
}

void KexiCSVImportDialog::optionsButtonClicked()
{
	KexiCSVImportOptionsDialog dlg(m_encoding, this);
	if (QDialog::Accepted != dlg.exec())
		return;

	if (m_encoding != dlg.encodingComboBox()->selectedEncoding()) {
		m_encoding = dlg.encodingComboBox()->selectedEncoding();
		if (!loadData())
			return;
		fillTable();
	}
}

#include "kexicsvimportdialog.moc"
