/* This file is part of the KDE project
   Copyright (C) 2001 Robert JACOLIN <rjacolin@ifrance.com>

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
*/

/*
   This file is based on the file :
    koffice/filters/kword/html/htmlexportdia.cc
	Copyright (C) 2001 Nicolas Goutte <goutte@kde.org>

   which was based on the old file:
    /home/kde/koffice/filters/kspread/csv/csvfilterdia.cc

   The old file was copyrighted by
    Copyright (C) 1999 David Faure <faure@kde.org>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#include <kapplication.h>
#include <lateximportdia.h>
#include <lateximportdia.moc>
#include <dcopclient.h>
#include <ktempfile.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

#include "latexparser.h"
#include "config.h"

#include "generator/kwordgenerator.h"

LATEXImportDia::LATEXImportDia(KoStore* out, QWidget *parent) :
						KDialog(parent,i18n("Latex Import Filter Parameters"),
									KDialog::Ok|KDialog::Cancel),
						DCOPObject("FilterConfigDia"), _out(out)
{
	_out = out;
	kapp->restoreOverrideCursor();
	createDialog();
	if(!kapp->dcopClient()->isRegistered() )
	{
		kapp->dcopClient()->registerAs("FilterConfigDia");
		kapp->dcopClient()->setDefaultObject(objId());
	}
}

void LATEXImportDia::createDialog()
{
	resize(size());
	QWidget *page = new QWidget( this );
	setMainWidget(page);
	Q3BoxLayout *mainLayout = new Q3VBoxLayout(page, KDialog::marginHint(), KDialog::spacingHint());
	styleBox = new Q3VButtonGroup(i18n("Document Style"), page);
	mainLayout->addWidget(styleBox);

	/* First part */
	Q3BoxLayout *styleLayout = new Q3VBoxLayout(page);

	latexStyleRBtn = new QRadioButton(i18n("Latex style"), styleBox);
	styleLayout->addWidget(latexStyleRBtn);

	kwordStyleRBtn = new QRadioButton(i18n("KWord style"), styleBox);
	styleLayout->addWidget(kwordStyleRBtn);

	/*typeLabel  = new ...
	styleLayout->addWidget(typeLabel);
	typeCBox = new ...
	styleLayout->addWidget(typeCBox);
	*/
	styleBox->setExclusive(true);
	styleBox->setButton(0);			/* LATEX STYLE IS DEFAULT */
	styleLayout->activate();

	/* Second part */
	langBox = new Q3VButtonGroup(i18n("Language"), page);
	mainLayout->addWidget(langBox);

	Q3BoxLayout *langLayout = new Q3VBoxLayout(langBox);

	unicodeRBtn = new QRadioButton(i18n("Unicode"), langBox);
	langLayout->addWidget(unicodeRBtn);

	latin1RBtn = new QRadioButton(i18n("latin1"), langBox);
	langLayout->addWidget(latin1RBtn);

	/*babelCheckBox = new ...
	langLayout->addWidget(babelCheckBox);

	babelCBox = new ...
	langLayout->addWidget(babelCBox);
	*/
	langBox->setExclusive(true);
	langBox->setButton(1);			/* LATIN1 IS THE DEFAULT. */
	langLayout->activate();

	/* Third part */
	docBox = new Q3VButtonGroup(i18n("Document Type"), page);
	mainLayout->addWidget(docBox);

	Q3BoxLayout *docLayout = new Q3VBoxLayout(docBox);

	newDocRBtn = new QRadioButton(i18n("New document"), docBox);
	docLayout->addWidget(newDocRBtn);

	embededRBtn = new QRadioButton(i18n("Embedded document"), docBox);
	docLayout->addWidget(embededRBtn);

	docBox->setExclusive(true);
	docBox->setButton(0);			/* NEW DOC IS THE DEFAULT. */
	docLayout->activate();

	/* Display the main layout */
	mainLayout->addStretch(5);
	mainLayout->activate();
}

void LATEXImportDia::state()
{
	Config config;

	if(newDocRBtn == docBox->selected())
		config.setType(TYPE_DOC);
	else if(embededRBtn == docBox->selected())
		config.setType(TYPE_EMBEDED);

	if(unicodeRBtn == langBox->selected())
		config.setEncoding(ENC_UNICODE);
	else if(latin1RBtn == langBox->selected())
		config.setEncoding(ENC_LATIN1);

	/*if(latexStyleRBtn == styleBox->selected())
		config.result += "LATEX";
	else if(kwordStyleRBtn == styleBox->selected())
		result += "KWORD";*/
}

void LATEXImportDia::slotOk()
{
	hide();
	state();
	kDebug(30522) << "LATEX FILTER --> BEGIN" << endl;
	LatexParser parser(_fileIn);
	Q3PtrList<Element>* root = parser.parse();
	kDebug(30522) << "---------- generate file -------------" << endl;
	kDebug(30522) << "command: " << root->count() << endl;
	Element* elt;
	for(elt = root->first(); elt; elt = root->next())
	{
		elt->print();
	}
	KwordGenerator generator(root);
	generator.convert(_out);
	kDebug(30522) << "LATEX FILTER --> END" << endl;
	reject();
}
