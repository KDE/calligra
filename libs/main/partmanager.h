/* This file is part of the KDE project
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
             (C) 1999 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef __kpartmanager_h__
#define __kpartmanager_h__

#include <QtGui/QWidget>

#include <komain_export.h>

class KComponentData;
class KXMLGUIClient;

namespace KoParts
{

class Part;

class PartManagerPrivate;

/**
 * The part manager is an object which knows about a collection of parts
 * (even nested ones) and handles activation/deactivation.
 *
 * Applications that want to embed parts without merging GUIs
 * only use a KoParts::PartManager. Those who want to merge GUIs use a
 * KoParts::MainWindow for example, in addition to a part manager.
 *
 * Parts know about the part manager to add nested parts to it.
 * See also KoParts::Part::manager() and KoParts::Part::setManager().
 */
class KOMAIN_EXPORT PartManager : public QObject
{
    Q_OBJECT
public:

    /**
     * Constructs a part manager.
     *
     * @param parent The toplevel widget (window / dialog) the
     *               partmanager should monitor for activation/selection
     *               events
     */
    PartManager( QWidget * parent );

    virtual ~PartManager();

    /**
     * @internal
     */
    virtual bool eventFilter( QObject *obj, QEvent *ev );

    /**
     * Adds a part to the manager.
     *
     * Sets it to the active part automatically if @p setActive is true (default ).
     * Behavior fix in KDE3.4: the part's widget is shown only if setActive is true,
     * it used to be shown in all cases before.
     */
    virtual void addPart( Part *part);

    /**
     * Removes a part from the manager (this does not delete the object) .
     *
     * Sets the active part to 0 if @p part is the activePart() .
     */
    virtual void removePart( Part *part );

    /**
     * Sets the active part.
     *
     * The active part receives activation events.
     *
     * @p widget can be used to specify which widget was responsible for the activation.
     * This is important if you have multiple views for a document/part , like in KOffice .
     */
    virtual void setActivePart( Part *part, QWidget *widget = 0 );

    /**
     * Returns the active widget of the current active part (see activePart ).
     */
    virtual QWidget *activeWidget() const;


Q_SIGNALS:

    /**
     * Emitted when the active part has changed.
     * @see setActivePart()
     **/
    void activePartChanged( KXMLGUIClient *newPart );

private Q_SLOTS:
    /**
     * Removes a part when it is destroyed.
     **/
    void slotObjectDestroyed();

    /**
     * @internal
     */
    void slotWidgetDestroyed();

    /**
     * @internal
     */
    void slotManagedTopLevelWidgetDestroyed();

private:
    Part * findPartFromWidget( QWidget * widget, const QPoint &pos );
    Part * findPartFromWidget( QWidget * widget );

    /**
     * Sets the selected part.
     *
     * The selected part receives selection events.
     *
     * @p widget can be used to specify which widget was responsible for the selection.
     * This is important if you have multiple views for a document/part , like in KOffice .
     */
    virtual void setSelectedPart( Part *part, QWidget *widget = 0 );


private:
    PartManagerPrivate* const d;
};

}

#endif

