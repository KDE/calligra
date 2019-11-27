/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
    Q_PROPERTY(QObject* theme READ theme NOTIFY themeChanged)
    Q_PROPERTY(QString themeID READ themeID WRITE setThemeID NOTIFY themeChanged)

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

    QObject* theme() const;

    QString themeID() const;
    void setThemeID(const QString& id);

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
    Private* const d;
};

#endif // SETTINGS_H
