//
// C++ Interface: krscriptimage
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCRIPTINGKRSCRIPTIMAGE_H
#define SCRIPTINGKRSCRIPTIMAGE_H

#include <QObject>
#include <QPointF>
#include <QSizeF>

class KRImageData;

namespace Scripting
{

	/**
		@author Adam Pigg <adam@piggz.co.uk>
	*/
	class Image : public QObject
	{
			Q_OBJECT
		public:
			Image ( KRImageData *);

			~Image();
		public slots:


			/**
		 * Get the position of the barcode
		 * @return position in points
			 */
			QPointF position();


			/**
			 * Sets the position of the barcode in points
			 * @param Position
			 */
			void setPosition ( const QPointF& );

			/**
			 * Get the size of the barcode
			 * @return size in points
			 */
			QSizeF size();

			/**
			 * Set the size of the barcode in points
			 * @param Size
			 */
			void setSize ( const QSizeF& );	
		private:
			KRImageData *_image;

	};

}

#endif
