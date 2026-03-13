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
//	Pelvis.cc is a destroyed biped.  R.I.P.  Life is pain, and in addition this should make it
//	less fun to move in the indoor games.  You should probably use the Biped instead unless
//	you are a Twinkie and thus more concerned with inventory systems than dynamic fun...

//      Comment rescinded.  Pelvis now fun.  End of line.
//	=================================================

//	Seamus, Nov 2, 1993...
//	========================

////#include <conio.h>
#include "edms_int.h" //This is the object type library. It is universal.
#include "idof.h"

//	Super secret Church-Blackley Boundary Condition Descriptor (BCD)...
//	===================================================================
// extern "C" {

#include "ss_flet.h"

fix EDMS_CYBER_FLOW1X = fix_make(100, 0);
fix EDMS_CYBER_FLOW2X = fix_make(200, 0);
fix EDMS_CYBER_FLOW3X = fix_make(270, 0);

int32_t EDMS_BCD = 0;
bool pelvis_is_climbing = false;
int32_t edms_ss_head_bcd_flags;

fix hacked_head_bob_1 = fix_make(1, 0);
fix hacked_head_bob_2 = fix_make(1, 0);

//}

#define EDMS_DIV_ZERO_TOLERANCE fix_from_float(.0005)

//      For lean-o-meter...
//      -------------------
static fix V_ceiling[3], V_floor[3], V_wall[3];

//	State information and utilities...
//	==================================
extern EDMS_Argblock_Pointer A;
extern fix S[MAX_OBJ][7][4], I[MAX_OBJ][DOF_MAX];
extern int32_t no_no_not_me[MAX_OBJ];

//	Functions...
//	============
extern void (*idof_functions[MAX_OBJ])(int32_t), (*equation_of_motion[MAX_OBJ][7])(int32_t);

//	Callbacks themselves...
//	-----------------------
extern void (*EDMS_wall_contact)(physics_handle caller);

static fix fix_one = fix_from_float(1.), point_five = fix_from_float(.5), two_pi = fix_from_float(6.283185);

//	Just a thought...
//	=================
static fix object0, object1, object2, object3, object4, // Howzat??
    object5, object6, object7, object8, object9, object10, object11, object12, object13, object14, object15, object16,
    object17, object18, object19, object20, object21, object22;

static fix sin_alpha = 0, cos_alpha = 0, sin_beta = 0, cos_beta = 0, sin_gamma = 0, cos_gamma = 0, lp_x = 0, lp_y = 0,
         lp_z = 0, Fmxm = 0, Fmym = 0, Fmzm = 0, T_beta = 0, T_gamma = 0;

static fix io17 = 0, io18 = 0, io19 = 0;

static fix checker = 0, wall_check = 0;

//      Global for head information...
//      ==============================
fix head_delta[3], head_kappa[3], body_delta[3], body_kappa[3];

//	Storage for running feel...
//	===========================
fix bob_arg = 0;

#pragma require_prototypes off

