//   layer list widget to be incorporated into a layer control widget
//
//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#include <klocale.h>

#include <koUndo.h>

#include "kimageshop_undo.h"

KImageShopCommand::KImageShopCommand( const QString& name, KImageShopDoc* doc )
  : KoCommand( name )
  , m_pDoc( doc )
{
}

KImageShopCommand::~KImageShopCommand()
{
}

