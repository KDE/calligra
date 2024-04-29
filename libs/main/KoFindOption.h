/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOFINDOPTION_H
#define KOFINDOPTION_H

#include "komain_export.h"
#include <QObject>
#include <QSharedDataPointer>
#include <QVariant>

/**
 * \brief A single searching option.
 *
 * This class provides a generic interface for a single search option.
 * It is used by KoFindOptionSet for storing the options and should
 * usually not be created manually.
 *
 * \see KoFindOptionSet
 */
class KOMAIN_EXPORT KoFindOption : public QObject
{
    Q_OBJECT
    /**
     * The name of this option. Used to identify options.
     */
    Q_PROPERTY(QString name READ name CONSTANT)
    /**
     * The title of the option. Can be used for, for example, action names.
     */
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    /**
     * Description of what the option does. Can be used for, among others, tooltips.
     */
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    /**
     * The value of the option.
     */
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    /**
     * Constructor.
     *
     * This will create a new option with an empty title, description and value.
     *
     * \param name The name of this option.
     * \param parent The parent object.
     */
    explicit KoFindOption(const QString &name, QObject *parent = nullptr);
    /**
     * Destructor.
     */
    ~KoFindOption() override;

    /**
     * Retrieve this option's name.
     *
     * \return The name of this option.
     */
    QString name() const;

    /**
     * Retrieve the title of this option.
     *
     * \return The title of this option, or an empty string if none was set.
     */
    QString title() const;
    /**
     * Retrieve the description of this option.
     *
     * \return The description of this option, or an empty string if none was set.
     */
    QString description() const;
    /**
     * Retrieve the value of this option.
     *
     * \return The value of this option, or an invalid QVariant if none was set.
     */
    QVariant value() const;

public Q_SLOTS:
    /**
     * Set the title of this option.
     *
     * \param newTitle The title to set.
     */
    void setTitle(const QString &newTitle);
    /**
     * Set the description of this option.
     *
     * \param newDescription The description to set.
     */
    void setDescription(const QString &newDescription);
    /**
     * Set the value of this option.
     *
     * \param newValue The new value to set.
     */
    void setValue(const QVariant &newValue);

Q_SIGNALS:
    /**
     * Emitted when the title of the option changes.
     */
    void titleChanged();
    /**
     * Emitted when the description of the option changes.
     */
    void descriptionChanged();
    /**
     * Emitted when the value of the option changes.
     */
    void valueChanged();

private:
    class Private;
    Private *const d;
};

#endif // KOFINDOPTION_H
