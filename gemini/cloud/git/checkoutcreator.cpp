/* This file is part of the KDE project
 * Copyright 2014  Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "checkoutcreator.h"

#include <kfiledialog.h>

#include <QDir>
#include <QFileDialog>
#include <QDebug>

class CheckoutCreator::Private
{
public:
    Private()
    {}
};

CheckoutCreator::CheckoutCreator(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

CheckoutCreator::~CheckoutCreator()
{
    delete d;
}

QString CheckoutCreator::getFile(QString caption, QString filter) const
{
    QString url = KFileDialog::getOpenFileName(KUrl(QDir::homePath()), filter, 0, caption);
    return url;
}

QString CheckoutCreator::getDir() const
{
    QString url = QFileDialog::getExistingDirectory();
    return url;
}

bool CheckoutCreator::isGitDir(QString directory) const
{
    QDir dir(directory);
    if(dir.exists(".git/config"))
        return true;
    return false;
}

#include "checkoutcreator.moc"

