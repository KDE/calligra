// Copyright (c) 1999 Mutiny Bay Software                         
// Copyright (c) 2000 Phil Thompson <phil@river-bank.demon.co.uk>
//
// This file contains the definition of the interface to the KDE GUI wrapper
// around the viewer widget.


#ifndef _REPORT_H
#define _REPORT_H

#include <kmainwindow.h>
#include <qstring.h>

#include "kugar.h"


class Report : public KMainWindow
{
	Q_OBJECT
  
public:
	Report(); 
	~Report();

	void renderReport();
	void setReportTemplate(const QString &);
	void setReportData(const QString &);

	QSize sizeHint() const;

private slots:
	void slotFileQuit();
	void slotPrint();
	void slotViewToolBar();
	void slotViewStatusBar();
	void slotPreferedTemplate(const QString &);
  
private:
	MReportViewer *rptviewer;	// The viewer widget.
};

#endif 
