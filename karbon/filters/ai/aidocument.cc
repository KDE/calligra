/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qregexp.h>
#include <qstring.h>
#include <qtextstream.h>

#include "aidocument.h"

#include <kdebug.h>


// TODO: look in "perl cookbook" for better regexp:
static const QString regFloat =
"(?:"
	"\\d*\\.\\d+"
")";

// path attributes ----->
static const QString regWindingOrder =
"(?:"
	"(?:0|1)" "\\s+" "D"
")";
static const QString regLineJoin =
"(?:"
	"(?:0|1|2)" "\\s+" "j"
")";
static const QString regLineCap =
"(?:" 
	"(?:0|1|2)" "\\s+" "J"
")";
static const QString regMiterLimit =
"(?:" +
	regFloat + "\\s+" "M"
")";
static const QString regLineWidth =
"(?:" +
	regFloat + "\\s+" "w"
")";

static const QString regPathAttributes =
"(?:" +
	regWindingOrder	+ "|" +
	regLineJoin		+ "|" +
	regLineCap		+ "|" +
	regMiterLimit	+ "|" +
	regLineWidth	+ 
")";
// <----- path attributes


// path construction ----->
static const QString regMoveTo =
"(?:" +
	regFloat + "\\s+" + regFloat + "\\s+"
	"m"
")";
static const QString regLineTo =
"(?:" +
	regFloat + "\\s+" + regFloat + "\\s+"
	"[lL]"
")";
static const QString regCurveTo =
"(?:" +
	regFloat + "\\s+" + regFloat + "\\s+" +
	regFloat + "\\s+" + regFloat + "\\s+" +
	regFloat + "\\s+" + regFloat + "\\s+"
	"[cC]"
")";
static const QString regCurve1To =
"(?:" +
	regFloat + "\\s+" + regFloat + "\\s+" +
	regFloat + "\\s+" + regFloat + "\\s+"
	"[vV]"
")";
static const QString regCurve2To =
"(?:" +
	regFloat + "\\s+" + regFloat + "\\s+" +
	regFloat + "\\s+" + regFloat + "\\s+"
	"[yY]"
")";

static const QString regPathConstructors =
"(?:" +
	regMoveTo	+ "|" +
	regLineTo	+ "|" +
	regCurveTo	+ "|" +
	regCurve1To	+ "|" +
	regCurve2To	+ 
")";
// <----- path construction

// path renderer ----->
static const QString regRendeCloseNone			= "N";
static const QString regRenderNone				= "n";
static const QString regRenderFill				= "F";
static const QString regRenderCloseFill			= "f";
static const QString regRenderStroke			= "S";
static const QString regRenderCloseStroke		= "s";
static const QString regRenderFillStroke		= "B";
static const QString regRenderCloseFillStroke	= "b";

static const QString regPathRenderer =
"(?:" +
	regRendeCloseNone			+ "|" +
	regRenderNone				+ "|" +
	regRenderFill				+ "|" +
	regRenderCloseFill			+ "|" +
	regRenderStroke				+ "|" +
	regRenderCloseStroke		+ "|" +
	regRenderFillStroke			+ "|" +
	regRenderCloseFillStroke	+ 
")";
// <----- path renderer

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
	QRegExp reg("");
	reg.setMinimal( true );

	int pos = 0;
	while ( pos >= 0 )
	{
		pos = reg.search( in, pos );
		if ( pos > -1 )
		{
			pos += reg.matchedLength();
			parsePath( s, reg.cap( 1 ) );
		}
	}
}

void
AiDocument::parsePath( QTextStream& s, const QString& in )
{
	kdDebug() << "path ***" << in << "***" << endl;
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

