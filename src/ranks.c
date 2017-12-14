/* Rank Code by Chili
 */
#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include "mud.h"
#include "rank.h"

/* Char Pointers of Players who hold ranks in the races and what not.. */
char *saiyan_ranks[8];
char *demon_ranks[8];
char *kaio_ranks[7];
char *android_ranks[7];
char *hylian_ranks[8];
char *human_ranks[7];
char *icer_ranks[8];
char *halfbreed_ranks[4];
char *bioandroid_ranks[7];
char *namek_ranks[7];
char *fierian_ranks[6];
char *reploid_ranks[7];
char *genie_ranks[6];
char *saibaman_ranks[7];
char *dragon_ranks[9];
char *mutant_ranks[8];

struct rank_table_t rank_table[] = {
  /* Starting Ansi Code, Male Name, Female Name, Gain Function to Call, Loss Functiotn o Call, 
   * Function to Check Race with, PL required, Rank Given */
  /* Saiyan Ranks */
  {"&Y", "Saiyan King", "Saiyan Queen", &saiyan_king_gain, &saiyan_king_loss, &is_pure_saiyan,  100000000000ULL, 3},
  {"&R", "Saiyan Prince", "Saiyan Princess", NULL, NULL, &is_pure_saiyan, 10000000000ULL ,  2},
  {"&R", "Saiyan Prince", "Saiyan Princess", NULL, NULL, &is_pure_saiyan, 10000000000ULL ,  2},
  {"&C", "Saiyan Elite", NULL, NULL, NULL, &is_pure_saiyan,  1000000000,  1},
  {"&C", "Saiyan Elite", NULL, NULL, NULL, &is_pure_saiyan,  1000000000, 1},
  {"&C", "Saiyan Elite", NULL, NULL, NULL, &is_pure_saiyan,  1000000000, 1},
  {"&C", "Saiyan Elite", NULL, NULL, NULL, &is_pure_saiyan,  1000000000, 1},
  /* Kaio Ranks */
  {"&R", "Supreme Kai", NULL, &supreme_kai_gain, &supreme_kai_loss, &is_kaio, 100000000000ULL, 6},
  {"&W", "Grand Kai", NULL, NULL, NULL, &is_kaio, 10000000000ULL , 5},
  {"&Y", "North Kai", NULL, NULL, NULL, &is_kaio, 1000000000, 4},
  {"&B", "West Kai", NULL, NULL, NULL, &is_kaio,  1000000000, 3},
  {"&G", "East Kai", NULL, NULL, NULL, &is_kaio,  1000000000, 2},
  {"&g", "South Kai", NULL, NULL, NULL, &is_kaio, 1000000000, 1},
  /* Demon Ranks */
  {"&R", "Demon King", "Demon Queen", NULL, &sanctuary_loss, &is_demon, 100000000000ULL, 3},
  {"&W", "Demon Warlord", NULL, NULL, NULL, &is_demon, 10000000000ULL, 2},
  {"&W", "Demon Warlord", NULL, NULL, NULL, &is_demon, 10000000000ULL, 2},
  {"&C", "Greater Demon", NULL, NULL, NULL, &is_demon, 1000000000, 1},
  {"&C", "Greater Demon", NULL, NULL, NULL, &is_demon, 1000000000, 1},
  {"&C", "Greater Demon", NULL, NULL, NULL, &is_demon, 1000000000, 1},
  {"&C", "Greater Demon", NULL, NULL, NULL, &is_demon, 1000000000, 1},
  /* Android(H-E-FM Models) Ranks */
  {"&w", "Prototype Model", NULL, NULL, &sanctuary_loss, &is_android, 100000000000ULL, 3},
  {"&p", "Rare Model", NULL, NULL, NULL, &is_android, 10000000000ULL, 2},
  {"&p", "Rare Model", NULL, NULL, NULL, &is_android, 10000000000ULL, 2},
  {"&Y", "Upgraded Model", NULL, NULL, NULL, &is_android, 1000000000, 1},
  {"&Y", "Upgraded Model", NULL, NULL, NULL, &is_android, 1000000000, 1},
  {"&Y", "Upgraded Model", NULL, NULL, NULL, &is_android, 1000000000, 1},
  /* Hylian Ranks */
  {"&z", "Hero of Time", "Heroine of Time", NULL, &sanctuary_loss, &is_hylian, 100000000000ULL, 4},
  {"&C", "Goron Ambassador", NULL, NULL, NULL, &is_hylian, 10000000000ULL, 3},
  {"&r", "Zoran Ambassador", NULL, NULL, NULL, &is_hylian, 10000000000ULL, 2},
  {"&c", "Hyrulian Knight", NULL, NULL, NULL, &is_hylian, 1000000000, 1},
  {"&c", "Hyrulian Knight", NULL, NULL, NULL, &is_hylian, 1000000000, 1},
  {"&c", "Hyrulian Knight", NULL, NULL, NULL, &is_hylian, 1000000000, 1},
  {"&c", "Hyrulian Knight", NULL, NULL, NULL, &is_hylian, 1000000000, 1},
  /* Human Ranks */
  {"&R", "Hero", "Valkyrie", NULL, &sanctuary_loss, &is_human, 100000000000ULL, 3},
  {"&r", "Human Paladin", NULL, NULL, NULL, &is_human, 10000000000ULL, 2},
  {"&r", "Human Paladin", NULL, NULL, NULL, &is_human, 10000000000ULL, 2},
  {"&O", "Human Knight", NULL, NULL, NULL, &is_human,  1000000000, 1},
  {"&O", "Human Knight", NULL, NULL, NULL, &is_human,  1000000000, 1},
  {"&O", "Human Knight", NULL, NULL, NULL, &is_human, 1000000000, 1},
  /* Icer Ranks */
  {"&w", "Icerian King", "Icerian Queen", &icer_king_gain, &icer_king_loss, &is_icer,  100000000000ULL, 3},
  {"&p", "Icerian Prince", "Icerian Princess", NULL, NULL, &is_icer, 10000000000ULL ,  2},
  {"&p", "Icerian Prince", "Icerian Princess", NULL, NULL, &is_icer, 10000000000ULL ,  2},
  {"&P", "Icerian Noble", NULL, NULL, NULL, &is_icer,  1000000000, 1},
  {"&P", "Icerian Noble", NULL, NULL, NULL, &is_icer,  1000000000, 1},
  {"&P", "Icerian Noble", NULL, NULL, NULL, &is_icer,  1000000000, 1},
  {"&P", "Icerian Noble", NULL, NULL, NULL, &is_icer,  1000000000, 1},
  /* Halfbreed Ranks */
  {"&Y", "Great Saiyaman", "Great Saiyagirl", &great_saiya_gain, &great_saiya_loss, &is_hb, 100000000000ULL, 3},
  {"&C", "Mystic Warrior", "Mystic Warrioress", NULL, NULL, &is_hb, 10000000000ULL, 2},
  {"&C", "Mystic Warrior", "Mystic Warrioress", NULL, NULL, &is_hb, 10000000000ULL, 2},
  /* Bioandroid Ranks */
  {"&p", "Ultimate Perfection", NULL, NULL, &sanctuary_loss, &is_bio, 100000000000ULL, 3},
  {"&R", "Perfection", NULL, NULL, NULL, &is_bio, 10000000000ULL, 2},
  {"&R", "Perfection", NULL, NULL, NULL, &is_bio, 10000000000ULL, 2},
  {"&O", "Superior Being", NULL, NULL, NULL, &is_bio, 1000000000, 1},
  {"&O", "Superior Being", NULL, NULL, NULL, &is_bio, 1000000000, 1},
  {"&O", "Superior Being", NULL, NULL, NULL, &is_bio, 1000000000, 1},
  /* Namek Ranks */
  {"&G", "Elder Namek", NULL, NULL, &sanctuary_loss, &is_namek, 100000000000ULL, 3},
  {"&g", "Namekian Guardian", NULL, NULL, NULL, &is_namek, 10000000000ULL, 2},
  {"&g", "Namekian Guardian", NULL, NULL, NULL, &is_namek, 10000000000ULL, 2},
  {"&c", "Namekian Healer", NULL, NULL, NULL, &is_namek, 1000000000, 1},
  {"&c", "Namekian Healer", NULL, NULL, NULL, &is_namek, 1000000000, 1},
  {"&c", "Namekian Healer", NULL, NULL, NULL, &is_namek, 1000000000, 1},
  /* Fierian Ranks */
  {"&z", "Dark Flame", NULL, NULL, &sanctuary_loss, &is_fierian, 100000000000ULL, 5},
  {"&z", "Black Flame", NULL, NULL, NULL, &is_fierian, 10000000000ULL, 4},
  {"&R", "Red Flame", NULL, NULL, NULL, &is_fierian, 10000000000ULL, 3},
  {"&B", "Blue Flame", NULL, NULL, NULL, &is_fierian, 10000000000ULL, 2},
  {"&Y", "Yellow Flame", NULL, NULL, NULL, &is_fierian, 1000000000, 1},
  /* Reploid Ranks */
  {"&z", "Omega", NULL, NULL, &sanctuary_loss, &is_reploid, 100000000000ULL, 6},
  {"&R", "Alpha", NULL, NULL, NULL, &is_reploid, 10000000000ULL, 5},
  {"&B", "Beta", NULL, NULL, NULL, &is_reploid, 10000000000ULL, 4},
  {"&G", "Gamma", NULL, NULL, NULL, &is_reploid,  1000000000, 3},
  {"&Y", "Delta", NULL, NULL, NULL, &is_reploid,  1000000000, 2},
  {"&p", "Epsilon", NULL, NULL, NULL, &is_reploid, 1000000000, 1},
  /* Majin Genie Ranks */
  {"&R", "Majin Overlord", NULL, &overlord_gain, &overlord_loss, &is_genie, 100000000000ULL, 3},
  {"&z", "Majin Acolyte", NULL, NULL, NULL, &is_genie, 10000000000ULL, 2},
  {"&p", "Majin Slave", NULL, NULL, NULL, &is_genie,  1000000000, 1},
  {"&p", "Majin Slave", NULL, NULL, NULL, &is_genie,  1000000000, 1},
  {"&p", "Majin Slave", NULL, NULL, NULL, &is_genie, 1000000000, 1},
  /* Saibaman Ranks */
  {"&Y", "Golden Saibaman", NULL, NULL, &sanctuary_loss, &is_saibaman, 100000000000ULL, 3},
  {"&z", "Silver Saibaman", NULL, NULL, NULL, &is_saibaman, 10000000000ULL, 2},
  {"&z", "Silver Saibaman", NULL, NULL, NULL, &is_saibaman, 10000000000ULL, 2},
  {"&R", "Red Saibaman", NULL, NULL, NULL, &is_saibaman, 1000000000, 1},
  {"&R", "Red Saibaman", NULL, NULL, NULL, &is_saibaman, 1000000000, 1},
  {"&R", "Red Saibaman", NULL, NULL, NULL, &is_saibaman, 1000000000, 1},
  /* Dragon Ranks */
  {"&z", "Omega Shenron", NULL, NULL, &sanctuary_loss, &is_dragon, 100000000000ULL, 8},
  {"&R", "Syn Shenron", NULL, NULL, NULL, &is_dragon, 10000000000ULL, 7},
  {"&g", "Haze Shenron", NULL, NULL, NULL, &is_dragon, 10000000000ULL, 6},
  {"&C", "Eis Shenron", NULL, NULL, NULL, &is_dragon, 10000000000ULL, 5},
  {"&r", "Nuova Shenron", NULL, NULL, NULL, &is_dragon, 1000000000, 4},
  {"&p", "Rage Shenron", NULL, NULL, NULL, &is_dragon, 1000000000, 3},
  {"&c", "Oceanus Shenron", NULL, NULL, NULL, &is_dragon, 1000000000, 2},
  {"&O", "Naturon Shenron", NULL, NULL, NULL, &is_dragon, 1000000000, 1},
  /* Mutant Ranks */
  {"&G", "Ultimate Mutation", NULL, NULL, &sanctuary_loss, &is_mutant,  100000000000ULL, 3},
  {"&g", "Elite Mutation", NULL, NULL, NULL, &is_mutant, 10000000000ULL ,  2},
  {"&g", "Elite Mutation", NULL, NULL, NULL, &is_mutant, 10000000000ULL ,  2},
  {"&O", "Superior Mutation", NULL, NULL, NULL, &is_mutant,  1000000000,  1},
  {"&O", "Superior Mutation", NULL, NULL, NULL, &is_mutant,  1000000000, 1},
  {"&O", "Superior Mutation", NULL, NULL, NULL, &is_mutant,  1000000000, 1},
  {"&O", "Superior Mutation", NULL, NULL, NULL, &is_mutant,  1000000000, 1},
  /* */
  {NULL, NULL, NULL, NULL, NULL, 0, 0, 0}
};

