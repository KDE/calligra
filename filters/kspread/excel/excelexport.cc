/* insert your license here */

#include <excelexport.h>
#include <kdebug.h>
#include <kgenericfactory.h>
//Added by qt3to4:
#include <Q3CString>


typedef KGenericFactory<ExcelExport, KoFilter> ExcelExportFactory;
//K_EXPORT_COMPONENT_FACTORY(libkspreadexcelexport, ExcelExportFactory("excelexport"))


ExcelExport::ExcelExport(KoFilter *, const char *, const QStringList&) : KoFilter() {
}


KoFilter::ConversionStatus ExcelExport::convert(const QByteArray& from, const QByteArray& to) {
	// Double check that's we really what we want to do
	if ( ( (to != "application/excel") && (to != "application/msexcel") ) || from != "application/x-kspread") {
		kWarning(0) << "Invalid mimetypes " << to << ", " << from << endl;
		return KoFilter::NotImplemented;
	}

	KSpreadLeader *leader = new KSpreadLeader(m_chain);
	ExcelWorker *worker = new ExcelWorker();
	leader->setWorker(worker);
	return leader->convert();
}


/*
KoFilter::ConversionStatus ExcelWorker::startDocument(KSpreadFilterProperty property) {
	return KoFilter::OK;
}


KoFilter::ConversionStatus ExcelWorker::startInfoLog(KSpreadFilterProperty property) {
	return KoFilter::OK;
}


KoFilter::ConversionStatus ExcelWorker::startInfoAuthor(KSpreadFilterProperty property) {
	return KoFilter::OK;
}


KoFilter::ConversionStatus ExcelWorker::startInfoAbout(KSpreadFilterProperty property) {
	return KoFilter::OK;
}


KoFilter::ConversionStatus ExcelWorker::startSpreadBook(KSpreadFilterProperty property) {
	return KoFilter::OK;
}


KoFilter::ConversionStatus ExcelWorker::startSpreadSheet(KSpreadFilterProperty property) {
	return KoFilter::OK;
}


KoFilter::ConversionStatus ExcelWorker::startSpreadCell(KSpreadFilterProperty property) {
	return KoFilter::OK;
}
*/

#include <excelexport.moc>
