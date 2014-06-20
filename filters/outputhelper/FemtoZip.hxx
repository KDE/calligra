/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* femtozip - superlightweight class to create a compressed ZIP archive
   Copyright (C) 2005-2006 Ariya Hidayat <ariya@kde.org>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   * Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   * Neither the name of the authors nor the names of its contributors may be
     used to endorse or promote products derived from this software without
     specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
   THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef USE_GSF_OUTPUT

#ifndef FEMTOZIP_H
#define FEMTOZIP_H

class FemtoZipPrivate;

/*!
	FemtoZip implements a ZIP archive.
*/

class FemtoZip
{
public:

	explicit FemtoZip(const char *zipfile);
	~FemtoZip();

	void createEntry(const char *name, int compressionLevel = 3);

	void writeString(const char *str);

	void closeEntry();

	enum
	{
		NoError = 0,
		ErrorCreateZip = 1,
		ErrorWriteData = 2
	};

	/*!
		Returns the error code of the latest operation.
	*/
	int errorCode() const;

private:
	// no copy and assign allowed
	FemtoZip(const FemtoZip &);
	FemtoZip &operator=(const FemtoZip &);

	FemtoZipPrivate *d;
};

#endif // FEMTOZIP_H

#endif // USE_GSF_OUTPUT

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
