/* writerperfect:
 *
 * Copyright (C) 2007 Fridrich Strba (fridrich.strba@bluewin.ch)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "OutputFileHelper.hxx"

#ifdef USE_GSF_OUTPUT
#include <gsf/gsf-utils.h>
#include <gsf/gsf-output-stdio.h>
#include <gsf/gsf-outfile.h>
#include <gsf/gsf-outfile-zip.h>
#include <gsf/gsf-input-stdio.h>
#else
#include "FemtoZip.hxx"
#endif

#include "DiskDocumentHandler.hxx"
#include "StdOutHandler.hxx"
#include <libwpd-stream/WPXStreamImplementation.h>

struct OutputFileHelperImpl
{
    OutputFileHelperImpl(const char *password) : mpPassword(password) {}
#ifdef USE_GSF_OUTPUT
    GsfOutfile *mpOutfile;
#else
    FemtoZip *mpOutfile;
#endif
    const char *mpPassword;
};


OutputFileHelper::OutputFileHelper(const char* outFileName, const char *password) :
#ifdef USE_GSF_OUTPUT
    m_impl(new OutputFileHelperImpl(password))
#else
    m_impl(new OutputFileHelperImpl(password))
#endif
{
    m_impl->mpOutfile = 0;
#ifdef USE_GSF_OUTPUT
    GsfOutput  *pOutput = 0;
    GError   *err = 0;

    gsf_init ();

    if (!outFileName)
            pOutput = 0;
    else
    {
            pOutput = GSF_OUTPUT(gsf_output_stdio_new (outFileName, &err));
            if (pOutput == 0) {
            if (err) {
                        g_warning ("'%s' error: %s", outFileName, err->message);
                            g_error_free (err);
            }
            gsf_shutdown ();
            }
        else {
            if (err)
                g_error_free (err);
            err = 0;
                m_impl->mpOutfile = GSF_OUTFILE(gsf_outfile_zip_new (pOutput, &err));
                if (m_impl->mpOutfile == 0) {
                if (err) {
                            g_warning ("'%s' error: %s",
                        "gsf_outfile_zip_new", err->message);
                                g_error_free (err);
                }
                gsf_shutdown ();
            }
            else {
                if (err)
                    g_error_free (err);
                err = 0;
                    g_object_unref (pOutput);
            }
        }
    }
#else
    if (outFileName)
        m_impl->mpOutfile = new FemtoZip(outFileName);
#endif
}

OutputFileHelper::~OutputFileHelper()
{
#ifdef USE_GSF_OUTPUT
    if (m_impl->mpOutfile && !gsf_output_close ((GsfOutput *) m_impl->mpOutfile))
        fprintf(stderr, "ERROR : Couldn't close outfile\n");

    if (m_impl->mpOutfile)
            g_object_unref (m_impl->mpOutfile);

    gsf_shutdown ();
#else
    if (m_impl->mpOutfile)
        delete m_impl->mpOutfile;
#endif
    if (m_impl)
        delete m_impl;
}

bool OutputFileHelper::writeChildFile(const char *childFileName, const char *str)
{
    if (!m_impl->mpOutfile)
        return true;
#ifdef USE_GSF_OUTPUT
    GsfOutput *child;
    if (0 != (child = gsf_outfile_new_child  (m_impl->mpOutfile, childFileName, false)))
    {
        bool res = gsf_output_puts (child, str) &&
            gsf_output_close (child);
        g_object_unref (child);
        return res;
    }
    return false;
#else
    m_impl->mpOutfile->createEntry(childFileName, 0);
    if (m_impl->mpOutfile->errorCode())
        return false;
    m_impl->mpOutfile->writeString(str);
    if (m_impl->mpOutfile->errorCode())
        return false;
    m_impl->mpOutfile->closeEntry();
    if (m_impl->mpOutfile->errorCode())
        return false;
    return true;
#endif
}

bool OutputFileHelper::writeChildFile(const char *childFileName, const char *str, const char compression_level)
{
    if (!m_impl->mpOutfile)
        return true;
#ifdef USE_GSF_OUTPUT
    GsfOutput *child;
#ifdef GSF_HAS_COMPRESSION_LEVEL
    if (0 != (child = gsf_outfile_new_child_full  (m_impl->mpOutfile, childFileName, false,"compression-level", compression_level, (void*)0)))
#else
    if (0 != (child = gsf_outfile_new_child  (m_impl->mpOutfile, childFileName, false)))
#endif
    {
        bool res = gsf_output_puts (child, str) &&
            gsf_output_close (child);
        g_object_unref (child);
        return res;
    }
    return false;
#else
    m_impl->mpOutfile->createEntry(childFileName, 0); // only storing without compressing works with FemtoZip
    if (m_impl->mpOutfile->errorCode())
        return false;
    m_impl->mpOutfile->writeString(str);
    if (m_impl->mpOutfile->errorCode())
        return false;
    m_impl->mpOutfile->closeEntry();
    if (m_impl->mpOutfile->errorCode())
        return false;
    return true;
#endif
}

bool OutputFileHelper::writeConvertedContent(const char *childFileName, const char *inFileName, const OdfStreamType streamType)
{
    WPXFileStream input(inFileName);

    if (!_isSupportedFormat(&input, m_impl->mpPassword))
        return false;

    input.seek(0, WPX_SEEK_SET);

    OdfDocumentHandler *pHandler;
#ifdef USE_GSF_OUTPUT
    GsfOutput *pContentChild = 0;
    if (m_impl->mpOutfile)
    {
            pContentChild = gsf_outfile_new_child(m_impl->mpOutfile, childFileName, false);
            pHandler = new DiskOdfDocumentHandler(pContentChild); // WLACH_REFACTORING: rename to DiskHandler
#else
    if (m_impl->mpOutfile)
    {
        m_impl->mpOutfile->createEntry(childFileName, 0);
        if (m_impl->mpOutfile->errorCode())
            return false;
        pHandler = new DiskOdfDocumentHandler(m_impl->mpOutfile);
#endif
    }
    else
            pHandler = new StdOutHandler();

    bool bRetVal = _convertDocument(&input, m_impl->mpPassword, pHandler, m_impl->mpOutfile ? streamType : ODF_FLAT_XML);

#ifdef USE_GSF_OUTPUT
    if (pContentChild)
    {
            gsf_output_close(pContentChild);
            g_object_unref(G_OBJECT (pContentChild));
    }

#else
    if (m_impl->mpOutfile)
        m_impl->mpOutfile->closeEntry();
#endif
    delete pHandler;

    return bRetVal;
}
