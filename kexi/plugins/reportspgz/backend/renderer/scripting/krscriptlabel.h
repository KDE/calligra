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

			/**Gets/sets the horizontal alignment, -1 Left, 0 Center, +1 Right*/
			int horizontalAlignment();
			void setHorizonalAlignment(int);
		
			/**Gets/sets the vertical alignment, -1 Top, 0 Middle, +1 Bottom*/
			int verticalAlignment();
			void setVerticalAlignment(int);
		
			QColor backgroundColor();
			void setBackgroundColor(QString);
		
			QColor foregroundColor();
			void setForegroundColor(QString);
		
			int backgroundOpacity();
			void setBackgroundOpacity(int);
		
			QColor lineColor();
			void setLineColor(QString);
			
			int lineWeight();
			void setLineWeight(int);
			
			/**Gets/sets the line style.  Valid values are those from Qt::PenStyle (0-5)*/
			int lineStyle();
			void setLineStyle(int);
			
			QPointF position();
			void setPosition(qreal, qreal);
			
			QSizeF size();
			void setSize(qreal,qreal);
					
		private:
			KRLabelData *_label;
	};
}

#endif
