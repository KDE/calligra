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

#include <cmath>

extern "C" {
   #include "glpk.h"
}

#include <gmm/gmm.h>
#include <lcms.h>

#include <QtAlgorithms>
#include <QString>

#include <klocale.h>

#include "KoColorSpaceRegistry.h"
#include "KoColorProfile.h"
#include "KoFallBack.h"

#include "compositeops/KoCompositeOpOver.h"
#include "compositeops/KoCompositeOpErase.h"
#include "compositeops/KoCompositeOpMultiply.h"
#include "compositeops/KoCompositeOpDivide.h"
#include "compositeops/KoCompositeOpBurn.h"

#include "kis_illuminant_profile.h"

#include "kis_ks_colorspace.h"

using namespace std;

KisKSColorSpace::KisKSColorSpace(KoColorProfile *p)
	: KoIncompleteColorSpace<KSFloatTraits, KoRGB16Fallback>("kscolorspace", "", KoColorSpaceRegistry::instance())
{
	if (profileIsCompatible(p))
		m_profile = dynamic_cast<KisIlluminantProfile*>(p);

	const quint32 ncols = m_profile->matrix().ncols();

	for (quint32 i = 0; i < 2*ncols; i+=2) {
		addChannel(new KoChannelInfo(i18n("Absorption"),
				   i+0 * sizeof(float),
				   KoChannelInfo::COLOR,
				   KoChannelInfo::FLOAT32,
				   sizeof(float),
				   QColor(0,0,255)));

		addChannel(new KoChannelInfo(i18n("Scattering"),
				   i+1 * sizeof(float),
				   KoChannelInfo::COLOR,
				   KoChannelInfo::FLOAT32,
				   sizeof(float),
				   QColor(255,0,0)));
	}

	addChannel(new KoChannelInfo(i18n("Alpha"),
			   2 * ncols * sizeof(float),
			   KoChannelInfo::ALPHA,
			   KoChannelInfo::FLOAT32,
			   sizeof(float)));

	addCompositeOp( new KoCompositeOpOver<KSFloatTraits>( this ) );
	addCompositeOp( new KoCompositeOpErase<KSFloatTraits>( this ) );
	addCompositeOp( new KoCompositeOpMultiply<KSFloatTraits>( this ) );
	addCompositeOp( new KoCompositeOpDivide<KSFloatTraits>( this ) );
	addCompositeOp( new KoCompositeOpBurn<KSFloatTraits>( this ) );

	hsRGB = cmsCreate_sRGBProfile();
	hXYZ  = cmsCreateXYZProfile();

	XYZ_BGR = cmsCreateTransform(hXYZ, TYPE_XYZ_DBL, hsRGB, TYPE_BGR_16,
								 INTENT_ABSOLUTE_COLORIMETRIC, cmsFLAGS_NOTPRECALC);
	BGR_XYZ = cmsCreateTransform(hsRGB, TYPE_BGR_16, hXYZ, TYPE_XYZ_DBL,
								 INTENT_ABSOLUTE_COLORIMETRIC, cmsFLAGS_NOTPRECALC);

	m_cache.reserve(1000);
}

bool KisKSColorSpace::profileIsCompatible(KoColorProfile* profile) const
{
	KisIlluminantProfile *test = dynamic_cast<KisIlluminantProfile*>(profile);
	if (!test)
		return false;

	return true;
}

