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
//	Collision intersection code for EDMS models...
//	==============================================

//	Seamus, "in Prozac we trust," 1994
//	==================================

#include "edms_int.h"
#include "idof.h"

//#ifdef EDMS_SHIPPABLE
////#include <mout.h>
//#endif

//              Collision wakeups go here...
//              ----------------------------
extern int32_t alarm_clock[MAX_OBJ];
extern int32_t no_no_not_me[MAX_OBJ];

bool do_work(int32_t object, int32_t other_object, fix my_rad, fix your_rad, fix *my_pos, fix *other_pos,
             fix *result0, fix *result1, fix *result2);

void shall_we_dance(int32_t object, fix *result0, fix *result1, fix *result2);

//	Call me instead of having special code everywhere...
//	====================================================
void shall_we_dance(int32_t object, fix *result0, fix *result1, fix *result2) {
    int32_t other_object;

    fix my_radius, your_radius;

    fix my_position[3], your_position[3];

    //	Collision B/C...
    //	----------------
    result0 = result1 = result2 = 0; // B/C...

    //	Here we assume that all hits are encompassed by the projection of the
    //	default radius.  If this is not true, then special care must be taken
    //	in the design of the model...
    //	-----------------------------

    // mask contains the bits corresponding to the objects that could be
    // intersecting object.

    uint32_t mask = are_you_there(object);
    uint32_t bit = 0; // which object bit we're checking

    while (mask != 0) {
        if (mask & 1) {
            // Object bit number 'bit' is on, we must check all objects which have that bit
            for (other_object = bit; other_object < MAX_OBJ && S[other_object][0][0] > END;
                 other_object += NUM_OBJECT_BITS) {

                if (other_object != object && fix_int(I[object][IDOF_COLLIDE]) != other_object) {

                    //	Okay, now we have a confirmed hash hit...
                    //	-----------------------------------------

                    //	Do the regular guy, workaday collision...
                    //	-----------------------------------------
                    my_position[0] = A[object][0][0];
                    my_position[1] = A[object][1][0];
                    my_position[2] = A[object][2][0];

                    // if you're asleep, then we have to look at STATE...
                    // --------------------------------------------------
                    if (no_no_not_me[other_object] == 1) {
                        your_position[0] = A[other_object][0][0];
                        your_position[1] = A[other_object][1][0];
                        your_position[2] = A[other_object][2][0];
                    } else {
                        your_position[0] = S[other_object][0][0];
                        your_position[1] = S[other_object][1][0];
                        your_position[2] = S[other_object][2][0];
                    }

                    my_radius = I[object][IDOF_RADIUS];
                    your_radius = I[other_object][IDOF_RADIUS];

                    do_work(object, other_object, my_radius, your_radius, my_position, your_position, result0, result1,
                            result2);

                    int32_t you_are_special = 0, I_am_special = 0;

                    //	Are YOU special???
                    //	------------------
                    if (I[other_object][IDOF_MODEL] == PELVIS) {
                        fix offset_x = fix_mul(I[other_object][0], fix_sin(fix_to_fang(A[other_object][4][0]))),
                          offset_y = fix_mul(fix_mul(fix_from_float(-1.5), I[other_object][0]), fix_sin(fix_to_fang(A[other_object][5][0]))),
                          offset_z = fix_mul(fix_mul(I[other_object][0], fix_cos(fix_to_fang(A[other_object][4][0]))), fix_cos(fix_to_fang(A[other_object][5][0])));

                        fix sin_alpha = 0, cos_alpha = 0;

                        fix_sincos(fix_to_fang(-A[other_object][3][0]), &sin_alpha, &cos_alpha);

                        fix final_x = fix_mul(cos_alpha, offset_x) + fix_mul(sin_alpha, offset_y);
                        fix final_y = fix_mul(-sin_alpha, offset_x) + fix_mul(cos_alpha, offset_y);

                        your_position[0] = A[other_object][0][0] + final_x;
                        your_position[1] = A[other_object][1][0] + final_y;
                        your_position[2] = A[other_object][2][0] + offset_z;

                        my_radius = I[object][IDOF_RADIUS];
                        your_radius = fix_mul(fix_from_float(.75), I[other_object][IDOF_PELVIS_RADIUS]);

                        do_work(object, other_object, my_radius, your_radius, my_position, your_position, result0,
                                result1, result2);
                    } // You're not special.

                    // Am I special???
                    // ---------------
                    if (I[object][IDOF_MODEL] == PELVIS) {
                        fix offset_x = fix_mul(I[object][0], fix_sin(fix_to_fang(A[object][4][0]))),
                          offset_y = fix_mul(fix_mul(fix_from_float(-1.5), I[object][0]), fix_sin(fix_to_fang(A[object][5][0]))),
                          offset_z = fix_mul(fix_mul(I[object][0], fix_cos(fix_to_fang(A[object][4][0]))), fix_cos(fix_to_fang(A[object][5][0])));

                        fix sin_alpha = 0, cos_alpha = 0;

                        fix_sincos(fix_to_fang(-A[object][3][0]), &sin_alpha, &cos_alpha);

                        fix final_x = fix_mul(cos_alpha, offset_x) + fix_mul(sin_alpha, offset_y);
                        fix final_y = fix_mul(-sin_alpha, offset_x) + fix_mul(cos_alpha, offset_y);

                        my_position[0] = A[object][0][0] + final_x;
                        my_position[1] = A[object][1][0] + final_y;
                        my_position[2] = A[object][2][0] + offset_z;

                        // if you're asleep, then we have to look at STATE...
                        // --------------------------------------------------
                        if (no_no_not_me[other_object] == 1) {
                            your_position[0] = A[other_object][0][0];
                            your_position[1] = A[other_object][1][0];
                            your_position[2] = A[other_object][2][0];
                        } else {
                            your_position[0] = S[other_object][0][0];
                            your_position[1] = S[other_object][1][0];
                            your_position[2] = S[other_object][2][0];
                        }

                        my_radius = fix_mul(fix_from_float(.75), I[object][IDOF_PELVIS_RADIUS]);
                        your_radius = I[other_object][IDOF_RADIUS];

                        do_work(object, other_object, my_radius, your_radius, my_position, your_position, result0,
                                result1, result2);
                    } // I'm not special...
                }     // No hash hit...
            }
        }

        // Shift over the mask so we're testing the next object bit
        mask >>= 1;
        bit++;
    }
}

