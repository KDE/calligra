/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include "vcommand.h"

void
VCommandHistory::undo()
{
	KCommandHistory::undo();
	m_part->repaintAllViews();
}

void
VCommandHistory::redo()
{
	KCommandHistory::redo();
	m_part->repaintAllViews();
}
