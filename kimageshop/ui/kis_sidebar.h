/*
 *  kis_sidebar.h - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <elter@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __kis_sidebar_h__
#define __kis_sidebar_h__

#include <qframe.h>
#include <kis_color.h>

class KDualColorButton;

class TopFrame : public QFrame
{
  Q_OBJECT;

 public:
  TopFrame( QWidget* parent = 0, const char* name = 0 );

 protected:
  virtual void drawContents ( QPainter * );
};

class ControlFrame : public QFrame
{
  Q_OBJECT;

 public:
  ControlFrame( QWidget* parent = 0, const char* name = 0 );
  ~ControlFrame();

  void setFGColor(const KisColor&);
  void setBGColor(const KisColor&);

 signals:
  void fgColorChanged(const KisColor&);
  void bgColorChanged(const KisColor&);

 protected:
  virtual void resizeEvent ( QResizeEvent * );

 protected slots:
  void slotFGColorSelected(const QColor&);
  void slotBGColorSelected(const QColor&);

 private:
  KDualColorButton  *m_pColorButton;
};

class KisSideBar : public QWidget
{
  Q_OBJECT;

 public:
  KisSideBar( QWidget* parent = 0, const char* name = 0 );
  ~KisSideBar();

 public slots:
  void slotSetFGColor(const KisColor&);
  void slotSetBGColor(const KisColor&);

 signals:
  void fgColorChanged(const KisColor&);
  void bgColorChanged(const KisColor&);

 protected:
  virtual void resizeEvent ( QResizeEvent * );

 protected slots:
  void slotFGColorSelected(const KisColor&);
  void slotBGColorSelected(const KisColor&);

 private:
  TopFrame     *m_pTopFrame;
  ControlFrame *m_pControlFrame;
};

#endif
