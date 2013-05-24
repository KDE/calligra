/* This file is part of the KDE project
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXI_LLONGVALIDATOR_H
#define KEXI_LLONGVALIDATOR_H

#include "kexiutils_export.h"

#include <QValidator>
class QWidget;

namespace KexiUtils
{

//! @short A validator for longlong data type.
/*!
  This can be used by QLineEdit or subclass to provide validated
  text entry.  Can be provided with a base value (default is 10), to allow
  the proper entry of hexadecimal, octal, or any other base numeric data.

  Based on KIntValidator code by Glen Parker <glenebob@nwlink.com>
*/
class KEXIUTILS_EXPORT LongLongValidator : public QValidator
{
public:
    LongLongValidator(QWidget * parent, int base = 10);
    LongLongValidator(qint64 bottom, qint64 top, QWidget * parent, int base = 10);
    virtual ~LongLongValidator();

    //! Validates the text, and returns the result.  Does not modify the parameters.
    virtual State validate(QString &, int &) const;

    //! Fixes the text if possible, providing a valid string.  The parameter may be modified.
    virtual void fixup(QString &) const;

    //! Sets the minimum and maximum values allowed.
    virtual void setRange(qint64 bottom, qint64 top);

    //! Sets the numeric base value.
    virtual void setBase(int base);

    //! \return the current minimum value allowed
    virtual qint64 bottom() const;

    //! \return the current maximum value allowed
    virtual qint64 top() const;

    //! \return the current numeric base
    virtual int base() const;

private:

    class Private;

    Private* const d;
};

}
#endif
