
// FIB, File Information Block -> header, right at the beginning of each
// .doc-file
// Names of the variables corresponding to the ones in the file format
// description (can be found at www.wotsit.com or just send a mail :)

#ifndef FIB_H
#define FIB_H

struct FIB {
    unsigned short wIdent;    // 0, fibh
    unsigned short nFib;
    unsigned short nProduct;
    unsigned short lid;
    short pnNext;
    unsigned short fDot:1;      // 10
    unsigned short fGlsy:1;
    unsigned short fComplex:1;
    unsigned short fHasPic:1;
    unsigned short cQuickSaves:4;
    unsigned short fEncrypted:1;
    unsigned short fWhichTblStm:1;
    unsigned short fReadOnlyRecommended:1;
    unsigned short fWriteReservation:1;
    unsigned short fExtChar:1;
    unsigned short fLoadOverride:1;
    unsigned short fFarEast:1;
    unsigned short fCrypto:1;
    unsigned short nFibBack; 
    unsigned long lKey;
    unsigned char envr;
    unsigned char fMac:1;
    unsigned char fEmptySpecial:1;
    unsigned char fLoadOverridePage:1;
    unsigned char fFutureSavedUndo:1;
    unsigned char fWord97Saved:1;
    unsigned char fSpare0:3;
    unsigned short chs;    // 20
    unsigned short chsTables;
    long fcMin;
    long fcMac;
    unsigned short csw;   // 32
    unsigned short wMagicCreated;   // rgsw
    unsigned short wMagicRevised;
    unsigned short wMagicCreatedPrivate;
    unsigned short wMagicRevisedPrivate; // 40
    short pnFbpChpFirst_W6;
    short pnChpFirst_W6;
    short cpnBteChp_W6;
    short pnFbpPapFirst_W6;
    short pnPapFirst_W6;      // 50
    short cpnBtePap_W6;
    short pnFbpLvcFirst_W6;
    short pnLvcFirst_W6;
    short cpnBteLvc_W6;
    short lidFE;   // 60
    unsigned short clw;
    long cbMac;
    long lProductCreated;
    long lProductRevised;  // 72
    long ccpText;
    long ccpFtn;     // 80
    long ccpHdd;
    long ccpMcr;
    long ccpAtn;   // 92
    long ccpEdn;
    long ccpTxbx;  // 100
    long ccpHdrTxbx;
    long pnFbpChpFirst;
    long pnChpFirst;   // 112
    long cpnBteChp;
    long pnFbpPapFirst;  // 120
    long pnPapFirst;
    long cpnBtePap;
    long pnFbpLvcFirst;  // 132
    long pnLvcFirst;
    long cpnBteLvc;   // 140
    long fcIslandFirst;
    long fcIslandLim;
    unsigned short cfclcb;  // 152
    long fcStshfOrig;
    unsigned long lcbStshfOrig;
    long fcStshf;  // 162
    unsigned long lcbStshf;
    long fcPlcffndRef;   // 170
    unsigned long lcbPlcffndRef;
    long fcPlcffndTxt;
    unsigned long lcbPlcffndTxt;   // 182
    long fcPlcfandRef;
    unsigned long lcbPlcfandRef;   // 190
    long fcPlcfandTxt;
    unsigned long lcbPlcfandTxt;
    long fcPlcfsed;   // 202
    unsigned long lcbPlcfsed;
    long fcPlcpad;    // 210
    unsigned long lcbPlcpad;
    long fcPlcfphe;
    unsigned long lcbPlcfphe;  // 222
    long fcSttbfglsy;
    unsigned long lcbSttbfglsy;  // 230
    long fcPlcfglsy;
    unsigned long lcbPlcfglsy;
    long fcPlcfhdd;     // 242
    unsigned long lcbPlcfhdd;
    long fcPlcfbteChpx; // 250
    unsigned long lcbPlcfbteChpx;
    long fcPlcfbtePapx;
    unsigned long lcbPlcfbtePapx; // 262
    long fcPlcfsea;
    unsigned long lcbPlcfsea; // 270
    long fcSttbfffn;
    unsigned long lcbSttbfffn;
    long fcPlcffldMom;   // 282
    unsigned long lcbPlcffldMom;
    long fcPlcffldHdr;   // 290
    unsigned long lcbPlcffldHdr;
    long fcPlcffldFtn;
    unsigned long lcbPlcffldFtn; // 302
    long fcPlcffldAtn;
    unsigned long lcbPlcffldAtn; // 310
    long fcPlcffldMcr;
    unsigned long lcbPlcffldMcr;
    long fcSttbfbkmk;
    unsigned long lcbSttbfbkmk;
    long fcPlcfbkf;  // 330
    unsigned long lcbPlcfbkf;
    long fcPlcfbkl;
    unsigned long lcbPlcfbkl;  // 342
    long fcCmds;
    unsigned long lcbCmds;  // 350
    long fcPlcmcr;
    unsigned long lcbPlcmcr;
    long fcSttbfmcr;    // 362
    unsigned long lcbSttbfmcr;
    long fcPrDrvr;    // 370
    unsigned long lcbPrDrvr;
    long fcPrEnvPort;
    unsigned long lcbPrEnvPort; // 382
    long fcPrEnvLand;
    unsigned long lcbPrEnvLand; // 390
    long fcWss;
    unsigned long lcbWss;
    long fcDop;  // 402
    unsigned long lcbDop;
    long fcSttbfAssoc;  // 410
    unsigned long lcbSttbfAssoc;
    long fcClx;
    unsigned long lcbClx;  // 422
    long fcPlcfpgdFtn;
    unsigned long lcbPlcfpgdFtn;  // 430
    long fcAutosaveSource;
    unsigned long lcbAutosaveSource;
    long fcGrpXstAtnOwners;  // 442
    unsigned long lcbGrpXstAtnOwners;
    long fcSttbfAtnbkmk;  // 450
    unsigned long lcbSttbfAtnbkmk;
    long fcPlcdoaMom;
    unsigned long lcbPlcdoaMom; // 462
    long fcPlcdoaHdr;
    unsigned long lcbPlcdoaHdr; // 470
    long fcPlcspaMom;
    unsigned long lcbPlcspaMom;
    long fcPlcspaHdr;  // 482
    unsigned long lcbPlcspaHdr;
    long fcPlcfAtnbkf; // 490
    unsigned long lcbPlcfAtnbkf;
    long fcPlcfAtnbkl;
    unsigned long lcbPlcfAtnbkl;  // 502
    long fcPms;
    unsigned long lcbPms;  // 510
    long fcFormFldSttbs;
    unsigned long lcbFormFldSttbs;
    long fcPlcfendRef;  // 522
    unsigned long lcbPlcfendRef;
    long fcPlcfendTxt;  // 530
    unsigned long lcbPlcfendTxt;
    long fcPlcffldEdn;
    unsigned long lcbPlcffldEdn;  // 542
    long fcPlcfpgdEdn;
    unsigned long lcbPlcfpgdEdn;  // 550
    long fcDggInfo;
    unsigned long lcbDggInfo;
    long fcSttbfRMark;  // 562
    unsigned long lcbSttbfRMark;
    long fcSttbCaption; // 570
    unsigned long lcbSttbCaption;
    long fcSttbAutoCaption;
    unsigned long lcbSttbAutoCaption;  // 582
    long fcPlcfwkb;
    unsigned long lcbPlcfwkb;  // 590
    long fcPlcfspl;
    unsigned long lcbPlcfspl;
    long fcPlcftxbxTxt;  // 602
    unsigned long lcbPclftxbxTxt;
    long fcPclffldTxbx;   // 610
    unsigned long lcbPlcffldTxbx;
    long fcPlcfhdrtxbxTxt;
    unsigned long lcbPlcfhdrtxbxTxt;  // 622
    long fcPlcffldHdrTxbx;
    unsigned long lcbPlcffldHdrTxbx;  // 630
    long fcStwUser;
    unsigned long lcbStwUser;
    long fcSttbttmbd;  // 642
    unsigned long lcbSttbttmbd;
    long fcUnused;  // 650
    unsigned long lcbUnused;
    long fcPgdMother;          // rgpgdbkd
    unsigned long lcbPgdMother;  // 662
    long fcBkdMother;
    unsigned long lcbBkdMother;  // 670
    long fcPdgFtn;
    unsigned long lcbPgdFtn;
    long fcBkdFtn;   // 682
    unsigned long lcbBkdFtn;
    long fcPgdEdn;  // 690
    unsigned long lcbPgdEdn;
    long fcBkdEdn;
    unsigned long lcbBkdEdn;  // 702
    long fcSttbfIntlFld;
    unsigned long lcbSttbfIntlFld; // 710
    long fcRouteSlip;
    unsigned long lcbRouteSlip;
    long fcSttbSavedBy;  // 722
    unsigned long lcbSttbSavedBy;
    long fcSttbFnm;  // 730
    unsigned long lcbSttbFnm;
    long fcPlcfLst;
    unsigned long lcbPlcfLst;  // 742
    long fcPlfLfo;
    unsigned long lcbPlfLfo; // 750
    long fcPlcftxbxBkd;
    unsigned long lcbPlcftxbxBkd;
    long fcPlcftxbxHdrBkd; // 762
    unsigned long lcbPlcftxbxHdrBkd;
    long fcDocUndo;  // 770
    unsigned long lcbDocUndo;
    long fcRgbuse;
    unsigned long lcbRgbuse;  // 782
    long fcUsp;
    unsigned long lcbUsp; // 790
    long fcUskf;
    unsigned long lcbUskf;
    long fcPlcupcRgbuse;  // 802
    unsigned long lcbPlcupcRgbuse;
    long fcPlcupcUsp;  // 810
    unsigned long lcbPlcupcUsp;
    long fcSttbGlsyStyle;
    unsigned long lcbSttbGlsyStyle; // 822
    long fcPlgosl;
    unsigned long lcbPlgosl;  // 830
    long fcPlcocx;
    unsigned long lcbPlcocx;
    long fcPlcfbteLvc;  // 842
    unsigned long lcbPlcfbteLvc;
    unsigned long dwLowDateTime; // 850, FILETIME
    unsigned long dwHighDateTime;
    long fcPlcflvc;
    unsigned long lcbPlcflvc; // 862
    long fcPlcasumy;
    unsigned long lcbPlcasumy; // 870
    long fcPlcfgram;
    unsigned long lcbPlcfgram;
    long fcSttbListNames;  // 882
    unsigned long lcbSttbListNames;
    long fcSttbfUssr;   // 890
    unsigned long lcbSttbfUssr;
};
#endif // FIB_H
