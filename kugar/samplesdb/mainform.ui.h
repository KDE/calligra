/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
#include "mdatabasereportengine.h"
#include "mreportviewer.h"

void mainForm::directAccessSlot()
{
	MReportViewer* rptviewer = new MReportViewer();
	MDatabaseReportEngine* rptdata = new MDatabaseReportEngine();

	/** open the kugar template for create the kugar date file from the database */
	QFile fd("sampledb1.kud");
	if (fd.open(IO_WriteOnly | IO_Translate))
	{
		if (!rptdata->createReportDataFile(&fd, "sampledb1.kut") )
			qDebug("Unable to create the data file: %s","sampledb1.kud");
		fd.close();
	}
	else
		qDebug("Unable to open data file: %s","sampledb1.kud");

	/** open the kugar template for the kugar viewer */
	QFile ff("sampledb1.kut");
	if (ff.open(IO_ReadOnly))
	{
		if (!rptviewer -> setReportTemplate(&ff))
			qDebug("Invalid data file: %s","sampledb1.kut");
		ff.close();
	}
	else
		qDebug("Unable to open data file: %s","sampledb1.kut");

	/** open the kugar data for the kugar viewer */
	QFile f("sampledb1.kud");
	if (f.open(IO_ReadOnly))
	{
		if (!rptviewer -> setReportData(&f))
			qDebug("Invalid data file: %1","sampledb1.kud");

		f.close();
	}
	else
		qDebug("Unable to open data file: %1","sampledb1.kud");

	rptviewer->renderReport();
	rptviewer->show();
	delete rptdata;
}
