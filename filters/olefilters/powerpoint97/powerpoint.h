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
#include <qobject.h>

class Powerpoint:
    public QObject
{

    Q_OBJECT

public:

    // Construction.

    Powerpoint();
    virtual ~Powerpoint();

    // Called to parse the given file.

    bool parse(
        myFile &mainStream,
        myFile &currentUser);
    bool parse(
        QDataStream &mainStream,
        QDataStream &currentUser);

private:
    Powerpoint(const Powerpoint &);
    const Powerpoint &operator=(const Powerpoint &);

    // Debug support.

    static const int s_area = 30512;

    // Use unambiguous names for Microsoft types.

    typedef unsigned char U8;
    typedef signed short S16;
    typedef unsigned short U16;
    typedef signed int S32;
    typedef unsigned int U32;

    myFile m_mainStream;

    // Common Header (MSOBFH)

    typedef struct
    {
        union
        {
            U16 info;
            struct
            {
                U16 version: 4;
                U16 instance: 12;
            } fields;
        } opcode;
        U16 type;
        U32 length;
    } Header;

    // Opcode handling and painter methods.

    void walk(
        U32 bytes,
        QDataStream &operands);
    void walk(
        U32 mainStreamOffset);
    void skip(
        U32 bytes,
        QDataStream &operands);
    void invokeHandler(
        Header &op,
        U32 bytes,
        QDataStream &operands);

    void opAnimationInfo(Header &op, U32 bytes, QDataStream &operands);
    void opAnimationInfoAtom(Header &op, U32 bytes, QDataStream &operands);
    void opArrayElementAtom(Header &op, U32 bytes, QDataStream &operands);
    void opBaseTextPropAtom(Header &op, U32 bytes, QDataStream &operands);
    void opBinaryTagData(Header &op, U32 bytes, QDataStream &operands);
    void opBookmarkCollection(Header &op, U32 bytes, QDataStream &operands);
    void opBookmarkEntityAtom(Header &op, U32 bytes, QDataStream &operands);
    void opBookmarkSeedAtom(Header &op, U32 bytes, QDataStream &operands);
    void opCharFormatAtom(Header &op, U32 bytes, QDataStream &operands);
    void opClientSignal1(Header &op, U32 bytes, QDataStream &operands);
    void opClientSignal2(Header &op, U32 bytes, QDataStream &operands);
    void opColorSchemeAtom(Header &op, U32 bytes, QDataStream &operands);
    void opCorePict(Header &op, U32 bytes, QDataStream &operands);
    void opCorePictAtom(Header &op, U32 bytes, QDataStream &operands);
    void opCString(Header &op, U32 bytes, QDataStream &operands);
    void opCurrentUserAtom(Header &op, U32 bytes, QDataStream &operands);
    void opDateTimeMCAtom(Header &op, U32 bytes, QDataStream &operands);
    void opDefaultRulerAtom(Header &op, U32 bytes, QDataStream &operands);
    void opDocRoutingSlip(Header &op, U32 bytes, QDataStream &operands);
    void opDocument(Header &op, U32 bytes, QDataStream &operands);
    void opDocumentAtom(Header &op, U32 bytes, QDataStream &operands);
    void opDocViewInfo(Header &op, U32 bytes, QDataStream &operands);
    void opEmFormatAtom(Header &op, U32 bytes, QDataStream &operands);
    void opEndDocument(Header &op, U32 bytes, QDataStream &operands);
    void opEnvironment(Header &op, U32 bytes, QDataStream &operands);
    void opExAviMovie(Header &op, U32 bytes, QDataStream &operands);
    void opExCDAudio(Header &op, U32 bytes, QDataStream &operands);
    void opExCDAudioAtom(Header &op, U32 bytes, QDataStream &operands);
    void opExControl(Header &op, U32 bytes, QDataStream &operands);
    void opExControlAtom(Header &op, U32 bytes, QDataStream &operands);
    void opExEmbed(Header &op, U32 bytes, QDataStream &operands);
    void opExEmbedAtom(Header &op, U32 bytes, QDataStream &operands);
    void opExHyperlink(Header &op, U32 bytes, QDataStream &operands);
    void opExHyperlinkAtom(Header &op, U32 bytes, QDataStream &operands);
    void opExLink(Header &op, U32 bytes, QDataStream &operands);
    void opExLinkAtom(Header &op, U32 bytes, QDataStream &operands);
    void opExLinkAtom_old(Header &op, U32 bytes, QDataStream &operands);
    void opExMCIMovie(Header &op, U32 bytes, QDataStream &operands);
    void opExMediaAtom(Header &op, U32 bytes, QDataStream &operands);
    void opExMIDIAudio(Header &op, U32 bytes, QDataStream &operands);
    void opExObjList(Header &op, U32 bytes, QDataStream &operands);
    void opExObjListAtom(Header &op, U32 bytes, QDataStream &operands);
    void opExObjRefAtom(Header &op, U32 bytes, QDataStream &operands);
    void opExOleObj(Header &op, U32 bytes, QDataStream &operands);
    void opExOleObjAtom(Header &op, U32 bytes, QDataStream &operands);
    void opExOleObjStg(Header &op, U32 bytes, QDataStream &operands);
    void opExPlain(Header &op, U32 bytes, QDataStream &operands);
    void opExPlainAtom(Header &op, U32 bytes, QDataStream &operands);
    void opExPlainLink(Header &op, U32 bytes, QDataStream &operands);
    void opExPlainLinkAtom(Header &op, U32 bytes, QDataStream &operands);
    void opExQuickTime(Header &op, U32 bytes, QDataStream &operands);
    void opExQuickTimeMovie(Header &op, U32 bytes, QDataStream &operands);
    void opExQuickTimeMovieData(Header &op, U32 bytes, QDataStream &operands);
    void opExSubscription(Header &op, U32 bytes, QDataStream &operands);
    void opExSubscriptionSection(Header &op, U32 bytes, QDataStream &operands);
    void opExternalObject(Header &op, U32 bytes, QDataStream &operands);
    void opExVideo(Header &op, U32 bytes, QDataStream &operands);
    void opExWAVAudioEmbedded(Header &op, U32 bytes, QDataStream &operands);
    void opExWAVAudioEmbeddedAtom(Header &op, U32 bytes, QDataStream &operands);
    void opExWAVAudioLink(Header &op, U32 bytes, QDataStream &operands);
    void opFontCollection(Header &op, U32 bytes, QDataStream &operands);
    void opFontEmbedData(Header &op, U32 bytes, QDataStream &operands);
    void opFontEntityAtom(Header &op, U32 bytes, QDataStream &operands);
    void opFooterMCAtom(Header &op, U32 bytes, QDataStream &operands);
    void opGenericDateMCAtom(Header &op, U32 bytes, QDataStream &operands);
    void opGlineAtom(Header &op, U32 bytes, QDataStream &operands);
    void opGLPointAtom(Header &op, U32 bytes, QDataStream &operands);
    void opGpointAtom(Header &op, U32 bytes, QDataStream &operands);
    void opGratioAtom(Header &op, U32 bytes, QDataStream &operands);
    void opGrColor(Header &op, U32 bytes, QDataStream &operands);
    void opGrColorAtom(Header &op, U32 bytes, QDataStream &operands);
    void opGrectAtom(Header &op, U32 bytes, QDataStream &operands);
    void opGscaling(Header &op, U32 bytes, QDataStream &operands);
    void opGscalingAtom(Header &op, U32 bytes, QDataStream &operands);
    void opGuideAtom(Header &op, U32 bytes, QDataStream &operands);
    void opGuideList(Header &op, U32 bytes, QDataStream &operands);
    void opHandout(Header &op, U32 bytes, QDataStream &operands);
    void opHeaderMCAtom(Header &op, U32 bytes, QDataStream &operands);
    void opHeadersFooters(Header &op, U32 bytes, QDataStream &operands);
    void opHeadersFootersAtom(Header &op, U32 bytes, QDataStream &operands);
    void opInt4ArrayAtom(Header &op, U32 bytes, QDataStream &operands);
    void opInteractiveInfo(Header &op, U32 bytes, QDataStream &operands);
    void opInteractiveInfoAtom(Header &op, U32 bytes, QDataStream &operands);
    void opIRRAtom(Header &op, U32 bytes, QDataStream &operands);
    void opList(Header &op, U32 bytes, QDataStream &operands);
    void opListPlaceholder(Header &op, U32 bytes, QDataStream &operands);
    void opMainMaster(Header &op, U32 bytes, QDataStream &operands);
    void opMasterText(Header &op, U32 bytes, QDataStream &operands);
    void opMetaFile(Header &op, U32 bytes, QDataStream &operands);
    void opNamedShow(Header &op, U32 bytes, QDataStream &operands);
    void opNamedShows(Header &op, U32 bytes, QDataStream &operands);
    void opNamedShowSlides(Header &op, U32 bytes, QDataStream &operands);
    void opNotes(Header &op, U32 bytes, QDataStream &operands);
    void opNotesAtom(Header &op, U32 bytes, QDataStream &operands);
    void opOEPlaceholderAtom(Header &op, U32 bytes, QDataStream &operands);
    void opOEShape(Header &op, U32 bytes, QDataStream &operands);
    void opOEShapeAtom(Header &op, U32 bytes, QDataStream &operands);
    void opOutlineTextRefAtom(Header &op, U32 bytes, QDataStream &operands);
    void opOutlineViewInfo(Header &op, U32 bytes, QDataStream &operands);
    void opParaFormatAtom(Header &op, U32 bytes, QDataStream &operands);
    void opPersistPtrFullBlock(Header &op, U32 bytes, QDataStream &operands);
    void opPersistPtrIncrementalBlock(Header &op, U32 bytes, QDataStream &operands);
    void opPowerPointStateInfoAtom(Header &op, U32 bytes, QDataStream &operands);
    void opPPDrawing(Header &op, U32 bytes, QDataStream &operands);
    void opPPDrawingGroup(Header &op, U32 bytes, QDataStream &operands);
    void opPrintOptions(Header &op, U32 bytes, QDataStream &operands);
    void opProgBinaryTag(Header &op, U32 bytes, QDataStream &operands);
    void opProgStringTag(Header &op, U32 bytes, QDataStream &operands);
    void opProgTags(Header &op, U32 bytes, QDataStream &operands);
    void opPSS(Header &op, U32 bytes, QDataStream &operands);
    void opRecolorEntryAtom(Header &op, U32 bytes, QDataStream &operands);
    void opRecolorInfoAtom(Header &op, U32 bytes, QDataStream &operands);
    void opRTFDateTimeMCAtom(Header &op, U32 bytes, QDataStream &operands);
    void opRulerIndentAtom(Header &op, U32 bytes, QDataStream &operands);
    void opRunArray(Header &op, U32 bytes, QDataStream &operands);
    void opRunArrayAtom(Header &op, U32 bytes, QDataStream &operands);
    void opScheme(Header &op, U32 bytes, QDataStream &operands);
    void opSchemeAtom(Header &op, U32 bytes, QDataStream &operands);
    void opSlide(Header &op, U32 bytes, QDataStream &operands);
    void opSlideAtom(Header &op, U32 bytes, QDataStream &operands);
    void opSlideBase(Header &op, U32 bytes, QDataStream &operands);
    void opSlideBaseAtom(Header &op, U32 bytes, QDataStream &operands);
    void opSlideList(Header &op, U32 bytes, QDataStream &operands);
    void opSlideListWithText(Header &op, U32 bytes, QDataStream &operands);
    void opSlideNumberMCAtom(Header &op, U32 bytes, QDataStream &operands);
    void opSlidePersist(Header &op, U32 bytes, QDataStream &operands);
    void opSlidePersistAtom(Header &op, U32 bytes, QDataStream &operands);
    void opSlideViewInfo(Header &op, U32 bytes, QDataStream &operands);
    void opSlideViewInfoAtom(Header &op, U32 bytes, QDataStream &operands);
    void opSorterViewInfo(Header &op, U32 bytes, QDataStream &operands);
    void opSound(Header &op, U32 bytes, QDataStream &operands);
    void opSoundCollAtom(Header &op, U32 bytes, QDataStream &operands);
    void opSoundCollection(Header &op, U32 bytes, QDataStream &operands);
    void opSoundData(Header &op, U32 bytes, QDataStream &operands);
    void opSrKinsoku(Header &op, U32 bytes, QDataStream &operands);
    void opSrKinsokuAtom(Header &op, U32 bytes, QDataStream &operands);
    void opSSDocInfoAtom(Header &op, U32 bytes, QDataStream &operands);
    void opSslideLayoutAtom(Header &op, U32 bytes, QDataStream &operands);
    void opSSSlideInfoAtom(Header &op, U32 bytes, QDataStream &operands);
    void opStyleTextPropAtom(Header &op, U32 bytes, QDataStream &operands);
    void opSubContainerCompleted(Header &op, U32 bytes, QDataStream &operands);
    void opSubContainerException(Header &op, U32 bytes, QDataStream &operands);
    void opSummary(Header &op, U32 bytes, QDataStream &operands);
    void opTextBookmarkAtom(Header &op, U32 bytes, QDataStream &operands);
    void opTextBytesAtom(Header &op, U32 bytes, QDataStream &operands);
    void opTextCharsAtom(Header &op, U32 bytes, QDataStream &operands);
    void opTextHeaderAtom(Header &op, U32 bytes, QDataStream &operands);
    void opTextRulerAtom(Header &op, U32 bytes, QDataStream &operands);
    void opTextSpecInfoAtom(Header &op, U32 bytes, QDataStream &operands);
    void opTexture(Header &op, U32 bytes, QDataStream &operands);
    void opTxCFStyleAtom(Header &op, U32 bytes, QDataStream &operands);
    void opTxInteractiveInfoAtom(Header &op, U32 bytes, QDataStream &operands);
    void opTxMasterStyleAtom(Header &op, U32 bytes, QDataStream &operands);
    void opTxPFStyleAtom(Header &op, U32 bytes, QDataStream &operands);
    void opTxSIStyleAtom(Header &op, U32 bytes, QDataStream &operands);
    void opTypeFace(Header &op, U32 bytes, QDataStream &operands);
    void opUserEditAtom(Header &op, U32 bytes, QDataStream &operands);
    void opVBAInfo(Header &op, U32 bytes, QDataStream &operands);
    void opVBAInfoAtom(Header &op, U32 bytes, QDataStream &operands);
    void opVBASlideInfo(Header &op, U32 bytes, QDataStream &operands);
    void opVBASlideInfoAtom(Header &op, U32 bytes, QDataStream &operands);
    void opViewInfo(Header &op, U32 bytes, QDataStream &operands);
    void opViewInfoAtom(Header &op, U32 bytes, QDataStream &operands);
};
#endif // POWERPOINT_H
