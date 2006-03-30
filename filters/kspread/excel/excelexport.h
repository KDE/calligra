/* insert your license here */

#ifndef EXCELEXPORT_TEST_H
#define EXCELEXPORT_TEST_H

#include <KoFilter.h>
#include <KSpreadLeader.h>
#include <KSpreadBaseWorker.h>
//Added by qt3to4:
#include <Q3CString>


class ExcelExport : public KoFilter {
	Q_OBJECT

public:
	ExcelExport(KoFilter *parent, const char*name, const QStringList&);
	virtual ~ExcelExport() {}

	virtual KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to);
};


class ExcelWorker : public KSpreadBaseWorker {
public:
	// Implement those
	//KoFilter::ConversionStatus startDocument(KSpreadFilterProperty property);
	//KoFilter::ConversionStatus startInfoLog(KSpreadFilterProperty property);
	//KoFilter::ConversionStatus startInfoAuthor(KSpreadFilterProperty property);
	//KoFilter::ConversionStatus startInfoAbout(KSpreadFilterProperty property);
	//KoFilter::ConversionStatus startSpreadBook(KSpreadFilterProperty property);
	//KoFilter::ConversionStatus startSpreadSheet(KSpreadFilterProperty property);
	//KoFilter::ConversionStatus startSpreadCell(KSpreadFilterProperty property);
};

#endif
