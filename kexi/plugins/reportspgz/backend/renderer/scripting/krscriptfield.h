//
// C++ Interface: krscriptfield
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KRSCRIPTFIELD_H
#define KRSCRIPTFIELD_H

#include <QObject>
#include <krfielddata.h>

/**
	@author Adam Pigg <adam@piggz.co.uk>
*/
class KRScriptField : public QObject
{
		Q_OBJECT
	public:
		KRScriptField ( KRFieldData* );

		~KRScriptField();
		
	public slots:
		/**Returns the source (column) that the field gets its data from*/
		QString source();
		/**Sets the source (column) for the field*/
		void setSource(const QString&);
		
		void setBackgroundOpacity(int);
		
	private:
		KRFieldData *_field;

};

#endif
