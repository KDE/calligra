/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qregexp.h>
#include <qstring.h>
#include <qtextstream.h>

#include "aidocument.h"

#include <kdebug.h>


AiDocument::AiDocument()
{
}

void
AiDocument::parse( QTextStream& s, const QString& in )
{
	s <<
		"<?xml version=\"0.1\" encoding=\"UTF-8\"?>\n"
		"<!DOCTYPE DOC>\n"
		"<DOC mime=\"application/x-karbon\" version=\"0.1\" "
			"editor=\"Karbon AI Import Filter\" >\n"
		"  <LAYER visible=\"1\" >\n"
	<< endl;

	QString head;
	QString body;

	QRegExp reg( "^.*(%!PS-Adobe-.*%%EndProlog)(.*%%EOF)" );
	if( reg.search( in ) != -1 )
	{
		head = reg.cap( 1 );
		body = reg.cap( 2 );
	}

	parseHead( s, head );
	parseBody( s, body );

	s <<
		" </LAYER>\n"
		"</DOC>\n"
	<< endl;
}

void
AiDocument::parseHead( QTextStream& s, const QString& in )
{
	QRegExp reg( "%%Creator:([^\n]+)" );
	if( reg.search( in ) != -1 )
		headCreator( reg.cap( 1 ).stripWhiteSpace() );

	reg.setPattern( "%%For:([^\n]+)" );
	if( reg.search( in ) != -1 )
		headAuthor( reg.cap( 1 ).stripWhiteSpace() );
}

void
AiDocument::parseBody( QTextStream& s, const QString& in )
{
	QRegExp reg( "%%Creator:([^\n]+)" );
	if( reg.search( in ) != -1 )
		headCreator( reg.cap( 1 ).stripWhiteSpace() );
}

void
AiDocument::lineTo( QTextStream& s, double x, double y )
{
	kdDebug() << "***lineto " << x << " " << y << endl;
}

void
AiDocument::headCreator( const QString& in )
{
	kdDebug() << "***\"" << in << "\"" << endl;
}

void
AiDocument::headAuthor( const QString& in )
{
	kdDebug() << "***\"" << in << "\"" << endl;
}