struct rank_tables_list_t  rank_lists[] = {
  {"KAIOSHIN", kaio_ranks, 7, &is_kaio},
  {"SAIYAN", saiyan_ranks, 8, &is_pure_saiyan},
  {"DEMON", demon_ranks, 8, &is_demon},
  {"ANDROID", android_ranks, 7, &is_android},
  {"HYLIAN", hylian_ranks, 8, &is_hylian},
  {"HUMAN", human_ranks, 7, &is_human},
  {"ICER", icer_ranks, 8, &is_icer},
  {"HALFBREED", halfbreed_ranks, 4, &is_hb},
  {"BIOANDROID", bioandroid_ranks, 7, &is_bio},
  {"NAMEK", namek_ranks, 7, &is_namek},
  {"FIERIAN", fierian_ranks, 6, &is_fierian},
  {"REPLOID", reploid_ranks, 7, &is_reploid},
  {"GENIE", genie_ranks, 6, &is_genie},
  {"SAIBAMAN", saibaman_ranks, 7, &is_saibaman},
  {"DRAGON", dragon_ranks, 9, &is_dragon},
  {"MUTANT", mutant_ranks, 8, &is_mutant},
  {NULL, 0}
};

RANK_EFFECT(overlord_loss) {
  if(xIS_SET( ch->affected_by, AFF_SANCTUARY))
    xREMOVE_BIT( ch->affected_by, AFF_SANCTUARY );
  ch->pcdata->learned[gsn_majin] = 0;
  ch->canmajin = FALSE;
}

