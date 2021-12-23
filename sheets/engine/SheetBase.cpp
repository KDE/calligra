/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "SheetBase.h"

#include <QObject>


using namespace Calligra::Sheets;

class Q_DECL_HIDDEN SheetBase::Private
{
public:
    Private(SheetBase *sheet);
    ~Private() {}
private:
    SheetBase *m_sheet;
};


SheetBase::Private::Private (SheetBase *sheet)
    : m_sheet(sheet)
{

}


SheetBase::SheetBase() : 
    d(new Private(this))
{
}

SheetBase::~SheetBase()
{
    delete d;
}

