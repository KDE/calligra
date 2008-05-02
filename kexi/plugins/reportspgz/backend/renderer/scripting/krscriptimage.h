/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)                  
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
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