void simplex(const gmm::dense_matrix<float> &M, vector<float> &X, const vector<float> &B)
{
	cout << "XYZ IN INGRESSO: " << B << endl;

	LPX *lp;

	lp = lpx_create_prob();
	lpx_set_prob_name(lp, "XYZ2REF");
	lpx_set_obj_dir(lp, LPX_MIN);

	lpx_add_rows(lp, 3);

	lpx_set_row_name(lp, 1, "X");
	lpx_set_row_name(lp, 2, "Y");
	lpx_set_row_name(lp, 3, "Z");

	for (int i = 0; i < 3; i++)
		lpx_set_row_bnds(lp, i+1, LPX_FX, B[i], B[i]);

	lpx_add_cols(lp, X.size());

	for (uint i = 0; i < X.size(); i++) {
		lpx_set_col_name(lp, i+1, QString("R"+QString::number(i+1)).toAscii().data());
		lpx_set_col_bnds(lp, i+1, LPX_DB, 0.00001, 0.99999);
		lpx_set_obj_coef(lp, i+1, 1.0);
	}

	int ind[X.size()+1];
	for (uint i = 1; i <= X.size(); i++)
		ind[i] = i;

	for (int i = 0; i < 3; i++) {
		double row[X.size()+1];
		for (int j = 0; j < X.size(); j++) {
			if (M(i, j))
				row[j+1] = M(i, j);
			else
				row[j+1] = 1e-11;
		}
		lpx_set_mat_row(lp, i+1, X.size(), ind, row);
	}
	lpx_simplex(lp);

	for (int i = 0; i < X.size(); i++)
		X[i] = lpx_get_col_prim(lp, i+1);

	vector<float> test(3);
	gmm::mult(M, X, test);
	cout << "COMPUTED XYZ: " << test << endl;
}
/*
void simplex(const gmm::dense_matrix<float> &M_1, vector<float> &X, const vector<float> &B)
{
	const float MAX_RATIO = 100000000.0;
	const uint rows = 3 + X.size() + X.size() + 1;
	const uint cols = X.size() + X.size() + X.size() + 1 + 1;
	gmm::dense_matrix<float> M(rows, cols);

	gmm::clear(M);

	for (uint i = 0; i < 3; i++) {
		for (uint j = 0; j < X.size(); j++) {
			M(i, j) = M_1(i, j);
		}
		M(i, cols-1) = B[i];
	}

	for (uint i = 3; i < 3 + X.size(); i++) {
		M(i, i-3) = M(i+X.size(), i-3) = M(i, (i+X.size())-3) = 1;
		M(i + X.size(), (i+2*X.size())-3) = -1;
		M(i, cols-1) = 0.999;
		M(i + X.size(), cols-1) = 0.001;
	}

	for (uint j = 0; j < X.size(); j++)
		M(rows-1, j) = -1;

	M(rows-1, cols-2) = 1;

	// Transform to a standard maximization problem
	while ( true ) {
		float max = 0.0;
		float ratio, ratio_min = MAX_RATIO;
		int i_starred = -1, i_pivot = -1, j_pivot = -1;
		for (uint j = 0; j < cols-1; j++) {
			uint count = 0, i_useful;
			for (uint i = 0; i < rows; i++) {
				if (M(i, j) != 0.0) {
					i_useful = i;
					count++;
				}
			}
			if (count == 1 && (M(i_useful, cols-1) / M(i_useful, j)) < 0) {
				i_starred = i_useful;
				break;
			}
		}

		if (i_starred < 0)
			break;

		for (uint j = 0; j < cols-1; j++) {
			if (M(i_starred, j) > max) {
				for (uint i = 0; i < rows; i++) {
					if (M(i, j) <= 0)
						continue;
					ratio = M(i, cols-1) / M(i, j);
					if ( ratio <= ratio_min ) {
						ratio_min = ratio;
						max = M(i_starred, j);
						i_pivot = i;
						j_pivot = j;
					}
				}
			}
		}

		if (max == 0.0) {
			for (uint i = 0; i < X.size(); i++)
				X[i] = 0.001;
			return;
		}

		for (int i = 0; i < rows; i++) {
			if (i == i_pivot || M(i, j_pivot) == 0.0)
				continue;
			float sour = M(i_pivot, j_pivot);
			float dest = M(i, j_pivot);
			for (uint j = 0; j < cols; j++) {
				if (j == j_pivot) {
					M(i, j) = 0;
					continue;
				}
				M(i, j) = sour * M(i, j) - dest * M(i_pivot, j);
			}
		}
	}

	while ( true ) {
		float min = 0.0;
		float ratio, ratio_min = MAX_RATIO;
		int i_pivot = -1, j_pivot = -1;
		for (uint j = 0; j < cols-1; j++) {
			if (M(rows-1, j) < min) {
				for (uint i = 0; i < rows-1; i++) {
					if (M(i, j) <= 0)
						continue;
					ratio = M(i, cols-1) / M(i, j);
					if ( ratio <= ratio_min ) {
						ratio_min = ratio;
						min = M(rows-1, j);
						i_pivot = i;
						j_pivot = j;
					}
				}
			}
		}

		if (min == 0.0)
			break;

		if (i_pivot < 0 || j_pivot < 0)
			return;

		for (int i = 0; i < rows; i++) {
			if (i == i_pivot || M(i, j_pivot) == 0.0)
				continue;
			float sour = M(i_pivot, j_pivot);
			float dest = M(i, j_pivot);
			for (uint j = 0; j < cols; j++) {
				if (j == j_pivot) {
					M(i, j) = 0;
					continue;
				}
				M(i, j) = sour * M(i, j) - dest * M(i_pivot, j);
			}
		}
	}

	for (uint j = 0; j < X.size(); j++) {
		int count = 0, i_useful;
		for (uint i = 0; i < rows; i++) {
			if (M(i, j) != 0.0) {
				i_useful = i;
				count++;
			}
		}
		if (count != 1)
			X[j] = 0;
		else
			X[j] = M(i_useful, cols-1) / M(i_useful, j);
	}
// 	cout << "RESULT: " << X << endl;
}

void simplex(const gmm::dense_matrix<float> &M_1, vector<float> &X, const vector<float> &B)
{
	const float MAX_RATIO = 100000000.0;
	const int rows = 3 + X.size() + 1;
	const int cols = X.size() + X.size() + 1 + 1;
	gmm::dense_matrix<float> M(rows, cols);

	gmm::clear(M);

	for (int i = 0; i < 3; i++) {
		for (unsigned int j = 0; j < X.size(); j++) {
			M(i, j) = M_1(i, j);
		}
		M(i, cols-1) = B[i];
	}

	for (int i = 3; i < rows - 1; i++) {
		M(i, i-3) = M(i, (i+X.size())-3) = 1;
		M(i, cols-1) = 1;
	}

	for (unsigned int j = 0; j < X.size(); j++)
		M(rows-1, j) = -1;

	M(rows-1, cols-2) = 1;

	while ( true ) {
		float min = 0.0;
		float ratio, ratio_min = MAX_RATIO;
		int i_pivot = -1, j_pivot = -1;
		for (int j = 0; j < cols-1; j++) {
			if (M(rows-1, j) < min) {
				for (int i = 0; i < rows-1; i++) {
					if (M(i, j) <= 0)
						continue;
					ratio = M(i, cols-1) / M(i, j);
					if ( ratio <= ratio_min ) {
						ratio_min = ratio;
						min = M(rows-1, j);
						i_pivot = i;
						j_pivot = j;
					}
				}
			}
		}

		if (min == 0.0)
			break;

		for (int i = 0; i < rows; i++) {
			if (i == i_pivot || M(i, j_pivot) == 0.0)
				continue;
			float sour = M(i_pivot, j_pivot);
			float dest = M(i, j_pivot);
			for (int j = 0; j < cols; j++) {
				if (j == j_pivot) {
					M(i, j) = 0;
					continue;
				}
				M(i, j) = sour * M(i, j) - dest * M(i_pivot, j);
			}
		}
	}

	for (unsigned int j = 0; j < X.size(); j++) {
		int count = 0, i_useful;
		for (int i = 0; i < rows; i++) {
			if (M(i, j) != 0.0) {
				i_useful = i;
				count++;
			}
		}
		if (count != 1)
			X[j] = 0;
		else
			X[j] = M(i_useful, cols-1) / M(i_useful, j);
	}
}
*/

