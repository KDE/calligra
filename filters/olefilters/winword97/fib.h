/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <wtrobin@carinthia.com>

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
*/

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
    unsigned int lKey;
    unsigned char envr;
    unsigned char fMac:1;
    unsigned char fEmptySpecial:1;
    unsigned char fLoadOverridePage:1;
    unsigned char fFutureSavedUndo:1;
    unsigned char fWord97Saved:1;
    unsigned char fSpare0:3;
    unsigned short chs;    // 20
    unsigned short chsTables;
    int fcMin;
    int fcMac;
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
    int cbMac;
    int lProductCreated;
    int lProductRevised;  // 72
    int ccpText;
    int ccpFtn;     // 80
    int ccpHdd;
    int ccpMcr;
    int ccpAtn;   // 92
    int ccpEdn;
    int ccpTxbx;  // 100
    int ccpHdrTxbx;
    int pnFbpChpFirst;
    int pnChpFirst;   // 112
    int cpnBteChp;
    int pnFbpPapFirst;  // 120
    int pnPapFirst;
    int cpnBtePap;
    int pnFbpLvcFirst;  // 132
    int pnLvcFirst;
    int cpnBteLvc;   // 140
    int fcIslandFirst;
    int fcIslandLim;
    unsigned short cfclcb;  // 152
    int fcStshfOrig;
    unsigned int lcbStshfOrig;
    int fcStshf;  // 162
    unsigned int lcbStshf;
    int fcPlcffndRef;   // 170
    unsigned int lcbPlcffndRef;
    int fcPlcffndTxt;
    unsigned int lcbPlcffndTxt;   // 182
    int fcPlcfandRef;
    unsigned int lcbPlcfandRef;   // 190
    int fcPlcfandTxt;
    unsigned int lcbPlcfandTxt;
    int fcPlcfsed;   // 202
    unsigned int lcbPlcfsed;
    int fcPlcpad;    // 210
    unsigned int lcbPlcpad;
    int fcPlcfphe;
    unsigned int lcbPlcfphe;  // 222
    int fcSttbfglsy;
    unsigned int lcbSttbfglsy;  // 230
    int fcPlcfglsy;
    unsigned int lcbPlcfglsy;
    int fcPlcfhdd;     // 242
    unsigned int lcbPlcfhdd;
    int fcPlcfbteChpx; // 250
    unsigned int lcbPlcfbteChpx;
    int fcPlcfbtePapx;
    unsigned int lcbPlcfbtePapx; // 262
    int fcPlcfsea;
    unsigned int lcbPlcfsea; // 270
    int fcSttbfffn;
    unsigned int lcbSttbfffn;
    int fcPlcffldMom;   // 282
    unsigned int lcbPlcffldMom;
    int fcPlcffldHdr;   // 290
    unsigned int lcbPlcffldHdr;
    int fcPlcffldFtn;
    unsigned int lcbPlcffldFtn; // 302
    int fcPlcffldAtn;
    unsigned int lcbPlcffldAtn; // 310
    int fcPlcffldMcr;
    unsigned int lcbPlcffldMcr;
    int fcSttbfbkmk;
    unsigned int lcbSttbfbkmk;
    int fcPlcfbkf;  // 330
    unsigned int lcbPlcfbkf;
    int fcPlcfbkl;
    unsigned int lcbPlcfbkl;  // 342
    int fcCmds;
    unsigned int lcbCmds;  // 350
    int fcPlcmcr;
    unsigned int lcbPlcmcr;
    int fcSttbfmcr;    // 362
    unsigned int lcbSttbfmcr;
    int fcPrDrvr;    // 370
    unsigned int lcbPrDrvr;
    int fcPrEnvPort;
    unsigned int lcbPrEnvPort; // 382
    int fcPrEnvLand;
    unsigned int lcbPrEnvLand; // 390
    int fcWss;
    unsigned int lcbWss;
    int fcDop;  // 402
    unsigned int lcbDop;
    int fcSttbfAssoc;  // 410
    unsigned int lcbSttbfAssoc;
    int fcClx;
    unsigned int lcbClx;  // 422
    int fcPlcfpgdFtn;
    unsigned int lcbPlcfpgdFtn;  // 430
    int fcAutosaveSource;
    unsigned int lcbAutosaveSource;
    int fcGrpXstAtnOwners;  // 442
    unsigned int lcbGrpXstAtnOwners;
    int fcSttbfAtnbkmk;  // 450
    unsigned int lcbSttbfAtnbkmk;
    int fcPlcdoaMom;
    unsigned int lcbPlcdoaMom; // 462
    int fcPlcdoaHdr;
    unsigned int lcbPlcdoaHdr; // 470
    int fcPlcspaMom;
    unsigned int lcbPlcspaMom;
    int fcPlcspaHdr;  // 482
    unsigned int lcbPlcspaHdr;
    int fcPlcfAtnbkf; // 490
    unsigned int lcbPlcfAtnbkf;
    int fcPlcfAtnbkl;
    unsigned int lcbPlcfAtnbkl;  // 502
    int fcPms;
    unsigned int lcbPms;  // 510
    int fcFormFldSttbs;
    unsigned int lcbFormFldSttbs;
    int fcPlcfendRef;  // 522
    unsigned int lcbPlcfendRef;
    int fcPlcfendTxt;  // 530
    unsigned int lcbPlcfendTxt;
    int fcPlcffldEdn;
    unsigned int lcbPlcffldEdn;  // 542
    int fcPlcfpgdEdn;
    unsigned int lcbPlcfpgdEdn;  // 550
    int fcDggInfo;
    unsigned int lcbDggInfo;
    int fcSttbfRMark;  // 562
    unsigned int lcbSttbfRMark;
    int fcSttbCaption; // 570
    unsigned int lcbSttbCaption;
    int fcSttbAutoCaption;
    unsigned int lcbSttbAutoCaption;  // 582
    int fcPlcfwkb;
    unsigned int lcbPlcfwkb;  // 590
    int fcPlcfspl;
    unsigned int lcbPlcfspl;
    int fcPlcftxbxTxt;  // 602
    unsigned int lcbPclftxbxTxt;
    int fcPclffldTxbx;   // 610
    unsigned int lcbPlcffldTxbx;
    int fcPlcfhdrtxbxTxt;
    unsigned int lcbPlcfhdrtxbxTxt;  // 622
    int fcPlcffldHdrTxbx;
    unsigned int lcbPlcffldHdrTxbx;  // 630
    int fcStwUser;
    unsigned int lcbStwUser;
    int fcSttbttmbd;  // 642
    unsigned int lcbSttbttmbd;
    int fcUnused;  // 650
    unsigned int lcbUnused;
    int fcPgdMother;          // rgpgdbkd
    unsigned int lcbPgdMother;  // 662
    int fcBkdMother;
    unsigned int lcbBkdMother;  // 670
    int fcPdgFtn;
    unsigned int lcbPgdFtn;
    int fcBkdFtn;   // 682
    unsigned int lcbBkdFtn;
    int fcPgdEdn;  // 690
    unsigned int lcbPgdEdn;
    int fcBkdEdn;
    unsigned int lcbBkdEdn;  // 702
    int fcSttbfIntlFld;
    unsigned int lcbSttbfIntlFld; // 710
    int fcRouteSlip;
    unsigned int lcbRouteSlip;
    int fcSttbSavedBy;  // 722
    unsigned int lcbSttbSavedBy;
    int fcSttbFnm;  // 730
    unsigned int lcbSttbFnm;
    int fcPlcfLst;
    unsigned int lcbPlcfLst;  // 742
    int fcPlfLfo;
    unsigned int lcbPlfLfo; // 750
    int fcPlcftxbxBkd;
    unsigned int lcbPlcftxbxBkd;
    int fcPlcftxbxHdrBkd; // 762
    unsigned int lcbPlcftxbxHdrBkd;
    int fcDocUndo;  // 770
    unsigned int lcbDocUndo;
    int fcRgbuse;
    unsigned int lcbRgbuse;  // 782
    int fcUsp;
    unsigned int lcbUsp; // 790
    int fcUskf;
    unsigned int lcbUskf;
    int fcPlcupcRgbuse;  // 802
    unsigned int lcbPlcupcRgbuse;
    int fcPlcupcUsp;  // 810
    unsigned int lcbPlcupcUsp;
    int fcSttbGlsyStyle;
    unsigned int lcbSttbGlsyStyle; // 822
    int fcPlgosl;
    unsigned int lcbPlgosl;  // 830
    int fcPlcocx;
    unsigned int lcbPlcocx;
    int fcPlcfbteLvc;  // 842
    unsigned int lcbPlcfbteLvc;
    unsigned int dwLowDateTime; // 850, FILETIME
    unsigned int dwHighDateTime;
    int fcPlcflvc;
    unsigned int lcbPlcflvc; // 862
    int fcPlcasumy;
    unsigned int lcbPlcasumy; // 870
    int fcPlcfgram;
    unsigned int lcbPlcfgram;
    int fcSttbListNames;  // 882
    unsigned int lcbSttbListNames;
    int fcSttbfUssr;   // 890
    unsigned int lcbSttbfUssr;

    bool ok;
};
#endif // FIB_H
