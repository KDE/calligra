/* This file is part of the KDE libraries
   Copyright (C) 2000 David Faure <faure@kde.org>
   Copyright (C) 2000 Dawit Alemayehu <adawit@kde.org>
   Copyright (C) 2007 Olivier Goffart <ogoffart at kde.org>
   Copyright (C) 2013 Jarosław Staniek <staniek@kde.org>

   Based on kpasswordwidget.h from kdelibs

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; see the file COPYING.LIB.  If
   not, write to the Free Software Foundation, Inc., 51 Franklin Street,
   Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef KEXIPASSWORDWIDGET_H
#define KEXIPASSWORDWIDGET_H

#include <kexi_export.h>

#include <QWidget>

/**
 * A widget for requesting a password and optionaly a login from the end user.
 *
 * Requesting a simple password, asynchronous
 */
class KEXIEXTWIDGETS_EXPORT KexiPasswordWidget : public QWidget
{
    Q_OBJECT

public:
    enum KexiPasswordWidgetFlag
    {
        NoFlags = 0x00,
        /**
         * If this flag is set, the "keep this password" checkbox will been shown,
         * otherwise, it will not be shown and keepPassword will have no effect.
         */
        ShowKeepPassword = 0x01,
        /**
         * If this flag is set, there will be an additional line to let the user enter his login.
         */
        ShowUsernameLine = 0x02,
        /**
         * If this flag is set, the login lineedit will be in read only mode.
         */
        UsernameReadOnly = 0x04,
        /**
         * If this flag is set, the Anonymous Login checkbox will be displayed
         */
        ShowAnonymousLoginCheckBox = 0x08,
        /**
         * If this flag is set, there will be an additional line to let the user enter the database name.
         */
        ShowDatabaseNameLine = 0x10,
        /**
         * If this flag is set, the database name lineedit will be in read only mode.
         */
        DatabaseNameReadOnly = 0x20,
        /**
         * If this flag is set, there will be an additional line to let the user enter the domain.
         */
        ShowDomainLine = 0x40,
        /**
         * If this flag is set, the domain lineedit will be in read only mode.
         */
        DomainReadOnly = 0x80,
        /**
         * If this flag is set, the icon is shown.
         */
        ShowIcon = 0x100
    };
    Q_DECLARE_FLAGS(KexiPasswordWidgetFlags, KexiPasswordWidgetFlag)

    enum ErrorType
    {
        UnknownError = 0,

        /**
         * A problem with the user name as entered
         **/
        UsernameError,

        /**
         * Incorrect password
         */
        PasswordError,

        /**
         * Error preventing further attempts, will result in disabling most of the interface
         */
        FatalError,

        /**
         * A problem with the domain as entered
         **/
        DomainError
    };

    /**
     * create a password widget
     *
     * @param parent the parent widget (default 0).
     * @param flags a set of KexiPasswordWidgetFlag flags
     */
    explicit KexiPasswordWidget(QWidget *parent = 0L,
                                const KexiPasswordWidgetFlags& flags = NoFlags);

    /**
     * Destructor
     */
    ~KexiPasswordWidget();

    /**
     * Sets the prompt to show to the user.
     * @param prompt        instructional text to be shown.
     */
    void setPrompt(const QString& prompt);

    /**
     * Returns the prompt
     */
    QString prompt() const;

    /**
     * set an image that appears next to the prompt.
     */
    void setPixmap(const QPixmap&);
    /**
     *
     */
    QPixmap pixmap() const;

    /**
     * Adds a comment line to the widget.
     *
     * This function allows you to add one additional comment
     * line to this widget.  Calling this function after a
     * comment has already been added will not have any effect.
     *
     * @param label       label for comment (ex:"Command:")
     * @param comment     the actual comment text.
     */
    void addCommentLine(const QString& label, const QString& comment);

    /**
     * Shows an error message in the widget.
     *
     * @param message the error message to show
     */
    void showErrorMessage(const QString& message, const ErrorType type = PasswordError);

