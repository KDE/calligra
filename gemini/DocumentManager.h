/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QObject>
#include <QVariantMap>

#include <KWDocument.h>

class RecentFileManager;
class Settings;
class ProgressProxy;
class KoPart;
class DocumentManager : public QObject
{
    Q_OBJECT
public:
    KoDocument *document() const;
    Q_INVOKABLE QObject *doc() const;
    KoPart *part(const QString &type = WORDS_MIME_TYPE);
    ProgressProxy *progressProxy() const;
    Settings *settingsManager() const;
    void setSettingsManager(Settings *newManager);
    RecentFileManager *recentFileManager() const;
    bool isTemporaryFile() const;

    Q_INVOKABLE void setDocAndPart(KoDocument *document, KoPart *part);

public Q_SLOTS:
    void newDocument(int width, int height, float resolution);
    void newDocument(const QVariantMap &options);
    void openDocument(const QString &document, bool import = false);
    void closeDocument();
    bool save();
    void saveAs(const QString &filename, const QString &mimetype);
    void reload();
    void setTemporaryFile(bool temp);

    static DocumentManager *instance();

Q_SIGNALS:
    void documentChanged();
    void aboutToDeleteDocument();
    void documentSaved();

private:
    explicit DocumentManager(QObject *parent = nullptr);
    ~DocumentManager() override;

    class Private;
    Private *const d;

    static DocumentManager *sm_instance;

private Q_SLOTS:
    void delayedNewDocument();
    void delayedSaveAs();
    void delayedOpenDocument();
};

#endif // DOCUMENTMANAGER_H
