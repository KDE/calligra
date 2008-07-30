/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXIDBCONNECTIONWIDGET_H
#define KEXIDBCONNECTIONWIDGET_H

#include "ui_kexidbconnectionwidget.h"
#include "ui_kexidbconnectionwidgetdetails.h"

#include <kexiprojectdata.h>

#include <KTabWidget>
#include <KDialog>

class KPushButton;
class KexiDBDriverComboBox;
class KexiDBConnectionTabWidget;

class KEXIEXTWIDGETS_EXPORT KexiDBConnectionWidget
  : public QWidget, protected Ui::KexiDBConnectionWidget
{
  Q_OBJECT
  
  public:
    KexiDBConnectionWidget( QWidget* parent = 0 );
    virtual ~KexiDBConnectionWidget();

    /*! Sets project data \a data.
     \a shortcutFileName is only used to check if the file is writable 
     (if no, "save changes" button will be disabled). */
    void setData(const KexiProjectData& data, const QString& shortcutFileName = QString());

    /*! Sets connection data \a data.
     \a shortcutFileName is only used to check if the file is writable 
     (if no, "save changes" button will be disabled). */
    void setData(const KexiDB::ConnectionData& data, 
      const QString& shortcutFileName = QString());

    KexiProjectData data();

    //! \return a pointer to 'save changes' button. You can call hide() for this to hide it.
    KPushButton* saveChangesButton() const;

    //! \return a pointer to 'test connection' button. You can call hide() for this to hide it.
    KPushButton* testConnectionButton() const;

    KexiDBDriverComboBox *driversCombo() const { return m_driversCombo; }

    //! \return true if only connection data is managed by this widget
    bool connectionOnly() const;

  signals:
    //! emitted when data saving is needed
    void saveChanges();

    void loadDBList();

  protected slots:
    void slotLocationBGrpClicked(int id);
    void slotCBToggled(bool on);

  protected:
    void setDataInternal(const KexiProjectData& data, bool connectionOnly, 
      const QString& shortcutFileName);

    KexiProjectData m_data;
    KexiDBDriverComboBox *m_driversCombo;

    class Private;
    Private * const d;
  
  friend class KexiDBConnectionTabWidget;
  friend class KexiDBConnectionDialog;
};

class KEXIEXTWIDGETS_EXPORT KexiDBConnectionWidgetDetails
  : public QWidget, public Ui::KexiDBConnectionWidgetDetails
{
  public:
    KexiDBConnectionWidgetDetails(QWidget* parent = 0);
    ~KexiDBConnectionWidgetDetails();
};

class KEXIEXTWIDGETS_EXPORT KexiDBConnectionTabWidget : public KTabWidget
{
  Q_OBJECT

  public:
    KexiDBConnectionTabWidget( QWidget* parent = 0 );
    virtual ~KexiDBConnectionTabWidget();

    /*! Sets connection data \a data.
     \a shortcutFileName is only used to check if the file is writable 
     (if no, "save changes" button will be disabled). */
    void setData(const KexiProjectData& data, const QString& shortcutFileName = QString());
    
    void setData(const KexiDB::ConnectionData& data, 
      const QString& shortcutFileName = QString());
    
    KexiProjectData currentProjectData();

    //! \return true if 'save password' option is selected
    bool savePasswordOptionSelected() const;

  signals:
    //! emitted when test connection is needed
    void testConnection();

  protected slots:
    void slotTestConnection();
    void slotSocketComboboxToggled( bool on );

  protected:
    KexiDBConnectionWidget *mainWidget;
    KexiDBConnectionWidgetDetails* detailsWidget;

  friend class KexiDBConnectionDialog;
};

class KEXIEXTWIDGETS_EXPORT KexiDBConnectionDialog : public KDialog
{
  Q_OBJECT

  public:
    /*! Creates a new connection dialog for project data \a data.
     Not only connection data is visible but also database name and and title.
     \a shortcutFileName is only used to check if the shortcut file is writable 
     (if no, "save changes" button will be disabled).
     The shortcut file is in .KEXIS format.
     Connect to saveChanges() signal to react on saving changes.
     If \a shortcutFileName is empty, the button will be hidden. 
     \a acceptButtonGuiItem allows to override default "Open" button's appearance. */
    KexiDBConnectionDialog(QWidget* parent, const KexiProjectData& data, 
      const QString& shortcutFileName = QString(), 
      const KGuiItem& acceptButtonGuiItem = KGuiItem(""));

    /*! Creates a new connection dialog for connection data \a data.
     Only connection data is visible: database name and and title fields are hidden.
     \a shortcutFileName is only used to check if the shortcut file is writable 
     (if no, "save changes" button will be disabled).
     The shortcut file is in .KEXIC format. 
     See above constructor for more details. */
    KexiDBConnectionDialog(QWidget* parent, const KexiDB::ConnectionData& data, 
      const QString& shortcutFileName = QString(),
      const KGuiItem& acceptButtonGuiItem = KGuiItem(""));

    ~KexiDBConnectionDialog();

    /*! \return project data displayed within the dialog.
     Information about database name and title can be empty if the dialog 
     contain only a connection data (if second constructor was used). */
    KexiProjectData currentProjectData();

    //! \return true if 'save password' option is selected
    bool savePasswordOptionSelected() const;

    KexiDBConnectionWidget *mainWidget() const;
    KexiDBConnectionWidgetDetails* detailsWidget() const;

  signals:
    //! emitted when data saving is needed
    void saveChanges();

    //! emitted when test connection is needed
    void testConnection();

    void loadDBList();

  protected:
    KexiDBConnectionTabWidget *m_tabWidget;
  
  private:
    void init(const KGuiItem& acceptButtonGuiItem);
};

#endif // KEXIDBCONNECTIONWIDGET_H
