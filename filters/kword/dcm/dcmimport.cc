#include <sstream>

#include <QFile>
#include <QString>
#include <QApplication>
#include <QTextDocument>
#include <QTextDocumentWriter>

#include <KoFilterChain.h>
#include <kgenericfactory.h>
#include <KoFilterManager.h>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmsr/dsrdoc.h>
#include <dcmtk/ofstd/ofstream.h>
#include <dcmtk/dcmsr/dsrtypes.h>

#include "dcmimport.h"
#include "DCMImportDialog.h"

typedef KGenericFactory<DCMImport> DCMImportFactory;
K_EXPORT_COMPONENT_FACTORY(libdcmimport, DCMImportFactory("kofficefilters"))

KoFilter::ConversionStatus DCMImport::convert(QByteArray const& from, QByteArray const& to)
{
	if (to != "application/x-kword" || from != "application/dicom")
		return KoFilter::NotImplemented;

	qDebug() << "Files: from: (" << from << ") " << m_chain->inputFile() << " to: (" << to << ") " << m_chain->outputFile();

	OFCondition c;

	DcmFileFormat ff;
	if ((c = ff.loadFile(m_chain->inputFile().toLocal8Bit().constData())).bad())
		return KoFilter::WrongFormat;

	DCMImportDialog* dialog = 0;
	if (!m_chain->manager()->getBatchMode()) {
		dialog = new DCMImportDialog(QApplication::activeWindow());
		if (!dialog)
			return KoFilter::OutOfMemory;
		if (!dialog->exec())
			return KoFilter::UserCancelled;
	}

	size_t readFlags = 0;

	if (dialog->getDS())
		readFlags |= DSRTypes::RF_readDigitalSignatures;

	if (dialog->getICIE())
		readFlags |= DSRTypes::RF_ignoreContentItemErrors;

	if (dialog->getIRC())
		readFlags |= DSRTypes::RF_ignoreRelationshipConstraints;

	if (dialog->getSICI())
		readFlags |= DSRTypes::RF_skipInvalidContentItems;

	delete dialog;

	DSRDocument doc;
	if((c = doc.read(*ff.getDataset(), readFlags)).bad())
		return KoFilter::ParsingError;

	std::ostringstream htmlStream;
	if ((c = doc.renderHTML(htmlStream)).bad())
		return KoFilter::StupidError;

	QTextDocument qdoc;
	qdoc.setHtml(QString(htmlStream.str().c_str()));

	QTextDocumentWriter qdocWriter(m_chain->outputFile(), "odf");
	if (qdocWriter.write(&qdoc))
		return KoFilter::OK;
	else
		return KoFilter::CreationError;
}
