/* This file is part of the KDE project
   Copyright (C) 2005,2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexicsvexport.h"
#include "kexicsvwidgets.h"
#include <main/startup/KexiStartupFileDialog.h>
#include <kexidb/cursor.h>
#include <kexidb/utils.h>
#include <core/KexiMainWindowIface.h>
#include <core/kexiproject.h>
#include <core/kexipartinfo.h>
#include <core/kexipartmanager.h>
#include <core/kexiguimsghandler.h>
#include <kexiutils/utils.h>
#include <widget/kexicharencodingcombobox.h>

#include <QTextStream>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qclipboard.h>
#include <kapplication.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kactivelabel.h>
#include <kpushbutton.h>
#include <kapplication.h>
#include <kdebug.h>
#include <ksavefile.h>


using namespace KexiCSVExport;

Options::Options()
 : mode(File), itemId(0), addColumnNames(true)
{
}

bool Options::assign( QMap<QString,QString>& args )
{
	mode = (args["destinationType"]=="file")
		? KexiCSVExport::File : KexiCSVExport::Clipboard;

	if (args.contains("delimiter"))
		delimiter = args["delimiter"];
	else
		delimiter = (mode==File) ? KEXICSV_DEFAULT_FILE_DELIMITER : KEXICSV_DEFAULT_CLIPBOARD_DELIMITER;

	if (args.contains("textQuote"))
		textQuote = args["textQuote"];
	else
		textQuote = (mode==File) ? KEXICSV_DEFAULT_FILE_TEXT_QUOTE : KEXICSV_DEFAULT_CLIPBOARD_TEXT_QUOTE;

	bool ok;
	itemId = args["itemId"].toInt(&ok);
	if (!ok || itemId<=0)
		return false;
	if (args.contains("forceDelimiter"))
		forceDelimiter = args["forceDelimiter"];
	if (args.contains("addColumnNames"))
		addColumnNames = (args["addColumnNames"]=="1");
	return true;
}

//------------------------------------

bool KexiCSVExport::exportData(KexiDB::TableOrQuerySchema& tableOrQuery, 
	const Options& options, int rowCount, QTextStream *predefinedTextStream)
{
	KexiDB::Connection* conn = tableOrQuery.connection();
	if (!conn)
		return false;

	if (rowCount == -1)
		rowCount = KexiDB::rowCount(tableOrQuery);
	if (rowCount == -1)
		return false;

//! @todo move this to non-GUI location so it can be also used via command line
//! @todo add a "finish" page with a progressbar.
//! @todo look at rowCount whether the data is really large; 
//!       if so: avoid copying to clipboard (or ask user) because of system memory

//! @todo OPTIMIZATION: use fieldsExpanded(true /*UNIQUE*/)
//! @todo OPTIMIZATION? (avoid multiple data retrieving) look for already fetched data within KexiProject..

	KexiDB::QuerySchema* query = tableOrQuery.query();
	if (!query)
		query = tableOrQuery.table()->query();

	KexiDB::QueryColumnInfo::Vector fields( query->fieldsExpanded( KexiDB::QuerySchema::WithInternalFields ) );
	QString buffer;

	KSaveFile *kSaveFile = 0;
	QTextStream *stream = 0;
	QTextStream *kSaveFileTextStream = 0; // we'll delete it as KSaveFile's stream

	const bool copyToClipboard = options.mode==Clipboard;
	if (copyToClipboard) {
//! @todo (during exporting): enlarge bufSize by factor of 2 when it became too small
		uint bufSize = qMin((rowCount<0 ? 10 : rowCount) * fields.count() * 20, (uint)128000);
		buffer.reserve( bufSize );
		if ((uint)buffer.capacity() < bufSize) {
			kWarning() << "KexiCSVExportWizard::exportData() cannot allocate memory for " << bufSize 
				<< " characters" << endl;
			return false;
		}
	}
	else {
		if (predefinedTextStream) {
			stream = predefinedTextStream;
		}
		else {
			if (options.fileName.isEmpty()) {//sanity
				kWarning() << "KexiCSVExportWizard::exportData(): fname is empty" << endl;
				return false;
			}
			kSaveFile = new KSaveFile(options.fileName);
			if (QFile::NoError == kSaveFile->error()) {
				kSaveFileTextStream = new QTextStream(kSaveFile);
				stream = kSaveFileTextStream;
			}
			if (QFile::NoError != kSaveFile->error() || !stream) {//sanity
				kWarning() << "KexiCSVExportWizard::exportData(): status != 0 or stream == 0" << endl;
//! @todo show error
				delete kSaveFileTextStream;
				delete kSaveFile;
				return false;
			}
		}
	}

//! @todo escape strings

#define _ERR \
	delete [] isText; \
	if (kSaveFile) { kSaveFile->abort(); delete kSaveFile; delete kSaveFileTextStream; } \
	return false

#define APPEND(what) \
		if (copyToClipboard) buffer.append(what); else (*stream) << (what)

