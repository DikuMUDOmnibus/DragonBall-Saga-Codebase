/****************************************************************************
 * [S]imulated [M]edieval [A]dventure multi[U]ser [G]ame      |   \\._.//   *
 * -----------------------------------------------------------|   (0...0)   *
 * SMAUG 1.4 (C) 1994, 1995, 1996, 1998  by Derek Snider      |    ).:.(    *
 * -----------------------------------------------------------|    {o o}    *
 * SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,      |   / ' ' \   *
 * Scryn, Rennard, Swordbearer, Gorog, Grishnakh, Nivek,      |~'~.VxvxV.~'~*
 * Tricops and Fireblade                                      |             *
 * ------------------------------------------------------------------------ *
 * Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael        *
 * Chastain, Michael Quan, and Mitchell Tse.                                *
 * Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,          *
 * Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.     *
 * ------------------------------------------------------------------------ *
 *			Specific object creation module			    *
 ****************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "mud.h"

int get_rank_by_pl( long double level )
{

  if( level < 5000 )
    return 1;
  else if( level < 100000 )
    return 2;
  else if( level < 1000000 )
    return 3;
  else if( level < 10000000 )
    return 4;
  else if( level < 100000000 )
    return 5;
  else if( level < 1000000000 )
    return 6;
  else if( level < 10000000000ULL )
    return 7;
  else if( level < 50000000000ULL )
    return 8;
  else if( level < 100000000000ULL )
    return 9;
  else if( level < 300000000000ULL )
    return 10;
  else if( level < 600000000000ULL )
    return 11;
  else if( level < 1000000000000ULL )
    return 12;
  else if( level < 10000000000000ULL )
    return 13;
  else if( level < 50000000000000ULL )
    return 14;
  else if( level < 100000000000000ULL )
    return 15;
  else
    return 16;

  return 0;
}

int calc_zeni( long double level, CHAR_DATA * killer )
{
  long double chklvl = 0;
  int zeros = 0;
  int i;
  int zeniBase = 0;
  int prevZeni = 0;
  int zeni = 0;
  double ratio = 0.00;
  long double chklvl2 = 0;

  for( i = 0; i < 100; i++ )  // i'm "cheeping" out and caping this at 1 googol -Goku 10.05.04
  {
    if( level / pow( 10, i ) < 10 )
    {
      break;
    }
  }

  chklvl = pow( 10, i );
  chklvl2 = pow( 10, i - 1 );

  /*
   * double zero count to smooth out increase -Goku 10.05.04 
   */
  if( level < chklvl - chklvl2 + chklvl2 )
    zeros = i * 2 - 1;
  else
    zeros = i * 2;

  switch ( zeros )
  {
    case 0:  // 0
    case 1:  // 1
      zeniBase = 1;
      prevZeni = 1;
      break;
    case 2:  // 5
      zeniBase = 5;
      prevZeni = 1;
      break;
    case 3:  // 10
      zeniBase = 10;
      prevZeni = 5;
      break;
    case 4:  // 50
      zeniBase = 20;
      prevZeni = 10;
      break;
    case 5:  // 100
    case 6:  // 500
      zeniBase = 30;
      prevZeni = 20;
      break;
    case 7:  // 1000
    case 8:  // 5000
      zeniBase = 40;
      prevZeni = 30;
      break;
    case 9:  // 10k
      zeniBase = 60;
      prevZeni = 40;
      break;
    case 10: // 50k
      zeniBase = 90;
      prevZeni = 60;
      break;
    case 11: // 100k
      zeniBase = 150;
      prevZeni = 90;
      break;
    case 12: // 500k
      zeniBase = 170;
      prevZeni = 150;
      break;
    case 13: // 1m
      zeniBase = 200;
      prevZeni = 170;
      break;
    case 14: // 5m
      zeniBase = 300;
      prevZeni = 200;
      break;
    case 15: // 10m
      zeniBase = 400;
      prevZeni = 300;
      break;
    default:
      zeniBase = 500 + ( ( zeros - 15 ) * 100 );
      prevZeni = 400 + ( ( zeros - 15 ) * 100 );
      break;
  }

  ratio = level / chklvl;

  zeni = prevZeni + ( ( zeniBase - prevZeni ) * ratio );
  zeni = ( dice( get_rank_by_pl( level ), zeni )
           + ( dice( get_rank_by_pl( level ), zeni ) / 10 + dice( get_curr_lck( killer ) / 3, zeni / 3 ) ) );

  return zeni / 4;

}