double coth(double z)
{
	return ( cosh(z) / sinh(z) );
}

double acoth(double z)
{
	return 0.5*log((1 + 1/z) / (1 - 1/z));
}

void computeKS(const vector<float> &vREF, vector<float> &vKS)
{
	double R, Rb, a, b, K, S;
	vector<float>::iterator i = vKS.begin();
	for (vector<float>::const_iterator j = vREF.begin(); j != vREF.end(); i += 2, j++) {
		R = *j;
		if (R > 0.99999) R = 0.99999;
		if (R < 0.00001) R = 0.00001;

		Rb = R - 0.000001;

		a = 0.5 * ( R + ( Rb - R + 1) / Rb );
		b = sqrt( a*a - 1 );

		S = (1 / b) * acoth( ( b*b - ( a - R ) * ( a - 1 ) ) / ( b * ( 1 - R ) ) );
		K = S * ( a - 1 );

		*(i+0) = K;
		*(i+1) = S;
	}
}

/*
void computeKS(const vector<float> &vREF, vector<float> &vKS)
{
	double R, K, S;
	vector<float>::iterator i = vKS.begin();
	for (vector<float>::const_iterator j = vREF.begin(); j != vREF.end(); i += 2, j++) {
		R = *j;
		if (R > 0.99999) R = 0.99999;
		if (R < 0.00001) R = 0.00001;
		K = ( pow( R - 1, 2 ) ) / ( 2 * R );
		S = 1;

		*(i+0) = K;
		*(i+1) = S;
	}
}
*/
/*
void computeKS(const vector<float> &vREF, vector<float> &vKS)
{
	vector<float>::iterator i = vKS.begin();
	for (vector<float>::const_iterator j = vREF.begin(); j != vREF.end(); i += 2, j++) {
		*(i+0) = (*j);
		*(i+1) = 1;
	}
}
*/

