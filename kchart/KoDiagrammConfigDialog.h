/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KODIAGRAMMCONFIGDIALOG_H__
#define __KODIAGRAMMCONFIGDIALOG_H__

#include <qtabdialog.h>

class KoDiagrammParameters;
class KoDiagrammColorConfigPage;
class KoDiagrammGeometryConfigPage;

class KoDiagrammConfigDialog : public QTabDialog
{
	Q_OBJECT

public:
	KoDiagrammConfigDialog( KoDiagrammParameters* params, 
							QWidget*	parent );

protected:
	KoDiagrammParameters* _params;
	KoDiagrammGeometryConfigPage* _geompage;
	KoDiagrammColorConfigPage* _colorpage;

protected slots:
	virtual void apply();
	virtual void defaults();
};

#endif
