/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2004 Peter Simonsson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef KOLINESTYLEACTION_H
#define KOLINESTYLEACTION_H

#include <kaction.h>

class KPopupMenu;
class QPoint;

class KoLineStyleAction : public KActionMenu
{
  Q_OBJECT
  public:
    KoLineStyleAction(const QString& text, const QString& icon, QObject* parent = 0, const char* name = 0);
    KoLineStyleAction(const QString& text, const QString& icon, const QObject* receiver,
      const char* slot, QObject* parent, const char* name = 0);
    ~KoLineStyleAction();
    
    KPopupMenu* popupMenu() const;
    void popup(const QPoint& global);
  
    virtual int plug(QWidget* widget, int index = -1);
    
    int currentStyle() { return m_currentStyle; }
    
  signals:
    /** Emited when a new line style have been selected */
    void newLineStyle(int);
  
  public slots:
    void setCurrentStyle(int style);
  
  protected slots:
    void execute(int index);
    
  protected:
    void createMenu();
  
  private:
    KPopupMenu* m_popup;
    int m_currentStyle;
};

#endif
