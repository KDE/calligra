/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOFORMULASHAPEPLUGIN_H
#define KOFORMULASHAPEPLUGIN_H

#include <QObject>
#include <QVariantList>

/**
 * @short A plugin for the formula shape and tool
 *
 * This class implements a formula plugin that is loadable by any flake supporting
 * Calligra application. It only contains a destructor and a constructor where the
 * latter has code in it. All functionality has to be in the constructor.
 * In the constructor the plugin registers a shape and a tool in the KoShapeRegistry
 * so that the applications "know" that the formula plugin exists.
 */
class KoFormulaShapePlugin : public QObject
{
    Q_OBJECT

public:
    KoFormulaShapePlugin(QObject *parent, const QVariantList &);
    ~KoFormulaShapePlugin() override;
};

#endif
