//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#ifndef __kimagesop_undo_h__
#define __kimagesop_undo_h__

#include <koUndo.h>

class KImageShopDoc;

class KImageShopCommand : public KoCommand
{
public:

  KImageShopCommand( const QString& name, KImageShopDoc* doc );
  ~KImageShopCommand();

protected:

  KImageShopDoc *m_pDoc;
};

#endif