RANK_EFFECT(overlord_gain) {
  ch->pcdata->learned[gsn_majin] = 100;
  ch->canmajin = TRUE;
}

RANK_EFFECT(icer_king_loss) {
  if(xIS_SET( ch->affected_by, AFF_SANCTUARY))
    xREMOVE_BIT( ch->affected_by, AFF_SANCTUARY );
  ch->pcdata->learned[gsn_deathbeam] = 0;
}

RANK_EFFECT(icer_king_gain) {
  ch->pcdata->learned[gsn_deathbeam] = 95;
}

RANK_EFFECT(supreme_kai_loss) {
  if(xIS_SET( ch->affected_by, AFF_SANCTUARY))
    xREMOVE_BIT( ch->affected_by, AFF_SANCTUARY );
  ch->pcdata->learned[gsn_manifestpotara] = 0;
}

RANK_EFFECT(supreme_kai_gain) {
  ch->pcdata->learned[gsn_manifestpotara] = 95;
}

RANK_EFFECT(saiyan_king_loss) {
  if(xIS_SET( ch->affected_by, AFF_SANCTUARY))
    xREMOVE_BIT( ch->affected_by, AFF_SANCTUARY );
  ch->pcdata->learned[gsn_lssj] = 0;
  REMOVE_BIT(ch->pcdata->flags, PCFLAG_KNOWSLSSJ);
}

