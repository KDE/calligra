/* This file is part of the KDE project
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

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
#ifndef KEXILOOKUPCOLUMNPAGE_H
#define KEXILOOKUPCOLUMNPAGE_H

#include <qwidget.h>
//Added by qt3to4:
#include <Q3Frame>
#include <Q3CString>
#include <QLabel>
#include <kexidb/field.h>
#include <kexidb/utils.h>
#include <koproperty/Set.h>

class KexiProject;
class QLabel;
class Q3Frame;

//! @short A page within table designer's property pane, providing lookup column editor.
/*! It's data model is basically KexiDB::LookupFieldSchema class, but the page does
 not create it directly but instead updates a property set that defines
 the field currently selected in the designer.

 @todo not all features of KexiDB::LookupFieldSchema class are displayed on this page yet
 */
class KexiLookupColumnPage : public QWidget
{
    Q_OBJECT

public:
    KexiLookupColumnPage(QWidget *parent);
    virtual ~KexiLookupColumnPage();

public slots:
    void setProject(KexiProject *prj);
    void clearRowSourceSelection(bool alsoClearComboBox = true);
    void clearBoundColumnSelection();
    void clearVisibleColumnSelection();

    //! Receives a pointer to a new property \a set (from KexiFormView::managerPropertyChanged())
    void assignPropertySet(KoProperty::Set* propertySet);

signals:
    //! Signal emitted when helper button 'Go to selected row sourcesource' is clicked.
    void jumpToObjectRequested(const Q3CString& mime, const Q3CString& name);

//  /*! Signal emitted when current bound column has been changed. */
//  void boundColumnChanged(const QString& string, const QString& caption,
    //  KexiDB::Field::Type type);

protected slots:
    void slotRowSourceTextChanged(const QString & string);
    void slotRowSourceChanged();
    void slotGotoSelectedRowSource();
    void slotBoundColumnSelected();
    void slotVisibleColumnSelected();

protected:
    void updateBoundColumnWidgetsAvailability();

    //! Used instead of m_propertySet->changeProperty() to honor m_propertySetEnabled
    void changeProperty(const Q3CString &property, const QVariant &value);

private:
    class Private;
    Private* const d;
};

#endif
