</$objtype/mkfile
BIN=/$objtype/bin/games
TARG=sshock

LIB=src/Libraries/libshock.a$O
HFILES=\
	src/MusicSrc/MusicDevice.h\
	src/GameSrc/Headers/FrUtils.h\
	src/GameSrc/Headers/ai.h\
	src/GameSrc/Headers/aiflags.h\
	src/GameSrc/Headers/airupt.h\
	src/GameSrc/Headers/amap.h\
	src/GameSrc/Headers/amaploop.h\
	src/GameSrc/Headers/ammomfd.h\
	src/GameSrc/Headers/anim.h\
	src/GameSrc/Headers/archiveformat.h\
	src/GameSrc/Headers/audiolog.h\
	src/GameSrc/Headers/automap.h\
	src/GameSrc/Headers/bark.h\
	src/GameSrc/Headers/biohelp.h\
	src/GameSrc/Headers/biotrax.h\
	src/GameSrc/Headers/canvchek.h\
	src/GameSrc/Headers/cardmfd.h\
	src/GameSrc/Headers/cit2d.h\
	src/GameSrc/Headers/citalog.h\
	src/GameSrc/Headers/citbark.h\
	src/GameSrc/Headers/citmat.h\
	src/GameSrc/Headers/citres.h\
	src/GameSrc/Headers/colors.h\
	src/GameSrc/Headers/combat.h\
	src/GameSrc/Headers/cone.h\
	src/GameSrc/Headers/criterr.h\
	src/GameSrc/Headers/cutsloop.h\
	src/GameSrc/Headers/cyber.h\
	src/GameSrc/Headers/cybermfd.h\
	src/GameSrc/Headers/cybmem.h\
	src/GameSrc/Headers/cybrloop.h\
	src/GameSrc/Headers/cybrnd.h\
	src/GameSrc/Headers/cybstrng.h\
	src/GameSrc/Headers/damage.h\
	src/GameSrc/Headers/diffq.h\
	src/GameSrc/Headers/digifx.h\
	src/GameSrc/Headers/dirac.h\
	src/GameSrc/Headers/doorparm.h\
	src/GameSrc/Headers/drugs.h\
	src/GameSrc/Headers/dynmem.h\
	src/GameSrc/Headers/effect.h\
	src/GameSrc/Headers/email.h\
	src/GameSrc/Headers/emailbit.h\
	src/GameSrc/Headers/faceobj.h\
	src/GameSrc/Headers/faketime.h\
	src/GameSrc/Headers/fatigue.h\
	src/GameSrc/Headers/fauxrint.h\
	src/GameSrc/Headers/fixtrmfd.h\
	src/GameSrc/Headers/fr3d.h\
	src/GameSrc/Headers/framer8.h\
	src/GameSrc/Headers/frcamera.h\
	src/GameSrc/Headers/frcursors.h\
	src/GameSrc/Headers/fredge.h\
	src/GameSrc/Headers/frflags.h\
	src/GameSrc/Headers/frintern.h\
	src/GameSrc/Headers/froslew.h\
	src/GameSrc/Headers/frparams.h\
	src/GameSrc/Headers/frprotox.h\
	src/GameSrc/Headers/frquad.h\
	src/GameSrc/Headers/frscreen.h\
	src/GameSrc/Headers/frshipm.h\
	src/GameSrc/Headers/frspans.h\
	src/GameSrc/Headers/frsubclp.h\
	src/GameSrc/Headers/frtables.h\
	src/GameSrc/Headers/frtypes.h\
	src/GameSrc/Headers/frtypesx.h\
	src/GameSrc/Headers/frworld.h\
	src/GameSrc/Headers/fullamap.h\
	src/GameSrc/Headers/fullscrn.h\
	src/GameSrc/Headers/game_screen.h\
	src/GameSrc/Headers/gameloop.h\
	src/GameSrc/Headers/gameobj.h\
	src/GameSrc/Headers/gamepal.h\
	src/GameSrc/Headers/gamerend.h\
	src/GameSrc/Headers/gamescr.h\
	src/GameSrc/Headers/gamesort.h\
	src/GameSrc/Headers/gamestrn.h\
	src/GameSrc/Headers/gamesys.h\
	src/GameSrc/Headers/gametime.h\
	src/GameSrc/Headers/gamewrap.h\
	src/GameSrc/Headers/gearmfd.h\
	src/GameSrc/Headers/gettmaps.h\
	src/GameSrc/Headers/gr2ss.h\
	src/GameSrc/Headers/grenades.h\
	src/GameSrc/Headers/hand.h\
	src/GameSrc/Headers/handart.h\
	src/GameSrc/Headers/hkeyfunc.h\
	src/GameSrc/Headers/hud.h\
	src/GameSrc/Headers/hudobj.h\
	src/GameSrc/Headers/ice.h\
	src/GameSrc/Headers/init.h\
	src/GameSrc/Headers/input.h\
	src/GameSrc/Headers/invdims.h\
	src/GameSrc/Headers/invent.h\
	src/GameSrc/Headers/invpages.h\
	src/GameSrc/Headers/leanmetr.h\
	src/GameSrc/Headers/loops.h\
	src/GameSrc/Headers/lvldata.h\
	src/GameSrc/Headers/mainloop.h\
	src/GameSrc/Headers/map.h\
	src/GameSrc/Headers/mapflags.h\
	src/GameSrc/Headers/mapnorm.h\
	src/GameSrc/Headers/mfdart.h\
	src/GameSrc/Headers/mfddims.h\
	src/GameSrc/Headers/mfdext.h\
	src/GameSrc/Headers/mfdfunc.h\
	src/GameSrc/Headers/mfdgadg.h\
	src/GameSrc/Headers/mfdgames.h\
	src/GameSrc/Headers/mfdgump.h\
	src/GameSrc/Headers/mfdint.h\
	src/GameSrc/Headers/mfdpanel.h\
	src/GameSrc/Headers/minimax.h\
	src/GameSrc/Headers/miscqvar.h\
	src/GameSrc/Headers/mlimbs.h\
	src/GameSrc/Headers/models.h\
	src/GameSrc/Headers/modtext.h\
	src/GameSrc/Headers/mouselook.h\
	src/GameSrc/Headers/movekeys.h\
	src/GameSrc/Headers/musicai.h\
	src/GameSrc/Headers/newmfd.h\
	src/GameSrc/Headers/obj3d.h\
	src/GameSrc/Headers/objapp.h\
	src/GameSrc/Headers/objart.h\
	src/GameSrc/Headers/objart2.h\
	src/GameSrc/Headers/objart3.h\
	src/GameSrc/Headers/objbit.h\
	src/GameSrc/Headers/objclass.h\
	src/GameSrc/Headers/objcrit.h\
	src/GameSrc/Headers/objects.h\
	src/GameSrc/Headers/objgame.h\
	src/GameSrc/Headers/objload.h\
	src/GameSrc/Headers/objmode.h\
	src/GameSrc/Headers/objprop.h\
	src/GameSrc/Headers/objsim.h\
	src/GameSrc/Headers/objstuff.h\
	src/GameSrc/Headers/objuse.h\
	src/GameSrc/Headers/objver.h\
	src/GameSrc/Headers/objwarez.h\
	src/GameSrc/Headers/objwpn.h\
	src/GameSrc/Headers/olhext.h\
	src/GameSrc/Headers/olhint.h\
	src/GameSrc/Headers/olhscan.h\
	src/GameSrc/Headers/otrip.h\
	src/GameSrc/Headers/palfx.h\
	src/GameSrc/Headers/pathfind.h\
	src/GameSrc/Headers/physics.h\
	src/GameSrc/Headers/physunit.h\
	src/GameSrc/Headers/player.h\
	src/GameSrc/Headers/playerlayout.h\
	src/GameSrc/Headers/plotware.h\
	src/GameSrc/Headers/popups.h\
	src/GameSrc/Headers/precompiled.h\
	src/GameSrc/Headers/rcolors.h\
	src/GameSrc/Headers/refstuf.h\
	src/GameSrc/Headers/render.h\
	src/GameSrc/Headers/rendfx.h\
	src/GameSrc/Headers/rendtool.h\
	src/GameSrc/Headers/safeedms.h\
	src/GameSrc/Headers/saveload.h\
	src/GameSrc/Headers/schedtyp.h\
	src/GameSrc/Headers/schedule.h\
	src/GameSrc/Headers/setploop.h\
	src/GameSrc/Headers/setup.h\
	src/GameSrc/Headers/sfxlist.h\
	src/GameSrc/Headers/shockolate_version.h\
	src/GameSrc/Headers/shodan.h\
	src/GameSrc/Headers/sideart.h\
	src/GameSrc/Headers/sideicon.h\
	src/GameSrc/Headers/sndcall.h\
	src/GameSrc/Headers/softdef.h\
	src/GameSrc/Headers/splash.h\
	src/GameSrc/Headers/splshpal.h\
	src/GameSrc/Headers/star.h\
	src/GameSrc/Headers/statics.h\
	src/GameSrc/Headers/status.h\
	src/GameSrc/Headers/strwrap.h\
	src/GameSrc/Headers/svgacurs.h\
	src/GameSrc/Headers/target.h\
	src/GameSrc/Headers/textmaps.h\
	src/GameSrc/Headers/texture.h\
	src/GameSrc/Headers/tfdirect.h\
	src/GameSrc/Headers/tickcount.h\
	src/GameSrc/Headers/tilecam.h\
	src/GameSrc/Headers/tilemap.h\
	src/GameSrc/Headers/tilename.h\
	src/GameSrc/Headers/tools.h\
	src/GameSrc/Headers/tpolys.h\
	src/GameSrc/Headers/treasure.h\
	src/GameSrc/Headers/trigger.h\
	src/GameSrc/Headers/verify.h\
	src/GameSrc/Headers/version.h\
	src/GameSrc/Headers/view360.h\
	src/GameSrc/Headers/viewhelp.h\
	src/GameSrc/Headers/visible.h\
	src/GameSrc/Headers/vitals.h\
	src/GameSrc/Headers/vmail.h\
	src/GameSrc/Headers/wares.h\
	src/GameSrc/Headers/weapons.h\
	src/GameSrc/Headers/wrapper.h\
	src/MacSrc/InitMac.h\
	src/MacSrc/MacTune.h\
	src/MacSrc/Modding.h\
	src/MacSrc/OpenGL.h\
	src/MacSrc/Prefs.h\
	src/MacSrc/Shock.h\
	src/MacSrc/ShockBitmap.h\
	src/MacSrc/Xmi.h\

