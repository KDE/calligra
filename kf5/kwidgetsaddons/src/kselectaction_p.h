/* This file is part of the KDE libraries
    Copyright (C) 1999 Reginald Stadlbauer <reggie@kde.org>
              (C) 1999 Simon Hausmann <hausmann@kde.org>
              (C) 2000 Nicolas Hadacek <haadcek@kde.org>
              (C) 2000 Kurt Granroth <granroth@kde.org>
              (C) 2000 Michael Koch <koch@kde.org>
              (C) 2001 Holger Freyther <freyther@kde.org>
              (C) 2002 Ellis Whitehead <ellis@kde.org>
              (C) 2002 Joseph Wenninger <jowenn@kde.org>
              (C) 2003 Andras Mantia <amantia@kde.org>
              (C) 2005-2006 Hamish Rodda <rodda@kde.org>
              (C) 2006 Albert Astals Cid <aacid@kde.org>
              (C) 2006 Clarence Dang <dang@kde.org>
              (C) 2006 Michel Hermier <michel.hermier@gmail.com>
              (C) 2007 Nick Shaforostoff <shafff@ukr.net>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KSELECTACTION_P_H
#define KSELECTACTION_P_H

#include <QComboBox>

class KSelectActionPrivate
{
    Q_DECLARE_PUBLIC(KSelectAction)

public:
  KSelectActionPrivate()
    : q_ptr(0)
  {
    m_edit = false;
    m_menuAccelsEnabled = true;
    m_comboWidth = -1;
    m_maxComboViewCount = -1;

    m_toolBarMode = KSelectAction::ComboBoxMode;
    m_toolButtonPopupMode = QToolButton::InstantPopup; //InstantPopup by default because there is no default action

    m_actionGroup = new QActionGroup(0L);
  }

  virtual ~KSelectActionPrivate()
  {
    // unhook the event filter, as the deletion of the actiongroup
    // will trigger it
    Q_FOREACH( QComboBox* box, m_comboBoxes )
        box->removeEventFilter( q_ptr );
    Q_FOREACH( QToolButton* button, m_buttons )
        button->removeEventFilter( q_ptr );
    delete m_actionGroup;
  }

  void _k_comboBoxDeleted(QObject*);
  void _k_comboBoxCurrentIndexChanged(int);

  void init(KSelectAction*);

  bool m_edit : 1;
  bool m_menuAccelsEnabled : 1;
  int m_comboWidth;
  int m_maxComboViewCount;

  KSelectAction::ToolBarMode m_toolBarMode;
  QToolButton::ToolButtonPopupMode m_toolButtonPopupMode;

  QActionGroup* m_actionGroup;

  QList<QToolButton*> m_buttons;
  QList<QComboBox*> m_comboBoxes;

  QString makeMenuText( const QString &_text )
  {
      if ( m_menuAccelsEnabled )
        return _text;
      QString text = _text;
      int i = 0;
      while ( i < text.length() ) {
          if ( text[ i ] == QLatin1Char('&') ) {
              text.insert( i, QLatin1Char('&') );
              i += 2;
          }
          else
              ++i;
      }
      return text;
  }

  KSelectAction *q_ptr;
};

/* vim: et sw=2 ts=2
 */

#endif // KSELECTACTION_P_H
