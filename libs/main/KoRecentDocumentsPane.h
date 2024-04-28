/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Peter Simonsson <psn@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KORECENTDOCUMENTSPANE_H
#define KORECENTDOCUMENTSPANE_H

#include "KoDetailsPane.h"

class KFileItem;
class QPixmap;
class KJob;

class KoRecentDocumentsPanePrivate;

/**
 * This widget is the recent doc part of the template opening widget.
 * The parent widget is initial widget in the document space of each Calligra component.
 * This widget shows a list of recent documents and can show their details or open it.
 */
class KoRecentDocumentsPane : public KoDetailsPane
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param parent the parent widget
     * @param header string used as header text in the listview
     */
    KoRecentDocumentsPane(QWidget *parent, const QString &header);
    ~KoRecentDocumentsPane() override;

protected Q_SLOTS:
    void selectionChanged(const QModelIndex &index) override;
    void openFile() override;
    void openFile(const QModelIndex &index) override;

    void previewResult(KJob *job);
    void updatePreview(const KFileItem &fileItem, const QPixmap &preview);
    void updateIcon(const KFileItem &fileItem, const QPixmap &pixmap);

private:
    KoRecentDocumentsPanePrivate *const d;
};

#endif
