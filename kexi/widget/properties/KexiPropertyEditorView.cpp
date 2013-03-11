/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

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

#include "KexiPropertyEditorView.h"
#include "KexiObjectInfoLabel.h"
#include <KexiMainWindowIface.h>
#include <koproperty/Set.h>
#include <koproperty/EditorView.h>
#include <koproperty/Property.h>

#include <QLayout>

#include <KLocale>

//! @internal
class KexiPropertyEditorView::Private
{
public:
    Private() {
    }
    KoProperty::EditorView *editor;
};

KexiPropertyEditorView::KexiPropertyEditorView(QWidget* parent)
        : KexiPropertyPaneViewBase(parent)
        , d(new Private())
{
    setObjectName("KexiPropertyEditorView");
    setWindowTitle(i18n("Properties"));
    //TODO: set a nice icon
//    setWindowIcon(KexiMainWindowIface::global()->thisWidget()->windowIcon());

    d->editor = new KoProperty::EditorView(this);
    layout()->addWidget(d->editor);
    setFocusProxy(d->editor);
    infoLabel()->setBuddy(d->editor);
    setFocusPolicy(Qt::WheelFocus);

    connect(d->editor, SIGNAL(propertySetChanged(KoProperty::Set*)),
            this, SLOT(slotPropertySetChanged(KoProperty::Set*)));

    slotPropertySetChanged(0);
}

KexiPropertyEditorView::~KexiPropertyEditorView()
{
    delete d;
}

QSize KexiPropertyEditorView::sizeHint() const
{
    return QSize(200, 200);//m_editor->sizeHint();
}

QSize KexiPropertyEditorView::minimumSizeHint() const
{
    return QSize(200, 200);//m_editor->sizeHint();
}

KoProperty::EditorView *KexiPropertyEditorView::editor() const
{
    return d->editor;
}

void KexiPropertyEditorView::slotPropertySetChanged(KoProperty::Set* set)
{
    //update information about selected object
    updateInfoLabelForPropertySet(set);
    d->editor->setEnabled(set);
}

#include "KexiPropertyEditorView.moc"
