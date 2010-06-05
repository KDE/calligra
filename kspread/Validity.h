/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002,2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Stefan Hetzl <shetzl@chello.at>
   Copyright 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999-2001 David Faure <faure@kde.org>
   Copyright 2000-2001 Werner Trobin <trobin@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1998-1999 Torben Weis <weis@kde.org>
   Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef KSPREAD_VALIDITY
#define KSPREAD_VALIDITY

// Qt
#include <QDate>
#include <QHash>
#include <QSharedDataPointer>
#include <QStringList>
#include <QTime>
#include <QVariant>

// KOffice
#include "kspread_export.h"

// KSpread
#include "Condition.h"

#include "KoXmlReaderForward.h"

namespace KSpread
{
class OdfLoadingContext;

/**
 * \class Validity
 * \ingroup Value
 *
 * Validates cell contents.
 *
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 */
class KSPREAD_EXPORT Validity
{
public:
    /// The action invoked, if the validity check fails.
    enum Action {
        Stop,       ///< Stop
        Warning,    ///< Warn
        Information ///< Inform
    };
    /// The type of the restriction.
    enum Restriction {
        None,       ///< No restriction
        Number,     ///< Restrict to numbers
        Text,       ///< Restrict to texts
        Time,       ///< Restrict to times
        Date,       ///< Restrict to dates
        Integer,    ///< Restrict to integers
        TextLength, ///< Restrict text length
        List        ///< Restrict to lists
    };

    /**
     * Constructor.
     * Creates a validity check, that allows any content.
     */
    Validity();

    /**
     * Copy Constructor.
     * Copies the validity \p other .
     */
    Validity(const Validity& other);

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
    bool testValidity(const Cell* cell) const;

    /**
     * \ingroup NativeFormat
     * Loads validity checks.
     */
    bool loadXML(Cell* const cell, const KoXmlElement& validityElement);

    /**
     * \ingroup NativeFormat
     * Saves validity checks.
     */
    QDomElement saveXML(QDomDocument& doc) const;

    /**
     * \ingroup OpenDocument
     * Loads validity checks.
     */
    void loadOdfValidation(Cell* const cell, const QString& validationName,
                           OdfLoadingContext& tableContext);

    Action action() const;
    bool allowEmptyCell() const;
    Conditional::Type condition() const;
    bool displayMessage() const;
    bool displayValidationInformation() const;
    const QString& messageInfo() const;
    const QDate& maximumDate() const;
    const QTime& maximumTime() const;
    double maximumValue() const;
    const QString& message() const;
    const QDate& minimumDate() const;
    const QTime& minimumTime() const;
    double minimumValue() const;
    Restriction restriction() const;
    const QString& title() const;
    const QString& titleInfo() const;
    const QStringList& validityList() const;

    void setAction(Action action);
    void setAllowEmptyCell(bool allow);
    void setCondition(Conditional::Type condition);
    void setDisplayMessage(bool display);
    void setDisplayValidationInformation(bool display);
    void setMaximumDate(const QDate& date);
    void setMaximumTime(const QTime& time);
    void setMaximumValue(double value);
    void setMessage(const QString& message);
    void setMessageInfo(const QString& info);
    void setMinimumDate(const QDate& date);
    void setMinimumTime(const QTime& time);
    void setMinimumValue(double value);
    void setRestriction(Restriction restriction);
    void setTitle(const QString& title);
    void setTitleInfo(const QString& info);
    void setValidityList(const QStringList& list);

    /// \note fake implementation to make QMap happy
    bool operator<(const Validity&) const {
        return true;
    }
    void operator=(const Validity&);
    bool operator==(const Validity& other) const;
    inline bool operator!=(const Validity& other) const {
        return !operator==(other);
    }

    static QHash<QString, KoXmlElement> preloadValidities(const KoXmlElement& body);

private:
    /**
     * \ingroup OpenDocument
     * Helper method for loadOdfValidation().
     */
    void loadOdfValidationCondition(QString &valExpression);

    /**
     * \ingroup OpenDocument
     * Helper method for loadOdfValidation().
     */
    void loadOdfValidationValue(const QStringList &listVal);

    class Private;
    QSharedDataPointer<Private> d;
};

} // namespace KSpread

Q_DECLARE_METATYPE(KSpread::Validity)
Q_DECLARE_TYPEINFO(KSpread::Validity, Q_MOVABLE_TYPE);

#endif // KSPREAD_VALIDITY
