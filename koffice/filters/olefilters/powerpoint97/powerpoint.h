/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

DESCRIPTION

    This is a generic parser for Microsoft Powerpoint documents. The output is
    a series of callbacks (a.k.a. virtual functions) which the caller can
    override as required.
*/

#ifndef POWERPOINT_H
#define POWERPOINT_H


#include <myfile.h>
#include <qdatastream.h>
#include <qptrlist.h>
#include <qmap.h>
#include <pptSlide.h>

typedef Q_INT32 sint4; // signed 4-byte integral value
typedef Q_INT16 sint2; // signed 4-byte integral value
typedef Q_UINT32 uint4; // unsigned 4-byte integral value
typedef Q_UINT16 uint2; // 2-byte
typedef Q_INT8 bool1; // 1-byte boolean
typedef Q_UINT8 ubyte1; // unsigned byte value
typedef uint2 psrType;
typedef uint4 psrSize; // each record is preceded by
// pssTypeType and pssSizeType.
typedef uint2 psrInstance;
typedef uint2 psrVersion;
typedef uint4 psrReference; // Saved object reference
//typedef QList<PptSlide> PptSlideList;

class Powerpoint
{
public:

    // Construction.

    Powerpoint();
    virtual ~Powerpoint();

    // Called to parse the given file.

    bool parse(
        myFile &mainStream,
        myFile &currentUser,
        myFile &pictures);

    typedef struct
    {
        unsigned type;
        QString data;
    } SlideText;

    typedef struct
    {
        unsigned persistentReference;
        QPtrList<SlideText> text;
    } Slide;

protected:

    virtual void gotDrawing(
        unsigned id,
        QString type,
        unsigned length,
        const char *data) = 0;
    virtual void gotSlide(
         PptSlide &slide) = 0;

private:
    Powerpoint(const Powerpoint &);
    const Powerpoint &operator=(const Powerpoint &);

    // Debug support.

public:
    static const int s_area;

private:
    myFile m_mainStream;
    myFile m_pictures;
    unsigned m_documentRef;
    bool m_documentRefFound;
    QMap<unsigned, unsigned> m_persistentReferences;
    unsigned 	m_editDepth;
    enum
    {
        PASS_GET_SLIDE_REFERENCES,
        PASS_GET_SLIDE_CONTENTS
    } m_pass;
    unsigned m_textType;
    
    QPtrList<PptSlide>	m_slideList;
    PptSlide*		m_pptSlide;

struct PSR_CurrentUserAtom
{
	uint4 size;
	uint4 magic; // Magic number to ensure this is a PowerPoint file.
	uint4 offsetToCurrentEdit; // Offset in main stream to current edit field.
	uint2 lenUserName;
	uint2 docFileVersion;
	ubyte1 majorVersion;
	ubyte1 minorVersion;
};

struct PSR_UserEditAtom
{
	sint4 lastSlideID; // slideID
	uint4 version; // This is major/minor/build which did the edit
	uint4 offsetLastEdit; // File offset of last edit
	uint4 offsetPersistDirectory; // Offset to PersistPtrs for
	// this file version.
	uint4 documentRef;
	uint4 maxPersistWritten; // Addr of last persist ref written to the file (max seen so far).
	sint2 lastViewType; // enum view type
};

struct PSR_SSlideLayoutAtom
{
	sint4 geom;
	ubyte1 placeholderId[8];
};

    PSR_CurrentUserAtom mCurrentUserAtom;
    PSR_UserEditAtom    mUserEditAtom;
    PSR_UserEditAtom*   mpLastUserEditAtom;
    Q_UINT32            mEditOffset;

    // Common Header.

    typedef struct
    {
        union
        {
            Q_UINT16 info;
            struct
            {
                Q_UINT16 version: 4;
                Q_UINT16 instance: 12;
            } fields;
        } opcode;
        Q_UINT16 type;
        Q_UINT32 length;
    } Header;

    // Opcode handling and painter methods.

