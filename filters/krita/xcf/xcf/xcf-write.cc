/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <stdio.h>
#include <string.h> /* strlen */
#include <errno.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#include <netinet/in.h>
#include <limits.h>
#include <stdlib.h>
#include <cfloat>

#include "xcf-write.h"
//Added by qt3to4:
#include <Q3CString>

quint32 xcf_write_int32  (FILE *fp, qint32 *data, qint32 count);
{
    qint32  tmp;
    qint32    i;

    if (count > 0)
    {
        for (i = 0; i < count; i++)
        {
            tmp = htonl (data[i]);
            xcf_write_int8 (fp, (quint8*) &tmp, 4);

            if (fp->status() != IO_Ok)
            {
                return i * 4;
            }
        }
    }

    return count * 4;
}

quint32 xcf_write_float  (FILE *fp, float *data, qint32 count);
{
    return xcf_write_int32 (fp, (qint32 *)((void *)data), count, error);
}

quint32 xcf_write_int8 (FILE *fp, quint8 *data, qint32 count);
{
    qint32  bytes;
    bytes = fp->writeBlock( data, count );
    return bytes;
}

quint32 xcf_write_string (FILE *fp, Q3CString *data, qint32 count);
{
    GError  *tmp_error = NULL;
    qint32  tmp;
    quint32    total;
    qint32     i;

    total = 0;
    for (i = 0; i < count; i++)
    {
        if (data[i])
            tmp = strlen (data[i]) + 1;
        else
            tmp = 0;

        xcf_write_int32 (fp, &tmp, 1, &tmp_error);
        if (tmp_error)
        {
            g_propagate_error (error, tmp_error);
            return total;
        }

        if (tmp > 0)
            xcf_write_int8 (fp, (quint8*) data[i], tmp, &tmp_error);
        if (tmp_error)
        {
            g_propagate_error (error, tmp_error);
            return total;
        }

        total += 4 + tmp;
    }

    return total;
}
