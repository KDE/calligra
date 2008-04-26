//
// C++ Interface: krscriptbarcode
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCRIPTINGKRSCRIPTBARCODE_H
#define SCRIPTINGKRSCRIPTBARCODE_H

#include <QObject>
#include <krbarcodedata.h>

class KRBarcodeData;

namespace Scripting
{

	/**
		@author Adam Pigg <adam@piggz.co.uk>
	*/
	class Barcode : public QObject
	{
			Q_OBJECT
		public:
			Barcode ( KRBarcodeData *f );

			~Barcode();

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

			/**
			 * Get the horizontal alignment
			 * -1 Left
			 * 0 Center
			 * +1 Right
			 * @return alignment
			*/
			int horizontalAlignment();

			/**
			 * Sets the horizontal alignment
			 * @param  Alignemnt
			 */
			void setHorizonalAlignment ( int );


			/**
			 * Get the control source (field name) of the barcode
			 * @return control source
			 */
			QString source();


			/**
			 * Set the control source (field name) of the barcode
			 * @param controlsource
			 */
			void setSource ( const QString& );


			/**
			 * Get the barcode format
			 * @return format as string
			 */
			QString format();
			
			/**
			 * Set the barcode format
			 * @param format 
			 */
			void setFormat ( const QString& );


		private:
			KRBarcodeData *_barcode;
	};

}

#endif