#define NORMALIZATION 65535.0

void KisKSColorSpace::fromRgbA16(const quint8 * srcU8, quint8 * dstU8, quint32 nPixels) const
{
	const quint32 ncols = m_profile->matrix().ncols();
	const quint16 *src16 = reinterpret_cast<const quint16 *>(srcU8);
	float *dstf = reinterpret_cast<float *>(dstU8);

	double XYZ[3];
	vector<float> vXYZ(3), vREF(ncols), vKS(2*ncols);

	for (quint32 i = 0; i < nPixels; i++) {
		cmsDoTransform(BGR_XYZ, const_cast<quint16*>(src16), XYZ, 1);

		for (quint32 j = 0; j < 3; j++) vXYZ[j] = XYZ[j];

		simplex(m_profile->matrix(), vREF, vXYZ);
		computeKS(vREF, vKS);

		for (quint32 j = 0; j < 2*ncols; j++) dstf[j] = vKS[j];

		dstf[2*ncols] = (float)(src16[3])/NORMALIZATION;

		dstf += 2*ncols + 1;
		src16 += 4;
	}
}


void computeReflectance(const vector<float> &vKS, vector<float> &vREF)
{
	double b, K, S, R;
	vector<float>::const_iterator i = vKS.begin();
	for (vector<float>::iterator j = vREF.begin(); j != vREF.end(); i += 2, j++) {
		K = *(i+0);
		S = *(i+1);
		b = sqrt( ( K / S ) * ( K / S + 2 ) );

		R = 1 / ( 1 + ( K / S ) + b * coth( b * S ) );

		*j = R;
	}
}

/*
void computeReflectance(const vector<float> &vKS, vector<float> &vREF)
{
	double K, S, q, R;
	vector<float>::const_iterator i = vKS.begin();
	for (vector<float>::iterator j = vREF.begin(); j != vREF.end(); i += 2, j++) {
		K = *(i+0);
		S = *(i+1);
		q = K / S;
		R = 1 + q - sqrt( q*q + 2 * q );

		*j = R;
	}
}
*/
/*
void computeReflectance(const vector<float> &vKS, vector<float> &vREF)
{
	vector<float>::const_iterator i = vKS.begin();
	for (vector<float>::iterator j = vREF.begin(); j != vREF.end(); i += 2, j++) {
		(*j) = *(i+0);
	}
}
*/
uint qHash(const vector<float> &v)
{
	float sum = 0;
	for (vector<float>::const_iterator i = v.begin(); i != v.end(); i++)
		sum += *i;

	return (uint)(sum*100000);
}

void KisKSColorSpace::toRgbA16(const quint8 * srcU8, quint8 * dstU8, quint32 nPixels) const
{
	const quint32 ncols = m_profile->matrix().ncols();
	const float *srcf = reinterpret_cast<const float *>(srcU8);
	quint16 *dst16 = reinterpret_cast<quint16 *>(dstU8);

	double XYZ[3];
	vector<float> vXYZ(3), vREF(ncols), vKS(2*ncols);

	for (quint32 i = 0; i < nPixels; i++) {
		for (quint32 j = 0; j < 2*ncols; j++) vKS[j] = srcf[j];

// 		uint hash = qHash(vKS);
// 		if (m_cache.contains(hash)) {
// 			memcpy((void*)dst16, (void*)m_cache[hash], 8);
// 		} else {
			computeReflectance(vKS, vREF);

			gmm::mult(m_profile->matrix(), vREF, vXYZ);

			for (quint32 j = 0; j < 3; j++) XYZ[j] = vXYZ[j];

			cmsDoTransform(XYZ_BGR, XYZ, dst16, 1);

			dst16[3] = (quint16)(srcf[2*ncols] * NORMALIZATION);

// 			if (m_cache.count() >= 1000)
// 				m_cache.clear();
// 			memcpy((void*)*(m_cache.insert(hash, new quint16[4])), (void*)dst16, 8);
// 		}

		srcf += 2*ncols + 1;
		dst16 += 4;
	}
}
