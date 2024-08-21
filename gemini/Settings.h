/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QQuickItem>
#include <QString>

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentFile READ currentFile WRITE setCurrentFile NOTIFY currentFileChanged)
    Q_PROPERTY(QString currentFileClass READ currentFileClass NOTIFY currentFileChanged)
    Q_PROPERTY(bool temporaryFile READ isTemporaryFile WRITE setTemporaryFile NOTIFY temporaryFileChanged)
    Q_PROPERTY(QQuickItem *focusItem READ focusItem WRITE setFocusItem NOTIFY focusItemChanged)
    Q_PROPERTY(QObject *theme READ theme NOTIFY themeChanged)
    Q_PROPERTY(QString themeID READ themeID WRITE setThemeID NOTIFY themeChanged)

public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings() override;

public Q_SLOTS:

    QString currentFile() const;
    QString currentFileClass() const;
    void setCurrentFile(const QString &fileName);

    bool isTemporaryFile() const;
    void setTemporaryFile(bool temp);

    QQuickItem *focusItem();
    void setFocusItem(QQuickItem *item);

    QObject *theme() const;

    QString themeID() const;
    void setThemeID(const QString &id);

    int mimeTypeToDocumentClass(QString mimeType) const;

Q_SIGNALS:
    void canvasChanged();
    void currentFileChanged();
    void temporaryFileChanged();
    void focusItemChanged();
    void themeChanged();
    void loadingFinished();

private:
    class Private;
    Private *const d;
};

#endif // SETTINGS_H
