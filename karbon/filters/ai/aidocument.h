/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __AIDOCUMENT_H__
#define __AIDOCUMENT_H__


class AiDocument;
extern AiDocument* aiDocument;

class QString;
class QTextStream;

class AiDocument
{
public:
	AiDocument();
	virtual ~AiDocument() {}

	void parse( QTextStream& s, const QString& in );

private:
	void parseHead( QTextStream& s, const QString& in );
	void parseBody( QTextStream& s, const QString& in );

	// head:
	void headCreator( const QString& in );
	void headAuthor( const QString& in );

	// path operators:
	void lineTo( QTextStream& s, double x, double y );
};

#endif

