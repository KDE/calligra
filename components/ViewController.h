/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef CALLIGRA_COMPONENTS_VIEWCONTROLLER_H
#define CALLIGRA_COMPONENTS_VIEWCONTROLLER_H

#include <QtQuick/QQuickItem>

namespace Calligra {
namespace Components {

class View;

/**
 * \brief Provides an object to control the document transformation within a View.
 *
 */

class ViewController : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(View* view READ view WRITE setView NOTIFY viewChanged)
public:
    ViewController(QQuickItem* parent = 0);
    virtual ~ViewController();

    View* view() const;
    void setView(View* newView);

Q_SIGNALS:
    void viewChanged();

private:
    class Private;
    Private* const d;
};

} // Namespace Components
} // Namepsace Calligra

#endif // CALLIGRA_COMPONENTS_VIEWCONTROLLER_H
