/* This file is part of the KDE project
   Copyright (C) 2003-2014 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXISTARTUPFILEHANDLER_H
#define KEXISTARTUPFILEHANDLER_H

#include <kexi_export.h>

#include <QSet>
#include <QObject>

class QUrl;
class KFileDialog;
class KUrlRequester;
class KexiContextMessage;

//! @short Handler for use with file Kexi-specific opening/saving widgets/dialogs
/*! Make sure this object is destroyed before the handled file dialog
    or URL requester, otherwise recent dirs information will not be updated.
    Alternativelly you can call saveRecentDir(). */
class KEXIEXTWIDGETS_EXPORT KexiStartupFileHandler : public QObject
{
    Q_OBJECT

public:
    /*! Dialog mode:
    - Opening opens existing database (or shortcut)
    - SavingFileBasedDB saves file-based database file
    - SavingServerBasedDB saves server-based (shortcut) file
    - CustomOpening can be used for opening other files, like CSV
    */
    enum ModeFlag {
        Opening = 1,
        SavingFileBasedDB = 2,
        SavingServerBasedDB = 4,
        Custom = 256
    };
    Q_DECLARE_FLAGS(Mode, ModeFlag)

    KexiStartupFileHandler(
        const QUrl &startDirOrVariable, Mode mode, KFileDialog *dialog);

    KexiStartupFileHandler(
        const QUrl &startDirOrVariable, Mode mode, KUrlRequester *requester);

    virtual ~KexiStartupFileHandler();

    /*! Helper. Displays "This file already exists. Do you want to overwrite it?"
     message.
    @return true if @a filePath file does not exists or user has agreed on overwriting,
    false in user do not want to overwrite. Blocks until user 'answers'
    by to signal askForOverwriting(const KexiContextMessage&)
    by triggering 'yes' or 'no' action. */
    bool askForOverwriting(const QString& filePath);

    //! @return mode for the handler.
    Mode mode() const;

    //! Sets mode for the handler.
    void setMode(Mode mode);

    QSet<QString> additionalFilters() const;

    //! Sets additional filters list, e.g. "text/x-csv"
    void setAdditionalFilters(const QSet<QString>& mimeTypes);

    QSet<QString> excludedFilters() const;

    //! Excludes filters list
    void setExcludedFilters(const QSet<QString>& mimeTypes);

    void setLocationText(const QString& fn);

    //! Sets default extension which will be added after accepting
    //! if user didn't provided one. This method is usable when there is
    //! more than one filter so there is no rule what extension should be selected
    //! (by default first one is selected).
    void setDefaultExtension(const QString& ext);

    /*! \return true if the current URL meets requies constraints
    (i.e. exists or doesn't exist);
    shows appropriate message box if needed. */
    bool checkSelectedUrl();

    /*! If true, user will be asked to accept overwriting existing file.
    This is true by default. */
    void setConfirmOverwrites(bool set);


    //! Updates the requested URL based on specified name. Performs any necessary character conversions.
    void updateUrl(const QString &name);
Q_SIGNALS:
    void askForOverwriting(const KexiContextMessage& message);

protected Q_SLOTS:
    void slotAccepted();
    void saveRecentDir();
    
    void messageWidgetActionYesTriggered();
    void messageWidgetActionNoTriggered();

private:
    void init(const QUrl &startDirOrVariable, Mode mode);
    void updateFilters();

    class Private;
    Private * const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KexiStartupFileHandler::Mode)

#endif
