/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, The Karbon Developers
*/

#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qhbuttongroup.h>
#include <qpushbutton.h>
#include <qtabwidget.h>

#include <klocale.h>
#include <koMainWindow.h>
#include <koView.h>

#include "karbon_part.h"

#include "vtransformdlg.h"

VTransformDlg::VTransformDlg( KarbonPart* part, KoView* parent, const char* /*name*/ )
	: QDockWindow( QDockWindow::OutsideDock, parent->shell() ), m_part ( part )
{
	setCaption( i18n( "Transform" ) );
	setCloseMode( QDockWindow::Always );

	//mTabWidget = new QTabWidget( this );
	//mRGBWidget = new QWidget(mTabWidget);

	// QGridLayout *mainLayout = new QGridLayout(mRGBWidget, 4, 1);

}

#include "vtransformdlg.moc"

