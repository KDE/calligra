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
class KRScriptLabel : public QObject
{
		Q_OBJECT
	public:
		KRScriptLabel ( KRLabelData * );

		~KRScriptLabel();

	public slots:
		QString caption();
		void setCaption(const QString&);

	private:
		KRLabelData *_label;
};

#endif
