/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
  
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qdir.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qdom.h>

#include <klocale.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kinstance.h>
#include <kdebug.h>

#include "karbon_factory.h"
#include "karbon_resourceserver.h"
#include "vgradient.h"
#include "vgradienttabwidget.h"
#include "vobject.h"
#include "vcomposite.h"
#include "vgroup.h"

KarbonResourceServer::KarbonResourceServer()
{
	kdDebug() << "-- Karbon ResourceServer --" << endl;
		
	// PATTERNS  
	kdDebug() << "Loading patterns:" << endl;
	m_patterns.setAutoDelete(true);

	// image formats
	QStringList formats;
	formats << "*.png" << "*.tif" << "*.xpm" << "*.bmp" << "*.jpg" << "*.gif";

	// init vars
	QStringList lst;
	QString format, file;

	// find patterns
	for( QStringList::Iterator it = formats.begin(); it != formats.end(); ++it )
	{
		format = *it;
		QStringList l = KarbonFactory::instance()->dirs()->findAllResources("kis_pattern", format, false, true);
		lst += l;
	}
	// load patterns
	for( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
	{
		file = *it;
		kdDebug() << " - " << file << endl;
		loadPattern( file );
	}
	kdDebug() << m_patterns.count() << " patterns loaded." << endl;

		// GRADIENTS
	kdDebug() << "Loading gradients:" << endl;
	m_gradients = new QPtrList<VGradientListItem>();
	m_gradients->setAutoDelete( true );

	formats.clear();
	lst.clear();
	formats << "*.kgr";
	
	// find Gradients
	for( QStringList::Iterator it = formats.begin(); it != formats.end(); ++it )
	{
		format = *it;
		QStringList l = KarbonFactory::instance()->dirs()->findAllResources("karbon_gradient", format, false, true);
		lst += l;
	}
	// load Gradients
	for( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
	{
		file = *it;
		kdDebug() << " - " << file << endl;
		loadGradient( file );
	}
	kdDebug() << m_gradients->count() << " gradients loaded." << endl;
	
	// CLIPARTS
	kdDebug() << "Loading cliparts:" << endl;
	m_cliparts = new QPtrList<VClipartIconItem>();
	m_cliparts->setAutoDelete( true );

	formats.clear();
	lst.clear();
	formats << "*.kclp";
	
	// find cliparts
	for( QStringList::Iterator it = formats.begin(); it != formats.end(); ++it )
	{
		format = *it;
		QStringList l = KarbonFactory::instance()->dirs()->findAllResources("karbon_clipart", format, false, true);
		lst += l;
	}
	// load cliparts
	for( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
	{
		file = *it;
		kdDebug() << " - " << file << endl;
		loadClipart( file );
	}
	kdDebug() << m_cliparts->count() << " cliparts loaded." << endl;
} // KarbonResourceServer::KarbonResourceServer

KarbonResourceServer::~KarbonResourceServer()
{
	m_patterns.clear();
	m_gradients->clear();
	delete m_gradients;
	m_cliparts->clear();
	delete m_cliparts;
} // KarbonResourceServer::~KarbonResourceServer

const VPattern *KarbonResourceServer::loadPattern( const QString& filename )
{
	VPattern *pattern = new VPattern( filename );

	if( pattern->isValid() )
		m_patterns.append( pattern );
	else
	{
		delete pattern;
		pattern = 0L;
	}

	return pattern;
}

VGradientListItem* KarbonResourceServer::addGradient( VGradient* gradient )
{
	int i = 1;
	char buffer[20];
	QFileInfo fi;
	
	sprintf( buffer, "%04d.kgr", i++ );
	fi.setFile( KarbonFactory::instance()->dirs()->saveLocation( "karbon_gradient" ) + buffer  );
	while ( fi.exists() == true )
	{
		sprintf( buffer, "%04d.kgr", i++ );
		fi.setFile( KarbonFactory::instance()->dirs()->saveLocation( "karbon_gradient" ) + buffer );
		kdDebug() << fi.fileName() << endl;
	}

	QString filename = KarbonFactory::instance()->dirs()->saveLocation( "karbon_gradient" ) + buffer;

	saveGradient( gradient, filename );

	m_gradients->append( new VGradientListItem( *gradient, filename ) );

	return m_gradients->last();
} // KarbonResourceServer::addGradient

void KarbonResourceServer::removeGradient( VGradientListItem* gradient )
{
	QFile file( gradient->filename() );
	if ( file.remove() )
		m_gradients->remove( gradient );
} // KarbonResourceServer::removeGradient

void KarbonResourceServer::loadGradient( const QString& filename )
{
	VGradient gradient;
	
	QFile f( filename );
	if ( f.open( IO_ReadOnly ) )
	{
		QDomDocument doc;
		if ( !( doc.setContent( &f ) ) )
			f.close();
		else
		{
			QDomElement e;
			QDomNode n = doc.documentElement().firstChild();
			if ( !n.isNull() )
			{
				e = n.toElement();
				if ( !e.isNull() )
					if ( e.tagName() == "GRADIENT" )
						gradient.load( e );
			}
		}
	}

	if( gradient.colorStops().count() > 1 )
		m_gradients->append( new VGradientListItem( gradient, filename ) );
} // KarbonResourceServer::loadGradient

void KarbonResourceServer::saveGradient( VGradient* gradient, const QString& filename )
{
	QFile file( filename );
	QDomDocument doc;
	QDomElement me = doc.createElement( "PREDEFGRADIENT" );
	doc.appendChild( me );
	gradient->save( me );
	if (!(file.open(IO_WriteOnly)))
		return;
	QTextStream ts(&file);
	doc.save(ts, 2);
	file.flush();
	file.close();
} // KarbonResourceServer::saveGradient

VClipartIconItem* KarbonResourceServer::addClipart( VObject* clipart )
{
	int i = 1;
	char buffer[20];
	sprintf( buffer, "%04d.kclp", i++ );
	while ( KStandardDirs::exists( KarbonFactory::instance()->dirs()->saveLocation( "karbon_clipart" ) + buffer  ) )
		sprintf( buffer, "%04d.kclp", i++ );

	QString filename = KarbonFactory::instance()->dirs()->saveLocation( "karbon_clipart" ) + buffer;

	saveClipart( clipart, filename );

	m_cliparts->append( new VClipartIconItem( clipart, filename ) );
	
	return m_cliparts->last();
} // KarbonResourceServer::addClipart

void KarbonResourceServer::removeClipart( VClipartIconItem* clipart )
{
	QFile file( clipart->filename() );
	if ( file.remove() )
		m_cliparts->remove( clipart );
} // KarbonResourceServer::removeClipart

void KarbonResourceServer::loadClipart( const QString& filename )
{
	VObject* clipart = 0L;
	
	QFile f( filename );
	if ( f.open( IO_ReadOnly ) )
	{
		QDomDocument doc;
		if ( !( doc.setContent( &f ) ) )
			f.close();
		else
		{
			QDomElement e;
			QDomNode n = doc.documentElement().firstChild();
			if ( !n.isNull() )
			{
				e = n.toElement();
				if ( !e.isNull() )
				{
					/*if ( e.tagName() == "TEXT" )
					{
						clipart = new VText(); 
						clipart->load( e );
					}
					else*/ if (e.tagName() == "COMPOSITE" )
						clipart = new VComposite( 0L ); 
					else if (e.tagName() == "GROUP" )
						clipart = new VGroup( 0L ); 
					if ( clipart )
						clipart->load( e );
				}
			}
		}
	}

	if ( clipart )
		m_cliparts->append( new VClipartIconItem( clipart, filename ) );

	delete clipart;
} // KarbonResourceServer::loadClipart

void KarbonResourceServer::saveClipart( VObject* clipart, const QString& filename )
{
	QFile file( filename );
	QDomDocument doc;
	QDomElement me = doc.createElement( "PREDEFCLIPART" );
	doc.appendChild( me );
	clipart->save( me );
	if (!(file.open(IO_WriteOnly)))
		return;
	QTextStream ts(&file);
	doc.save(ts, 2);
	file.flush();
	file.close();
} // KarbonResourceServer::saveClipart
