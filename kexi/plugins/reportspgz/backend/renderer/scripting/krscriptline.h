//
// C++ Interface: krscriptline
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCRIPTINGKRSCRIPTLINE_H
#define SCRIPTINGKRSCRIPTLINE_H

#include <QObject>
#include <QPointF>
#include <QColor>

class KRLineData;

namespace Scripting
{

	/**
		@author Adam Pigg <adam@piggz.co.uk>
	*/
	class Line : public QObject
	{
			Q_OBJECT
		public:
			Line ( KRLineData * );

			~Line();
			
		public slots:
			/**
			 * Return the start position of the line
			 * @return start position
			 */
			QPointF startPosition();
			
			/**
			 * Set the start position of the line
			 * @param StartPosition
			 */
			void setStartPosition(const QPointF&);
			
			/**
			 * Return the end position of the line
			 * @return end position
			 */
			QPointF endPosition();
			
			/**
			 * Set the end position of the line
			 * @param EndPosition
			 */
			void setEndPosition(const QPointF&);
			
			/**
			 * Return the color of the line
			 * @return line color
			 */
			QColor lineColor();
			
			/**
			 * Sets the line color
			 * @param LineColor
			 */
			void setLineColor(const QColor&);
			
			/**
			 * Return the weight (width) of the line
			 * @return Weight
			 */
			int lineWeight();
			
			/**
			 * Set the weight (width) of the line
			 * @param Weight 
			 */
			void setLineWeight(int);
			
			/**
			 * Return the line style.  Valid values are those from Qt::PenStyle (0-5)
			 * @return Style
			 */
			int lineStyle();
			
			
			/**
			 * Set the style of the line
			 * @param Style From Qt::PenStyle (0-5) 
			 */
			void setLineStyle(int);
			
		private:
			KRLineData *_line;

	};

}

#endif
