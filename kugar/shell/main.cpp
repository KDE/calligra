// Copyright (c) 1999 Mutiny Bay Software
// Copyright (c) 2000 Phil Thompson <phil@river-bank.demon.co.uk>
//
// The main module for the KDE Kugar shell.


#include <kapplication.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kcmdlineargs.h>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "report.h"


extern "C" int kdemain(int argc,char **argv)
{
	KAboutData about("kugar", I18N_NOOP("Kugar"), VERSION,
		I18N_NOOP("A template driven report viewer for XML data."),
		KAboutData::License_GPL_V2,
		"Copyright (c) 1999-2000 Mutiny Bay Software\n"
		"Copyright (c) 2000 Phil Thompson",
		I18N_NOOP("Kugar merges XML data files with XML templates\n"
		"to display and print high quality reports."),
		"http://www.thekompany.com/projects/kugar",
		"cloudtemple@mksat.net");
	about.addAuthor("Alexander Dymo", I18N_NOOP("Current maintainer, Report Designer, General Enhancements, Bugfixes"), "cloudtemple@mksat.net",
		"http://www.cloudtemple.mksat.net");
	about.addAuthor("Phil Thompson", I18N_NOOP("Former maintainer, Shell, Konqueror Part"),
		"phil@river-bank.demon.co.uk");
	about.addAuthor("Mutiny Bay Software", I18N_NOOP("Original author"), "info@mutinybaysoftware.com");

	// Handle the command line.

	static KCmdLineOptions options[] = {
		{"d <data>", I18N_NOOP("The XML data file"),0},
		{"r <reportURL>", I18N_NOOP("The XML template file URL"),0},
		KCmdLineLastOption
	};

	KCmdLineArgs::init(argc,argv,&about);
	KCmdLineArgs::addCmdLineOptions(options);

	KApplication a;

	Report *report = new Report();

	a.setMainWidget(report);

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	QCString opt;

	opt = args -> getOption("d");

	if (!opt.isNull())
		report->setReportData(opt);

	opt = args -> getOption("r");

	if (!opt.isNull())
	{
	    if (opt.find('/') == -1)
		opt.prepend("./");
		report->setReportTemplate(opt);
	}

	args -> clear();

	report->renderReport();
	report->show();

	return a.exec();
}
