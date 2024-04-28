/* Part of the Calligra project
 * SPDX-FileCopyrightText: 2019 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QMap>
#include <QObject>
#include <QString>

class CollectionItemModel;

class StencilBoxDockerLoader : public QObject
{
    Q_OBJECT
public:
    StencilBoxDockerLoader(StencilBoxDocker *parent)
        : q(parent)
    {
    }

    bool addCollection(const QString &path);

    QMap<QString, CollectionItemModel *> m_modelMap;

public Q_SLOTS:
    void loadShapeCollections();

Q_SIGNALS:
    void resultReady();

private:
    StencilBoxDocker *q;
};