RANK_EFFECT(saiyan_king_gain) {
  ch->pcdata->learned[gsn_lssj] = 100;
  SET_BIT(ch->pcdata->flags, PCFLAG_KNOWSLSSJ);
}


RANK_EFFECT(great_saiya_gain) {
  ch->pcdata->learned[gsn_mystic] = 100;
  SET_BIT(ch->pcdata->flags, PCFLAG_KNOWSMYSTIC);
}

RANK_EFFECT(great_saiya_loss) {
  if(xIS_SET(ch->affected_by, AFF_SANCTUARY))
    xREMOVE_BIT( ch->affected_by, AFF_SANCTUARY );
  ch->pcdata->learned[gsn_mystic] = 0;
  REMOVE_BIT(ch->pcdata->flags, PCFLAG_KNOWSMYSTIC);
}

RANK_EFFECT(sanctuary_loss) {
  if(xIS_SET(ch->affected_by, AFF_SANCTUARY))
    xREMOVE_BIT( ch->affected_by, AFF_SANCTUARY );
}

/* This checks if a player has a rank.. If they're in a rank table but don't have
 * a rank its removed.. If they have a rank but aren't ina  rank table.. Thats
 * fixed as well
 *  TODO: This function should be tied into nanny to assist in purging ranks
 * Return Codes
 *             0 -> flat out has no rank
 *            -1 -> Has rank but not in a ranktable.. Usually from idle-purges
 *            -2 -> In rank table but no rank on them.. Happens from manual edits
 *            -3 -> Race has no rank table
 *
 *          After this function is called.. saving the character should be in order with any return below 0
 */

