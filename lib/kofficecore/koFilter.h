/* This file is part of the KOffice libraries
   Copyright (C) 2001 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __koffice_filter_h__
#define __koffice_filter_h__

#include <qobject.h>
#include <qmap.h>

class QIODevice;
class KoFilterChain;

/**
 * The base class for import and export filters.
 *
 * Derive your filter class from this base class and implement
 * the @ref convert() method. Don't forget to specify the Q_OBJECT
 * macro in your class even if you don't use signals nor slots.
 * This is needed as filters are created on the fly.
 * The m_chain member allows access to the @ref KoFilterChain
 * which invokes the filter to query for input/output.
 *
 * Take care: The m_chain pointer is invalid while the constructor
 * runs due to the implementation -- @em don't use it in the constructor.
 * After the constructor, when running the @ref convert() method it's
 * guaranteed to be valid, so no need to check against 0.
 *
 * @author Werner Trobin <trobin@kde.org>
 */
class KoFilter : public QObject
{
    Q_OBJECT

    friend class KoFilterEntry;  // needed for the filter chain pointer :(

public:
    /**
     * This enum is used to signal the return state of your filter.
     * Return OK in @ref convert() in case everything worked as expected.
     * Feel free to add some more error conditions @em before the last item
     * if it's needed.
     */
    enum ConversionStatus { OK, StupidError, UsageError, CreationError, FileNotFound,
                            StorageCreationError, BadMimeType, BadConversionGraph,
                            EmbeddedDocError, WrongFormat, NotImplemented,
                            ParsingError, InternalError, UnexpectedEOF,
                            UnexpectedOpcode, UserCancelled,
                            JustInCaseSomeBrokenCompilerUsesLessThanAByte = 255 };

    virtual ~KoFilter();

    /**
     * The filter chain calls this method to perform the actual conversion.
     * The passed mimetypes should be a pair of those you specified in your
     * .desktop file.
     * You @em have to implement this method to make the filter work.
     * @param from The mimetype of the source file/document
     * @param to The mimetype of the destination file/document
     * @return The error status, see the @ref ConversionStatus enum.
     *         KoFilter::OK means that everything is allright.
     */
    virtual ConversionStatus convert( const QCString& from, const QCString& to ) = 0;

signals:
    /**
     * Emit this signal with a value in the range of 1...100 to have some
     * progress feedback for the user in the statusbar of the application.
     * @param value The actual progress state. Should always remain in
     * the range 1..100.
     */
    void sigProgress( int value );

protected:
    /**
     * This is the constructor your filter has to call, obviously.
     */
    KoFilter();

    /**
     * Use this pointer to access all information about input/output
     * during the conversion. @em Don't use it in the constructor -
     * it's invalid while constructing the object!
     */
    KoFilterChain* m_chain;

private:
    KoFilter( const KoFilter& rhs );
    KoFilter& operator=( const KoFilter& rhs );
};


// Note: Template method pattern
// For all *import* filters embedding parts or images.
class KoEmbeddingFilter : public KoFilter
{
    Q_OBJECT

public:
    virtual ~KoEmbeddingFilter();

    // The index of the currently processed part
    int currentPart() const;

    struct PartEntry
    {
        QString link;
        QString mimetype;
    };

protected:
    KoEmbeddingFilter();

    // returns the key for the part entry
    // to.isEmpty() -> nearest part, like for exp0rt
    int embedPart( const QCString& from, QCString& to,
                   KoFilter::ConversionStatus& status );

private:
    KoEmbeddingFilter( const KoEmbeddingFilter& rhs );
    KoEmbeddingFilter& operator=( const KoEmbeddingFilter& rhs );

    // method related to embedPart()
    // Save the contents to this (already opened) file
    virtual void savePartContents( QIODevice* file );

    // maps keys to part entries
    QMap<int, PartEntry> m_partMap;
    int m_currentPart;
};

#endif
