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
//	Here is the beginning of the EDMS dirac frame object.  Here it is the
//	guise of the System shock cyberspace model, or is it???
//	=======================================================

//	Jon Blackley, May 14, 1994
//	==========================

#include "edms_int.h" //This is the model type library. It is universal.
#include "edms_mod.h"

#define EDMS_CYBER_CURRENT_ALIGN fix_from_float(.06)

//	Super secret Church-Blackley Boundary Condition Descriptor (BCD)...
//	===================================================================
#include "idof.h"
#include "ss_flet.h"

extern fix EDMS_CYBER_FLOW1X;
extern fix EDMS_CYBER_FLOW2X;
extern fix EDMS_CYBER_FLOW3X;

extern int32_t EDMS_BCD;

//	Here are the internal degrees of freedom.  First we get the aerodynamic forces
//	from the (external) aero model, then the interactions and solid B/C here...
//	===========================================================================
void dirac_frame_idof(int32_t object) {

    //      Here's the real work...
    //      -----------------------
    extern void dirac_mechanicals(int32_t object, fix F[3], fix T[3]);
    extern void shall_we_dance(int32_t object, fix *result0, fix *result1, fix *result2);

    //      For alignment...
    //      ----------------
    extern void mech_localize(fix *X, fix *Y, fix *Z);
    extern void mech_globalize(fix *X, fix *Y, fix *Z);
    fix F_T[3];

    fix e0, e1, e2, e3, // For speed plus beauty!
        ed0, ed1, ed2, ed3;

    fix collide_x, collide_y, collide_z;

    fix T[3], F[3];

    //	Now deal with the quaternion (2nd order) bullshit...
    //	====================================================
    e0 = A[object][3][0];
    e1 = A[object][4][0];
    e2 = A[object][5][0];
    e3 = A[object][6][0];
    ed0 = A[object][3][1];
    ed1 = A[object][4][1];
    ed2 = A[object][5][1];
    ed3 = A[object][6][1];

    fix beta_dot = fix_mul(fix_make(2,0), fix_mul(e0, ed1) + fix_mul(e3, ed2) - fix_mul(e2, ed3) - fix_mul(e1, ed0));
    fix alpha_dot = fix_mul(fix_make(2,0), fix_mul(-e3, ed1) + fix_mul(e0, ed2) + fix_mul(e1, ed3) - fix_mul(e2, ed0));
    fix gamma_dot = fix_mul(fix_make(2,0), fix_mul(e2, ed1) - fix_mul(e1, ed2) + fix_mul(e0, ed3) - fix_mul(e3, ed0));

    //	Zero the results...
    //	===================
    F[0] = F[1] = F[2] = T[0] = T[1] = T[2] = 0;

    //	Are we hitting anything yet?
    //	----------------------------
    shall_we_dance(object, &collide_x, &collide_y, &collide_z);
    F[0] += fix_mul(collide_x, I[object][23]);
    F[1] += fix_mul(collide_y, I[object][23]);
    F[2] += fix_mul(collide_z, I[object][23]);

    //	CyberSpace BCD information...
    //	-----------------------------
    indoor_terrain(A[object][0][0], A[object][1][0], A[object][2][0],
		   I[object][26], -1, TFD_RCAST);

    if (ss_edms_bcd_flags & SS_BCD_CURR_ON) {

        F_T[0] = F_T[1] = F_T[2] = 0;

        fix current_strength = EDMS_CYBER_FLOW1X;
        if ((ss_edms_bcd_flags & SS_BCD_CURR_SPD) == SS_BCD_CURR_MID)
            current_strength = EDMS_CYBER_FLOW2X;
        if ((ss_edms_bcd_flags & SS_BCD_CURR_SPD) == SS_BCD_CURR_HIGH)
            current_strength = EDMS_CYBER_FLOW3X;

        if ((ss_edms_bcd_flags & SS_BCD_REPUL_TYPE) == SS_BCD_REPUL_UP)
            F_T[2] = current_strength;
        if ((ss_edms_bcd_flags & SS_BCD_REPUL_TYPE) == SS_BCD_REPUL_DOWN)
            F_T[2] = -current_strength;

        if (F_T[2] == 0) {
            if ((ss_edms_bcd_flags & SS_BCD_CURR_DIR) == SS_BCD_CURR_E)
                F_T[0] = current_strength;
            if ((ss_edms_bcd_flags & SS_BCD_CURR_DIR) == SS_BCD_CURR_W)
                F_T[0] = -current_strength;
            if ((ss_edms_bcd_flags & SS_BCD_CURR_DIR) == SS_BCD_CURR_N)
                F_T[1] = current_strength;
            if ((ss_edms_bcd_flags & SS_BCD_CURR_DIR) == SS_BCD_CURR_S)
                F_T[1] = -current_strength;
        }

        //              Auto alignment...
        //              -----------------
        mech_localize(&F_T[0], &F_T[1], &F_T[2]);
        if (I[object][2] == 0)
            T[2] += fix_mul(EDMS_CYBER_CURRENT_ALIGN, F_T[1]);
        if (I[object][1] == 0)
            T[1] += fix_mul(EDMS_CYBER_CURRENT_ALIGN, F_T[2]);

        F[0] += F_T[0]; // Note that these are locals...
        //                F[1] += F_T[1];
        //                F[2] += F_T[2];
    }

    //	Get the mechanical info...
    //	==========================
    dirac_mechanicals(object, F, T);

    //      Jeeeeeezuz...
    //      -------------
    mech_globalize(&F[0], &F[1], &F[2]);

    //      mout << F[2] << "\n";

    //	For now, just pop them in...
    //	============================
    S[object][0][2] = fix_mul(I[object][21], F[0]) - fix_mul(fix_mul(fix_from_float(.5), I[object][25]), A[object][0][1]);
    S[object][1][2] = fix_mul(I[object][21], F[1]) - fix_mul(fix_mul(fix_from_float(.5), I[object][25]), A[object][1][1]);
    S[object][2][2] = fix_mul(I[object][21], F[2]) - fix_mul(fix_mul(fix_from_float(.5), I[object][25]), A[object][2][1]) - I[object][27];

    fix T_alpha_temp = fix_make(T[0], I[object][22]) - fix_mul(fix_make(5,0), alpha_dot);
    fix T_beta_temp = fix_make(T[1], I[object][22]) - fix_mul(fix_make(5,0), beta_dot);
    fix T_gamma_temp = fix_make(T[2], I[object][22]) - fix_mul(fix_make(5,0), gamma_dot);

    S[object][3][2] = fix_mul(fix_from_float(-.5), fix_mul(e1, T_beta_temp) + fix_mul(e2, T_alpha_temp) + fix_mul(e3, T_gamma_temp) + fix_mul(ed1, beta_dot) +
                             fix_mul(ed2, alpha_dot) + fix_mul(ed3, gamma_dot));

    S[object][4][2] = fix_mul(fix_from_float(.5), fix_mul(e0, T_beta_temp) + fix_mul(e2, T_gamma_temp) - fix_mul(e3, T_alpha_temp) + fix_mul(ed0, beta_dot) +
                            fix_mul(ed2, gamma_dot) - fix_mul(ed3, alpha_dot));

    S[object][5][2] = fix_mul(fix_from_float(.5), fix_mul(e0, T_alpha_temp) + fix_mul(e3, T_beta_temp) - fix_mul(e1, T_gamma_temp) + fix_mul(ed0, alpha_dot) +
                            fix_mul(ed3, beta_dot) - fix_mul(ed1, gamma_dot));

    S[object][6][2] = fix_mul(fix_from_float(.5), fix_mul(e1, T_alpha_temp) - fix_mul(e2, T_beta_temp) + fix_mul(e0, T_gamma_temp) + fix_mul(ed0, gamma_dot) +
                            fix_mul(ed1, alpha_dot) - fix_mul(ed2, beta_dot));

    //      mout << "Inside dframe2\n";

    //	That's all, folks. Give 'em some air.  Move along...
    //	====================================================
}

