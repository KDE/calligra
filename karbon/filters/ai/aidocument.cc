/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qregexp.h>
#include <qstring.h>
#include <qtextstream.h>

#include "aidocument.h"

#include <kdebug.h>


// TODO: look in "perl cookbook" for better regexp:
static inline bool isNumber( const QString& in )
{
	QRegExp reg( "^(?:\\d*\\.)?\\d+$" );	// TODO: fail safe?
	return in.find( reg, 0 ) != -1;
}

AiDocument::AiDocument()
{
	m_envStack.push( document );
}

void
AiDocument::parse( QTextStream& s, const QString& in )
{
	s <<
		"<?xml version=\"0.1\" encoding=\"UTF-8\"?>\n"
		"<!DOCTYPE DOC>\n"
		"<DOC mime=\"application/x-karbon\" version=\"0.1\" "
			"editor=\"Karbon AI Import Filter\" >\n"
		"<LAYER visible=\"1\" >\n"
	<< endl;

	QStringList tokens = QStringList::split( QRegExp( "\\s+" ), in );

	QStringList::const_iterator itr = tokens.begin();
	for( ; itr != tokens.end(); ++itr )
	{
		// choose environment:

		// document ----->
		if( m_envStack.top() == document )
		{
			if( (*itr).startsWith( "%!PS-Adobe-" ) )
			{
				m_envStack.push( head );
				--itr;
				continue;
			}
		}
		// <----- document

		// head ----->
		else if( m_envStack.top() == head )
		{
			if( *itr == "%%EndProlog" )
			{
				m_envStack.pop();
				m_envStack.push( body );
				continue;
			}
		}
		// <----- head

		// body ---->
		else if( m_envStack.top() == body )
		{
			if( m_stringStack.count() == 1  )
			{
				if( m_numberStack.count() == 2 )
				{
					// a moveto starts a path:
					if(
						*( m_stringStack.top() ) == "m" ||
						*( m_stringStack.top() ) == "M" )
					{
						m_envStack.push( path );
						--itr;
						continue;
					}
				}

				m_stringStack.clear();
				m_numberStack.clear();
			}

			if( (*itr).startsWith( "%AI" ) )
			{
				m_envStack.push( definition );
				--itr;
				continue;
			}
			else if( *itr == "%%EOF" )
			{
				m_envStack.pop();
				continue;
			}
			else if( isNumber( *itr ) )
			{
				m_stringStack.clear();
				m_envStack.push( number );
				--itr;
				continue;
			}
			else
			{
				m_envStack.push( string );
				--itr;
				continue;
			}
		}
		// <----- body

		// path ----->
		else if( m_envStack.top() == path )
		{
			if(
				m_numberStack.count() == 0 &&
				m_stringStack.count() == 1 )
			{
				m_stringStack.clear();
				m_numberStack.clear();
			}
			else if(
				m_numberStack.count() == 1 &&
				m_stringStack.count() == 1 )
			{
				m_stringStack.clear();
				m_numberStack.clear();
			}
			else if(
				m_numberStack.count() == 2 &&
				m_stringStack.count() == 1  )
			{
				if(
					*( m_stringStack.top() ) == "m" ||
					*( m_stringStack.top() ) == "M" )
				{
					QString val[2];
					val[1] = *( m_numberStack.pop() );
					val[0] = *( m_numberStack.pop() );
					moveTo( s, val[0], val[1] );
				}
				else if(
					*( m_stringStack.top() ) == "l" ||
					*( m_stringStack.top() ) == "L" )
				{
					QString val[2];
					val[1] = *( m_numberStack.pop() );
					val[0] = *( m_numberStack.pop() );
					lineTo( s, val[0], val[1] );
				}

				m_stringStack.clear();
				m_numberStack.clear();
			}
			else if(
				m_numberStack.count() == 4 &&
				m_stringStack.count() == 1  )
			{
				m_stringStack.clear();
				m_numberStack.clear();
			}
			else if(
				m_numberStack.count() == 6 &&
				m_stringStack.count() == 1  )
			{
				m_stringStack.clear();
				m_numberStack.clear();
			}

			if( isNumber( *itr ) )
			{
				m_stringStack.clear();
				m_envStack.push( number );
				--itr;
				continue;
			}
			else
			{
				m_envStack.push( string );
				--itr;
				continue;
			}
		}
		else if( m_envStack.top() == definition )
		{
			if( (*itr).startsWith( "%AI" ) )
			{
				m_envStack.pop();
				continue;
			}
		}
		else if( m_envStack.top() == number )
		{
			m_numberStack.push( &(*itr) );
			m_envStack.pop();
			continue;
		}
		else if( m_envStack.top() == string )
		{
			m_stringStack.push( &(*itr) );
			m_envStack.pop();
			continue;
		}
	}
	// <----- path

	s <<
		"</LAYER>\n"
		"</DOC>\n"
	<< endl;
}


void
AiDocument::moveTo( QTextStream& s, const QString& x, const QString& y )
{
	s << "<MOVE x=\"" + x + "\" y=\"" + y + "\" />" << endl;
}

void
AiDocument::curveTo( QTextStream& s,
	const QString& x1, const QString& y1,
	const QString& x2, const QString& y2,
	const QString& x3, const QString& y3 )
{
	s << "<CURVE "
		"x1=\"" + x1 + "\" y1=\"" + y1 + "\" "
		"x2=\"" + x2 + "\" y2=\"" + y2 + "\" "
		"x3=\"" + x3 + "\" y3=\"" + y3 + "\" "
	"/>" << endl;
}

void
AiDocument::curve1To( QTextStream& s,
	const QString& x2, const QString& y2,
	const QString& x3, const QString& y3 )
{
	s << "<CURVE1 "
		"x2=\"" + x2 + "\" y2=\"" + y2 + "\" "
		"x3=\"" + x3 + "\" y3=\"" + y3 + "\" "
	"/>" << endl;
}

void
AiDocument::curve2To( QTextStream& s,
	const QString& x1, const QString& y1,
	const QString& x3, const QString& y3 )
{
	s << "<CURVE2 "
		"x1=\"" + x1 + "\" y1=\"" + y1 + "\" "
		"x3=\"" + x3 + "\" y3=\"" + y3 + "\" "
	"/>" << endl;
}

void
AiDocument::lineTo( QTextStream& s, const QString& x, const QString& y )
{
	s << "<LINE x=\"" + x + "\" y=\"" + y + "\" />" << endl;
}

