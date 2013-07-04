/* This file is part of the KDE project
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
             (C) 1999 David Faure <faure@kde.org>

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
#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <kaction.h>

#include <kxmlguiwindow.h>

#include <part.h>

class QString;

namespace KoParts
{

class MainWindowPrivate;

/**
 * A KPart-aware main window, whose user interface is described in XML.
 *
 * Inherit your main window from this class
 * and don't forget to call setXMLFile() in the inherited constructor.
 *
 * It implements all internal interfaces in the case of a
 * KMainWindow as host: the builder and servant interface (for menu
 * merging).
 */
class KOMAIN_EXPORT MainWindow : public KXmlGuiWindow, virtual public PartBase
{
  Q_OBJECT
 public:
  /**
   * Constructor, same signature as KMainWindow.
   */
  explicit MainWindow( QWidget* parent = 0, Qt::WindowFlags f = KDE_DEFAULT_WINDOWFLAGS );
  /// @deprecated, remove the name argument and use setObjectName instead
#ifndef KDE_NO_DEPRECATED
  KDE_CONSTRUCTOR_DEPRECATED explicit MainWindow( QWidget* parent, const char *name = 0, Qt::WindowFlags f = KDE_DEFAULT_WINDOWFLAGS );
#endif
  /**
   * Destructor.
   */
  virtual ~MainWindow();

public Q_SLOTS:
  virtual void configureToolbars();

protected Q_SLOTS:

  /**
   * Create the GUI (by merging the host's and the active part's)
   * You _must_ call this in order to see any GUI being created.
   *
   * In a main window with multiple parts being shown (e.g. as in Konqueror)
   * you need to connect this slot to the
   * KPartManager::activePartChanged() signal
   *
   * @param part The active part (set to 0L if no part).
   */
  void createGUI( KoParts::Part * part );

  /**
   * Called when the active part wants to change the statusbar message
   * Reimplement if your mainwindow has a complex statusbar
   * (with several items)
   */
  virtual void slotSetStatusBarText( const QString & );

  /**
   * Rebuilds the GUI after KEditToolbar changed the toolbar layout.
   * @see configureToolbars()
   * KDE4: make this virtual. (For now we rely on the fact that it's called
   * as a slot, so the metaobject finds it here).
   */
  void saveNewToolbarConfig();

protected:
  virtual void createShellGUI( bool create = true );

private:
  MainWindowPrivate* const d;
};

}

#endif
