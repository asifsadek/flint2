/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2011 William Hart
    Copyright (C) 2013 Martin Lee

******************************************************************************/

#include <stdlib.h>
#include <mpir.h>
#include "flint.h"
#include "fmpz_mod_poly.h"
#include "ulong_extras.h"

void _fmpz_mod_poly_divrem_newton_preinv (fmpz* Q, fmpz* R, const fmpz* A,
                                            slong lenA, const fmpz* B, slong lenB,
                                            const fmpz* Binv, slong lenBinv, const fmpz_t p)
{
    const slong lenQ = lenA - lenB + 1;

    _fmpz_mod_poly_div_newton_preinv (Q, A, lenA, B, lenB, Binv, lenBinv, p);

    if (lenB > 1)
    {
        if (lenQ >= lenB - 1)
            _fmpz_mod_poly_mullow(R, Q, lenQ, B, lenB - 1, p, lenB - 1);
        else
            _fmpz_mod_poly_mullow(R, B, lenB - 1, Q, lenQ, p, lenB - 1);

        _fmpz_vec_sub(R, A, R, lenB - 1);
    }

    _fmpz_vec_scalar_mod_fmpz(R, R, lenB - 1, p);
}

void fmpz_mod_poly_divrem_newton_preinv(fmpz_mod_poly_t Q, fmpz_mod_poly_t R,
                                          const fmpz_mod_poly_t A, const fmpz_mod_poly_t B,
                                          const fmpz_mod_poly_t Binv)
{
    const slong lenA = A->length, lenB = B->length, lenBinv= Binv->length;
    fmpz *q, *r;

    if (lenB == 0)
    {
        printf("Exception (fmpz_mod_poly_divrem_newton_preinv). Division by zero.\n");
        abort();
    }

    if (lenA < lenB)
    {
        fmpz_mod_poly_set(R, A);
        fmpz_mod_poly_zero(Q);
        return;
    }

    if (lenA > 2*lenB-2)
    {
        printf ("Exception (fmpz_mod_poly_divrem_newton_preinv).\n");
    }

    if (Q == A || Q == B || Q == Binv)
    {
        q = _fmpz_vec_init(lenA - lenB + 1);
    }
    else
    {
        fmpz_mod_poly_fit_length(Q, lenA - lenB + 1);
        q = Q->coeffs;
    }
    if (R == A || R == B || R == Binv)
    {
        r = _fmpz_vec_init(lenB - 1);
    }
    else
    {
        fmpz_mod_poly_fit_length(R, lenB - 1);
        r = R->coeffs;
    }

    _fmpz_mod_poly_divrem_newton_preinv (q, r, A->coeffs, lenA,
                                           B->coeffs, lenB, Binv->coeffs,
                                           lenBinv, &B->p);

    if (Q == A || Q == B || Q == Binv)
    {
        _fmpz_vec_clear(Q->coeffs, lenA - lenB + 1);
        Q->coeffs = q;
        Q->alloc  = lenA - lenB + 1;
    }
    if (R == A || R == B || R == Binv)
    {
        _fmpz_vec_clear(R->coeffs, lenB - 1);
        R->coeffs = r;
        R->alloc  = lenB - 1;
    }
    Q->length = lenA - lenB + 1;
    R->length = lenB - 1;

    _fmpz_mod_poly_normalise(R);
}