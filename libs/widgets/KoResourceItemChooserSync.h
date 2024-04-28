/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2014 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KORESOURCEITEMCHOOSERSYNC_H
#define KORESOURCEITEMCHOOSERSYNC_H

#include <QObject>
#include <QScopedPointer>

#include "kowidgets_export.h"

/**
 * KoResourceItemChooserSync is a singleton that sync the size of entries in the
 * resource item choosers between different choosers
 * To use the syncing it has to be turned on in the KoResourceItemChooser
 */
class KOWIDGETS_EXPORT KoResourceItemChooserSync : public QObject
{
    Q_OBJECT
public:
    KoResourceItemChooserSync();
    ~KoResourceItemChooserSync() override;
    static KoResourceItemChooserSync *instance();

    /// Gets the base length
    /// @returns the base length of items
    int baseLength();

    /// Set the base length
    /// @param length base length for the items, will be clamped if outside range
    void setBaseLength(int length);

Q_SIGNALS:
    /// Signal is emitted when the base length is changed and will trigger and update in
    /// the resource item choosers
    void baseLenghtChanged(int length);

private:
    KoResourceItemChooserSync(const KoResourceItemChooserSync &);
    KoResourceItemChooserSync operator=(const KoResourceItemChooserSync &);

private:
    struct Private;
    const QScopedPointer<Private> d;
};

#endif // KORESOURCEITEMCHOOSERSYNC_H
