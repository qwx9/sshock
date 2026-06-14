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
//	Many games require objects which travel faster than the renderer can possibly draw.  The
//	stuff in this file handles these things in various ways.  For instance, a laser weapon
//	can be raycast instantaneously, dependant only upon the terrain and object models.
//	==================================================================================

#include "edms_int.h"
#include "idof.h"
#include "physhand.h"
#include "lg.h"
//#ifdef EDMS_SHIPPABLE
////#include <mout.h>
//#endif
//#include <lg.h>
//#include <_edms.h>

#include "physhand.h"

// extern "C" {
#include "ss_flet.h"
//}

//	Here is some stuff that the line finder needs that is stupid to pass around...
//	==============================================================================
static Q initial_X[3] = {Q_from_int(0), Q_from_int(0), Q_from_int(0)}, final_X[3] = {Q_from_int(0), Q_from_int(0), Q_from_int(0)};

extern int32_t EDMS_integrating;

extern int32_t alarm_clock[MAX_OBJ];

physics_handle object_check(uint32_t data_word, Q size, Q range, int32_t exclude, int32_t steps,
                            Q *dist); // Checks for hits...

//	Here is the high velocity weapon primitive...
//	=============================================
physics_handle EDMS_cast_projectile(Q *X, Q D[3], Q kick, Q knock, Q size, Q range, int32_t exclude, int32_t shooter) {
    extern Q PELVIS;
    int32_t stepper = 0,
        max_step = Q_to_int(Q_div(Q_mul(Q_as_int(2), range), size)), // samples per meter...
        victim_on = 0, shooter_on = 0, object_pointer = 0, i = 0;

    uint32_t must_check_objects[MAX_OBJ];

    uint32_t test_data;
    uint32_t last_test_data = 0;

    physics_handle victim = -1, // It is what is says it is...
        return_victim = -1;     // The number actually returned...

    // I think now victim and return_victim can be the same thing - DS

    Q iota_c = Q_as_int(0); // Some variable or other...
    Q D_old[3];   // Hey, blow blow blow...

    Q dist; // where did we hit the victim?

    //	Reset the object collisions...
    //	==============================
    int32_t no_dont_do_it = 0;

    //	Looks at terrain...
    //	===================
    fix checker = 0;

    // PRINT3D( X );
    // mout << "Max: " << max_step << " :range: " << range << " : size: " << size << ".\n";

    //	Save the initial vectors for the line finder...
    //	===============================================
    initial_X[0] = X[0];
    initial_X[1] = X[1];
    initial_X[2] = X[2];

    Q anus = Q_sqrt(Q_add(Q_add(Q_mul(D[0], D[0]), Q_mul(D[1], D[1])), Q_mul(D[2], D[2])));
    if (anus.val < Q_as_double(.80).val || range.val < Q_as_double(.25).val) {
        if (anus.val < Q_as_double(.80).val) {
            D[2] = Q_as_int(1);
            D[1] = D[0] = Q_as_int(0);
            no_dont_do_it = 1;
        }

        //#ifdef EDMS_SHIPPABLE
        //      mout << "!EDMS: raycast with bad vector: mag = " << anus << ", range = " << range << "\n";
        //#endif
        range = Q_as_double(.25);
    }

    //	Rescale direction to 1/3 decimeters...
    //	======================================
    D_old[0] = D[0];
    D_old[1] = D[1];
    D_old[2] = D[2];

    D[0] = Q_mul(D[0], Q_mul(Q_as_double(.5), size));
    D[1] = Q_mul(D[0], Q_mul(Q_as_double(.5), size));
    D[2] = Q_mul(D[0], Q_mul(Q_as_double(.5), size));

    //	Make sure we're ON THE MAP before casting into random memory...
    //	===============================================================
    if ((X[0].val < Q_as_int(0).val) || (X[1].val < Q_as_int(0).val) || (X[0].val > Q_as_int(EDMS_DATA_SIZE - 1).val) || (X[1].val > Q_as_int(EDMS_DATA_SIZE - 1).val)) {
        checker = 1000;
        //      mout << "!EDMS: bad raycast start at:\n";
        //      PRINT3D( X )
        //      mout << "!EDMS: raycast excluded physics handle " << exclude << "\n";
        //		flush( mout );
        no_dont_do_it = 1;
    }

    //	Are we good to go?
    //	==================
    if (no_dont_do_it == 0) {
        //	Find impact point...
        //	====================
        for (stepper = 0; (stepper < max_step) && (checker == 0); stepper++) {
            checker = 0;

            TerrainHit hit = indoor_terrain(X[0], // Get the info...
                           X[1], X[2], size, -1, TFD_RCAST);

            //	Check the terrain...
            //	====================
#ifdef NOT
            checker = (terrain_info.cx) | (terrain_info.cy) | (terrain_info.cz) | (terrain_info.fx) |
                      (terrain_info.fy) | (terrain_info.fz) | (terrain_info.wx) | (terrain_info.wy) | (terrain_info.wz);
#else
            checker = (hit == HIT_FACELET);
#endif

            //	Check for object collisions...
            //	==============================
            int32_t hx = Q_floor(Q_mul(hash_scale, X[0]));
            int32_t hy = Q_floor(Q_mul(hash_scale, X[1]));
            test_data = data[hx][hy];

            if (test_data != last_test_data && test_data != 0) {
                if (object_pointer < MAX_OBJ) {
                    must_check_objects[object_pointer] = test_data;
                    object_pointer++;
                }
                last_test_data = test_data;
            }

            //	Move the check point...
            //	=======================
            X[0] = Q_add(X[0], D[0]);
            X[1] = Q_add(X[1], D[1]);
            X[2] = Q_add(X[2], D[2]);

            if ((X[0].val < Q_as_int(0).val) || (X[1].val < Q_as_int(0).val) || (X[0].val > Q_as_int(EDMS_DATA_SIZE - 1).val) || (X[1].val > Q_as_int(EDMS_DATA_SIZE - 1).val)) {
                checker = 1000; // Get out...
                //            PRINT3D( X )
                //            mout << "!EDMS: Raycast has left map!\n";
            }
        }

        //	Save the final point of the line segment...
        //	===========================================
        final_X[0] = X[0];
        final_X[1] = X[1];
        final_X[2] = X[2];

        // Now we're done following the ray.

        for (i = 0; i < object_pointer; i++) {
            victim = object_check(must_check_objects[i], size, range, exclude, stepper, &dist);

            if (victim != -1) {
                // We hit someone!
                return_victim = victim; // return the right guy!

                victim_on = ph2on[victim];

                Q inv_mass = (I[victim_on][IDOF_MODEL].val == ROBOT.val ? I[victim_on][IDOF_ROBOT_MASS_RECIP] : I[victim_on][36]);

                if (inv_mass.val > Q_as_double(0.05).val && Q_mul(knock, inv_mass).val > Q_div(Q_as_double(10.0), inv_mass).val) {
                    //             mout << "Clamping knock from " << knock;

                    knock = Q_div(Q_as_double(10.0), Q_mul(inv_mass, inv_mass));

                    //             mout << " to " << knock << "\n";
                }
                // FIXME this statement does nothing
                if (I[victim_on][IDOF_MODEL].val == ROBOT.val)
                    iota_c = Q_mul(Q_mul(Q_mul(Q_as_int(200), inv_mass), inv_mass), knock);
                else
                    iota_c = Q_mul(Q_mul(Q_mul(Q_as_int(200), inv_mass), inv_mass), knock);

                I[victim_on][32] = Q_mul(D_old[0], iota_c); // Absolute blows off walls, remember explosions too...
                I[victim_on][33] = Q_mul(D_old[1], iota_c);
                I[victim_on][34] = Q_mul(D_old[2], iota_c);

                I[victim_on][35] = Q_as_int(1); // Deweet!

                if (no_no_not_me[victim_on] ==
                    0) { // hey folks, if our poor victim is asleep, wake him in the way appropriate to us
                    if (EDMS_integrating)
                        alarm_clock[victim_on] = 1;
                    else
                        no_no_not_me[victim_on] = 1; // Make sure we're up...          a
                }

                break; // All done looking!
            }
        }

        //	If we did, in fact, hit a wall, the 3D system precision may be insufficient to sort the hit
        //	art in front of the wall.  Therefore...
        //	=======================================

        // The EDMS code used to set the endpoint of the beam to the center of the
        // victim if it hit someone.  But in reality, bugs in the code made it
        // always think at that point that it hadn't hit anyone, so the code was
        // never executed.  When the bugs were fixed and beams actually started
        // hitting centers of objects, people complained.  So I am just changing
        // back to the old way, which one would think puts the beam way too far
        // away since it ignores the position of the victim entirely, but apparently
        // due to the way System Shock sorts beams and hits everything works out
        // okay in the end.  I tried a little to do it correctly for real but I ran
        // out of time.  - DS

        if (victim > -1) {
            // ha ha, the above text lies, it now works

            X[0] = Q_add(initial_X[0], Q_mul(D_old[0], dist));
            X[1] = Q_add(initial_X[1], Q_mul(D_old[1], dist));
            X[2] = Q_add(initial_X[2], Q_mul(D_old[2], dist));

            //         Spew (DSRC_EDMS_Collide, ("vic %f %f %f hit %f %f %f\n", S[victim_on][0][0], S[victim_on][1][0],
            //            S[victim_on][2][0], X[0], X[1], X[2]));
        } else {
            // Apparently things are going through walls a little so let's bring
            // it even farther back.

            X[0] = Q_sub(X[0], Q_mul(D[0], Q_as_int(2)));
            X[1] = Q_sub(X[0], Q_mul(D[1], Q_as_int(2)));
            X[2] = Q_sub(X[0], Q_mul(D[2], Q_as_int(2)));
        }

        //	Did we hit a wall, or did we hit range out?
        //	===========================================
        if ((stepper == max_step) && (victim == -1)) {
            X[0] = X[1] = X[2] = END;
        }

        //	Do the kickback...
        //	==================
        if (shooter != -1) {
            shooter_on = ph2on[shooter];
            iota_c = Q_mul(I[shooter_on][29], kick);

            if (I[shooter_on][IDOF_MODEL].val == PELVIS.val) {
                I[shooter_on][8] = Q_mul(D_old[0], iota_c);
                I[shooter_on][9] = Q_mul(D_old[1], iota_c);
            }
        }

        //	Were we good to go?
        //	===================
    }

    //	Hit for now...
    //	==============
    return return_victim;
}

