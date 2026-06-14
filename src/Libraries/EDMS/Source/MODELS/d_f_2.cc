/*

Copyright (C) 2015-2018 Night Dive Studios, LLC.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
//	Mechanical model for frame...
//	=============================

#include "edms_int.h"
#include "edms_mod.h" //State and such...

//      Auto-alignment...
//      -----------------
#include "ss_flet.h"

//	Utilities...
//	============
static Q n = Q_from_int(0), r = Q_from_int(0), e = Q_from_int(0), mag = Q_from_int(0), roll_delta = Q_from_int(0), kappa = Q_from_int(0), delta = Q_from_int(0), sin_wheel, cos_wheel;

static Q X[3], XD[3], Z[3], FW[3], D[3];

int32_t counter, dummy1, dummy2;

//      Orientation...
//      --------------
static Q e0, e1, e2, e3, ed0, ed1, ed2, ed3;

//	Terrain returns...
//	------------------
static Q B_C_return[3];
static Q BC_test = Q_from_int(0);

//	Go for it, just go for it!
//	==========================
void dirac_mechanicals(int object, Q F[3], Q T[3]) {

    void mech_globalize(Q *, Q *, Q *), mech_localize(Q *, Q *, Q *);

    void get_boundary_conditions(int32_t object, Q raduis, Q position[3], Q derivitaves[3]);

    Q kappa, delta, mechanical_drag;

    Q *sc;

    //	The points in question, note that the 4th column is steerage...
    //	===============================================================
    Q structure[6][4] = {
    	{Q_from_int(0), Q_from_int(0), Q_from_int(0), Q_from_int(0)},
    	{Q_from_int(0), Q_from_double(.1), Q_from_int(0), Q_from_int(0)},
    	{Q_from_double(-.1), Q_from_int(0), Q_from_int(0), Q_from_int(0)},
    	{Q_from_int(0), Q_from_double(-.1), Q_from_int(0), Q_from_int(0)},
    	{Q_from_int(0), Q_from_int(0), Q_from_double(.1), Q_from_int(0)},
    	{Q_from_int(0), Q_from_int(0), Q_from_double(-.1), Q_from_int(0)}
    };

    //      Get the orientation...
    //      ----------------------
    e0 = A[object][3][0];
    e1 = A[object][4][0];
    e2 = A[object][5][0];
    e3 = A[object][6][0];
    ed0 = A[object][3][1];
    ed1 = A[object][4][1];
    ed2 = A[object][5][1];
    ed3 = A[object][6][1];

    //	From the actual model...
    //	------------------------
    Q beta_dot = Q_mul(Q_as_int(2), (Q_sub(Q_sub(Q_add(Q_mul(e0, ed1), Q_mul(e3, ed2)), Q_mul(e2, ed3)), Q_mul(e1, ed0))));
    Q alpha_dot = Q_mul(Q_as_int(2), (Q_sub(Q_add(Q_add(Q_mul(Q_neg(e3), ed1), Q_mul(e0, ed2)), Q_mul(e1, ed3)), Q_mul(e2, ed0))));
    Q gamma_dot = Q_mul(Q_as_int(2), (Q_sub(Q_add(Q_sub(Q_mul(e2, ed1), Q_mul(e1, ed2)), Q_mul(e0, ed3)), Q_mul(e3, ed0))));

    //      Steering...
    //      -----------
    Q_sincos(Q_as_int(0) /*I[object][xxx]*/, &sin_wheel, &cos_wheel);

    int32_t count = 0;

    //	Check every @#!$ point...
    //	=========================
    //	for ( counter = 0; counter < 1/*6*/; counter++ ) {

    sc = structure[counter]; // Isn't this clever?

    //	Take the structure point and find the terrain intersection...
    //	-------------------------------------------------------------
    //		X[0] = sc[0];
    //		X[1] = sc[1];
    //		X[2] = sc[2];
    //
    //		mech_globalize( X[0], X[1], X[2] );

    X[0] = A[object][0][0];
    X[1] = A[object][1][0];
    X[2] = A[object][2][0];

    //	Now find the moments of the structure elements...
    //	-------------------------------------------------
    //		XD[0] = -XPT_0( sc );			//Fucking angular velocity...
    //		XD[1] = -XPT_1( sc );
    //		XD[2] = -XPT_2( sc );

    //		mech_globalize( XD[0], XD[1], XD[2] );	//Need it at home...

    //              Notice notice notice...
    //              -----------------------
    XD[0] = A[object][0][1];
    XD[1] = A[object][1][1];
    XD[2] = A[object][2][1];

    B_C_return[0] = B_C_return[1] = B_C_return[2] = Q_as_int(0);

    //	Get the terrain information, and project it to useful axes...
    //	-------------------------------------------------------------
    get_boundary_conditions(object, Q_as_double(.15), X, XD);

    FW[0] = B_C_return[0]; // global...
    FW[1] = B_C_return[1];
    FW[2] = B_C_return[2];

    mech_localize(&FW[0], &FW[1], &FW[2]); // now local...

    //		Wheels or drag?
    //		---------------
    //		roll_delta = 0;//-( I[object][25] );

    D[0] = Q_as_int(0); // roll_delta*XD[0];
    D[1] = Q_as_int(0); // roll_delta*XD[1];
    D[2] = Q_as_int(0); // roll_delta*XD[2];		//Oleo damping...

    mech_localize(&D[0], &D[1], &D[2]);

    //		Steerable...
    //		------------
    //		if ( sc[3] == 2 ) {
    //			D[0] *= sin_wheel;
    //			D[1] *= cos_wheel;
    //			}

    //		Not...
    //		------
    //		if ( sc[3] == 1 ) D[0] *= .07;		//X direction wheels...

    //                D[0] *= .5;
    //		Z[0] = 	FW[0] + D[0];			//Temp for torques...
    //		Z[1] = 	FW[1] + D[1];
    //		Z[2] = 	FW[2] + D[2];

    F[0] = Q_add(F[0], FW[0]); // This is local...
    F[1] = Q_add(F[1], FW[1]);
    F[2] = Q_add(F[2], FW[2]);

    //		T[0] -= XP_0( sc, Z );			//Beta,
    //		T[1] -= XP_1( sc, Z );			//Alpha,
    //		T[2] -= XP_2( sc, Z );			//Gamma...

    //              PRINT3D( T )
    //              PRINT3D( F )

    //              Auto alignment...
    //              -----------------
    if (!(ss_edms_bcd_flags & SS_BCD_CURR_ON)) {
        //              if ( (EDMS_BCD < 10) || (EDMS_BCD > 27) ) {

        if (FW[0].val > Q_as_int(10).val)
            FW[0] = Q_as_int(10);
        if (FW[1].val > Q_as_int(10).val)
            FW[1] = Q_as_int(10);
        if (FW[0].val < Q_as_int(-10).val)
            FW[0] = Q_as_int(-10);
        if (FW[1].val < Q_as_int(-10).val)
            FW[1] = Q_as_int(-10);

        T[0] = Q_add(T[0], Q_mul(Q_as_double(.6), FW[0]));
        T[0] = Q_mul(T[0], Q_sub(Q_as_int(1), Q_mul(Q_as_int(2), Q_as_int(FW[2].val <= Q_as_int(0).val))));

        T[1] = Q_add(T[1], Q_mul(Q_as_double(-.5), FW[1]));
        T[1] = Q_mul(T[1], Q_sub(Q_as_int(1), Q_mul(Q_as_int(2), Q_as_int(FW[2].val <= Q_as_int(0).val))));
    }

    //	}

    //              Controls...
    //              -----------
    F[0] = Q_add(F[0], Q_mul((Q_sub(Q_as_int(1), BC_test)), I[object][0])); // Control inputs...

    T[0] = Q_add(T[0], I[object][1]);                    //                            - .1*FW[1];
    T[1] = Q_add(T[1], Q_add(Q_mul(Q_as_double(-1.5), gamma_dot), I[object][3])); //           + .1*FW[0];
    T[2] = Q_add(T[2], Q_mul(Q_as_double(-.8), I[object][2]));

    //	So be it...
    //	===========
}