// line endings should be as in RFC 4180
#define CSV_EOLN "\r\n"

	// 0. Cache information
	const uint fieldsCount = query->fieldsExpanded().count(); //real fields count without internals
	const Q3CString delimiter( options.delimiter.left(1).toLatin1() );
	const bool hasTextQuote = !options.textQuote.isEmpty();
	const QString textQuote( options.textQuote.left(1) );
	const Q3CString escapedTextQuote( (textQuote + textQuote).toLatin1() ); //ok?
	//cache for faster checks
	bool *isText = new bool[fieldsCount]; 
	bool *isDateTime = new bool[fieldsCount]; 
	bool *isTime = new bool[fieldsCount]; 
	bool *isBLOB = new bool[fieldsCount]; 
	uint *visibleFieldIndex = new uint[fieldsCount];
//	bool isInteger[fieldsCount]; //cache for faster checks
//	bool isFloatingPoint[fieldsCount]; //cache for faster checks
	for (uint i=0; i<fieldsCount; i++) {
		KexiDB::QueryColumnInfo* ci;
		const int indexForVisibleLookupValue = fields[i]->indexForVisibleLookupValue();
		if (-1 != indexForVisibleLookupValue) {
			ci = query->expandedOrInternalField( indexForVisibleLookupValue );
			visibleFieldIndex[i] = indexForVisibleLookupValue;
		}
		else {
			ci = fields[i];
			visibleFieldIndex[i] = i;
		}

		isText[i] = ci->field->isTextType();
		isDateTime[i] = ci->field->type()==KexiDB::Field::DateTime;
		isTime[i] = ci->field->type()==KexiDB::Field::Time;
		isBLOB[i] = ci->field->type()==KexiDB::Field::BLOB;
//		isInteger[i] = fields[i]->field->isIntegerType() 
//			|| fields[i]->field->type()==KexiDB::Field::Boolean;
//		isFloatingPoint[i] = fields[i]->field->isFPNumericType();
	}

	// 1. Output column names
	if (options.addColumnNames) {
		for (uint i=0; i<fieldsCount; i++) {
			if (i>0)
				APPEND( delimiter );
			if (hasTextQuote){
				APPEND( textQuote + fields[i]->captionOrAliasOrName().replace(textQuote, escapedTextQuote) + textQuote );
			}
			else {
				APPEND( fields[i]->captionOrAliasOrName() );
			}
		}
		APPEND(CSV_EOLN);
	}
	
	KexiGUIMessageHandler handler;
	KexiDB::Cursor *cursor = conn->executeQuery(*query);
	if (!cursor) {
		handler.showErrorMessage(conn);
		_ERR;
	}
	for (cursor->moveFirst(); !cursor->eof() && !cursor->error(); cursor->moveNext()) {
		const uint realFieldCount = qMin(cursor->fieldCount(), fieldsCount);
		for (uint i=0; i<realFieldCount; i++) {
			const uint real_i = visibleFieldIndex[i];
			if (i>0)
				APPEND( delimiter );
			if (cursor->value(real_i).isNull())
				continue;
			if (isText[real_i]) {
				if (hasTextQuote)
					APPEND( textQuote + QString(cursor->value(real_i).toString()).replace(textQuote, escapedTextQuote) + textQuote );
				else
					APPEND( cursor->value(real_i).toString() );
			}
			else if (isDateTime[real_i]) { //avoid "T" in ISO DateTime
				APPEND( cursor->value(real_i).toDateTime().date().toString(Qt::ISODate)+" "
					+ cursor->value(real_i).toDateTime().time().toString(Qt::ISODate) );
			}
			else if (isTime[real_i]) { //time is temporarily stored as null date + time...
				APPEND( cursor->value(real_i).toTime().toString(Qt::ISODate) );
			}
			else if (isBLOB[real_i]) { //BLOB is escaped in a special way
				if (hasTextQuote)
//! @todo add options to suppport other types from KexiDB::BLOBEscapingType enum...
					APPEND( textQuote + KexiDB::escapeBLOB(cursor->value(real_i).toByteArray(), KexiDB::BLOBEscapeHex) + textQuote );
				else
					APPEND( KexiDB::escapeBLOB(cursor->value(real_i).toByteArray(), KexiDB::BLOBEscapeHex) );
			}
			else {//other types
				APPEND( cursor->value(real_i).toString() );
			}
		}
		APPEND(CSV_EOLN);
	}

	if (copyToClipboard)
		buffer.squeeze();

	if (!conn->deleteCursor(cursor)) {
		handler.showErrorMessage(conn);
		_ERR;
	}

	if (copyToClipboard)
		kapp->clipboard()->setText(buffer, QClipboard::Clipboard);

	delete [] isText;
	delete [] isDateTime;
	delete [] isTime;
	delete [] isBLOB;
	delete [] visibleFieldIndex;

	if (kSaveFile) {
		kSaveFile->close();
		delete kSaveFileTextStream;
		delete kSaveFile;
	}
	return true;
}