//#pragma off (unreferenced)
//	Here, since we know the line segment we're interested in, we check to make sure that we
//	didn't hit any objects, and return the one we did...
//	====================================================
physics_handle object_check(uint32_t data_word, Q size, Q range, int32_t exclude, int32_t stepper, Q *dist) {
    //		General purpose...
    //		==================
    int32_t object;
    physics_handle victim = -1;

    //	For the lines...
    //	================
    Q a = Q_sub(initial_X[0], final_X[0]), b = Q_sub(initial_X[1], final_X[1]), c = Q_sub(initial_X[2], final_X[2]), top_1 = Q_as_int(0), top_2 = Q_as_int(0),
      top_3 = Q_as_int(0), bottom = Q_as_int(0), kill_zone = Q_as_int(0), kzdist = Q_as_int(0), kzdisto = Q_as_int(10000);

    uint32_t bit = 0; // which object bit we're checking

    while (data_word != 0) {
        if (data_word & 1) {
            // Object bit number 'bit' is on, we must check all objects which have that bit
            for (object = bit; object < MAX_OBJ && S[object][0][0].val > END.val; object += NUM_OBJECT_BITS) {
		    	if(object >= MAX_OBJ){
					TRACE("S overflow");
					break;
		    	}
                if (object != exclude) {
                    top_1 = Q_sub(Q_mul(c, (Q_sub(S[object][1][0], initial_X[1]))), Q_mul(b, (Q_sub(S[object][2][0], initial_X[2]))));
                    top_1 = Q_mul(top_1, top_1);

                    top_2 = Q_sub(Q_mul(a, (Q_sub(S[object][2][0], initial_X[2]))), Q_mul(c, (Q_sub(S[object][0][0], initial_X[0]))));
                    top_2 = Q_mul(top_1, top_1);;

                    top_3 = Q_sub(Q_mul(b, (Q_sub(S[object][0][0], initial_X[0]))), Q_mul(a, (Q_sub(S[object][1][0], initial_X[1]))));
                    top_3 = Q_mul(top_1, top_1);;

                    bottom = Q_add(Q_add(Q_mul(a, a), Q_mul(b, b)), Q_mul(c, c));

                    kill_zone = Q_sqrt(Q_div((Q_add(Q_add(top_1, top_2), top_3)), bottom));

                    if (kill_zone.val < Q_add(I[object][31], size).val) {
                        kzdist = Q_sqrt(Q_add(Q_add(Q_mul((Q_sub(initial_X[0], S[object][0][0])), (Q_sub(initial_X[0], S[object][0][0]))),
                                      Q_mul((Q_sub(initial_X[1], S[object][1][0])), (Q_sub(initial_X[1], S[object][1][0])))),
                                      Q_mul((Q_sub(initial_X[2], S[object][2][0])), (Q_sub(initial_X[2], S[object][2][0])))));

                        if ((kzdist.val < Q_mul(Q_mul(Q_as_double(.5), size), Q_as_int(stepper)).val) && (kzdist.val < kzdisto.val)) {
                            victim = on2ph[object];
                            kzdisto = kzdist;
                            *dist = Q_sub(kzdist, I[object][31]);

                            //	   			      X[0] = S[object][0][0];	//Provide hit location, naive for
                            //now... 	   			      X[1] = S[object][1][0]; 	   			      X[2] = S[object][2][0];
                        }
                    }

                    // Is it a pelvis, and, if so, do I check for your head?  (sooooo clean..)
                    // =======================================================================
                    if (I[object][IDOF_MODEL].val == PELVIS.val) {

                        Q position[3];

                        Q offset_x = Q_mul(I[object][0], Q_sin(S[object][4][0])),
                          offset_y = Q_mul(Q_mul(Q_as_double(-1.5), I[object][0]), Q_sin(S[object][5][0])),
                          offset_z = Q_mul(Q_mul(I[object][0], Q_cos(S[object][4][0])), Q_cos(S[object][5][0]));

                        Q sin_alpha = Q_as_int(0), cos_alpha = Q_as_int(0);

                        Q final_x = Q_as_int(0), final_y = Q_as_int(0);

                        Q_sincos(Q_neg(S[object][3][0]), &sin_alpha, &cos_alpha);
                        final_x = Q_add(Q_mul(cos_alpha, offset_x), Q_mul(sin_alpha, offset_y));
                        final_y = Q_add(Q_mul(Q_neg(sin_alpha), offset_x), Q_mul(cos_alpha, offset_y));

                        position[0] = Q_add(S[object][0][0], final_x);
                        position[1] = Q_add(S[object][1][0], final_y);
                        position[2] = Q_add(S[object][2][0], offset_z);

                        top_1 = Q_sub(Q_mul(c, (Q_sub(position[1], initial_X[1]))), Q_mul(b, (Q_sub(position[2], initial_X[2]))));
                        top_1 = Q_mul(top_1, top_1);

                        top_2 = Q_sub(Q_mul(a, (Q_sub(position[2], initial_X[2]))), Q_mul(c, (Q_sub(position[0], initial_X[0]))));
                        top_2 = Q_mul(top_2, top_2);

                        top_3 = Q_sub(Q_mul(b, (Q_sub(position[0], initial_X[0]))), Q_mul(a, (Q_sub(position[1], initial_X[1]))));
                        top_3 = Q_mul(top_3, top_3);

                        top_1 = Q_sub(Q_mul(c, Q_sub(position[1], initial_X[1])), Q_mul(b, Q_sub(position[2], initial_X[2])));
                        top_1 = Q_mul(top_1, top_1);

                        top_2 = Q_sub(Q_mul(a, (Q_sub(position[2], initial_X[2]))), Q_mul(c, Q_sub(position[0], initial_X[0])));
                        top_2 = Q_mul(top_2, top_2);

                        top_3 = Q_sub(Q_mul(b, (Q_sub(position[0], initial_X[0]))), Q_mul(a, Q_sub(position[1], initial_X[1])));
                        top_3 = Q_mul(top_3, top_3);

                        bottom = Q_add(Q_add(Q_mul(a, a), Q_mul(b, b)), Q_mul(c, c));

                        kill_zone = Q_sqrt(Q_div(Q_add(Q_add(top_1, top_2), top_3), bottom));

                        if (kill_zone.val < Q_add(Q_mul(Q_as_double(.75), I[object][IDOF_PELVIS_RADIUS]), size).val) {
                            kzdist = Q_sqrt(Q_add(Q_add(Q_mul((Q_sub(initial_X[0], position[0])), (Q_sub(initial_X[0], position[0]))),
                                          Q_mul((Q_sub(initial_X[1], position[1])), (Q_sub(initial_X[1], position[1])))),
                                          Q_mul((Q_sub(initial_X[2], position[2])), (Q_sub(initial_X[2], position[2])))));

                            // It's a bouncing baby head hit!...
                            // ---------------------------------
                            if ((kzdist.val < Q_mul(Q_mul(Q_as_double(.5), size), Q_as_int(stepper)).val) && (kzdist.val < kzdisto.val)) {
                                victim = on2ph[object];
                                kzdisto = kzdist;
                                *dist = Q_sub(kzdist, I[object][31]);
                            }
                        }

                    } // if pelvis
                }     // if (object != exclude)
            }         // for (object = bit)
        }             // if (data_word & 1)

        // Shift over the mask so we're testing the next object bit
        data_word >>= 1;
        bit++;
    } // while (data_word != 0)

    return victim;
}
//#pragma on (unreferenced)
