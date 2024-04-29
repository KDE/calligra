/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOFINDOPTIONSET_H
#define KOFINDOPTIONSET_H

#include "komain_export.h"
#include <QObject>

class KoFindOption;

/**
 * \brief A collection of search option that are supported by the backend.
 *
 * This class manages options for searching. Through this class, backends
 * can support a different set of options for searching. An instance of
 * this class can be retrieved from the backend by calling
 * KoFindBase::options(). The individual options can then be retrieved
 * from the KoFindOptions instance and used to populate the UI.
 */
class KOMAIN_EXPORT KoFindOptionSet : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor.
     *
     * Constructs an instance without any options.
     */
    explicit KoFindOptionSet(QObject *parent = nullptr);
    /**
     * Destructor.
     */
    ~KoFindOptionSet() override;

    /**
     * Retrieve a specific option.
     *
     * \param name The name of the option to retrieve.
     *
     * \return The option corresponding to the id, or 0 if it was not found.
     */
    KoFindOption *option(const QString &name) const;
    /**
     * Retrieve a list of all properties.
     *
     * \return A list of options.
     */
    QList<KoFindOption *> options() const;

    /**
     * Add an empty option.
     *
     * This will add an option with no title, description or value set.
     * You should set these values yourself before using the option.
     *
     * \param name A name to identify the option by.
     *
     * \return The new option.
     */
    KoFindOption *addOption(const QString &name);
    /**
     * Add a new option.
     *
     * \param name A name to identify the option by.
     * \param title The title of the option, for example "Case Sensitive".
     * \param description A description for the option, for example "Only generate a match if
     * the case of the possibly matched text matches that of the text to search for".
     * \param value The initial value of the option.
     *
     * \return The option just created.
     */
    KoFindOption *addOption(const QString &name, const QString &title, const QString &description, const QVariant &value);

    /**
     * Remove an option from the set.
     *
     * \param name The name of the option to remove.
     */
    void removeOption(const QString &name);

public Q_SLOTS:
    /**
     * Set the value of an option.
     *
     * \param name The name of the option to set.
     * \param value The value to set the option to.
     */
    void setOptionValue(const QString &name, const QVariant &value);
    /**
     * Replace an option with another one.
     *
     * \param name The name of the option to replace.
     * \param newOption The new option to replace the old option with.
     */
    void replaceOption(const QString &name, KoFindOption *newOption);

private:
    class Private;
    Private *const d;
};

#endif // KOFINDOPTIONSET_H