    /**
     * Returns the password entered by the user.
     * @return the password
     */
    QString password() const;

    /**
     * set the default username.
     */
    void setUsername(const QString&);

    /**
     * Returns the username entered by the user.
     * @return the user name
     */
    QString username() const;

    /**
     * set the default domain.
     */
    void setDomain(const QString&);

    /**
     * Returns the domain entered by the user.
     * @return the domain name
     */
    QString domain() const;

    /**
     * set anonymous mode (all other fields will be grayed out)
     */
    void setAnonymousMode(bool anonymous);

    /**
     * @return anonymous mode has been selected.
     */
    bool anonymousMode() const;

    /**
     * Determines whether supplied authorization should
     * persist even after the application has been closed.
     *
     * this is set with the check password checkbox is the ShowKeepCheckBox flag
     * is set in the constructor, if it is not set, this function return false
     *
     * @return true to keep the password
     */
    bool keepPassword() const;

    /**
     * Check or uncheck the "keep password" checkbox.
     * This can be used to check it before showing the widget, to tell
     * the user that the password is stored already (e.g. in the wallet).
     * enableKeep must have been set to true in the constructor.
     *
     * has only effect if ShowKeepCheckBox is set in the constructor
     */
    void setKeepPassword(bool b);

    /**
     * Sets the username field read-only and sets the
     * focus to the password field.
     *
     * This can also be set by passing UsernameReadOnly as flag in the constructor.
     *
     * @param readOnly true to set the user field to read-only
     */
    void setUsernameReadOnly(bool readOnly);

    /**
     * Shows the database name field.
     */
    void showDatabaseName(bool show);

    /**
     * Sets the database name field read-only.
     *
     * This can also be set by passing DatabaseNameReadOnly as flag in the constructor.
     *
     * @param readOnly true to set the database name field to read-only
     */
    void setDatabaseNameReadOnly(bool readOnly);

    /**
     * Set the database name.
     */
    void setDatabaseName(const QString& databaseName);

    /**
     * Presets the password.
     * @param password the password to set
     */
    void setPassword(const QString& password);

    /**
     * Presets a number of login+password pairs that the user can choose from.
     * The passwords can be empty if you simply want to offer usernames to choose from.
     *
     * This require the flag ShowUnernameLine to be set in the constructoe, and not the flag UsernameReadOnly
     * @param knownLogins map of known logins: the keys are usernames, the values are passwords.
     */
    void setKnownLogins(const QMap<QString, QString>& knownLogins);

Q_SIGNALS:
    /**
     * emitted when the widget has been accepted
     * @param password  the entered password
     * @param keep true if the "remember password" checkbox was checked, false otherwise.  false if ShowKeepPassword was not set in the constructor
     */
    void gotPassword(const QString& password, bool keep);

    /**
     * emitted when the widget has been accepted, and ShowUsernameLine was set on the constructor
     * @param username the entered username
     * @param password  the entered password
     * @param keep true if the "remember password" checkbox was checked, false otherwise.  false if ShowKeepPassword was not set in the constructor
     */
    void gotUsernameAndPassword(const QString& username, const QString& password, bool keep);

    /**
     * emitted when the user presses the return key on the widget
     */
    void returnPressed();

protected:
    /**
     * Virtual function that can be overridden to provide password
     * checking in derived classes. It should return @p true if the
     * password is valid, @p false otherwise.
     */
    virtual bool checkPassword();

private:
    Q_PRIVATE_SLOT(d, void activated( const QString& userName ))
    Q_PRIVATE_SLOT(d, void updateFields())

private:
    class KexiPasswordWidgetPrivate;
    friend class KexiPasswordWidgetPrivate;
    KexiPasswordWidgetPrivate* const d;

    Q_DISABLE_COPY(KexiPasswordWidget)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KexiPasswordWidget::KexiPasswordWidgetFlags)

#endif
