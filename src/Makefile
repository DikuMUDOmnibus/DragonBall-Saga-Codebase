CC      = gcc

# Arbin don't uncomment this line bastard, or you won't be able to hotboot while profiling. :P
#PROF   = -p

# Uncomment the line below if you have problems with math functions
MATH_LINK = -lm 

# Uncomment the two lines below if compiling on a Solaris box
#SOLARIS_FLAG = -Dsun -DSYSV
#SOLARIS_LINK = -lnsl -lsocket

#Uncomment the line below if you are getting a line like:
#interp.c:757: warning: int format, time_t arg (arg 7)
TIME = -DTIMEFORMAT

#Uncomment the line below if you are getting implicit decleration of re_exec
#REG = -DREGEX

#Uncomment the line below if you are getting undefined re_exec errors
NEED_REG = -lgnuregex

#Uncomment the line below if you are getting undefined crypt errors
#NEED_CRYPT = -NOCRYPT

#DBUGFLG = -DREQUESTS

#Uncomment the line below if you want a performance increase though beware
#your core files may not be as much of a benefit if you do.
#OPT_FLAG = -finline-functions -funroll-loops -fdefer-pop -fstrength-reduce
OPT_FLAG = -DMCCP

C_FLAGS = $(OPT_FLAG) -g3 -O -Wall -Wuninitialized $(PROF) $(NOCRYPT) $(DBUGFLG) -DSMAUG $(SOLARIS_FLAG) $(TIME) $(REG)
L_FLAGS = $(OPT_FLAG) $(PROF) $(SOLARIS_LINK) $(NEED_CRYPT) ${MATH_LINK} -lz

#D_FLAGS : For the DNS Slave process. No need in linking all the extra libs for this.
D_FLAGS = -O -g3 $(PROF) $(SOLARIS_LINK)

C_FILES = act_comm.c act_info.c act_move.c act_obj.c act_wiz.c ban.c \
          board.c boards.c bounty.c build.c clans.c colorize.c comm.c \
          comments.c const.c db.c deity.c editor.c fight.c finger.c gocials.c grub.c \
          handler.c hashstr.c hiscores.c hotboot.c house.c ibuild.c ident.c \
          imm_host.c interp.c magic.c makeobjs.c malloc.c mapout.c marry.c \
          misc.c mpxset.c mud_comm.c mud_prog.c new_fun.c pfiles.c planes.c \
          planet.c player.c polymorph.c rare.c requests.c reset.c save.c \
          services.c ranks.c shops.c skills.c skills_android.c skills_dbs.c space.c \
	   special.c stat_obj.c tables.c track.c update.c dns.c skills_genie.c \
	   new_auth.c renumber.c md5.c skills_human.c skills_kaio.c \
          skills_saibaman.c skills_karn.c skills_terrian.c skills_fusion.c skills_icer.c \
          skills_yardratian.c skills_shadow_dragons.c skills_quest.c backup.c \
          fusion.c skills_fierian.c skills_hylian.c starmap.c skills_reploid.c changes.c \

H_FILES = mud.h board.h finger.h hotboot.h house.h pfiles.h rare.h new_auth.h md5.h bet.h changes.h

O_FILES := $(patsubst %.c,o/%.o,$(C_FILES))

all:
	make dbs

dbs: $(O_FILES)
	rm -f dbsaga
	$(CC) $(L_FLAGS) $(USE_IMC) -o dbsaga $(O_FILES) -lcrypt
	chmod g+w dbsaga
	chmod a+x dbsaga
	chmod g+w $(O_FILES)

dbs2: $(O_FILES)
	rm -f dbsaga2
	$(CC) $(L_FLAGS) $(USE_IMC) -o dbsaga2 $(O_FILES)
	chmod g+w dbsaga2
	chmod a+x dbsaga2
	chmod g+w $(O_FILES)

dns: resolver.o
	rm -f resolver
	$(CC) $(D_FLAGS) -o resolver resolver.o
	chmod g+w resolver
	chmod a+x resolver
	chmod g+w resolver.o

.c.o: mud.h
	$(CC) -c $(C_FLAGS) $(USE_IMC) $<

clean:
	rm -f o/*.o dbsaga *~

o/%.o: %.c
	$(CC) -c $(C_FLAGS) $< -o $@

indent:
	indent -ts2 -nut -nsaf -nsai -nsaw -npcs -npsl -ncs -nbc -bls -prs -bap -cbi0 -cli2 -bli0 -l125 -lp -i2 -cdb -c1 -cd1 -sc -pmt $(C_FILES) $(H_FILES)
