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

	void begin( QTextStream& s );
	void end( QTextStream& s );

	// header:
	void headerCreator( QString* in = 0L );
	void headerAuthor( QString* in = 0L );

private:
};

#endif

