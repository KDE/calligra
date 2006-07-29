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
*/

#include <kdebug.h>
#include <myfile.h>
#include <powerpoint.h>

const int Powerpoint::s_area = 30512;

Powerpoint::Powerpoint()
{
	mEditOffset = 0;
	m_pptSlide = 0;
}

Powerpoint::~Powerpoint()
{
    m_persistentReferences.clear();
    //m_slides.clear();
}

void Powerpoint::invokeHandler(
    Header &op,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    typedef void (Powerpoint::*method)(Header &op, Q_UINT32 bytes, QDataStream &operands);

    typedef struct
    {
        const char *name;
        Q_UINT16 opcode;
        method handler;
    } opcodeEntry;

    static const opcodeEntry funcTab[] =
    {
        { "ANIMATIONINFO",              4116,   0 /* &Powerpoint::opAnimationInfo */ },
        { "ANIMATIONINFOATOM",          4081,   0 /* &Powerpoint::opAnimationInfoAtom */ },
        { "ARRAYELEMENTATOM",           2030,   0 /* &Powerpoint::opArrayElementAtom */ },
        { "BASETEXTPROPATOM",           4002,   0 /* &Powerpoint::opBaseTextPropAtom */ },
        { "BINARYTAGDATA",              5003,   0 /* &Powerpoint::opBinaryTagData */ },
        { "BOOKMARKCOLLECTION",         2019,   0 /* &Powerpoint::opBookmarkCollection */ },
        { "BOOKMARKENTITYATOM",         4048,   0 /* &Powerpoint::opBookmarkEntityAtom */ },
        { "BOOKMARKSEEDATOM",           2025,   0 /* &Powerpoint::opBookmarkSeedAtom */ },
        { "CHARFORMATATOM",             4066,   0 /* &Powerpoint::opCharFormatAtom */ },
        { "CLIENTSIGNAL1",              6,      0 /* &Powerpoint::opClientSignal1 */ },
        { "CLIENTSIGNAL2",              7,      0 /* &Powerpoint::opClientSignal2 */ },
        { "COLORSCHEMEATOM",            2032,   &Powerpoint::opColorSchemeAtom },
        { "COREPICT",                   4037,   0 /* &Powerpoint::opCorePict */ },
        { "COREPICTATOM",               4038,   0 /* &Powerpoint::opCorePictAtom */ },
        { "CSTRING",                    4026,   &Powerpoint::opCString },
        { "CURRENTUSERATOM",            4086,   &Powerpoint::opCurrentUserAtom },
        { "DATETIMEMCATOM",             4087,   0 /* &Powerpoint::opDateTimeMCAtom */ },
        { "DEFAULTRULERATOM",           4011,   0 /* &Powerpoint::opDefaultRulerAtom */ },
        { "DOCROUTINGSLIP",             1030,   0 /* &Powerpoint::opDocRoutingSlip */ },
        { "DOCUMENT",                   1000,   &Powerpoint::opDocument },
        { "DOCUMENTATOM",               1001,   &Powerpoint::opDocumentAtom },
        { "DOCVIEWINFO",                1014,   0 /* &Powerpoint::opDocViewInfo */ },
        { "EMFORMATATOM",               4065,   0 /* &Powerpoint::opEmFormatAtom */ },
        { "ENDDOCUMENT",                1002,   &Powerpoint::opEndDocument },
        { "ENVIRONMENT",                1010,   &Powerpoint::opEnvironment },
        { "EXAVIMOVIE",                 4102,   0 /* &Powerpoint::opExAviMovie */ },
        { "EXCDAUDIO",                  4110,   0 /* &Powerpoint::opExCDAudio */ },
        { "EXCDAUDIOATOM",              4114,   0 /* &Powerpoint::opExCDAudioAtom */ },
        { "EXCONTROL",                  4078,   0 /* &Powerpoint::opExControl */ },
        { "EXCONTROLATOM",              4091,   0 /* &Powerpoint::opExControlAtom */ },
        { "EXEMBED",                    4044,   &Powerpoint::opExEmbed },
        { "EXEMBEDATOM",                4045,   &Powerpoint::opExEmbedAtom },
        { "EXHYPERLINK",                4055,   0 /* &Powerpoint::opExHyperlink */ },
        { "EXHYPERLINKATOM",            4051,   0 /* &Powerpoint::opExHyperlinkAtom */ },
        { "EXLINK",                     4046,   0 /* &Powerpoint::opExLink */ },
        { "EXLINKATOM",                 4049,   0 /* &Powerpoint::opExLinkAtom */ },
        { "EXLINKATOM_OLD",             4047,   0 /* &Powerpoint::opExLinkAtom_old */ },
        { "EXMCIMOVIE",                 4103,   0 /* &Powerpoint::opExMCIMovie */ },
        { "EXMEDIAATOM",                4100,   0 /* &Powerpoint::opExMediaAtom */ },
        { "EXMIDIAUDIO",                4109,   0 /* &Powerpoint::opExMIDIAudio */ },
        { "EXOBJLIST",                  1033,   &Powerpoint::opExObjList },
        { "EXOBJLISTATOM",              1034,   &Powerpoint::opExObjListAtom },
        { "EXOBJREFATOM",               3009,   &Powerpoint::opExObjRefAtom },
        { "EXOLEOBJ",                   4034,   &Powerpoint::opExOleObj },
        { "EXOLEOBJATOM",               4035,   &Powerpoint::opExOleObjAtom },
        { "EXOLEOBJSTG",                4113,   &Powerpoint::opExOleObjStg },
        { "EXPLAIN",                    4053,   0 /* &Powerpoint::opExPlain */ },
        { "EXPLAINATOM",                4039,   0 /* &Powerpoint::opExPlainAtom */ },
        { "EXPLAINLINK",                4054,   0 /* &Powerpoint::opExPlainLink */ },
        { "EXPLAINLINKATOM",            4036,   0 /* &Powerpoint::opExPlainLinkAtom */ },
        { "EXQUICKTIME",                4073,   0 /* &Powerpoint::opExQuickTime */ },
        { "EXQUICKTIMEMOVIE",           4074,   0 /* &Powerpoint::opExQuickTimeMovie */ },
        { "EXQUICKTIMEMOVIEDATA",       4075,   0 /* &Powerpoint::opExQuickTimeMovieData */ },
        { "EXSUBSCRIPTION",             4076,   0 /* &Powerpoint::opExSubscription */ },
        { "EXSUBSCRIPTIONSECTION",      4077,   0 /* &Powerpoint::opExSubscriptionSection */ },
        { "EXTERNALOBJECT",             4027,   0 /* &Powerpoint::opExternalObject */ },
        { "EXVIDEO",                    4101,   0 /* &Powerpoint::opExVideo */ },
        { "EXWAVAUDIOEMBEDDED",         4111,   0 /* &Powerpoint::opExWAVAudioEmbedded */ },
        { "EXWAVAUDIOEMBEDDEDATOM",     4115,   0 /* &Powerpoint::opExWAVAudioEmbeddedAtom */ },
        { "EXWAVAUDIOLINK",             4112,   0 /* &Powerpoint::opExWAVAudioLink */ },
        { "FONTCOLLECTION",             2005,   &Powerpoint::opFontCollection },
        { "FONTEMBEDDATA",              4024,   0 /* &Powerpoint::opFontEmbedData */ },
        { "FONTENTITYATOM",             4023,   &Powerpoint::opFontEntityAtom },
        { "FOOTERMCATOM",               4090,   0 /* &Powerpoint::opFooterMCAtom */ },
        { "GENERICDATEMCATOM",          4088,   0 /* &Powerpoint::opGenericDateMCAtom */ },
        { "GLINEATOM",                  10004,  0 /* &Powerpoint::opGlineAtom */ },
        { "GLPOINTATOM",                10003,  0 /* &Powerpoint::opGLPointAtom */ },
        { "GPOINTATOM",                 3034,   0 /* &Powerpoint::opGpointAtom */ },
        { "GRATIOATOM",                 3031,   0 /* &Powerpoint::opGratioAtom */ },
        { "GRCOLOR",                    3020,   0 /* &Powerpoint::opGrColor */ },
        { "GRCOLORATOM",                10002,  0 /* &Powerpoint::opGrColorAtom */ },
        { "GRECTATOM",                  3025,   0 /* &Powerpoint::opGrectAtom */ },
        { "GSCALING",                   3032,   0 /* &Powerpoint::opGscaling */ },
        { "GSCALINGATOM",               10001,  0 /* &Powerpoint::opGscalingAtom */ },
        { "GUIDEATOM",                  1019,   0 /* &Powerpoint::opGuideAtom */ },
        { "GUIDELIST",                  2026,   0 /* &Powerpoint::opGuideList */ },
        { "HANDOUT",                    4041,   0 /* &Powerpoint::opHandout */ },
        { "HEADERMCATOM",               4089,   0 /* &Powerpoint::opHeaderMCAtom */ },
        { "HEADERSFOOTERS",             4057,   &Powerpoint::opHeadersFooters },
        { "HEADERSFOOTERSATOM",         4058,   &Powerpoint::opHeadersFootersAtom },
        { "INT4ARRAYATOM",              2031,   0 /* &Powerpoint::opInt4ArrayAtom */ },
        { "INTERACTIVEINFO",            4082,   0 /* &Powerpoint::opInteractiveInfo */ },
        { "INTERACTIVEINFOATOM",        4083,   0 /* &Powerpoint::opInteractiveInfoAtom */ },
        { "IRRATOM",                    2,      0 /* &Powerpoint::opIRRAtom */ },
        { "LIST",                       2000,   &Powerpoint::opList },
        { "LISTPLACEHOLDER",            2017,   0 /* &Powerpoint::opListPlaceholder */ },
        { "MAINMASTER",                 1016,   &Powerpoint::opMainMaster },
        { "MASTERTEXT",                 4068,   0 /* &Powerpoint::opMasterText */ },
        { "METAFILE",                   4033,   0 /* &Powerpoint::opMetaFile */ },
        { "NAMEDSHOW",                  1041,   0 /* &Powerpoint::opNamedShow */ },
        { "NAMEDSHOWS",                 1040,   0 /* &Powerpoint::opNamedShows */ },
        { "NAMEDSHOWSLIDES",            1042,   0 /* &Powerpoint::opNamedShowSlides */ },
        { "NOTES",                      1008,   &Powerpoint::opNotes },
        { "NOTESATOM",                  1009,   &Powerpoint::opNotesAtom },
        { "OEPLACEHOLDERATOM",          3011,   0 /* &Powerpoint::opOEPlaceholderAtom */ },
        { "OESHAPE",                    3008,   0 /* &Powerpoint::opOEShape */ },
        { "OESHAPEATOM",                3035,   0 /* &Powerpoint::opOEShapeAtom */ },
        { "OUTLINETEXTREFATOM",         3998,   0 /* &Powerpoint::opOutlineTextRefAtom */ },
        { "OUTLINEVIEWINFO",            1031,   &Powerpoint::opOutlineViewInfo },
        { "PARAFORMATATOM",             4067,   0 /* &Powerpoint::opParaFormatAtom */ },
        { "PERSISTPTRFULLBLOCK",        6001,   0 /* &Powerpoint::opPersistPtrFullBlock */ },
        { "PERSISTPTRINCREMENTALBLOCK", 6002,   &Powerpoint::opPersistPtrIncrementalBlock },
        { "POWERPOINTSTATEINFOATOM",    10,     0 /* &Powerpoint::opPowerPointStateInfoAtom */ },
        { "PPDRAWING",                  1036,   &Powerpoint::opPPDrawing },
        { "PPDRAWINGGROUP",             1035,   &Powerpoint::opPPDrawingGroup },
        { "PRINTOPTIONS",               6000,   0 /* &Powerpoint::opPrintOptions */ },
        { "PROGBINARYTAG",              5002,   0 /* &Powerpoint::opProgBinaryTag */ },
        { "PROGSTRINGTAG",              5001,   0 /* &Powerpoint::opProgStringTag */ },
        { "PROGTAGS",                   5000,   0 /* &Powerpoint::opProgTags */ },
        { "PSS",                        3,      0 /* &Powerpoint::opPSS */ },
        { "RECOLORENTRYATOM",           4062,   0 /* &Powerpoint::opRecolorEntryAtom */ },
        { "RECOLORINFOATOM",            4071,   0 /* &Powerpoint::opRecolorInfoAtom */ },
        { "RTFDATETIMEMCATOM",          4117,   0 /* &Powerpoint::opRTFDateTimeMCAtom */ },
        { "RULERINDENTATOM",            10000,  0 /* &Powerpoint::opRulerIndentAtom */ },
        { "RUNARRAY",                   2028,   0 /* &Powerpoint::opRunArray */ },
        { "RUNARRAYATOM",               2029,   0 /* &Powerpoint::opRunArrayAtom */ },
        { "SCHEME",                     1012,   0 /* &Powerpoint::opScheme */ },
        { "SCHEMEATOM",                 1013,   0 /* &Powerpoint::opSchemeAtom */ },
        { "SLIDE",                      1006,   &Powerpoint::opSlide },
        { "SLIDEATOM",                  1007,   &Powerpoint::opSlideAtom },
        { "SLIDEBASE",                  1004,   0 /* &Powerpoint::opSlideBase */ },
        { "SLIDEBASEATOM",              1005,   0 /* &Powerpoint::opSlideBaseAtom */ },
        { "SLIDELIST",                  4084,   0 /* &Powerpoint::opSlideList */ },
        { "SLIDELISTWITHTEXT",          4080,   &Powerpoint::opSlideListWithText },
        { "SLIDENUMBERMCATOM",          4056,   0 /* &Powerpoint::opSlideNumberMCAtom */ },
        { "SLIDEPERSIST",               1003,   0 /* &Powerpoint::opSlidePersist */ },
        { "SLIDEPERSISTATOM",           1011,   &Powerpoint::opSlidePersistAtom },
        { "SLIDEVIEWINFO",              1018,   &Powerpoint::opSlideViewInfo },
        { "SLIDEVIEWINFOATOM",          1022,   0 /* &Powerpoint::opSlideViewInfoAtom */ },
        { "SORTERVIEWINFO",             1032,   0 /* &Powerpoint::opSorterViewInfo */ },
        { "SOUND",                      2022,   0 /* &Powerpoint::opSound */ },
        { "SOUNDCOLLATOM",              2021,   0 /* &Powerpoint::opSoundCollAtom */ },
        { "SOUNDCOLLECTION",            2020,   0 /* &Powerpoint::opSoundCollection */ },
        { "SOUNDDATA",                  2023,   0 /* &Powerpoint::opSoundData */ },
        { "SRKINSOKU",                  4040,   &Powerpoint::opSrKinsoku },
        { "SRKINSOKUATOM",              4050,   0 /* &Powerpoint::opSrKinsokuAtom */ },
        { "SSDOCINFOATOM",              1025,   &Powerpoint::opSSDocInfoAtom },
        { "SSLIDELAYOUTATOM",           1015,   &Powerpoint::opSSSlideLayoutAtom },
        { "SSSLIDEINFOATOM",            1017,   0 /* &Powerpoint::opSSSlideInfoAtom */ },
        { "STYLETEXTPROPATOM",          4001,   &Powerpoint::opStyleTextPropAtom },
        { "SUBCONTAINERCOMPLETED",      1,      0 /* &Powerpoint::opSubContainerCompleted */ },
        { "SUBCONTAINEREXCEPTION",      4,      0 /* &Powerpoint::opSubContainerException */ },
        { "SUMMARY",                    1026,   0 /* &Powerpoint::opSummary */ },
        { "TEXTBOOKMARKATOM",           4007,   0 /* &Powerpoint::opTextBookmarkAtom */ },
        { "TEXTBYTESATOM",              4008,   &Powerpoint::opTextBytesAtom },
        { "TEXTCHARSATOM",              4000,   &Powerpoint::opTextCharsAtom },
        { "TEXTHEADERATOM",             3999,   &Powerpoint::opTextHeaderAtom },
        { "TEXTRULERATOM",              4006,   0 /* &Powerpoint::opTextRulerAtom */ },
        { "TEXTSPECINFOATOM",           4010,   &Powerpoint::opTextSpecInfoAtom },
        { "TEXTURE",                    1027,   0 /* &Powerpoint::opTexture */ },
        { "TXCFSTYLEATOM",              4004,   0 /* &Powerpoint::opTxCFStyleAtom */ },
        { "TXINTERACTIVEINFOATOM",      4063,   0 /* &Powerpoint::opTxInteractiveInfoAtom */ },
        { "TXMASTERSTYLEATOM",          4003,   &Powerpoint::opTxMasterStyleAtom },
        { "TXPFSTYLEATOM",              4005,   0 /* &Powerpoint::opTxPFStyleAtom */ },
        { "TXSISTYLEATOM",              4009,   &Powerpoint::opTxSIStyleAtom },
        { "TYPEFACE",                   4025,   0 /* &Powerpoint::opTypeFace */ },
        { "USEREDITATOM",               4085,   &Powerpoint::opUserEditAtom },
        { "VBAINFO",                    1023,   &Powerpoint::opVBAInfo },
        { "VBAINFOATOM",                1024,   0 /* &Powerpoint::opVBAInfoAtom */ },
        { "VBASLIDEINFO",               1028,   0 /* &Powerpoint::opVBASlideInfo */ },
        { "VBASLIDEINFOATOM",           1029,   0 /* &Powerpoint::opVBASlideInfoAtom */ },
        { "VIEWINFO",                   1020,   0 /* &Powerpoint::opViewInfo */ },
        { "VIEWINFOATOM",               1021,   0 /* &Powerpoint::opViewInfoAtom */ },
        { NULL,                         0,      0 },
        { "MSOD",                       0,      &Powerpoint::opMsod }
    };
    unsigned i;
    method result;

    // Scan lookup table for operation.

    for (i = 0; funcTab[i].name; i++)
    {
        if (funcTab[i].opcode == op.type)
        {
            break;
        }
    }

    // Invoke handler.

    result = funcTab[i].handler;
    if (!result && (op.type >= 0xF000) && (0xFFFF >= op.type))
        result = funcTab[++i].handler;
    if (!result)
    {
        if (funcTab[i].name)
            kdWarning(s_area) << "invokeHandler: unsupported opcode: " <<
                funcTab[i].name <<
                " operands: " << bytes << endl;
        else
            kdWarning(s_area) << "invokeHandler: unsupported opcode: " <<
                op.type <<
                " operands: " << bytes << endl;

        // Skip data we cannot use.
        skip(bytes, operands);
    }
    else
    {
        kdDebug(s_area) << "invokeHandler: opcode: " << funcTab[i].name <<
            " operands: " << bytes << endl;

        // We don't invoke the handler directly on the incoming operands, but
        // via a temporary datastream. This adds overhead, but eliminates the
        // need for the individual handlers to read *exactly* the right amount
        // of data (thus speeding development, and possibly adding some
        // future-proofing).

        if (bytes)
        {
            QByteArray *record = new QByteArray(bytes);
            QDataStream *body;

            operands.readRawBytes(record->data(), bytes);
            body = new QDataStream(*record, IO_ReadOnly);
            body->setByteOrder(QDataStream::LittleEndian);
            (this->*result)(op, bytes, *body);
            delete body;
            delete record;
        }
        else
        {
            QDataStream *body = new QDataStream();

            (this->*result)(op, bytes, *body);
            delete body;
        }
    }
}

