/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qgroupbox.h>
#include <qstring.h>

#include <ktoolbar.h>

#include "vtoolcontainer.h"

VToolContainer::VToolContainer( const QString& text, QObject* parent, const char* name )
	: KAction( text, 0L, parent, name )
{
}

int
VToolContainer::plug( QWidget* widget, int index )
{
	if ( widget->inherits( "KToolBar" ) )
	{
		KToolBar* tb = static_cast<KToolBar*>( widget );

		int id = KAction::getToolButtonID();

		QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, 0L, widget );
		static_cast<KActionCollection*>( parent() )->action( "tool_select" )->plug( group );
		static_cast<KActionCollection*>( parent() )->action( "tool_polygon" )->plug( group );

		tb->insertWidget( id, group->width(), group, index );

		addContainer( tb, id );

		return containerCount() - 1;
	}

	return -1;
}

#include "vtoolcontainer.moc"