//	We need to transform from global coordinates to the local airplane
//	coordinaates, a simple rotation.  The order is left up to this routine
//	which MODIFIES ITS ARGUMENTS...
//	===============================
void mech_globalize(Q *X, Q *Y, Q *Z) {

    Q x = *X, y = *Y, z = *Z;

    *X = Q_add(Q_add(Q_mul(x, (Q_sub(Q_sub(Q_add(Q_mul(e0, e0), Q_mul(e1, e1)), Q_mul(e2, e2)), Q_mul(e3, e3)))), Q_mul(y, (Q_mul(Q_as_int(2), (Q_sub(Q_mul(e1, e2), Q_mul(e0, e3))))))), Q_mul(z, (Q_mul(Q_as_int(2), (Q_add(Q_mul(e1, e3), Q_mul(e0, e2)))))));

    *Y = Q_add(Q_add(Q_mul(x, (Q_mul(Q_as_int(2), (Q_add(Q_mul(e1, e2), Q_mul(e0, e3)))))), Q_mul(y, (Q_sub(Q_add(Q_sub(Q_mul(e0, e0), Q_mul(e1, e1)), Q_mul(e2, e2)), Q_mul(e3, e3))))), Q_mul(z, (Q_mul(Q_as_int(2), (Q_sub(Q_mul(e2, e3), Q_mul(e0, e1)))))));

    *Z = Q_add(Q_add(Q_mul(x, (Q_mul(Q_as_int(2), (Q_add(Q_mul(Q_neg(e0), e2), Q_mul(e1, e3)))))), Q_mul(y, (Q_mul(Q_as_int(2), (Q_add(Q_mul(e2, e3), Q_mul(e0, e1))))))), Q_mul(z, (Q_add(Q_sub(Q_sub(Q_mul(e0, e0), Q_mul(e1, e1)), Q_mul(e2, e2)), Q_mul(e3, e3)))));
}

