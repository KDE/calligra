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

#ifndef KOLINEWIDTHACTION_H
#define KOLINEWIDTHACTION_H

#include <kaction.h>
#include <kdialogbase.h>

#include <koUnit.h>

class KPopupMenu;
class QPoint;

class KoLineWidthAction : public KActionMenu
{
  Q_OBJECT
  public:
    KoLineWidthAction(const QString& text, const QString& icon, QObject* parent = 0, const char* name = 0);
    KoLineWidthAction(const QString& text, const QString& icon, const QObject* receiver,
      const char* slot, QObject* parent, const char* name = 0);
    ~KoLineWidthAction();
    
    KPopupMenu* popupMenu() const;
    void popup(const QPoint& global);
  
    virtual int plug(QWidget* widget, int index = -1);
    
    double currentWidth() { return m_currentWidth; }
    
  signals:
    /** Emited when a new line width have been selected */
    void newLineWidth(double);
  
  public slots:
    void setCurrentWidth(double width);
    void setUnit(KoUnit::Unit unit);
  
  protected slots:
    void execute(int index);
    
  protected:
    void createMenu();
  
  private:
    KPopupMenu* m_popup;
    double m_currentWidth;
    int m_currentIndex;
    KoUnit::Unit m_unit;
};

class KoUnitDoubleSpinBox;

/** This class provides a dialog for setting a custom line width */
class KoLineWidthChooser : public KDialogBase
{
  Q_OBJECT
  public:
    KoLineWidthChooser(QWidget* parent = 0, const char* name = 0);
    
    double width();
  
  public slots:
    void setUnit(KoUnit::Unit unit);
    void setWidth(double width);
    
  private:
    KoUnit::Unit m_unit;
    KoUnitDoubleSpinBox* m_lineWidthUSBox;
};

#endif
