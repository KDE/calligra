//
// C++ Interface: krscriptsection
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KRSCRIPTSECTION_H
#define KRSCRIPTSECTION_H

#include <QObject>
#include <krsectiondata.h>

/**
	@author Adam Pigg <adam@piggz.co.uk>
*/
namespace Scripting
{
	class Section : public QObject
	{
		Q_OBJECT
		public:
			Section ( KRSectionData* );

			~Section();

		public slots:
			/**Returns the background color of the section*/
			QColor backgroundColor();
		/**Sets the background color of the section to the given color 
			name, in the style #rrggbb*/
			void   setBackgroundColor(const QString&);
		
			/**Returns the section height as a real number, in points*/
			qreal height();
			/**Sets the section height to the given value in points*/
			void setHeight(qreal);
				
			/**Returns the name of the section*/
			QString name();
		
			/**Returns an object in the section, by number*/
			QObject* objectByNumber(int);
			/**Returns an object in the section, by name*/
			QObject* objectByName(const QString&);
		
		private:
			KRSectionData *_section;
	};
}
#endif
