/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef PolygonPreview_h_
#define PolygonPreview_h_

#include <qframe.h>

class PolygonPreview : public QFrame {

    Q_OBJECT

public:
    PolygonPreview(QWidget* parent = 0L, const char* name = 0L);

    virtual QSize sizeHint() const { return QSize(150, 150); }
    virtual QSize minimumSizeHint() const { return QSize(70, 70); }
    virtual QSizePolicy sizePolicy() const { return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); }

public slots:
    void slotSharpness (int value);
    void slotConcavePolygon ();
    void slotConvexPolygon ();
    void increaseNumOfCorners ();
    void decreaseNumOfCorners ();
    void setNumOfCorners (int value);

protected:
    virtual void paintEvent(QPaintEvent *e);

private:
    int nCorners;
    int sharpness;
    bool isConcave;
};

#endif
