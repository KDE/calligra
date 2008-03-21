//
// C++ Interface: krscriptdebug
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KRSCRIPTDEBUG_H
#define KRSCRIPTDEBUG_H

#include <QObject>

/**
	@author Adam Pigg <adam@piggz.co.uk>
*/
class KRScriptDebug : public QObject
{
		Q_OBJECT
	public:
		KRScriptDebug ( QObject *parent = 0 );

		~KRScriptDebug();
		
	public slots:
		void print(const QString&);

};

#endif
