/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __EPSIMPORT_H__
#define __EPSIMPORT_H__

#include <qobject.h>

#include <koFilter.h>

class QDomElement;
class QTextStream;

class KProcess;

class EpsImport : public KoFilter
{
	Q_OBJECT

public:
	EpsImport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~EpsImport();

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

protected slots:
	void procOutput( KProcess* proc, char* buffer, int len );
	void procFailed();
	void procExited( KProcess* proc );

private:
	void stopProc();

	KProcess* m_proc;
};

#endif