//	Here are the internal degrees of freedom:
//	=========================================
void pelvis_idof(int32_t object) {

    // attemp to speed up something
    fix *i_object = I[object];
    fix temp_Q;

    //      To do the head motion, collisions, and climbing...
    //      --------------------------------------------------
    void get_head_of_death(int32_t), get_body_of_death(int32_t), do_climbing(int32_t object);

    //	Call me instead of having special code everywhere...
    //	====================================================
    extern void shall_we_dance(int32_t object, fix *result0, fix *result1, fix *result2);

    pelvis_is_climbing = false;

    indoor_terrain(A[object][0][0], // Get the info...
                   A[object][1][0], A[object][2][0], i_object[22],
		   on2ph[object], TFD_FULL);

    V_ceiling[0] = terrain_info.cx; // Put it in...
    V_ceiling[1] = terrain_info.cy;
    V_ceiling[2] = terrain_info.cz;

    V_floor[0] = terrain_info.fx;
    V_floor[1] = terrain_info.fy;
    V_floor[2] = terrain_info.fz;

    fix mag = fix_mul(i_object[18], i_object[18]) + fix_mul(i_object[19], i_object[19]);
    if (mag < fix_from_float(.1)
    && fix_abs(V_floor[0]) < fix_mul(fix_from_float(.05), i_object[22])
    && fix_abs(V_floor[1]) < fix_mul(fix_from_float(.05), i_object[22])) {
        V_floor[1] = 0; // Turns on SlopeStand(tm)...
        V_floor[0] = 0;
    }

    V_wall[0] = terrain_info.wx;
    V_wall[1] = terrain_info.wy;
    V_wall[2] = terrain_info.wz;

    object0 = V_wall[0] + V_floor[0] + V_ceiling[0]; // V_raw...
    object1 = V_wall[1] + V_floor[1] + V_ceiling[1];
    object2 = V_wall[2] + V_floor[2] + V_ceiling[2];

    //		checker = sqrt(object0*object0 + object1*object1 + object2*object2);
    checker = fix_sqrt(fix_mul(object0, object0) + fix_mul(object1, object1) +
                           fix_mul(object2, object2));

    if (checker > EDMS_DIV_ZERO_TOLERANCE) { // To get primitive...
        object3 = fix_div(fix_one, checker);
        object9 = fix_make(1, 0); // Are we in the rub???
    } else
        checker = object9 = 0;

    if (i_object[10] == 2)
        object9 = fix_make(1, 0); // Cyberspace...

    object4 = fix_mul(object3, object0); // The primitive V_n...
    object5 = fix_mul(object3, object1);
    object6 = fix_mul(object3, object2);

    object7 = fix_mul(i_object[21],
                          (fix_mul(A[object][0][1], object4) // Delta_magnitude...
                           + fix_mul(A[object][1][1], object5) + fix_mul(A[object][2][1], object6)));

    object8 = i_object[20];

    if (i_object[10] > 0) {
        object7 = fix_mul(object7, fix_make(2, 0));
        object8 = fix_mul(object8, fix_make(2, 0));
    }

    object4 = fix_mul(object7, object4); // Delta...
    object5 = fix_mul(object7, object5);
    object6 = fix_mul(object7, object6);

    //		CONTROL...
    //		==========

    //		Head motion for fucking hacking...
    //		----------------------------------
    fix x_ease = A[object][0][0] - S[object][0][0];
    fix y_ease = A[object][1][0] - S[object][1][0];
    fix bob_delta = fix_sqrt(fix_mul(x_ease, x_ease) + fix_mul(y_ease, y_ease));
    fix bob_speed = fix_sqrt(fix_mul(A[object][0][1], A[object][0][1]) + fix_mul(A[object][1][1], A[object][1][1]));

    bob_arg += fix_div(fix_mul(fix_make(5, 0), bob_delta), (bob_speed + fix_make(1, 0)));

    if (bob_arg > two_pi)
        bob_arg = bob_arg - two_pi;

    fix bob_fac = fix_mul(bob_speed, fix_abs(fix_sin(fix_to_fang(bob_arg))));

#define EDMS_HEAD_BOB_HEIGHT 2

    if (bob_fac > EDMS_HEAD_BOB_HEIGHT)
        bob_fac = EDMS_HEAD_BOB_HEIGHT;

    bob_fac = fix_make(EDMS_HEAD_BOB_HEIGHT, 0) - fix_mul(0x09999, bob_fac); // 0x9999 = .6

    if (i_object[10] > 0)
        bob_fac = 1;

    io18 = fix_mul(i_object[18], bob_fac);
    io19 = fix_mul(i_object[19], bob_fac);
    io17 = i_object[17];

    //		Let's not power through the walls anymore...
    //		--------------------------------------------
    io18 *= (V_wall[0] == 0);
    io19 *= (V_wall[1] == 0);
    io17 *= (V_ceiling[2] == 0);
    if ((V_floor[2] == 0) && (io17 > 0))
        io17 = 0;

    //		Cyberama...
    //		-----------
    if ((object9 == 0) && (io17 >= 0) && (i_object[25] > 0x08000)) // 0x08000 = .5
        io18 = io19 = io17 = 0;

    //      Here are collisions with other objects...
    //      =========================================
    shall_we_dance(object, &object10, &object11, &object12);

    object10 = fix_mul(object10, i_object[20]); // More general than it was...
    object11 = fix_mul(object11, i_object[20]);
    object12 = fix_mul(object12, i_object[20]);

    //	Let's not power through the walls anymore...
    //	--------------------------------------------
    //  	object10 *= ( (V_wall[0]< 0x028f) && (V_wall[0]>-0x028f));	// 0x028f = 0.01
    if (!((V_wall[0] < 0x028f) && (V_wall[0] > -0x028f)))
        object10 = 0;
    //  	object11 *= ( (V_wall[1]<0x028f) && (V_wall[1]>-0x028f));
    if (!((V_wall[1] < 0x028f) && (V_wall[1] > -0x028f)))
        object11 = 0;

    //	Back to business...
    //	===================
    fix_sincos(fix_to_fang(A[object][3][0]), &sin_alpha, &cos_alpha); // Positive for local...
    fix_sincos(fix_to_fang(A[object][4][0]), &sin_beta, &cos_beta);
    fix_sincos(fix_to_fang(A[object][5][0]), &sin_gamma, &cos_gamma);

    //      The head...
    //      ===========
    int32_t edms_ss_bcd_flags_save = ss_edms_bcd_flags; // Save off info for after head call...
    int32_t edms_ss_param_save = ss_edms_bcd_param;

    fix head_check_x = 0;
    fix head_check_y = 0;

    get_head_of_death(object);

    if (terrain_info.wx == 0)
        head_check_x = FIX_UNIT;
    if (terrain_info.wy == 0)
        head_check_y = FIX_UNIT;

    //     	io18 *= head_check_x;
    //      io19 *= head_check_y;

    edms_ss_head_bcd_flags = ss_edms_bcd_flags;
    if (terrain_info.cz != 0 || head_kappa[2] != 0)
        i_object[17] = io17 = 0;

    //      The Body...
    //      ===========
    get_body_of_death(object);
    //        io18 *= ( body_kappa[0] == 0 );
    //        io19 *= ( body_kappa[1] == 0 );
    if (body_kappa[0] != 0)
        io18 = 0;
    if (body_kappa[1] != 0)
        io19 = 0;

    ss_edms_bcd_flags = edms_ss_bcd_flags_save;
    ss_edms_bcd_param = edms_ss_param_save;

    //      Do climbing...
    //      ==============
    do_climbing(object);

    //      Fateful attempt(Jump)...
    //      ========================
    //        object18 = 800*(io17>0)*(object9>0)*( io17 - A[object][2][1] ); //Jump...
    object18 = 0;
    if (io17 > 0 && object9 > 0)
        object18 = fix_mul(fix_make(800, 0), (io17 - A[object][2][1]));

    //            Jump jets...
    //            ------------
    if ((io17 < 0) && (terrain_info.cz == 0))
        object18 = 800 * (-io17 - A[object][2][1]); // Jump jets...

    //      Climbing overriden with repulsors...
    //      ====================================
    if (ss_edms_bcd_flags & SS_BCD_REPUL_ON) {

        //              Get the speed...
        fix repulsor_speed = fix_make(21, 0);
        if ((ss_edms_bcd_flags & SS_BCD_REPUL_SPD) == SS_BCD_REPUL_NORM)
            repulsor_speed = fix_make(7, 0);

        //              Assume we're going up, unless...
        if ((ss_edms_bcd_flags & SS_BCD_REPUL_TYPE) == SS_BCD_REPUL_DOWN)
            repulsor_speed = fix_mul(repulsor_speed, fix_from_float(-.5));

        //              The parameter should be the desired height....
        fix repul_height;
        repul_height = ss_edms_bcd_param;

        fix nearness_or_something = repul_height - A[object][2][0];
        if (fix_abs(nearness_or_something) <= fix_from_float(.333))
            repulsor_speed = fix_mul(repulsor_speed, fix_mul(fix_make(3,0) , nearness_or_something));

        io17 = repulsor_speed;
        if ((fix_abs(A[object][2][1] - i_object[17]) > .6 * i_object[17]) && (terrain_info.cz == 0) &&
            (repulsor_speed >= 0))
            io17 += fix_mul(fix_make(50, 0), i_object[17]);

        object18 = fix_mul(i_object[26], (io17 - A[object][2][1]) + i_object[25]);

        if (fix_abs(io18) < fix_from_float(.01))
            io18 = fix_mul(i_object[18], V_wall[0] == 0);
        if (fix_abs(io19) < fix_from_float(.01))
            io19 = fix_mul(i_object[19], V_wall[1] == 0);

        object9 = FIX_UNIT;
    }

    //      Do climbing...
    //      ==============
    do_climbing(object);

    //	Crouch torso bend thang and boogie boogie boogie...
    //	===================================================
    if ((i_object[7] > 0.0) || (i_object[0] < i_object[6]))
        i_object[0] = fix_mul(i_object[6], FIX_UNIT - fix_mul(fix_from_float(.636), fix_abs(S[object][4][0]))); // Crouch...
    else
        i_object[0] = i_object[6];

    //	Cyberspace for real...
    //	======================
    fix drug_addict0 = fix_mul(i_object[IDOF_PELVIS_ROLL_DRAG], A[object][0][1]);
    fix drug_addict1 = fix_mul(i_object[IDOF_PELVIS_ROLL_DRAG], A[object][1][1]);
    if (fix_abs(io18) == 0 && i_object[10] > 0)
        drug_addict0 = fix_mul(drug_addict0, fix_from_float(0.2)); // Skateware drag reduction...
    if (abs(io19) == 0 && i_object[10] > 0)
        drug_addict1 = fix_mul(drug_addict1, fix_from_float(0.2));

    //      Pelvis specifics...
    //      ===================
    object20 = fix_mul(object8, object0) - object4 + head_kappa[0] - head_delta[0] + io18 + body_kappa[0] -
               body_delta[0] // F_mxyz...
               + fix_mul(object9, -drug_addict0) + object10;

    object21 = fix_mul(object8, object1) - object5 + head_kappa[1] - head_delta[1] + io19 + body_kappa[1] - body_delta[1] +
               fix_mul(object9, -drug_addict1) + object11;

    object22 = fix_mul(object8, object2) - fix_mul(object18 == 0, object6) + head_kappa[2] - head_delta[2] + object18 +
               body_kappa[2] - body_delta[2] + fix_mul(object9, fix_mul(-i_object[23], A[object][2][1])) + object12;

    //	Damage control...
    //	=================
    fix dam0 = fix_mul(object8, object0) - object4 + head_kappa[0] - head_delta[0];
    fix dam1 = fix_mul(object8, object1) - object5 + head_kappa[1] - head_delta[1];
    fix dam2 = fix_mul(object8, object2) - fix_mul(object18 == 0, object6) + head_kappa[2] - head_delta[2];

    i_object[14] = abs(dam0) + abs(dam1) + abs(dam2) - fix_mul(fix_mul(fix_make(2,0), i_object[26]), i_object[25]); // Damage??
    if (i_object[14] > 0)
        i_object[14] = fix_mul(fix_mul(i_object[14], i_object[IDOF_PELVIS_MASS_RECIP]), (io17 < fix_from_float(.5)));
    else
        i_object[14] = 0;

    //	Is there a projectile hit?
    //	==========================
    if (i_object[35] > 0) {

        //		Let's not power through the walls anymore...
        //		--------------------------------------------
        i_object[32] = fix_mul(i_object[32], ((V_wall[0] < 0.01) && (V_wall[0] > -0.01)));
        i_object[33] = fix_mul(i_object[33], ((V_wall[1] < 0.01) && (V_wall[1] > -0.01)));
        i_object[34] = fix_mul(i_object[34], ((V_ceiling[2] < 0.01) && (V_ceiling[2] > -0.01)));

        object20 += i_object[32];
        object21 += i_object[33];
        object22 += i_object[34];

        i_object[35] = 0;
        i_object[32] = 0;
        i_object[33] = 0;
        i_object[34] = 0;
    }

    Fmxm = fix_mul(object20, cos_alpha) + fix_mul(object21, sin_alpha); // Locals...
    Fmym = fix_mul(-object20, sin_alpha) + fix_mul(object21, cos_alpha);

    lp_z = fix_mul(fix_mul(fix_mul(fix_from_float(-.1), i_object[0]), cos_beta), cos_gamma);

    fix Head_tau_beta = fix_mul(fix_mul(fix_mul(fix_from_float(-.1), i_object[0]), sin_beta), head_kappa[2] - head_delta[2]),
      Head_tau_gamma = fix_mul(fix_mul(fix_mul(fix_from_float(-.1), i_object[0]), sin_gamma), head_kappa[2] - head_delta[2]);

    if (((V_wall[1] != 0) && (head_check_y == 0)) || ((V_wall[0] != 0) && (head_check_x == 0)))
        i_object[15] = 0;

    T_beta = -fix_mul(lp_z, Fmxm) + i_object[7] + Head_tau_beta; // Actual torques...
    T_gamma = -fix_mul(-Fmym, lp_z) + fix_mul(fix_mul(fix_from_float(.04), i_object[16]), +Head_tau_gamma);

    //	Kickbacks...
    //	============
    if (fix_abs(i_object[8]) > 0) {
        T_beta -= fix_mul(cos_alpha, i_object[8]) + fix_mul(sin_alpha, i_object[9]);
        T_gamma = fix_mul(-sin_alpha, i_object[8]) + fix_mul(cos_alpha, i_object[9]);

        object20 -= i_object[8]; // For zero g...
        object21 -= i_object[9];

        i_object[8] = i_object[9] = 0;
    }

    object17 = fix_mul(i_object[28], FIX_UNIT + fix_mul(fix_from_float(1.2), i_object[16] == 0)); // 3 is 2

    //	Angular play (citadel) ...
    //	==========================
    if (S[object][3][0] > two_pi)
        S[object][3][0] -= two_pi;
    if (S[object][3][0] < -two_pi)
        S[object][3][0] += two_pi;

    //	Try the equations of motion here for grins...
    //	=============================================
    S[object][0][2] = fix_mul(i_object[IDOF_PELVIS_MASS_RECIP], object20);
    S[object][1][2] = fix_mul(i_object[IDOF_PELVIS_MASS_RECIP], object21);
    S[object][2][2] = fix_mul(i_object[IDOF_PELVIS_MASS_RECIP], object22-i_object[IDOF_PELVIS_GRAVITY]);
    S[object][3][2] = fix_mul(i_object[IDOF_PELVIS_MOI_RECIP], i_object[16] - fix_mul(object17, A[object][3][1]));
    S[object][4][2] = fix_mul(i_object[IDOF_PELVIS_MOI_RECIP], (T_beta - fix_mul(fix_mul(fix_from_float(1.5), i_object[1]), A[object][4][0]) /**(1-.5*(i_object[10]==1))*/
                                      - fix_mul(fix_mul(fix_from_float(.8), i_object[2]), A[object][4][1]) /**(1-.5*(i_object[10]==1))*/));

    S[object][5][2] = fix_mul(i_object[IDOF_PELVIS_MOI_RECIP], (T_gamma - fix_mul(i_object[1], A[object][5][0]) /**(1-.5*(i_object[10]==1))*/
                                      - fix_mul(fix_mul(fix_from_float(.8), i_object[2]), A[object][5][1])    /**(1-.5*(i_object[10]==1))*/
                                      + i_object[15]));

    //	That's all, folks...
    //	====================
}

