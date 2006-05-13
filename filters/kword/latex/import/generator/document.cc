/* This file is part of the KDE project
 * Copyright (C) 2003 Robert JACOLIN <rjacolin@ifrance.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <qdom.h>
#include <QTextStream>
//Added by qt3to4:
#include <QByteArray>
#include <Q3PtrList>

#include "kdebug.h"
#include <KoStore.h>
#include "document.h"
#include "latex.h"
#include <param.h>
#include <command.h>

bool Document::analyse(Q3PtrList<Element>* root)
{
	bool error = true;
	error &= analyseDocumentClass((Command*) Latex::instance()->getCommand(root, "documentclass"));
	
	/* Analyse body */
	_body.analyse(Latex::instance()->getEnv(root, "document"));
	return true;
}

bool Document::analyseDocumentClass(Command* documentclass)
{
	kWarning(documentclass != NULL) << "no documentclass found !" << endl;
	Q3PtrList<Param> params = documentclass->getOptions();
	Param* param;
	for ( param = params.first(); param; param = params.next() )
	{
		if(param->getKey() == "a4paper")
		{
		}
		else if(param->getKey() == "11pt")
		{
		}
	}
}

bool Document::generate(KoStore* store)
{
	QDomDocument doc("KWORD");
	doc.appendChild(doc.createProcessingInstruction("xml", 
				"version=\"1.0\" encoding=\"UTF-8\""));
	
	/* DOC */
  QDomElement root = doc.createElement("DOC");
	root.setAttribute("editor", "LaTex Import Filter");
	root.setAttribute("mime", "application/x-kword");
	root.setAttribute("syntaxVersion", "1");
  doc.appendChild(root);
	
	/* PAPER */
	
	/* ATTRIBUTES */

	/* FRAMESETS */
	QDomElement body = doc.createElement("FRAMESETS");
	root.appendChild(body);

	/* generate body */
	_body.generate(body, doc);
	
	kDebug(30522) << "serialize" << endl;
	serialize(store, doc);
	return true;
}

void Document::serialize(KoStore* store, QDomDocument doc)
{
	QByteArray str = doc.toCString();
	qWarning(str);
	if(store->open("root"))
	{
		store->write((const char *)str, str.length());
		store->close();
	}
}
