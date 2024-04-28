/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QChar>
#include <QString>

/** Enum encoding all states of  mo's form attribute */
enum Form {
    Prefix /**< mo is a prefix*/,
    Infix /**< mo is a infix - used for all cases where it's not prefix or postfix*/,
    Postfix /**< mo is a postfix*/,
    InvalidForm
};

/**
 * @short Dictionary to look up operator attributes and entity mappings
 *
 * In section 3.2.5.2 of the MathML spec the numerous attributes for the mo element are
 * described. Further in section 3.2.5.7 the spec states that a MathML renderer should
 * have a dictionary to look up those attributes values. This class implements such a
 * dictionary by a script generated source file.
 * It also implements a lookup method for entities as MathML allows a number of them.
 * The method mapEntity maps a given "&...;" representation to the corresponding QChar.
 */
class Dictionary
{
public:
    /// Standard constructor sets default values to operator attributes
    Dictionary();

    /// @return The mapped entity represented as a QChar
    QChar mapEntity(const QString &entity);

    bool queryOperator(const QString &queriedOperator, Form form);

    /// @return The form value of the currently queried operator
    Form form() const;

    /// @return The fence value of the currently queried operator
    bool fence() const;

    /// @return The separator value of the currently queried operator
    bool separator() const;

    /// @return The lspace value of the currently queried operator
    QString lSpace() const;

    /// @return The rspace value of the currently queried operator
    QString rSpace() const;

    /// @return The stretchy value of the currently queried operator
    bool stretchy() const;

    /// @return The symmetric value of the currently queried operator
    bool symmetric() const;

    /// @return The maxsize value of the currently queried operator
    qreal maxSize() const;

    /// @return The minsize value of the currently queried operator
    qreal minSize() const;
    bool largeOp() const;
    bool moveableLimits() const;
    bool accent() const;

private:
    /// The queried operators form value
    Form m_form;

    /// The queried operators fence value
    bool m_fence;

    /// The queried operators separator value
    bool m_separator;

    /// The queried operators lspace value
    QString m_lspace;

    /// The queried operators rspace value
    QString m_rspace;

    /// The queried operators stretchy value
    bool m_stretchy;

    /// The queried operators symmetric value
    bool m_symmetric;

    /// The queried operators maxsize value
    QString m_maxsize;

    /// The queried operators minsize value
    QString m_minsize;

    /// The queried operators largeop value
    bool m_largeop;

    /// The queried operators movablelimits value
    bool m_movablelimits;

    /// The queried operators accent value
    bool m_accent;
};

#endif // DICTIONARY_H
