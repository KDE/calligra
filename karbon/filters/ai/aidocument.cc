/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qregexp.h>
#include <qstring.h>
#include <qtextstream.h>

#include "aidocument.h"

#include <kdebug.h>


static inline bool isNumber( const QString& in )
{
	QRegExp reg( "^-?\\d*\\.?\\d+$" );	// TODO: fail safe?
	return in.find( reg, 0 ) != -1;
}

AiDocument::AiDocument()
{
	m_envStack.push( document );
}


// this function is written with performance in mind.
// it's not very nice to look at.

void
AiDocument::parse( QTextStream& s, const QString& in )
{
	s <<
		"<?xml version=\"0.1\" encoding=\"UTF-8\"?>\n"
		"<!DOCTYPE DOC>\n"
		"<DOC mime=\"application/x-karbon\" version=\"0.1\" "
			"editor=\"Karbon AI Import Filter\" >"
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
			}
		}
		// <----- head

		// body ---->
		else if( m_envStack.top() == body )
		{
			if( *itr == "%%BeginSetup" )
			{
				m_stringStack.clear();
				m_numberStack.clear();
				m_envStack.push( setup );
			}
			else if( *itr == "%%EOF" )
			{
				m_stringStack.clear();
				m_numberStack.clear();
				m_envStack.pop();
			}
			else if( *itr == "%AI5_BeginLayer" )
			{
				layerBegin( s );
				m_stringStack.clear();
				m_numberStack.clear();
				m_envStack.push( layer );
			}
		}
		// <----- body

		// setup ----->
		else if( m_envStack.top() == setup )
		{
			if( *itr == "%%EndSetup" )
				m_envStack.pop();
		}
		// <----- setup

		// layer ----->
		else if( m_envStack.top() == layer )
		{
			if( (*itr).startsWith( "%AI5_EndLayer" ) )
			{
				layerEnd( s );
				m_stringStack.clear();
				m_numberStack.clear();
				m_envStack.pop();
			}

			if( m_stringStack.count() == 1  )
			{
				if( m_numberStack.count() == 2 )
				{
					// a moveto starts a path:
					if(
						*( m_stringStack.top() ) == "m" ||
						*( m_stringStack.top() ) == "M" )
					{
						pathBegin( s );
						QString val[2];
						val[1] = *( m_numberStack.pop() );
						val[0] = *( m_numberStack.pop() );
						moveTo( s, val[0], val[1] );
						m_stringStack.clear();
						m_numberStack.clear();
						m_envStack.push( path );
					}
				}
// TODO: for now:
m_stringStack.clear();
m_numberStack.clear();
			}

			if( isNumber( *itr ) )
			{
				m_stringStack.clear();
				m_envStack.push( number );
			}
			else
				m_envStack.push( string );
		}
		// <----- layer

		// path ----->
		else if( m_envStack.top() == path )
		{
			if( (*itr).startsWith( "%AI5_EndLayer" ) )
			{
				pathEnd( s );
				m_stringStack.clear();
				m_numberStack.clear();
				m_envStack.pop();
				--itr;		// pass it through to layer.
				continue;
			}

//kdDebug() << *itr << "\t" << m_stringStack.count() << "\t" << m_numberStack.count() << endl;

			if( m_stringStack.count() == 1 )
			{
				if( m_numberStack.count() == 0 )
				{
					if(
						*( m_stringStack.top() ) == "N" ||
						*( m_stringStack.top() ) == "n" ||
						*( m_stringStack.top() ) == "F" ||
						*( m_stringStack.top() ) == "f" ||
						*( m_stringStack.top() ) == "S" ||
						*( m_stringStack.top() ) == "s" ||
						*( m_stringStack.top() ) == "B" ||
						*( m_stringStack.top() ) == "b" ||
						*( m_stringStack.top() ) == "N" ||
						*( m_stringStack.top() ) == "n" )
					{
						m_stringStack.clear();
						m_numberStack.clear();
					}
				}
				else if( m_numberStack.count() == 1 )
				{
					if(
						*( m_stringStack.top() ) == "d" ||
						*( m_stringStack.top() ) == "i" ||
						*( m_stringStack.top() ) == "D" ||
						*( m_stringStack.top() ) == "j" ||
						*( m_stringStack.top() ) == "J" ||
						*( m_stringStack.top() ) == "M" ||
						*( m_stringStack.top() ) == "w" )
					{
						m_stringStack.clear();
						m_numberStack.clear();
					}
				}
				else if( m_numberStack.count() == 2 )
				{
					if(
						*( m_stringStack.top() ) == "m" ||
						*( m_stringStack.top() ) == "M" )
					{
						// moveto of next path is end of this:
						pathEnd( s );

						pathBegin( s );
						QString val[2];
						val[1] = *( m_numberStack.pop() );
						val[0] = *( m_numberStack.pop() );
						moveTo( s, val[0], val[1] );
						m_stringStack.clear();
						m_numberStack.clear();
					}
					else if(
						*( m_stringStack.top() ) == "l" ||
						*( m_stringStack.top() ) == "L" )
					{
						QString val[2];
						val[1] = *( m_numberStack.pop() );
						val[0] = *( m_numberStack.pop() );
						lineTo( s, val[0], val[1] );
						m_stringStack.clear();
						m_numberStack.clear();
					}
				}
				else if( m_numberStack.count() == 4 )
				{
					if(
						*( m_stringStack.top() ) == "v" ||
						*( m_stringStack.top() ) == "V" )
					{
						QString val[4];
						val[3] = *( m_numberStack.pop() );
						val[2] = *( m_numberStack.pop() );
						val[1] = *( m_numberStack.pop() );
						val[0] = *( m_numberStack.pop() );
						curve1To( s, val[0], val[1], val[2], val[3] );
						m_stringStack.clear();
						m_numberStack.clear();
					}
					else if(
						*( m_stringStack.top() ) == "y" ||
						*( m_stringStack.top() ) == "Y" )
					{
						QString val[4];
						val[3] = *( m_numberStack.pop() );
						val[2] = *( m_numberStack.pop() );
						val[1] = *( m_numberStack.pop() );
						val[0] = *( m_numberStack.pop() );
						curve2To( s, val[0], val[1], val[2], val[3] );
						m_stringStack.clear();
						m_numberStack.clear();
					}
				}
				else if( m_numberStack.count() == 6 )
				{
					if(
						*( m_stringStack.top() ) == "c" ||
						*( m_stringStack.top() ) == "C" )
					{
						QString val[6];
						val[5] = *( m_numberStack.pop() );
						val[4] = *( m_numberStack.pop() );
						val[3] = *( m_numberStack.pop() );
						val[2] = *( m_numberStack.pop() );
						val[1] = *( m_numberStack.pop() );
						val[0] = *( m_numberStack.pop() );
						curveTo( s,
							val[0], val[1],
							val[2], val[3],
							val[4], val[5] );
						m_stringStack.clear();
						m_numberStack.clear();
					}
				}
			}

			if( isNumber( *itr ) )
			{
				m_stringStack.clear();
				m_envStack.push( number );
			}
			else
				m_envStack.push( string );
		}
		// <----- path

		// definition ----->
		else if( m_envStack.top() == definition )
		{
			if( (*itr).startsWith( "%AI" ) )
				m_envStack.pop();
		}
		// <----- definition


		// missing "else" is no mistake!
		// number ----->
		if( m_envStack.top() == number )
		{
			m_numberStack.push( &(*itr) );
			m_envStack.pop();
		}
		// <----- number

		// string ----->
		else if( m_envStack.top() == string )
		{
			m_stringStack.push( &(*itr) );
			m_envStack.pop();
		}
		// <----- string

	}
	s <<
		"</DOC>\n"
	<< endl;
}


void
AiDocument::layerBegin( QTextStream& s )
{
	s << "<LAYER visible=\"1\">" << endl;
}

void
AiDocument::layerEnd( QTextStream& s )
{
	s << "</LAYER>" << endl;
}

void
AiDocument::pathBegin( QTextStream& s )
{
	s <<
		"<PATH>\n"
		"<SEGMENTS>"
	<< endl;
}

void
AiDocument::pathEnd( QTextStream& s )
{
	s <<
		"</SEGMENTS>\n"
		"</PATH>"
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

