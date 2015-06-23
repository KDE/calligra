/* This file is part of the KDE project
   Copyright (C) 2005-2014 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIDATAAWAREVIEW_H
#define KEXIDATAAWAREVIEW_H

#include "kexidataviewcommon_export.h"
#include <KexiView.h>
#include <core/kexisearchandreplaceiface.h>

class KexiDataAwareObjectInterface;
class KexiSharedActionClient;

/*! @short Provides a view displaying record-based data.

 The KexiDataAwareView is used to implement differently-looking views
 for displaying record-based data in a consistent way:
 - tabular data views
 - form data view

 Action implementations like data editing and deleting are shared for different
 view types to keep even better consistency.
 The view also implements KexiSearchAndReplaceViewInterface to support search/replace features
 used by shared KexiFindDialog.
*/
class KEXIDATAVIEWCOMMON_EXPORT KexiDataAwareView : public KexiView,
            public KexiSearchAndReplaceViewInterface
{
    Q_OBJECT

public:
    explicit KexiDataAwareView(QWidget *parent = 0);

    virtual ~KexiDataAwareView();

    QWidget* mainWidget() const;

    virtual QSize minimumSizeHint() const;

    virtual QSize sizeHint() const;

    KexiDataAwareObjectInterface* dataAwareObject() const;

    /*! Sets up data for find/replace dialog, based on view's data model.
     Implemented for KexiSearchAndReplaceViewInterface. */
    virtual bool setupFindAndReplace(QStringList& columnNames, QStringList& columnCaptions,
                                     QString& currentColumnName);

    /*! Finds \a valueToFind within the view.
     Implemented for KexiSearchAndReplaceViewInterface. */
    virtual tristate find(const QVariant& valueToFind,
                          const KexiSearchAndReplaceViewInterface::Options& options, bool next);

    /*! Finds \a valueToFind within the view and replaces with \a replacement.
     Implemented for KexiSearchAndReplaceViewInterface. */
    virtual tristate findNextAndReplace(const QVariant& valueToFind,
                                        const QVariant& replacement,
                                        const KexiSearchAndReplaceViewInterface::Options& options, bool replaceAll);

public Q_SLOTS:
    void deleteAllRecords();
    void deleteCurrentRecord();
    void deleteAndStartEditCurrentCell();
    void startEditOrToggleValue();
    bool acceptRecordEditing();
    bool cancelRecordEditing();
    void sortAscending();
    void sortDescending();
    void copySelection();
    void cutSelection();
    void paste();
    void slotGoToFirstRecord();
    void slotGoToPreviusRecord();
    void slotGoToNextRecord();
    void slotGoToLastRecord();
    void slotGoToNewRecord();

    /*! @return true if data editing is in progress.
     * Implemented for KexiView. */
    bool isDataEditingInProgress() const;

    /*! Identical to acceptRecordEditing()
     * @todo replace acceptRecordEditing() with this method
     * Implemented for KexiView. */
    virtual tristate saveDataChanges();

    /*! Identical to cancelRecordEditing()
     * @todo replace cancelRecordEditing() with this method
     * Implemented for KexiView. */
    virtual tristate cancelDataChanges();

protected Q_SLOTS:
    void slotCellSelected(int record, int column);
    void reloadActions();
    void slotUpdateRecordActions(int record);
    //! Updates 'save/cancel record changes' actions
    void slotUpdateSaveCancelActions();
    void slotClosing(bool* cancel);

protected:
    void init(QWidget* viewWidget, KexiSharedActionClient* actionClient,
              KexiDataAwareObjectInterface* dataAwareObject,
              // temporary, for KexiFormView in design mode
              bool noDataAware = false
             );
    void initActions();
    virtual void updateActions(bool activated);

    QWidget* internalView() const;

    virtual bool eventFilter(QObject *o, QEvent *e);
    QAction* sharedActionRequested(QKeyEvent *ke, const char *actionName);

private:
    class Private;
    Private * const d;
};

#endif