bool Powerpoint::parse(
    myFile &mainStream,
    myFile &currentUser,
    myFile &pictures)
{
    unsigned i;

    m_mainStream = mainStream;
    m_pictures = pictures;
    m_documentRef = 0;
    m_documentRefFound = false;
    m_persistentReferences.clear();
    m_slideList.clear();
    m_editDepth = 0;

    // Find the slide references.

    m_pass = PASS_GET_SLIDE_REFERENCES;
    kdError(s_area) << "parseing step 1 walkRecord..." << endl;
    walkRecord(currentUser.length, currentUser.data);//get curren user information

    kdError(s_area) << "parseing step 2 walkReference..." << endl;
    //for(i = 0; i <= 2; i++)//walk the 2 slide atoms
    //{
    //	kdError(s_area) << "ref == :" << i <<endl;
	//walkReference(i);
    //}
    kdError(s_area) << "walking slide list!!!!.." << endl;
    if(mEditOffset != 0) walk(mEditOffset);
    else walkDocument();
    // We should have a complete list of slide persistent references.
    m_pass = PASS_GET_SLIDE_CONTENTS;
    kdError(s_area) << "TOTAL SLIDES XXxx: " << m_slideList.count() << endl;

     for (i = 0; i < m_slideList.count(); i++)
    {
        m_pptSlide = m_slideList.at(i);
        walkReference(i);
        gotSlide(*m_pptSlide);
    }

    return true;
}

