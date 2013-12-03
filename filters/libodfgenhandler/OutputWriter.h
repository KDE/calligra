/* This file is part of the Calligra project.
 *
 * Copyright 2013 Yue Liu <yue.liu@mail.com>
 *
 * Based on writerperfect code, major Contributor(s):
 *
 *  Copyright (C) 2007 Fridrich Strba (fridrich.strba@bluewin.ch)
 *
 * The contents of this file may be used under the terms
 * of the GNU Lesser General Public License Version 2.1 or later
 * (LGPLv2.1+), in which case the provisions of the LGPLv2.1+ are
 * applicable instead of those above.
 *
 * For further information visit http://libwpd.sourceforge.net
 */

#ifndef OUTPUTWRITER_H
#define OUTPUTWRITER_H

#include <libodfgen/OdfDocumentHandler.hxx>

class KoStore;
class WPXInputStream;

class OutputWriter
{
public:
    explicit OutputWriter(KoStore* store);
    virtual ~OutputWriter();

    bool writeChildFile(const char *childFileName, const char *str);
    bool writeChildFile(const char *childFileName, const char *str, const char);
    bool writeConvertedContent(const char *childFileName,
                               const char *inFileName,
                               const OdfStreamType streamType);

private:
    OutputWriter(OutputWriter const &);
    OutputWriter &operator=(OutputWriter const &);

    virtual bool _isSupportedFormat(WPXInputStream *input) = 0;
    virtual bool _convertDocument(WPXInputStream *input,
                                  OdfDocumentHandler *handler,
                                  const OdfStreamType streamType) = 0;
    KoStore* m_store;
};

#endif // OUTPUTWRITER_H

