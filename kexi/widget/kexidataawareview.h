/* This file is part of the KDE project
   Copyright (C) 2005-2009 Jarosław Staniek <staniek@kde.org>

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

#include <kexi_export.h>
#include <KexiView.h>
#include <kexisearchandreplaceiface.h>

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
class KEXIEXTWIDGETS_EXPORT KexiDataAwareView : public KexiView,
            public KexiSearchAndReplaceViewInterface
{
    Q_OBJECT

public:
    KexiDataAwareView(QWidget *parent = 0);

    virtual ~KexiDataAwareView();

    QWidget* mainWidget() const;

    virtual QSize minimumSizeHint() const;

    virtual QSize sizeHint() const;

    KexiDataAwareObjectInterface* dataAwareObject() const {
        return m_dataAwareObject;
    }

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

public slots:
    void deleteAllRows();
    void deleteCurrentRow();
    void deleteAndStartEditCurrentCell();
    void startEditOrToggleValue();
    bool acceptRowEdit();
    void cancelRowEdit();
    void sortAscending();
    void sortDescending();
    void copySelection();
    void cutSelection();
    void paste();
    void slotGoToFirstRow();
    void slotGoToPreviusRow();
    void slotGoToNextRow();
    void slotGoToLastRow();
    void slotGoToNewRow();
    /*  void editFind();
        void slotFind();
        void editFindNext();
        void editFindPrevious();
        void editReplace();*/

protected slots:
//  void slotCellSelected(const QVariant& v); //!< @internal
    void slotCellSelected(int col, int row);
    void reloadActions();
    void slotUpdateRowActions(int row);
    void slotClosing(bool& cancel);

protected:
    void init(QWidget* viewWidget, KexiSharedActionClient* actionClient,
              KexiDataAwareObjectInterface* dataAwareObject,
              // temporary, for KexiFormView in design mode
              bool noDataAware = false
             );
    void initActions();
    virtual void updateActions(bool activated);

    QWidget* m_internalView;
    KexiSharedActionClient* m_actionClient;
    KexiDataAwareObjectInterface* m_dataAwareObject;
};

#endif