void Powerpoint::opColorSchemeAtom(
    Header & /* op */,
    Q_UINT32,
    QDataStream &operands)
{
    struct
    {
        Q_UINT32 background;
        Q_UINT32 textAndLines;
        Q_UINT32 shadows;
        Q_UINT32 titleText;
        Q_UINT32 fills;
        Q_UINT32 accent;
        Q_UINT32 accentAndHyperlink;
        Q_UINT32 accentAndFollowedHyperlink;
    } data;

    operands >> data.background >> data.textAndLines >> data.shadows >>
        data.titleText >> data.fills >> data.accent >> data.accentAndHyperlink >>
        data.accentAndFollowedHyperlink;
}

//
// A Unicode String.
//
void Powerpoint::opCString(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    QString value;
    unsigned i;

    for (i = 0; i < bytes / 2; i++)
    {
        Q_UINT16 tmp;

        operands >> tmp;
        value += tmp;
    }
    kdDebug(s_area) << "value: " << value << endl;
}

void Powerpoint::opCurrentUserAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &operands)
{

    const Q_UINT32 MAGIC_NUMBER = (Q_UINT32)(-476987297);
    //unsigned i;

    operands >> mCurrentUserAtom.size >> mCurrentUserAtom.magic >> mCurrentUserAtom.offsetToCurrentEdit >>
        mCurrentUserAtom.lenUserName >> mCurrentUserAtom.docFileVersion >> mCurrentUserAtom.majorVersion >>
        mCurrentUserAtom.minorVersion;

    kdDebug(s_area) << "\nPSR_CurrentUserAtom:" <<
        "\n\tsize: " << mCurrentUserAtom.size <<
        "\n\tmagic: " << mCurrentUserAtom.magic <<
        "\n\toffsetToCurrentEdit: " << mCurrentUserAtom.offsetToCurrentEdit <<
        "\n\tlenUserName: " << mCurrentUserAtom.lenUserName <<
        "\n\tdocFileVersion: " << mCurrentUserAtom.docFileVersion <<
        "\n\tmajorVersion: " << mCurrentUserAtom.majorVersion <<
        "\n\tminorVersion: " << mCurrentUserAtom.minorVersion << endl;

    switch (m_pass)
    {
    case PASS_GET_SLIDE_REFERENCES:
        if (mCurrentUserAtom.size != 20)
        {
            kdError(s_area) << "invalid size: " << mCurrentUserAtom.size << endl;
        }
        if (mCurrentUserAtom.magic != MAGIC_NUMBER)
        {
            kdError(s_area) << "invalid magic number: " << mCurrentUserAtom.magic << endl;
        }
        if ((mCurrentUserAtom.docFileVersion != 1012) ||
            (mCurrentUserAtom.majorVersion != 3) ||
            (mCurrentUserAtom.minorVersion != 0))// ||
           // (data.release < 8) ||
           // (data.release > 10))
        {
            kdError(s_area) << "invalid version: " << mCurrentUserAtom.docFileVersion <<
                "." << mCurrentUserAtom.majorVersion <<
                "." << mCurrentUserAtom.minorVersion <<
                endl;
                //"." << data.release << endl;
        }

        // Now walk main stream starting at current edit point.

        walkRecord(mCurrentUserAtom.offsetToCurrentEdit);
        break;
    case PASS_GET_SLIDE_CONTENTS:
        break;
    };
}

