/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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

#ifndef KEXIDSWELCOME_H
#define KEXIDSWELCOME_H

#include <qwidget.h>

class KexiDataSourceWizard;

/**
 * This page is part of the KexiDataSourceWizard
 * it is the greeting page per default, where people
 * can choose whether they want to use the wizard or not.
 */
class KEXIEXTWIDGETS_EXPORT KexiDSWelcome : public QWidget
{
    Q_OBJECT

public:
    KexiDSWelcome(KexiDataSourceWizard *parent);
    ~KexiDSWelcome();

protected slots:
    void setUseWizard(bool use);

private:
    KexiDataSourceWizard *m_wiz;
};

#endif