    void walk(
        Q_UINT32 bytes,
        QDataStream &operands);
    void walk(
        Q_UINT32 mainStreamOffset);
    void walkRecord(
        Q_UINT32 bytes,
        const unsigned char *operands);
    void walkRecord(
        Q_UINT32 mainStreamOffset);
    void walkReference(
        Q_UINT32 reference);
    void skip(
        Q_UINT32 bytes,
        QDataStream &operands);
    void invokeHandler(
        Header &op,
        Q_UINT32 bytes,
        QDataStream &operands);
    void walkDocument();

    void opAnimationInfo(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opAnimationInfoAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opArrayElementAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opBaseTextPropAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opBinaryTagData(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opBookmarkCollection(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opBookmarkEntityAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opBookmarkSeedAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opCharFormatAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opClientSignal1(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opClientSignal2(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opColorSchemeAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opCorePict(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opCorePictAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opCString(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opCurrentUserAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opDateTimeMCAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opDefaultRulerAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opDocRoutingSlip(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opDocument(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opDocumentAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opDocViewInfo(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opEmFormatAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opEndDocument(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opEnvironment(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExAviMovie(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExCDAudio(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExCDAudioAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExControl(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExControlAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExEmbed(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExEmbedAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExHyperlink(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExHyperlinkAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExLink(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExLinkAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExLinkAtom_old(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExMCIMovie(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExMediaAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExMIDIAudio(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExObjList(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExObjListAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExObjRefAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExOleObj(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExOleObjAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExOleObjStg(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExPlain(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExPlainAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExPlainLink(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExPlainLinkAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExQuickTime(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExQuickTimeMovie(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExQuickTimeMovieData(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExSubscription(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExSubscriptionSection(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExternalObject(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExVideo(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExWAVAudioEmbedded(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExWAVAudioEmbeddedAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opExWAVAudioLink(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opFontCollection(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opFontEmbedData(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opFontEntityAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opFooterMCAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opGenericDateMCAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opGlineAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opGLPointAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opGpointAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opGratioAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opGrColor(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opGrColorAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opGrectAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opGscaling(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opGscalingAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opGuideAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opGuideList(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opHandout(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opHeaderMCAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opHeadersFooters(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opHeadersFootersAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opInt4ArrayAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opInteractiveInfo(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opInteractiveInfoAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opIRRAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opList(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opListPlaceholder(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opMainMaster(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opMasterText(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opMetaFile(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opMsod(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opNamedShow(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opNamedShows(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opNamedShowSlides(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opNotes(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opNotesAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opOEPlaceholderAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opOEShape(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opOEShapeAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opOutlineTextRefAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opOutlineViewInfo(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opParaFormatAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opPersistPtrFullBlock(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opPersistPtrIncrementalBlock(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opPersistPtrIncrementalBlock2(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opPowerPointStateInfoAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opPPDrawing(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opPPDrawingGroup(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opPrintOptions(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opProgBinaryTag(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opProgStringTag(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opProgTags(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opPSS(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opRecolorEntryAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opRecolorInfoAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opRTFDateTimeMCAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opRulerIndentAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opRunArray(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opRunArrayAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opScheme(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSchemeAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSlide(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSlideAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSlideBase(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSlideBaseAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSlideList(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSlideListWithText(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSlideNumberMCAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSlidePersist(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSlidePersistAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSlideViewInfo(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSlideViewInfoAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSorterViewInfo(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSound(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSoundCollAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSoundCollection(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSoundData(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSrKinsoku(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSrKinsokuAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSSDocInfoAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSSSlideLayoutAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSSSlideInfoAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opStyleTextPropAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSubContainerCompleted(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSubContainerException(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opSummary(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opTextBookmarkAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opTextBytesAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opTextCharsAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opTextHeaderAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opTextRulerAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opTextSpecInfoAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opTexture(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opTxCFStyleAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opTxInteractiveInfoAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opTxMasterStyleAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opTxPFStyleAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opTxSIStyleAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opTypeFace(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opUserEditAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opVBAInfo(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opVBAInfoAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opVBASlideInfo(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opVBASlideInfoAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opViewInfo(Header &op, Q_UINT32 bytes, QDataStream &operands);
    void opViewInfoAtom(Header &op, Q_UINT32 bytes, QDataStream &operands);
};
#endif
