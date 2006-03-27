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

#include <glib-object.h>

#include "libgimpbase/gimpbase.h"

#include "xcf-read.h"

#include "gimp-intl.h"
//Added by qt3to4:
#include <Q3CString>


quint32
xcf_read_int32 (FILE    *fp,
		qint32 *data,
		qint32     count)
{
  quint32 total;

  total = count;
  if (count > 0)
    {
      xcf_read_int8 (fp, (quint8 *) data, count * 4);

      while (count--)
        {
          *data = g_ntohl (*data);
          data++;
        }
    }

  return total * 4;
}

quint32
xcf_read_float (FILE   *fp,
		float *data,
		qint32    count)
{
  return xcf_read_int32 (fp, (qint32 *) ((void *) data), count);
}

quint32
xcf_read_int8 (FILE   *fp,
	       quint8 *data,
	       qint32    count)
{
  quint32 total;
  qint32  bytes;

  total = count;
  while (count > 0)
    {
      bytes = fread ((char *) data, sizeof (char), count, fp);
      if (bytes <= 0) /* something bad happened */
        break;
      count -= bytes;
      data += bytes;
    }

  return total;
}

quint32
xcf_read_string (FILE   *fp,
		 Q3CString **data,
		 qint32    count)
{
  qint32 tmp;
  quint32   total;
  qint32    i;

  total = 0;
  for (i = 0; i < count; i++)
    {
      total += xcf_read_int32 (fp, &tmp, 1);
      if (tmp > 0)
        {
          Q3CString *str;

          str = g_new (Q3CString, tmp);
          total += xcf_read_int8 (fp, (quint8*) str, tmp);

          if (str[tmp - 1] != '\0')
            str[tmp - 1] = '\0';

          data[i] = gimp_any_to_utf8 (str, -1,
                                      _("Invalid UTF-8 string in XCF file"));

          g_free (str);
        }
      else
        {
          data[i] = NULL;
        }
    }

  return total;
}