//      Here we'll get the head information we all want so badly...
//      ===========================================================
void get_head_of_death(int32_t object) {
    fix *i_object = I[object];
    fix vec0, vec1, vec2, test, mul, vv0, vv1, vv2, dmag, kmag;

    fix offset_x = fix_mul(i_object[0], fix_sin(fix_to_fang(A[object][4][0]))), offset_y = fix_mul(fix_mul(fix_from_float(-1.5), i_object[0]), fix_sin(fix_to_fang(A[object][5][0]))),
      offset_z = fix_mul(fix_mul(i_object[0], fix_cos(fix_to_fang(A[object][4][0]))), fix_cos(fix_to_fang(A[object][5][0])));

    fix sin_alpha = 0, cos_alpha = 0;

    fix final_x = 0, final_y = 0;

    fix_sincos(fix_to_fang(-A[object][3][0]), &sin_alpha, &cos_alpha);
    final_x = fix_mul(cos_alpha, offset_x) + fix_mul(sin_alpha, offset_y);
    final_y = fix_mul(-sin_alpha, offset_x) + fix_mul(cos_alpha, offset_y);

    indoor_terrain(A[object][0][0] + final_x, A[object][1][0] + final_y, A[object][2][0] + offset_z, fix_mul(fix_from_float(.75), i_object[22]),
                   fix_make(-1,0) /*on2ph[object]*/, TFD_FULL);

    fix mag = fix_mul(i_object[18], i_object[18]) + fix_mul(i_object[19], i_object[19]);
    if (mag < fix_from_float(.1) && fix_abs(V_floor[0]) < fix_mul(fix_from_float(.05), i_object[22]) && fix_abs(V_floor[1]) < fix_mul(fix_from_float(.05), i_object[22])) {
        terrain_info.fx = terrain_info.fy = 0;
    }

    vec0 = terrain_info.fx + terrain_info.cx + terrain_info.wx;
    vec1 = terrain_info.fy + terrain_info.cy + terrain_info.wy;
    vec2 = terrain_info.fz + terrain_info.cz + terrain_info.wz;

    test = fix_sqrt(fix_mul(vec0, vec0) + fix_mul(vec1, vec1) + fix_mul(vec2, vec2));

    if (test > EDMS_DIV_ZERO_TOLERANCE)
        mul = fix_div(fix_one, test); // To get primitive...
    else
        test = mul = 0;

    vv0 = fix_mul(mul, vec0); // The primitive V_n...
    vv1 = fix_mul(mul, vec1);
    vv2 = fix_mul(mul, vec2);

    dmag = fix_mul(i_object[IDOF_PELVIS_D], fix_mul(A[object][0][1], vv0) // Delta_magnitude...
                           + fix_mul(A[object][1][1], vv1) + fix_mul(A[object][2][1], vv2));

    head_delta[0] = fix_mul(dmag, vv0); // Delta...
    head_delta[1] = fix_mul(dmag, vv1);
    head_delta[2] = fix_mul(dmag, vv2);

    //		if (test < .5*i_object[22]) kmag = i_object[20];			//Omega_magnitude...
    //              else kmag = i_object[20]/test;
    kmag = i_object[IDOF_PELVIS_K];

    head_kappa[0] = fix_mul(kmag, vec0);
    head_kappa[1] = fix_mul(kmag, vec1);
    head_kappa[2] = fix_mul(kmag, vec2);
}

