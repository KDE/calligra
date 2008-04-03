//
// C++ Interface: krscriptconstants
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KRSCRIPTCONSTANTS_H
#define KRSCRIPTCONSTANTS_H

#include <QObject>

/**
	@author Adam Pigg <adam@piggz.co.uk>
*/
class KRScriptConstants : public QObject
{
		Q_OBJECT
	public:
		KRScriptConstants ( QObject *parent = 0 );

		~KRScriptConstants();

		Q_ENUMS ( PenStyle );
		
		enum PenStyle{QtNoPen = 0, QtSolidLine, QtDashLine, QtDotLine, QtDashDotLine, QtDashDotDotLine};
};

#endif
