/* This file is part of the KDE project
   Copyright (C) 2005 Peter Simonsson <psn@linux.se>

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
 * Boston, MA 02110-1301, USA.
*/
#ifndef KOOPENPANE_H
#define KOOPENPANE_H

#include <koOpenPaneBase.h>

class KoCustomDocumentCreator;
class KConfig;
class KoTemplateGroup;
class KoOpenPanePrivate;
class KInstance;
class QPixmap;
class KListViewItem;
class KoTemplatesPane;

class KoOpenPane : public KoOpenPaneBase
{
  Q_OBJECT

  public:
    KoOpenPane(QWidget *parent, KInstance* instance, const QString& templateType = QString::null);
    virtual ~KoOpenPane();

    KListViewItem* addPane(const QString& title, const QString& icon, QWidget* widget);
    KListViewItem* addPane(const QString& title, const QPixmap& icon, QWidget* widget);

  protected slots:
    void showOpenFileDialog();

    void selectionChanged(QListViewItem* item);

  signals:
    void openExistingFile(const QString&);
    void openTemplate(const QString&);

    /// Emitted when the always use template has changed
    void alwaysUseChanged(KoTemplatesPane* sender, const QString& alwaysUse);

  protected:
    void initRecentDocs();
    void initTemplates(const QString& templateType);

  private:
    KoOpenPanePrivate* d;
};

#endif //KOOPENPANE_H
