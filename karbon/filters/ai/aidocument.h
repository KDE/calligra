/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __AIDOCUMENT_H__
#define __AIDOCUMENT_H__

#include <qptrstack.h>		// value stacks
#include <qvaluestack.h>	// environment stack

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

protected:
	void moveTo( QTextStream& s, const QString& x, const QString& y );
	void curveTo( QTextStream& s,
		const QString& x1, const QString& y1,
		const QString& x2, const QString& y2,
		const QString& x3, const QString& y3 );
	void curve1To( QTextStream& s,
		const QString& x2, const QString& y2,
		const QString& x3, const QString& y3 );
	void curve2To( QTextStream& s,
		const QString& x1, const QString& y1,
		const QString& x3, const QString& y3 );
	void lineTo( QTextStream& s, const QString& x, const QString& y );

private:
	enum environment
	{
		document,
		head,
		body,
		path,
		definition,	// currently e.g. %AI3_BeginPattern etc

		number,
		string
	};

	// environment stack. keeps track of where we are during parsing.
	QValueStack<environment> m_envStack;

	// found values:
	QPtrStack<QString> m_numberStack;
	QPtrStack<QString> m_stringStack;
};
#endif

