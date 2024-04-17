/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 2006 Fredrik Edemar <f_edemar@linux.se>

   $Id: KoViewIface.h 539508 2006-05-10 20:01:40Z mlaurent $

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef __KoViewAdaptor_h__
#define __KoViewAdaptor_h__

#ifndef QT_NO_DBUS

#include <QMap>
#include <QDBusAbstractAdaptor>

class QString;

#include "komain_export.h"

class KoView;

class KOMAIN_EXPORT KoViewAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.calligra.view")
public:
    explicit KoViewAdaptor(KoView *view);

    ~KoViewAdaptor() override;

public Q_SLOTS: // METHODS
    Q_SCRIPTABLE QStringList/*DCOPCStringList*/ actions();

protected:
    KoView *m_pView;

};

#endif // QT_NO_DBUS

#endif