void generate_treasure( CHAR_DATA * killer, CHAR_DATA * ch, OBJ_DATA * corpse )
{
  int tchance;
  int zeni;

  /*
   * Rolling for the initial check to see if we should be generating anything at all 
   */
  tchance = number_range( 1, 100 );

  if( tchance <= 35 )
  {
    return;
  }

  else if( tchance <= 65 )
  {
    zeni = calc_zeni( ch->exp, killer );
    if( zeni < 0 )
    {
      bug( "RTG Made less than 0 zeni: PC(%s) Mob(%d) Room(%d) Zeni(%d)", killer->name, ch->pIndexData->vnum,
           ch->in_room->vnum, zeni );
      return;
    }
    if( ch->in_room )
    {
      ch->in_room->area->gold_looted += zeni;
      sysdata.global_looted += zeni / 100;
    }
    if( economy_has( ch->in_room->area, zeni ) )
    {
      lower_economy( ch->in_room->area, zeni );
      obj_to_obj( create_money( zeni ), corpse );
    }
    return;
  }

}

/*
 * Make a fire.
 */
void make_fire( ROOM_INDEX_DATA * in_room, sh_int timer )
{
  OBJ_DATA *fire;

  fire = create_object( get_obj_index( OBJ_VNUM_FIRE ), 0 );
  fire->timer = number_fuzzy( timer );
  obj_to_room( fire, in_room );
  return;
}

/*
 * Make a trap.
 */
OBJ_DATA *make_trap( int v0, int v1, int v2, int v3 )
{
  OBJ_DATA *trap;

  trap = create_object( get_obj_index( OBJ_VNUM_TRAP ), 0 );
  trap->timer = 0;
  trap->value[0] = v0;
  trap->value[1] = v1;
  trap->value[2] = v2;
  trap->value[3] = v3;
  return trap;
}


/*
 * Turn an object into scraps.		-Thoric
 */
void make_scraps( OBJ_DATA * obj )
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *scraps, *tmpobj;
  CHAR_DATA *ch = NULL;

  separate_obj( obj );
  scraps = create_object( get_obj_index( OBJ_VNUM_SCRAPS ), 0 );
  scraps->timer = number_range( 5, 15 );

  /*
   * don't make scraps of scraps of scraps of ... 
   */
  if( obj->pIndexData->vnum == OBJ_VNUM_SCRAPS )
  {
    STRFREE( scraps->short_descr );
    scraps->short_descr = STRALLOC( "some debris" );
    STRFREE( scraps->description );
    scraps->description = STRALLOC( "Bits of debris lie on the ground here." );
  }
  else
  {
    sprintf( buf, scraps->short_descr, obj->short_descr );
    STRFREE( scraps->short_descr );
    scraps->short_descr = STRALLOC( buf );
    sprintf( buf, scraps->description, obj->short_descr );
    STRFREE( scraps->description );
    scraps->description = STRALLOC( buf );
  }

  if( obj->carried_by )
  {
    act( AT_OBJECT, "$p falls to the ground in scraps!", obj->carried_by, obj, NULL, TO_CHAR );
    if( obj == get_eq_char( obj->carried_by, WEAR_WIELD )
        && ( tmpobj = get_eq_char( obj->carried_by, WEAR_DUAL_WIELD ) ) != NULL )
      tmpobj->wear_loc = WEAR_WIELD;

    obj_to_room( scraps, obj->carried_by->in_room );
  }
  else if( obj->in_room )
  {
    if( ( ch = obj->in_room->first_person ) != NULL )
    {
      act( AT_OBJECT, "$p is reduced to little more than scraps.", ch, obj, NULL, TO_ROOM );
      act( AT_OBJECT, "$p is reduced to little more than scraps.", ch, obj, NULL, TO_CHAR );
    }
    obj_to_room( scraps, obj->in_room );
  }
  if( ( obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_KEYRING
        || obj->item_type == ITEM_QUIVER || obj->item_type == ITEM_CORPSE_PC ) && obj->first_content )
  {
    if( ch && ch->in_room )
    {
      act( AT_OBJECT, "The contents of $p fall to the ground.", ch, obj, NULL, TO_ROOM );
      act( AT_OBJECT, "The contents of $p fall to the ground.", ch, obj, NULL, TO_CHAR );
    }
    if( obj->carried_by )
      empty_obj( obj, NULL, obj->carried_by->in_room );
    else if( obj->in_room )
      empty_obj( obj, NULL, obj->in_room );
    else if( obj->in_obj )
      empty_obj( obj, obj->in_obj, NULL );
  }
  extract_obj( obj );
}

