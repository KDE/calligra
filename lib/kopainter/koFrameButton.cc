/* This file is part of the KDE project
  Copyright (c) 1999 Matthias Elter <me@kde.org>
  Copyright (c) 2001 Igor Janssen <rm@linux.ru.net>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koFrameButton.h"

KoFrameButton::KoFrameButton(QWidget *parent, const char *name):
QLabel(parent, name)
{
  setAutoResize(true);
  setFrameStyle(Panel | Raised);
  setLineWidth(1);
  setMaximumHeight(8);
  mActive = false;
  mToggle = false;
}

KoFrameButton::KoFrameButton(const QString &text, QWidget *parent, const char *name):
QLabel(parent, name)
{
  setFrameStyle(Panel | Raised);
  setLineWidth(1);
  setText(text);
  setAlignment(AlignHCenter | AlignVCenter);
  mActive = false;
  mToggle = false;
}

KoFrameButton::KoFrameButton(const QPixmap &pixmap, QWidget *parent, const char *name):
QLabel(parent, name)
{
  setFrameStyle(Panel | Raised);
  setLineWidth(1);
  setPixmap(pixmap);
  mActive = false;
  mToggle = false;
}

void KoFrameButton::mousePressEvent(QMouseEvent *)
{
  setFrameStyle(Panel | Sunken);
}

void KoFrameButton::mouseReleaseEvent(QMouseEvent *)
{
  if(mToggle)
  {
    mActive = !mActive;
    if(mActive)
      setFrameStyle(Panel | Sunken);
    else
      setFrameStyle(Panel | Raised);
  }
  else
    setFrameStyle(Panel | Raised);

  emit clicked();
  emit clicked(mText);
}

void KoFrameButton::setOn(bool v)
{
  if(!mToggle)
    return;

  mActive = v;

  if(mActive)
    setFrameStyle(Panel | Sunken);
  else
    setFrameStyle(Panel | Raised);	
}


void KoFrameButton::setToggleButton(bool v)
{
  mToggle = v;
}

void KoFrameButton::setText(const QString &t)
{
  mText = t;
  QLabel::setText(t);
}

#include "koFrameButton.moc"
