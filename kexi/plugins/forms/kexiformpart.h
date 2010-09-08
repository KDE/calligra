/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005-2009 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIFORMPART_H
#define KEXIFORMPART_H

#include <qdom.h>

#include <kexi.h>
#include <kexipart.h>
#include <kexiblobbuffer.h>
#include <KexiWindowData.h>

namespace KFormDesigner
{
class WidgetLibrary;
class Form;
class WidgetTreeWidget;
}

namespace KexiDB
{
class FieldList;
}

class KexiDataSourcePage;

//! Kexi Form Plugin
/*! It just creates a \ref KexiFormView. See there for most of code. */
class KEXIFORMUTILS_EXPORT KexiFormPart : public KexiPart::Part
{
    Q_OBJECT

public:
    KexiFormPart(QObject *parent, const QVariantList &);
    virtual ~KexiFormPart();

    KexiDataSourcePage* dataSourcePage() const;

    KFormDesigner::WidgetTreeWidget* widgetTreePage() const;

    void generateForm(KexiDB::FieldList *list, QDomDocument &domDoc);

    class TempData : public KexiWindowData
    {
    public:
        TempData(QObject* parent);
        ~TempData();
        QPointer<KFormDesigner::Form> form;
        QPointer<KFormDesigner::Form> previewForm;
        QString tempForm;
        QPoint scrollViewContentsPos; //!< to preserve contents pos after switching
        //!< to other view
        int resizeMode; //!< form's window's resize mode -one of KexiFormView::ResizeMode items
        //! Used in KexiFormView::setUnsavedLocalBLOBs()
        QHash<QWidget*, KexiBLOBBuffer::Id_t> unsavedLocalBLOBs;
        //! Used when loading a form from (temporary) XML in Data View
        //! to get unsaved blobs collected at design mode.
        QHash<QByteArray, KexiBLOBBuffer::Id_t> unsavedLocalBLOBsByName;
    };

    virtual KLocalizedString i18nMessage(const QString& englishMessage,
                                         KexiWindow* window) const;

protected:
    virtual KexiWindowData* createWindowData(KexiWindow* window);

    virtual KexiView* createView(QWidget *parent, KexiWindow* window,
                                 KexiPart::Item &item, Kexi::ViewMode viewMode = Kexi::DataViewMode, 
                                 QMap<QString, QVariant>* staticObjectArgs = 0);

    virtual void initPartActions();
    virtual void initInstanceActions();
    virtual void setupCustomPropertyPanelTabs(KTabWidget *tab);

protected slots:
//2.0 not needed, the code from slot receiving this signal is moved to Form itself
//    void slotAutoTabStopsSet(KFormDesigner::Form *form, bool set);
//2.0 moved to KexiFormManager    void slotAssignAction();
//2.0 moved to Form    void slotPropertyChanged(QWidget *widget, const QByteArray &name, const QVariant &value);
//2.0 moved to KexiFormManager    void slotWidgetCreatedByFormsLibrary(QWidget* widget);

private:
    class Private;
    Private* d;
};

#endif