//	We need to transform from local coordinates back to the global coordinates
//	for the actual EDMS model...
//	============================
void mech_localize(Q *X, Q *Y, Q *Z) {

    Q x = *X, y = *Y, z = *Z;

    *X = Q_add(Q_add(Q_mul(x, (Q_sub(Q_sub(Q_add(Q_mul(e0, e0), Q_mul(e1, e1)), Q_mul(e2, e2)), Q_mul(e3, e3)))), Q_mul(y, (Q_mul(Q_as_int(2), (Q_add(Q_mul(e1, e2), Q_mul(e0, e3))))))), Q_mul(z, (Q_mul(Q_as_int(2), (Q_sub(Q_mul(e1, e3), Q_mul(e0, e2)))))));

    *Y = Q_add(Q_add(Q_mul(x, (Q_mul(Q_as_int(2), (Q_sub(Q_mul(e1, e2), Q_mul(e0, e3)))))), Q_mul(y, (Q_sub(Q_add(Q_sub(Q_mul(e0, e0), Q_mul(e1, e1)), Q_mul(e2, e2)), Q_mul(e3, e3))))), Q_mul(z, (Q_mul(Q_as_int(2), (Q_add(Q_mul(e2, e3), Q_mul(e0, e1)))))));

    *Z = Q_add(Q_add(Q_mul(x, (Q_mul(Q_as_int(2), (Q_add(Q_mul(e0, e2), Q_mul(e1, e3)))))), Q_mul(y, (Q_mul(Q_as_int(2), (Q_sub(Q_mul(e2, e3), Q_mul(e0, e1))))))), Q_mul(z, (Q_add(Q_sub(Q_sub(Q_mul(e0, e0), Q_mul(e1, e1)), Q_mul(e2, e2)), Q_mul(e3, e3)))));
}

//      Get the Real story based on the novella (System shock version), returning
//	the result in the B_C_return[3] and BC_test global variables...
//      ==============================================================
void get_boundary_conditions(int object, Q radius, Q position[3], Q derivatives[3]) {

    //	Schmeck...
    //	----------
    Q vec0, vec1, vec2, mul, vv0, vv1, vv2, dmag, kmag;

    //        if (position[0] != A[object][0][0] ) mout << position[0] << " : " << A[object][0][0] << "\n";
    //        if (position[1] != A[object][0][1] ) mout << position[1] << " : " << A[object][1][0] << "\n";
    //        if (position[2] != A[object][0][2] ) mout << position[2] << " : " << A[object][2][0] << "\n";

    //        if (derivatives[0] != A[object][0][1] ) mout << derivatives[0] << " : " << A[object][0][1] << "\n";
    //        if (derivatives[1] != A[object][1][1] ) mout << derivatives[1] << " : " << A[object][1][1] << "\n";
    //        if (derivatives[2] != A[object][2][1] ) mout << derivatives[2] << " : " << A[object][2][1] << "\n";

    //	Find locations...
    //	-----------------
    indoor_terrain(position[0], position[1], position[2], radius,
		   on2ph[object], TFD_FULL);

    //		Convert-a-tron...
    //		-----------------
    vec0 = Q_as_fix(terrain_info.fx + terrain_info.cx + terrain_info.wx);
    vec1 = Q_as_fix(terrain_info.fy + terrain_info.cy + terrain_info.wy);
    vec2 = Q_as_fix(terrain_info.fz + terrain_info.cz + terrain_info.wz);

    BC_test = Q_sqrt(Q_add(Q_add(Q_mul(vec0, vec0), Q_mul(vec1, vec1)), Q_mul(vec2, vec2)));

    if (BC_test.val > EDMS_DIV_ZERO_TOLERANCE.val) {
        mul = Q_div(Q_as_int(1), BC_test); // To get primitive...
        BC_test = Q_as_int(1);
    }

    else
        BC_test = mul = Q_as_int(0);

    //                mout << BC_test << "\n";

    vv0 = Q_mul(mul, vec0); // The primitive V_n...
    vv1 = Q_mul(mul, vec1);
    vv2 = Q_mul(mul, vec2);

    //		"rate" magnitude to all you aero-astro guys...
    //		----------------------------------------------
    dmag = Q_mul(I[object][24], Q_add(Q_add(Q_mul(derivatives[0], vv0), // Delta_magnitude...
                            Q_mul(derivatives[1], vv1)), Q_mul(derivatives[2], vv2)));

    //                PRINT3D( derivatives );

    B_C_return[0] = Q_mul(Q_neg(dmag), vv0); // Delta...
    B_C_return[1] = Q_mul(Q_neg(dmag), vv1);
    B_C_return[2] = Q_mul(Q_neg(dmag), vv2);

    kmag = I[object][23];

    B_C_return[0] = Q_add(B_C_return[0], Q_mul(kmag, vec0)); // Kappa...
    B_C_return[1] = Q_add(B_C_return[1], Q_mul(kmag, vec1));
    B_C_return[2] = Q_add(B_C_return[2], Q_mul(kmag, vec2));

    //                PRINT3D( B_C_return )
}
