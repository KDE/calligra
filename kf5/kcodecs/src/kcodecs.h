/*
   Copyright (C) 2000-2001 Dawit Alemayehu <adawit@kde.org>
   Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License (LGPL)
   version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

   The quoted-printable codec as described in RFC 2045, section 6.7. is by
   Rik Hemsley (C) 2001.
*/

#ifndef KCODECS_H
#define KCODECS_H

#define KBase64 KCodecs

#include <kcodecs_export.h>
#include <QtCore/QString>

class QByteArray;
class QIODevice;

/**
 * A wrapper class for the most commonly used encoding and
 * decoding algorithms.  Currently there is support for encoding
 * and decoding input using base64, uu and the quoted-printable
 * specifications.
 *
 * \b Usage:
 *
 * \code
 * QByteArray input = "Aladdin:open sesame";
 * QByteArray result = KCodecs::base64Encode(input);
 * cout << "Result: " << result.data() << endl;
 * \endcode
 *
 * <pre>
 * Output should be
 * Result: QWxhZGRpbjpvcGVuIHNlc2FtZQ==
 * </pre>
 *
 * The above example makes use of the convenience functions
 * (ones that accept/return null-terminated strings) to encode/decode
 * a string.  If what you need is to encode or decode binary data, then
 * it is highly recommended that you use the functions that take an input
 * and output QByteArray as arguments.  These functions are specifically
 * tailored for encoding and decoding binary data.
 *
 * @short A collection of commonly used encoding and decoding algorithms.
 * @author Dawit Alemayehu <adawit@kde.org>
 * @author Rik Hemsley <rik@kde.org>
 */
namespace KCodecs
{
  /**
   * Encodes the given data using the quoted-printable algorithm.
   *
   * @param in      data to be encoded.
   * @param useCRLF if true the input data is expected to have
   *                CRLF line breaks and the output will have CRLF line
   *                breaks, too.
   * @return        quoted-printable encoded string.
   */
  KCODECS_EXPORT QByteArray quotedPrintableEncode(const QByteArray & in,
                                        bool useCRLF = true);

  /**
   * Encodes the given data using the quoted-printable algorithm.
   *
   * Use this function if you want the result of the encoding
   * to be placed in another array which cuts down the number
   * of copy operation that have to be performed in the process.
   * This is also the preferred method for encoding binary data.
   *
   * NOTE: the output array is first reset and then resized
   * appropriately before use, hence, all data stored in the
   * output array will be lost.
   *
   * @param in      data to be encoded.
   * @param out     encoded data.
   * @param useCRLF if true the input data is expected to have
   *                CRLF line breaks and the output will have CRLF line
   *                breaks, too.
   */
  KCODECS_EXPORT void quotedPrintableEncode(const QByteArray & in, QByteArray& out,
                                    bool useCRLF);

  /**
   * Decodes a quoted-printable encoded data.
   *
   * Accepts data with CRLF or standard unix line breaks.
   *
   * @param in  data to be decoded.
   * @return    decoded string.
   */
  KCODECS_EXPORT QByteArray quotedPrintableDecode(const QByteArray & in);

  /**
   * Decodes a quoted-printable encoded data.
   *
   * Accepts data with CRLF or standard unix line breaks.
   * Use this function if you want the result of the decoding
   * to be placed in another array which cuts down the number
   * of copy operation that have to be performed in the process.
   * This is also the preferred method for decoding an encoded
   * binary data.
   *
   * NOTE: the output array is first reset and then resized
   * appropriately before use, hence, all data stored in the
   * output array will be lost.
   *
   * @param in   data to be decoded.
   * @param out  decoded data.
   */
  KCODECS_EXPORT void quotedPrintableDecode(const QByteArray & in, QByteArray& out);


  /**
   * Encodes the given data using the uuencode algorithm.
   *
   * The output is split into lines starting with the number of
   * encoded octets in the line and ending with a newline.  No
   * line is longer than 45 octets (60 characters), excluding the
   * line terminator.
   *
   * @param in   data to be uuencoded
   * @return     uuencoded string.
   */
  KCODECS_EXPORT QByteArray uuencode( const QByteArray& in );

