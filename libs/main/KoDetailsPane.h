/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Peter Simonsson <psn@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KODETAILSPANE_H
#define KODETAILSPANE_H

#include "ui_KoDetailsPaneBase.h"

class QEvent;
class QUrl;
class QStandardItemModel;

class KoDetailsPanePrivate;

class KoDetailsPane : public QWidget, public Ui_KoDetailsPaneBase
{
    Q_OBJECT

public:
    KoDetailsPane(QWidget *parent, const QString &header);
    ~KoDetailsPane() override;

    bool eventFilter(QObject *watched, QEvent *e) override;

    /// @return the model used in the document list
    QStandardItemModel *model() const;

Q_SIGNALS:
    /// Emitted when a file is requested to be opened
    void openUrl(const QUrl &);

    /// This is used to keep all splitters in different details panes synced
    void splitterResized(KoDetailsPane *sender, const QList<int> &sizes);

public Q_SLOTS:
    /// This is used to keep all splitters in different details panes synced
    void resizeSplitter(KoDetailsPane *sender, const QList<int> &sizes);

protected Q_SLOTS:
    /// This is called when the selection in the listview changed
    virtual void selectionChanged(const QModelIndex &index) = 0;
    virtual void openFile();
    virtual void openFile(const QModelIndex &index) = 0;

    void changePalette();

protected:
    enum Extents { IconExtent = 64, PreviewExtent = 128 };

private:
    KoDetailsPanePrivate *const d;
};

#endif // KODETAILSPANE_H