OBJ_DATA *make_deathCertificate( CHAR_DATA * ch, CHAR_DATA * killer )
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *dc;
  EXTRA_DESCR_DATA *ed;

  dc = create_object( get_obj_index( 610 ), 1 );

  sprintf( buf, "%s death certificate", ch->name );
  STRFREE( dc->name );
  dc->name = STRALLOC( buf );
  sprintf( buf, "%s's death certificate", ch->name );
  STRFREE( dc->short_descr );
  dc->short_descr = STRALLOC( buf );

  ed = SetOExtra( dc, dc->name );
  sprintf( buf,
           "Certificate of Death\n\r"
           "--------------------\n\r"
           "Name: &W%s&w\n\r"
           "Rank: &W%s&w\n\r"
           "Sex : &W%s&w  Race: &W%s&w\n\r"
           "Cause of Death: &WHomicide&w\n\r"
           "Time of Death : &W%s&w\r"
           "Area of the Crime : &W%s&w\n\r"
           "Scene of the Crime: &W%s&w\n\r"
           "Suspect Information\n\r"
           "-------------------\n\r"
           "Name: &W%s&w\n\r"
           "Rank: &W%s&w\n\r"
           "Sex : &W%s&w  Race: &W%s&w\n\r"
           "Reason for Involvement:\n\r"
           "  &W'Possible' Self Defense&w\n\r",
           ch->name,
           get_rank( ch ),
           ch->sex == SEX_MALE ? "Male" : ch->sex == SEX_FEMALE ? "Female" : "Neutral", capitalize( get_race( ch ) ),
           ctime( &current_time ),
           killer->in_room->area->name,
           killer->in_room->name,
           killer->name,
           get_rank( killer ),
           killer->sex == SEX_MALE ? "Male" : killer->sex == SEX_FEMALE ? "Female" : "Neutral",
           capitalize( get_race( killer ) ) );
  STRFREE( ed->description );
  ed->description = STRALLOC( buf );

  return dc;
}

/*
 * Goku's global drop code.  Checks if a mob will drop any of
 * the items defined when it's killed.  -Goku 07.28.04
 */
#define GLOBAL_DROP_OBJ_TOKEN 2

void global_drop_check( OBJ_DATA * corpse, CHAR_DATA * ch, CHAR_DATA * killer )
{
  int luckMod;

  luckMod = get_curr_lck( killer );

  /*
   * if luck is lower than 0, no drops 
   */
  if( luckMod < 1 )
    return;

  if( number_range( luckMod, 10000 ) <= 1 )
  {
    obj_to_obj( create_object( get_obj_index( GLOBAL_DROP_OBJ_TOKEN ), 0 ), corpse );
  }

  return;
}

#undef GLOBAL_DROP_OBJ_TOKEN

