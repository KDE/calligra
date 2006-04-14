/* This file is part of the KDE project
   Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "keximacrotextview.h"

#include <ktextedit.h>
#include <kdebug.h>

#include <kexidialogbase.h>
#include <kexidb/connection.h>

#include "lib/macro.h"

/**
* \internal d-pointer class to be more flexible on future extension of the
* functionality without to much risk to break the binary compatibility.
*/
class KexiMacroTextView::Private
{
	public:

		/**
		* The Editor used to display and edit the XML text.
		*/
		KTextEdit* editor;

};

KexiMacroTextView::KexiMacroTextView(KexiMainWindow *mainwin, QWidget *parent, ::KoMacro::Macro* const macro)
	: KexiMacroView(mainwin, parent, macro, "KexiMacroTextView")
	, d( new Private() )
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	d->editor = new KTextEdit(this);
	d->editor->setTextFormat(Qt::PlainText);
	layout->addWidget(d->editor);

	connect(d->editor, SIGNAL(textChanged()), this, SLOT(editorChanged()));
}

KexiMacroTextView::~KexiMacroTextView()
{
	delete d;
}

void KexiMacroTextView::editorChanged()
{
	setDirty(true);
}

bool KexiMacroTextView::loadData()
{
	QString data;
	if(! loadDataBlock(data)) {
		kexipluginsdbg << "KexiMacroTextView::loadData(): no DataBlock" << endl;
		return false;
	}

	kdDebug() << QString("KexiMacroTextView::loadData()\n%1").arg(data) << endl;
	disconnect(d->editor, SIGNAL(textChanged()), this, SLOT(editorChanged()));
	d->editor->setText(data);
	setDirty(false);
	connect(d->editor, SIGNAL(textChanged()), this, SLOT(editorChanged()));
	return true;
}

tristate KexiMacroTextView::storeData(bool /*dontAsk*/)
{
	kexipluginsdbg << "KexiMacroTextView::storeData(): " << parentDialog()->partItem()->name() << " [" << parentDialog()->id() << "]" << endl;
	return storeDataBlock( d->editor->text() );
}

#include "keximacrotextview.moc"