Q dx, dy, dz;

//	Here's the meat of the sutuation...
//	===================================
bool do_work(int32_t object, int32_t other_object, fix my_rad, fix your_rad, fix *my_pos, fix *other_pos,
             fix *result0, fix *result1, fix *result2) {
    fix cm_radius = (my_rad + your_rad);

    // First do a preliminary check to avoid overflow.
    dx = my_pos[0] - other_pos[0];
    dy = my_pos[1] - other_pos[1];
    dz = my_pos[2] - other_pos[2];

    if (dx >= cm_radius || dy >= cm_radius || dz >= cm_radius) {
        return false; // couldn't possibly collide
    }

    // Test for primary collision...
    // =============================
    fix test_radius = fix_sqrt(fix_mul(dx, dx) + fix_mul(dy, dy) + fix_mul(dz, dz));

    if ((test_radius < cm_radius) && (test_radius > fix_from_float(0.0005))) {

        // Is there a problem???
        // ---------------------
        if (test_radius < fix_from_float(.03))
            test_radius = fix_from_float(.03);

        //	Callback...
        //	-----------
        physics_handle C = on2ph[object], V = on2ph[other_object];

        int32_t badness = fix_int(fix_mul(fix_make(20,0), fix_from_float(1.) - fix_div(test_radius, cm_radius)));

        fix location[3];

        location[0] = my_pos[0];
        location[1] = my_pos[1];
        location[2] = my_pos[2];

        EDMS_object_collision(C, V, badness, 0, 0, location);

        fix Eta = (cm_radius - test_radius); // Eta...

        test_radius = fix_div(FIX_UNIT, test_radius);
        *result0 += fix_mul(fix_mul(Eta, dx), test_radius);
        *result1 += fix_mul(fix_mul(Eta, dy), test_radius);
        *result2 += fix_mul(fix_mul(Eta, dz), test_radius);

        // God save the Queen...
        // ---------------------
        if (*result0 > my_rad)
            *result0 = my_rad;
        if (*result0 < -my_rad)
            *result0 = -my_rad;

        if (*result1 > my_rad)
            *result1 = my_rad;
        if (*result1 < -my_rad)
            *result1 = -my_rad;

        //	Wakeup...
        //	=========
        if (no_no_not_me[other_object] == 0) {
            //       mout << "Other guy was asleep: " << other_object << "\n";
            //   		collision_wakeup( other_object );
            alarm_clock[other_object] = 1;
        }

        return true; // collision
    }                // End of radius check...
    else {
        return false;
    }
}