/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA * ch, CHAR_DATA * killer )
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *corpse;
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  char *name;

  if( !ch )
  {
    if( !killer )
      bug( "make_corpse: NULL TARGET and NULL KILLER", 0 );
    else
      bug( "make_corpse: NULL TARGET, %s is the KILLER", killer->name, 0 );
    return;
  }
  if( !killer )
  {
    if( !ch )
      bug( "make_corpse: NULL KILLER and NULL TARGET", 0 );
    else
      bug( "make_corpse: NULL KILLER, %s as TARGET", ch->name, 0 );
    return;
  }

  if( IS_NPC( ch ) )
  {
    name = ch->short_descr;
    corpse = create_object( get_obj_index( OBJ_VNUM_CORPSE_NPC ), 0 );
    corpse->timer = 6;
    if( ch->gold > 0 )
    {
      if( ch->in_room )
      {
        ch->in_room->area->gold_looted += ch->gold;
        sysdata.global_looted += ch->gold / 100;
      }
      obj_to_obj( create_money( ch->gold ), corpse );
      ch->gold = 0;
    }
    else if( ch->gold < 0 && !IS_NPC( killer ) )
      generate_treasure( killer, ch, corpse );

/* Cannot use these!  They are used.
	corpse->value[0] = (int)ch->pIndexData->vnum;
	corpse->value[1] = (int)ch->max_hit;
*/
/*	Using corpse cost to cheat, since corpses not sellable */
    corpse->cost = ( -( int )ch->pIndexData->vnum );
    corpse->value[2] = corpse->timer;
  }
  else
  {
    name = ch->name;
    corpse = create_object( get_obj_index( OBJ_VNUM_CORPSE_PC ), 0 );
    if( in_arena( ch ) )
      corpse->timer = 30;
    else
      corpse->timer = 15;
    corpse->value[2] = ( int )( corpse->timer / 8 );
    corpse->value[4] = ch->level;
    if( CAN_PKILL( ch ) && sysdata.pk_loot )
      xSET_BIT( corpse->extra_flags, ITEM_CLANCORPSE );
    /*
     * Pkill corpses get save timers, in ticks (approx 70 seconds)
     * This should be anough for the killer to type 'get all corpse'. 
     */
    if( !IS_NPC( ch ) && !IS_NPC( killer ) )
      corpse->value[3] = 1;
    else
      corpse->value[3] = 0;
  }

  if( CAN_PKILL( ch ) && CAN_PKILL( killer ) && ch != killer )
  {
    sprintf( buf, "%s", killer->name );
    STRFREE( corpse->action_desc );
    corpse->action_desc = STRALLOC( buf );
  }

  /*
   * Added corpse name - make locate easier , other skills 
   */
  sprintf( buf, "corpse %s", name );
  STRFREE( corpse->name );
  corpse->name = STRALLOC( buf );

  sprintf( buf, corpse->short_descr, name );
  STRFREE( corpse->short_descr );
  corpse->short_descr = STRALLOC( buf );

  sprintf( buf, corpse->description, name );
  STRFREE( corpse->description );
  corpse->description = STRALLOC( buf );

  for( obj = ch->first_carrying; obj; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( !IS_NPC( ch ) && obj->wear_loc != -1 )
      continue;
    obj_from_char( obj );

    if( obj->item_type == ITEM_DRAGONBALL )
    {
      obj_to_room( obj, ch->in_room );
      continue;
    }

    if( ( !IS_NPC( ch ) && IS_OBJ_STAT( obj, ITEM_INVENTORY ) ) || IS_OBJ_STAT( obj, ITEM_DEATHROT ) )
      extract_obj( obj );
    else
      obj_to_obj( obj, corpse );
  }

  /*
   * global drop? -Goku 07.28.04 
   */
  if( !IS_NPC( killer ) && IS_NPC( ch ) && !xIS_SET( ch->affected_by, AFF_NO_GLOBAL_DROP ) && killer->pl / ch->exp <= 5 )
    global_drop_check( corpse, ch, killer );

/*
	if (!IS_NPC(ch) && !IS_NPC(killer))
	{
		obj_to_obj( make_deathCertificate(ch, killer), corpse );
	}
*/
  if( spaceDeath )
    obj_to_room( corpse, get_room_index( ROOM_CORPSE_DROPOFF ) );
  else
    obj_to_room( corpse, ch->in_room );

  spaceDeath = FALSE;
  return;
}



void make_blood( CHAR_DATA * ch )
{
  OBJ_DATA *obj;

  obj = create_object( get_obj_index( OBJ_VNUM_BLOOD ), 0 );
  obj->timer = number_range( 2, 4 );
  obj->value[1] = number_range( 3, UMIN( 5, ch->level ) );
  obj_to_room( obj, ch->in_room );
}


void make_bloodstain( CHAR_DATA * ch )
{
  OBJ_DATA *obj;

  obj = create_object( get_obj_index( OBJ_VNUM_BLOODSTAIN ), 0 );
  obj->timer = number_range( 1, 2 );
  obj_to_room( obj, ch->in_room );
}


/*
 * make some coinage
 */
OBJ_DATA *create_money( int amount )
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;

  if( amount <= 0 )
  {
    bug( "Create_money: zero or negative money %d.", amount );
    amount = 1;
  }

  if( amount == 1 )
  {
    obj = create_object( get_obj_index( OBJ_VNUM_MONEY_ONE ), 0 );
  }
  else
  {
    obj = create_object( get_obj_index( OBJ_VNUM_MONEY_SOME ), 0 );
    sprintf( buf, obj->short_descr, amount );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );
    obj->value[0] = amount;
  }

  return obj;
}