void Powerpoint::opDocument(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::opDocumentAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &/*operands*/)
{
}

void Powerpoint::opEndDocument(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::opEnvironment(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

//
// Contains an ExEmbedAtom and 3 CStrings (the menu name, the program id which
// unqiuely identifies the type of object, and the "paste special" name).
//
void Powerpoint::opExEmbed(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

//
// Information about an embedded object.
//
void Powerpoint::opExEmbedAtom(
    Header & /* op */,
    Q_UINT32,
    QDataStream &operands)
{
    struct
    {
        Q_INT32 followColorScheme;
        Q_UINT8 cantLockServerB;
        Q_UINT8 noSizeToServerB;
        Q_UINT8 isTable;
    } data;

    operands >> data.followColorScheme;
    operands >> data.cantLockServerB;
    operands >> data.noSizeToServerB;
    operands >> data.isTable;
}

void Powerpoint::opFontCollection(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::opFontEntityAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &/*operands*/)
{
}

void Powerpoint::opHeadersFooters(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::opHeadersFootersAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &/*operands*/)
{
}

void Powerpoint::opList(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::opMainMaster(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::opMsod(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    char *data;

    switch (m_pass)
    {
    case PASS_GET_SLIDE_REFERENCES:
        break;
    case PASS_GET_SLIDE_CONTENTS:
        data = new char[bytes];
        operands.readRawBytes((char *)data, bytes);
kdError() <<"       drgid: "<< m_pptSlide->getPsrReference() << endl;
        gotDrawing(m_pptSlide->getPsrReference(), "msod", bytes, data);
        delete [] data;
        break;
    };
}

void Powerpoint::opNotes(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::opNotesAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &operands)
{
    struct
    {
        Q_INT32 slideId;    // Id for the corresponding slide.
        Q_UINT16 flags;
    } data;

    operands >> data.slideId >> data.flags;
}

//
// Contains an ExObjListAtom and a list of all objects in a document.
//
void Powerpoint::opExObjList(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

//
// Get the next unique identifier for OLE objects.
//
void Powerpoint::opExObjListAtom(
    Header & /* op */,
    Q_UINT32,
    QDataStream &operands)
{
    Q_UINT32 objectSeedId;

    operands >> objectSeedId;
    kdDebug(s_area) << "next OLE obj id: " << objectSeedId << endl;
}

void Powerpoint::opExObjRefAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &/*operands*/)
{
}

void Powerpoint::opExOleObj(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &/*operands*/)
{
}

void Powerpoint::opExOleObjAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &operands)
{
    struct
    {
        Q_UINT32 drawAspect;
        Q_INT32 type;
        Q_INT32 objID;
        Q_INT32 subType;
        Q_INT8 isBlank;
    } data;

    operands >> data.drawAspect;
    operands >> data.type;
    operands >> data.objID;
    operands >> data.subType;
    operands >> data.isBlank;
    kdDebug(s_area) << ((data.type == 0) ? "embedded " : "linked ") <<
        "OLE obj id: " << data.objID << endl;
}

void Powerpoint::opExOleObjStg(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &/*operands*/)
{
}

void Powerpoint::opOutlineViewInfo(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::opPersistPtrIncrementalBlock2(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &/*operands*/)
{
    //PSR_UserEditAtom userEdit;
    //Q_INT16 offsetToEdit = mCurrentUserAtom.offsetToCurrentEdit;

    /* huh?
    while(0 < offsetToEdit)
    {

    }*/

//       mpLastUserEditAtom;

}

void Powerpoint::opPersistPtrIncrementalBlock(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    struct
    {
        union
        {
            Q_UINT32 info;
            struct
            {
                Q_UINT32 offsetNumber: 20;
                Q_UINT32 offsetCount: 12;
            } fields;
        } header;
        Q_UINT32 offset;
    } data;
    Q_UINT32 length = 0;

    while (length < bytes)
    {
        unsigned i;

        // Walk references numbered between:
        //
        //    offsetNumber..offsetNumber + offsetCount - 1
        //
        operands >> data.header.info;
        length += sizeof(data.header.info);
	kdDebug(s_area) << "length1: " << length << endl;
	kdDebug(s_area) << "m_pass: " << m_pass << endl;
	//kdDebug(s_area) << "PASS_GET_SLIDE_REFERENCES: " << PASS_GET_SLIDE_REFERENCES << endl;
	kdDebug(s_area) << "data.header.fields.offsetCount: " << data.header.fields.offsetCount << endl;

        for (i = 0; i < data.header.fields.offsetCount; i++)
        {
            unsigned reference = data.header.fields.offsetNumber + i;

            operands >> data.offset;
            length += sizeof(data.offset);
	    //kdDebug(s_area) << "length2: " << length << endl;
            switch (m_pass)
            {
            case PASS_GET_SLIDE_REFERENCES:

                // Create a record of this persistent reference.

                if (m_persistentReferences.end() == m_persistentReferences.find(reference))
                {
		    if(reference < 5)
		    {
                    	kdDebug(s_area) << "persistent reference: " << reference <<
                        		   ": " << data.offset << endl;
	            }
                    m_persistentReferences.insert(reference, data.offset);
                }
                else
                {
                    // This reference has already been seen! Since the parse proceeds
                    // backwards in time form the most recent edit, I assume this means
                    // that this is an older version of this slide...so just ignore it.
                   // kdDebug(s_area) << "superseded reference: " << reference <<
                   //     ": " << data.offset << endl;
                }
                break;
            case PASS_GET_SLIDE_CONTENTS:
                break;
            };
        }
 	kdDebug(s_area) << "LEAVING persistant block -- " << endl;
   }

}

void Powerpoint::opPPDrawing(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::opPPDrawingGroup(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::opSlide(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::opSlideAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &operands)
{
    struct
    {
        Q_UINT8 layout[12];  // Slide layout descriptor.
        Q_INT32 masterId;   // Id of the master of the slide. Zero for a master slide.
        Q_INT32 notesId;    // Id for the corresponding notes slide. Zero if slide has no notes.
        Q_UINT16 flags;
    } data;

    Header tmp;
    tmp.type = 1015;
    tmp.length = sizeof(data.layout);
    invokeHandler(tmp, tmp.length, operands);
    operands >> data.masterId >> data.notesId >> data.flags;

    kdDebug(s_area) << "\nSlideAtom:" <<
                       "\n\tmasterId: " << data.masterId <<
		       "\n\tnotesId: " << data.notesId <<
		       "\n\tflags: " << data.flags << endl;

    //invokeHandler(tmp, tmp.length, operands);

}

void Powerpoint::opSlideListWithText(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::opSlidePersistAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &operands)
{
    struct
    {
        Q_UINT32 psrReference;   // Logical reference to the slide persist object.
        Q_UINT32 flags;          // If bit 3 set then slide contains shapes other than placeholders.
        Q_INT32 numberTexts;    // Number of placeholder texts stored with the persist object.
        Q_INT32 slideId;        // Unique slide identifier, used for OLE link monikers for example.
        Q_UINT32 reserved;
    } data;

    operands >> data.psrReference >> data.flags >> data.numberTexts >>
        data.slideId >> data.reserved;

     kdDebug(s_area) << "\nopSlidePersistAtom: " <<
    	               "\n\tpsrReference: " << data.psrReference <<
		       "\n\tflags: " << data.flags <<
		       "\n\tnumberTexts: " << data.numberTexts <<
		       "\n\tslideId: " << data.slideId <<
		       "\n\treserved: " << data.reserved << endl;

   switch (m_pass)
    {
    case PASS_GET_SLIDE_REFERENCES:
    	if(data.slideId > 0)//if not master slide.. is there another way to tell???
	{
		m_pptSlide = new PptSlide;
		m_pptSlide->setPsrReference(data.psrReference);
		m_slideList.append(m_pptSlide);

		kdDebug(s_area) << "XXXXXXXXXslide: " << data.psrReference <<
		" has texts: " << data.numberTexts << endl;
	}
        break;
    case PASS_GET_SLIDE_CONTENTS:
        break;
    };
}

void Powerpoint::opSlideViewInfo(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::opSrKinsoku(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::opSSDocInfoAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &/*operands*/)
{
}

void Powerpoint::opSSSlideLayoutAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &operands)
{

    PSR_SSlideLayoutAtom data;
    unsigned i;

    operands >> data.geom;
    for (i = 0; i < sizeof(data.placeholderId); i++)
    {
        operands >> data.placeholderId[i];
    }

    kdDebug(s_area) << "\nPSR_SSlideLayoutAtom:" <<
                       "\n\tgeom: " << data.geom <<
		       "\n\tplaceholderId[0]: " << data.placeholderId[0] <<
		       "\n\tplaceholderId[1]: " << data.placeholderId[1] <<
		       "\n\tplaceholderId[2]: " << data.placeholderId[2] <<
		       "\n\tplaceholderId[3]: " << data.placeholderId[3] <<
		       "\n\tplaceholderId[4]: " << data.placeholderId[4] <<
		       "\n\tplaceholderId[5]: " << data.placeholderId[5] <<
		       "\n\tplaceholderId[6]: " << data.placeholderId[6] <<
		       "\n\tplaceholderId[7]: " << data.placeholderId[7] << endl;

}
//this is where we set bold/italic/etc and paragraph styles
void Powerpoint::opStyleTextPropAtom(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{

	Q_UINT16 totalLength = 0;
	Q_UINT16 length = 0;
	Q_UINT32 style1 = 0;
	Q_UINT32 style2 = 0;
	Q_UINT16 style3 = 0;
	Q_UINT32 BOLD 		= 0x00010000;
	Q_UINT32 ITALIC		= 0x00020000;
	Q_UINT32 UNDERLINE 	= 0x00040000;

	//--get the paragraph style?
	operands >> length >> style1 >> style2;
	totalLength += 10;

	kdDebug(s_area) << "\nopStyleTextPropAtom1:" <<
			"\n\tlength: " << length <<
			"\n\tstyle1: " << style1 <<
			"\n\tstyle2: " << style2 << endl;
	//--

	//--get the char styles
	while(bytes > totalLength)
	{
		length = 0;
		style1 = 0;
		style2 = 0;
		style3 = 0;

		operands >> length >> style1;
		totalLength += 6;
		if(style1 == 0)
		{
			operands >> style3;
			totalLength += 2;
		}
		else
		{
			operands >> style2;
			totalLength += 4;
		}

		kdDebug(s_area) << "\nopStyleTextPropAtom2:" <<
				"\n\tlength: " << length <<
				"\n\tstyle1: " << style1 <<
				"\n\tstyle2: " << style2 <<
				"\n\tstyle3: " << style3 << endl;
		if(style1 & BOLD)
			kdDebug(s_area) << "BOLD here" << endl;
		if(style1 & ITALIC)
			kdDebug(s_area) << "ITALIC here" << endl;
		if(style1 & UNDERLINE)
			kdDebug(s_area) << "UNDERLINE here" << endl;
	}
	//--
}

void Powerpoint::opTextBytesAtom(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    QString data;
    unsigned i;

    for (i = 0; i < bytes; i++)
    {
        Q_UINT8 tmp;

        operands >> tmp;
        data += tmp;
    }
     kdDebug(s_area) << "\nopTextBytesAtom: " <<
    		       "\n\tdata: " << data << endl;

    //SlideText *text;
    switch (m_pass)
    {
    case PASS_GET_SLIDE_REFERENCES:
        //text = new SlideText;
        //text->type = m_textType;
        //text->data = data;
        //m_slide->text.append(text);
	if(m_pptSlide)
	{
		m_pptSlide->addText(data, m_textType);
		kdDebug(s_area) << "Text Added: " << data << " type: " << m_textType << endl;
	}
	else
	     	kdDebug(s_area) << "WEVE GOTS US AN ERROR!!!" << endl;

        break;
    case PASS_GET_SLIDE_CONTENTS:
        break;
    };
}

void Powerpoint::opTextCharsAtom(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    QString data;
    unsigned i;

    for (i = 0; i < bytes / 2; i++)
    {
        Q_UINT16 tmp;

        operands >> tmp;
        data += tmp;
    }

    //SlideText *text;
    switch (m_pass)
    {
    case PASS_GET_SLIDE_REFERENCES:
        //text = new SlideText;
        //text->type = m_textType;
        //text->data = data;
        //m_slide->text.append(text);
	if(m_pptSlide)
	{
		m_pptSlide->addText(data, m_textType);
		kdDebug(s_area) << "Text Added: " << data << " type: " << m_textType << endl;
	}
	else
	     	kdDebug(s_area) << "WEVE GOTS US AN ERROR!!!" << endl;

        break;
    case PASS_GET_SLIDE_CONTENTS:
        break;
    };
}

void Powerpoint::opTextHeaderAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &operands)
{
    struct
    {
        Q_UINT32 txType;    // Type of text:
                            //
                            // 0 Title
                            // 1 Body
                            // 2 Notes
                            // 3 Not Used
                            // 4 Other (Text in a shape)
                            // 5 Center body (subtitle in title slide)
                            // 6 Center title (title in title slide)
                            // 7 Half body (body in two-column slide)
                            // 8 Quarter body (body in four-body slide)
     } data;

    operands >> data.txType;

    kdDebug(s_area) << "\nopTextHeaderAtom:" <<
    		       "\n\ttxType: " << data.txType << endl;

    switch (m_pass)
    {
    case PASS_GET_SLIDE_REFERENCES:
        m_textType = data.txType;
        break;
    case PASS_GET_SLIDE_CONTENTS:
        break;
    };
}

void Powerpoint::opTextSpecInfoAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &/*operands*/)
{
}

void Powerpoint::opTxMasterStyleAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &/*operands*/)
{
}

void Powerpoint::opTxSIStyleAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &/*operands*/)
{
}

//
// This routine is where the parse actually gets going. It should be the first
// structure encoutered in the main OLE stream.
//
void Powerpoint::opUserEditAtom(
    Header & /* op */,
    Q_UINT32 /*bytes*/,
    QDataStream &operands)
{

    operands >> mUserEditAtom.lastSlideID  >> mUserEditAtom.version >> mUserEditAtom.offsetLastEdit >>
        mUserEditAtom.offsetPersistDirectory >> mUserEditAtom.documentRef >>
        mUserEditAtom.maxPersistWritten >> mUserEditAtom.lastViewType;

    if(mEditOffset == 0)mEditOffset = mUserEditAtom.offsetLastEdit;

    kdDebug(s_area) << "\nPSR_UserEditAtom:" <<
                       "\n\tlastSlideID: " << mUserEditAtom.lastSlideID <<
		       "\n\tversion: " << mUserEditAtom.version <<
		       "\n\toffsetLastEdit: " << mUserEditAtom.offsetLastEdit <<
		       "\n\toffsetPersistDirectory: " << mUserEditAtom.offsetPersistDirectory <<
		       "\n\tdocumentRef: " << mUserEditAtom.documentRef <<
		       "\n\tmaxPersistWritten: " << mUserEditAtom.maxPersistWritten <<
		       "\n\tlastViewType: " << mUserEditAtom.lastViewType << endl;

    if (!m_documentRefFound)
    {
        m_documentRef = mUserEditAtom.documentRef;
        m_documentRefFound = true;
    }
    switch (m_pass)
    {
    case PASS_GET_SLIDE_REFERENCES:

        // Gather the persistent references. That should fill the list of
        // references which we then use to look up our document.

        walkRecord(mUserEditAtom.offsetPersistDirectory);

        // Now recursively walk the main OLE stream parsing previous edits.

        if (mUserEditAtom.offsetLastEdit)
        {
            m_editDepth++;
            walkRecord(mUserEditAtom.offsetLastEdit);
            m_editDepth--;
        }
        break;
    case PASS_GET_SLIDE_CONTENTS:
        break;
    };
}

void Powerpoint::opVBAInfo(
    Header & /* op */,
    Q_UINT32 bytes,
    QDataStream &operands)
{
    walk(bytes, operands);
}

void Powerpoint::skip(Q_UINT32 bytes, QDataStream &operands)
{
    if ((int)bytes < 0)
    {
        kdError(s_area) << "skip: " << (int)bytes << endl;
        return;
    }
    if (bytes)
    {
        Q_UINT32 i;
        Q_UINT8 discard;

        kdDebug(s_area) << "skip: " << bytes << endl;
        for (i = 0; i < bytes; i++)
        {
            operands >> discard;
        }
    }
}

//
// Handle a container record.
//
void Powerpoint::walk(Q_UINT32 bytes, QDataStream &operands)
{
    Header op;
    Q_UINT32 length = 0;

    // Stop parsing when there are no more records. Note that we stop as soon
    // as we cannot get a complete header.
    while (length + 8 <= bytes)
    {
        operands >> op.opcode.info >> op.type >> op.length;

        // If we get some duff data, protect ourselves.
        if (length + op.length + 8 > bytes)
        {
            op.length = bytes - length - 8;
        }
        length += op.length + 8;

        // Package the arguments...

	invokeHandler(op, op.length, operands);
    }

    // Eat unexpected data that the caller may expect us to consume.
    skip(bytes - length, operands);
}

void Powerpoint::walk(Q_UINT32 mainStreamOffset)
{
    Q_UINT32 length = m_mainStream.length - mainStreamOffset;
    QByteArray a;

    a.setRawData((const char *)m_mainStream.data + mainStreamOffset, length);
    QDataStream stream(a, IO_ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    walk(length, stream);
    a.resetRawData((const char *)m_mainStream.data + mainStreamOffset, length);
}
void Powerpoint::walkDocument()
{
    QByteArray a;
    Q_UINT32 mainStreamOffset = 0;
    Q_UINT32 bytes = m_mainStream.length - mainStreamOffset;

    a.setRawData((const char *)m_mainStream.data + mainStreamOffset, bytes);
    QDataStream stream(a, IO_ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    //--get tho the slide list
    Header op;
    Q_UINT32 length = 0;

    // Stop parsing when there are no more records. Note that we stop as soon
    // as we cannot get a complete header.
    while (length + 8 <= bytes && op.type != 1000)//document
    {
        stream >> op.opcode.info >> op.type >> op.length;

        // If we get some duff data, protect ourselves.
        if (length + op.length + 8 > bytes)
        {
            op.length = bytes - length - 8;
        }
        length += op.length + 8;
    }

    //--
    invokeHandler(op, op.length, stream);
    a.resetRawData((const char *)m_mainStream.data + mainStreamOffset, bytes);

}

void Powerpoint::walkRecord(Q_UINT32 bytes, const unsigned char *operands)
{
    // First read what should be the next header using one stream.

    kdError(s_area) << "WalkRecord - bytes: " << bytes << endl;

    Q_UINT32 length = sizeof(Header);
    QByteArray a;
    Header op;

    a.setRawData((const char *)operands, bytes);
    QDataStream stream1(a, IO_ReadOnly);
    stream1.setByteOrder(QDataStream::LittleEndian);
    stream1 >> op.opcode.info >> op.type >> op.length;
    a.resetRawData((const char *)operands, bytes);

    // Armed with the length, parse in the usual way using a second stream.

    length += op.length;
    a.setRawData((const char *)operands, length);
    QDataStream stream2(a, IO_ReadOnly);
    stream2.setByteOrder(QDataStream::LittleEndian);
    walk(length, stream2);
    a.resetRawData((const char *)operands, length);
}

void Powerpoint::walkRecord(Q_UINT32 mainStreamOffset)
{
    walkRecord(sizeof(Header), m_mainStream.data + mainStreamOffset);
}

void Powerpoint::walkReference(Q_UINT32 reference)
{
    if (m_persistentReferences.end() == m_persistentReferences.find(reference))
    {
        kdError(s_area) << "cannot find reference: " << reference << endl;
    }
    else
    {
        unsigned offset = m_persistentReferences[reference];
        kdDebug(s_area) << "found reference: " << reference <<
            " offset: " << offset << endl;
        walkRecord(offset);
	kdDebug(s_area) << "****************************" << endl;

    }
}
