/* This file is part of the KDE project
   Copyright (C) 2003 Peter Simonsson <psn@linux.se>,
   theKompany.com & Dave Marotti

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

#ifndef KIVIOARROWHEADACTION_H
#define KIVIOARROWHEADACTION_H


#include <kaction.h>

class KPopupMenu;

class KivioArrowHeadAction : public KActionMenu
{
  Q_OBJECT
  public:
    KivioArrowHeadAction(const QString &text, const QString &pix,
      QObject* parent = 0, const char *name = 0);
    ~KivioArrowHeadAction();

    KPopupMenu* popupMenu() const;
    void popup( const QPoint& global );

    virtual int plug( QWidget* widget, int index = -1 );

    int currentStartArrow();
    int currentEndArrow();

  public slots:
    void setCurrentStartArrow(int c);
    void setCurrentEndArrow(int c);
    void setEmitSignals(bool e) { m_emitSignals = e; }

  signals:
    void startChanged(int);
    void endChanged(int);

  protected:
    void loadArrowHeads(KPopupMenu* combo);

  protected:
    KPopupMenu* m_popup;
    KPopupMenu* m_startPopup;
    KPopupMenu* m_endPopup;
    int m_currentStart;
    int m_currentEnd;
    bool m_emitSignals;
};

#endif
