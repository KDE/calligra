/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Robert JACOLIN <rjacolin@ifrance.com>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
   This file use code from koTemplateOpenDia for the method chooseSlot.
*/

#include <kspreadlatexexportdiaImpl.h>

#include <qdir.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>

#include <kapplication.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <krecentdocument.h>
#include <ktempfile.h>
#include <kurlrequester.h>
#include <kfiledialog.h>
#include <kdebug.h>
#include <koFilterManager.h>
#include <kcombobox.h>
#include <knuminput.h>

#include <dcopclient.h>

#include "document.h"

/*#ifdef __FreeBSD__
#include <unistd.h>
#endif*/

/*
 *  Constructs a KSpreadLatexExportDiaImpl which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
KSpreadLatexExportDiaImpl::KSpreadLatexExportDiaImpl(KoStore* in, QWidget* parent, 
		const char* name_, bool modal, WFlags fl )
    : LatexExportDia( parent, name_, modal, fl ),
						DCOPObject("LatexFilterConfigDia"), _in( in )
{
	int i = 0;

	kapp->restoreOverrideCursor();

	/* Recent files */
	_config = new KConfig("kspreadlatexexportdialog");
	_config->setGroup( "KSpread latex export filter" );
	QString value;
	while(i < 10)
	{
		/*value = _config->readEntry( QString("Recent%1").arg(i) );
		kdDebug() << "recent : " << value << endl;
		if(!value.isEmpty())
		{
			_recentList.append( value );
			recentBox->insertItem(value);
		}
		else
			i = 10;*/
		i = i + 1;
	}

	/*if(!kapp->dcopClient()->isRegistered() )
	{
		kapp->dcopClient()->registerAs("FilterConfigDia");
		kapp->dcopClient()->setDefaultObject(objId());
	}*/
}

/*
 *  Destroys the object and frees any allocated resources
 */
KSpreadLatexExportDiaImpl::~KSpreadLatexExportDiaImpl()
{
    delete _config;
}

/**
 * Called when thecancel button is clicked.
 * Close the dialog box.
 */
void KSpreadLatexExportDiaImpl::reject()
{
	kdDebug() << "Export cancelled" << endl;
	QDialog::reject();
}

/**
 * Called when the user clic on the ok button. The xslt sheet is put on the recent list which is
 * saved, then export the document.
 */
void KSpreadLatexExportDiaImpl::accept()
{
	hide();
	kdDebug() << "KSPREAD LATEX EXPORT FILTER --> BEGIN" << endl;
	Config* config = Config::instance();
	/* Document tab */
	if(embededButton == typeGroup->selected())
		config->setEmbeded(true);
	else
		config->setEmbeded(false);
	if(kwordStyleButton == styleGroup->selected())
		config->useKwordStyle();
	else
		config-> useLatexStyle();
	config->setClass(classComboBox->currentText());
	config->setQuality(qualityComboBox->currentText());
	config->setDefaultFontSize(defaultFontSize->value());
	
	/* Pictures tab */
	if(pictureCheckBox->isChecked())
		config->convertPictures();
	//config->setPicturesDir(pathPictures.text());
	
	/* Language tab */
	Document doc(_in, _fileOut);	
	kdDebug() << "---------- analyse file -------------" << endl;
	doc.analyse();
	kdDebug() << "---------- generate file -------------" << endl;
	doc.generate();
	kdDebug() << "KSPREAD LATEX EXPORT FILTER --> END" << endl;
}

void KSpreadLatexExportDiaImpl::addLanguage()
{
	kdDebug() << "add a new language supported" << languagesList->currentText() << endl;
	QString text = languagesList->currentText();
	languagesList->removeItem(languagesList->currentItem());
	langUsedList->insertItem(text);
}

void KSpreadLatexExportDiaImpl::removeLanguage()
{
	kdDebug() << "remove a lanugage" << langUsedList->currentText() << endl;
	QString text = langUsedList->currentText();
	langUsedList->removeItem(langUsedList->currentItem());
	languagesList->insertItem(text);
}

#include <kspreadlatexexportdiaImpl.moc>