//      Control dirac_frame...
//      ======================
void control_dirac_frame(int32_t object, fix forward, fix pitch, fix yaw, fix roll) {
    I[object][0] = fix_mul(fix_make(3,0), forward);
    I[object][1] = pitch;
    I[object][2] = yaw;
    I[object][3] = roll;
}

//	Sets up everything needed to make the Dirac frame object, including conversion
//	of angles to spinors and such.  Probably should have an external utility for
//	resetting these...
//	==================
int32_t make_Dirac_frame(fix init_state[6][3], fix params[10]) {

    //	Have some variables...
    //	======================
    int32_t object_number = -1, // Three guesses...
        error_code = -1;    // Guilty until...

    //	We need ignorable coordinates...
    //	================================
    extern void null_function(int32_t);

    fix sin_alpha = 0, cos_alpha = 0, sin_beta = 0, cos_beta = 0, sin_gamma = 0, cos_gamma = 0;

    //        mout << "Making dframe1\n";

    //	First find out which object we're going to be...
    //	================================================
    while (S[++object_number][0][0] > END)
        ; // Jon's first C trickie...

    //	Is it an allowed object number?  Are we full?  Why are we here?  Is there a God?
    //	================================================================================
    if (object_number < MAX_OBJ) {

        //		Now we can create the frame:  first dump the initial state vector...
        //		=====================================================================
        for (int32_t coord = 0; coord < 3; coord++) {
            for (int32_t deriv = 0; deriv < 2; deriv++) {
                S[object_number][coord][deriv] = A[object_number][coord][deriv] =
                    init_state[coord][deriv]; // For collisions...
            }
        }

        //		Now convert the input Euler angles and derivatives into quaternion
        //		initial conditions...
        //		=====================

        //		Zeros...
        //		--------
        fix_sincos(fix_to_fang(fix_mul(fix_from_float(.5), init_state[3][0])), &sin_alpha, &cos_alpha);
        fix_sincos(fix_to_fang(fix_mul(fix_from_float(.5), init_state[4][0])), &sin_beta, &cos_beta);
        fix_sincos(fix_to_fang(fix_mul(fix_from_float(.5), init_state[5][0])), &sin_gamma, &cos_gamma);

        S[object_number][3][0] = A[object_number][3][0] =
            fix_mul(fix_mul(cos_gamma, cos_alpha), cos_beta) + fix_mul(fix_mul(sin_gamma, sin_alpha), sin_beta);

        S[object_number][4][0] = A[object_number][4][0] =
            fix_mul(fix_mul(cos_gamma, cos_alpha), sin_beta) - fix_mul(fix_mul(sin_gamma, sin_alpha), cos_beta);

        S[object_number][5][0] = A[object_number][5][0] =
            fix_mul(fix_mul(cos_gamma, sin_alpha), cos_beta )+ fix_mul(fix_mul(sin_gamma, cos_alpha), sin_beta);

        S[object_number][6][0] = A[object_number][6][0] =
            fix_mul(fix_mul(-cos_gamma, sin_alpha), sin_beta) + fix_mul(fix_mul(sin_gamma, cos_alpha), cos_beta);

        //		Firsts...
        //		---------
        S[object_number][3][1] =
            fix_mul(fix_from_float(-.5), fix_mul(S[object_number][4][0], init_state[4][1]) + fix_mul(S[object_number][5][0], init_state[3][1]) +
                   fix_mul(S[object_number][6][0], init_state[5][1]));

        S[object_number][4][1] =
            fix_mul(fix_from_float(.5), fix_mul(S[object_number][3][0], init_state[4][1]) + fix_mul(S[object_number][5][0], init_state[5][1]) -
                  fix_mul(S[object_number][6][0], init_state[3][1]));

        S[object_number][5][1] =
            fix_mul(fix_from_float(.5), fix_mul(S[object_number][3][0], init_state[3][1]) + fix_mul(S[object_number][6][0], init_state[4][1]) -
                  fix_mul(S[object_number][4][0], init_state[5][1]));

        S[object_number][6][1] =
            fix_mul(fix_from_float(.5), fix_mul(S[object_number][3][0], init_state[5][1]) + fix_mul(S[object_number][4][0], init_state[3][1]) -
                  fix_mul(S[object_number][5][0], init_state[4][1]));

        // mout << "AA: " << S[object_number][3][1] << "\n";
        // mout << "BB: " << S[object_number][4][1] << "\n";
        // mout << "GG: " << S[object_number][5][1] << "\n";

        //        mout << "Making dframe2\n";
        //        for ( int ioi = 0; ioi < 7; ioi++ ) { mout << "S[" << object_number << "][" << ioi << "][0]: " <<
        //        S[object_number][ioi][0] << "\n"; }

        //		Put in the appropriate parameters...
        //		====================================
        for (int copy = 0; copy < 10; copy++) {
            I[object_number][copy + 20] = params[copy];
        }
        I[object_number][IDOF_MODEL] = D_FRAME; // Hey, you are what you eat.

        //		Now tell Soliton where to look for the equations of motion...
        //		=============================================================
        idof_functions[object_number] = dirac_frame_idof;

        equation_of_motion[object_number][0] = equation_of_motion[object_number][1] =
            equation_of_motion[object_number][2] = equation_of_motion[object_number][3] = // Nice symmetries, huh.
            equation_of_motion[object_number][4] = equation_of_motion[object_number][5] =
                equation_of_motion[object_number][6] = null_function;

        //		Put in the collision information...
        //		===================================
        I[object_number][31] = I[object_number][26];
        I[object_number][32] = I[object_number][33] = I[object_number][34] = I[object_number][35] = 0;
        I[object_number][36] = I[object_number][26]; // Shrugoff "mass"...
        I[object_number][IDOF_COLLIDE] = fix_make(-1,0);
        I[object_number][IDOF_AUTODESTRUCT] = 0; // No kill I...

        //              Zero the controls...
        //              ====================
        I[object_number][0] = I[object_number][1] = I[object_number][3] = I[object_number][2] = 0;

        //        mout << "Making dframe3\n";
        //        for (int tt = 20; tt < 31; tt++) mout << "I[" << tt << "]: " << I[object_number][tt] << "\n";

        //		Wake me up...
        //		=============
        no_no_not_me[object_number] = 1;

        //		Things seem okay...
        //		===================
        error_code = object_number;
    }

    //	Inform the caller...
    //	====================
    return error_code;
}

//	Nota Bene:  Los parametros del model son:
//	=========================================

//	Number   |   Comment
//	--------------------
//	0        |   Mass
//	1        |   One over Mass
//	2        |   1/I
//	3        |   Kappa
//	4        |   Delta
//	5        |   Drag
//	6        |   Size
//	7	 |   gravity
//	8        |   ???
//	9	 |   ???
//	==========================================
//	So there.
