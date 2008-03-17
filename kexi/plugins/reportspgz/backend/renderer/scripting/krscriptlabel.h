//
// C++ Interface: krscriptlabel
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KRSCRIPTLABEL_H
#define KRSCRIPTLABEL_H

#include <QObject>
#include <krlabeldata.h>

/**
	@author Adam Pigg <adam@piggz.co.uk>
*/
namespace Scripting
{
	class Label : public QObject
	{
		Q_OBJECT
		public:
			Label ( KRLabelData * );

			~Label();

		public slots:
			QString caption();
			void setCaption(const QString&);

			int horizontalAlignment();
			void setHorizonalAlignment(int);
		
			int verticalAlignment();
			void setVerticalAlignment(int);
		
			QColor backgroundColor();
			void setBackgroundColor(QColor);
		
			QColor foregroundColor();
			void setForegroundColor(QColor);
		
			int backgroundOpacity();
			void setBackgroundOpacity(int);
		
			QColor lineColor();
			void setLineColor(QColor);
			
			int lineWeight();
			void setLineWeight(int);
			
			int lineStyle();
			void setLineStyle(int);
			
			QPointF position();
			void setPosition(QPointF);
			
			QSizeF size();
			void setSize(QSizeF);
					
		private:
			KRLabelData *_label;
	};
}

#endif