int has_rank(CHAR_DATA *ch) {
  int i, r;

  for(i = 0 ; rank_lists[i].name != NULL ; i++)
    if(rank_lists[i].race(ch))
      break;
  
  /* There race can't have a rank */
  if(rank_lists[i].name == NULL && ch->rank > 0) {
    ch->rank = 0;
    return -3;
  }

  for(r = 0; r < rank_lists[i].high ; r++)
    if(rank_lists[i].rank[r] != NULL && !strcmp(rank_lists[i].rank[r], ch->name)) {
      /* They're in the rank table.. */
      if(ch->rank > 0)
	return 1;
      else {
	STRFREE(rank_lists[i].rank[r]);
	rank_lists[i].rank[r] = NULL;
	return -2;
      }
    }

  if(ch->rank > 0) {
    ch->rank = 0;
    return -1;
  } else return 0;

}

void do_abandonrank(CHAR_DATA *ch, char *argument) {
  char tbuf[MAX_STRING_LENGTH];
  int i, r, rt;


  memset(tbuf, '\0', MAX_STRING_LENGTH);

  if(ch->rank < 1) {
    send_to_char("You never had a rank in the first place idiot.", ch);
    return;
  }

  for(i = 0 ; rank_lists[i].name != NULL ; i++)
    if(rank_lists[i].race(ch))
      break;
  
  if(rank_lists[i].name == NULL) {
    send_to_char("No clue how you got a rank buddy.. But thats been fixed. :)", ch);
    ch->rank = 0;
    return;
  }

  /* Make sure they are actually in one of the rank tables too */

  for(r = 0; r < rank_lists[i].high ; r++)
    if(rank_lists[i].rank[r] != NULL && !strcmp(rank_lists[i].rank[r], ch->name)) {
      STRFREE(rank_lists[i].rank[r]);
      rank_lists[i].rank[r] = NULL;
      ch_printf(ch, "You forfeit your rank of %s.\r\n", get_ch_rank(ch));
      sprintf(tbuf, "%s forfeits %s rank of %s", ch->name, (ch->sex == SEX_MALE ? "his" : (ch->sex == SEX_FEMALE ? "her" : "it's")), 
	  get_ch_rank(ch));
      do_info(ch, tbuf);
      break;
    }

  if(*tbuf == '\0') {
    send_to_char("Somehow you have a rank but not really... Thats Fixed :)", ch);
    ch->rank = 0;
    return;
  }

  r = rank_lists[i].high - r;
  for(rt = 0 ; rank_table[rt].rank_name_m != NULL ; rt++) {
    if(rank_table[rt].race(ch)) {
      r--;
    } else continue;
    if(r == 0)
      break;
  }


  if(rank_table[rt].func_loss != NULL) 
    rank_table[rt].func_loss(ch);

  ch->rank = 0;
  save_sysdata(sysdata);

}

void initialize_ranks() {
  int i, r;

  /* Null out our rank spots first */

  for(i = 0; rank_lists[i].name != NULL; i++)
    for(r = 0; r <= rank_lists[i].high ; r++)
      rank_lists[i].rank[r] = NULL;
}

/* Retrieve their character rank if they have one */
char *get_ch_rank(CHAR_DATA *ch) {
  int i;

  for(i = 0; rank_table[i].rank_name_m != NULL ; i++)
    if(rank_table[i].race(ch)  && rank_table[i].value == ch->rank) 
      return ((ch->sex == SEX_FEMALE && rank_table[i].rank_name_f) ? rank_table[i].rank_name_f : rank_table[i].rank_name_m);
  return "";

}

int get_rank_list_num(CHAR_DATA *ch) {
  int i;

  for(i = 0 ; rank_lists[i].name !=NULL ; i++)
    if(rank_lists[i].race(ch))
      return i;
  return -1;
}

