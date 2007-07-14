/* This file is part of the KDE project
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999-2001 Laurent Montel <montel@kde.org>
   Copyright 1998-1999 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "AddNamedAreaDialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include <kmessagebox.h>

#include "Doc.h"
#include "Localization.h"
#include "NamedAreaManager.h"
#include "Sheet.h"
#include "View.h"
#include "Selection.h"

using namespace KSpread;

AddNamedAreaDialog::AddNamedAreaDialog(View* parent)
    : KDialog(parent)
    , m_pView(parent)
{
    setButtons(Ok | Cancel);
    setCaption(i18n("Add Named Area"));
    setModal(true);
    setObjectName("AddNamedAreaDialog");

    QWidget* widget = new QWidget();
    setMainWidget(widget);

    QVBoxLayout* layout = new QVBoxLayout(widget);

    QLabel* label = new QLabel(i18n("Enter the area name:"), widget);
    layout->addWidget(label);

    m_areaName = new QLineEdit(widget);
    m_areaName->setFocus();
    m_areaName->setMinimumWidth(m_areaName->sizeHint().width()* 3);
    layout->addWidget(m_areaName);

    enableButtonOk(!m_areaName->text().isEmpty());

    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
    connect(m_areaName, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotAreaNameChanged(const QString&)));
}

void AddNamedAreaDialog::slotAreaNameChanged(const QString& name)
{
    enableButtonOk(!name.isEmpty());
}

void AddNamedAreaDialog::slotOk()
{
    const QString name = m_areaName->text();
    if (!name.isEmpty())
    {
        const QRect rect = m_pView->selection()->lastRange();
        if (!m_pView->doc()->namedAreaManager()->contains(name))
        {
            m_pView->doc()->emitBeginOperation(false);
            m_pView->doc()->namedAreaManager()->insert(m_pView->activeSheet(), rect, name);
            m_pView->slotUpdateView(m_pView->activeSheet());
            accept();
        }
        else
            KMessageBox::error(this, i18n("This name is already used."));
    }
    else
        KMessageBox::error(this, i18n("Area name is empty."));
}

#include "AddNamedAreaDialog.moc"