  /**
   * Encodes the given data using the uuencode algorithm.
   *
   * Use this function if you want the result of the encoding
   * to be placed in another array and cut down the number of
   * copy operation that have to be performed in the process.
   * This is the preffered method for encoding binary data.
   *
   * NOTE: the output array is first reset and then resized
   * appropriately before use, hence, all data stored in the
   * output array will be lost.
   *
   * @param in   data to be uuencoded.
   * @param out  uudecoded data.
   */
  KCODECS_EXPORT void uuencode( const QByteArray& in, QByteArray& out );

  /**
   * Decodes the given data using the uudecode algorithm.
   *
   * Any 'begin' and 'end' lines like those generated by
   * the utilities in unix and unix-like OS will be
   * automatically ignored.
   *
   * @param in   data to be decoded.
   * @return     decoded string.
   */
  KCODECS_EXPORT QByteArray uudecode( const QByteArray& in );

  /**
   * Decodes the given data using the uudecode algorithm.
   *
   * Use this function if you want the result of the decoding
   * to be placed in another array which cuts down the number
   * of copy operation that have to be performed in the process.
   * This is the preferred method for decoding binary data.
   *
   * Any 'begin' and 'end' lines like those generated by
   * the utilities in unix and unix-like OS will be
   * automatically ignored.
   *
   * NOTE: the output array is first reset and then resized
   * appropriately before use, hence, all data stored in the
   * output array will be lost.
   *
   * @param in   data to be decoded.
   * @param out  uudecoded data.
   */
  KCODECS_EXPORT void uudecode( const QByteArray& in, QByteArray& out );


  /**
   * Encodes the given data using the base64 algorithm.
   *
   * The boolean argument determines if the encoded data is
   * going to be restricted to 76 characters or less per line
   * as specified by RFC 2045.  If @p insertLFs is true, then
   * there will be 76 characters or less per line.
   *
   * @param in         data to be encoded.
   * @param insertLFs  limit the number of characters per line.
   *
   * @return           base64 encoded string.
   */
  KCODECS_EXPORT QByteArray base64Encode( const QByteArray& in, bool insertLFs = false);

  /**
   * Encodes the given data using the base64 algorithm.
   *
   * Use this function if you want the result of the encoding
   * to be placed in another array which cuts down the number
   * of copy operation that have to be performed in the process.
   * This is also the preferred method for encoding binary data.
   *
   * The boolean argument determines if the encoded data is going
   * to be restricted to 76 characters or less per line as specified
   * by RFC 2045.  If @p insertLFs is true, then there will be 76
   * characters or less per line.
   *
   * NOTE: the output array is first reset and then resized
   * appropriately before use, hence, all data stored in the
   * output array will be lost.
   *
   * @param in        data to be encoded.
   * @param out       encoded data.
   * @param insertLFs limit the number of characters per line.
   */
  KCODECS_EXPORT void base64Encode( const QByteArray& in, QByteArray& out,
                            bool insertLFs = false );

  /**
   * Decodes the given data that was encoded using the
   * base64 algorithm.
   *
   * @param in   data to be decoded.
   * @return     decoded string.
   */
  KCODECS_EXPORT QByteArray base64Decode( const QByteArray& in );

  /**
   * Decodes the given data that was encoded with the base64
   * algorithm.
   *
   * Use this function if you want the result of the decoding
   * to be placed in another array which cuts down the number
   * of copy operation that have to be performed in the process.
   * This is also the preferred method for decoding an encoded
   * binary data.
   *
   * NOTE: the output array is first reset and then resized
   * appropriately before use, hence, all data stored in the
   * output array will be lost.
   *
   * @param in   data to be decoded.
   * @param out  decoded data.
   */
  KCODECS_EXPORT void base64Decode( const QByteArray& in, QByteArray& out );


  /**
   * Decodes string @p text according to RFC2047,
   * i.e., the construct =?charset?[qb]?encoded?=
   *
   * Note: a more rubust version of this function is included in kdepimlibs/libkmime
   *
   * @param text source string
   * @returns the decoded string
   */
  KCODECS_EXPORT QString decodeRFC2047String(const QString &text);


}

#endif // KCODECS_H
