/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2004 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 1999-2002, 2004 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002, 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2003 Stefan Hetzl <shetzl@chello.at>
   SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2002 Harri Porten <porten@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 1999-2001 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2000-2001 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org
   SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 1999 Michael Reiher <michael.reiher@gmx.de>
   SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_VALIDITY
#define CALLIGRA_SHEETS_VALIDITY

// Sheets
#include "Value.h"
#include "sheets_engine_export.h"

namespace Calligra
{
namespace Sheets
{
class CellBase;
class ValueConverter;
class ValueParser;

/**
 * \class Validity
 * \ingroup Value
 *
 * Validates cell contents.
 *
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT Validity
{
public:
    /// The action invoked, if the validity check fails.
    enum Action {
        Stop, ///< Stop
        Warning, ///< Warn
        Information ///< Inform
    };
    /// The type of the restriction.
    enum Restriction {
        NoRestriction, ///< No restriction
        Number, ///< Restrict to numbers
        Text, ///< Restrict to texts
        Time, ///< Restrict to times
        Date, ///< Restrict to dates
        Integer, ///< Restrict to integers
        TextLength, ///< Restrict text length
        List ///< Restrict to lists
    };

    enum Type { None, Equal, Superior, Inferior, SuperiorEqual, InferiorEqual, Between, Different, DifferentTo, IsTrueFormula };

    /**
     * Constructor.
     * Creates a validity check, that allows any content.
     */
    Validity();

    /**
     * Copy Constructor.
     * Copies the validity \p other .
     */
    Validity(const Validity &other);

    /**
     * Destructor.
     */
    ~Validity();

    /**
     * \return \c true if this validity check allows any content
     */
    bool isEmpty() const;

    /**
     * Tests whether the content of \p cell is allowed.
     * \return \c true if the content is valid
     */
    bool testValidity(const CellBase *cell) const;

    Action action() const;
    bool allowEmptyCell() const;
    Type condition() const;

    bool displayMessage() const;
    bool displayValidationInformation() const;
    const QString &messageInfo() const;
    const QString &message() const;

    const Value &maximumValue() const;
    const Value &minimumValue() const;

    Restriction restriction() const;
    const QString &title() const;
    const QString &titleInfo() const;
    const QStringList &validityList() const;

    void setAction(Action action);
    void setAllowEmptyCell(bool allow);
    void setCondition(Type condition);

    void setDisplayMessage(bool display);
    void setDisplayValidationInformation(bool display);
    void setMessage(const QString &message);
    void setMessageInfo(const QString &info);

    void setMaximumValue(const Value &value);
    void setMinimumValue(const Value &value);

    void setRestriction(Restriction restriction);
    void setTitle(const QString &title);
    void setTitleInfo(const QString &info);
    void setValidityList(const QStringList &list);

    /// \note fake implementation to make QMap happy
    bool operator<(const Validity &) const
    {
        return true;
    }
    void operator=(const Validity &);
    bool operator==(const Validity &other) const;
    inline bool operator!=(const Validity &other) const
    {
        return !operator==(other);
    }

private:
    class Private;
    QSharedDataPointer<Private> d;
};

} // namespace Sheets
} // namespace Calligra

Q_DECLARE_METATYPE(Calligra::Sheets::Validity)
Q_DECLARE_TYPEINFO(Calligra::Sheets::Validity, Q_MOVABLE_TYPE);

#endif // CALLIGRA_SHEETS_VALIDITY
