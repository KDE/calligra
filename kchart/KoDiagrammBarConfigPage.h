/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMBARCONFIGPAGE_H__
#define __KODIAGRAMMBARCONFIGPAGE_H__

#include <qwidget.h>
#include <qlineedit.h>
#include "KoDiagrammParameters.h"

class QRadioButton;

class KoDiagrammBarConfigPage : public QWidget
{
	Q_OBJECT

public:
	KoDiagrammBarConfigPage( QWidget* parent );

	void setOverwriteMode( OverwriteMode overwrite );
	OverwriteMode overwriteMode() const;
	void setXAxisDistance( int xbardist );
	int xAxisDistance() const;

private:
	QRadioButton* _sidebysideRB;
	QRadioButton* _ontopRB;
	QRadioButton* _infrontRB;
	QLineEdit* _xbardist;
};


inline void KoDiagrammBarConfigPage::setXAxisDistance( int xbardist )
{
	_xbardist->setText( QString().setNum( xbardist ) );
}

inline int KoDiagrammBarConfigPage::xAxisDistance() const
{
	QString num = _xbardist->text();
	return num.toInt();
}




#endif
