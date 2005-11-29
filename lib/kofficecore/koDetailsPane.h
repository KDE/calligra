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
#ifndef KODETAILSPANE_H
#define KODETAILSPANE_H

#include <klistview.h>

#include "koDetailsPaneBase.h"

class KoTemplateGroup;
class KInstance;
class QListViewItem;
class KoRecentDocumentsPanePrivate;
class KoRichTextListItemPrivate;
class KFileItem;
class QPixmap;

namespace KIO {
  class Job;
}

class KoTemplatesPanePrivate;

class KoTemplatesPane : public KoDetailsPaneBase
{
  Q_OBJECT
  public:
    KoTemplatesPane(QWidget* parent, KInstance* instance, KoTemplateGroup* group);
    ~KoTemplatesPane();

    /// Returns true if a template in this group was the last one selected
    bool isSelected();

  signals:
    void openTemplate(const QString&);

  protected slots:
    void selectionChanged(QListViewItem* item);
    void openTemplate();
    void openTemplate(QListViewItem* item);
    void alwaysUseToggled(bool on);

    void changePalette();

  private:
    KoTemplatesPanePrivate* d;
};


class KoRecentDocumentsPane : public KoDetailsPaneBase
{
  Q_OBJECT
  public:
    KoRecentDocumentsPane(QWidget* parent, KInstance* instance);
    ~KoRecentDocumentsPane();

  signals:
    void openFile(const QString&);

  protected slots:
    void selectionChanged(QListViewItem* item);
    void openFile();
    void openFile(QListViewItem* item);

    void previewResult(KIO::Job* job);
    void updatePreview(const KFileItem* fileItem, const QPixmap& preview);

    void changePalette();

  private:
    KoRecentDocumentsPanePrivate* d;
};

#endif //KODETAILSPANE_H
