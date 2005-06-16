/***************************************************************************
              kreportviewer.h  -  Kugar KDE report viewer widget
              -------------------
    begin     : Tue Jun 04 2002
    copyright : (C) 2002 Alexander Dymo
    email     : cloudtemple@mksat.net
 ***************************************************************************/

#ifndef KREPORTVIEWER_H
#define KREPORTVIEWER_H

#include "mreportviewer.h"

class KPrinter;

/**Kugar KDE report viewer widget
  *@author Alexander Dymo
  */

class KReportViewer : public MReportViewer
{
	Q_OBJECT

public:
	KReportViewer(QWidget *parent=0, const char *name=0):
		MReportViewer(parent, name) {}
        KReportViewer(MReportEngine* engine, QWidget *parent=0, const char *name=0):
		MReportViewer(engine,parent,name) {}

	void printReport();
	void printReport(KPrinter &printer);
	void setupPrinter(KPrinter &printer);
private:
};

#endif