OFILES=\
	src/GameSrc/FrUtils.$O\
	src/GameSrc/ai.$O\
	src/GameSrc/airupt.$O\
	src/GameSrc/amap.$O\
	src/GameSrc/amaploop.$O\
	src/GameSrc/ammomfd.$O\
	src/GameSrc/anim.$O\
	src/GameSrc/archiveformat.$O\
	src/GameSrc/audiolog.$O\
	src/GameSrc/automap.$O\
	src/GameSrc/bark.$O\
	src/GameSrc/biohelp.$O\
	src/GameSrc/cardmfd.$O\
	src/GameSrc/citres.$O\
	src/GameSrc/combat.$O\
	src/GameSrc/cone.$O\
	src/GameSrc/criterr.$O\
	src/GameSrc/cutsloop.$O\
	src/GameSrc/cyber.$O\
	src/GameSrc/cybermfd.$O\
	src/GameSrc/cybmem.$O\
	src/GameSrc/cybrnd.$O\
	src/GameSrc/damage.$O\
	src/GameSrc/digifx.$O\
	src/GameSrc/drugs.$O\
	src/GameSrc/effect.$O\
	src/GameSrc/email.$O\
	src/GameSrc/faceobj.$O\
	src/GameSrc/fixtrmfd.$O\
	src/GameSrc/frcamera.$O\
	src/GameSrc/frclip.$O\
	src/GameSrc/frcompil.$O\
	src/GameSrc/frmain.$O\
	src/GameSrc/frobj.$O\
	src/GameSrc/froslew.$O\
	src/GameSrc/frpipe.$O\
	src/GameSrc/frpts.$O\
	src/GameSrc/frsetup.$O\
	src/GameSrc/frtables.$O\
	src/GameSrc/frterr.$O\
	src/GameSrc/frutil.$O\
	src/GameSrc/fullamap.$O\
	src/GameSrc/fullscrn.$O\
	src/GameSrc/gameloop.$O\
	src/GameSrc/gameobj.$O\
	src/GameSrc/gamerend.$O\
	src/GameSrc/gamesort.$O\
	src/GameSrc/gamestrn.$O\
	src/GameSrc/gamesys.$O\
	src/GameSrc/gametime.$O\
	src/GameSrc/gamewrap.$O\
	src/GameSrc/gearmfd.$O\
	src/GameSrc/gr2ss.$O\
	src/GameSrc/grenades.$O\
	src/GameSrc/hand.$O\
	src/GameSrc/hflip.$O\
	src/GameSrc/hkeyfunc.$O\
	src/GameSrc/hud.$O\
	src/GameSrc/hudobj.$O\
	src/GameSrc/init.$O\
	src/GameSrc/input.$O\
	src/GameSrc/invent.$O\
	src/GameSrc/leanmetr.$O\
	src/GameSrc/mainloop.$O\
	src/GameSrc/map.$O\
	src/GameSrc/mfdfunc.$O\
	src/GameSrc/mfdgadg.$O\
	src/GameSrc/mfdgames.$O\
	src/GameSrc/mfdgump.$O\
	src/GameSrc/mfdpanel.$O\
	src/GameSrc/minimax.$O\
	src/GameSrc/mlimbs.$O\
	src/GameSrc/mouselook.$O\
	src/GameSrc/movekeys.$O\
	src/GameSrc/musicai.$O\
	src/GameSrc/newai.$O\
	src/GameSrc/newmfd.$O\
	src/GameSrc/objapp.$O\
	src/GameSrc/objects.$O\
	src/GameSrc/objload.$O\
	src/GameSrc/objprop.$O\
	src/GameSrc/objsim.$O\
	src/GameSrc/objuse.$O\
	src/GameSrc/olh.$O\
	src/GameSrc/olhscan.$O\
	src/GameSrc/palfx.$O\
	src/GameSrc/pathfind.$O\
	src/GameSrc/physics.$O\
	src/GameSrc/player.$O\
	src/GameSrc/plotware.$O\
	src/GameSrc/popups.$O\
	src/GameSrc/render.$O\
	src/GameSrc/rendtool.$O\
	src/GameSrc/saveload.$O\
	src/GameSrc/schedule.$O\
	src/GameSrc/screen.$O\
	src/GameSrc/setup.$O\
	src/GameSrc/shodan.$O\
	src/GameSrc/sideicon.$O\
	src/GameSrc/sndcall.$O\
	src/GameSrc/star.$O\
	src/GameSrc/statics.$O\
	src/GameSrc/status.$O\
	src/GameSrc/target.$O\
	src/GameSrc/textmaps.$O\
	src/GameSrc/tfdirect.$O\
	src/GameSrc/tfutil.$O\
	src/GameSrc/tickcount.$O\
	src/GameSrc/tools.$O\
	src/GameSrc/trigger.$O\
	src/GameSrc/view360.$O\
	src/GameSrc/viewhelp.$O\
	src/GameSrc/vitals.$O\
	src/GameSrc/vmail.$O\
	src/GameSrc/wares.$O\
	src/GameSrc/weapons.$O\
	src/GameSrc/wrapper.$O\
	src/MusicSrc/MusicDevice.$O\
	src/MacSrc/InitMac.$O\
	src/MacSrc/MacTune.$O\
	src/MacSrc/Modding.$O\
	src/MacSrc/Prefs.$O\
	src/MacSrc/SDLSound.$O\
	src/MacSrc/Shock.$O\
	src/MacSrc/ShockBitmap.$O\
	src/MacSrc/Xmi.$O\

