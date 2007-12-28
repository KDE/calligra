/*
 *  Copyright (c) 2007 Emanuele Tamponi <emanuele@valinor.it>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_kslin_colorspace.h"
#include "kis_ks_colorspace.h"

#include "channel_converter.h"
#include "kis_illuminant_profile.h"
#include <KoColorProfile.h>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_cblas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

KisKSLinColorSpace::KisKSLinColorSpace(KoColorProfile *p)
: parent(p, "KS3LINEARF32", i18n("3-pairs Absorption-Scattering Linear (32 Bits Float)")), m_inverse(0)
{
    if (!profileIsCompatible(p))
        return;

    int s;
    gsl_permutation *perm = gsl_permutation_alloc(3);
    gsl_matrix *tmp = gsl_matrix_alloc(m_profile->T()->size1, m_profile->T()->size2);

    m_inverse = gsl_matrix_alloc(tmp->size1, tmp->size2);

    gsl_matrix_memcpy(tmp, m_profile->T());
    gsl_linalg_LU_decomp(tmp, perm, &s);
    gsl_linalg_LU_invert(tmp, perm, m_inverse);

    gsl_permutation_free(perm);
    gsl_matrix_free(tmp);
}

KisKSLinColorSpace::~KisKSLinColorSpace()
{
    if (m_inverse)
        gsl_matrix_free(m_inverse);
}

void KisKSLinColorSpace::RGBToReflectance() const
{
    gsl_blas_dgemv(CblasNoTrans, 1.0, m_inverse, m_rgbvec, 0.0, m_refvec);
}
