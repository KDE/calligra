/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMBARCONFIGDIALOG_H__
#define __KODIAGRAMMBARCONFIGDIALOG_H__

#include "KoDiagrammConfigDialog.h"

class KoDiagrammParameters;
class KoDiagrammBarConfigPage;

class KoDiagrammBarConfigDialog : public KoDiagrammConfigDialog
{
	Q_OBJECT

public:
	KoDiagrammBarConfigDialog( KoDiagrammParameters* params, 
							   QWidget*	parent );

public slots:
	virtual void apply();
	virtual void defaults();

private:
	KoDiagrammBarConfigPage* _barpage;
};

#endif