void get_body_of_death(int32_t object) {
    fix *i_object = I[object];

    fix vec0, vec1, vec2, test, mul, vv0, vv1, vv2, dmag, kmag;

    fix half_height = fix_mul(fix_from_float(.5), i_object[0]);

    fix offset_x = fix_mul(half_height, fix_sin(fix_to_fang(A[object][4][0]))), offset_y = fix_mul(fix_mul(fix_from_float(-1.5), half_height), fix_sin(fix_to_fang(A[object][5][0]))),
      offset_z = fix_mul(fix_mul(half_height, fix_cos(fix_to_fang(A[object][4][0]))), fix_cos(fix_to_fang(A[object][5][0])));

    fix sin_alpha = 0, cos_alpha = 0;

    fix final_x = 0, final_y = 0;

    fix_sincos(fix_to_fang(-A[object][3][0]), &sin_alpha, &cos_alpha);
    final_x = fix_mul(cos_alpha, offset_x) + fix_mul(sin_alpha, offset_y);
    final_y = fix_mul(-sin_alpha, offset_x) + fix_mul(cos_alpha, offset_y);

    indoor_terrain(A[object][0][0] + final_x, A[object][1][0] + final_y, A[object][2][0] + offset_z, fix_mul(fix_from_float(.33), i_object[0]),
                   fix_make(-1,0) /*on2ph[object]*/, TFD_FULL);

    //      Zero result!
    //      ============
    body_kappa[0] = body_kappa[1] = body_kappa[2] = 0;
    body_delta[0] = body_delta[1] = body_delta[2] = 0;

    //      Do ANYTHING?
    //      ------------
    fix abtotal = fix_abs(terrain_info.fx) + fix_abs(terrain_info.fy) + fix_abs(terrain_info.fz);
    abtotal += fix_abs(terrain_info.wx) + fix_abs(terrain_info.wy) + fix_abs(terrain_info.wz);
    abtotal += fix_abs(terrain_info.cx) + fix_abs(terrain_info.cy) + fix_abs(terrain_info.cz);
    if (abtotal != 0) {
        fix mag = fix_mul(i_object[18], i_object[18]) + fix_mul(i_object[19], i_object[19]);
        if (mag < fix_from_float(.1) && fix_abs(V_floor[0]) < fix_mul(fix_from_float(.05), i_object[22]) && fix_abs(V_floor[1]) < fix_mul(fix_from_float(.05), i_object[22]))
            terrain_info.fx = terrain_info.fy = 0;

        vec0 = terrain_info.fx + terrain_info.cx + terrain_info.wx;
        vec1 = terrain_info.fy + terrain_info.cy + terrain_info.wy;
        vec2 = terrain_info.fz + terrain_info.cz + terrain_info.wz;

        test = fix_sqrt(fix_mul(vec0, vec0) + fix_mul(vec1, vec1) + fix_mul(vec2, vec2));

        if (test > EDMS_DIV_ZERO_TOLERANCE)
            mul = fix_div(fix_one, test); // To get primitive...
        else
            test = mul = 0;

        vv0 = fix_mul(mul, vec0); // The primitive V_n...
        vv1 = fix_mul(mul, vec1);
        vv2 = fix_mul(mul, vec2);

        vec2 = vv2 = 0;

        dmag = fix_mul(i_object[IDOF_PELVIS_D], (fix_mul(A[object][0][1], vv0) // Delta_magnitude...
                               + fix_mul(A[object][1][1], vv1) + fix_mul(A[object][2][1], vv2)));

        body_delta[0] = fix_mul(dmag, vv0); // Delta...
        body_delta[1] = fix_mul(dmag, vv1);
        body_delta[2] = fix_mul(dmag, vv2);

        kmag = i_object[20];

        body_kappa[0] = fix_mul(kmag, vec0);
        body_kappa[1] = fix_mul(kmag, vec1);
        body_kappa[2] = fix_mul(kmag, vec2);

    } // Do NOTHING...
}

