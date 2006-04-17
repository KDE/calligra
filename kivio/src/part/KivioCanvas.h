/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KIVIOCANVAS_H
#define KIVIOCANVAS_H

#include <QWidget>

class QPaintEvent;
class KivioView;

class KivioCanvas : public QWidget
{
  Q_OBJECT

  public:
    KivioCanvas(KivioView* parent);
    ~KivioCanvas();

    /// The offset from the left
    int offsetX() const;
    /// The offset from the top
    int offsetY() const;

    /// The zoomed width of the page displayed
    int pageWidth() const;
    /// The zoomed height of the page displayed
    int pageHeight() const;

  public slots:
    /// Set the offset from the left to @p offset
    void setOffsetX(int offset);
    /// Set the offset from the top to @p offset
    void setOffsetY(int offset);

  protected:
    virtual void paintEvent(QPaintEvent* event);

  private:
    int m_offsetX;
    int m_offsetY;
};

#endif