/* Retrieve what their rank would be called if they were at X num */
char *get_ch_rank_at(CHAR_DATA *ch, int num) {
  int i;

  for(i = 0; rank_table[i].rank_name_m != NULL ; i++)
    if(rank_table[i].race(ch) && rank_table[i].value == num) 
      return ((ch->sex == SEX_FEMALE && rank_table[i].rank_name_f) ? rank_table[i].rank_name_f : rank_table[i].rank_name_m);
  return "";

}



  /* We're passing over the filepoitner to sysdata.dat file..  Only write the ranks that aren't empty */
void save_ranks(FILE *fp) { 
  int r, i;

  for(r = 0;  rank_lists[r].name != NULL ; r++) 
    for(i = 0; i < rank_lists[r].high ; i++) 
      if(rank_lists[r].rank[i] != NULL) 
	/* Write this rank */
	fprintf(fp, "RANK_ENTRY %s~ %d %s~\n", rank_lists[r].name, i, rank_lists[r].rank[i] );

}


void load_rank(char *race, int rank_num, char *player) {
  int i;

  /* Make sure Race is a valid race */
  for(i = 0 ; rank_lists[i].name != NULL; i++)
    if(!str_cmp(rank_lists[i].name, race))
      break;

  if(rank_lists[i].name == NULL)
    return;

  /* Make sure rank_num is valid *
   */

  if(rank_num < 0 || rank_num >= rank_lists[i].high)
    return;
  
  /* load the guy */
  rank_lists[i].rank[rank_num] = player;
}

void do_showrank( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *rch;
  sex_types sex;
  char tbuf[MAX_STRING_LENGTH];
  char arg[MAX_STRING_LENGTH];
  int i, ri;
  short cur_l;

  *tbuf = '\0';
  argument = one_argument(argument, arg);
  
  if(!arg || !*arg) {
    send_to_char("&rMust supply a valid race to show ranks for.&d\r\n", ch);
    return;
  }

  for(ri = 0; rank_lists[ri].name != NULL; ri++)
    if(string_prefix(rank_lists[ri].name, arg))
      break;
  if(!rank_lists[ri].name) {
    send_to_char("&rNo such rank table.&d\r\n", ch);
    return;
  }
  
  send_to_char("\r\n", ch);
  ch_printf(ch, "&c%s ranks and who currently holds them:\r\n", mud_capstr(rank_lists[ri].name) );
  send_to_char("&z====================================================&d\r\n", ch );

  /* Find our spot first */
  cur_l = rank_lists[ri].high-1;

  for(i = 0 ; rank_table[i].race != rank_lists[ri].race; i++)
    ;

  while(rank_table[i].race == rank_lists[ri].race) {
    if(rank_table[i].color_prefix != NULL)
      send_to_char(rank_table[i].color_prefix, ch);
    if(rank_lists[ri].rank[cur_l] == NULL) {
      sex = SEX_MALE; 
      strcpy(tbuf, "&z(Vacant)&d");
    } else if((rch = get_char_name(rank_lists[ri].rank[cur_l])))
	sex = rch->sex;
    else
      sex = SEX_NEUTRAL;
    
    
    ch_printf(ch, "%-20s: %-17s\r\n", 
	sex == SEX_FEMALE && rank_table[i].rank_name_f != NULL ? rank_table[i].rank_name_f : rank_table[i].rank_name_m,
	*tbuf != '\0' ? tbuf : rank_lists[ri].rank[cur_l] );

    *tbuf = '\0';
    cur_l--;
    i++;
  }
}


