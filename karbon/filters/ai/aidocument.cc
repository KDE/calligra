/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qstring.h>
#include <qtextstream.h>

#include "aidocument.h"

#include <kdebug.h>


AiDocument* aiDocument;

AiDocument::AiDocument()
{
	aiDocument = this;
}

void
AiDocument::begin( QTextStream& s )
{
	s <<
		"<?xml version=\"0.1\" encoding=\"UTF-8\"?>\n"
		"<!DOCTYPE DOC>\n"
		"<DOC mime=\"application/x-karbon\" version=\"0.1\" "
			"editor=\"Karbon AI Import Filter\" >\n"
		"  <LAYER visible=\"1\" >\n"
	<< endl;
}

void
AiDocument::end( QTextStream& s )
{
	s <<
		" </LAYER>\n"
		"</DOC>\n"
	<< endl;
}

void
AiDocument::headerCreator( QString* in )
{
	if( in == 0L )
		return;

	kdDebug() << "\"" << *in << "\"" << endl;
	delete in;
}

void
AiDocument::headerAuthor( QString* in )
{
	if( in == 0L )
		return;

	kdDebug() << "\"" << *in << "\"" << endl;
	delete in;
}

