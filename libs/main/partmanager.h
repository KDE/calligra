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
    Q_ENUMS( SelectionPolicy )
    Q_PROPERTY( SelectionPolicy selectionPolicy READ selectionPolicy WRITE setSelectionPolicy )
    Q_PROPERTY( bool allowNestedParts READ allowNestedParts WRITE setAllowNestedParts )
    Q_PROPERTY( bool ignoreScrollBars READ ignoreScrollBars WRITE setIgnoreScrollBars )
public:
    /// Selection policy. The default policy of a PartManager is Direct.
    enum SelectionPolicy { Direct, TriState };

    /**
     * This extends QFocusEvent::Reason with the non-focus-event reasons for partmanager to activate a part.
     * To test for "any focusin reason", use < ReasonLeftClick
     * NoReason usually means: explicit activation with @ref setActivePart.
     */
    enum Reason { ReasonLeftClick = 100, ReasonMidClick, ReasonRightClick, NoReason };

    /**
     * Constructs a part manager.
     *
     * @param parent The toplevel widget (window / dialog) the
     *               partmanager should monitor for activation/selection
     *               events
     */
    PartManager( QWidget * parent );
    /**
     * Constructs a part manager.
     *
     * @param topLevel The toplevel widget (window / dialog ) the
     *                 partmanager should monitor for activation/selection
     *                 events
     * @param parent   The parent QObject.
     */
    PartManager( QWidget * topLevel, QObject *parent );
    virtual ~PartManager();

    /**
     * Sets the selection policy of the partmanager.
     */
    void setSelectionPolicy( SelectionPolicy policy );
    /**
     * Returns the current selection policy.
     */
    SelectionPolicy selectionPolicy() const;

    /**
     * Specifies whether the partmanager should handle/allow nested parts
     * or not.
     *
     *  This is a property the shell has to set/specify. Per
     * default we assume that the shell cannot handle nested
     * parts. However in case of a KOffice shell for example we allow
     * nested parts.  A Part is nested (a child part) if its parent
     * object inherits KoParts::Part.  If a child part is activated and
     * nested parts are not allowed/handled, then the top parent part in
     * the tree is activated.
     */
    void setAllowNestedParts( bool allow );
    /**
     * @see setAllowNestedParts
     */
    bool allowNestedParts() const;

    /**
     * Specifies whether the partmanager should ignore mouse click events for
     * scrollbars or not. If the partmanager ignores them, then clicking on the
     * scrollbars of a non-active/non-selected part will not change the selection
     * or activation state.
     *
     * The default value is false (read: scrollbars are NOT ignored).
     */
    void setIgnoreScrollBars( bool ignore );
    /**
     * @see setIgnoreScrollBars
     */
    bool ignoreScrollBars() const;

    /**
     * Specifies which mouse buttons the partmanager should react upon.
     * By default it reacts on all mouse buttons (LMB/MMB/RMB).
     * @param buttonMask a combination of Qt::ButtonState values e.g. Qt::LeftButton | Qt::MidButton
     */
    void setActivationButtonMask( short int buttonMask );
    /**
     * @see setActivationButtonMask
     */
    short int activationButtonMask() const;

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
    virtual void addPart( Part *part, bool setActive = true );

    /**
     * Removes a part from the manager (this does not delete the object) .
     *
     * Sets the active part to 0 if @p part is the activePart() .
     */
    virtual void removePart( Part *part );

    /**
     * Replaces @p oldPart with @p newPart, and sets @p newPart as active if
     * @p setActive is true.
     * This is an optimised version of removePart + addPart
     */
    virtual void replacePart( Part * oldPart, Part * newPart, bool setActive = true );

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
     * Returns the active part.
     **/
    virtual Part *activePart() const;

    /**
     * Returns the active widget of the current active part (see activePart ).
     */
    virtual QWidget *activeWidget() const;

    /**
     * Sets the selected part.
     *
     * The selected part receives selection events.
     *
     * @p widget can be used to specify which widget was responsible for the selection.
     * This is important if you have multiple views for a document/part , like in KOffice .
     */
    virtual void setSelectedPart( Part *part, QWidget *widget = 0 );

    /**
     * Returns the current selected part.
     */
    virtual Part *selectedPart() const;

    /**
     * Returns the selected widget of the current selected part (see selectedPart ).
     */
    virtual QWidget *selectedWidget() const;

    /**
     * Returns the list of parts being managed by the partmanager.
     */
    const QList<Part *> parts() const;

    /**
     * Adds the @p topLevel widget to the list of managed toplevel widgets.
     * Usually a PartManager only listens for events (for activation/selection)
     * for one toplevel widget (and its children) , the one specified in the
     * constructor. Sometimes however (like for example when using the KDE dockwidget
     * library) , it is necessary to extend this.
     */
    void addManagedTopLevelWidget( const QWidget *topLevel );
    /**
     * Removes the @p topLevel widget from the list of managed toplevel widgets.
     * @see addManagedTopLevelWidget
     */
    void removeManagedTopLevelWidget( const QWidget *topLevel );

    /**
     * @return the reason for the last activePartChanged signal emitted.
     * @see Reason
     */
    int reason() const;

Q_SIGNALS:
    /**
     * Emitted when a new part has been added.
     * @see addPart()
     **/
    void partAdded( KoParts::Part *part );
    /**
     * Emitted when a part has been removed.
     * @see removePart()
     **/
    void partRemoved( KoParts::Part *part );
    /**
     * Emitted when the active part has changed.
     * @see setActivePart()
     **/
    void activePartChanged( KoParts::Part *newPart );

protected:
    /**
     * Changes the active instance when the active part changes.
     * The active instance is used by KBugReport and KAboutDialog.
     * Override if you really need to - usually you don't need to.
     */
    virtual void setActiveComponent(const KComponentData &instance);

    /**
     * Sets whether the PartManager ignores explict set focus requests
     * from the part.
     *
     * By default this option is set to false. Set it to true to prevent
     * the part from sending explicit set focus requests to the client
     * application.
     *
     * @since 4.10
     */
    void setIgnoreExplictFocusRequests(bool);

protected Q_SLOTS:
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

private:
    PartManagerPrivate* const d;
};

}

#endif