</sys/src/cmd/mkone

CLEANFILES=$CLEANFILES $OFILES

# -DDEBUGGING_BLIT -DUSE_OPENGL -DUSEFLUIDSYNTH=1 -DSVGA_SUPPORT
CFLAGS=$CFLAGS -p -D__plan9__ -D__${objtype}__ \
	-DUSE_SDL_MIXER \
	-I/sys/include/npe \
	-I/sys/include/npe/SDL2 \
	-Isrc/GameSrc/Headers \
	-Isrc/Libraries/H \
	-Isrc/Libraries/2D/Source \
	-Isrc/Libraries/2D/Source/GR \
	-Isrc/Libraries/3D/Source \
	-Isrc/Libraries/AFILE/Source \
	-Isrc/Libraries/DSTRUCT/Source \
	-Isrc/Libraries/FIX/Source \
	-Isrc/Libraries/EDMS/Source \
	-Isrc/Libraries/INPUT/Source \
	-Isrc/Libraries/LG/Source \
	-Isrc/Libraries/LG/Source/LOG/src \
	-Isrc/Libraries/PALETTE/Source \
	-Isrc/Libraries/RND/Source \
	-Isrc/Libraries/RES/Source \
	-Isrc/Libraries/SND/Source \
	-Isrc/Libraries/UI/Source \
	-Isrc/Libraries/VOX/Source \
	-Isrc/MusicSrc \
	-Isrc/MacSrc \

%.$O: %.c
	$CC $CFLAGS -o $target $stem.c

$LIB: mklib

mklib:
	cd src/Libraries
	mk

clean nuke:V:
	rm -f *.[$OS] [$OS].out $TARG $CLEANFILES
	cd src/Libraries
	mk $target