//      Climbing stuff also removed for speed of compilations...
//      ========================================================
void do_climbing(int32_t object) {
    fix *i_object = I[object];

    //      Hellishness...
    //      ==============
    if ((i_object[17] > 0) &&
        ((ss_edms_bcd_flags & SS_BCD_MISC_CLIMB) || (edms_ss_head_bcd_flags & SS_BCD_MISC_CLIMB))) {
        fix ass = fix_sqrt(fix_mul(fix_mul(fix_from_float(.05), i_object[18]), i_object[18]) + fix_mul(fix_mul(fix_from_float(.05), i_object[19]), i_object[19]));
        fix ratio = fix_mul(i_object[18], object0) + fix_mul(i_object[19], object1);

        if (ratio > 0)
            ass = 0;

        pelvis_is_climbing = true;

        if (checker > 0) {
            io17 = fix_mul(fix_from_float(.02), ass); // + 100*( .2*i_object[22] - V_[floor][2] );
            if ((terrain_info.cz != 0))
                io17 = 0;
            io18 = fix_mul_div(fix_mul(fix_mul(fix_from_float(-.4), i_object[IDOF_PELVIS_RADIUS]), object0), object8, checker) + fix_mul(fix_from_float(.5), i_object[18]);
            io19 = fix_mul_div(fix_mul(fix_mul(fix_from_float(-.4), i_object[IDOF_PELVIS_RADIUS]), object1), object8, checker) + fix_mul(fix_from_float(.5), i_object[19]);
            i_object[16] = fix_mul(i_object[16], fix_from_float(.5));

            //                    Set the mojo...
            //                    ===============
            object18 = fix_mul(fix_mul(fix_make(800,0), (io17 > 0)), (io17 - A[object][2][1]));
        }
    }

    //      AutoClimbing(tm) is for wussies (is superseeded by climbing)...
    //      ===============================================================
    else if ((ss_edms_bcd_flags & SS_BCD_MISC_STAIR) /*&& (i_object[17] == 0) (io17==0) */) {
        if ((checker > 0) && (fix_abs(i_object[18]) + fix_abs(i_object[19]) > fix_from_float(.01))) {
            fix ratio = fix_mul(i_object[18] + A[object][0][1], object0) + fix_mul(i_object[19] + A[object][1][1], object1);

            if (ratio <= 0) {
                io17 = fix_from_float(.5);

                io18 = fix_mul_div(fix_mul(fix_mul(fix_from_float(-.3), i_object[IDOF_PELVIS_RADIUS]), object0), object8, checker) + fix_mul(fix_from_float(.2), i_object[18]);
                io19 = fix_mul_div(fix_mul(fix_mul(fix_from_float(-.3), i_object[IDOF_PELVIS_RADIUS]), object1), object8, checker) + fix_mul(fix_from_float(.2), i_object[19]);

                //                              Set the mojo...
                //                              ===============
                object18 = fix_mul(fix_mul(fix_make(800,0), (io17 > 0)), io17 - A[object][2][1]);
            } else {
                io18 = i_object[18];
                io19 = i_object[19];
            }
        }
    }
} // End of climbing nonsense...

