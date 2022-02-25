/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QString>
#include <QQuickItem>

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile NOTIFY currentFileChanged)
    Q_PROPERTY(QString currentFileClass READ currentFileClass NOTIFY currentFileChanged)
    Q_PROPERTY(bool temporaryFile READ isTemporaryFile WRITE setTemporaryFile NOTIFY temporaryFileChanged)
    Q_PROPERTY(QQuickItem* focusItem READ focusItem WRITE setFocusItem NOTIFY focusItemChanged)

public:
    explicit Settings( QObject* parent = 0);
    ~Settings() override;

public Q_SLOTS:

    QString currentFile() const;
    QString currentFileClass() const;
    void setCurrentFile(const QString &fileName);

    bool isTemporaryFile() const;
    void setTemporaryFile(bool temp);

    QQuickItem *focusItem();
    void setFocusItem(QQuickItem *item);

    int mimeTypeToDocumentClass(QString mimeType) const;

Q_SIGNALS:
    void canvasChanged();
    void currentFileChanged();
    void temporaryFileChanged();
    void focusItemChanged();
    void loadingFinished();

private:
    class Private;
    Private* const d;
};

#endif // SETTINGS_H
