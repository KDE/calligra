//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#ifndef __layerdlg_h__
#define __layerdlg_h__

#include <qtabdialog.h>

class KImageShopDoc;

class LayerTab : public QWidget
{
  Q_OBJECT

public:

  LayerTab( KImageShopDoc* doc, QWidget* _parent = 0, const char* _name = 0, WFlags _flags = 0 );
};

class ChannelTab : public QWidget
{
  Q_OBJECT
 
public:
 
  ChannelTab( KImageShopDoc* doc, QWidget* _parent = 0, const char* _name = 0, WFlags _flags = 0 );
};

class LayerDialog : public QTabDialog
{
  Q_OBJECT

public:

  LayerDialog( KImageShopDoc* doc, QWidget* _parent = 0, const char* _name = 0, WFlags _flags = 0 );
};

#endif // __layerdlg_h__

