#include <qapplication.h>
#include <qfile.h>

//#include "kugarview.h"
#include "form1.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );

	Form1 w;
    w.show();

	//KugarView* rptviewer = new KugarView();
	//QFile f("sample3.kud");
	//if (f.open(IO_ReadOnly))
	//{
	//	if (!rptviewer -> setReportData(&f))
	//		qDebug("Invalid data file: %1","sample1.kud");

	//	f.close();
	//}
	//else
	//	qDebug("Unable to open data file: %1","sample1.kud");

	//QFile ff("sample3.kut");
	//if (ff.open(IO_ReadOnly))
	//{
	//	if (!rptviewer -> setReportTemplate(&ff))
	//		qDebug("Invalid data file: %1","sample1.kud");

	//	ff.close();
	//}
	//else
	//	qDebug("Unable to open data file: %1","sample1.kud");

	//rptviewer->renderReport();
	//rptviewer->show();

	a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
