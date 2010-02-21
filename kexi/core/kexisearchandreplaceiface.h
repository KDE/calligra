/* This file is part of the KDE project
   Copyright (C) 2007 Jarosław Staniek <staniek@kde.org>

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

#ifndef KexiSearchAndReplaceViewInterface_H
#define KexiSearchAndReplaceViewInterface_H

#include <kexi_export.h>
#include <kexiutils/tristate.h>
#include <qstring.h>
class QVariant;
class QStringList;

//! @short An interface used by Kexi views (KexiView) supporting search/replace features
class KEXICORE_EXPORT KexiSearchAndReplaceViewInterface
{
public:
    KexiSearchAndReplaceViewInterface();
    virtual ~KexiSearchAndReplaceViewInterface();

    //! @short Specifies options for find and replace operations.
    /*! A GUI for setting these options is provided by KexiFindDialog class. */
    class KEXICORE_EXPORT Options
    {
    public:
        Options();

        //! Special values for columnNumber.
        enum SpecialLookInValue {
            AllColumns = -1,   //!< "all columns" (the default)
            CurrentColumn = -2 //!< "current column"
        };
        //! Column number to look in, AllColumns means "all columns" (the default)
        //! and CurrentColumn means "current column".
        int columnNumber;

        //! Specifies possible options for text matching
        enum TextMatching {
            MatchAnyPartOfField = 0, //!< Matched text can be any part of field (the default)
            MatchWholeField = 1,     //!< Matched text must be the whole field
            MatchStartOfField = 2    //!< Matched text must be at the start of field
        };

        //! Specifies possible options for text matching
        TextMatching textMatching;

        //! Specifies search direction
        enum SearchDirection {
            SearchUp = 0,      //!< Search up (previous) from the current position
            SearchDown = 1,    //!< Search down (next) from the current position (the default)
            SearchAllRows = 2, //!< Search from the first to the last row
            DefaultSearchDirection = SearchDown //! Used to mark the default
        };

        //! Specifies search direction
        SearchDirection searchDirection;

        //! True for searching is case-sensitive (false by default)
    bool caseSensitive;

        //! True for searching for whole words only (false by default)
    bool wholeWordsOnly;

        //! True if question should be displayed before every replacement made (true by default)
    bool promptOnReplace;
    };

    /*! Sets up data for find/replace dialog, based on view's data model.
     \a columnNames should contain column name, \a columnCaptions should contain column captions,
     and \a currentColumnName should beset to current column's name.
     Implementation should set up values and return true if find/replace dialog should be filled. */
    virtual bool setupFindAndReplace(QStringList& columnNames, QStringList& columnCaptions,
                                     QString& currentColumnName) = 0;

    /*! Finds \a valueToFind within the view.
     \a options are used to control the process. Selection is moved to found value.
     \return true if value has been found, false if value has not been found,
     and cancelled if there is nothing to find or there is no data to search in.
     If \a next is true, "find next" is performed, else "find previous" is performed. */
    virtual tristate find(const QVariant& valueToFind,
                          const KexiSearchAndReplaceViewInterface::Options& options, bool next) = 0;

    /*! Finds \a valueToFind within the view and replaces with \a replacement
     \a options are used to control the process.
     \return true if value has been found and replaced, false if value
     has not been found and replaced, and cancelled if there is nothing
     to find or there is no data to search in or the data is read only.
     If \a replaceAll is true, all found values are replaced. */
    virtual tristate findNextAndReplace(const QVariant& valueToFind, const QVariant& replacement,
                                        const KexiSearchAndReplaceViewInterface::Options& options, bool replaceAll) = 0;
};

#endif