//	We might for now want to set some external forces on the pelvis...
//	==================================================================
void pelvis_set_control(int32_t pelvis, fix forward, fix turn, fix sidestep, fix lean, fix jump, int32_t crouch) {
    const fix pi_by_two = fix_from_float(1.5707); // Yea, flixpoint...

    fix_sincos(fix_to_fang(S[pelvis][3][0]), &object0, &object1);

    //	Get rid of it all...
    //	--------------------
    I[pelvis][15] = I[pelvis][16] = I[pelvis][17] = I[pelvis][18] = I[pelvis][19] = I[pelvis][7] = 0;

    //		Here's the thrust of the situation...
    //		-------------------------------------
    I[pelvis][18] = fix_mul(fix_mul(forward, object1), I[pelvis][IDOF_PELVIS_MASS]);
    I[pelvis][19] = fix_mul(fix_mul(forward, object0), I[pelvis][IDOF_PELVIS_MASS]);

    //		And the sidestep is off by pi/two...
    //      	------------------------------------
    fix_sincos(fix_to_fang(S[pelvis][3][0] - pi_by_two), &object0, &object1);
    I[pelvis][18] += fix_mul(fix_mul(sidestep, object1), I[pelvis][IDOF_PELVIS_MASS]);
    I[pelvis][19] += fix_mul(fix_mul(sidestep, object0), I[pelvis][IDOF_PELVIS_MASS]);

    //		And the turn of the...
    //		----------------------
    I[pelvis][16] = fix_mul(turn, I[pelvis][IDOF_PELVIS_MOI]);

    //		Jump jets of joy...
    //		-------------------
    if (jump > 0)
        I[pelvis][17] = fix_mul(fix_mul(fix_from_float(.003), I[pelvis][IDOF_PELVIS_MASS]), jump);
    if (jump < 0)
        I[pelvis][17] = fix_mul(fix_mul(fix_from_float(.0006), I[pelvis][IDOF_PELVIS_MASS]), jump);

    //		And finally leaning about...
    //		----------------------------
    I[pelvis][15] = fix_mul(fix_mul(fix_from_float(.04), lean), I[pelvis][1]); // Exactly the angle!

    //		Crouching (overpowers jumping )...
    //		----------------------------------
    if (crouch > 0)
        I[pelvis][7] = fix_mul(fix_mul(fix_from_float(.20), crouch), I[pelvis][1]);

    //	Wake up...
    //	==========
    no_no_not_me[pelvis] =
        (fix_abs(I[pelvis][15]) + fix_abs(I[pelvis][16]) + fix_abs(I[pelvis][17]) + fix_abs(I[pelvis][18]) + fix_abs(I[pelvis][19]) > 0) ||
        (no_no_not_me[pelvis] == 1);
}

