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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
   This file use code from koTemplateOpenDia for the method chooseSlot.
*/

#include <kspreadlatexexportdiaImpl.h>

#include <QDir>
#include <QComboBox>
#include <q3buttongroup.h>
#include <QRadioButton>
#include <QCheckBox>

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
#include <KoFilterManager.h>
#include <kcombobox.h>
#include <knuminput.h>

#include "latexexportAdaptor.h"

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
		const char* name_, bool modal, Qt::WFlags fl )
    : LatexExportDia( parent, name_, modal, fl ), _in( in )
{
	int i = 0;

	kapp->restoreOverrideCursor();

	/* Recent files */
	_config = new KConfig("kspreadlatexexportdialog");
	_config->setGroup( "KSpread latex export filter" );
	QString value;
	while(i < 10)
	{
		/*value = _config->readPathEntry( QString("Recent%1").arg(i) );
		kDebug(30522) << "recent : " << value << endl;
		if(!value.isEmpty())
		{
			_recentList.append( value );
			recentBox->insertItem(value);
		}
		else
			i = 10;*/
		i = i + 1;
	}

	new LatexExportAdaptor(this);
    QDBus::sessionBus().registerObject("/filter/latex", this);

	
	/* All these items inserted must not be translated so they are inserted here 
	 * without i18n() method. */
	/*qualityComboBox->insertItem("final");
  qualityComboBox->insertItem("draft");*/

	classComboBox->addItem("article");
  classComboBox->addItem("book");
  classComboBox->addItem("letter");
  classComboBox->addItem("report");
  classComboBox->addItem("slides");

	encodingComboBox->addItem("unicode");
  encodingComboBox->addItem("ansinew");
  encodingComboBox->addItem("applemac");
  encodingComboBox->addItem("ascii");
  encodingComboBox->addItem("latin1");
  encodingComboBox->addItem("latin2");
  encodingComboBox->addItem("latin3");
  encodingComboBox->addItem("latin5");
  encodingComboBox->addItem("cp437");
  encodingComboBox->addItem("cp437de");
  encodingComboBox->addItem("cp850");
  encodingComboBox->addItem("cp852");
  encodingComboBox->addItem("cp865");
  encodingComboBox->addItem("cp1250");
  encodingComboBox->addItem("cp1252");
  encodingComboBox->addItem("decmulti");
  encodingComboBox->addItem("next");

	languagesList->insertItem("american");
	languagesList->insertItem("austrian");
	languagesList->insertItem("bahasa");
	languagesList->insertItem("brazil");
	languagesList->insertItem("breton");
	languagesList->insertItem("catalan");
	languagesList->insertItem("croatian");
	languagesList->insertItem("czech");
	languagesList->insertItem("danish");
	languagesList->insertItem("dutch");
	languagesList->insertItem("english");
	languagesList->insertItem("esperanto");
	languagesList->insertItem("finnish");
	languagesList->insertItem("francais");
	languagesList->insertItem("french");
	languagesList->insertItem("galician");
	languagesList->insertItem("german");
	languagesList->insertItem("germanb");
	languagesList->insertItem("hungarian");
	languagesList->insertItem("magyar");
	languagesList->insertItem("italian");
	languagesList->insertItem("norsk");
	languagesList->insertItem("nynorsk");
	languagesList->insertItem("polish");
	languagesList->insertItem("portuges");
	languagesList->insertItem("romanian");
	languagesList->insertItem("russian");
	languagesList->insertItem("spanish");
	languagesList->insertItem("slovak" );
	languagesList->insertItem("slovene");
	languagesList->insertItem("swedish");
	languagesList->insertItem("turkish");
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
	kDebug(30522) << "Export cancelled" << endl;
	QDialog::reject();
}

/**
 * Called when the user clic on the ok button. The xslt sheet is put on the recent list which is
 * saved, then export the document.
 */
void KSpreadLatexExportDiaImpl::accept()
{
	hide();
	kDebug(30522) << "KSPREAD LATEX EXPORT FILTER --> BEGIN" << endl;
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
	/* class names are not translated */
	config->setClass(classComboBox->currentText());

	if(qualityComboBox->currentItem() == 0)
		config->setQuality("final");
	else
		config->setQuality("draft");
	config->setDefaultFontSize(defaultFontSize->value());
	
	/* Pictures tab */
	if(pictureCheckBox->isChecked())
		config->convertPictures();
	config->setPicturesDir(pathPictures->url());
	
	/* Language tab */
	config->setEncoding(encodingComboBox->currentText());
	for(unsigned int index = 0; index < langUsedList->count(); index++)
	{
		kDebug(30522) << "lang. : " << langUsedList->item(index)->text() << endl;
		config->addLanguage(langUsedList->item(index)->text());
	}
	
	/* The default language is the first language in the list */
	if(langUsedList->item(0) != NULL)
		config->setDefaultLanguage(langUsedList->item(0)->text());
	if( !(langUsedList->currentText().isEmpty()) )
	{
		kDebug(30522) << "default lang. : " << langUsedList->currentText() << endl;
		config->setDefaultLanguage(langUsedList->currentText());
	}

	Document doc(_in, _fileOut);	
	kDebug(30522) << "---------- analyse file -------------" << endl;
	doc.analyse();
	kDebug(30522) << "---------- generate file -------------" << endl;
	doc.generate();
	kDebug(30522) << "KSPREAD LATEX EXPORT FILTER --> END" << endl;
}

void KSpreadLatexExportDiaImpl::addLanguage()
{
	kDebug(30522) << "add a new language supported" << languagesList->currentText() << endl;
	QString text = languagesList->currentText();
	languagesList->removeItem(languagesList->currentItem());
	langUsedList->insertItem(text);
}

void KSpreadLatexExportDiaImpl::removeLanguage()
{
	kDebug(30522) << "remove a lanugage" << langUsedList->currentText() << endl;
	QString text = langUsedList->currentText();
	langUsedList->removeItem(langUsedList->currentItem());
	languagesList->insertItem(text);
}

#include <kspreadlatexexportdiaImpl.moc>
