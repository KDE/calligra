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
			
		private:
			KRImageData *_image;

	};

}

#endif
