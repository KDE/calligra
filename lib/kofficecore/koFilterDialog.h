/* This file is part of the KDE libraries
   Copyright (C) 2000 Werner Trobin <wtrobin@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __koffice_filter_dialog_h__
#define __koffice_filter_dialog_h__

#include <qwidget.h>

/**
 * This is an abstract base class for filter configuration dialogs.
 * Derive your dialog from this one. Create a dialog in the CTOR ("on top"
 * of this widget) and destroy it in the DTOR. KOffice will look for the
 * dialog (via the trader and service stuff) and display it in the file
 * dialog when appropriate. When the user changes the state (e.g. checks a
 * checkbox, enters a password in a lineedit,...) you have to react on
 * that action (at least in a way that you can "read" the state if your
 * implementation of state() gets called). After the user clicked 'Ok',
 * the state() method will be called. In this method you have to create a
 * QString and return it. This string will be passed to the CTOR of the
 * filter (as 'config' argument). Of course the format of this string is
 * up to you, but using XML (QDom/QXML) seems to be a good choice...
 * For further information see koffice/filters/HOWTO
 *
 * @ref KoFilterManager
 * @ref KoFilter
 *
 * @author Werner Trobin
 * @short An abstract base class for your filter dialogs
 */

class KoFilterDialog : public QWidget {

    Q_OBJECT

public:
    KoFilterDialog(QWidget *parent=0L, QString name=QString::null);
    virtual ~KoFilterDialog() {}
    virtual const QString state() = 0;
};
#endif