//	Sets up everything needed to manufacture a pelvis with initial state vector
//	init_state[][] and EDMS motion parameters params[] into soliton. Returns the
//	object number, or else a negative error code (see Soliton.CPP for error handling and codes).
//	============================================================================================
int32_t make_pelvis(fix init_state[6][3], fix params[10]) {
    //	Have some variables...
    //	======================
    int32_t object_number = -1, // Three guesses...
        error_code = -1;    // Guilty until...

    //	We need ignorable coordinates...
    //	================================
    extern void null_function(int);

    //	First find out which object we're going to be...
    //	================================================
    while (S[++object_number][0][0] > END)
        ; // Jon's first C trickie...

    //	Is it an allowed object number?  Are we full? Why are we here? Is there a God?
    //	==============================================================================
    if (object_number < MAX_OBJ) {

        //		Now we can create the pelvis:  first dump the initial state vector...
        //		=====================================================================
        for (int32_t coord = 0; coord < 6; coord++) {
            for (int32_t deriv = 0; deriv < 3; deriv++) { // Has alpha now...
                S[object_number][coord][deriv] = A[object_number][coord][deriv] =
                    init_state[coord][deriv]; // For collisions...
            }
        }

        //		Put in the appropriate pelvis parameters...
        //		===========================================
        for (int copy = 0; copy < 10; copy++) {
            I[object_number][copy + 20] = params[copy];
        }

        I[object_number][IDOF_MODEL] = PELVIS; // Hey, you are what you eat.

        // We need some information that won't fit in the usual areas...
        // =============================================================
        // I[object_number][0] =                                           //For reference...
        I[object_number][6] = fix_mul(fix_from_float(.5), I[object_number][IDOF_PELVIS_RADIUS]);
        I[object_number][1] = fix_mul(fix_make(20,0), I[object_number][IDOF_PELVIS_MOI]);
        I[object_number][2] = fix_mul(fix_make(4,0), fix_sqrt(fix_mul(I[object_number][IDOF_PELVIS_MOI], I[object_number][1])));

        // Put in the collision information...
        // ===================================
        I[object_number][31] = I[object_number][IDOF_PELVIS_RADIUS];
        I[object_number][32] = I[object_number][33] = I[object_number][34] = I[object_number][35] = 0;
        I[object_number][36] = I[object_number][IDOF_PELVIS_MASS_RECIP]; // Shrugoff "mass"...
        I[object_number][IDOF_COLLIDE] = fix_make(-1,0);
        I[object_number][IDOF_AUTODESTRUCT] = 0; // No kill I...

        // Zero the control initially...
        // =============================
        I[object_number][7] = I[object_number][8] = I[object_number][9] = I[object_number][15] = I[object_number][16] =
            I[object_number][18] = I[object_number][19] = I[object_number][17] = 0;

        // Now tell Soliton where to look for the equations of motion...
        // =============================================================
        idof_functions[object_number] = pelvis_idof;

        equation_of_motion[object_number][0] = // Nice symmetries, huh.
            equation_of_motion[object_number][1] = equation_of_motion[object_number][2] =
                equation_of_motion[object_number][3] = equation_of_motion[object_number][4] =
                    equation_of_motion[object_number][5] = null_function;

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

//	ATTENZIONE:  Los parametros del model son:
//	==========================================

//	Number   |   Comment
//	--------------------
//	0        |   K
//	1        |   d
//	2        |   Radius
//	3        |   Rolling Drag
//	4        |   1/Mass
//	5        |   gravity
//	6        |   mass
//	7	 	 |   1/moi
//	8        |   rotational drag
//	9	 	 |   moi
//	==========================================
//	So there.

bool EDMS_pelvis_is_climbing(void)
{
    return pelvis_is_climbing;
}
    
void EDMS_lean_o_meter(physics_handle ph, fix *lean, fix *crouch) {

    *lean = *crouch = 0;

    //      Are you for real?
    //      -----------------
    if (ph > -1) {

        int32_t on = ph2on[ph];

        // Are you a pelvis...
        // -------------------
        if (I[on][IDOF_MODEL] == PELVIS) {
            *lean = S[on][5][0];
            *crouch = I[on][0] - fix_mul(fix_make(3,0), V_floor[2]);

        } // Pelvis check...

    } // For real...
}

#pragma require_prototypes on
