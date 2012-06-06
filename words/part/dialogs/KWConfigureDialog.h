/* This file is part of the KDE project
   Copyright (C) 2009 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef KWCONFIGUREDIALOG_H
#define KWCONFIGUREDIALOG_H

#include <kpagedialog.h>

class KWView;
class KoConfigDocumentPage;
class KoConfigGridPage;
class KoConfigMiscPage;
class KoConfigAuthorPage;

class KWConfigureDialog : public KPageDialog
{
    Q_OBJECT

public:
    KWConfigureDialog(KWView* parent);

public slots:
    void slotApply();
    void slotDefault();

private:
    KoConfigGridPage* m_gridPage;
    KoConfigDocumentPage* m_docPage;
    KoConfigMiscPage* m_miscPage;
    KoConfigAuthorPage *m_authorPage;

};

#endif // KWCONFIGUREDIALOG_H
