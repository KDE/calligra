/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMAREACONFIGDIALOG_H__
#define __KODIAGRAMMAREACONFIGDIALOG_H__

#include "KoDiagrammConfigDialog.h"

class KoDiagrammParameters;

class KoDiagrammAreaConfigDialog : public KoDiagrammConfigDialog
{
	Q_OBJECT

public:
	KoDiagrammAreaConfigDialog( KoDiagrammParameters* params, 
								QWidget*	parent );
};

#endif
