/* This file is part of the KOffice project
   Copyright (C) 2011 Matus Uzak <matus.uzak@ixonos.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

const quint16 Word8nFib = 0x00c1;

/**
 * FibBase is the fixed size portion of the Fib structure.  
 *
 * The Fib structure contains information about the document and specifies the
 * file pointers to various portions that make up the document.
 *
 * According to [MS-DOC] — v20101219 Word (.doc) Binary File Format
 */
struct FibBase {
public:
    /**
     * Creates an empty FibBase structure and sets the defaults
     */
    FibBase();

    /**
     * Creates a FibBase structure, calls the read f.
     */
    FibBase(LEInputStream& in);

    /**
     * Read the FibBase part of the Fib and rewind the stream.
     */
    bool read(LEInputStream& in);

    /**
     * Set all variables to inital values (default is 0)
     */
    void clear();

    /**
     * An unsigned integer that specifies that this is a Word Binary File.
     * This value MUST be 0xA5EC.
     */
    quint16 wIdent;

    /**
     * An unsigned integer that specifies the version number of the file format
     * used.  Superseded by FibRgCswNew.nFibNew if it is present.
     */
    quint16 nFib;

    /**
     * This value is undefined and MUST be ignored.
     */
    quint16 unused;

    /**
     * A LID that specifies the install language of the application that is
     * producing the document.  If nFib is 0x00D9 or greater, then any East
     * Asian install lid or any install lid with a base language of Spanish,
     * German or French MUST be recorded as lidAmerican.  If the nFib is 0x0101
     * or greater, then any install lid with a base language of Vietnamese,
     * Thai, or Hindi MUST be recorded as lidAmerican.
     */
    quint16 lid;

    /**
     * An unsigned integer that specifies the offset in the WordDocument stream
     * of the FIB for the document which contains all the AutoText items.  If
     * this value is 0, there are no AutoText items attached.
     */
    quint16 pnNext;

    /**
     * Specifies whether this is a document template.
     */
    quint16 fDot:1;

    /**
     * Specifies whether this is a document that contains only AutoText items.
     */
    quint16 fGlsy:1;

    /**
     * Specifies that the last save operation that was performed on this
     * document was an incremental save operation.
     */
    quint16 fComplex:1;

    /**
     * When set to 0, there SHOULD<12> be no pictures in the document.
     */
    quint16 fHasPic:1;

    /**
     * An unsigned integer.  If nFib is less than 0x00D9, then cQuickSaves
     * specifies the number of consecutive times this document was
     * incrementally saved.  If nFib is 0x00D9 or greater, then cQuickSaves
     * MUST be 0xF.
     */
    quint16 cQuickSaves:4;

    /**
     * Specifies whether the document is encrypted or obfuscated as specified
     * in Encryption and Obfuscation.
     */
    quint16 fEncrypted:1;

    /**
     * Specifies the Table stream to which the FIB refers. When this value is
     * set to 1, use 1Table; when this value is set to 0, use 0Table.
     */
    quint16 fWhichTblStm:1;

    /**
     * Specifies whether the document author recommended that the document be
     * opened in read-only mode.
     */
    quint16 fReadOnlyRecommended:1;

    /**
     * Specifies whether the document has a write-reservation password.
     */
    quint16 fWriteReservation:1;

    /**
     * This value MUST be 1.
     */
    quint16 fExtChar:1;

    /**
     * Specifies whether to override the language information and font that are
     * specified in the paragraph style at istd 0 (the normal style) with the
     * defaults that are appropriate for the installation language of the
     * application.
     */
    quint16 fLoadOverride:1;

    /**
     * Specifies whether the installation language of the application that
     * created the document was an East Asian language.
     */
    quint16 fFarEast:1;

    /**
     * If fEncrypted is 1, this bit specifies whether the document is
     * obfuscated by using XOR obfuscation; otherwise, this bit MUST be
     * ignored.
     */
    quint16 fObfuscated:1;

    /**
     * This value SHOULD<13> be 0x00BF.  This value MUST be 0x00BF or 0x00C1.
     */
    quint16 nFibBack;

    /**
     * If fEncryption is 1 and fObfuscation is 1, this value specifies the XOR
     * obfuscation password verifier.  If fEncryption is 1 and fObfuscation is
     * 0, this value specifies the size of the EncryptionHeader that is stored
     * at the beginning of the Table stream as described in Encryption and
     * Obfuscation. Otherwise, this value MUST be 0.
     */
    quint16 lKey;

    /**
     * This value MUST be 0, and MUST be ignored.
     */
    quint8 envr;

    /**
     * This value MUST be 0, and MUST be ignored.
     */
    quint8 fMac:1;

    /**
     * This value SHOULD<14> be 0 and SHOULD<15> be ignored.
     */
    quint8 fEmptySpecial:1;

    /**
     * Specifies whether to override the section properties for page size,
     * orientation, and margins with the defaults that are appropriate for the
     * installation language of the application.
     */
    quint8 fLoadOverridePage:1;

    /**
     * This value is undefined and MUST be ignored.
     */
    quint8 reserved1:1;

    /**
     * This value is undefined and MUST be ignored.
     */
    quint8 reserved2:1;

    /**
     * This value is undefined and MUST be ignored.
     */
    quint8 fSpare0:3;

    /**
     * This value MUST be 0 and MUST be ignored.
     */
    quint16 reserved3;

    /**
     * This value MUST be 0 and MUST be ignored.
     */
    quint16 reserved4;

    /**
     * This value is undefined and MUST be ignored.
     */
    quint32 reserved5;

    /**
     * This value is undefined and MUST be ignored.
     */
    quint32 reserved6;
};