void do_claimrank( CHAR_DATA * ch, char *argument )
{
  char tbuf[MAX_STRING_LENGTH];
  int ri, i, ft, r;
  int taken = 0;


  for(i = 0 ;  rank_lists[i].name != NULL; i++)
    if(rank_lists[i].race(ch))
      break;

  if(rank_lists[i].name == NULL) {
    ch_printf( ch, "&wCommand unavailable to your race.&d\n\r" );
    return;
  }

 
  if(is_fused(ch)) {
    send_to_char("&wCommand not available while fused.&d\r\n", ch);
    return;
  }


  if(!argument || !*argument) {
    send_to_char("&rInvalid syntax.  Must supply a rank you would like to claim.&d", ch);
    return;
  }

  for(ri = 0; rank_lists[ri].name != NULL; ri++)
    if(rank_lists[ri].race(ch))
      break;

  if(rank_lists[ri].name == NULL) {
    bug("Fatal Error, ranks.c:%d", __LINE__ );
    return;
  }

  ft = rank_lists[ri].high;

  /* Ok.. now check and see if arg is a rank for their race to pick.. */
  for(i = 0; rank_table[i].rank_name_m != NULL ; i++)  {
    if(rank_table[i].race == rank_lists[ri].race)
      ft--;
    if(rank_table[i].race(ch) && 
	(((ch->sex == SEX_FEMALE && string_prefix(rank_table[i].rank_name_f, argument)) ||
	  ((ch->sex != SEX_FEMALE || rank_table[i].rank_name_f == NULL ) && 
	   string_prefix(rank_table[i].rank_name_m, argument))))) {
                /* Ok we got a good match.. now lets make sure they can actually take this rank */
           if(rank_table[i].value < ch->rank) {
	     send_to_char("You really want to demote yourself that bad?", ch);
	     return;
	   } else if(rank_lists[ri].rank[ft] != NULL) { /* Rank Taken */
	     taken = 1;
	     continue;
	   } else if(ch->exp < rank_table[i].pl_req) {
	     pager_printf(ch, "You need to obtain a base powerlevel of %s to claim that rank.", num_punct_ld(rank_table[i].pl_req));
	     return;
	   }
	   /* Make sure they didn't have a rank before */
	   if(ch->rank > 0){  /* free it */
	     /* TODO: run func_loss on old rank.. my brain stopped at this hour.. later */
	     for(r = 0; r < rank_lists[ri].high ; r++)
	       if(!str_cmp(rank_lists[ri].rank[r], ch->name)) { 
		 STRFREE(rank_lists[ri].rank[r]);
		 rank_lists[ri].rank[r] = NULL;
		 break;
	       }
	   }

	   /* They're good claim it */
	   if(rank_table[i].func_gain != NULL) /* see if there is any special effects to apply for this */
	     rank_table[i].func_gain(ch);
	   ch->rank = rank_table[i].value;
	   rank_lists[ri].rank[ft] = STRALLOC(ch->name);
	   sprintf(tbuf, "%s has claimed the rank of %s.\r\n", ch->name, get_ch_rank(ch));
	   send_to_char(tbuf, ch);
	   sprintf(tbuf, "%s has claimed the rank of %s.", ch->name, get_ch_rank(ch));
	   do_info(ch, tbuf);
	
	   save_char_obj(ch);
	   save_sysdata(sysdata);
	   return;
	}
  }
  if(taken) {
    send_to_char("That rank is already taken.", ch);
    return;
  }
  send_to_char("No such rank to claim.", ch);
}



/* Return 1 string begins with prefix, 0 otherwise */
int string_prefix(char *string, char *prefix) {
  if(!string || !prefix)
    return 0;
  while(*string && *prefix && toupper(*string++) == toupper(*prefix++))
    ;
  return *prefix == '\0';
}

char *mud_capstr(char *string) {
  static char tbuf[MAX_STRING_LENGTH];
  char *bp;
  char *p;

  memset(tbuf, '\0', MAX_STRING_LENGTH);
  bp = tbuf;
  p = string;

  if(!string || !*string)
    return "";

  sbuf_chr(toupper(*p++), tbuf, &bp);

  while(p && *p != '\0')
    sbuf_chr(tolower(*p++), tbuf, &bp);

  *bp = '\0';

  return tbuf;
 
}

int
sbuf_format(char *buf, char **bptr, const char *v_args, ...)
{
  int len, blen, clen;
  char tbuf[MAX_STRING_LENGTH];
  va_list args;

  va_start(args, v_args);

  memset(tbuf, '\0', MAX_STRING_LENGTH);
  vsnprintf(tbuf, sizeof(tbuf), v_args, args);
  va_end(args);

  if(*tbuf == '\0')
    return 0;

  if((tbuf+1) && *(tbuf+1) == '\0')
    return sbuf_chr(*tbuf, buf, bptr);

  len = strlen(tbuf);
  blen = *bptr - buf;

  if(blen > (MAX_STRING_LENGTH - 1))
    return len;

  clen = ((len + blen) <= (MAX_STRING_LENGTH - 1)) ? len : (MAX_STRING_LENGTH - 1) - blen;

  memcpy(*bptr, tbuf, clen);

  *bptr += clen;

  return len - clen;
}


