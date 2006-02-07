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


Q_UINT32
xcf_read_int32 (FILE    *fp,
		Q_INT32 *data,
		Q_INT32     count)
{
  Q_UINT32 total;

  total = count;
  if (count > 0)
    {
      xcf_read_int8 (fp, (Q_UINT8 *) data, count * 4);

      while (count--)
        {
          *data = g_ntohl (*data);
          data++;
        }
    }

  return total * 4;
}

Q_UINT32
xcf_read_float (FILE   *fp,
		float *data,
		Q_INT32    count)
{
  return xcf_read_int32 (fp, (Q_INT32 *) ((void *) data), count);
}

Q_UINT32
xcf_read_int8 (FILE   *fp,
	       Q_UINT8 *data,
	       Q_INT32    count)
{
  Q_UINT32 total;
  Q_INT32  bytes;

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

Q_UINT32
xcf_read_string (FILE   *fp,
		 QCString **data,
		 Q_INT32    count)
{
  Q_INT32 tmp;
  Q_UINT32   total;
  Q_INT32    i;

  total = 0;
  for (i = 0; i < count; i++)
    {
      total += xcf_read_int32 (fp, &tmp, 1);
      if (tmp > 0)
        {
          QCString *str;

          str = g_new (QCString, tmp);
          total += xcf_read_int8 (fp, (Q_UINT8*) str, tmp);

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
