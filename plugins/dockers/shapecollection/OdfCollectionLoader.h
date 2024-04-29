/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Peter Simonsson <peter.simonsson@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOODFCOLLECTIONLOADER_H
#define KOODFCOLLECTIONLOADER_H

#include <KoXmlReader.h>

#include <QList>
#include <QObject>
#include <QStringList>

class KoOdfReadStore;
class KoOdfLoadingContext;
class KoShapeLoadingContext;
class QTimer;
class KoShape;

class OdfCollectionLoader : public QObject
{
    Q_OBJECT
public:
    explicit OdfCollectionLoader(const QString &path, QObject *parent = nullptr);
    ~OdfCollectionLoader() override;

    void load();

    QList<KoShape *> shapeList() const
    {
        return m_shapeList;
    }
    QString collectionPath() const
    {
        return m_path;
    }

protected:
    void nextFile();
    void loadNativeFile(const QString &path);

protected Q_SLOTS:
    void loadShape();

private:
    KoOdfReadStore *m_odfStore;
    QTimer *m_loadingTimer;
    KoOdfLoadingContext *m_loadingContext;
    KoShapeLoadingContext *m_shapeLoadingContext;
    KoXmlElement m_body;
    KoXmlElement m_page;
    KoXmlElement m_shape;
    QList<KoShape *> m_shapeList;
    QString m_path;
    QStringList m_fileList;

Q_SIGNALS:
    /**
     * Emitted when the loading failed
     * @param reason Reason the loading failed.
     */
    void loadingFailed(const QString &reason);

    void loadingFinished();
};

#endif // KOODFCOLLECTIONLOADER_H
