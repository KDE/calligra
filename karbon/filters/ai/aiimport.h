/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __AIIMPORT_H__
#define __AIIMPORT_H__

#include <qobject.h>

#include <koFilter.h>

class QDomElement;
class QTextStream;

class AiDocument;

class AiImport : public KoFilter
{
	Q_OBJECT

public:
	AiImport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~AiImport();

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

private:
	AiDocument* m_aiDocument;
};

#endif

