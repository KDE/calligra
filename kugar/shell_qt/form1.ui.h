/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
#include "kugarqt.h"
#include <qfile.h>
#include <qmessagebox.h>

void Form1::init()
{
	MReportViewer* rptviewer = new MReportViewer(this);

	setCentralWidget(rptviewer);

	QFile f("sample3.kud");

	if (f.open(IO_ReadOnly))
	{
		if (!rptviewer -> setReportData(&f))
			QMessageBox::warning(this,"",tr("Invalid data file: %1").arg("sample1.kud"));

		f.close();
	}
	else
		QMessageBox::warning(this,"",tr("Unable to open data file: %1").arg("sample1.kud"));

	QFile ff("sample3.kut");

	if (ff.open(IO_ReadOnly))
	{
		if (!rptviewer -> setReportTemplate(&ff))
			QMessageBox::warning(this,"",tr("Invalid data file: %1").arg("sample1.kut"));

		ff.close();
	}
	else
		QMessageBox::warning(this,"",tr("Unable to open data file: %1").arg("sample1.kud"));

	rptviewer->renderReport();
	rptviewer->show();
}

void Form1::fileNew()
{

}

void Form1::fileOpen()
{

}

void Form1::fileSave()
{

}

void Form1::fileSaveAs()
{

}

void Form1::filePrint()
{

}

void Form1::fileExit()
{

}

void Form1::editUndo()
{

}

void Form1::editRedo()
{

}

void Form1::editCut()
{

}

void Form1::editCopy()
{

}

void Form1::editPaste()
{

}

void Form1::editFind()
{

}

void Form1::helpIndex()
{

}

void Form1::helpContents()
{

}

void Form1::helpAbout()
{

}

void Form1::destroy()
{

}
