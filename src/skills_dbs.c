#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"
#include "skills.h"

extern bool removeGenieTrans args( ( CHAR_DATA * ch ) );
void transform_golden_oozaru args( ( CHAR_DATA * ch ) );
void transform_oozaru args( ( CHAR_DATA * ch ) );
int summon_state;

AREA_DATA *summon_area;
ROOM_INDEX_DATA *summon_room;

/* this is for spirit bomb, if we ever need it some where else
   it should probably be moved to new_fun.c  -Goku */
int get_npc_rank( CHAR_DATA * ch )
{

  if( !IS_NPC( ch ) )
    return get_true_rank( ch );

  if( ch->exp < 5000 )
    return 1;
  else if( ch->exp < 100000 )
    return 2;
  else if( ch->exp < 1000000 )
    return 3;
  else if( ch->exp < 10000000 )
    return 4;
  else if( ch->exp < 100000000 )
    return 5;
  else if( ch->exp < 1000000000 )
    return 6;
  else if( ch->exp < 10000000000ULL )
    return 7;
  else if( ch->exp < 50000000000ULL )
    return 8;
  else if( ch->exp < 100000000000ULL )
    return 9;
  else if( ch->exp < 300000000000ULL )
    return 10;
  else if( ch->exp < 600000000000ULL )
    return 11;
  else if( ch->exp < 1000000000000ULL )
    return 12;
  else if( ch->exp >= 1000000000000ULL )
    return 13;

  return 0;
}

/* For puting people back together */
void reuniteSplitForms( CHAR_DATA * ch )
{
  int count = 0;
  CHAR_DATA *och;
  CHAR_DATA *och_next;
  CHAR_DATA *purgeQue[5];


  for( och = first_char; och; och = och_next )
  {
    och_next = och->next;

    if( !IS_NPC( och ) )
      continue;

    if( xIS_SET( och->affected_by, AFF_SPLIT_FORM ) && och->master == ch )
    {
      if( och->in_room != ch->in_room )
      {
        send_to_char( "You must be in the same room as your double.", ch );
        return;
      }
      extract_char( och, TRUE );
      xREMOVE_BIT( ( ch )->affected_by, AFF_SPLIT_FORM );
      act( AT_SKILL, "You and your double merge back into one being.", ch, NULL, NULL, TO_CHAR );
      act( AT_SKILL, "$n and $s double merge back into one being.", ch, NULL, NULL, TO_NOTVICT );
      return;
    }

    if( xIS_SET( och->affected_by, AFF_BIOJR ) && och->master == ch )
    {
      act( AT_BLUE, "$n explodes in a cloud of blue smoke.", och, NULL, NULL, TO_NOTVICT );
      extract_char( och, TRUE );
    }


    if( ( xIS_SET( och->affected_by, AFF_TRI_FORM ) || xIS_SET( och->affected_by, AFF_MULTI_FORM ) ) && och->master == ch )
    {
      purgeQue[count] = och;
      count++;
    }
  }

  if( xIS_SET( ( ch )->affected_by, AFF_BIOJR ) )
  {
    xREMOVE_BIT( ( ch )->affected_by, AFF_BIOJR );
    return;
  }

  if( ( xIS_SET( ch->affected_by, AFF_TRI_FORM ) && count == 2 ) )
  {
    if( purgeQue[0]->in_room != ch->in_room || purgeQue[1]->in_room != ch->in_room )
    {
      send_to_char( "You must be in the same room as all your doubles.", ch );
      return;
    }
    else
    {
      extract_char( purgeQue[0], TRUE );
      extract_char( purgeQue[1], TRUE );
      xREMOVE_BIT( ( ch )->affected_by, AFF_TRI_FORM );

      if( IS_DEMON( ch ) )
      {
        act( AT_DGREY, "You banish your minions back to the Shadow Earth realm.", ch, NULL, NULL, TO_CHAR );
        act( AT_DGREY, "$n banishes $s minions back to the Shadow Earth realm.", ch, NULL, NULL, TO_NOTVICT );
      }
      else
      {
        act( AT_YELLOW, "You and your doubles merge back into one being.", ch, NULL, NULL, TO_CHAR );
        act( AT_YELLOW, "$n and $s doubles merge back into one being.", ch, NULL, NULL, TO_NOTVICT );
      }
      return;
    }
  }

  if( xIS_SET( ch->affected_by, AFF_MULTI_FORM ) && count == 3 )
  {
    if( purgeQue[0]->in_room != ch->in_room || purgeQue[1]->in_room != ch->in_room || purgeQue[2]->in_room != ch->in_room )
    {
      send_to_char( "You must be in the same room as all your doubles.", ch );
      return;
    }
    else
    {
      extract_char( purgeQue[0], TRUE );
      extract_char( purgeQue[1], TRUE );
      extract_char( purgeQue[2], TRUE );
      xREMOVE_BIT( ( ch )->affected_by, AFF_MULTI_FORM );
      act( AT_WHITE, "You and your doubles merge back into one being.", ch, NULL, NULL, TO_CHAR );
      act( AT_WHITE, "$n and $s doubles merge back into one being.", ch, NULL, NULL, TO_NOTVICT );
      return;
    }
  }

  send_to_char( "Something weird happened?", ch );
  /*
   * bug("reuniteSplitForms: couldn't clean splits normaly, purging all of %s's forms", ch->name);
   */

  for( och = first_char; och; och = och_next )
  {
    och_next = och->next;

    if( !IS_NPC( och ) )
      continue;

    if( ( xIS_SET( och->affected_by, AFF_SPLIT_FORM )
          || xIS_SET( och->affected_by, AFF_TRI_FORM )
          || xIS_SET( och->affected_by, AFF_MULTI_FORM ) || xIS_SET( och->affected_by, AFF_BIOJR ) ) && och->master == ch )
    {
      extract_char( och, TRUE );
    }
  }
  xREMOVE_BIT( ( ch )->affected_by, AFF_BIOJR );
  xREMOVE_BIT( ( ch )->affected_by, AFF_MULTI_FORM );
  xREMOVE_BIT( ( ch )->affected_by, AFF_TRI_FORM );
  xREMOVE_BIT( ( ch )->affected_by, AFF_SPLIT_FORM );

  return;
}

/* Copy player stats to a mob for split, tri, and multi form */
void statsToMob( CHAR_DATA * ch, CHAR_DATA * victim, sh_int form_gsn, bool botched, sh_int amount, sh_int num )
{

  char buf[MAX_STRING_LENGTH];
  float botchMod = 0;

  if( IS_DEMON( ch ) )  /* For the minions command */
  {
    sprintf( buf, "minion%s%d", ch->name, num );
    STRFREE( victim->name );
    victim->name = STRALLOC( buf );
    STRFREE( victim->short_descr );
    victim->short_descr = STRALLOC( "Demon Minion" );
  }
  else if( form_gsn == gsn_clone )  /* For the bio clone skill */
  {
    sprintf( buf, "clone%s%d", ch->name, num );
    STRFREE( victim->name );
    victim->name = STRALLOC( buf );
    STRFREE( victim->short_descr );
    sprintf( buf, "%s Jr", ch->name );
    victim->short_descr = STRALLOC( buf );
  }
  else  /* Split, Tri, and Multi form */
  {
    sprintf( buf, "split%s%d", ch->name, num );
    STRFREE( victim->name );
    victim->name = STRALLOC( buf );
    STRFREE( victim->short_descr );
    victim->short_descr = STRALLOC( ch->name );
  }

  if( xIS_SET( ch->affected_by, AFF_SNAMEK ) )
    xSET_BIT( victim->affected_by, AFF_SNAMEK );
  if( xIS_SET( ch->affected_by, AFF_HYPER ) )
    xSET_BIT( victim->affected_by, AFF_HYPER );
  if( xIS_SET( ch->affected_by, AFF_GROWTH ) )
    xSET_BIT( victim->affected_by, AFF_GROWTH );
  if( xIS_SET( ch->affected_by, AFF_GIANT ) )
    xSET_BIT( victim->affected_by, AFF_GIANT );
  if( xIS_SET( ch->affected_by, AFF_KAIOKEN ) )
    xSET_BIT( victim->affected_by, AFF_KAIOKEN );
  if( xIS_SET( ch->affected_by, AFF_SSJ ) )
    xSET_BIT( victim->affected_by, AFF_SSJ );
  if( xIS_SET( ch->affected_by, AFF_SSJ2 ) )
    xSET_BIT( victim->affected_by, AFF_SSJ2 );
  if( xIS_SET( ch->affected_by, AFF_SSJ3 ) )
    xSET_BIT( victim->affected_by, AFF_SSJ3 );
  if( xIS_SET( ch->affected_by, AFF_SSJ4 ) )
    xSET_BIT( victim->affected_by, AFF_SSJ4 );
  if( xIS_SET( ch->affected_by, AFF_DEAD ) )
    xSET_BIT( victim->affected_by, AFF_DEAD );
  if( xIS_SET( ch->affected_by, AFF_USSJ ) )
    xSET_BIT( victim->affected_by, AFF_USSJ );
  if( xIS_SET( ch->affected_by, AFF_USSJ2 ) )
    xSET_BIT( victim->affected_by, AFF_USSJ2 );
  if( xIS_SET( ch->affected_by, AFF_FLYING ) )
    xSET_BIT( victim->affected_by, AFF_FLYING );
  if( xIS_SET( ch->affected_by, AFF_SEMIPERFECT ) )
    xSET_BIT( victim->affected_by, AFF_SEMIPERFECT );
  if( xIS_SET( ch->affected_by, AFF_PERFECT ) )
    xSET_BIT( victim->affected_by, AFF_PERFECT );
  if( xIS_SET( ch->affected_by, AFF_ULTRAPERFECT ) )
    xSET_BIT( victim->affected_by, AFF_ULTRAPERFECT );
  if( xIS_SET( ch->affected_by, AFF_EXTREME ) )
    xSET_BIT( victim->affected_by, AFF_EXTREME );
  if( xIS_SET( ch->affected_by, AFF_MYSTIC ) )
    xSET_BIT( victim->affected_by, AFF_MYSTIC );
  //Lssj in splits
  if( xIS_SET( ch->affected_by, AFF_LSSJ ) )
    xSET_BIT( victim->affected_by, AFF_LSSJ );
  if( xIS_SET( ch->affected_by, AFF_EVILBOOST ) )
    xSET_BIT( victim->affected_by, AFF_EVILBOOST );
  if( xIS_SET( ch->affected_by, AFF_EVILSURGE ) )
    xSET_BIT( victim->affected_by, AFF_EVILSURGE );
  if( xIS_SET( ch->affected_by, AFF_EVILOVERLOAD ) )
    xSET_BIT( victim->affected_by, AFF_EVILOVERLOAD );
  if( xIS_SET( ch->affected_by, AFF_EVIL_TRANS ) )
    xSET_BIT( victim->affected_by, AFF_EVIL_TRANS );
  if( xIS_SET( ch->affected_by, AFF_SUPER_TRANS ) )
    xSET_BIT( victim->affected_by, AFF_SUPER_TRANS );
  if( xIS_SET( ch->affected_by, AFF_KID_TRANS ) )
    xSET_BIT( victim->affected_by, AFF_KID_TRANS );
  if( xIS_SET( ch->affected_by, AFF_UNAMEK ) )
    xSET_BIT( victim->affected_by, AFF_UNAMEK );
  if( xIS_SET( ch->affected_by, AFF_MICER ) )
    xSET_BIT( victim->affected_by, AFF_MICER );
  if( xIS_SET( ch->affected_by, AFF_DEMBRACE ) )
    xSET_BIT( victim->affected_by, AFF_DEMBRACE );
  if( xIS_SET( ch->affected_by, AFF_ARAPTURE ) )
    xSET_BIT( victim->affected_by, AFF_ARAPTURE );
  if( xIS_SET( ch->affected_by, AFF_SERALIGHT ) )
    xSET_BIT( victim->affected_by, AFF_SERALIGHT );
  if( xIS_SET( ch->affected_by, AFF_T1 ) )
    xSET_BIT( victim->affected_by, AFF_T1 );
  if( xIS_SET( ch->affected_by, AFF_T2 ) )
    xSET_BIT( victim->affected_by, AFF_T2 );
  if( xIS_SET( ch->affected_by, AFF_T3 ) )
    xSET_BIT( victim->affected_by, AFF_T3 );
  if( xIS_SET( ch->affected_by, AFF_T4 ) )
    xSET_BIT( victim->affected_by, AFF_T4 );
  if( xIS_SET( ch->affected_by, AFF_OMEGA ) )
    xSET_BIT( victim->affected_by, AFF_OMEGA );
  if( xIS_SET( ch->affected_by, AFF_VM2 ) )
    xSET_BIT( victim->affected_by, AFF_VM2 );
  if( xIS_SET( ch->affected_by, AFF_VM1 ) )
    xSET_BIT( victim->affected_by, AFF_VM1 );
  if( xIS_SET( ch->affected_by, AFF_HALO ) )
    xSET_BIT( victim->affected_by, AFF_HALO );
  if( xIS_SET( ch->affected_by, AFF_SUPER_OOZARU ) )
    xSET_BIT( victim->affected_by, AFF_SUPER_OOZARU );
  if( xIS_SET( ch->affected_by, AFF_SAIBAMAN1 ) )
    xSET_BIT( victim->affected_by, AFF_SAIBAMAN1 );
  if( xIS_SET( ch->affected_by, AFF_SAIBAMAN2 ) )
    xSET_BIT( victim->affected_by, AFF_SAIBAMAN2 );
  if( xIS_SET( ch->affected_by, AFF_SAIBAMAN3 ) )
    xSET_BIT( victim->affected_by, AFF_SAIBAMAN3 );
  if( xIS_SET( ch->affected_by, AFF_MUTATION1 ) )
    xSET_BIT( victim->affected_by, AFF_MUTATION1 );
  if( xIS_SET( ch->affected_by, AFF_MUTATION2 ) )
    xSET_BIT( victim->affected_by, AFF_MUTATION2 );
  if( xIS_SET( ch->affected_by, AFF_MUTATION3 ) )
    xSET_BIT( victim->affected_by, AFF_MUTATION3 );
  if( xIS_SET( ch->affected_by, AFF_MUTATION4 ) )
    xSET_BIT( victim->affected_by, AFF_MUTATION4 );
  if( xIS_SET( ch->affected_by, AFF_MUTATION5 ) )
    xSET_BIT( victim->affected_by, AFF_MUTATION5 );

  if( !botched )
  {
    sprintf( buf, "%s is standing here.", victim->short_descr );
    STRFREE( victim->long_descr );
    victim->long_descr = STRALLOC( buf );

    victim->perm_str = get_curr_str( ch );
    victim->perm_dex = get_curr_dex( ch );
    victim->perm_int = get_curr_int( ch );
    victim->perm_con = get_curr_con( ch );
    victim->perm_lck = get_curr_lck( ch );
    victim->sex = ch->sex;
    /*
     * victim->exp = ch->pl;
     */
    victim->alignment = ch->alignment;

    if( form_gsn == gsn_split_form )
    {
      victim->exp = ( ch->exp * 0.75 );
      victim->pl = ( ch->pl * 0.75 );
      victim->hit = 20;
      victim->max_hit = 20;
      victim->mana = ch->mana / 2;
      victim->max_mana = ch->max_mana / 2;
    }
    else if( form_gsn == gsn_tri_form )
    {
      victim->exp = ( ch->exp / 2 );
      victim->pl = ( ch->pl / 2 );
      victim->hit = 20;
      victim->max_hit = 20;
      victim->mana = ch->mana / 2;
      victim->max_mana = ch->max_mana / 2;
    }
    else if( form_gsn == gsn_multi_form )
    {
      victim->exp = ( ch->exp / 4 );
      victim->pl = ( ch->pl / 4 );
      victim->hit = 20;
      victim->max_hit = 20;
      victim->mana = ch->mana / 2;
      victim->max_mana = ch->max_mana / 2;
    }
    else if( form_gsn == gsn_clone )
    {
      if( amount < 5 )
        amount = 5;
      if( amount > 10 )
        amount = 10;
      victim->exp = ( ch->exp / amount );
      victim->pl = ( ch->pl / amount );
      victim->hit = 20;
      victim->max_hit = 20;
      victim->mana = ch->mana / 2;
      victim->max_mana = ch->max_mana / 2;
    }
  }
  else
  {
    sprintf( buf, "%sSomething doesn't look right with this creature.\n\r", victim->description );
    STRFREE( victim->description );
    victim->description = STRALLOC( buf );
    sprintf( buf, "%s is standing here.\n\r", victim->short_descr );
    STRFREE( victim->long_descr );
    victim->long_descr = STRALLOC( buf );

    botchMod = ( float )number_range( ch->pcdata->learned[form_gsn] / 2, ch->pcdata->learned[form_gsn] ) / 100;

    victim->perm_str = UMAX( 1, botchMod * get_curr_str( ch ) );
    victim->perm_dex = UMAX( 1, botchMod * get_curr_dex( ch ) );
    victim->perm_int = UMAX( 1, botchMod * get_curr_int( ch ) );
    victim->perm_con = UMAX( 1, botchMod * get_curr_con( ch ) );
    victim->perm_lck = UMAX( 1, botchMod * get_curr_lck( ch ) );
    victim->sex = ch->sex;
    /*
     * victim->exp = UMAX(1, botchMod * ch->pl);
     */
    victim->alignment = ch->alignment;

    if( form_gsn == gsn_split_form )
    {
      victim->exp = UMAX( 1, botchMod * ( ch->exp * 0.75 ) );
      victim->pl = UMAX( 1, botchMod * ( ch->pl * 0.75 ) );
      victim->hit = URANGE( 1, botchMod * 20, 20 );
      victim->max_hit = URANGE( 1, botchMod * 20, 20 );
      victim->mana = UMAX( 1, botchMod * ( ch->mana / 2 ) );
      victim->max_mana = UMAX( 1, botchMod * ( ch->max_mana / 2 ) );
    }
    else if( form_gsn == gsn_tri_form )
    {
      victim->exp = UMAX( 1, botchMod * ( ch->exp / 2 ) );
      victim->pl = UMAX( 1, botchMod * ( ch->pl / 2 ) );
      victim->hit = URANGE( 1, botchMod * 20, 20 );
      victim->max_hit = URANGE( 1, botchMod * 20, 20 );
      victim->mana = UMAX( 1, botchMod * ( ch->mana / 2 ) );
      victim->max_mana = UMAX( 1, botchMod * ( ch->max_mana / 2 ) );
    }
    else if( form_gsn == gsn_multi_form )
    {
      victim->exp = UMAX( 1, botchMod * ( ch->exp / 4 ) );
      victim->pl = UMAX( 1, botchMod * ( ch->pl / 4 ) );
      victim->hit = URANGE( 1, botchMod * 20, 20 );
      victim->max_hit = URANGE( 1, botchMod * 20, 20 );
      victim->mana = UMAX( 1, botchMod * ( ch->mana / 2 ) );
      victim->max_mana = UMAX( 1, botchMod * ( ch->max_mana / 2 ) );
    }
    else if( form_gsn == gsn_clone )
    {
      victim->exp = UMAX( 1, botchMod * ( ch->exp / amount ) );
      victim->pl = UMAX( 1, botchMod * ( ch->pl / amount ) );
      victim->hit = URANGE( 1, botchMod * 20, 20 );
      victim->max_hit = URANGE( 1, botchMod * 20, 20 );
      victim->mana = UMAX( 1, botchMod * ( ch->mana / 2 ) );
      victim->max_mana = UMAX( 1, botchMod * ( ch->max_mana / 2 ) );
    }
  }

  return;

}

/* Remove the stats placed on players from racial transformations */
void transStatRemove( CHAR_DATA * ch )
{
  AFFECT_DATA *tAff;
  AFFECT_DATA *tAffNext;

  if( !ch->first_affect )
    return;

  for( tAff = ch->first_affect; tAff; tAff = tAffNext )
  {
    tAffNext = tAff->next;
    if( tAff->affLocator == LOC_TRANS_STAT_APPLY )
      affect_remove( ch, tAff );
  }

  return;
}

/* Apply stats to characters who use racial transformations */
void transStatApply( CHAR_DATA * ch, int strMod, int spdMod, int intMod, int conMod )
{
  AFFECT_DATA affStr;
  AFFECT_DATA affSpd;
  AFFECT_DATA affInt;
  AFFECT_DATA affCon;

  transStatRemove( ch );

  if( strMod )
  {
    affStr.type = -1;
    affStr.duration = -1;
    affStr.location = APPLY_STR;
    affStr.modifier = strMod;
    xCLEAR_BITS( affStr.bitvector );
    affStr.affLocator = LOC_TRANS_STAT_APPLY;
    affect_to_char( ch, &affStr );
  }

  if( spdMod )
  {
    affSpd.type = -1;
    affSpd.duration = -1;
    affSpd.location = APPLY_DEX;
    affSpd.modifier = spdMod;
    xCLEAR_BITS( affSpd.bitvector );
    affSpd.affLocator = LOC_TRANS_STAT_APPLY;
    affect_to_char( ch, &affSpd );
  }

  if( intMod )
  {
    affInt.type = -1;
    affInt.duration = -1;
    affInt.location = APPLY_INT;
    affInt.modifier = intMod;
    xCLEAR_BITS( affInt.bitvector );
    affInt.affLocator = LOC_TRANS_STAT_APPLY;
    affect_to_char( ch, &affInt );
  }

  if( conMod )
  {
    affCon.type = -1;
    affCon.duration = -1;
    affCon.location = APPLY_CON;
    affCon.modifier = conMod;
    xCLEAR_BITS( affCon.bitvector );
    affCon.affLocator = LOC_TRANS_STAT_APPLY;
    affect_to_char( ch, &affCon );
  }

  return;
}

void rage( CHAR_DATA * ch, CHAR_DATA * victim )
{

  if( !ch->desc )
    return;

  if( !victim )
    return;
  if( IS_NPC( ch ) )
    return;
  if( !ch->in_room )
    return;
  if( !victim->in_room )
    return;

  if( ch->pcdata->learned[gsn_ssj] > 0 )
    return;

  if( ch->delay <= 0 )
  {
    if( !is_saiyan( ch ) && !is_hb( ch ) )
      return;

    if( ch->rage < 500 || ch->exp < 8000000 )
      return;

    if( ch->pcdata->learned[gsn_ssj] > 0 )
      return;

    switch ( number_range( 1, 5 ) )
    {
      default:
      case 1:
      case 2:
      case 3:
      case 4:
        break;
      case 5:
        if( number_percent(  ) > ( ( ch->rage - 400 ) / 50 ) )
        {
          switch ( number_range( 1, 10 ) )
          {
            default:
              break;
            case 1:
              act( AT_BLUE, "Your hair flashes blonde, but quickly returns to normal.", ch, NULL, victim, TO_CHAR );
              act( AT_BLUE, "$n's hair flashes blonde, but quickly returns to normal.", ch, NULL, victim, TO_VICT );
              act( AT_BLUE, "$n's hair flashes blonde, but quickly returns to normal.", ch, NULL, victim, TO_NOTVICT );
              break;
            case 2:
              act( AT_BLUE, "You scream out, in pure rage, your hatred for $N.", ch, NULL, victim, TO_CHAR );
              act( AT_BLUE, "$n screams out, in pure rage, $s hatred for you.", ch, NULL, victim, TO_VICT );
              act( AT_BLUE, "$n screams out, in pure rage, $s hatred for $N.", ch, NULL, victim, TO_NOTVICT );
              break;
            case 3:
              break;
            case 4:
              break;
            case 5:
              break;
            case 6:
              break;
            case 7:
              break;
            case 8:
              break;
            case 9:
              break;
            case 10:
              break;
          }
          return;
        }
        ch->delay_vict = victim;
        ch->delay = 6;
        break;
    }
  }

  switch ( ch->delay )
  {
    case 6:
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "You scream out, in pure rage, your hatred for $N.", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "$n screams out, in pure rage, $s hatred for you.", ch, NULL, victim, TO_VICT );
      act( AT_RED, "$n screams out, in pure rage, $s hatred for $N.", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 5:
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "Your hair flashes blonde, but quickly returns to normal.", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "$n's hair flashes blonde, but quickly returns to normal.", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "$n's hair flashes blonde, but quickly returns to normal.", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 4:
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "Your hair flashes blonde, but quickly returns to normal.", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "$n's hair flashes blonde, but quickly returns to normal.", ch, NULL, victim, TO_VICT );
      act( AT_RED, "$n's hair flashes blonde, but quickly returns to normal.", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 3:
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "Your hair flashes blonde, but slowly fades back to normal.", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "$n's hair flashes blonde, but slowly fades back to normal.", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "$n's hair flashes blonde, but slowly fades back to normal.", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 2:
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "Your voice bellows out through the area, with a deep angry howl.", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "$n's voice bellows out through the area, with a deep angry howl.", ch, NULL, victim, TO_VICT );
      act( AT_RED, "$n's voice bellows out through the area, with a deep angry howl.", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 1:
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "Your hair flashes blonde and your eyes turn blue as a fiery aura erupts around you.", ch, NULL,
           victim, TO_CHAR );
      act( AT_YELLOW, "You look up at $N, with rage filled eyes, ready to kill...", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "$n's hair flashes blonde and $s eyes turn blue as a fiery aura erupts around $m.", ch, NULL, victim,
           TO_VICT );
      act( AT_YELLOW, "$n looks up at you, with rage filled eyes, ready to kill...", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "$n's hair flashes blonde and $s eyes turn blue as a fiery aura erupts around $m.", ch, NULL, victim,
           TO_NOTVICT );
      act( AT_YELLOW, "$n looks up at $N, with rage filled eyes, ready to kill...", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );

      ch->pcdata->learned[gsn_ssj] = 10;
      if(can_give_light_effect(ch))
	ch->in_room->light--;
      if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_KAIOKEN );
      if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_OOZARU );
      xSET_BIT( ( ch )->affected_by, AFF_SSJ );
      ch->in_room->light++;
      ch->pl = ch->exp * 10;
      transStatApply( ch, 10, 5, 3, 5 );
      ch->rage = 0;
      ch->delay = 0;
      add_timer( ch, TIMER_ASUPRESSED, 6, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 6, NULL, 1 );
      break;
  }
  return;
}

void rage2( CHAR_DATA * ch, CHAR_DATA * victim )
{

  if( !ch->desc )
    return;

  if( !victim )
    return;
  if( IS_NPC( ch ) )
    return;
  if( !ch->in_room )
    return;
  if( !victim->in_room )
    return;

  if( ch->pcdata->learned[gsn_ssj2] > 0 && ch->pcdata->learned[gsn_ssj] <= 0 )
    return;

  if( !xIS_SET( ch->affected_by, AFF_SSJ ) )
    return;

  if( ch->delay <= 0 )
  {
    if( !is_saiyan( ch ) && !is_hb( ch ) )
      return;

    if( ch->rage < 800 || ch->exp < 50000000 )
      return;

    if( ch->pcdata->learned[gsn_ssj2] > 0 )
      return;

    switch ( number_range( 1, 5 ) )
    {
      default:
      case 1:
      case 2:
      case 3:
      case 4:
        break;
      case 5:
        if( number_percent(  ) > ( ( ch->rage - 800 ) / 50 ) )
        {
          switch ( number_range( 1, 10 ) )
          {
            default:
             break;
            case 1:
              act( AT_BLUE, "Electricity begins arcing around your body, but it quickly disappears.", ch, NULL, victim,
                   TO_CHAR );
              act( AT_BLUE, "Electricity begins arcing around $n's body, but it quickly disappears.", ch, NULL, victim,
                   TO_VICT );
              act( AT_BLUE, "Electricity begins arcing around $n's body, but it quickly disappears.", ch, NULL, victim,
                   TO_NOTVICT );
              break;
            case 2:
              act( AT_BLUE, "You scream out, in pure rage, your hatred for $N.", ch, NULL, victim, TO_CHAR );
              act( AT_BLUE, "$n screams out, in pure rage, $s hatred for you.", ch, NULL, victim, TO_VICT );
              act( AT_BLUE, "$n screams out, in pure rage, $s hatred for $N.", ch, NULL, victim, TO_NOTVICT );
              break;
            case 3:
              break;
            case 4:
              break;
            case 5:
              break;
            case 6:
              break;
            case 7:
              break;
            case 8:
              break;
            case 9:
              break;
            case 10:
              break;
          }
          return;
        }
        ch->delay_vict = victim;
        ch->delay = 6;
        break;
    }
  }

  switch ( ch->delay )
  {
    case 6:
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "You scream out, in pure rage, your hatred for $N.", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "$n screams out, in pure rage, $s hatred for you.", ch, NULL, victim, TO_VICT );
      act( AT_RED, "$n screams out, in pure rage, $s hatred for $N.", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 5:
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "Electricity begins arcing around your body.", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "Electricity begins arcing around $n's body.", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "Electricity begins arcing around $n's body.", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 4:
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "Electricity violently arcs around your body.", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "Electricity violently arcs around $n's body.", ch, NULL, victim, TO_VICT );
      act( AT_RED, "Electricity violently arcs around $n's body.", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 3:
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "Your hair flies back and begins to stand straight up.", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "$n's hair flies back and begins to stand straight up.", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "$n's hair flies back and begins to stand straight up.", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 2:
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "Your voice bellows out through the area, with a deep angry howl.", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "$n's voice bellows out through the area, with a deep angry howl.", ch, NULL, victim, TO_VICT );
      act( AT_RED, "$n's voice bellows out through the area, with a deep angry howl.", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 1:
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "Your hair flies back all the way and electricity arcs all around you as your aura flares brightly.",
           ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "$n's hair flies back all the way and electricity arcs all around $m as $s aura flares brightly.", ch,
           NULL, victim, TO_VICT );
      act( AT_YELLOW, "$n's hair flies back all the way and electricity arcs all around $m as $s aura flares brightly", ch,
           NULL, victim, TO_NOTVICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );

      ch->pcdata->learned[gsn_ssj2] = 10;
      if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_KAIOKEN );
      if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_OOZARU );
      if( !xIS_SET( ch->affected_by, AFF_SSJ ) )
        xSET_BIT( ( ch )->affected_by, AFF_SSJ );
      xSET_BIT( ( ch )->affected_by, AFF_SSJ2 );
      ch->pl = ch->exp * 16;
      transStatApply( ch, 16, 8, 4, 8 );
      ch->delay = 0;
      ch->rage = 0;
      add_timer( ch, TIMER_ASUPRESSED, 6, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 6, NULL, 1 );
      break;
  }
  return;
}

void rage3( CHAR_DATA * ch, CHAR_DATA * victim )
{

  if( !ch->desc )
    return;

  if( !victim )
    return;
  if( IS_NPC( ch ) )
    return;
  if( !ch->in_room )
    return;
  if( !victim->in_room )
    return;

  if( ch->pcdata->learned[gsn_ssj3] > 0 && ch->pcdata->learned[gsn_ssj] <= 0 && ch->pcdata->learned[gsn_ssj2] <= 0 )
    return;

  if( !xIS_SET( ch->affected_by, AFF_SSJ ) && !xIS_SET( ch->affected_by, AFF_SSJ2 ) )
    return;

  if( ch->delay <= 0 )
  {
    if( !is_saiyan( ch ) && !is_hb( ch ) )
      return;

    if( ch->rage < 1000 || ch->exp < 500000000 )
      return;

    if( ch->pcdata->learned[gsn_ssj3] > 0 )
      return;

    switch ( number_range( 1, 5 ) )
    {
      default:
      case 1:
      case 2:
      case 3:
      case 4:
        break;
      case 5:
        if( number_percent(  ) > ( ( ch->rage - 1000 ) / 50 ) )
        {
          return;
        }
        ch->delay_vict = victim;
        ch->delay = 6;
        break;
    }
  }

  switch ( ch->delay )
  {
    case 6:
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "You scream out, in pure rage, your hatred for $N.", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "$n screams out, in pure rage, $s hatred for you.", ch, NULL, victim, TO_VICT );
      act( AT_RED, "$n screams out, in pure rage, $s hatred for $N.", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 5:
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "Electricity begins arcing around your body.", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "Electricity begins arcing around $n's body.", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "Electricity begins arcing around $n's body.", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 4:
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "Electricity violently arcs around your body.", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "Electricity violently arcs around $n's body.", ch, NULL, victim, TO_VICT );
      act( AT_RED, "Electricity violently arcs around $n's body.", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 3:
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "Your hair begins to flow all the way down your back.", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "$n's hair begins to flow all the way down $s back.", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "$n's hair begins to flow all the way down $s back.", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 2:
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "Your voice bellows out through the area, with a deep angry howl.", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "$n's voice bellows out through the area, with a deep angry howl.", ch, NULL, victim, TO_VICT );
      act( AT_RED, "$n's voice bellows out through the area, with a deep angry howl.", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 1:
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "Your eyebrows disappear and your aura flares even brighter.", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "$n's eyebrows disappear and $s aura flares even brighter.", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "$n's eyebrows disappear and $s aura flares even brighter.", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );

      ch->pcdata->learned[gsn_ssj3] = 10;
      if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_KAIOKEN );
      if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_OOZARU );
      if( !xIS_SET( ch->affected_by, AFF_SSJ ) )
        xSET_BIT( ( ch )->affected_by, AFF_SSJ );
      if( !xIS_SET( ch->affected_by, AFF_SSJ2 ) )
        xSET_BIT( ( ch )->affected_by, AFF_SSJ2 );
      xSET_BIT( ( ch )->affected_by, AFF_SSJ3 );
      ch->pl = ch->exp * 24;
      transStatApply( ch, 24, 12, 6, 12 );
      ch->delay = 0;
      ch->rage = 0;
      add_timer( ch, TIMER_ASUPRESSED, 6, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 6, NULL, 1 );
      break;
  }
  return;
}

void rage4( CHAR_DATA * ch, CHAR_DATA * victim )
{

  if( !ch->desc )
    return;

  if( !victim )
    return;
  if( IS_NPC( ch ) )
    return;
  if( !ch->in_room )
    return;
  if( !victim->in_room )
    return;

  if( ch->pcdata->learned[gsn_ssj4] > 0 && ch->pcdata->learned[gsn_ssj] <= 0
      && ch->pcdata->learned[gsn_ssj2] <= 0 && ch->pcdata->learned[gsn_ssj3] <= 0 )
    return;

  if( !xIS_SET( ch->affected_by, AFF_GOLDEN_OOZARU ) )
    return;

  /*
   * if (!xIS_SET(ch->affected_by, AFF_SSJ) && !xIS_SET(ch->affected_by, AFF_SSJ2)
   * && !xIS_SET(ch->affected_by, AFF_SSJ3))
   * return;
   */

  if( ch->delay <= 0 )
  {
    if( !is_saiyan( ch ) )
      return;

    if( ch->rage < 1500 || ch->exp < 2000000000 )
      return;

    if( ch->pcdata->learned[gsn_ssj4] > 0 )
      return;

    switch ( number_range( 1, 5 ) )
    {
      default:
      case 1:
      case 2:
      case 3:
      case 4:
        break;
      case 5:
        if( number_percent(  ) > ( ( ch->rage - 1500 ) / 50 ) )
        {
          return;
        }
        ch->delay_vict = victim;
        ch->delay = 6;
        break;
    }
  }

  switch ( ch->delay )
  {
    case 6:
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "You scream out, in pure rage, your hatred for $N.", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "$n screams out, in pure rage, $s hatred for you.", ch, NULL, victim, TO_VICT );
      act( AT_RED, "$n screams out, in pure rage, $s hatred for $N.", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 5:
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "Electricity violently arcs around your body.", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "Electricity violently arcs around $n's body.", ch, NULL, victim, TO_VICT );
      act( AT_RED, "Electricity violently arcs around $n's body.", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 4:
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "Red fur begins to sprout all over your body.", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "Red fur begins to sprout all over $n's body.", ch, NULL, victim, TO_VICT );
      act( AT_RED, "Red fur begins to sprout all over $n's body.", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 3:
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "Your hair flashes black.", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "$n's hair flashes black.", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "$n's hair flashes black.", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 2:
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "Your voice bellows out through the area, with a deep angry howl.", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "$n's voice bellows out through the area, with a deep angry howl.", ch, NULL, victim, TO_VICT );
      act( AT_RED, "$n's voice bellows out through the area, with a deep angry howl.", ch, NULL, victim, TO_NOTVICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_VICT );
      act( AT_YELLOW, "=-", ch, NULL, victim, TO_NOTVICT );
      ch->delay -= 1;
      add_timer( ch, TIMER_DELAY, 4, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 4, NULL, 1 );
      break;
    case 1:
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "You howl with anger as your hair turns black again and grow", ch, NULL, NULL, TO_CHAR );
      act( AT_RED, "thick red fur all over your body, finally you sprout a", ch, NULL, NULL, TO_CHAR );
      act( AT_RED, "tail and look up with glaring eyes inset within a red outline.", ch, NULL, NULL, TO_CHAR );
      act( AT_RED, "$n howls with anger as $s hair turns black again and grows", ch, NULL, NULL, TO_NOTVICT );
      act( AT_RED, "thick red fur all over $s body, finally $e sprouts a", ch, NULL, NULL, TO_NOTVICT );
      act( AT_RED, "tail and looks up at you with glaring eyes inset within a red outline.", ch, NULL, NULL, TO_NOTVICT );
      act( AT_RED, "You look up at $N, with rage filled eyes, ready to kill...", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "$n looks up at you, with rage filled eyes, ready to kill...", ch, NULL, victim, TO_VICT );
      act( AT_RED, "$n looks up at $N, with rage filled eyes, ready to kill...", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "=-", ch, NULL, victim, TO_VICT );
      act( AT_RED, "=-", ch, NULL, victim, TO_NOTVICT );

      ch->pcdata->learned[gsn_ssj4] = 10;
      if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_KAIOKEN );
      if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_OOZARU );
      if( xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_GOLDEN_OOZARU );
      if( !xIS_SET( ch->affected_by, AFF_SSJ ) )
        xSET_BIT( ( ch )->affected_by, AFF_SSJ );
      if( !xIS_SET( ch->affected_by, AFF_SSJ2 ) )
        xSET_BIT( ( ch )->affected_by, AFF_SSJ2 );
      if( !xIS_SET( ch->affected_by, AFF_SSJ3 ) )
        xSET_BIT( ( ch )->affected_by, AFF_SSJ3 );
      xSET_BIT( ( ch )->affected_by, AFF_SSJ4 );
      ch->pl = ch->exp * 30;
      transStatApply( ch, 30, 15, 8, 15 );
      ch->delay = 0;
      ch->rage = 0;
      add_timer( ch, TIMER_ASUPRESSED, 8, NULL, 1 );
      add_timer( victim, TIMER_ASUPRESSED, 8, NULL, 1 );
      break;
  }
  return;
}

void heart_calc( CHAR_DATA * ch, char *argument )
{
  double pl_mult = 1;
  double pl_mult2 = 1;
  int con = 0;
  double chk = 0;

  /*
   * if (ch->race != 1 || IS_NPC(ch))
   * return;
   */
  if( !is_human( ch ) || IS_NPC( ch ) )
    return;

  if( ch->exp > ch->pl )
    return;

  if( !IS_SET( ch->pcdata->combatFlags, CMB_NO_HEART ) )
    return;

  if( !xIS_SET( ch->affected_by, AFF_HEART ) )
    ch->heart_pl = ch->pl;

  con = ( get_curr_con( ch ) ) - 10;

  if( con < 1 )
  {
    if( xIS_SET( ch->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    ch->pl = ch->heart_pl;
    return;
  }

  if( ch->hit >= 90 || ch->hit <= 0 )
  {
    if( xIS_SET( ch->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    ch->pl = ch->heart_pl;
    return;
  }

  pl_mult = con / 10;

  chk = ( double )( 18 - ( ch->hit / 5 ) ) / 18 * pl_mult;

  if( chk <= 1 )
  {
    if( xIS_SET( ch->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    ch->pl = ch->heart_pl;
    return;
  }

  if( !xIS_SET( ch->affected_by, AFF_HEART ) )
  {
    xSET_BIT( ch->affected_by, AFF_HEART );
  }

  pl_mult2 = ( double )ch->heart_pl / ch->exp;
  chk = ( double )pl_mult2 + chk;

  ch->pl = ch->exp * chk;

  act( AT_RED, "", ch, NULL, NULL, TO_CHAR );
  act( AT_RED, "", ch, NULL, NULL, TO_NOTVICT );


  return;
}


void do_powerdown( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *och;
  CHAR_DATA *och_next;

  if( IS_NPC( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( IS_AFFECTED( ch, AFF_BIOJR ) && IS_NPC( ch ) )
    return;

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }
  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if(can_give_light_effect(ch)) /* remove their lighting effect if they had one */
    ch->in_room->light--;
 
  if( ch->pl <= ch->exp )
  {
    send_to_char( "You are already powered down.\n\r", ch );
    ch->powerup = 0;
    if( xIS_SET( ( ch )->affected_by, AFF_POWERUPS ) )
    {
      xREMOVE_BIT( ( ch )->affected_by, AFF_POWERUPS );
    }
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ ) )
    {
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ );
      if( !IS_NPC( ch ) )
      {
        ch->pcdata->haircolor = ch->pcdata->orignalhaircolor;
        ch->pcdata->eyes = ch->pcdata->orignaleyes;
      }
    }
   if( xIS_SET( ( ch )->affected_by, AFF_UNAMEK ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_UNAMEK );
   if( xIS_SET( ( ch )->affected_by, AFF_MICER ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_MICER );
    if( xIS_SET( ( ch )->affected_by, AFF_USSJ ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_USSJ );
    if( xIS_SET( ( ch )->affected_by, AFF_USSJ2 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_USSJ2 );
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ2 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ2 );
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ3 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ3 );
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ4 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ4 );
    if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_KAIOKEN );
    if( xIS_SET( ( ch )->affected_by, AFF_SNAMEK ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_SNAMEK );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER2 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER2 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER3 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER3 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER4 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER4 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER5 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER5 );
    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    if( xIS_SET( ( ch )->affected_by, AFF_HYPER ) )
      xREMOVE_BIT( ch->affected_by, AFF_HYPER );
    if( xIS_SET( ( ch )->affected_by, AFF_EXTREME ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_EXTREME );
    if( xIS_SET( ( ch )->affected_by, AFF_SEMIPERFECT ) )
      xREMOVE_BIT( ch->affected_by, AFF_SEMIPERFECT );
    if( xIS_SET( ( ch )->affected_by, AFF_PERFECT ) )
      xREMOVE_BIT( ch->affected_by, AFF_PERFECT );
    if( xIS_SET( ( ch )->affected_by, AFF_ULTRAPERFECT ) )
      xREMOVE_BIT( ch->affected_by, AFF_ULTRAPERFECT );
    if( xIS_SET( ( ch )->affected_by, AFF_GROWTH ) )
      xREMOVE_BIT( ch->affected_by, AFF_GROWTH );
    if( xIS_SET( ( ch )->affected_by, AFF_GIANT ) )
      xREMOVE_BIT( ch->affected_by, AFF_GIANT );
    if( xIS_SET( ( ch )->affected_by, AFF_EVIL_TRANS ) )
      xREMOVE_BIT( ch->affected_by, AFF_EVIL_TRANS );
    if( xIS_SET( ( ch )->affected_by, AFF_SUPER_TRANS ) )
      xREMOVE_BIT( ch->affected_by, AFF_SUPER_TRANS );
    if( xIS_SET( ( ch )->affected_by, AFF_KID_TRANS ) )
      xREMOVE_BIT( ch->affected_by, AFF_KID_TRANS );
    if( xIS_SET( ( ch )->affected_by, AFF_MYSTIC ) )
      xREMOVE_BIT( ch->affected_by, AFF_MYSTIC );
    if( xIS_SET( ( ch )->affected_by, AFF_LSSJ ) )
      xREMOVE_BIT( ch->affected_by, AFF_LSSJ );
    if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
      xREMOVE_BIT( ch->affected_by, AFF_MAJIN );
    if( xIS_SET( ( ch )->affected_by, AFF_SIGMA_VIRUS ) )
      xREMOVE_BIT( ch->affected_by, AFF_SIGMA_VIRUS );
    if( xIS_SET( ( ch )->affected_by, AFF_SENT_CHIP ) )
      xREMOVE_BIT( ch->affected_by, AFF_SENT_CHIP );
    if( xIS_SET( ( ch )->affected_by, AFF_SUPERANDROID ) )
      xREMOVE_BIT( ch->affected_by, AFF_SUPERANDROID );
    if( xIS_SET( ( ch )->affected_by, AFF_EVILBOOST ) )
      xREMOVE_BIT( ch->affected_by, AFF_EVILBOOST );
    if( xIS_SET( ( ch )->affected_by, AFF_EVILSURGE ) )
      xREMOVE_BIT( ch->affected_by, AFF_EVILSURGE );
    if( xIS_SET( ( ch )->affected_by, AFF_EVILOVERLOAD ) )
      xREMOVE_BIT( ch->affected_by, AFF_EVILOVERLOAD );
    if( xIS_SET( ( ch )->affected_by, AFF_T1 ) )
      xREMOVE_BIT( ch->affected_by, AFF_T1 );
    if( xIS_SET( ( ch )->affected_by, AFF_T2 ) )
      xREMOVE_BIT( ch->affected_by, AFF_T2 );
    if( xIS_SET( ( ch )->affected_by, AFF_T3 ) )
      xREMOVE_BIT( ch->affected_by, AFF_T3 );
    if( xIS_SET( ( ch )->affected_by, AFF_T4 ) )
      xREMOVE_BIT( ch->affected_by, AFF_T4 );
    if( xIS_SET( ( ch )->affected_by, AFF_OMEGA ) )
      xREMOVE_BIT( ch->affected_by, AFF_OMEGA );
    if( xIS_SET( ( ch )->affected_by, AFF_VM1 ) )
      xREMOVE_BIT( ch->affected_by, AFF_VM1 );
    if( xIS_SET( ( ch )->affected_by, AFF_VM2 ) )
      xREMOVE_BIT( ch->affected_by, AFF_VM2 );
    if( xIS_SET( ( ch )->affected_by, AFF_ESSJ ) )
      xREMOVE_BIT( ch->affected_by, AFF_ESSJ );
    if( xIS_SET( ( ch )->affected_by, AFF_HSSJ ) )
      xREMOVE_BIT( ch->affected_by, AFF_HSSJ );
    if( xIS_SET( ( ch )->affected_by, AFF_SUPER_OOZARU ) )
      xREMOVE_BIT( ch->affected_by, AFF_SUPER_OOZARU );
    if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN1 ) )
      xREMOVE_BIT( ch->affected_by, AFF_SAIBAMAN1 );
    if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN2 ) )
      xREMOVE_BIT( ch->affected_by, AFF_SAIBAMAN2 );
    if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN3 ) )
      xREMOVE_BIT( ch->affected_by, AFF_SAIBAMAN3 );
    if( xIS_SET( ( ch )->affected_by, AFF_PUPPET ) )
      xREMOVE_BIT( ch->affected_by, AFF_PUPPET );
    if( xIS_SET( ( ch )->affected_by, AFF_SFLAME1 ) )
      xREMOVE_BIT( ch->affected_by, AFF_SFLAME1 );
    if( xIS_SET( ( ch )->affected_by, AFF_SFLAME2 ) )
      xREMOVE_BIT( ch->affected_by, AFF_SFLAME2 );
    if( xIS_SET( ( ch )->affected_by, AFF_SFLAME3 ) )
      xREMOVE_BIT( ch->affected_by, AFF_SFLAME3 );
    if( xIS_SET( ( ch )->affected_by, AFF_UFLAME ) )
      xREMOVE_BIT( ch->affected_by, AFF_UFLAME );
    if( xIS_SET( ( ch )->affected_by, AFF_ADULT ) )
      xREMOVE_BIT( ch->affected_by, AFF_ADULT );
    if( xIS_SET( ( ch )->affected_by, AFF_DEKU ) )
      xREMOVE_BIT( ch->affected_by, AFF_DEKU );
    if( xIS_SET( ( ch )->affected_by, AFF_GORON ) )
      xREMOVE_BIT( ch->affected_by, AFF_GORON );
    if( xIS_SET( ( ch )->affected_by, AFF_ZORA ) )
      xREMOVE_BIT( ch->affected_by, AFF_ZORA );	
    if( xIS_SET( ( ch )->affected_by, AFF_FDEITY ) )
      xREMOVE_BIT( ch->affected_by, AFF_FDEITY );
    if( xIS_SET( ( ch )->affected_by, AFF_ACTIVE_FORM ) )
      xREMOVE_BIT( ch->affected_by, AFF_ACTIVE_FORM );
    if( xIS_SET( ( ch )->affected_by, AFF_POWER_FORM ) )
      xREMOVE_BIT( ch->affected_by, AFF_POWER_FORM );
    if( xIS_SET( ( ch )->affected_by, AFF_PROTO_FORM ) )
      xREMOVE_BIT( ch->affected_by, AFF_PROTO_FORM );	  
    if( xIS_SET( ( ch )->affected_by, AFF_OMEGA_FORM ) )
      xREMOVE_BIT( ch->affected_by, AFF_OMEGA_FORM );
    if( xIS_SET( ( ch )->affected_by, AFF_INNERWILL ) )
      xREMOVE_BIT( ch->affected_by, AFF_INNERWILL );
    if( xIS_SET( ( ch )->affected_by, AFF_MECHAEVO ) )
      xREMOVE_BIT( ch->affected_by, AFF_MECHAEVO );
    if( xIS_SET( ( ch )->affected_by, AFF_ANCIENTEVO ) )
      xREMOVE_BIT( ch->affected_by, AFF_ANCIENTEVO );
    if( xIS_SET( ( ch )->affected_by, AFF_POWERUPS ) )
      xREMOVE_BIT( ch->affected_by, AFF_POWERUPS );
    if( xIS_SET( ( ch )->affected_by, AFF_MUTATION1 ) )
      xREMOVE_BIT( ch->affected_by, AFF_MUTATION1 );
    if( xIS_SET( ( ch )->affected_by, AFF_MUTATION2 ) )
      xREMOVE_BIT( ch->affected_by, AFF_MUTATION2 );
    if( xIS_SET( ( ch )->affected_by, AFF_MUTATION3 ) )
      xREMOVE_BIT( ch->affected_by, AFF_MUTATION3 );
    if( xIS_SET( ( ch )->affected_by, AFF_MUTATION4 ) )
      xREMOVE_BIT( ch->affected_by, AFF_MUTATION4 );
    if( xIS_SET( ( ch )->affected_by, AFF_MUTATION5 ) )
      xREMOVE_BIT( ch->affected_by, AFF_MUTATION5 );
    transStatRemove( ch );
    heart_calc( ch, "" );
    if( is_splitformed( ch ) )
    {
      for( och = first_char; och; och = och_next )
      {
        och_next = och->next;

        if( !IS_NPC( och ) )
          continue;

        if( is_split( och ) && och->master == ch )
          do_powerdown( och, "" );
      }
    }
    return;
  }
  if( ch->exp != ch->pl && xIS_SET( ch->affected_by, AFF_OOZARU ) )
  {
    send_to_char( "You can't while you are an Oozaru.\n\r", ch );
    return;
  }
  if( ch->exp != ch->pl && xIS_SET( ch->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    send_to_char( "You can't while you are a Golden Oozaru.\n\r", ch );
    return;
  }
  if( ch->exp != ch->pl && xIS_SET( ch->affected_by, AFF_MAKEOSTAR ) )
  {
    send_to_char( "You can't while you are being affected by the Makeo Star.\n\r", ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_SSJ )
      || xIS_SET( ( ch )->affected_by, AFF_USSJ )
      || xIS_SET( ( ch )->affected_by, AFF_USSJ2 )
      || xIS_SET( ( ch )->affected_by, AFF_SSJ2 )
      || xIS_SET( ( ch )->affected_by, AFF_SSJ3 )
      || xIS_SET( ( ch )->affected_by, AFF_SSJ4 )
      || xIS_SET( ( ch )->affected_by, AFF_KAIOKEN )
      || xIS_SET( ( ch )->affected_by, AFF_HYPER )
      || xIS_SET( ( ch )->affected_by, AFF_EXTREME )
      || xIS_SET( ( ch )->affected_by, AFF_SNAMEK )
      || xIS_SET( ( ch )->affected_by, AFF_ICER2 )
      || xIS_SET( ( ch )->affected_by, AFF_ICER3 )
      || xIS_SET( ( ch )->affected_by, AFF_ICER4 )
      || xIS_SET( ( ch )->affected_by, AFF_ICER5 )
      || xIS_SET( ( ch )->affected_by, AFF_SEMIPERFECT )
      || xIS_SET( ( ch )->affected_by, AFF_PERFECT )
      || xIS_SET( ( ch )->affected_by, AFF_ULTRAPERFECT )
      || xIS_SET( ( ch )->affected_by, AFF_GROWTH )
      || xIS_SET( ( ch )->affected_by, AFF_GIANT )
      || xIS_SET( ( ch )->affected_by, AFF_EVIL_TRANS )
      || xIS_SET( ( ch )->affected_by, AFF_SUPER_TRANS )
      || xIS_SET( ( ch )->affected_by, AFF_KID_TRANS )
      || xIS_SET( ( ch )->affected_by, AFF_MYSTIC )
      || xIS_SET( ( ch )->affected_by, AFF_LSSJ )
      || xIS_SET( ( ch )->affected_by, AFF_MAJIN )
      || xIS_SET( ( ch )->affected_by, AFF_SIGMA_VIRUS )
      || xIS_SET( ( ch )->affected_by, AFF_SENT_CHIP )
      || xIS_SET( ( ch )->affected_by, AFF_SUPERANDROID )
      || xIS_SET( ( ch )->affected_by, AFF_EVILBOOST )
      || xIS_SET( ( ch )->affected_by, AFF_EVILSURGE )
      || xIS_SET( ( ch )->affected_by, AFF_EVILOVERLOAD )
      || xIS_SET( ( ch )->affected_by, AFF_UNAMEK )
      || xIS_SET( ( ch )->affected_by, AFF_MICER )
      || xIS_SET( ( ch )->affected_by, AFF_T1 )
      || xIS_SET( ( ch )->affected_by, AFF_T2 )
      || xIS_SET( ( ch )->affected_by, AFF_T3 )
      || xIS_SET( ( ch )->affected_by, AFF_T4 )
      || xIS_SET( ( ch )->affected_by, AFF_OMEGA )
      || xIS_SET( ( ch )->affected_by, AFF_VM1 )
      || xIS_SET( ( ch )->affected_by, AFF_VM2 )
      || xIS_SET( ( ch )->affected_by, AFF_HSSJ )
      || xIS_SET( ( ch )->affected_by, AFF_ESSJ )
      || xIS_SET( ( ch )->affected_by, AFF_SUPER_OOZARU )
      || xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN1 )
      || xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN2 ) 
	  || xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN3 )
	  || xIS_SET( ( ch )->affected_by, AFF_PUPPET )
	  || xIS_SET( ( ch )->affected_by, AFF_SFLAME1 )
	  || xIS_SET( ( ch )->affected_by, AFF_SFLAME2 )
	  || xIS_SET( ( ch )->affected_by, AFF_SFLAME3 )
	  || xIS_SET( ( ch )->affected_by, AFF_UFLAME )
	  || xIS_SET( ( ch )->affected_by, AFF_ADULT )
	  || xIS_SET( ( ch )->affected_by, AFF_GORON )
	  || xIS_SET( ( ch )->affected_by, AFF_DEKU )
	  || xIS_SET( ( ch )->affected_by, AFF_ZORA )
	  || xIS_SET( ( ch )->affected_by, AFF_FDEITY )
	  || xIS_SET( ( ch )->affected_by, AFF_ACTIVE_FORM )
	  || xIS_SET( ( ch )->affected_by, AFF_POWER_FORM )
	  || xIS_SET( ( ch )->affected_by, AFF_PROTO_FORM ) 
	  || xIS_SET( ( ch )->affected_by, AFF_OMEGA_FORM )
	  || xIS_SET( ( ch )->affected_by, AFF_INNERWILL )
	  || xIS_SET( ( ch )->affected_by, AFF_MECHAEVO )
	  || xIS_SET( ( ch )->affected_by, AFF_ANCIENTEVO )
	  || xIS_SET( ( ch )->affected_by, AFF_POWERUPS )
	  || xIS_SET( ( ch )->affected_by, AFF_MUTATION1 )
	  || xIS_SET( ( ch )->affected_by, AFF_MUTATION2 )
	  || xIS_SET( ( ch )->affected_by, AFF_MUTATION3 )
	  || xIS_SET( ( ch )->affected_by, AFF_MUTATION4 )
	  || xIS_SET( ( ch )->affected_by, AFF_MUTATION5 ) )
  {
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ ) )
    {
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ );
      if( !IS_NPC( ch ) )
      {
        ch->pcdata->haircolor = ch->pcdata->orignalhaircolor;
        ch->pcdata->eyes = ch->pcdata->orignaleyes;
      }
    }
    if( xIS_SET( ( ch )->affected_by, AFF_USSJ ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_USSJ );
    if( xIS_SET( ( ch )->affected_by, AFF_USSJ2 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_USSJ2 );
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ2 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ2 );
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ3 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ3 );
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ4 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ4 );
    if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_KAIOKEN );
    if( xIS_SET( ( ch )->affected_by, AFF_SNAMEK ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_SNAMEK );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER2 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER2 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER3 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER3 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER4 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER4 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER5 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER5 );
    if( xIS_SET( ( ch )->affected_by, AFF_HYPER ) )
      xREMOVE_BIT( ch->affected_by, AFF_HYPER );
    if( xIS_SET( ( ch )->affected_by, AFF_EXTREME ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_EXTREME );
    if( xIS_SET( ( ch )->affected_by, AFF_SEMIPERFECT ) )
      xREMOVE_BIT( ch->affected_by, AFF_SEMIPERFECT );
    if( xIS_SET( ( ch )->affected_by, AFF_PERFECT ) )
      xREMOVE_BIT( ch->affected_by, AFF_PERFECT );
    if( xIS_SET( ( ch )->affected_by, AFF_ULTRAPERFECT ) )
      xREMOVE_BIT( ch->affected_by, AFF_ULTRAPERFECT );
    if( xIS_SET( ( ch )->affected_by, AFF_GROWTH ) )
      xREMOVE_BIT( ch->affected_by, AFF_GROWTH );
    if( xIS_SET( ( ch )->affected_by, AFF_GIANT ) )
      xREMOVE_BIT( ch->affected_by, AFF_GIANT );
    if( xIS_SET( ( ch )->affected_by, AFF_EVIL_TRANS ) )
      xREMOVE_BIT( ch->affected_by, AFF_EVIL_TRANS );
    if( xIS_SET( ( ch )->affected_by, AFF_SUPER_TRANS ) )
      xREMOVE_BIT( ch->affected_by, AFF_SUPER_TRANS );
    if( xIS_SET( ( ch )->affected_by, AFF_KID_TRANS ) )
      xREMOVE_BIT( ch->affected_by, AFF_KID_TRANS );
    if( xIS_SET( ( ch )->affected_by, AFF_MYSTIC ) )
      xREMOVE_BIT( ch->affected_by, AFF_MYSTIC );
    if( xIS_SET( ( ch )->affected_by, AFF_LSSJ ) )
      xREMOVE_BIT( ch->affected_by, AFF_LSSJ );
    if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
      xREMOVE_BIT( ch->affected_by, AFF_MAJIN );
    if( xIS_SET( ( ch )->affected_by, AFF_SIGMA_VIRUS ) )
      xREMOVE_BIT( ch->affected_by, AFF_SIGMA_VIRUS );
    if( xIS_SET( ( ch )->affected_by, AFF_SENT_CHIP ) )
      xREMOVE_BIT( ch->affected_by, AFF_SENT_CHIP );
    if( xIS_SET( ( ch )->affected_by, AFF_SUPERANDROID ) )
      xREMOVE_BIT( ch->affected_by, AFF_SUPERANDROID );
    if( xIS_SET( ( ch )->affected_by, AFF_EVILBOOST ) )
      xREMOVE_BIT( ch->affected_by, AFF_EVILBOOST );
    if( xIS_SET( ( ch )->affected_by, AFF_EVILSURGE ) )
      xREMOVE_BIT( ch->affected_by, AFF_EVILSURGE );
    if( xIS_SET( ( ch )->affected_by, AFF_EVILOVERLOAD ) )
      xREMOVE_BIT( ch->affected_by, AFF_EVILOVERLOAD );
    if( xIS_SET( ( ch )->affected_by, AFF_UNAMEK ) )
      xREMOVE_BIT( ch->affected_by, AFF_UNAMEK );
    if( xIS_SET( ( ch )->affected_by, AFF_MICER ) )
      xREMOVE_BIT( ch->affected_by, AFF_MICER );
    if( xIS_SET( ( ch )->affected_by, AFF_DEMBRACE ) )
      xREMOVE_BIT( ch->affected_by, AFF_DEMBRACE );
    if( xIS_SET( ( ch )->affected_by, AFF_ARAPTURE ) )
      xREMOVE_BIT( ch->affected_by, AFF_ARAPTURE );
    if( xIS_SET( ( ch )->affected_by, AFF_SERALIGHT ) )
      xREMOVE_BIT( ch->affected_by, AFF_SERALIGHT );
    if( xIS_SET( ( ch )->affected_by, AFF_T1 ) )
      xREMOVE_BIT( ch->affected_by, AFF_T1 );
    if( xIS_SET( ( ch )->affected_by, AFF_SUPER_OOZARU ) )
      xREMOVE_BIT( ch->affected_by, AFF_SUPER_OOZARU );
    if( xIS_SET( ( ch )->affected_by, AFF_T2 ) )
      xREMOVE_BIT( ch->affected_by, AFF_T2 );
    if( xIS_SET( ( ch )->affected_by, AFF_T3 ) )
      xREMOVE_BIT( ch->affected_by, AFF_T3 );
    if( xIS_SET( ( ch )->affected_by, AFF_T4 ) )
      xREMOVE_BIT( ch->affected_by, AFF_T4 );
    if( xIS_SET( ( ch )->affected_by, AFF_OMEGA ) )
      xREMOVE_BIT( ch->affected_by, AFF_OMEGA );
    if( xIS_SET( ( ch )->affected_by, AFF_VM1 ) )
      xREMOVE_BIT( ch->affected_by, AFF_VM1 );
    if( xIS_SET( ( ch )->affected_by, AFF_VM2 ) )
      xREMOVE_BIT( ch->affected_by, AFF_VM2 );
    if( xIS_SET( ( ch )->affected_by, AFF_HSSJ ) )
      xREMOVE_BIT( ch->affected_by, AFF_HSSJ );
    if( xIS_SET( ( ch )->affected_by, AFF_ESSJ ) )
      xREMOVE_BIT( ch->affected_by, AFF_ESSJ );
    if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN1 ) )
      xREMOVE_BIT( ch->affected_by, AFF_SAIBAMAN1 );
    if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN2 ) )
      xREMOVE_BIT( ch->affected_by, AFF_SAIBAMAN2 );
    if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN3 ) )
      xREMOVE_BIT( ch->affected_by, AFF_SAIBAMAN3 );
    if( xIS_SET( ( ch )->affected_by, AFF_PUPPET ) )
      xREMOVE_BIT( ch->affected_by, AFF_PUPPET );
    if( xIS_SET( ( ch )->affected_by, AFF_SFLAME1 ) )
      xREMOVE_BIT( ch->affected_by, AFF_SFLAME1 );
    if( xIS_SET( ( ch )->affected_by, AFF_SFLAME2 ) )
      xREMOVE_BIT( ch->affected_by, AFF_SFLAME2 );	
    if( xIS_SET( ( ch )->affected_by, AFF_SFLAME3 ) )
      xREMOVE_BIT( ch->affected_by, AFF_SFLAME3 );
    if( xIS_SET( ( ch )->affected_by, AFF_UFLAME ) )
      xREMOVE_BIT( ch->affected_by, AFF_UFLAME );
    if( xIS_SET( ( ch )->affected_by, AFF_ADULT ) )
      xREMOVE_BIT( ch->affected_by, AFF_ADULT );
    if( xIS_SET( ( ch )->affected_by, AFF_DEKU ) )
      xREMOVE_BIT( ch->affected_by, AFF_DEKU );
    if( xIS_SET( ( ch )->affected_by, AFF_GORON ) )
      xREMOVE_BIT( ch->affected_by, AFF_GORON );
    if( xIS_SET( ( ch )->affected_by, AFF_ZORA ) )
      xREMOVE_BIT( ch->affected_by, AFF_ZORA );	
    if( xIS_SET( ( ch )->affected_by, AFF_FDEITY ) )
      xREMOVE_BIT( ch->affected_by, AFF_FDEITY );
    if( xIS_SET( ( ch )->affected_by, AFF_ACTIVE_FORM ) )
      xREMOVE_BIT( ch->affected_by, AFF_ACTIVE_FORM );	  
    if( xIS_SET( ( ch )->affected_by, AFF_POWER_FORM ) )
      xREMOVE_BIT( ch->affected_by, AFF_POWER_FORM );	
    if( xIS_SET( ( ch )->affected_by, AFF_PROTO_FORM ) )
      xREMOVE_BIT( ch->affected_by, AFF_PROTO_FORM );
    if( xIS_SET( ( ch )->affected_by, AFF_OMEGA_FORM ) )
      xREMOVE_BIT( ch->affected_by, AFF_OMEGA_FORM );	
    if( xIS_SET( ( ch )->affected_by, AFF_INNERWILL ) )
      xREMOVE_BIT( ch->affected_by, AFF_INNERWILL );
    if( xIS_SET( ( ch )->affected_by, AFF_MECHAEVO ) )
      xREMOVE_BIT( ch->affected_by, AFF_MECHAEVO );
    if( xIS_SET( ( ch )->affected_by, AFF_ANCIENTEVO ) )
      xREMOVE_BIT( ch->affected_by, AFF_ANCIENTEVO );
    if( xIS_SET( ( ch )->affected_by, AFF_POWERUPS ) )
      xREMOVE_BIT( ch->affected_by, AFF_POWERUPS );
    if( xIS_SET( ( ch )->affected_by, AFF_MUTATION1 ) )
      xREMOVE_BIT( ch->affected_by, AFF_MUTATION1 );
    if( xIS_SET( ( ch )->affected_by, AFF_MUTATION2 ) )
      xREMOVE_BIT( ch->affected_by, AFF_MUTATION2 );
    if( xIS_SET( ( ch )->affected_by, AFF_MUTATION3 ) )
      xREMOVE_BIT( ch->affected_by, AFF_MUTATION3 );
    if( xIS_SET( ( ch )->affected_by, AFF_MUTATION4 ) )
      xREMOVE_BIT( ch->affected_by, AFF_MUTATION4 );
    if( xIS_SET( ( ch )->affected_by, AFF_MUTATION5 ) )
      xREMOVE_BIT( ch->affected_by, AFF_MUTATION5 );
  }
  ch->powerup = 0;
  send_to_pager_color( "&BYou power down and return to normal.\n\r", ch );
  if( xIS_SET( ( ch )->affected_by, AFF_POWERUPS ) )
  {
    xREMOVE_BIT( ( ch )->affected_by, AFF_POWERUPS );
  }
  if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
    xREMOVE_BIT( ch->affected_by, AFF_HEART );
  transStatRemove( ch );
  ch->pl = ch->exp;
  heart_calc( ch, "" );

  if( is_splitformed( ch ) )
  {
    for( och = first_char; och; och = och_next )
    {
      och_next = och->next;

      if( !IS_NPC( och ) )
        continue;

      if( is_split( och ) && och->master == ch )
        do_powerdown( och, "" );
    }
  }

  return;
}

void do_powerup( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *och;
  CHAR_DATA *och_next;

  float pl_mult = 1;
  int auraColor = AT_YELLOW;

  if( IS_NPC( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( IS_AFFECTED( ch, AFF_BIOJR ) && IS_NPC( ch ) )
    return;

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SSJ )
      || xIS_SET( ( ch )->affected_by, AFF_SSJ2 )
      || xIS_SET( ( ch )->affected_by, AFF_SSJ3 )
      || xIS_SET( ( ch )->affected_by, AFF_SSJ4 )
      || xIS_SET( ( ch )->affected_by, AFF_KAIOKEN )
      || xIS_SET( ( ch )->affected_by, AFF_HYPER )
      || xIS_SET( ( ch )->affected_by, AFF_SNAMEK )
      || xIS_SET( ( ch )->affected_by, AFF_ICER2 )
      || xIS_SET( ( ch )->affected_by, AFF_ICER3 )
      || xIS_SET( ( ch )->affected_by, AFF_ICER4 )
      || xIS_SET( ( ch )->affected_by, AFF_ICER5 )
      || xIS_SET( ( ch )->affected_by, AFF_SEMIPERFECT )
      || xIS_SET( ( ch )->affected_by, AFF_PERFECT )
      || xIS_SET( ( ch )->affected_by, AFF_ULTRAPERFECT )
      || xIS_SET( ( ch )->affected_by, AFF_OOZARU )
      || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU )
      || xIS_SET( ( ch )->affected_by, AFF_EXTREME )
      || xIS_SET( ( ch )->affected_by, AFF_MYSTIC )
      || xIS_SET( ( ch )->affected_by, AFF_LSSJ )
      || xIS_SET( ( ch )->affected_by, AFF_MAJIN )
      || xIS_SET( ( ch )->affected_by, AFF_SIGMA_VIRUS )
      || xIS_SET( ( ch )->affected_by, AFF_SENT_CHIP )
      || xIS_SET( ( ch )->affected_by, AFF_SUPERANDROID )
      || xIS_SET( ( ch )->affected_by, AFF_MAKEOSTAR )
      || xIS_SET( ( ch )->affected_by, AFF_EVILBOOST )
      || xIS_SET( ( ch )->affected_by, AFF_EVILSURGE )
      || xIS_SET( ( ch )->affected_by, AFF_EVILOVERLOAD )
      || xIS_SET( ( ch )->affected_by, AFF_EVIL_TRANS )
      || xIS_SET( ( ch )->affected_by, AFF_KID_TRANS )
      || xIS_SET( ( ch )->affected_by, AFF_SUPER_TRANS )
      || xIS_SET( ( ch )->affected_by, AFF_UNAMEK )
      || xIS_SET( ( ch )->affected_by, AFF_MICER )
      || xIS_SET( ( ch )->affected_by, AFF_SUPER_OOZARU )
      || xIS_SET( ( ch )->affected_by, AFF_T1 )
      || xIS_SET( ( ch )->affected_by, AFF_T2 )
      || xIS_SET( ( ch )->affected_by, AFF_T3 )
      || xIS_SET( ( ch )->affected_by, AFF_T4 )
      || xIS_SET( ( ch )->affected_by, AFF_OMEGA )
      || xIS_SET( ( ch )->affected_by, AFF_VM1 )
      || xIS_SET( ( ch )->affected_by, AFF_VM2 )
      || xIS_SET( ( ch )->affected_by, AFF_ESSJ )
      || xIS_SET( ( ch )->affected_by, AFF_HSSJ )
      || xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN1 )
      || xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN2 ) 
	  || xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN3 )
	  || xIS_SET( ( ch )->affected_by, AFF_PUPPET )
	  || xIS_SET( ( ch )->affected_by, AFF_SFLAME1 )
	  || xIS_SET( ( ch )->affected_by, AFF_SFLAME2 )
	  || xIS_SET( ( ch )->affected_by, AFF_SFLAME3 )
	  || xIS_SET( ( ch )->affected_by, AFF_UFLAME )
	  || xIS_SET( ( ch )->affected_by, AFF_ADULT )
	  || xIS_SET( ( ch )->affected_by, AFF_GORON )
	  || xIS_SET( ( ch )->affected_by, AFF_DEKU )
	  || xIS_SET( ( ch )->affected_by, AFF_ZORA )
	  || xIS_SET( ( ch )->affected_by, AFF_FDEITY )
	  || xIS_SET( ( ch )->affected_by, AFF_ACTIVE_FORM )
	  || xIS_SET( ( ch )->affected_by, AFF_POWER_FORM )
	  || xIS_SET( ( ch )->affected_by, AFF_PROTO_FORM ) 
	  || xIS_SET( ( ch )->affected_by, AFF_OMEGA_FORM )
	  || xIS_SET( ( ch )->affected_by, AFF_DEMBRACE )
	  || xIS_SET( ( ch )->affected_by, AFF_ARAPTURE )
	  || xIS_SET( ( ch )->affected_by, AFF_SERALIGHT )
	  || xIS_SET( ( ch )->affected_by, AFF_INNERWILL )
	  || xIS_SET( ( ch )->affected_by, AFF_MECHAEVO )
	  || xIS_SET( ( ch )->affected_by, AFF_ANCIENTEVO )
	  || xIS_SET( ( ch )->affected_by, AFF_MUTATION1 )
	  || xIS_SET( ( ch )->affected_by, AFF_MUTATION2 )
	  || xIS_SET( ( ch )->affected_by, AFF_MUTATION3 )
	  || xIS_SET( ( ch )->affected_by, AFF_MUTATION4 )
	  || xIS_SET( ( ch )->affected_by, AFF_MUTATION5 ) )
  {
    send_to_pager_color( "&BYou can't power up any more in this form.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->pcdata->auraColorPowerUp > 0 )
    auraColor = ch->pcdata->auraColorPowerUp;

  if( !str_cmp( argument, "max" ) )
  {
    if( is_android( ch ) || is_superandroid( ch ) )
    {
      xSET_BIT( ch->affected_by, AFF_POWERUPS );
      act( auraColor,
           "With a bright flash and a deafening boom that fills the air with a static charge you power up to your maximum.",
           ch, NULL, NULL, TO_CHAR );
      act( auraColor,
           "With a bright flash and a deafening boom that fills the air with a static charge $n powers up to $s maximum.",
           ch, NULL, NULL, TO_NOTVICT );
    }
    else
    {
      xSET_BIT( ch->affected_by, AFF_POWERUPS );
      act( auraColor, "With a blinding flash you power up to your maximum.", ch, NULL, NULL, TO_CHAR );
      act( auraColor, "With a blinding flash $n powers up to $s maximum.", ch, NULL, NULL, TO_NOTVICT );
    }
    if( ch->exp <= 100000 )
    {
      ch->powerup = 1;
      pl_mult = 1.1;
    }
    else if( ch->exp <= 250000 )
    {
      ch->powerup = 2;
      pl_mult = 1.2;
    }
    else if( ch->exp <= 500000 )
    {
      ch->powerup = 3;
      pl_mult = 1.3;
    }
    else if( ch->exp <= 1000000 )
    {
      ch->powerup = 4;
      pl_mult = 1.4;
    }
    else if( ch->exp <= 2000000 )
    {
      ch->powerup = 5;
      pl_mult = 1.5;
    }
    else if( ch->exp <= 5000000 )
    {
      ch->powerup = 6;
      pl_mult = 1.6;
    }
    else
    {
      ch->powerup = 7;
      pl_mult = 1.7;
    }

    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    ch->pl = ch->exp * pl_mult;
    heart_calc( ch, "" );
    if( is_splitformed( ch ) )
    {
      for( och = first_char; och; och = och_next )
      {
        och_next = och->next;

        if( !IS_NPC( och ) )
          continue;

        if( is_split( och ) && och->master == ch )
          do_powerup( och, argument );
      }
    }
    return;
  }

  if( ch->pl < ch->exp )
  {
    act( auraColor, "You power up to your base.", ch, NULL, NULL, TO_CHAR );
    act( auraColor, "$n powers up to $s base.", ch, NULL, NULL, TO_NOTVICT );
    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    ch->powerup = 0;
    ch->pl = ch->exp;
    heart_calc( ch, "" );
    if( is_splitformed( ch ) )
    {
      for( och = first_char; och; och = och_next )
      {
        och_next = och->next;

        if( !IS_NPC( och ) )
          continue;

        if( is_split( och ) && och->master == ch )
          do_powerup( och, argument );
      }
    }
    return;
  }

  if( ch->powerup == 0 )
  {
    if( is_android( ch ) || is_superandroid( ch ) )
    {
      act( auraColor, "A soft hum begins to fill the air as you power up for the first time.", ch, NULL, NULL, TO_CHAR );
      act( auraColor, "A soft hum begins to fill the air as $n powers up for the first time.", ch, NULL, NULL, TO_NOTVICT );
    }
    else
    {
      act( auraColor, "You begin to glow as you power up for the first time.", ch, NULL, NULL, TO_CHAR );
      act( auraColor, "$n begins to glow as $e powers up for the first time.", ch, NULL, NULL, TO_NOTVICT );
    }
    ch->powerup = 1;
    pl_mult = 1.1;
    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    xSET_BIT( ch->affected_by, AFF_POWERUPS );
    ch->pl = ch->exp * pl_mult;
    heart_calc( ch, "" );
    if( is_splitformed( ch ) )
    {
      for( och = first_char; och; och = och_next )
      {
        och_next = och->next;

        if( !IS_NPC( och ) )
          continue;

        if( is_split( och ) && och->master == ch )
          do_powerup( och, argument );
      }
    }
  }
  else if( ch->powerup == 1 && ch->exp > 100000 )
  {
    if( is_android( ch ) || is_superandroid( ch ) )
    {
      act( auraColor,
           "The soft hum coming from you is joined by whirring and grinding noises as you power up for the second time.", ch,
           NULL, NULL, TO_CHAR );
      act( auraColor,
           "The soft hum coming from $n is joined by whirring and grinding noises as $e powers up for the second time.", ch,
           NULL, NULL, TO_NOTVICT );
    }
    else
    {
      act( auraColor, "You begin to glow more as you power up for the second time.", ch, NULL, NULL, TO_CHAR );
      act( auraColor, "$n begins to glow more as $e powers up for the second time.", ch, NULL, NULL, TO_NOTVICT );
    }
    ch->powerup = 2;
    pl_mult = 1.2;
    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    ch->pl = ch->exp * pl_mult;
    heart_calc( ch, "" );
    if( is_splitformed( ch ) )
    {
      for( och = first_char; och; och = och_next )
      {
        och_next = och->next;

        if( !IS_NPC( och ) )
          continue;

        if( is_split( och ) && och->master == ch )
          do_powerup( och, argument );
      }
    }
  }
  else if( ch->powerup == 2 && ch->exp > 250000 )
  {
    if( is_android( ch ) || is_superandroid( ch ) )
    {
      act( auraColor, "The air around you gains a static charge as you power up for the third time.", ch, NULL, NULL,
           TO_CHAR );
      act( auraColor, "The air around $n gains a static charge as $e powers up for the third time.", ch, NULL, NULL,
           TO_NOTVICT );
    }
    else
    {
      act( auraColor, "You begin to glow brightly as you power up for the third time.", ch, NULL, NULL, TO_CHAR );
      act( auraColor, "$n begins to glow brightly as $e powers up for the third time.", ch, NULL, NULL, TO_NOTVICT );
    }
    ch->powerup = 3;
    pl_mult = 1.3;
    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    ch->pl = ch->exp * pl_mult;
    heart_calc( ch, "" );
    if( is_splitformed( ch ) )
    {
      for( och = first_char; och; och = och_next )
      {
        och_next = och->next;

        if( !IS_NPC( och ) )
          continue;

        if( is_split( och ) && och->master == ch )
          do_powerup( och, argument );
      }
    }
  }
  else if( ch->powerup == 3 && ch->exp > 500000 )
  {
    if( is_android( ch ) || is_superandroid( ch ) )
    {
      act( auraColor, "An internal engine roars to life, the ground shaking softly, as you power up for the fourth time.",
           ch, NULL, NULL, TO_CHAR );
      act( auraColor, "An internal engine roars to life, the ground shaking softly, as $e powers up for the fourth time.",
           ch, NULL, NULL, TO_NOTVICT );
    }
    else
    {
      act( auraColor, "The ground trembles under your feet as you power up for the fourth time.", ch, NULL, NULL, TO_CHAR );
      act( auraColor, "The ground trembles under $n's feet as $e powers up for the fourth time.", ch, NULL, NULL,
           TO_NOTVICT );
    }
    ch->powerup = 4;
    pl_mult = 1.4;
    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    ch->pl = ch->exp * pl_mult;
    heart_calc( ch, "" );
    if( is_splitformed( ch ) )
    {
      for( och = first_char; och; och = och_next )
      {
        och_next = och->next;

        if( !IS_NPC( och ) )
          continue;

        if( is_split( och ) && och->master == ch )
          do_powerup( och, argument );
      }
    }
  }
  else if( ch->powerup == 4 && ch->exp > 1000000 )
  {
    if( is_android( ch ) || is_superandroid( ch ) )
    {
      act( auraColor,
           "A high-pitched whine comes from your body, cracks appearing under your feet, as you power up for the fifth time.",
           ch, NULL, NULL, TO_CHAR );
      act( auraColor,
           "A high-pithced whine comes from $n's body, cracks appearing under $s feet, as $e powers up for the fifth time.",
           ch, NULL, NULL, TO_NOTVICT );
    }
    else
    {
      act( auraColor, "The ground really shakes as you power up for the fifth time.", ch, NULL, NULL, TO_CHAR );
      act( auraColor, "The ground really shakes as $n powers up for the fifth time.", ch, NULL, NULL, TO_NOTVICT );
    }
    ch->powerup = 5;
    pl_mult = 1.5;
    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    ch->pl = ch->exp * pl_mult;
    heart_calc( ch, "" );
    if( is_splitformed( ch ) )
    {
      for( och = first_char; och; och = och_next )
      {
        och_next = och->next;

        if( !IS_NPC( och ) )
          continue;

        if( is_split( och ) && och->master == ch )
          do_powerup( och, argument );
      }
    }
  }
  else if( ch->powerup == 5 && ch->exp > 2000000 )
  {
    if( is_android( ch ) || is_superandroid( ch ) )
    {
      act( auraColor, "Engine roaring loudly and electricity crackling through the air, you power up for the sixth time.",
           ch, NULL, NULL, TO_CHAR );
      act( auraColor, "Engine roaring loudly and electricity crackling through the air, $n powers up for the sixth time.",
           ch, NULL, NULL, TO_NOTVICT );
    }
    else
    {
      act( auraColor, "Pieces of rock fly around as you power up for the sixth time.", ch, NULL, NULL, TO_CHAR );
      act( auraColor, "Pieces of rock fly around as $n powers up for the sixth time.", ch, NULL, NULL, TO_NOTVICT );
    }
    ch->powerup = 6;
    pl_mult = 1.6;
    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    ch->pl = ch->exp * pl_mult;
    heart_calc( ch, "" );
    if( is_splitformed( ch ) )
    {
      for( och = first_char; och; och = och_next )
      {
        och_next = och->next;

        if( !IS_NPC( och ) )
          continue;

        if( is_split( och ) && och->master == ch )
          do_powerup( och, argument );
      }
    }
  }
  else if( ch->powerup == 6 && ch->exp > 5000000 )
  {
    if( is_android( ch ) || is_superandroid( ch ) )
    {
      act( auraColor,
           "The stench of ozone fills the humming air, a deafening boom blasting from you, as you power up for the last time.",
           ch, NULL, NULL, TO_CHAR );
      act( auraColor,
           "The stench of ozone fills the humming air, a deafening boom blasting from $n, as $e powers up for the last time.",
           ch, NULL, NULL, TO_NOTVICT );
    }
    else
    {
      act( auraColor, "Large chunks of rock crumble all around as you power up for the last time.", ch, NULL, NULL,
           TO_CHAR );
      act( auraColor, "Large chunks of rock crumble all around as $n powers up for the last time.", ch, NULL, NULL,
           TO_NOTVICT );
    }
    ch->powerup = 7;
    pl_mult = 1.7;
    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    ch->pl = ch->exp * pl_mult;
    heart_calc( ch, "" );
    if( is_splitformed( ch ) )
    {
      for( och = first_char; och; och = och_next )
      {
        och_next = och->next;

        if( !IS_NPC( och ) )
          continue;

        if( is_split( och ) && och->master == ch )
          do_powerup( och, argument );
      }
    }
  }
  else
    send_to_pager_color( "&BYou are already powered up to your max.\n\r", ch );

  return;
}

void show_char_to_char_scan( CHAR_DATA * list, CHAR_DATA * ch, OBJ_DATA * scouter )
{
  CHAR_DATA *rch;
  long double scaned_pl = ( long double )0;
  long double scouter_pl = ( long double )0;

  for( rch = list; rch; rch = rch->next_in_room )
  {
    if( rch == ch )
      continue;

    if( IS_NPC( rch ) )
      scaned_pl = rch->exp;
    else
      scaned_pl = rch->pl;

    if( !is_android( ch ) || !is_superandroid( ch ) )
    {
      scouter_pl = ( long double )( scouter->value[2] * ( long double )1000 );
      if( ( long double )scaned_pl > ( long double )scouter_pl )
      {
        scaned_pl = -1;
      }
      if( scaned_pl == -1 )
      {
        act( AT_GREY, "Your scouter explodes from a very strong power.", ch, NULL, rch, TO_CHAR );
        act( AT_GREY, "$n's scouter explodes from someones power.", ch, NULL, rch, TO_ROOM );
        obj_from_char( scouter );
        extract_obj( scouter );
        break;
      }
    }
    if( is_android( ch ) || is_superandroid( ch ) )
    {
      if( ch->pl < ch->exp )
      {
        if( scaned_pl > ( long double )( ch->exp * ( long double )8 ) )
        {
          return;
        }
      }
      else
      {
        if( scaned_pl > ( long double )( ch->pl * ( long double )8 ) )
        {
          return;
        }
      }
    }
    pager_printf_color( ch, "You detect a power level of %s.\n\r", num_punct_ld( scaned_pl ) );
    return;
  }
}

void do_scan( CHAR_DATA * ch, char *argument )
{
  ROOM_INDEX_DATA *was_in_room;
  EXIT_DATA *pexit;
  sh_int dir = -1;
  sh_int dist;
  sh_int max_dist = 0;
  OBJ_DATA *scouter = NULL;
  CHAR_DATA *victim = NULL;
  long double scaned_pl;
  long double scouter_pl;
  bool HBTCset = FALSE;

  set_char_color( AT_ACTION, ch );

  if( IS_AFFECTED( ch, AFF_BLIND ) )
  {
    send_to_char( "Not very effective when you're blind...\n\r", ch );
    return;
  }

  if( !is_android( ch ) && !is_superandroid( ch ) && !IS_BIOANDROID( ch ) &&
      !wearing_sentient_chip( ch ) && ( scouter = has_scouter( ch ) ) == NULL )
  {
    send_to_char( "You need a scouter to do that.\n\r", ch );
    return;
  }

  if( argument[0] == '\0' )
  {
    send_to_char( "You must scan in a direction or focus on a person.\n\r", ch );
    return;
  }

  if(   /*( dir = get_door( argument ) ) == -1
         * && */ ( victim = get_char_room( ch, argument ) ) == NULL )
  {
    send_to_char( "Scan what?\n\r", ch );
    return;
  }

  if( IS_IMMORTAL( ch ) && get_trust( ch ) < 59 )
  {
    send_to_char( "You shouldn't be scanning mortals.\n\r", ch );
    return;
  }

  if( victim )
  {
    act( AT_GREY, "You scan $N with your scouter.", ch, NULL, victim, TO_CHAR );
    if( IS_NPC( victim ) )
      scaned_pl = victim->exp;
    else
      scaned_pl = victim->pl;

    if( !is_android( ch ) && !IS_BIOANDROID( ch ) && !is_superandroid( ch ) && !wearing_sentient_chip( ch ) )
    {
      scouter_pl = ( ( long double )( scouter->value[2] ) ) * 10000000;
      if( ( long double )scaned_pl > ( long double )scouter_pl )
      {
        scaned_pl = -1;
      }
    }
    if( scaned_pl == -1 )
    {
      act( AT_GREY, "Your scouter explodes from $N's power.", ch, NULL, victim, TO_CHAR );
      obj_from_char( scouter );
      extract_obj( scouter );
      return;
    }
    if( is_android( ch ) || is_superandroid( ch ) || IS_BIOANDROID( ch ) || wearing_sentient_chip( ch ) )
    {
      int a = 0;
      if( is_android( ch ) || wearing_sentient_chip( ch ) )
        a = 8;
      if( is_superandroid( ch ) || IS_BIOANDROID( ch ) )
        a = 24;
      if( ch->pl < ch->exp )
      {
        if( scaned_pl > ch->exp * a )
        {
          pager_printf_color( ch, "Their energy is to high to scan." );
          return;
        }
      }
      else
      {
        if( scaned_pl > ch->pl * a )
        {
          pager_printf_color( ch, "Their energy is to high to scan." );
          return;
        }
      }
    }
    if( IS_NPC( victim ) )
    {
      pager_printf_color( ch, " &z______________________________ \n\r" );
      pager_printf_color( ch, "&z   &RScanning &W%s}w...&D&z\n\r", victim->short_descr );
      pager_printf_color( ch, "&z   &RRace:&W %s&z             \n\r", capitalize( npc_race[victim->race] ) );
      pager_printf_color( ch, "&z   &RPowerlevel: &W%s\n\r", num_punct_ld( scaned_pl ) );
      pager_printf_color( ch, "&z ______________________________&D\n\r " );
    }

//      pager_printf_color( ch, "%s's power level is %s.", victim->short_descr, num_punct_ld( scaned_pl ) );
    else
    {
      pager_printf_color( ch, " &z_______________________________\n\r " );
      pager_printf_color( ch, "&z &R Scanning &W%s}w...&D&z \n\r", victim->name );
      pager_printf_color( ch, "&z   &RRace:&W %s&z             \n\r", race_table[victim->race]->race_name );
      pager_printf_color( ch, "&z   &RPowerlevel: &W%s&z\n\r", num_punct_ld( scaned_pl ) );
      pager_printf_color( ch, "&z _______________________________&D\n\r " );
    }


//      pager_printf_color( ch, "%s's power level is %s.", victim->name, num_punct_ld( scaned_pl ) );
    return;
  }
  return;
  if( !IS_NPC( ch ) && ch->pcdata->nextHBTCDate != 0 )
  {
    HBTCset = TRUE;
  }

  was_in_room = ch->in_room;
  act( AT_GREY, "Scanning $t...", ch, dir_name[dir], NULL, TO_CHAR );
  act( AT_GREY, "$n scans $t.", ch, dir_name[dir], NULL, TO_ROOM );

  if( ( pexit = get_exit( ch->in_room, dir ) ) == NULL )
  {
    act( AT_GREY, "You can't see $t.", ch, dir_name[dir], NULL, TO_CHAR );
    return;
  }

  if( scouter )
    max_dist = scouter->value[2] / 10000;
  else
    max_dist = ch->exp / 100000;

  if( max_dist > 15 )
    max_dist = 15;

  for( dist = 1; dist <= max_dist; )
  {

    char_from_room( ch );
    char_to_room( ch, pexit->to_room );
    set_char_color( AT_RMNAME, ch );
    send_to_char( ch->in_room->name, ch );
    send_to_char( "\n\r", ch );
    show_char_to_char_scan( ch->in_room->first_person, ch, scouter );
    if( !has_scouter( ch ) )
      break;

    switch ( ch->in_room->sector_type )
    {
      default:
        dist++;
        break;
      case SECT_AIR:
        if( number_percent(  ) < 80 )
          dist++;
        break;
      case SECT_INSIDE:
      case SECT_FIELD:
      case SECT_UNDERGROUND:
        dist++;
        break;
      case SECT_FOREST:
      case SECT_CITY:
      case SECT_DESERT:
      case SECT_HILLS:
        dist += 2;
        break;
      case SECT_WATER_SWIM:
      case SECT_WATER_NOSWIM:
        dist += 3;
        break;
      case SECT_MOUNTAIN:
      case SECT_UNDERWATER:
      case SECT_OCEANFLOOR:
        dist += 4;
        break;
    }

    if( dist >= max_dist )
    {
      act( AT_GREY, "There is to much interference to scan farther $t.", ch, dir_name[dir], NULL, TO_CHAR );
      break;
    }
    if( ( pexit = get_exit( ch->in_room, dir ) ) == NULL )
    {
      act( AT_GREY, "Your scans $t are being blocked by an unknown force...", ch, dir_name[dir], NULL, TO_CHAR );
      break;
    }
  }

  char_from_room( ch );
  char_to_room( ch, was_in_room );

  if( HBTCset == FALSE )
  {
    ch->pcdata->nextHBTCDate = 0;
  }
  return;
}

void do_dbradar( CHAR_DATA * ch, char *argument )
{
  OBJ_DATA *radar;
  CHAR_DATA *vch;
  AREA_DATA *area;
  OBJ_DATA *obj;
  sh_int count = 0;

  if( ( radar = has_dragonradar( ch ) ) == NULL )
  {
    send_to_char( "You must be holding a dragonball radar to do that.\n\r", ch );
    return;
  }

  if( argument[0] == '\0' )
  {
    ch_printf( ch, "\n\rSyntax: dbradar area\n\r" );
    ch_printf( ch, "    or: dbradar (victim)\n\r" );
    ch_printf( ch,
               "\n\rUsing \"dbradar area\" will sweep the entire area and tell you if it detects any dragonballs, and how many.\n\r" );
    ch_printf( ch, "Using \"dbradar (victim)\" will scan a person and see if they are holding any dragonballs.\n\r" );
    return;
  }
  if( !str_cmp( argument, "area" ) )
  {
    act( AT_GREY, " ", ch, NULL, NULL, TO_CHAR );
    act( AT_GREY, "You tap the button on the top of the dragonball radar a couple times.", ch, NULL, NULL, TO_CHAR );
    act( AT_GREY, " ", ch, NULL, NULL, TO_NOTVICT );
    act( AT_GREY, "$n taps the button on the top of $s dragonball radar a couple times.", ch, NULL, NULL, TO_NOTVICT );
    area = ch->in_room->area;
    for( obj = first_object; obj; obj = obj->next )
    {
      if( obj->item_type != ITEM_DRAGONBALL )
        continue;
      if( !obj->in_room && !obj->carried_by )
        continue;
      if( obj->in_room )
      {
        if( obj->in_room->area != area )
          continue;
        count++;
        continue;
      }
      if( obj->carried_by )
      {
        if( !obj->carried_by->in_room )
          continue;
        if( obj->carried_by->in_room->area != area )
          continue;
        count++;
        continue;
      }
    }
    ch_printf( ch, "&gThere are &Y%d &gdragonball(s) in this region.\n\r", count );
    return;
  }
  if( ( vch = get_char_room( ch, argument ) ) == NULL )
  {
    ch_printf( ch, "There is nobody here by that name.\n\r" );
    return;
  }
  else
  {
    act( AT_GREY, " ", ch, NULL, NULL, TO_CHAR );
    act( AT_GREY, "You tap the button on the top of the dragonball radar a couple times.", ch, NULL, NULL, TO_CHAR );
    act( AT_GREY, " ", ch, NULL, NULL, TO_NOTVICT );
    act( AT_GREY, "$n taps the button on the top of $s dragonball radar a couple times.", ch, NULL, NULL, TO_NOTVICT );
    if( ( obj = carrying_dball( vch ) ) == NULL )
    {
      ch_printf( ch, "%s is not carrying any dragonballs.\n\r", vch->name );
      return;
    }
    OBJ_DATA *obj_next;
    for( obj = vch->first_carrying; obj != NULL; obj = obj_next )
    {
      obj_next = obj->next_content;
      if( obj->item_type == ITEM_DRAGONBALL )
      {
        count++;
      }
    }
    ch_printf( ch, "&g%s is carrying &Y%d &gdragonball(s).\n\r", vch->name, count );
    return;
  }
  do_dbradar( ch, "" );
}

/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA * ch, CHAR_DATA * victim )
{
  int chances = 0;

  return FALSE;

  if( !IS_AWAKE( victim ) )
    return FALSE;

  if( IS_NPC( victim ) && !xIS_SET( victim->defenses, DFND_PARRY ) )
    return FALSE;

  if( IS_NPC( victim ) && IS_NPC( ch ) )
    chances = victim->exp / ch->exp;
  if( IS_NPC( victim ) && !IS_NPC( ch ) )
    chances = victim->exp / ch->pl;
  if( !IS_NPC( victim ) && IS_NPC( ch ) )
    chances = victim->pl / ch->exp;
  if( !IS_NPC( victim ) && !IS_NPC( ch ) )
    chances = victim->pl / ch->pl;

  chances += get_curr_int( victim ) - 5;

  /*
   * Put in the call to chance() to allow penalties for misaligned
   * clannies.  
   */
  if( chances != 0 && victim->morph )
    chances += victim->morph->parry;

  if( IS_NPC( victim ) && IS_NPC( ch ) && !chance( victim, chances + ( victim->exp / ch->exp ) ) )
  {
    learn_from_failure( victim, gsn_dodge );
    return FALSE;
  }
  if( IS_NPC( victim ) && !IS_NPC( ch ) && !chance( victim, chances + ( victim->exp / ch->pl ) ) )
  {
    learn_from_failure( victim, gsn_dodge );
    return FALSE;
  }
  if( !IS_NPC( victim ) && IS_NPC( ch ) && !chance( victim, chances + ( victim->pl / ch->exp ) ) )
  {
    learn_from_failure( victim, gsn_dodge );
    return FALSE;
  }
  if( !IS_NPC( victim ) && !IS_NPC( ch ) && !chance( victim, chances + ( victim->pl / ch->pl ) ) )
  {
    learn_from_failure( victim, gsn_dodge );
    return FALSE;
  }

  if( !IS_NPC( victim ) && !IS_SET( victim->pcdata->flags, PCFLAG_GAG ) )
     /*SB*/ act( AT_SKILL, "You anticipate $n's attack and block it.", ch, NULL, victim, TO_VICT );

  if( !IS_NPC( ch ) && !IS_SET( ch->pcdata->flags, PCFLAG_GAG ) ) /* SB */
    act( AT_SKILL, "$N anticipates your attack and blocks it.", ch, NULL, victim, TO_CHAR );

  learn_from_success( victim, gsn_parry );
  return TRUE;
}



/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA * ch, CHAR_DATA * victim )
{
  int chances = 0;

  return FALSE;

  if( !IS_AWAKE( victim ) )
    return FALSE;

  if( IS_NPC( victim ) && !xIS_SET( victim->defenses, DFND_DODGE ) )
    return FALSE;

  if( IS_NPC( victim ) && IS_NPC( ch ) )
    chances = victim->exp / ch->exp;
  if( IS_NPC( victim ) && !IS_NPC( ch ) )
    chances = victim->exp / ch->pl;
  if( !IS_NPC( victim ) && !IS_NPC( ch ) )
    chances = victim->pl / ch->exp;
  if( !IS_NPC( victim ) && !IS_NPC( ch ) )
    chances = victim->pl / ch->pl;

  chances += get_curr_dex( victim ) - 5;

  if( chances != 0 && victim->morph != NULL )
    chances += victim->morph->dodge;

  /*
   * Consider luck as a factor 
   */
  if( IS_NPC( victim ) && IS_NPC( ch ) && !chance( victim, chances + ( victim->exp / ch->exp ) ) )
  {
    learn_from_failure( victim, gsn_dodge );
    return FALSE;
  }
  if( IS_NPC( victim ) && !IS_NPC( ch ) && !chance( victim, chances + ( victim->exp / ch->pl ) ) )
  {
    learn_from_failure( victim, gsn_dodge );
    return FALSE;
  }
  if( !IS_NPC( victim ) && IS_NPC( ch ) && !chance( victim, chances + ( victim->pl / ch->exp ) ) )
  {
    learn_from_failure( victim, gsn_dodge );
    return FALSE;
  }
  if( !IS_NPC( victim ) && !IS_NPC( ch ) && !chance( victim, chances + ( victim->pl / ch->pl ) ) )
  {
    learn_from_failure( victim, gsn_dodge );
    return FALSE;
  }

  if( !IS_NPC( victim ) && !IS_SET( victim->pcdata->flags, PCFLAG_GAG ) )
    act( AT_SKILL, "You dodge $n's attack.", ch, NULL, victim, TO_VICT );

  if( !IS_NPC( ch ) && !IS_SET( ch->pcdata->flags, PCFLAG_GAG ) )
    act( AT_SKILL, "$N dodges your attack.", ch, NULL, victim, TO_CHAR );

  learn_from_success( victim, gsn_dodge );
  return TRUE;
}

void do_tail( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->level < skill_table[gsn_tail]->skill_level[ch->class] )
  {
    send_to_char( "That isn't quite one of your natural skills.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_tail]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_tail ) )
  {
    learn_from_success( ch, gsn_tail );
    global_retcode = damage( ch, victim, number_range( 1, ch->level ), gsn_tail );
  }
  else
  {
    learn_from_failure( ch, gsn_tail );
    global_retcode = damage( ch, victim, 0, gsn_tail );
  }
  return;
}

void do_kaiocreate( CHAR_DATA * ch, char *argument )
{
  OBJ_DATA *o;
  OBJ_INDEX_DATA *pObjIndex;

  if( IS_NPC( ch ) )
    return;

  if( argument[0] == '\0' )
  {
    ch_printf( ch, "\n\rSyntax: Kaio create <item>\n\r" );
    ch_printf( ch, "\n\r" );
    ch_printf( ch, "Item can only be one of the following:\n\r" );
    ch_printf( ch, "\n\r" );
    ch_printf( ch, "shirt pants belt vest sleeves boots\n\r" );
    return;
  }

  if( !( !str_cmp( argument, "shirt" ) ) && !( !str_cmp( argument, "pants" ) ) &&
      !( !str_cmp( argument, "belt" ) ) && !( !str_cmp( argument, "vest" ) ) &&
      !( !str_cmp( argument, "sleeves" ) ) && !( !str_cmp( argument, "boots" ) ) )
  {
    do_kaiocreate( ch, "" );
    return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_kaiocreate]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }
  if( ch->mana < skill_table[gsn_kaiocreate]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->carry_number >= 19 )
  {
    ch_printf( ch, "You haven't got any room.\n\r" );
    return;
  }


  WAIT_STATE( ch, skill_table[gsn_kaiocreate]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_kaiocreate ) )
  {
    int a = 0;
    if( !str_cmp( argument, "shirt" ) )
      a = 1210;
    else if( !str_cmp( argument, "pants" ) )
      a = 1211;
    else if( !str_cmp( argument, "vest" ) )
      a = 1212;
    else if( !str_cmp( argument, "belt" ) )
      a = 1213;
    else if( !str_cmp( argument, "sleeves" ) )
      a = 1214;
    else if( !str_cmp( argument, "boots" ) )
      a = 1215;
    if( a == 0 )
    {
      bug( "Serious problem in function kaio create", 0 );
      return;
    }
    act( AT_SKILL, "You focus your powers as a kaio and craft an item from nothingness.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n focuses $s powers as a kaio and crafts an item from nothingness.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_success( ch, gsn_kaiocreate );
    pObjIndex = get_obj_index( a );
    o = create_object_new( pObjIndex, 1, ORIGIN_OINVOKE, ch->name );
    o = obj_to_char( o, ch );
    save_char_obj( ch );
  }
  else
  {
    act( AT_SKILL, "You fail to use your kaio powers to create an item.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n fails to use $s kaio powers to create an item.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_kaiocreate );
  }
  ch->mana -= skill_table[gsn_kaiocreate]->min_mana;
  return;
}

void do_rescue( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *fch;
  int percent;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( IS_AFFECTED( ch, AFF_BERSERK ) )
  {
    send_to_char( "You aren't thinking clearly...\n\r", ch );
    return;
  }

  one_argument( argument, arg );
  if( arg[0] == '\0' )
  {
    send_to_char( "Rescue whom?\n\r", ch );
    return;
  }

  if( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( victim == ch )
  {
    send_to_char( "How about fleeing instead?\n\r", ch );
    return;
  }

  if( ch->mount )
  {
    send_to_char( "You can't do that while mounted.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && IS_NPC( victim ) )
  {
    send_to_char( "They don't need your help!\n\r", ch );
    return;
  }

  if( !ch->fighting )
  {
    send_to_char( "Too late...\n\r", ch );
    return;
  }

  if( ( fch = who_fighting( victim ) ) == NULL )
  {
    send_to_char( "They are not fighting right now.\n\r", ch );
    return;
  }

  if( who_fighting( victim ) == ch )
  {
    send_to_char( "Just running away would be better...\n\r", ch );
    return;
  }

  if( IS_AFFECTED( victim, AFF_BERSERK ) )
  {
    send_to_char( "Stepping in front of a berserker would not be an intelligent decision.\n\r", ch );
    return;
  }

  percent = number_percent(  ) - ( get_curr_lck( ch ) - 14 ) - ( get_curr_lck( victim ) - 16 );

  WAIT_STATE( ch, skill_table[gsn_rescue]->beats );
  if( !can_use_skill( ch, percent, gsn_rescue ) )
  {
    send_to_char( "You fail the rescue.\n\r", ch );
    act( AT_SKILL, "$n tries to rescue you!", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "$n tries to rescue $N!", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_rescue );
    return;
  }

  act( AT_SKILL, "You rescue $N!", ch, NULL, victim, TO_CHAR );
  act( AT_SKILL, "$n rescues you!", ch, NULL, victim, TO_VICT );
  act( AT_SKILL, "$n moves in front of $N!", ch, NULL, victim, TO_NOTVICT );

  learn_from_success( ch, gsn_rescue );
  adjust_favor( ch, 8, 1 );
  stop_fighting( fch, FALSE );
  stop_fighting( victim, FALSE );
  if( ch->fighting )
    stop_fighting( ch, FALSE );

  set_fighting( ch, fch );
  set_fighting( fch, ch );
  return;
}

void do_doublekick( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_dkick ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_dkick]->skill_level[ch->class] )
  {
    send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_dkick]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_dkick]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_dkick]->focus;

  WAIT_STATE( ch, skill_table[gsn_dkick]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_dkick ) )
  {
    learn_from_success( ch, gsn_dkick );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 1, 3 ) ), gsn_dkick );
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 1, 3 ) ), gsn_dkick );

    ch->melee = FALSE;
  }
  else
  {
    learn_from_failure( ch, gsn_dkick );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, 0, gsn_dkick );
    global_retcode = damage( ch, victim, 0, gsn_dkick );
    ch->melee = FALSE;
  }

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_dkick]->min_mana;
  return;
}


void do_doublepunch( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_dpunch ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_dpunch]->skill_level[ch->class] )
  {
    send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_dpunch]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_dpunch]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_dpunch]->focus;

  WAIT_STATE( ch, skill_table[gsn_dpunch]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_dpunch ) )
  {
    learn_from_success( ch, gsn_dpunch );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 1, 3 ) ), gsn_dpunch );
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 1, 3 ) ), gsn_dpunch );

    ch->melee = FALSE;
  }
  else
  {
    learn_from_failure( ch, gsn_dpunch );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, 0, gsn_dpunch );
    global_retcode = damage( ch, victim, 0, gsn_dpunch );
    ch->melee = FALSE;
  }

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_dpunch]->min_mana;
  return;
}

void do_kick( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_kick ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_kick]->skill_level[ch->class] )
  {
    send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_kick]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }


  if( ch->focus < skill_table[gsn_kick]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_kick]->focus;

  WAIT_STATE( ch, skill_table[gsn_kick]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_kick ) )
  {
    learn_from_success( ch, gsn_kick );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 2, 4 ) ), gsn_kick );
    ch->melee = FALSE;

    if( is_saiyan_s( ch ) && ch->pcdata->learned[gsn_ssj4] >= 100
        && ch->pcdata->learned[gsn_super_oozaru] < 10 && ch->style == STYLE_BERSERK && ch->exp >= 2500000000000.0 )
    {
      switch ( number_range( 1, 100 ) )
      {
        case 100:
          switch ( number_range( 1, 100 ) )
          {
            case 100:
              switch ( number_range( 1, 100 ) )
              {
                case 100:
                  send_to_char( "&YYou now know Super Oozaru. Enjoy.\n\r", ch );
                  ch->pcdata->learned[gsn_super_oozaru] = 10;
                  break;
                default:
                  //send_to_char("Default function, but it works.\n\r", ch);
                  break;
              }
              break;
            default:
              //send_to_char("Default function, but it works.\n\r", ch);
              break;
          }
          break;
        default:
          //send_to_char("Default function, but it works.\n\r", ch);
          break;
      }
    }
  }
  else
  {
    learn_from_failure( ch, gsn_kick );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, 0, gsn_kick );
    ch->melee = FALSE;
  }

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_kick]->min_mana;
  return;
}

void do_punch( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_punch ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_punch]->skill_level[ch->class] )
  {
    send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_punch]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_punch]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_punch]->focus;

  WAIT_STATE( ch, skill_table[gsn_punch]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_punch ) )
  {
    learn_from_success( ch, gsn_punch );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 1, 3 ) ), gsn_punch );
    ch->melee = FALSE;
  }
  else
  {
    learn_from_failure( ch, gsn_punch );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, 0, gsn_punch );
    ch->melee = FALSE;
  }

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_punch]->min_mana;
  return;
}

void do_kaioken( CHAR_DATA * ch, char *argument )
{
  int arg = 1;
  long double max = 0.0;
  int strMod = 0, spdMod = 0, intMod = 0, conMod = 0;
  char buf[MAX_STRING_LENGTH];

  arg = atoi( argument );

  if(arg < 2)
    arg = 1;

  if( !IS_NPC( ch ) && ch->pcdata->release_date != 0 )
  {
    send_to_char( "&BYour attack fizzles out, suddenly.\n\r", ch );
    return;
  }

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_kaioken ) )
      return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_SSJ )
      || xIS_SET( ( ch )->affected_by, AFF_SSJ2 )
      || xIS_SET( ( ch )->affected_by, AFF_SSJ3 ) || xIS_SET( ( ch )->affected_by, AFF_SSJ4 ) )
  {
    send_to_char( "You can't use kaioken if you are a Super Saiyan.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    send_to_char( "You can't use kaioken while you are an Oozaru.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_EXTREME ) )
  {
    send_to_char( "You can't use kaioken while using the 'Extreme Technique'.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_HYPER ) )
  {
    send_to_char( "You can't use kaioken while you are using the Hyper technique.\n\r", ch );
    return;
  }

  transStatRemove( ch );
  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
     xREMOVE_BIT( ( ch )->affected_by, AFF_KAIOKEN );
    if(!can_give_light_effect(ch))
      ch->in_room->light--;
    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    ch->pl = ch->exp;
    heart_calc( ch, "" );
    return;
  }

  max = ( ch->exp / 1000000 ) + 3;

  if( arg > max )
  {
    act( AT_SKILL, "Your body can't sustain that level of Kaioken.", ch, NULL, NULL, TO_CHAR );
    return;
  }

  if( arg > 10 && !IS_KAIO( ch ) )
  {
    act( AT_SKILL, "Kaioken can't go beyond 10 times.", ch, NULL, NULL, TO_CHAR );
    return;
  }
  else if( arg > 14 && IS_KAIO( ch ) )
  {
    act( AT_SKILL, "Kaioken can't go beyond 14 times.", ch, NULL, NULL, TO_CHAR );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_kaioken]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_kaioken ) )
  {
    ch->powerup = 0;
    if(arg == 1) {
    sprintf( buf, "Bright red flames erupt around you as you yell out KAIOKEN!!!" );
    act( AT_FIRE, buf, ch, NULL, NULL, TO_CHAR );
    if( IS_NPC( ch ) )
      sprintf( buf, "Bright red flames erupt around %s as $e yells out KAIOKEN!!!", ch->short_descr);
    else
      sprintf( buf, "Bright red flames erupt around %s as $e yells out KAIOKEN!!!", ch->name );
 
    } else {
    sprintf( buf, "Bright red flames erupt around you as you yell out KAIOKEN TIMES %d!!!", arg );
    act( AT_FIRE, buf, ch, NULL, NULL, TO_CHAR );
    if( IS_NPC( ch ) )
      sprintf( buf, "Bright red flames erupt around %s as $e yells out KAIOKEN TIMES %d!!!", ch->short_descr, arg );
    else
      sprintf( buf, "Bright red flames erupt around %s as $e yells out KAIOKEN TIMES %d!!!", ch->name, arg );
    }
    act( AT_FIRE, buf, ch, NULL, NULL, TO_NOTVICT );
    if(!can_give_light_effect(ch))
      ch->in_room->light++;
    xSET_BIT( ( ch )->affected_by, AFF_KAIOKEN );
    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    if(arg == 1)
      ch->pl = ch->exp * 1.5;
    else 
      ch->pl = ch->exp * arg;
    heart_calc( ch, "" );
    learn_from_success( ch, gsn_kaioken );
    switch ( arg )
    {
      case 1:
	strMod = 2;
	spdMod = 4;
	intMod = 0;
	conMod = -1;
	break;
      case 2:
        strMod = 4;
        spdMod = 6;
        intMod = 0;
        conMod = -1;
        break;
      case 3:
        strMod = 6;
        spdMod = 8;
        intMod = 0;
        conMod = -2;
        break;
      case 4:
        strMod = 8;
        spdMod = 11;
        intMod = 1;
        conMod = -3;
        break;
      case 5:
        strMod = 10;
        spdMod = 13;
        intMod = 1;
        conMod = -3;
        break;
      case 6:
        strMod = 12;
        spdMod = 16;
        intMod = 2;
        conMod = -4;
        break;
      case 7:
        strMod = 14;
        spdMod = 18;
        intMod = 2;
        conMod = -4;
        break;
      case 8:
        strMod = 16;
        spdMod = 20;
        intMod = 3;
        conMod = -5;
        break;
      case 9:
        strMod = 18;
        spdMod = 22;
        intMod = 3;
        conMod = -5;
        break;
      case 10:
        strMod = 20;
        spdMod = 24;
        intMod = 4;
        conMod = -6;
        break;
      case 11:
        strMod = 22;
        spdMod = 26;
        intMod = 5;
        conMod = -7;
        break;
      case 12:
        strMod = 24;
        spdMod = 28;
        intMod = 6;
        conMod = -8;
        break;
      case 13:
        strMod = 26;
        spdMod = 30;
        intMod = 7;
        conMod = -9;
        break;
      case 14:
        strMod = 28;
        spdMod = 32;
        intMod = 8;
        conMod = -10;
        break;
    }
    transStatApply( ch, strMod, spdMod, intMod, conMod );
  }
  else
  {
    if(arg == 1) {
     sprintf( buf, "You yell out KAIOKEN!!!  Red flames flicker around your body, but quickly fade away." );
      act( AT_FIRE, buf, ch, NULL, NULL, TO_CHAR );
      sprintf( buf, "%s yells out KAIOKEN!!!  Red flames flicker around $s body, but quickly fade away.", ch->name);
      act( AT_FIRE, buf, ch, NULL, NULL, TO_NOTVICT );
    } else { 
      sprintf( buf, "You yell out KAIOKEN TIMES %d!!!  Red flames flicker around your body, but quickly fade away.", arg );
      act( AT_FIRE, buf, ch, NULL, NULL, TO_CHAR );
      sprintf( buf, "%s yells out KAIOKEN TIMES %d!!!  Red flames flicker around $s body, but quickly fade away.", ch->name,
             arg );
      act( AT_FIRE, buf, ch, NULL, NULL, TO_NOTVICT );
    }
    learn_from_failure( ch, gsn_kaioken );
  }

  return;


}

void do_ssj( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_ssj ) )
      return;
  }
  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSLSSJ ) )
    {
      ch_printf( ch,
                 "You are unable to call upon those powers while you are posessed with the power of the Legendary super Saiyan.\n\r" );
      return;
    }
  }
  if( xIS_SET( ( ch )->affected_by, AFF_SUPER_OOZARU ) )
  {
    send_to_char( "You have to power out of Super Oozaru to do this.\n\r", ch );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You can't transform in to a Super Saiyan while in Kaioken.\n\r", ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_SSJ )
      || xIS_SET( ( ch )->affected_by, AFF_SSJ2 )
      || xIS_SET( ( ch )->affected_by, AFF_SSJ3 ) || xIS_SET( ( ch )->affected_by, AFF_SSJ4 ) )
  {
   send_to_char( "You power down and return to normal.\n\r", ch );
    if( xIS_SET( ( ch )->affected_by, AFF_USSJ ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_USSJ );
    if( xIS_SET( ( ch )->affected_by, AFF_USSJ2 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_USSJ2 );
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ );
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ2 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ2 );
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ3 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ3 );
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ4 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ4 );
    ch->pl = ch->exp;
    transStatRemove( ch );
   if(!can_give_light_effect(ch)) 
      ch->in_room->light--;
    if( !IS_NPC( ch ) )
    {
      ch->pcdata->haircolor = ch->pcdata->orignalhaircolor;
      ch->pcdata->eyes = ch->pcdata->orignaleyes;
    }
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    send_to_char( "You can't transform while you are an Oozaru.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_ssj]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_ssj]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_ssj ) )
  {

    if(!can_give_light_effect(ch))
      ch->in_room->light++;
    act( AT_YELLOW, "Your hair flashes blonde and your eyes turn blue as a fiery aura erupts around you.", ch, NULL, NULL,
         TO_CHAR );
    act( AT_YELLOW, "$n's hair flashes blonde and $s eyes turn blue as a fiery aura erupts around $m.", ch, NULL, NULL,
         TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_SSJ );
    ch->pl = ch->exp * 12;
    transStatApply( ch, 12, 7, 5, 7 );

    learn_from_success( ch, gsn_ssj );
    if( !IS_NPC( ch ) )
    {
      ch->pcdata->eyes = 0;
      ch->pcdata->haircolor = 3;
    }
  }
  else
  {
    switch ( number_range( 1, 3 ) )
    {
      default:
        act( AT_BLUE, "Your hair flashes blonde, but quickly returns to normal.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "$n's hair flashes blonde, but quickly returns to normal.", ch, NULL, NULL, TO_NOTVICT );
        break;
      case 1:
        act( AT_BLUE, "Your hair flashes blonde, but quickly returns to normal.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "$n's hair flashes blonde, but quickly returns to normal.", ch, NULL, NULL, TO_NOTVICT );
        break;
      case 2:
        act( AT_BLUE, "Your hair flashes blonde, but quickly returns to normal.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "$n's hair flashes blonde, but quickly returns to normal.", ch, NULL, NULL, TO_NOTVICT );
        break;
      case 3:
        act( AT_BLUE, "You almost pop a vein trying to transform in to a Super Saiyan.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "$n almost pops a vein trying to transform in to a Super Saiyan", ch, NULL, NULL, TO_NOTVICT );
        break;
    }
    learn_from_failure( ch, gsn_ssj );
  }

  ch->mana -= skill_table[gsn_ssj]->min_mana;
  return;
}

void do_ssj2( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_ssj2 ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSLSSJ ) )
    {
      ch_printf( ch,
                 "You are unable to call upon those powers while you are posessed with the power of the Legendary super Saiyan.\n\r" );
      return;
    }
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SUPER_OOZARU ) )
  {
    send_to_char( "You have to power out of Super Oozaru to do this.\n\r", ch );
    return;
  }
  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You can't transform in to a Super Saiyan while in Kaioken.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_USSJ ) || xIS_SET( ( ch )->affected_by, AFF_USSJ2 ) )
  {
    send_to_char( "You have to power out of USSJ to do this.\n\r", ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_SSJ2 ) )
  {
    send_to_char( "You power down to Super Saiyan 1.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ2 );

    if( xIS_SET( ( ch )->affected_by, AFF_SSJ3 ) )
    {
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ3 );
    }
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ4 ) )
    {
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ4 );
    }


    ch->pl = ch->exp * 12;
    transStatApply( ch, 12, 7, 5, 7 );
    if( !IS_NPC( ch ) )
    {
      ch->pcdata->eyes = 0;
      ch->pcdata->haircolor = 3;
    }
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    send_to_char( "You can't transform while you are an Oozaru.\n\r", ch );
    return;
  }

  if( !xIS_SET( ( ch )->affected_by, AFF_SSJ ) )
  {
    if( ch->mana < skill_table[gsn_ssj2]->min_mana * 1.5 )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }

    WAIT_STATE( ch, skill_table[gsn_ssj2]->beats );
    if( can_use_skill( ch, number_percent(  ), gsn_ssj2 ) )
    {
      act( AT_YELLOW, "Your hair flashes blonde, standing up in a mass of spikes. "
           "Your eyes turn blue as a fiery aura bursts into existence around you, "
           "quickly becoming charged with sheets of electricity.", ch, NULL, NULL, TO_CHAR );
      act( AT_YELLOW, "$n's hair flashes blonde, standing up in a mass of spikes. "
           "$n's eyes turn blue as a fiery aura bursts into existence around $m, "
           "quickly becoming charged with sheets of electricity.", ch, NULL, NULL, TO_NOTVICT );
      if(!can_give_light_effect(ch))
	ch->in_room->light++;
      xSET_BIT( ( ch )->affected_by, AFF_SSJ );
      xSET_BIT( ( ch )->affected_by, AFF_SSJ2 );
      ch->pl = ch->exp * 18;
      transStatApply( ch, 18, 10, 6, 10 );
      if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
        ch->pl = ch->pl * 1.1;
      learn_from_success( ch, gsn_ssj2 );
      if( !IS_NPC( ch ) )
      {
        ch->pcdata->eyes = 0;
        ch->pcdata->haircolor = 3;
      }
    }
    else
    {
      switch ( number_range( 1, 2 ) )
      {
        default:
          act( AT_BLUE, "Electricity begins arcing around your body, but it quickly disappears.", ch, NULL, NULL, TO_CHAR );
          act( AT_BLUE, "Electricity begins arcing around $n's body, but it quickly disappears.", ch, NULL, NULL,
               TO_NOTVICT );
          break;
        case 1:
          act( AT_BLUE, "Your hair flashes blonde and starts to spike up, but quickly returns to normal.", ch, NULL, NULL,
               TO_CHAR );
          act( AT_BLUE, "$n's hair flashes blonde and starts to spike up, but quickly returns to normal.", ch, NULL, NULL,
               TO_NOTVICT );
          break;
        case 2:
          act( AT_BLUE, "You almost pop a vein trying to transform in to a Super Saiyan 2.", ch, NULL, NULL, TO_CHAR );
          act( AT_BLUE, "$n almost pops a vein trying to transform in to a Super Saiyan 2.", ch, NULL, NULL, TO_NOTVICT );
          break;
      }
      learn_from_failure( ch, gsn_ssj2 );
    }

    ch->mana -= skill_table[gsn_ssj2]->min_mana * 1.5;
    return;
  }

  if( ch->mana < skill_table[gsn_ssj2]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_ssj2]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_ssj2 ) )
  {
//  act( AT_YELLOW, "Your hair flies back and electricity arcs around you as your aura flares brightly.", ch, NULL, NULL, TO_CHAR );
//  act( AT_YELLOW, "$n's hair flies back and electricity arcs around $m as $s aura flares brightly.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW, "Your hair straightens and stands more on end, your aura flaring as "
         "electricity arcs around you.", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "$n's hair straightens and stands more on end, $s aura flaring as "
         "electricity arcs around $m.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_SSJ2 );
    ch->pl = ch->exp * 18;
    transStatApply( ch, 18, 10, 6, 10 );
    learn_from_success( ch, gsn_ssj2 );
    if( !IS_NPC( ch ) )
    {
      ch->pcdata->eyes = 0;
      ch->pcdata->haircolor = 3;
    }
  }
  else
  {
    switch ( number_range( 1, 2 ) )
    {
      default:
        act( AT_BLUE, "Electricity begins arcing around your body, but it quickly disappears.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "Electricity begins arcing around $n's body, but it quickly disappears.", ch, NULL, NULL, TO_NOTVICT );
        break;
      case 1:
        act( AT_BLUE, "Electricity begins arcing around your body, but it quickly disappears.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "Electricity begins arcing around $n's body, but it quickly disappears.", ch, NULL, NULL, TO_NOTVICT );
        break;
      case 2:
        act( AT_BLUE, "You almost pop a vein trying to transform in to a Super Saiyan 2.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "$n almost pops a vein trying to transform in to a Super Saiyan 2.", ch, NULL, NULL, TO_NOTVICT );
        break;
    }
    learn_from_failure( ch, gsn_ssj2 );
  }

  ch->mana -= skill_table[gsn_ssj2]->min_mana;
  return;
}

void do_ssj3( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_ssj3 ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }
  if( xIS_SET( ( ch )->affected_by, AFF_SUPER_OOZARU ) )
  {
    send_to_char( "You have to power out of Super Oozaru to do this.\n\r", ch );
    return;
  }
  if( xIS_SET( ch->affected_by, AFF_HSSJ ) )
  {
    xREMOVE_BIT( ch->affected_by, AFF_HSSJ );
    transStatRemove( ch );
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSLSSJ ) )
    {
      ch_printf( ch,
                 "You are unable to call upon those powers while you are posessed with the power of the Legendary super Saiyan.\n\r" );
      return;
    }
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You can't transform in to a Super Saiyan while using Kaioken.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_USSJ ) || xIS_SET( ( ch )->affected_by, AFF_USSJ2 ) )
  {
    send_to_char( "You have to power out of USSJ to do this.\n\r", ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_SSJ3 ) )
  {
    send_to_char( "You power down to Super Saiyan 2.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ3 );

    if( xIS_SET( ( ch )->affected_by, AFF_SSJ4 ) )
    {
      xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ4 );
    }

    ch->pl = ch->exp * 18;
    transStatApply( ch, 18, 10, 6, 10 );
    if( !IS_NPC( ch ) )
    {
      ch->pcdata->eyes = 0;
      ch->pcdata->haircolor = 3;
    }
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    send_to_char( "You can't transform while you are an Oozaru.\n\r", ch );
    return;
  }

  if( !xIS_SET( ( ch )->affected_by, AFF_SSJ2 ) )
  {
    if( ch->mana < skill_table[gsn_ssj3]->min_mana * 2 )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }

    WAIT_STATE( ch, skill_table[gsn_ssj3]->beats );
    if( can_use_skill( ch, number_percent(  ), gsn_ssj3 ) )
    {
      act( AT_YELLOW, "A brilliant golden aura explodes around you as your hair suddenly flashes a similar color. "
           "It flows rapidly down your back as your eyebrows disappear.", ch, NULL, NULL, TO_CHAR );
      act( AT_YELLOW, "A brilliant golden aura explodes around $n as $s hair suddenly flashes a similar color. "
           "It flows rapidly down $s back as $s eyebrows disappear.", ch, NULL, NULL, TO_NOTVICT );
      if(!can_give_light_effect(ch))
	ch->in_room->light++;
      xSET_BIT( ( ch )->affected_by, AFF_SSJ );
      xSET_BIT( ( ch )->affected_by, AFF_SSJ2 );
      xSET_BIT( ( ch )->affected_by, AFF_SSJ3 );
      ch->pl = ch->exp * 26;
      transStatApply( ch, 26, 14, 8, 14 );
      if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
        ch->pl = ch->pl * 1.1;
      learn_from_success( ch, gsn_ssj3 );
      if( !IS_NPC( ch ) )
      {
        ch->pcdata->eyes = 0;
        ch->pcdata->haircolor = 3;
      }
    }
    else
    {
      switch ( number_range( 1, 3 ) )
      {
        default:
          act( AT_BLUE, "Your hair begins to crawl down your back, but it quickly returns.", ch, NULL, NULL, TO_CHAR );
          act( AT_BLUE, "$n's hair begins to crawl down $s back, but it quickly returns.", ch, NULL, NULL, TO_NOTVICT );
          break;
        case 1:
          act( AT_BLUE, "Your hair begins to crawl down your back, but it quickly returns.", ch, NULL, NULL, TO_CHAR );
          act( AT_BLUE, "$n's hair begins to crawl down $s back, but it quickly returns.", ch, NULL, NULL, TO_NOTVICT );
          break;
        case 2:
          act( AT_BLUE, "You almost pop a vein trying to transform in to a Super Saiyan 3.", ch, NULL, NULL, TO_CHAR );
          act( AT_BLUE, "$n almost pops a vein trying to transform in to a Super Saiyan 3.", ch, NULL, NULL, TO_NOTVICT );
          break;
        case 3:
          act( AT_BLUE, "Your hair starts to lengthen and change color, but quickly returns to normal.", ch, NULL, NULL,
               TO_CHAR );
          act( AT_BLUE, "$n's hair starts to lengthen and change color, but quickly returns to normal.", ch, NULL, NULL,
               TO_NOTVICT );
          break;
      }
      learn_from_failure( ch, gsn_ssj3 );
    }

    ch->mana -= skill_table[gsn_ssj3]->min_mana * 2;
    return;
  }

  if( ch->mana < skill_table[gsn_ssj3]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_ssj3]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_ssj3 ) )
  {
//  act( AT_YELLOW, "Your hair flows down your back and your eyebrows disappear as your aura flares even brighter.", ch, NULL, NULL, TO_CHAR );
//  act( AT_YELLOW, "$n's hair flows down $s back and $s eyebrows disappear as $s aura flares even brighter.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW, "Your hair lengthens, flowing down your back.  Your eyebrows disappear as your "
         "aura flashes a bright gold.", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "$n's hair lengthens, flowing down $s back.  $n's eyebrows disappear as $s "
         "aura flashes a bright gold.", ch, NULL, NULL, TO_NOTVICT );
    if(!can_give_light_effect(ch))
      ch->in_room->light++;
    xSET_BIT( ( ch )->affected_by, AFF_SSJ3 );
      ch->pl = ch->exp * 26;
      transStatApply( ch, 26, 14, 8, 14 );

    learn_from_success( ch, gsn_ssj3 );
    if( !IS_NPC( ch ) )
    {
      ch->pcdata->eyes = 0;
      ch->pcdata->haircolor = 3;
    }
  }
  else
  {
    switch ( number_range( 1, 3 ) )
    {
      default:
        act( AT_BLUE, "Your hair begins to crawl down your back, but it quickly returns.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "$n's hair begins to crawl down $s back, but it quickly returns.", ch, NULL, NULL, TO_NOTVICT );
        break;
      case 1:
        act( AT_BLUE, "Your hair begins to crawl down your back, but it quickly returns.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "$n's hair begins to crawl down $s back, but it quickly returns.", ch, NULL, NULL, TO_NOTVICT );
        break;
      case 2:
        act( AT_BLUE, "You almost pop a vein trying to transform in to a Super Saiyan 3.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "$n almost pops a vein trying to transform in to a Super Saiyan 3.", ch, NULL, NULL, TO_NOTVICT );
        break;
      case 3:
        act( AT_BLUE, "Your aura begins to flare up, but it quickly shrinks down again.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "$n's aura begins to flare up, but it quickly shrinks down again.", ch, NULL, NULL, TO_NOTVICT );
        break;
    }
    learn_from_failure( ch, gsn_ssj3 );
  }

  ch->mana -= skill_table[gsn_ssj3]->min_mana;
  return;
}

void do_ssj4( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_ssj4 ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSLSSJ ) )
    {
      ch_printf( ch,
                 "You are unable to call upon those powers while you are posessed with the power of the Legendary super Saiyan.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( !IS_NPC( ch ) )
  {
    if( ch->pcdata->tail < 1 )
    {
      ch_printf( ch, "You can't become a Super Saiyan 4 without a tail!\n\r" );
      return;
    }
  }

  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You can't transform in to a Super Saiyan while in Kaioken.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SUPER_OOZARU ) )
  {
    send_to_char( "You have to power out of Super Oozaru to do this.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_USSJ ) || xIS_SET( ( ch )->affected_by, AFF_USSJ2 ) )
  {
    send_to_char( "You have to power out of USSJ to do this.\n\r", ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_SSJ4 ) )
  {
    send_to_char( "You power down to Super Saiyan 3.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ4 );
      ch->pl = ch->exp * 26;
      transStatApply( ch, 26, 14, 8, 14 );
    if( !IS_NPC( ch ) )
    {
      ch->pcdata->eyes = 0;
      ch->pcdata->haircolor = 3;
    }
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    send_to_char( "You can't transform while you are an Oozaru.\n\r", ch );
    return;
  }

  if( !xIS_SET( ( ch )->affected_by, AFF_SSJ3 ) )
  {
    if( ch->mana < skill_table[gsn_ssj4]->min_mana * 3 )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }

    WAIT_STATE( ch, skill_table[gsn_ssj4]->beats );
    if( can_use_skill( ch, number_percent(  ), gsn_ssj4 ) )
    {
      if(!can_give_light_effect(ch))
	ch->in_room->light++;
      act( AT_RED, "You howl with anger as a menacing, fiery, red aura explodes around you. "
           "Thick red fur sprouts all over your body as your hair turns a darker variant of normal.", ch, NULL, NULL,
           TO_CHAR );
      act( AT_RED,
           "$n howls with anger as a menacing, fiery, red aura explodes around $m. "
           "Thick red fur sprouts all over $s body as $s hair turns a darker variant of normal.", ch, NULL, NULL,
           TO_NOTVICT );
      xSET_BIT( ( ch )->affected_by, AFF_SSJ );
      xSET_BIT( ( ch )->affected_by, AFF_SSJ2 );
      xSET_BIT( ( ch )->affected_by, AFF_SSJ3 );
      xSET_BIT( ( ch )->affected_by, AFF_SSJ4 );
      ch->pl = ch->exp * 36;
      transStatApply( ch, 39, 24, 15, 24 );
      learn_from_success( ch, gsn_ssj4 );
      if( !IS_NPC( ch ) )
      {
        ch->pcdata->eyes = 4;
        ch->pcdata->haircolor = 9;
      }
    }
    else
    {
      switch ( number_range( 1, 4 ) )
      {
        default:
          act( AT_BLUE, "Your hair flashes black, but quickly returns to blonde.", ch, NULL, NULL, TO_CHAR );
          act( AT_BLUE, "$n's hair flashes black, but quickly returns to blonde.", ch, NULL, NULL, TO_NOTVICT );
          break;
        case 1:
          act( AT_BLUE, "Your hair flashes black, but quickly returns to blonde.", ch, NULL, NULL, TO_CHAR );
          act( AT_BLUE, "$n's hair flashes black, but quickly returns to blonde.", ch, NULL, NULL, TO_NOTVICT );
          break;
        case 2:
          act( AT_BLUE, "You almost pop a vein trying to transform in to a Super Saiyan 4.", ch, NULL, NULL, TO_CHAR );
          act( AT_BLUE, "$n almost pops a vein trying to transform in to a Super Saiyan 4.", ch, NULL, NULL, TO_NOTVICT );
          break;
        case 3:
          act( AT_BLUE, "Red fur begins to sprout all over your body, but quickly disappears.", ch, NULL, NULL, TO_CHAR );
          act( AT_BLUE, "Red fur begins to sprout all over $n's body, but quickly disappears.", ch, NULL, NULL, TO_NOTVICT );
          break;
        case 4:
          act( AT_BLUE, "Your aura flashes a fiery red, but quickly goes back to gold.", ch, NULL, NULL, TO_CHAR );
          act( AT_BLUE, "$n's aura flashes a fiery red, but quickly goes back to gold.", ch, NULL, NULL, TO_NOTVICT );
          break;
      }
      learn_from_failure( ch, gsn_ssj4 );
    }

    ch->mana -= skill_table[gsn_ssj4]->min_mana * 3;
    return;
  }

  if( ch->mana < skill_table[gsn_ssj4]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_ssj4]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_ssj4 ) )
  {
//  act( AT_RED, "You howl with anger as your hair turns black again and grow", ch, NULL, NULL, TO_CHAR );
//  act( AT_RED, "thick red fur all over your body, finally you sprout a", ch, NULL, NULL, TO_CHAR );
//  act( AT_RED, "tail and look up with glaring eyes inset within a red outline.", ch, NULL, NULL, TO_CHAR );
//  act( AT_RED, "$n howls with anger as $s hair turns black again and grows", ch, NULL, NULL, TO_NOTVICT );
//  act( AT_RED, "thick red fur all over $s body, finally $e sprouts a", ch, NULL, NULL, TO_NOTVICT );
//  act( AT_RED, "tail and looks up at you with glaring eyes inset within a red outline.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_RED, "You howl with anger, your golden aura exploding and reforming as a fiery red.  "
         "Your hair returns to its original shade and form as thick red fur begins "
         "to sprout all over your body.", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "$n howls with anger, $s golden aura exploding and reforming as a fiery red.  "
         "$n's hair returns to its original shade and form as thick red fur begins "
         "to sprout all over $s body.", ch, NULL, NULL, TO_NOTVICT );
    if(!can_give_light_effect(ch))
      ch->in_room->light++;
    xSET_BIT( ( ch )->affected_by, AFF_SSJ4 );
    ch->pl = ch->exp * 36;
    transStatApply( ch, 39, 24, 15, 24 );

    learn_from_success( ch, gsn_ssj4 );
    if( !IS_NPC( ch ) )
    {
      ch->pcdata->eyes = 4;
      ch->pcdata->haircolor = 9;
    }
  }
  else
  {
    switch ( number_range( 1, 4 ) )
    {
      default:
        act( AT_BLUE, "Your hair flashes black, but quickly returns to blonde.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "$n's hair flashes black, but quickly returns to blonde.", ch, NULL, NULL, TO_NOTVICT );
        break;
      case 1:
        act( AT_BLUE, "Your hair flashes black, but quickly returns to blonde.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "$n's hair flashes black, but quickly returns to blonde.", ch, NULL, NULL, TO_NOTVICT );
        break;
      case 2:
        act( AT_BLUE, "You almost pop a vein trying to transform in to a super Saiyan 4.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "$n almost pops a vein trying to transform in to a super Saiyan 4.", ch, NULL, NULL, TO_NOTVICT );
        break;
      case 3:
        act( AT_BLUE, "Red fur begins to sprout all over your body, but quickly disappears.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "Red fur begins to sprout all over $n's body, but quickly disappears.", ch, NULL, NULL, TO_NOTVICT );
        break;
      case 4:
        act( AT_BLUE, "Your aura flashes a fiery red, but quickly goes back to gold.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "$n's aura flashes a fiery red, but quickly goes back to gold.", ch, NULL, NULL, TO_NOTVICT );
        break;
    }
    learn_from_failure( ch, gsn_ssj4 );
  }

  ch->mana -= skill_table[gsn_ssj4]->min_mana;
  return;
}

void do_super_namek( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_snamek ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_UNAMEK ) )
    xREMOVE_BIT( ( ch )->affected_by, AFF_UNAMEK );

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  double pl_mult = 0;
  int arg;
  bool affGrowth = FALSE;
  int sizeStr = 0, sizeSpd = 0, sizeCon = 0, sizeInt = 0;

  arg = atoi( argument );

  if( xIS_SET( ( ch )->affected_by, AFF_GROWTH ) )
  {
    affGrowth = TRUE;
    sizeStr = 10;
    sizeCon = 5;
    sizeInt = 5;
    sizeSpd = -10;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_GIANT ) )
  {
    affGrowth = TRUE;
    sizeStr = 25;
    sizeCon = 15;
    sizeInt = 10;
    sizeSpd = -25;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SNAMEK ) && arg == 0 )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_SNAMEK );
    transStatRemove( ch );
    ch->pl = ch->exp;
    if( affGrowth )
    {
      transStatApply( ch, sizeStr, sizeSpd, sizeInt, sizeCon );
    }
    return;
  }
  else if( !xIS_SET( ( ch )->affected_by, AFF_SNAMEK ) && arg == 0 )
  {
    ch_printf( ch, "You're not in Super Namek, though!\n\r" );
    return;
  }

  pl_mult = ( double )pow( ch->exp, 0.373 ) * 0.01;
  if( pl_mult > 26 )
    pl_mult = 26;
  if( pl_mult < 5 )
    pl_mult = ( double )2 + pl_mult;
  else if( pl_mult < 6 )
    pl_mult = ( double )1 + pl_mult;
  else if( pl_mult < 6.5 )
    pl_mult = ( double )0.5 + pl_mult;

  if( arg > pl_mult )
    pl_mult = ( int )pl_mult;
  else if( arg < 1 )
    pl_mult = 1;
  else
    pl_mult = arg;

  if( ch->mana < skill_table[gsn_snamek]->min_mana * pl_mult )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_snamek]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_snamek ) )
  {
    act( AT_SKILL, "You draw upon the ancient knowledge of the Namekians to transform into a Super Namek.", ch, NULL, NULL,
         TO_CHAR );
    act( AT_SKILL, "$n draws upon the ancient knowledge of the Namekians to transform into a Super Namek.", ch, NULL, NULL,
         TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_SNAMEK );


    ch->pl = ch->exp * pl_mult;
    if( affGrowth )
    {
      transStatApply( ch, pl_mult / 4 + sizeStr, pl_mult / 2 + sizeSpd, pl_mult + sizeInt, pl_mult / 4 + sizeCon );
    }
    else
    {
      transStatApply( ch, pl_mult / 4, pl_mult / 2, pl_mult, pl_mult / 4 );
    }

    learn_from_success( ch, gsn_snamek );
  }
  else
  {
    act( AT_SKILL, "You can not quite comprehend the ancient knowledge necessary to become a Super Namek.", ch, NULL, NULL,
         TO_CHAR );
    act( AT_SKILL, "$n can not quite comprehend the ancient knowledge necessary to become a Super Namek.", ch, NULL, NULL,
         TO_NOTVICT );
    learn_from_failure( ch, gsn_snamek );
  }

  ch->mana -= skill_table[gsn_snamek]->min_mana * pl_mult;
  return;
}

void do_icer_transform_2( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_icer2 ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_ICER2 )
      || xIS_SET( ( ch )->affected_by, AFF_ICER3 )
      || xIS_SET( ( ch )->affected_by, AFF_ICER4 ) || xIS_SET( ( ch )->affected_by, AFF_ICER5 ) )
  {
    send_to_char( "You power down and transform into your first form.\n\r", ch );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER2 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER2 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER3 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER3 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER4 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER4 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER5 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER5 );
    ch->pl = ch->exp;
    transStatRemove( ch );
    return;
  }

  if( ch->mana < skill_table[gsn_icer2]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_icer2]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_icer2 ) )
  {
//  act( AT_SKILL, "The temperature in the room plummets as you transform into your second form.", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "A dread chill emanates from you as your stature grows, your body and", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "muscle mass increase and your horns become more prominent, completing your", ch, NULL, NULL, TO_CHAR );
//        act( AT_SKILL, "transformation to your second form.", ch, NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "A dread chill emanates from you as your form grows huge in size, your muscles bulge and your horns"
         " become much more prominent, completing your transformation to your second form.", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "The temperature in the room plummets as $n transforms into $s second form.", ch, NULL, NULL, TO_NOTVICT );
//  act( AT_SKILL, "A dread chill emanates from $n as $s stature grows, $s body and muscle", ch, NULL, NULL, TO_NOTVICT );
//  act( AT_SKILL, "mass increases and $s horns become more prominent, completing $s", ch, NULL, NULL, TO_NOTVICT );
//  act( AT_SKILL, "transformation to $s second form.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_PURPLE, "A dread chill emanates from $n as $s form grows huge in size, $s muscles bulge and $s horns"
         " become much more prominent, completing $s transformation to $s second form.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_ICER2 );
    ch->pl = ch->exp * 7;
    transStatApply( ch, 4, 4, 2, 7 );

    learn_from_success( ch, gsn_icer2 );
  }
  else
  {
    act( AT_PURPLE, "The temperature dips a few degrees, but quickly returns to normal, as you are unable to transform.", ch,
         NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "The temperature dips a few degrees, but quickly returns to normal, as $n is unable to transform.", ch,
         NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_icer2 );
  }
  if( xIS_SET( ( ch )->affected_by, AFF_ICER3 ) )
    xREMOVE_BIT( ( ch )->affected_by, AFF_ICER3 );
  if( xIS_SET( ( ch )->affected_by, AFF_ICER4 ) )
    xREMOVE_BIT( ( ch )->affected_by, AFF_ICER4 );
  if( xIS_SET( ( ch )->affected_by, AFF_ICER5 ) )
    xREMOVE_BIT( ( ch )->affected_by, AFF_ICER5 );

  ch->mana -= skill_table[gsn_icer2]->min_mana;
  return;
}

void do_icer_transform_3( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_icer3 ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_ICER3 )
      || xIS_SET( ( ch )->affected_by, AFF_ICER4 ) || xIS_SET( ( ch )->affected_by, AFF_ICER5 ) )
  {
    send_to_char( "You power down and transform into your second form.\n\r", ch );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER3 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER3 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER4 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER4 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER5 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER5 );
    if( !xIS_SET( ( ch )->affected_by, AFF_ICER2 ) )
      xSET_BIT( ( ch )->affected_by, AFF_ICER2 );
    ch->pl = ch->exp * 7;
    transStatApply( ch, 4, 4, 2, 7 );

    return;
  }

  if( ch->mana < skill_table[gsn_icer3]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( !xIS_SET( ( ch )->affected_by, AFF_ICER2 ) )
  {
    send_to_char( "You must be in second form first.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_icer3]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_icer3 ) )
  {
//  act( AT_SKILL, "The temperature in the room plummets as you transform into your third form.", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "A cold breeze ruffles any loose items as your body shrinks a little, the back", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "of your head elongates, your nose disappears, your lips form into what almost", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "looks like a bill and your horns shrink, finishing your transformation to your third form."
//                     , ch, NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "A cold breeze swirls out from your form as you shrink slightly, your head elongates and your"
         " mouth and nose form into a sort of beak.  Your horns shrink, leaving you looking like quite"
         " a monster in your third form.", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "The temperature in the room plummets as $n transforms into $s third form.", ch, NULL, NULL, TO_NOTVICT );
//  act( AT_SKILL, "A cold breeze ruffles loose items around $n as $s body shrinks a little, the back"
//                     , ch, NULL, NULL, TO_NOTVICT );
//  act( AT_SKILL, "of $s head elongates, $s nose disappears, $s lips form into what looks like a bill"
//                     , ch, NULL, NULL, TO_NOTVICT );
//  act( AT_SKILL, "and $s horns shrink, completing $s transformation to $s third form.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_PURPLE, "A cold breeze swirls out from $n's form as $e shrinks slightly, $s head elongates and $s"
         " mouth and nose form into a sort of beak.  $n's horns shrink, leaving $m looking like quite"
         " a monster in $s third form.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_ICER3 );
    xREMOVE_BIT( ( ch )->affected_by, AFF_ICER2 );
    ch->pl = ch->exp * 14;
    transStatApply( ch, 7, 7, 4, 14 );

    learn_from_success( ch, gsn_icer3 );
  }
  else
  {
    act( AT_PURPLE, "The temperature dips a few degrees, but quickly returns to normal, as you are unable to transform.", ch,
         NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "The temperature dips a few degrees, but quickly returns to normal, as $n is unable to transform.", ch,
         NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_icer3 );
  }

  if( xIS_SET( ( ch )->affected_by, AFF_ICER4 ) )
    xREMOVE_BIT( ( ch )->affected_by, AFF_ICER4 );
  if( xIS_SET( ( ch )->affected_by, AFF_ICER5 ) )
    xREMOVE_BIT( ( ch )->affected_by, AFF_ICER5 );
  ch->mana -= skill_table[gsn_icer3]->min_mana;
  return;
}

void do_icer_transform_4( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_icer4 ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_ICER4 ) || xIS_SET( ( ch )->affected_by, AFF_ICER5 ) )
  {
    send_to_char( "You power down and transform into your third form.\n\r", ch );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER2 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER2 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER4 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER4 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER5 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER5 );
    if( !xIS_SET( ( ch )->affected_by, AFF_ICER3 ) )
      xSET_BIT( ( ch )->affected_by, AFF_ICER3 );
    ch->pl = ch->exp * 14;
    transStatApply( ch, 7, 7, 4, 14 );

    return;
  }

  if( ch->mana < skill_table[gsn_icer4]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( !xIS_SET( ( ch )->affected_by, AFF_ICER3 ) )
  {
    send_to_char( "You must be in third form first.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_icer4]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_icer4 ) )
  {
//  act( AT_SKILL, "The temperature in the room plummets as you transform into your fourth form.", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "With a sudden rush of freezing air your body reverts back to normal stature", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "while at the same time your horns disappear, you skin becomes as smooth as", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "glass and your facial features turn quite comely, bringing you to your fourth form.", ch, NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "With a sudden gust of freezing air your body reverts to a more normal shape and size.  "
         "Your horns disappear completely now, your skin and features turning as smooth as glass, "
         "leaving you looking rather sleek in your fourth form.", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "The temperature in the room plummets as $n transforms into $s fourth form.", ch, NULL, NULL, TO_NOTVICT );
//  act( AT_SKILL, "A sudden rush of freezing air swirls as $n reverts back to normal stature,", ch, NULL, NULL, TO_NOTVICT );
//  act( AT_SKILL, "$s horns disappear, $s skin becomes smooth as glass and $s facial features", ch, NULL, NULL, TO_NOTVICT );
//  act( AT_SKILL, "turn quite comely, finishing $s transformation to $s fourth form.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_PURPLE, "With a sudden gust of freezing air $n's body reverts to a more normal shape and size.  "
         "$n's horns disappear completely now, $s skin and features turning as smooth as glass, "
         "leaving $m looking rather sleek in $s fourth form.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_ICER4 );
    xREMOVE_BIT( ( ch )->affected_by, AFF_ICER3 );
    ch->pl = ch->exp * 20;
    transStatApply( ch, 10, 10, 5, 20 );

    learn_from_success( ch, gsn_icer4 );
  }
  else
  {
    act( AT_PURPLE, "The temperature dips a few degrees, but quickly returns to normal, as you are unable to transform.", ch,
         NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "The temperature dips a few degrees, but quickly returns to normal, as $n is unable to transform.", ch,
         NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_icer4 );
  }

  if( xIS_SET( ( ch )->affected_by, AFF_ICER5 ) )
    xREMOVE_BIT( ( ch )->affected_by, AFF_ICER5 );
  ch->mana -= skill_table[gsn_icer4]->min_mana;
  return;
}

void do_icer_transform_5( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_icer5 ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_ICER5 ) )
  {
    send_to_char( "You power down and transform into your fourth form.\n\r", ch );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER2 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER2 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER3 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER3 );
    if( xIS_SET( ( ch )->affected_by, AFF_ICER5 ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_ICER5 );
    if( !xIS_SET( ( ch )->affected_by, AFF_ICER4 ) )
      xSET_BIT( ( ch )->affected_by, AFF_ICER4 );
    ch->pl = ch->exp * 20;
    transStatApply( ch, 10, 10, 5, 20 );

    return;
  }

  if( ch->mana < skill_table[gsn_icer5]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( !xIS_SET( ( ch )->affected_by, AFF_ICER4 ) )
  {
    send_to_char( "You must be in fourth form first.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_icer5]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_icer5 ) )
  {
//  act( AT_SKILL, "The temperature in the room plummets as you transform into your fifth form.", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "An arctic tempest billows around you as your stature and size increase greatly,", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "armored-plates form at key points for extra protection, spikes bristle about", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "your body, and a final armored mask slides into place over your face, ending", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "the transformation to your fifth form.", ch, NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "An arctic tempest roars as your body grows huge, your muscles bulging.  Armored plates "
         "slide over weak points, spikes bristling over your body for extra protection.  An armored "
         "mask slides over your face, ending your transformation to your fifth form.", ch, NULL, NULL, TO_CHAR );
//  act( AT_SKILL, "The temperature in the room plummets as $n transforms into $s fifth form.", ch, NULL, NULL, TO_NOTVICT );
//  act( AT_SKILL, "An arctic tempest billows around $n as $s stature and size increase greatly,", ch, NULL, NULL, TO_NOTVICT );
//  act( AT_SKILL, "armored-plates form at key points for extra protection, spikes bristle about", ch, NULL, NULL, TO_NOTVICT );
//  act( AT_SKILL, "$s body, and a final armored mask slides into place over $s face, ending", ch, NULL, NULL, TO_NOTVICT );
//  act( AT_SKILL, "the transformation to $s fifth form.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_PURPLE, "An arctic tempest roars as $n's body grows huge, $s muscles bulging.  Armored plates "
         "slide over weak points, spikes bristling over $s body for extra protection.  An armored "
         "mask slides over $n's face, ending $s transformation to $s fifth form.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_ICER5 );
    xREMOVE_BIT( ( ch )->affected_by, AFF_ICER4 );
    ch->pl = ch->exp * 26;
    transStatApply( ch, 13, 13, 7, 26 );

    learn_from_success( ch, gsn_icer5 );
  }
  else
  {
    act( AT_PURPLE, "The temperature dips a few degrees, but quickly returns to normal, as you are unable to transform.", ch,
         NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "The temperature dips a few degrees, but quickly returns to normal, as $n is unable to transform.", ch,
         NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_icer5 );
  }

  ch->mana -= skill_table[gsn_icer5]->min_mana;
  return;
}

void do_fly( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_fly ) )
      return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_FLYING ) )
  {
    send_to_char( "You float down and land on the ground.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_FLYING );
    return;
  }

  if( ch->mana < skill_table[gsn_fly]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_fly]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_fly ) )
  {
    act( AT_SKILL, "You rise up into the currents of air....", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n rises up into the currents of air....", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_FLYING );
    learn_from_success( ch, gsn_fly );
  }
  else
  {
    act( AT_SKILL, "You try to fly but you fall back down to the earth.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n tries to fly but falls back down to the earth.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_fly );
  }

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_fly]->min_mana;

  return;
}

void do_energy_ball( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_energy_ball ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_energy_ball]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_energy_ball]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_energy_ball]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_energy_ball]->focus;

  WAIT_STATE( ch, skill_table[gsn_energy_ball]->beats );

  sh_int z = get_aura( ch );

  if( can_use_skill( ch, number_percent(  ), gsn_energy_ball ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 6, 8 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_BLUE, "An orb of energy flickers softly as it emerges from your unfolded hand.", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "Spreading your fingers widely changes the sputter into a more dominant ball", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "with a majority of ki focused on the core of the attack. With a solemn look", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "you release the sphere in $N's direction and injure $M slightly. &W[$t]", ch, num_punct( dam ), victim,
         TO_CHAR );

    act( AT_BLUE, "An orb of energy flickers softly as it emerges from $n's unfolded", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "hand. Spreading $s fingers widely changes the sputter into a more dominant", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "ball with a majority of ki focused on the core of the attack. With a solemn", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "look $e releases the sphere in your direction and injures you slightly. &W[$t]", ch, num_punct( dam ),
         victim, TO_VICT );

    act( AT_BLUE, "An orb of energy flickers softly as it emerges from $n's unfolded", ch, NULL, victim, TO_NOTVICT );
    act( AT_BLUE, "hand. Spreading $s fingers widely changes the sputter into a more dominant", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_BLUE, "ball with a majority of ki focused on the core of the attack. With a solemn", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_BLUE, "look $e releases the sphere in $N's direction and injures $M", ch, NULL, victim, TO_NOTVICT );
    act( AT_BLUE, "slightly. &W[$t]", ch, num_punct( dam ), victim, TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_energy_ball );
    learn_from_success( ch, gsn_energy_ball );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( z, "You missed $N with your energy ball.", ch, NULL, victim, TO_CHAR );
    act( z, "$n misses you with $s energy ball.", ch, NULL, victim, TO_VICT );
    act( z, "$n missed $N with an energy ball.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_energy_ball );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_energy_ball]->min_mana;
  return;
}

bool is_ssj4( CHAR_DATA * ch )
{
  if( xIS_SET( ch->affected_by, AFF_SSJ4 ) )
    return TRUE;

  return FALSE;
}

void do_kamehameha( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_kamehameha ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_kamehameha]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_kamehameha]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_kamehameha]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_kamehameha]->focus;

  WAIT_STATE( ch, skill_table[gsn_kamehameha]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_kamehameha ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 20, 25 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    if( is_ssj4( ch ) )
    {
      act( AT_RED, "You put your arms back and cup your hands. 'KA-ME-HA-ME-HA!!!!'	", ch, NULL, victim, TO_CHAR );
      act( AT_RED, "You push your hands forward, throwing a blue beam at $N. &W[$t]", ch, num_punct( dam ), victim,
           TO_CHAR );
      act( AT_RED, "$n puts $s arms back and cups $s hands. 'KA-ME-HA-ME-HA!!!!'	", ch, NULL, victim, TO_VICT );
      act( AT_RED, "$n pushes $s hands forward, throwing a blue beam at you. &W[$t]", ch, num_punct( dam ), victim,
           TO_VICT );
      act( AT_RED, "$n puts $s arms back and cups $s hands. 'KA-ME-HA-ME-HA!!!!'	", ch, NULL, victim, TO_NOTVICT );
      act( AT_RED, "$n pushes $s hands forward, throwing a blue beam at $N. &W[$t]", ch, num_punct( dam ), victim,
           TO_NOTVICT );
      dam = ki_absorb( victim, ch, dam, gsn_kamehameha );
      learn_from_success( ch, gsn_kamehameha );
      global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
      act( AT_LBLUE, "You put your arms back and cup your hands. 'KA-ME-HA-ME-HA!!!!'	", ch, NULL, victim, TO_CHAR );
      act( AT_LBLUE, "You push your hands forward, throwing a blue beam at $N. &W[$t]", ch, num_punct( dam ), victim,
           TO_CHAR );
      act( AT_LBLUE, "$n puts $s arms back and cups $s hands. 'KA-ME-HA-ME-HA!!!!'	", ch, NULL, victim, TO_VICT );
      act( AT_LBLUE, "$n pushes $s hands forward, throwing a blue beam at you. &W[$t]", ch, num_punct( dam ), victim,
           TO_VICT );
      act( AT_LBLUE, "$n puts $s arms back and cups $s hands. 'KA-ME-HA-ME-HA!!!!'	", ch, NULL, victim, TO_NOTVICT );
      act( AT_LBLUE, "$n pushes $s hands forward, throwing a blue beam at $N. &W[$t]", ch, num_punct( dam ), victim,
           TO_NOTVICT );
      dam = ki_absorb( victim, ch, dam, gsn_kamehameha );
      learn_from_success( ch, gsn_kamehameha );
      global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
  }
  else
  {
    act( AT_LBLUE, "You missed $N with your kamehameha.", ch, NULL, victim, TO_CHAR );
    act( AT_LBLUE, "$n misses you with $s kamehameha.", ch, NULL, victim, TO_VICT );
    act( AT_LBLUE, "$n missed $N with a kamehameha.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_kamehameha );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_kamehameha]->min_mana;
  return;
}


//Gehenoms SuperKame Do func (not a biggie) lol :p
void do_super_kamehameha( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_super_kamehameha ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

/*
	if ( !xIS_SET(ch->affected_by, AFF_SSJ2) )
	{
		if	( !xIS_SET(ch->affected_by, AFF_SSJ3) )
		{
			send_to_char( "You must be in ssj2 or ssj3 to do this attack.\n\r" ,ch);
			return;
		}
	}
*/

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_super_kamehameha]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_super_kamehameha]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_super_kamehameha]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_super_kamehameha]->focus;

  WAIT_STATE( ch, skill_table[gsn_super_kamehameha]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_super_kamehameha ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 75, 80 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( AT_LBLUE, "You clasp your hands together and close your eyes to focus on the spiritual", ch, NULL, victim,
         TO_CHAR );
    act( AT_LBLUE, "summoning about to arise. Eyes flashing open, your power level flares up as", ch, NULL, victim,
         TO_CHAR );
    act( AT_LBLUE, "your hands cup a small ball of energy forming from the surrounding atmosphere.", ch, NULL, victim,
         TO_CHAR );
    act( AT_LBLUE, "Screaming out with animosity you call out &B'KA-ME-HAA-MEE'&C and the image of", ch, NULL, victim,
         TO_CHAR );
    act( AT_LBLUE, "another warrior appears behind with a second ball of energy embraced in $s", ch, NULL, victim, TO_CHAR );
    act( AT_LBLUE, "hands. &B'HAAAAAAA!!'&C The two consecutive balls are released and blend into a", ch, NULL, victim,
         TO_CHAR );
    act( AT_LBLUE, "more destructive assault as the attack strikes $N. &W[$t]", ch, num_punct( dam ), victim, TO_CHAR );

    act( AT_LBLUE, "$n clasps $s hands together and closes $s eyes to focus on the spiritual", ch, NULL, victim, TO_VICT );
    act( AT_LBLUE, "summoning about to arise. Eyes flashing open, $n's power level flares up as", ch, NULL, victim,
         TO_VICT );
    act( AT_LBLUE, "$s hands cup a small ball of energy forming from the surrounding atmosphere.", ch, NULL, victim,
         TO_VICT );
    act( AT_LBLUE, "Screaming out with animosity $e calls out &B'KA-ME-HAA-MEE'&C and the image of", ch, NULL, victim,
         TO_VICT );
    act( AT_LBLUE, "another warrior appears behind with a second ball of energy embraced in $s", ch, NULL, victim, TO_VICT );
    act( AT_LBLUE, "hands. &B'HAAAAAAA!!'&C The two consecutive balls are released and blend into a", ch, NULL, victim,
         TO_VICT );
    act( AT_LBLUE, "more destructive assault as the attack strikes you. &W[$t]", ch, num_punct( dam ), victim, TO_VICT );

    act( AT_LBLUE, "$n clasps $s hands together and closes $s eyes to focus on the spiritual", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_LBLUE, "summoning about to arise. Eyes flashing open, $n's power level flares up as", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_LBLUE, "$s hands cup a small ball of energy forming from the surrounding atmosphere.", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_LBLUE, "Screaming out with animosity $e calls out &B'KA-ME-HAA-MEE'&C and the image of", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_LBLUE, "another warrior appears behind with a second ball of energy embraced in $s", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_LBLUE, "hands. &B'HAAAAAAA!!'&C The two consecutive balls are released and blend into a", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_LBLUE, "more destructive assault as the attack strikes $N. &W[$t]", ch, num_punct( dam ), victim, TO_NOTVICT );
    dam = ki_absorb( victim, ch, dam, gsn_super_kamehameha );
    learn_from_success( ch, gsn_super_kamehameha );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_LBLUE, "You clasp your hands together and close your eyes to focus on the spiritual", ch, NULL, victim,
         TO_CHAR );
    act( AT_LBLUE, "summoning about to arise. Eyes flashing open, your power level flares up as", ch, NULL, victim,
         TO_CHAR );
    act( AT_LBLUE, "your hands cup a small ball of energy forming from the surrounding atmosphere.", ch, NULL, victim,
         TO_CHAR );
    act( AT_LBLUE, "Screaming out with animosity you call out &B'KA-ME-HAA-MEE'&C and the image of", ch, NULL, victim,
         TO_CHAR );
    act( AT_LBLUE, "another warrior appears behind with a second ball of energy embraced in $s", ch, NULL, victim, TO_CHAR );
    act( AT_LBLUE, "hands. &B'HAAAAAAA!!'&C The two consecutive balls are released and blend into a", ch, NULL, victim,
         TO_CHAR );
    act( AT_LBLUE, "more destructive assault but unexpectedly take a turn and miss the intended ", ch, NULL, victim,
         TO_CHAR );
    act( AT_LBLUE, "target as the iconic warrior disappears.", ch, NULL, victim, TO_CHAR );

    act( AT_LBLUE, "$n clasps $s hands together and closes $s eyes to focus on the spiritual", ch, NULL, victim, TO_VICT );
    act( AT_LBLUE, "summoning about to arise. Eyes flashing open, $n's power level flares up as", ch, NULL, victim,
         TO_VICT );
    act( AT_LBLUE, "$s hands cup a small ball of energy forming from the surrounding atmosphere.", ch, NULL, victim,
         TO_VICT );
    act( AT_LBLUE, "Screaming out with animosity $e calls out &B'KA-ME-HAA-MEE'&C and the image of", ch, NULL, victim,
         TO_VICT );
    act( AT_LBLUE, "another warrior appears behind with a second ball of energy embraced in $s", ch, NULL, victim, TO_VICT );
    act( AT_LBLUE, "hands. &B'HAAAAAAA!!'&C The two consecutive balls are released and blend into a", ch, NULL, victim,
         TO_VICT );
    act( AT_LBLUE, "more destructive assault but unexpectedly take a turn and miss you as the", ch, NULL, victim, TO_VICT );
    act( AT_LBLUE, "iconic warrior disappears.", ch, NULL, victim, TO_VICT );

    act( AT_LBLUE, "$n clasps $s hands together and closes $s eyes to focus on the spiritual", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_LBLUE, "summoning about to arise. Eyes flashing open, $n's power level flares up as", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_LBLUE, "$s hands cup a small ball of energy forming from the surrounding atmosphere.", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_LBLUE, "Screaming out with animosity $e calls out &B'KA-ME-HAA-MEE'&C and the image of", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_LBLUE, "another warrior appears behind with a second ball of energy embraced in $s", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_LBLUE, "hands. &B'HAAAAAAA!!'&C The two consecutive balls are released and blend into a", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_LBLUE, "more destructive assault but unexpectedly take a turn and miss the intended", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_LBLUE, "target as the iconic warrior disappears.", ch, NULL, victim, TO_NOTVICT );

    learn_from_failure( ch, gsn_super_kamehameha );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_super_kamehameha]->min_mana;
  return;
}

void do_masenko( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_masenko ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_masenko]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_masenko]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_masenko]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_masenko]->focus;

  WAIT_STATE( ch, skill_table[gsn_masenko]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_masenko ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 10, 16 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( AT_YELLOW, "You hold your arms high above your head, charging your ki.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "You throw your hands at $N sending a yellow beam at $M, hitting $M in the chest. &W[$t]", ch,
         num_punct( dam ), victim, TO_CHAR );
    act( AT_YELLOW, "$n holds $s arms high above $s head, charging $s ki.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n throws $s hands at you sending a yellow beam at you, hitting you in the chest. &W[$t]", ch,
         num_punct( dam ), victim, TO_VICT );
    act( AT_YELLOW, "$n holds $s arms high above $s head, charging $s ki.", ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW, "$n throws $s hands at $N sending a yellow beam at $N, hitting $M in the chest. &W[$t]", ch,
         num_punct( dam ), victim, TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_masenko );
    learn_from_success( ch, gsn_masenko );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_YELLOW, "You missed $N with your masenko blast.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "$n misses you with $s masenko blast.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n missed $N with a masenko blast.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_masenko );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_masenko]->min_mana;
  return;
}

void do_sbc( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_sbc ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_sbc]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_sbc]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_sbc]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_sbc]->focus;

  WAIT_STATE( ch, skill_table[gsn_sbc]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_sbc ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 22, 26 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
/*	act( AT_SKILL, "You put two fingers to your forehead, charging your energy.", ch, NULL, victim, TO_CHAR );
 *	act( AT_SKILL, "Your throw your hand foward, sending a corkscrew beam at $N. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
 *	act( AT_SKILL, "$n puts two fingers to $s forehead, charging $s energy.", ch, NULL, victim, TO_VICT );
 *	act( AT_SKILL, "$n throws $s hands foward, sending a corkscrew beam at you. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
 *	act( AT_SKILL, "$n puts two fingers to $s forehead, charging $s energy.", ch, NULL, victim, TO_NOTVICT );
 *	act( AT_SKILL, "$n throws $s hands foward, sending a corkscrew beam at $N. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );*/

    act( AT_YELLOW, "You put two fingers to your forehead, crackling energy gathering at their tips.", ch, NULL, victim,
         TO_CHAR );
    act( AT_YELLOW, "Your thrust your fingers toward $N, " "sending out a corkscrew beam. &W[$t]", ch, num_punct( dam ),
         victim, TO_CHAR );
    act( AT_YELLOW, "$n puts two fingers to $s forehead as crackling energy gathers at their tips.", ch, NULL, victim,
         TO_VICT );
    act( AT_YELLOW, "$n thrusts $s fingers toward $N, " "sending out a corkscrew beam. &W[$t]", ch, num_punct( dam ), victim,
         TO_VICT );
    act( AT_YELLOW, "$n puts two fingers to $s forehead as " "crackling energy gathers at their tips.", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "$n throws $s fingers toward $N " "sending out a corkscrew beam. &W[$t]", ch, num_punct( dam ), victim,
         TO_NOTVICT );
    dam = ki_absorb( victim, ch, dam, gsn_sbc );
    learn_from_success( ch, gsn_sbc );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_YELLOW, "You missed $N with your special beam cannon.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "$n misses you with $s special beam cannon.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n missed $N with a special beam cannon.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_sbc );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_sbc]->min_mana;
  return;
}

void do_dd( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_dd ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_dd]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_dd]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_dd]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_dd]->focus;

  WAIT_STATE( ch, skill_table[gsn_dd]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_dd ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 12, 18 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
/*	act( AT_SKILL, "You hold your arm above your head and create a spinning ki disk.", ch, NULL, victim, TO_CHAR );
 *	act( AT_SKILL, "You throw it at $N, and it spins toward $m. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
 *	act( AT_SKILL, "$n holds $s arm above $s head and creates a spinning ki disk.", ch, NULL, victim, TO_VICT );
 *	act( AT_SKILL, "$n throws it at you, and it spins toward you. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
 *	act( AT_SKILL, "$n holds $s arm above $s head and creates a spinning ki disk.", ch, NULL, victim, TO_NOTVICT );
 *	act( AT_SKILL, "$n throws it at $N, and it spins toward $N. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );*/

    act( AT_YELLOW, "You hold your hand aloft and create a spinning ki disk above your palm.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "You throw the whirling disk and it quickly "
         "slices through the air on its way to $N. &W[$t]", ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_YELLOW, "$n holds $s hand aloft and creates a spinning ki disk above $s palm.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n the whirling disk and it quickly slices "
         "through the air on its way to you. &W[$t]", ch, num_punct( dam ), victim, TO_VICT );
    act( AT_YELLOW, "$n holds $s hand aloft and creates a spinning ki disk above $s palm.", ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW, "$n throws the whirling disk and it quickly "
         "slices through the air on its way to $N. &W[$t]", ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_dd );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_YELLOW, "You missed $N with your destructo disk.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "$n misses you with $s destructo disk.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n missed $N with a destructo disk.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_dd );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_dd]->min_mana;
  return;
}

void do_ff( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_ff ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_ff]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_ff]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_ff]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_ff]->focus;

  WAIT_STATE( ch, skill_table[gsn_ff]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_ff ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 56, 62 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( AT_YELLOW,
         "Your body begins to surge with power, arms outstretched. Bringing your hands together, a clang like echo is heard all around the area. ",
         ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW,
         "A swirling vortex of sparkling energy gathers then disappears into your hands, exploding forth like a raging storm towards $N. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_YELLOW,
         "$n begins to surge with power, arms outstretched. Bringing $s hands together, a clang like echo is heard all around the area. ",
         ch, NULL, victim, TO_VICT );
    act( AT_YELLOW,
         "A swirling vortex of sparkling energy gathers then disappears into $s hands, exploding forth like a raging storm towards you. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_YELLOW,
         "$n begins to surge with power, arms outstretched. Bringing $s hands together, a clang like echo is heard all around the area. ",
         ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW,
         "A swirling vortex of sparkling energy gathers then disappears into $s hands, exploding forth like a raging storm towards $N. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    /*
     * dam = ki_absorb( victim, ch, dam, gsn_ff );
     */
    learn_from_success( ch, gsn_ff );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_YELLOW, "You missed $N with your final flash.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "$n misses you with $s final flash.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n missed $N with a final flash.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_ff );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_ff]->min_mana;
  return;
}

void do_suppress( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *och;
  CHAR_DATA *och_next;
  long double arg = 0;

  if( IS_NPC( ch ) && !is_split( ch ) )
  {
    send_to_char( "huh?.\n\r", ch );
    return;
  }

  if( IS_AFFECTED( ch, AFF_BIOJR ) && IS_NPC( ch ) )
    return;

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( argument[0] != '\0' )
  {
    arg = is_number( argument ) ? atof( argument ) : -1;
    if( atof( argument ) < -1 && arg == -1 )
      arg = atof( argument );

    if( arg < 1 )
    {
      send_to_char( "You can not set your suppress level lower than 1.\n\r", ch );
      return;
    }

    if( arg > ch->exp )
    {
      send_to_char( "You can not set your suppress level higher than your base power.\n\r", ch );
      return;
    }
    if( !IS_NPC( ch ) )
      ch->pcdata->suppress = arg;
    send_to_char( "Okay.  Suppress level set.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_suppress]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->position == POS_EVASIVE || ch->position == POS_DEFENSIVE
      || ch->position == POS_AGGRESSIVE || ch->position == POS_BERSERK || ch->position == POS_FIGHTING )
  {
    send_to_char( "You can't suppress while in battle.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_suppress]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_suppress ) )
  {
    if( ch->exp != ch->pl && xIS_SET( ch->affected_by, AFF_OOZARU ) )
    {
      send_to_char( "You can't while you are an Oozaru.\n\r", ch );
      return;
    }
    if( ch->exp != ch->pl && xIS_SET( ch->affected_by, AFF_GOLDEN_OOZARU ) )
    {
      send_to_char( "You can't while you are a Golden Oozaru.\n\r", ch );
      return;
    }
    if( ch->exp != ch->pl && xIS_SET( ch->affected_by, AFF_MAKEOSTAR ) )
    {
      send_to_char( "You can't while you are being affected by the Makeo Star.\n\r", ch );
      return;
    }

    removeGenieTrans( ch );

    if( xIS_SET( ( ch )->affected_by, AFF_SSJ )
        || xIS_SET( ( ch )->affected_by, AFF_USSJ )
        || xIS_SET( ( ch )->affected_by, AFF_USSJ2 )
        || xIS_SET( ( ch )->affected_by, AFF_SSJ2 )
        || xIS_SET( ( ch )->affected_by, AFF_SSJ3 )
        || xIS_SET( ( ch )->affected_by, AFF_SSJ4 )
        || xIS_SET( ( ch )->affected_by, AFF_LSSJ )
        || xIS_SET( ( ch )->affected_by, AFF_MAJIN )
        || xIS_SET( ( ch )->affected_by, AFF_SIGMA_VIRUS )
        || xIS_SET( ( ch )->affected_by, AFF_SENT_CHIP )
        || xIS_SET( ( ch )->affected_by, AFF_KAIOKEN )
        || xIS_SET( ( ch )->affected_by, AFF_HYPER )
        || xIS_SET( ( ch )->affected_by, AFF_EXTREME )
        || xIS_SET( ( ch )->affected_by, AFF_SNAMEK )
        || xIS_SET( ( ch )->affected_by, AFF_ICER2 )
        || xIS_SET( ( ch )->affected_by, AFF_ICER3 )
        || xIS_SET( ( ch )->affected_by, AFF_ICER4 )
        || xIS_SET( ( ch )->affected_by, AFF_ICER5 )
        || xIS_SET( ( ch )->affected_by, AFF_SEMIPERFECT )
        || xIS_SET( ( ch )->affected_by, AFF_PERFECT )
        || xIS_SET( ( ch )->affected_by, AFF_ULTRAPERFECT )
        || xIS_SET( ( ch )->affected_by, AFF_MYSTIC )
        || xIS_SET( ( ch )->affected_by, AFF_EVILBOOST )
        || xIS_SET( ( ch )->affected_by, AFF_EVILSURGE )
        || xIS_SET( ( ch )->affected_by, AFF_EVILOVERLOAD )
        || xIS_SET( ( ch )->affected_by, AFF_UNAMEK )
        || xIS_SET( ( ch )->affected_by, AFF_DEMBRACE )
        || xIS_SET( ( ch )->affected_by, AFF_ARAPTURE )
        || xIS_SET( ( ch )->affected_by, AFF_SERALIGHT )
        || xIS_SET( ( ch )->affected_by, AFF_SUPER_OOZARU )
        || xIS_SET( ( ch )->affected_by, AFF_T1 )
        || xIS_SET( ( ch )->affected_by, AFF_T2 )
        || xIS_SET( ( ch )->affected_by, AFF_T3 )
        || xIS_SET( ( ch )->affected_by, AFF_T4 )
        || xIS_SET( ( ch )->affected_by, AFF_OMEGA )
        || xIS_SET( ( ch )->affected_by, AFF_VM1 )
        || xIS_SET( ( ch )->affected_by, AFF_VM2 )
        || xIS_SET( ( ch )->affected_by, AFF_HSSJ )
        || xIS_SET( ( ch )->affected_by, AFF_ESSJ )
        || xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN1 )
        || xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN2 ) || xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN3 ) )
    {
      if( xIS_SET( ( ch )->affected_by, AFF_SSJ ) )
      {
        xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ );
        if( !IS_NPC( ch ) )
        {
          ch->pcdata->haircolor = ch->pcdata->orignalhaircolor;
          ch->pcdata->eyes = ch->pcdata->orignaleyes;
        }
      }
      if( xIS_SET( ( ch )->affected_by, AFF_USSJ ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_USSJ );
      if( xIS_SET( ( ch )->affected_by, AFF_USSJ2 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_USSJ2 );
      if( xIS_SET( ( ch )->affected_by, AFF_SSJ2 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ2 );
      if( xIS_SET( ( ch )->affected_by, AFF_SSJ3 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ3 );
      if( xIS_SET( ( ch )->affected_by, AFF_SSJ4 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_SSJ4 );
      if( xIS_SET( ( ch )->affected_by, AFF_LSSJ ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_LSSJ );
      if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_MAJIN );
      if( xIS_SET( ( ch )->affected_by, AFF_SIGMA_VIRUS ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_SIGMA_VIRUS );
      if( xIS_SET( ( ch )->affected_by, AFF_SENT_CHIP ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_SENT_CHIP );
      if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_KAIOKEN );
      if( xIS_SET( ( ch )->affected_by, AFF_SNAMEK ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_SNAMEK );
      if( xIS_SET( ( ch )->affected_by, AFF_EXTREME ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_EXTREME );
      if( xIS_SET( ( ch )->affected_by, AFF_HYPER ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_HYPER );
      if( xIS_SET( ( ch )->affected_by, AFF_ICER2 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_ICER2 );
      if( xIS_SET( ( ch )->affected_by, AFF_ICER3 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_ICER3 );
      if( xIS_SET( ( ch )->affected_by, AFF_ICER4 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_ICER4 );
      if( xIS_SET( ( ch )->affected_by, AFF_ICER5 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_ICER5 );
      if( xIS_SET( ( ch )->affected_by, AFF_SEMIPERFECT ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_SEMIPERFECT );
      if( xIS_SET( ( ch )->affected_by, AFF_PERFECT ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_PERFECT );
      if( xIS_SET( ( ch )->affected_by, AFF_ULTRAPERFECT ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_ULTRAPERFECT );
      if( xIS_SET( ( ch )->affected_by, AFF_MYSTIC ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_MYSTIC );
      if( xIS_SET( ( ch )->affected_by, AFF_EVILBOOST ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_EVILBOOST );
      if( xIS_SET( ( ch )->affected_by, AFF_EVILSURGE ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_EVILSURGE );
      if( xIS_SET( ( ch )->affected_by, AFF_EVILOVERLOAD ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_EVILOVERLOAD );
      if( xIS_SET( ( ch )->affected_by, AFF_UNAMEK ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_UNAMEK );
      if( xIS_SET( ( ch )->affected_by, AFF_DEMBRACE ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_DEMBRACE );
      if( xIS_SET( ( ch )->affected_by, AFF_ARAPTURE ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_ARAPTURE );
      if( xIS_SET( ( ch )->affected_by, AFF_SERALIGHT ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_SERALIGHT );
      if( xIS_SET( ( ch )->affected_by, AFF_T4 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_T4 );
      if( xIS_SET( ( ch )->affected_by, AFF_T3 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_T3 );
      if( xIS_SET( ( ch )->affected_by, AFF_T2 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_T2 );
      if( xIS_SET( ( ch )->affected_by, AFF_T1 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_T1 );
      if( xIS_SET( ( ch )->affected_by, AFF_OMEGA ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_OMEGA );
      if( xIS_SET( ( ch )->affected_by, AFF_VM1 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_VM1 );
      if( xIS_SET( ( ch )->affected_by, AFF_VM2 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_VM2 );
      if( xIS_SET( ( ch )->affected_by, AFF_HSSJ ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_HSSJ );
      if( xIS_SET( ( ch )->affected_by, AFF_ESSJ ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_ESSJ );
      if( xIS_SET( ( ch )->affected_by, AFF_SUPER_OOZARU ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_SUPER_OOZARU );
      if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN1 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_SAIBAMAN1 );
      if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN2 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_SAIBAMAN2 );
      if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN3 ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_SAIBAMAN3 );
    }
    ch->powerup = 0;
    if( !IS_NPC( ch ) )
    {
      send_to_pager_color( "&BYou power down and suppress your power.\n\r", ch );
      act( AT_BLUE, "$n takes a slow, deep breath and exhales calmly.", ch, NULL, NULL, TO_NOTVICT );
    }
    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );

    if( !IS_NPC( ch ) )
    {
      if( ch->pcdata->suppress < 1 || ch->pcdata->suppress > ch->exp )
        ch->pcdata->suppress = ch->exp;

      ch->pl = ch->pcdata->suppress;
    }
    else
    {
      if( !ch->master )
        return;
      if( !ch->master->pcdata )
        return;

      if( ch->master->pcdata->suppress < 1 || ch->master->pcdata->suppress > ch->exp )
        ch->master->pcdata->suppress = ch->exp;

      ch->pl = ch->master->pcdata->suppress;
    }

    transStatRemove( ch );
    heart_calc( ch, "" );
    if( !IS_NPC( ch ) )
    {
      learn_from_success( ch, gsn_suppress );
      if( is_splitformed( ch ) )
      {
        for( och = first_char; och; och = och_next )
        {
          och_next = och->next;

          if( !IS_NPC( och ) )
            continue;

          if( ch->pl < ch->exp )
            continue;

          if( is_split( och ) && och->master == ch )
            do_suppress( och, "" );
        }
      }
    }
    if( IS_NPC( ch ) )
    {
      send_to_pager_color( "&BYou power down and suppress your power.\n\r", ch );
      act( AT_BLUE, "$n takes a slow, deep breath and exhales calmly.", ch, NULL, NULL, TO_NOTVICT );
    }
  }
  else
  {
    act( AT_SKILL, "You try but can't seem to suppress your power.", ch, NULL, NULL, TO_CHAR );
    learn_from_failure( ch, gsn_suppress );
  }

  ch->mana -= skill_table[gsn_suppress]->min_mana;
  return;

}

void do_meditate( CHAR_DATA * ch, char *argument )
{
  float left = 0;
  float right = 0;

  if( IS_NPC( ch ) )
    return;

  left = ( float )ch->mana / ch->max_mana;
  right = ( float )ch->pcdata->learned[gsn_meditate] / 100;

  if( ch->fighting )
  {
    send_to_char( "&wYou can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( left >= right )
  {
    send_to_char( "&wYou are already at peace with everything.\n\r", ch );
    return;
  }

  switch ( ch->substate )
  {
    default:
      if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
      {
        send_to_char( "&wYou can't concentrate enough for that.\n\r", ch );
        return;
      }
      if( ch->mount )
      {
        send_to_char( "&wYou can't do that while mounted.\n\r", ch );
        return;
      }
      add_timer( ch, TIMER_DO_FUN, number_range( 1, 3 ), do_meditate, 1 );
      send_to_char( "&wYou begin to meditate...\n\r", ch );
      act( AT_PLAIN, "$n begins to meditate...", ch, NULL, NULL, TO_ROOM );
      return;

    case 1:
      if( ch->fighting )
      {
        send_to_char( "&wYou stop meditating...\n\r", ch );
        act( AT_PLAIN, "$n stops meditating...", ch, NULL, NULL, TO_ROOM );
        return;
      }

      if( can_use_skill( ch, number_percent(  ), gsn_meditate ) )
      {
        send_to_char( "&wYou meditate peacefully, collecting energy from the cosmos\n\r", ch );
        learn_from_success( ch, gsn_meditate );
        ch->mana += ( float )right / 50 * ch->max_mana;
      }
      else
      {
        send_to_char( "&wYou spend several minutes in deep concentration, but fail to collect any energy.\n\r", ch );
        learn_from_failure( ch, gsn_meditate );
      }

      if( left >= right )
      {
        send_to_char( "&wYou are now at peace with everything.\n\r", ch );
        return;
      }

      add_timer( ch, TIMER_DO_FUN, number_range( 1, 3 ), do_meditate, 2 );
      return;
    case 2:
      if( ch->fighting )
      {
        send_to_char( "&wYou stop meditating...\n\r", ch );
        act( AT_PLAIN, "$n stops meditating...", ch, NULL, NULL, TO_ROOM );
        return;
      }

      if( can_use_skill( ch, number_percent(  ), gsn_meditate ) )
      {
        send_to_char( "&wYou meditate peacefully, collecting energy from the cosmos\n\r", ch );
        learn_from_success( ch, gsn_meditate );
        ch->mana += ( float )right / 50 * ch->max_mana;
      }
      else
      {
        send_to_char( "&wYou spend several minutes in deep concentration, but fail to collect any energy.\n\r", ch );
        learn_from_failure( ch, gsn_meditate );
      }

      if( left >= right )
      {
        send_to_char( "&wYou are now at peace with everything.\n\r", ch );
        return;
      }

      add_timer( ch, TIMER_DO_FUN, number_range( 1, 3 ), do_meditate, 1 );
      return;
    case SUB_TIMER_DO_ABORT:
      ch->substate = SUB_NONE;
      send_to_char( "&wYou stop meditating...\n\r", ch );
      act( AT_PLAIN, "$n stops meditating...", ch, NULL, NULL, TO_ROOM );
      return;
  }
  return;
}

void do_scatter_shot( CHAR_DATA * ch, char *arg )
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int shot = 0;
  int shots = 0;
  int dam = 0;
  int energy = 0;
  int damPerShot = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_scatter_shot ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_scatter_shot]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( arg[0] == '\0' || atoi( arg ) <= 0 )
  {
    send_to_char( "Syntax: scatter <# of Shots>\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  shot = atoi( arg );
  if( shot > 50 )
    shot = 50;
  energy = shot * skill_table[gsn_scatter_shot]->min_mana;
  shots = shot;
  strcpy( buf, num_punct( shot ) );

  if( ch->mana < energy )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < ( skill_table[gsn_scatter_shot]->focus * ( 1 + ( shot / 10 ) ) ) )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= ( skill_table[gsn_scatter_shot]->focus * ( 1 + ( shot / 10 ) ) );

  sh_int z = get_aura( ch );

  if( IS_MUTANT(ch) )
  {
    WAIT_STATE( ch, ( skill_table[gsn_scatter_shot]->beats * ( 1 + ( shot / 20 ) ) ) );
  }
  else
  {
    WAIT_STATE( ch, ( skill_table[gsn_scatter_shot]->beats * ( 1 + ( shot / 10 ) ) ) );
  }
  if( can_use_skill( ch, number_percent(  ), gsn_scatter_shot ) )
  {
    while( shots > 0 )
    {
      switch ( number_range( 0, 1 ) )
      {
        default:
        case 1:
        case 2:

        case 3:
          dam += number_range( 1, 2 );
          break;
        case 4:
          dam += number_range( 0, 1 );
          break;
      }
      shots--;
    }
//  strcpy(buf2, num_punct(dam));
    if( IS_MUTANT(ch) )
    {
      damPerShot = number_range( 3, 6 );
    }
    else
    {
      damPerShot = number_range( 1, 4 );
    }
    strcpy( buf2, num_punct( get_attmod( ch, victim ) * dam + damPerShot ) );

    act( z, "You power up and yell, 'SCATTER SHOT!'", ch, NULL, victim, TO_CHAR );
    act2( z, "You launch a barrage of $t energy balls towards $N, "
          "all exploding on contact. &W[$T]", ch, buf, victim, TO_CHAR, buf2 );
    act( z, "$n powers up and yells, 'SCATTER SHOT!'", ch, NULL, victim, TO_VICT );
    act2( z, "$e launches a barrage of $t energy balls towards you, "
          "all exploding on contact. &W[$T]", ch, buf, victim, TO_VICT, buf2 );
    act( z, "$n powers up and yells, 'SCATTER SHOT!'", ch, NULL, victim, TO_NOTVICT );
    act2( z, "$e launches a barrage of $t energy balls towards $N, "
          "all exploding on contact. &W[$T]", ch, buf, victim, TO_NOTVICT, buf2 );

    learn_from_success( ch, gsn_scatter_shot );
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * dam * damPerShot ), TYPE_HIT );
  }
  else
  {
    act( z, "You launch a barrage of $t energy balls towards $N but $E is to fast to hit.", ch, buf, victim, TO_CHAR );
    act( z, "$n launches a barrage of $t energy balls towards $N but you are to fast to hit.", ch, buf, victim, TO_VICT );
    act( z, "$n launches a barrage of $t energy balls towards $N but $E is to fast to hit.", ch, buf, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_scatter_shot );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  if( !is_android_h( ch ) )
    ch->mana -= energy;
  return;
}

void do_sense( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  char *msg;
  long double diff;

  one_argument( argument, arg );

  if( arg[0] == '\0' )
  {
    send_to_char( "Sense whos power?\n\r", ch );
    return;
  }

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_sense ) )
      return;
  }

  if( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    send_to_char( "They must be too far away for you to sense.\n\r", ch );
    return;
  }
  if( victim == ch )
  {
    pager_printf_color( ch, "You would get slaughtered in a battle against %s.\n\r", himher( ch, FALSE ) );
    return;
  }

  if( is_android( victim ) || is_superandroid( victim ) || wearing_sentient_chip( victim ) )
  {
    pager_printf_color( ch, "You can't sense any thing from %s.\n\r", himher( ch, FALSE ) );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_sense]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_sense ) )
  {
    if( !IS_NPC( victim ) )
      diff = ( long double )victim->pl / ch->pl * 10;
    else
      diff = ( long double )victim->exp / ch->pl * 10;

    if( diff < 1 )
      msg = "$N has power... to sense? LOL! That's a good one!";
    else if( diff <= 1 )
      msg = "That fly buzzing around would be more challenging.";
    else if( diff <= 2 )
      msg = "Now where did that chicken go?";
    else if( diff <= 3 )
      msg = "You sense a feeble power coming from $N.";
    else if( diff <= 4 )
      msg = "You sense a laughable power coming from $N.";
    else if( diff <= 5 )
      msg = "You sense a very small power coming from $N.";
    else if( diff <= 6 )
      msg = "You sense an insignificant power coming from $N.";
    else if( diff <= 7 )
      msg = "You sense a small power coming from $N.";
    else if( diff <= 8 )
      msg = "You sense a minor power coming from $N.";
    else if( diff <= 9 )
      msg = "You sense a lesser power coming from $N.";
    else if( diff <= 10 )
      msg = "$E seems to be about as strong as you.";
    else if( diff <= 20 )
      msg = "You sense a greater power coming from $N.";
    else if( diff <= 30 )
      msg = "You sense a significant power coming from $N.";
    else if( diff <= 40 )
      msg = "You sense a substantial power coming from $N.";
    else if( diff <= 50 )
      msg = "You sense an imposing power coming from $N.";
    else if( diff <= 60 )
      msg = "You sense a disturbing power coming from $N.";
    else if( diff <= 70 )
      msg = "You sense a frightening power coming from $N.";
    else if( diff <= 80 )
      msg = "You sense a horrifying power coming from $N.";
    else if( diff <= 90 )
      msg = "Do you feel lucky, punk?";
    else if( diff <= 100 )
      msg = "Some times, talking is the best answer...";
    else if( diff <= 250 )
      msg = "Just walk away...";
    else if( diff <= 500 )
      msg = "Are you mad!?";
    else if( diff <= 750 )
      msg = "You ARE mad!";
    else if( diff <= 1000 )
      msg = "Why don't you dig a grave for yourself first?";
    else
      msg = "What do you want on your tombstone?";
    act( AT_CONSIDER, msg, ch, NULL, victim, TO_CHAR );
    learn_from_success( ch, gsn_sense );
  }
  else
  {
    pager_printf_color( ch, "You try, but you sense nothing from %s.\n\r", himher( victim, FALSE ) );
    learn_from_failure( ch, gsn_sense );
  }

  return;
}

void do_ddd( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_ddd ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_ddd]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_ddd]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_ddd]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_ddd]->focus;

  WAIT_STATE( ch, skill_table[gsn_ddd]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_ddd ) )
  {
    switch ( number_range( 1, 100 ) )
    {
      case 100:
      case 99:
        if( IS_NPC( victim ) || ( !IS_NPC( victim ) && ( !xIS_SET( ch->act, PLR_SPAR ) || !xIS_SET( ch->act, PLR_SPAR ) ) ) )
        {

          /*
           * Redone so that instant death can't be abused to instant kill
           * * players over 2x their power. -- Islvin
           */
          if( victim->pl / ch->pl >= 2 )
          {
            dam = get_attmod( ch, victim ) * number_range( 10, 13 );
            act( AT_RED, "You charge two spinning energy disks above your head and hurl them towards $N.", ch, NULL, victim,
                 TO_CHAR );
            act( AT_RED, "They fly staight towards $N, but their damage is absorbed by $s powerful aura.", ch, NULL, victim,
                 TO_CHAR );
            act( AT_RED, "$n charges two spinning energy disks above $s head and hurls them towards you.", ch, NULL, victim,
                 TO_VICT );
            act( AT_RED, "They fly staight towards you, but their damage is absorbed by your powerful aura.", ch, NULL,
                 victim, TO_VICT );
            act( AT_RED, "$n charges two spinning energy disks above $s head and hurl them towards $N.", ch, NULL, victim,
                 TO_NOTVICT );
            act( AT_RED, "They fly staight towards $N, but their damage is absorbed by $s powerful aura.", ch, NULL, victim,
                 TO_NOTVICT );
            learn_from_success( ch, gsn_ddd );
            global_retcode = damage( ch, victim, dam, TYPE_HIT );
            break;
          }

/* Redone so it _is_ instant death -- Melora
 * 	if (victim->max_hit >= victim->hit)
 * 		dam = victim->max_hit * 2;
 * 	else
 * 		dam = victim->hit * 2;
 */
          dam = 999999999;
          act( AT_RED, "You charge two spinning energy disks above your head and hurl them towards $N.", ch, NULL, victim,
               TO_CHAR );
          act( AT_DGREY, "THEY BOTH CUT STRAIGHT THROUGH $N!!!  Killing $M instantly.", ch, NULL, victim, TO_CHAR );
          act( AT_RED, "$n charges two spinning energy disks above $s head and hurls them towards you.", ch, NULL, victim,
               TO_VICT );
          act( AT_DGREY, "THEY BOTH CUT STRAIGHT THROUGH YOU!!!  Killing you instantly.", ch, NULL, victim, TO_VICT );
          act( AT_RED, "$n charges two spinning energy disks above $s head and hurl them towards $N.", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_DGREY, "THEY BOTH CUT STRAIGHT THROUGH $N!!!  Killing $M instantly.", ch, NULL, victim, TO_NOTVICT );

          learn_from_success( ch, gsn_ddd );
          global_retcode = damage( ch, victim, dam, TYPE_HIT );
          break;
        }
      default:
        dam = get_attmod( ch, victim ) * number_range( 24, 30 );
        if( ch->charge > 0 )
          dam = chargeDamMult( ch, dam );
        act( AT_RED, "You charge two spinning energy disks above your head and hurl them towards $N. &W[$t]", ch,
             num_punct( dam ), victim, TO_CHAR );
        act( AT_RED, "$n charges two spinning energy disks above $s head and hurls them towards you. &W[$t]", ch,
             num_punct( dam ), victim, TO_VICT );
        act( AT_RED, "$n charges two spinning energy disks above $s head and hurl them towards $N. &W[$t]", ch,
             num_punct( dam ), victim, TO_NOTVICT );

        learn_from_success( ch, gsn_ddd );
        global_retcode = damage( ch, victim, dam, TYPE_HIT );
        break;
    }
  }
  else
  {
    act( AT_RED, "You missed $N with your dual destructo disk.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n misses you with $s dual destructo disk.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n missed $N with a dual destructo disk.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_ddd );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_ddd]->min_mana;
  return;
}


void do_super_dragonfist( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_super_dragonfist ) )
      return;
  }

  if( !IS_NPC( ch ) && IS_SET( ch->pcdata->flags, PCFLAG_KNOWSLSSJ ) )
  {
    ch_printf( ch, "LSSJ's can't use this.\n\r" );
    return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_super_dragonfist]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }
  if( !is_bio( ch ) || !is_human(ch) || !is_saiyan(ch))
  {
    if( !SAIYANRANKED( ch ) )
    {
      if( ch->race == race_lookup( "saiyan" ) )
      {
        ch_printf( ch, "Huh?\n\r" );
        return;
      }
    }
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_super_dragonfist]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_super_dragonfist]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_super_dragonfist]->focus;


  WAIT_STATE( ch, skill_table[gsn_super_dragonfist]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_super_dragonfist ) )
  {
    switch ( number_range( 1, 100 ) )
    {
      case 100:
      case 95:
        if( IS_NPC( victim ) || ( !IS_NPC( victim ) && ( !xIS_SET( ch->act, PLR_SPAR ) || !xIS_SET( ch->act, PLR_SPAR ) ) ) )
        {


          if( victim->pl / ch->pl >= 2 )
          {
            act( AT_DGREY, "With impeccable swiftness you hurtle toward $N with an outstretched arm", ch, NULL, victim,
                 TO_CHAR );
            act( AT_DGREY, "pinpointing the location of impact. Calling upon the eternal dragon for strength", ch, NULL,
                 victim, TO_CHAR );
            act( AT_DGREY, "you yell, &YDRAGON FIST!. &zAbruptly your once prolonged arm has now altered", ch, NULL, victim,
                 TO_CHAR );
            act( AT_DGREY, "into an impressive golden dragon with mouth gaping extensively. The initial blow", ch, NULL,
                 victim, TO_CHAR );
            act( AT_DGREY, "devastates $N only to be pursued by a torrential upsurge of energy", ch, NULL, victim, TO_CHAR );
            act( AT_DGREY, "created from the outburst of the assault.", ch, NULL, victim, TO_CHAR );

            act( AT_DGREY, "With impeccable swiftness $n hurtles toward you with an outstretched arm", ch, NULL, victim,
                 TO_VICT );
            act( AT_DGREY, "pinpointing the location of impact. Calling upon the eternal dragon for strength", ch, NULL,
                 victim, TO_VICT );
            act( AT_DGREY, "$e yells, &YDRAGON FIST!. &zAbruptly $s once prolonged arm has now", ch, NULL, victim, TO_VICT );
            act( AT_DGREY, "altered into an impressive golden dragon with mouth gaping extensively. The", ch, NULL, victim,
                 TO_VICT );
            act( AT_DGREY, "initial blow devastates you only to be pursued by a torrential upsurge of energy", ch, NULL,
                 victim, TO_VICT );
            act( AT_DGREY, "created from the outburst of the assault.", ch, NULL, victim, TO_VICT );

            act( AT_DGREY, "With impeccable swiftness $n hurtles toward $N with an outstretched", ch, NULL, victim,
                 TO_NOTVICT );
            act( AT_DGREY, "arm pinpointing the location of impact. Calling upon the eternal dragon for", ch, NULL, victim,
                 TO_NOTVICT );
            act( AT_DGREY, "strength $e yells, &YDRAGON FIST!. &zAbruptly $n.s once prolonged", ch, NULL, victim,
                 TO_NOTVICT );
            act( AT_DGREY, "arm has now altered into an impressive golden dragon with mouth gaping", ch, NULL, victim,
                 TO_NOTVICT );
            act( AT_DGREY, "extensively. The initial blow devastates $N only to be pursued by a torrential", ch, NULL,
                 victim, TO_NOTVICT );
            act( AT_DGREY, "upsurge of energy created from the outburst of the assault.", ch, NULL, victim, TO_NOTVICT );
            dam = 0;
            break;
          }

          dam = 999999999;
          act( AT_YELLOW,
               "You jump high into the air your fist pointing upwards your aura flaring up as you beging to gather energy in your fist your eyes staring right at $N. &D",
               ch, NULL, victim, TO_CHAR );
          act( AT_YELLOW,
               "You clench your fist tight as you release a tremendous amount of energy  and yell out 'Dragon Fist' the energy  taking shape of a huge dragon and slamming at $N with quick speed.&D",
               ch, NULL, victim, TO_CHAR );

          act( AT_DGREY, "The Dragon, than flys through $N body!!!  Killing $N instantly.", ch, num_punct( dam ), victim,
               TO_CHAR );

          act( AT_YELLOW,
               "$n jumps high into the air his fist pointing upwards his aura flaring up as he beging to gather energy in his fist his eyes staring right at you. &D",
               ch, NULL, victim, TO_VICT );
          act( AT_YELLOW,
               "$n clenches his fist tight as he releases a tremendous amount of energy  and yell out 'Dragon Fist' the energy  taking shape of a huge dragon and slamming at you with quick speed. &D",
               ch, NULL, victim, TO_VICT );

          act( AT_DGREY, "The Dragon, than flys through your body!!!  Killing you instantly.", ch, num_punct( dam ), victim,
               TO_VICT );

          act( AT_YELLOW,
               "$n jumps high into the air his fist pointing upwards his aura flaring up as he beging to gather energy in his fist his eyes staring right at $N.&D",
               ch, NULL, victim, TO_NOTVICT );
          act( AT_YELLOW,
               "$n clenches his fist tight as he releases a tremendous amount of energy  and yells out 'Dragon Fist' the energy  taking shape of a huge dragon and slamming at $N with quick speed.&D",
               ch, NULL, victim, TO_NOTVICT );
          act( AT_DGREY, "The Dragon, than flys through $N body!!!  Killing $N instantly.", ch, num_punct( dam ), victim,
               TO_NOTVICT );

          learn_from_success( ch, gsn_super_dragonfist );
          global_retcode = damage( ch, victim, dam, TYPE_HIT );
          break;
        }
      default:
        dam = get_attmod( ch, victim ) * number_range( 130, 160 );
        if( ch->charge > 0 )
          dam = chargeDamMult( ch, dam );
        act( AT_DGREY, "With impeccable swiftness you hurtle toward $N with an outstretched arm", ch, NULL, victim,
             TO_CHAR );
        act( AT_DGREY, "pinpointing the location of impact. Calling upon the eternal dragon for strength", ch, NULL, victim,
             TO_CHAR );
        act( AT_DGREY, "you yell, &YDRAGON FIST!. &zAbruptly your once prolonged arm has now altered", ch, NULL, victim,
             TO_CHAR );
        act( AT_DGREY, "into an impressive golden dragon with mouth gaping extensively. The initial blow", ch, NULL, victim,
             TO_CHAR );
        act( AT_DGREY, "devastates $N only to be pursued by a torrential upsurge of energy", ch, NULL, victim, TO_CHAR );
        act( AT_DGREY, "created from the outburst of the assault. &W[$t]", ch, num_punct( dam ), victim, TO_CHAR );

        act( AT_DGREY, "With impeccable swiftness $n hurtles toward you with an outstretched arm", ch, NULL, victim,
             TO_VICT );
        act( AT_DGREY, "pinpointing the location of impact. Calling upon the eternal dragon for strength", ch, NULL, victim,
             TO_VICT );
        act( AT_DGREY, "$e yells, &YDRAGON FIST!. &zAbruptly $s once prolonged arm has now", ch, NULL, victim, TO_VICT );
        act( AT_DGREY, "altered into an impressive golden dragon with mouth gaping extensively. The", ch, NULL, victim,
             TO_VICT );
        act( AT_DGREY, "initial blow devastates you only to be pursued by a torrential upsurge of energy", ch, NULL, victim,
             TO_VICT );
        act( AT_DGREY, "created from the outburst of the assault. &W[$t]", ch, num_punct( dam ), victim, TO_VICT );

        act( AT_DGREY, "With impeccable swiftness $n hurtles toward $N with an outstretched", ch, NULL, victim, TO_NOTVICT );
        act( AT_DGREY, "arm pinpointing the location of impact. Calling upon the eternal dragon for", ch, NULL, victim,
             TO_NOTVICT );
        act( AT_DGREY, "strength $e yells, &YDRAGON FIST!. &zAbruptly $n.s once prolonged", ch, NULL, victim, TO_NOTVICT );
        act( AT_DGREY, "arm has now altered into an impressive golden dragon with mouth gaping", ch, NULL, victim,
             TO_NOTVICT );
        act( AT_DGREY, "extensively. The initial blow devastates $N only to be pursued by a torrential", ch, NULL, victim,
             TO_NOTVICT );
        act( AT_DGREY, "upsurge of energy created from the outburst of the assault. &W[$t]", ch, num_punct( dam ), victim,
             TO_NOTVICT );

        learn_from_success( ch, gsn_super_dragonfist );
        global_retcode = damage( ch, victim, dam, TYPE_HIT );
        break;
    }
  }
  else
  {
    act( AT_YELLOW, "You missed $N with your Super Dragon Fist", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "$n misses you with $s Super Dragon Fist.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n missed $N with the Super Dragon Fist.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_super_dragonfist );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_super_dragonfist]->min_mana;
  return;
}


void do_death_ball( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_death_ball ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_death_ball]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_death_ball]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_death_ball]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_death_ball]->focus;

  WAIT_STATE( ch, skill_table[gsn_death_ball]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_death_ball ) )
  {
    switch ( number_range( 1, 100 ) )
    {
      case 100:
      case 99:
        if( IS_NPC( victim ) || ( !IS_NPC( victim ) && ( !xIS_SET( ch->act, PLR_SPAR ) || !xIS_SET( ch->act, PLR_SPAR ) ) ) )
        {

          /*
           * Redone so it can't be used to kill players
           * * above 5x their pl. Was going to do 2x like
           * * DDD, but if I did that, it'd remove the challenge
           * * from Coolers. -- Islvin
           */
          if( victim->pl / ch->pl >= 2 )
          {
            dam = get_attmod( ch, victim ) * number_range( 5, 6 );
            act( AT_RED, "You charge a huge ball of energy on the tip of your finger and then direct it quickly towards $n.",
                 ch, NULL, victim, TO_CHAR );
            act( AT_RED, "$n is enveloped by the death ball, but when all is done, seems unaffected.", ch, NULL, victim,
                 TO_CHAR );
            act( AT_RED, "$n charges a huge ball of energy on the tip of $s finger and then directs it quickly towards you.",
                 ch, NULL, victim, TO_VICT );
            act( AT_RED, "You are enveloped by the death ball, but seem unaffected when it's over.", ch, NULL, victim,
                 TO_VICT );
            act( AT_RED, "$n charges a huge ball of energy on the tip of $s finger and then directs it quickly towards $N.",
                 ch, NULL, victim, TO_NOTVICT );
            act( AT_RED, "$N is enveloped by the death ball, but when all is done, seems unaffected.", ch, NULL, victim,
                 TO_NOTVICT );
            learn_from_success( ch, gsn_death_ball );
            global_retcode = damage( ch, victim, dam, TYPE_HIT );
            break;
          }

/* Redo damage so it _is_ instant death
 * Melora
 * 	if (victim->max_hit >= victim->hit)
 * 		dam = victim->max_hit * 2;
 * 	else
 * 		dam = victim->hit * 2;
 */
          dam = 999999999;
          act( AT_RED, "You charge a huge ball of energy on the tip of your finger and then direct it quickly towards $N.",
               ch, NULL, victim, TO_CHAR );
          act( AT_RED, "$N is slowly enveloped by the death ball, killing $M instantly.", ch, NULL, victim, TO_CHAR );
          act( AT_RED, "$n charges a huge ball of energy on the tip of $s finger and then directs it quickly towards you.",
               ch, NULL, victim, TO_VICT );
          act( AT_RED, "You are slowly enveloped by the death ball, killing you instantly.", ch, NULL, victim, TO_VICT );
          act( AT_RED, "$n charges a huge ball of energy on the tip of $s finger and then directs it quickly towards $N.",
               ch, NULL, victim, TO_NOTVICT );
          act( AT_RED, "$N is slowly enveloped by the death ball, killing $M instantly.", ch, NULL, victim, TO_NOTVICT );

          learn_from_success( ch, gsn_death_ball );
          global_retcode = damage( ch, victim, dam, TYPE_HIT );
          break;
        }
      default:
        dam = get_attmod( ch, victim ) * number_range( 55, 65 );
        if( ch->charge > 0 )
          dam = chargeDamMult( ch, dam );
        act( AT_RED,
             "You raise your arm to the heavens, palm open. A swirling vortex of hellish light gathers into a ball, hovering lifelessly above. You point, effortlessly, at $N sending them to their impending demise. &W[$t]",
             ch, num_punct( dam ), victim, TO_CHAR );
        act( AT_RED,
             "$n raises $m arm to the heavens, palm open. A swirling vortex of hellish light gathers into a ball, hovering lifelessly above. $n points effortlessly, sending you to your impending demise. &W[$t]",
             ch, num_punct( dam ), victim, TO_VICT );
        act( AT_RED,
             "$n raises $m arm to the heavens, palm open. A swirling vortex of hellish light gathers into a ball, hovering lifelessly above. $n points effortlessly, at $N sending them to their impending demise. &W[$t]",
             ch, num_punct( dam ), victim, TO_NOTVICT );

        learn_from_success( ch, gsn_death_ball );
        global_retcode = damage( ch, victim, dam, TYPE_HIT );
        break;
    }
  }
  else
  {
    act( AT_RED, "You missed $N with your death ball.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n misses you with $s death ball.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n missed $N with a death ball.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_death_ball );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_death_ball]->min_mana;
  return;
}


void do_super_nova( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_super_nova ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_super_nova]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_super_nova]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_super_nova]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_super_nova]->focus;

  WAIT_STATE( ch, skill_table[gsn_super_nova]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_super_nova ) )
  {
    switch ( number_range( 1, 100 ) )
    {
      case 100:
      case 95:
        if( IS_NPC( victim ) || ( !IS_NPC( victim ) && ( !xIS_SET( ch->act, PLR_SPAR ) || !xIS_SET( ch->act, PLR_SPAR ) ) ) )
        {

          /*
           * Redone so it can't be used to kill players
           * * above 5x their pl. Was going to do 2x like
           * * DDD, but if I did that, it'd remove the challenge
           * * from Coolers. -- Islvin
           */
          if( victim->pl / ch->pl >= 2 )
          {
            dam = get_attmod( ch, victim ) * number_range( 10, 11 );
            act( AT_RED, "The sky somewhat darkens as you call the necesary energy for your Super Nova.", ch, NULL, victim,
                 TO_CHAR );
            act( AT_RED, "Raising one finger into the air a gigantic ball of orange ki forms above your head.", ch, NULL,
                 victim, TO_CHAR );
            act( AT_RED,
                 "Spotting your target you send it flying off at $N. Coliding with $N, the ball of ki explodes with the force of a Super Nova giving credit to it's name. ",
                 ch, NULL, victim, TO_CHAR );
            act( AT_RED, "$N is enveloped by the Super Nova, but when all is done, seems unaffected.", ch, NULL, victim,
                 TO_CHAR );


            act( AT_RED, "The sky somewhat darkens as $n call the necesary energy for $s Super Nova.", ch, NULL, victim,
                 TO_VICT );
            act( AT_RED, "Raising one finger into the air a gigantic ball of orange ki forms above $s head.", ch, NULL,
                 victim, TO_VICT );
            act( AT_RED,
                 "Spotting $s target $n send it flying off at you. Coliding with you, the ball of ki explodes with the force of a Super Nova giving credit to it's name. ",
                 ch, NULL, victim, TO_VICT );
            act( AT_RED, "You are enveloped by the Super Nova, but when all is done, seems unaffected.", ch, NULL, victim,
                 TO_VICT );

            act( AT_RED, "The sky somewhat darkens as $n call the necesary energy for $s Super Nova.", ch, NULL, victim,
                 TO_NOTVICT );
            act( AT_RED, "Raising one finger into the air a gigantic ball of orange ki forms above $s head.", ch, NULL,
                 victim, TO_NOTVICT );
            act( AT_RED,
                 "Spotting $s target $n send it flying off at $N. Coliding with $N, the ball of ki explodes with the force of a Super Nova giving credit to it's name. ",
                 ch, NULL, victim, TO_NOTVICT );
            act( AT_RED, "$N is enveloped by the Super Nova, but when all is done, seems unaffected.", ch, NULL, victim,
                 TO_NOTVICT );
            learn_from_success( ch, gsn_super_nova );
            global_retcode = damage( ch, victim, dam, TYPE_HIT );
            break;
          }

/* Redo damage so it _is_ instant death
 * Melora
 * 	if (victim->max_hit >= victim->hit)
 * 		dam = victim->max_hit * 2;
 * 	else
 * 		dam = victim->hit * 2;
 */
          dam = 999999999;
          act( AT_RED, "The sky somewhat darkens as you call the necesary energy for your Super Nova.", ch, NULL, victim,
               TO_CHAR );
          act( AT_RED, "Raising one finger into the air a gigantic ball of orange ki forms above your head.", ch, NULL,
               victim, TO_CHAR );
          act( AT_RED,
               "Spotting your target you send it flying off at $N. Coliding with $N, the ball of ki explodes with the force of a Super Nova giving credit to it's name. ",
               ch, NULL, victim, TO_CHAR );
          act( AT_RED, "$N is slowly enveloped by the Super Nova attack, killing $M instantly.", ch, NULL, victim, TO_CHAR );

          act( AT_RED, "The sky somewhat darkens as $n call the necesary energy for $s Super Nova.", ch, NULL, victim,
               TO_VICT );
          act( AT_RED, "Raising one finger into the air a gigantic ball of orange ki forms above $s head.", ch, NULL, victim,
               TO_VICT );
          act( AT_RED,
               "Spotting $s target $n send it flying off at you. Coliding with you, the ball of ki explodes with the force of a Super Nova giving credit to it's name. ",
               ch, NULL, victim, TO_VICT );
          act( AT_RED, "You are slowly enveloped by the Super Nova attack, killing you instantly.", ch, NULL, victim,
               TO_VICT );

          act( AT_RED, "The sky somewhat darkens as $n call the necesary energy for $s Super Nova.", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_RED, "Raising one finger into the air a gigantic ball of orange ki forms above $s head.", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_RED,
               "Spotting $s target $n send it flying off at $N. Coliding with $N, the ball of ki explodes with the force of a Super Nova giving credit to it's name. ",
               ch, NULL, victim, TO_NOTVICT );
          act( AT_RED, "$N is slowly enveloped by the Super Nova attack, killing $M instantly.", ch, NULL, victim,
               TO_NOTVICT );

          learn_from_success( ch, gsn_super_nova );
          global_retcode = damage( ch, victim, dam, TYPE_HIT );
          break;
        }
      default:
        dam = get_attmod( ch, victim ) * number_range( 70, 80 );
        if( ch->charge > 0 )
          dam = chargeDamMult( ch, dam );

        act( AT_RED, "The sky somewhat darkens as you call the necesary energy for your Super Nova.", ch, NULL, victim,
             TO_CHAR );
        act( AT_RED, "Raising one finger into the air a gigantic ball of orange ki forms above your head.", ch, NULL, victim,
             TO_CHAR );
        act( AT_RED,
             "Spotting your target you send it flying off at $N. Coliding with $N, the ball of ki explodes with the force of a Super Nova giving credit to it's name. &W[$t]",
             ch, num_punct( dam ), victim, TO_CHAR );

        act( AT_RED, "The sky somewhat darkens as $n call the necesary energy for $s Super Nova.", ch, NULL, victim,
             TO_VICT );
        act( AT_RED, "Raising one finger into the air a gigantic ball of orange ki forms above $s head.", ch, NULL, victim,
             TO_VICT );
        act( AT_RED,
             "Spotting $s target $n send it flying off at you. Coliding with you, the ball of ki explodes with the force of a Super Nova giving credit to it's name. &W[$t]",
             ch, num_punct( dam ), victim, TO_VICT );

        act( AT_RED, "The sky somewhat darkens as $n call the necesary energy for $s Super Nova.", ch, NULL, victim,
             TO_NOTVICT );
        act( AT_RED, "Raising one finger into the air a gigantic ball of orange ki forms above $s head.", ch, NULL, victim,
             TO_NOTVICT );
        act( AT_RED,
             "Spotting $s target $n send it flying off at $N. Coliding with $N, the ball of ki explodes with the force of a Super Nova giving credit to it's name. &W[$t]",
             ch, num_punct( dam ), victim, TO_NOTVICT );

        learn_from_success( ch, gsn_super_nova );
        global_retcode = damage( ch, victim, dam, TYPE_HIT );
        break;
    }
  }
  else
  {
    act( AT_RED, "You missed $N with your Super Nova.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n misses you with $s Super Nova.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n missed $N with a Super Nova.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_super_nova );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_super_nova]->min_mana;
  return;
}



void do_eye_beam( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;
  int dam2 = 0;
  int dam3 = get_curr_str( ch ) / 100;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_eye_beam ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_eye_beam]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_eye_beam]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_eye_beam]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_eye_beam]->focus;

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_eye_beam]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_eye_beam ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 13, 15 );
    dam2 = get_attmod( ch, victim ) * number_range( 12, 15 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( z, "Your eyes begin to glow as two beams shoot out from them at $N. &W[$t]", ch, num_punct( dam ), victim,
         TO_CHAR );
    act( z, "$n's eyes begin to glow as two beams shoot out from them at you. &W[$t]", ch, num_punct( dam ), victim,
         TO_VICT );
    act( z, "$n's eyes begin to glow as two beams shoot out from them at $N. &W[$t]", ch, num_punct( dam ), victim,
         TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_eye_beam );
    learn_from_success( ch, gsn_eye_beam );
    dam += dam3;
    dam2 += dam3;
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
    global_retcode = damage( ch, victim, dam2, TYPE_HIT );
  }
  else
  {
    act( z, "You missed $N with your eye beam.", ch, NULL, victim, TO_CHAR );
    act( z, "$n misses you with $s eye beam.", ch, NULL, victim, TO_VICT );
    act( z, "$n missed $N with a eye beam.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_eye_beam );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_eye_beam]->min_mana;
  return;
}

void do_finger_beam( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_finger_beam ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_finger_beam]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_finger_beam]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_finger_beam]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_finger_beam]->focus;

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_finger_beam]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_finger_beam ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 16, 22 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( z, "You point your hand towards $N, firing a beam of energy from the tip of your finger. &W[$t]", ch,
         num_punct( dam ), victim, TO_CHAR );
    act( z, "$n points $s hand towards you, firing a beam of energy from the tip of $s finger. &W[$t]", ch, num_punct( dam ),
         victim, TO_VICT );
    act( z, "$n points $s hand towards $N, firing a beam of energy from the tip of $s finger. &W[$t]", ch, num_punct( dam ),
         victim, TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_finger_beam );
    learn_from_success( ch, gsn_finger_beam );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( z, "You missed $N with your finger beam.", ch, NULL, victim, TO_CHAR );
    act( z, "$n misses you with $s finger beam.", ch, NULL, victim, TO_VICT );
    act( z, "$n missed $N with a finger beam.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_finger_beam );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_finger_beam]->min_mana;
  return;
}

void do_tribeam( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_tribeam ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_tribeam]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_tribeam]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_tribeam]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_tribeam]->focus;

  WAIT_STATE( ch, skill_table[gsn_tribeam]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_tribeam ) )
  {
    /*
     * dam = get_attmod(ch, victim) * number_range( 34, 40 );
     */
    dam = get_attmod( ch, victim ) * number_range( 110, 150 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
/*	act( AT_SKILL, "You form your fingers into a triangle, spotting $N in your sights before pummeling $M with a powerfull beam of inner energy. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
 *	act( AT_SKILL, "$n forms $s fingers into a triangle, spotting you in $s sights before pummeling you with a powerfull beam of inner energy. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
 *	act( AT_SKILL, "$n forms $s fingers into a triangle, spotting $N in $s sights before pummeling $M with a powerfull beam of inner energy. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT ); */

    act( AT_YELLOW, "You form your fingers and thumbs into a triangle, "
         "lining up $N in your sights.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "Powerful inner energy gathers in the space between your hands, "
         "blasting out to pummel $N. &W[$t]", ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_YELLOW, "$n forms $s fingers and thumbs into a triangle, "
         "lining you up in $s sights.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "Powerful inner energy gathers in the space between $s hands, "
         "blasting out to pummel you. &W[$t]", ch, num_punct( dam ), victim, TO_VICT );
    act( AT_YELLOW, "$n forms $s fingers and thumbs into a triangle, "
         "lining up $N in $s sights.", ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW, "Powerful inner energy gathers in the space between $s hands, "
         "blasting out to pummel $N. &W[$t]", ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_tribeam );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_YELLOW, "You missed $N with your tri-beam attack.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "$n misses you with $s tri-beam attack.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n missed $N with a tri-beam attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_tribeam );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_tribeam]->min_mana;
  return;
}



void do_kakusandan( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_kakusandan ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_kakusandan]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_kakusandan]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_kakusandan]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_kakusandan]->focus;

  WAIT_STATE( ch, skill_table[gsn_kakusandan]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_kakusandan ) )
  {
    /*
     * dam = get_attmod(ch, victim) * number_range( 34, 40 );
     */
    dam = get_attmod( ch, victim ) * number_range( 90, 120 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
/*	act( AT_SKILL, "You form your fingers into a triangle, spotting $N in your sights before pummeling $M with a powerfull beam of inner energy. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
 *	act( AT_SKILL, "$n forms $s fingers into a triangle, spotting you in $s sights before pummeling you with a powerfull beam of inner energy. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
 *	act( AT_SKILL, "$n forms $s fingers into a triangle, spotting $N in $s sights before pummeling $M with a powerfull beam of inner energy. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT ); */

  if( IS_VIDEL(ch) )
  {
      act( AT_PINK, "You shove both palms forward and stretch all fingers apart widely.", ch, NULL, victim, TO_CHAR );
    act( AT_PINK, "A mist of spiraling energy emerges from your hands and two blasts of ki erupt into the air.", ch, NULL,
         victim, TO_CHAR );
    act( AT_PINK,
         "Driving you back slightly, you retain your footing and converge your palms together which result in the two beams of energy coinciding into one.",
         ch, NULL, victim, TO_CHAR );
    act( AT_PINK, "Then in a snap, the beam shatters into an abundant amount of ki bolts that rain down on $N &W[$t]", ch,
         num_punct( dam ), victim, TO_CHAR );


    act( AT_PINK, "$n shoves both palms forward and stretch all fingers apart widely.", ch, NULL, victim, TO_VICT );
    act( AT_PINK, "A mist of spiraling energy emerges from $s hands and two blasts of ki erupt into the air.", ch, NULL,
         victim, TO_VICT );
    act( AT_PINK,
         "Driving $n back slightly, $n retaines $s footing and converge $s palms together which result in the two beams of energy coinciding into one.",
         ch, NULL, victim, TO_VICT );
    act( AT_PINK, "Then in a snap, the beam shatters into an abundant amount of ki bolts that rain down on you &W[$t]", ch,
         num_punct( dam ), victim, TO_VICT );


    act( AT_PINK, "$n shoves both palms forward and stretch all fingers apart widely.", ch, NULL, victim, TO_NOTVICT );
    act( AT_PINK, "A mist of spiraling energy emerges from $s hands and two blasts of ki erupt into the air.", ch, NULL,
         victim, TO_NOTVICT );
    act( AT_PINK,
         "Driving $n back slightly, $n retaines $s footing and converge $s palms together which result in the two beams of energy coinciding into one.",
         ch, NULL, victim, TO_NOTVICT );
    act( AT_PINK, "Then in a snap, the beam shatters into an abundant amount of ki bolts that rain down on $N &W[$t]", ch,
         num_punct( dam ), victim, TO_NOTVICT );
  }
  else
  {
    act( AT_YELLOW, "You shove both palms forward and stretch all fingers apart widely.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "A mist of spiraling energy emerges from your hands and two blasts of ki erupt into the air.", ch, NULL,
         victim, TO_CHAR );
    act( AT_YELLOW,
         "Driving you back slightly, you retain your footing and converge your palms together which result in the two beams of energy coinciding into one.",
         ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "Then in a snap, the beam shatters into an abundant amount of ki bolts that rain down on $N &W[$t]", ch,
         num_punct( dam ), victim, TO_CHAR );


    act( AT_YELLOW, "$n shoves both palms forward and stretch all fingers apart widely.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "A mist of spiraling energy emerges from $s hands and two blasts of ki erupt into the air.", ch, NULL,
         victim, TO_VICT );
    act( AT_YELLOW,
         "Driving $n back slightly, $n retaines $s footing and converge $s palms together which result in the two beams of energy coinciding into one.",
         ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "Then in a snap, the beam shatters into an abundant amount of ki bolts that rain down on you &W[$t]", ch,
         num_punct( dam ), victim, TO_VICT );


    act( AT_YELLOW, "$n shoves both palms forward and stretch all fingers apart widely.", ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW, "A mist of spiraling energy emerges from $s hands and two blasts of ki erupt into the air.", ch, NULL,
         victim, TO_NOTVICT );
    act( AT_YELLOW,
         "Driving $n back slightly, $n retaines $s footing and converge $s palms together which result in the two beams of energy coinciding into one.",
         ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW, "Then in a snap, the beam shatters into an abundant amount of ki bolts that rain down on $N &W[$t]", ch,
         num_punct( dam ), victim, TO_NOTVICT );
  }



    /*
     * act( AT_YELLOW, "$n forms $s fingers and thumbs into a triangle, "
     * "lining you up in $s sights.", ch, NULL, victim, TO_VICT );
     * act( AT_YELLOW, "Powerful inner energy gathers in the space between $s hands, "
     * "blasting out to pummel you. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
     * act( AT_YELLOW, "$n forms $s fingers and thumbs into a triangle, "
     * "lining up $N in $s sights.", ch, NULL, victim, TO_NOTVICT );
     * act( AT_YELLOW, "Powerful inner energy gathers in the space between $s hands, "
     * "blasting out to pummel $N. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );
     */

    learn_from_success( ch, gsn_kakusandan );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_YELLOW, "You missed $N with your Kakusandan attack.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "$n misses you with $s Kakusandan attack.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n missed $N with a Kakusandan attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_kakusandan );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_kakusandan]->min_mana;
  return;
}


void do_solar_flare( CHAR_DATA * ch, char *argument )
{
  AFFECT_DATA af;
  CHAR_DATA *vch, *vch_next;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_solar_flare ) )
      return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_solar_flare]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }
  if( xIS_SET( ch->in_room->room_flags, ROOM_ARENA ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
  {
    send_to_char( "There's no need to use that here.\n\r", ch );
    return;
  }
  if( who_fighting( ch ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }
  if( ch->mana < skill_table[gsn_solar_flare]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_solar_flare]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_solar_flare]->focus;

  WAIT_STATE( ch, skill_table[gsn_solar_flare]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_solar_flare ) )
  {
    act( AT_WHITE, "You put your hands to your face and yell out 'SOLAR FLARE!' emitting a blinding flash of light.", ch,
         NULL, NULL, TO_CHAR );
    act( AT_WHITE, "$n puts $s hands to $s face and yells out 'SOLAR FLARE!' blinding everyone in the room!", ch, NULL, NULL,
         TO_NOTVICT );

    learn_from_success( ch, gsn_solar_flare );
    for( vch = ch->in_room->first_person; vch; vch = vch_next )
    {

      vch_next = vch->next_in_room;
      if( !IS_AWAKE( vch ) )
        continue;

      if( number_range( 1, 100 ) < ( get_curr_int( vch ) / 10 ) )
      {
        send_to_char( "You manage to look away at the last moment and shield " "your eyes.\n\r", vch );
        continue;
      }

      if( ch != vch )
      {
        af.type = gsn_solar_flare;
        af.duration = 10;
        af.location = APPLY_HITROLL;
        af.modifier = -6;
        af.bitvector = meb( AFF_BLIND );
        affect_to_char( vch, &af );
      }

    }

  }
  else
  {
    act( AT_WHITE, "You put your hands to your face but forget the words.  Nothing happens.", ch, NULL, NULL, TO_CHAR );
    act( AT_WHITE, "$n raises $s hands to $s head, gets a stupid look on $s face, then stands there.", ch, NULL, NULL,
         TO_NOTVICT );
    learn_from_failure( ch, gsn_solar_flare );
  }
  ch->mana -= skill_table[gsn_solar_flare]->min_mana;
  return;
}

void do_hyper( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_hyper ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_HYPER ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_HYPER );
    ch->pl = ch->exp;
    transStatRemove( ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_EXTREME ) )
  {
    send_to_char( "You can't use the hyper technique while using extreme.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You can't use the hyper technique while using kaioken.\n\r", ch );
    return;
  }
  if( ch->mana < skill_table[gsn_hyper]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_hyper]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_hyper ) )
  {
    act( z, "You clench your fists and yell out as all your muscles bulge and your aura explodes outward.", ch, NULL, NULL,
         TO_CHAR );
    act( z, "$n clenches $s fists and yells out as all of $s muscles bulge and $s aura explodes outward.", ch, NULL, NULL,
         TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_HYPER );
    if( xIS_SET( ( ch )->affected_by, AFF_HEART ) )
      xREMOVE_BIT( ch->affected_by, AFF_HEART );
    ch->pl = ch->exp * 20;
    transStatApply( ch, 26, 20, 14, 32 );

    heart_calc( ch, "" );
    learn_from_success( ch, gsn_hyper );
  }
  else
  {
    act( z, "You clench your fists and yell out, but nothing happens.", ch, NULL, NULL, TO_CHAR );
    act( z, "$n clenchs $s fists and yells out, but nothing happens.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_hyper );
  }

  ch->mana -= skill_table[gsn_hyper]->min_mana;
  return;
}

void do_instant_trans( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  CHAR_DATA *fch;
  CHAR_DATA *fch_next;
  ROOM_INDEX_DATA *prev_room;

//  if( !str_cmp( get_race( ch ), "kaio" ) || !str_cmp( get_race( ch ), "yardratian" ) )
  if( IS_KAIO( ch ) || is_yardratian( ch ) )
  {
    if( ch->mana < ( skill_table[gsn_instant_trans]->min_mana / 5 ) )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }
  }
  else
  {
    if( ch->mana < skill_table[gsn_instant_trans]->min_mana )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }
  }

  if( IS_NPC( ch ) )
    return;

  if( ( is_android( ch ) || is_superandroid( ch ) ) && !wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "Huh?\n\r" );
    return;
  }

  if( ch->fighting )
  {
    send_to_char( "You can't find enough time to concentrate.\n\r", ch );
    return;
  }

  if( xIS_SET( ch->in_room->room_flags, ROOM_ASTRALSHIELD ) )
  {
    ch_printf( ch, "&RThis room is blocking your ability to de-materialize.\n\r" );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_instant_trans]->beats );

  if( ( victim = get_char_ki_world( ch, argument ) ) == NULL
      || !can_astral( ch, victim )
      || !in_hard_range( ch, victim->in_room->area )
      || !in_soft_range( ch, victim->in_room->area )
      || xIS_SET( victim->in_room->room_flags, ROOM_PROTOTYPE )
      || ( is_android( victim ) && !wearing_sentient_chip( victim ) )
      || ( is_superandroid( victim ) && !wearing_sentient_chip( victim ) ) || ( victim->rank > 0 ) || IS_IMMORTAL( victim ) )
  {
    send_to_char( "You can't seem to sense their energy anywhere.\n\r", ch );
    return;
  }

  if( is_split( victim ) )
  {
    send_to_char( "You can't seem to sense their energy anywhere.\n\r", ch );
    return;
  }

  if( !IS_NPC( victim ) && !victim->desc )
  {
    send_to_char( "You cannot IT to someone who has lost link.\n\r", ch );
    return;
  }

  if( !victim->in_room )
  {
    ch_printf( ch, "You can't IT to them at the moment.\n\r" );
    return;
  }

  if( ch->in_room == victim->in_room )
  {
    ch_printf( ch, "You're already there!\n\r" );
    return;
  }
  if( victim->master == ch )
  {
    ch_printf( ch, "You can't IT to someone that is following you!\n\r" );
    return;
  }

  if( can_use_skill( ch, number_percent(  ), gsn_instant_trans ) )
  {
    learn_from_success( ch, gsn_instant_trans );
    act( AT_MAGIC, "You disappear in a flash of light!", ch, NULL, NULL, TO_CHAR );
    act( AT_MAGIC, "$n disappears in a flash of light!", ch, NULL, NULL, TO_ROOM );
    prev_room = ch->in_room;
    char_from_room( ch );
    if( ch->mount )
    {
      char_from_room( ch->mount );
      char_to_room( ch->mount, victim->in_room );
    }
    char_to_room( ch, victim->in_room );
    for( fch = prev_room->first_person; fch; fch = fch_next )
    {
      fch_next = fch->next_in_room;
      if( ( fch != ch ) && fch->master && fch->master == ch )
      {
        char_from_room( fch );
        if( !is_leet( fch ) )
        {
          act( AT_ACTION, "You disappear in a flash of light!", fch, NULL, ch, TO_CHAR );
          act( AT_ACTION, "$n disappears in a flash of light!", fch, NULL, ch, TO_ROOM );
        }
        else
        {
          act( AT_ACTION, "You disappear in a flash of light! Omigawd u hax0r!", fch, NULL, ch, TO_CHAR );
          act( AT_ACTION, "$n disappears in a flash of light! OMGHAX!!!", fch, NULL, ch, TO_ROOM );
        }
        char_to_room( fch, victim->in_room );
        act( AT_MAGIC, "You appear in a flash of light!", fch, NULL, ch, TO_CHAR );
        act( AT_MAGIC, "$n appears in a flash of light!", fch, NULL, ch, TO_ROOM );
        do_look( fch, "auto" );
      }
    }
    act( AT_MAGIC, "You appear in a flash of light!", ch, NULL, NULL, TO_CHAR );
    act( AT_MAGIC, "$n appears in a flash of light!", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
  }
  else
  {
    learn_from_failure( ch, gsn_instant_trans );
    send_to_char( "&BYou can't seem to sense their energy anywhere.\n\r", ch );
  }
  if( !is_android_h( ch ) )
  {
    if( IS_KAIO( ch ) || is_yardratian( ch ) )
      ch->mana -= ( skill_table[gsn_instant_trans]->min_mana / 5 );
    else
      ch->mana -= skill_table[gsn_instant_trans]->min_mana;
  }
  return;

}

void chargeDrain( CHAR_DATA * ch )
{
  SKILLTYPE *skill = skill_table[ch->skillGsn];

  if( !skill )
  {
    bug( "chargeDrain: no skill gsn. %s (sn %d)", ch->name, ch->skillGsn );
    return;
  }


  if( !is_android_h( ch ) )
  {
    ch->mana -= skill->min_mana;
    ch->mana = URANGE( 0, ch->mana, ch->max_mana );
  }
  ch->focus -= skill->focus / 2;
  ch->focus = URANGE( 0, ch->focus, get_curr_int( ch ) );
  return;
}
void do_charge( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  SKILLTYPE *skill = NULL;
  int chargeGsn = 0;

  if( IS_NPC( ch ) )
    return;

  if( ch->pcdata->learned[gsn_charge2] <= 0 )
  {
    send_to_char( "&wYou must learn charge level 1 before you can charge attacks.\n\r", ch );
    return;
  }

  if( !ch->fighting && ch->charge > 0 )
  {
    send_to_char( "&wHmm... you seem to have lost your victim...\n\r", ch );
    ch->substate = SUB_NONE;
    ch->skillGsn = -1;
    ch->charge = 0;
    ch->timerDelay = 0;
    ch->timerType = 0;
    ch->timerDo_fun = NULL;
    return;
  }

  if( !ch->fighting )
  {
    send_to_char( "&wYou aren't fighting anything.\n\r", ch );
    ch->substate = SUB_NONE;
    ch->skillGsn = -1;
    ch->charge = 0;
    ch->timerDelay = 0;
    ch->timerType = 0;
    ch->timerDo_fun = NULL;
    return;
  }

  one_argument( argument, arg );

  switch ( ch->charge )
  {
    case 0:
      chargeGsn = gsn_charge2;
      break;
    case 1:
      chargeGsn = gsn_charge2;
      break;
    case 2:
      chargeGsn = gsn_charge3;
      break;
    case 3:
      chargeGsn = gsn_charge4;
      break;
    case 4:
      chargeGsn = gsn_charge5;
      break;
    default:
      chargeGsn = -1;
      break;
  }

  /*
   * energy and focus checks for charge and the attack 
   */

  switch ( ch->substate )
  {
    default:
      bug( "do_charge: illegal substate" );
      return;

    case SUB_NONE:
      if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
      {
        send_to_char( "&wYou can't concentrate enough for that.\n\r", ch );
        return;
      }
      if( ch->mount )
      {
        send_to_char( "&wYou can't do that while mounted.\n\r", ch );
        return;
      }
      if( ch->position == POS_AGGRESSIVE || ch->position == POS_BERSERK )
      {
        send_to_char( "&wYou can't concentrate enough for that.\n\r", ch );
        return;
      }

      ch->skillGsn = skill_lookup( arg );
      if( ch->skillGsn > 0 && ( ( ch->pcdata && ch->pcdata->learned[ch->skillGsn] > 0 ) ) )
      {
        skill = skill_table[ch->skillGsn];
      }
      else
      {
        send_to_char( "&wYou don't know any skill like that.\n\r", ch );
        return;
      }

      if( skill->canCharge == 0 )
      {
        send_to_char( "&wYou can't charge that.\n\r", ch );
        return;
      }

      if( ch->mana < ( skill->min_mana * 2 ) )
      {
        send_to_char( "&wYou need more energy before you can charge that.\n\r", ch );
        return;
      }
      if( ch->focus < ( skill->focus * 1.5 ) )
      {
        send_to_char( "&wYou need to focus more before you can charge that.\n\r", ch );
        return;
      }

      WAIT_STATE( ch, 10 );
      ch->charge = 0;
      learn_from_success( ch, chargeGsn );
      ch->substate = SUB_CMD_LOOP_TIMER;
      ch->timerDelay = number_range( 1, 2 );
      ch->timerType = 1;
      ch->timerDo_fun = do_charge;
      pager_printf( ch, "&wYou begin to charge your %s...\n\r", skill->name );
      act( AT_PLAIN, "$n begins to charge up an attack...", ch, NULL, NULL, TO_ROOM );
      return;

    case SUB_CMD_LOOP_TIMER:
      skill = skill_table[ch->skillGsn];
      if( chargeGsn != -1 && ch->pcdata->learned[chargeGsn] > 0
          && can_use_skill( ch, number_percent(  ), chargeGsn )
          && ch->mana >= ( skill->min_mana * 2 ) && ch->focus >= ( skill->focus * 1.5 ) )
      {
        pager_printf( ch, "&wYou channel more energy in to your %s.\n\r", skill_table[ch->skillGsn]->name );
        act( AT_PLAIN, "$n continues charging up $s attack.", ch, NULL, NULL, TO_ROOM );
        learn_from_success( ch, chargeGsn );
        /*
         * subtract focus and energy 
         */
        ch->charge++;
        chargeDrain( ch );
        ch->timerDelay = number_range( 1, 2 );
        ch->timerType = 1;
        ch->timerDo_fun = do_charge;
        return;
      }
      else
      {
        if( ch->mana < skill->min_mana )
          pager_printf( ch, "&wYou finish channeling all your available energy in to %s, and fire it!\n\r",
                        skill_table[ch->skillGsn]->name );
        else if( ch->mana < skill->min_mana )
          pager_printf( ch, "&wYou lose your focus and %s fires!\n\r", skill_table[ch->skillGsn]->name );
        else
          pager_printf( ch, "&wYou can't control your %s any more and it fires!\n\r", skill_table[ch->skillGsn]->name );
        if( chargeGsn != -1 )
          learn_from_failure( ch, chargeGsn );
        /*
         * do attack 
         */
        break;
      }

    case SUB_TIMER_DO_ABORT:
    {
      if( !ch->fighting )
      {
        bug( "skills_dbs:3749" );
        pager_printf( ch, "&wYou power down your %s since you're not fighting any more.", skill_table[ch->skillGsn]->name );
        ch->timerDelay = 0;
        ch->timerType = 0;
        ch->substate = SUB_NONE;
        ch->skillGsn = -1;
        ch->charge = 0;
        ch->timerDo_fun = NULL;
        return;
      }
      if( !ch->fighting->who )
        bug( "skills_dbs:3759" );
      if( !ch->fighting->who->name )
        bug( "skills_dbs:3761" );
      if( !ch->skillGsn )
        bug( "skills_dbs:3763" );
      if( !skill_table[ch->skillGsn]->name )
        bug( "skills_dbs:3765" );
/* There was a bug that people can exploit when killing mobs.
 * They could charge, then do 'k (mob)'.  It'd break the charge,
 * and immediately engage them in combat with the mob.  Mostly
 * used against fighters in COU, Coolers, and the larger auto-repop
 * mobs.
 * My fix for this created a bug where people could use a timer
 * to set themselves into a 'charge loop' so to speak.  Could just
 * keep doing 'charge (skill)' in a spar, and they'd never attack
 * their partner--very useful for letting someone spam, but also
 * enabled them to cheat.
 * So, if any more changes are made to this part, take this into
 * consideration.  --Islvin
 */
      if( xIS_SET( ch->act, PLR_SPAR ) )
      {
        pager_printf( ch, "&wYou fire your charged up %s at %s!\n\r", skill_table[ch->skillGsn]->name,
                      ch->fighting->who->name );
        if( chargeGsn != -1 )
          learn_from_success( ch, chargeGsn );
        break;
      }
      pager_printf( ch, "&wIn an effort to multitask, you fumble your charge.\n\r", skill_table[ch->skillGsn]->name );
      ch->timerDelay = 0;
      ch->timerType = 0;
      ch->substate = SUB_NONE;
      ch->skillGsn = -1;
      ch->charge = 0;
      ch->timerDo_fun = NULL;
      if( chargeGsn != -1 )
        learn_from_failure( ch, chargeGsn );
      return;

    }
  }

  /*
   * fire!!! 
   */
  ch->timerDelay = 0;
  ch->timerType = 0;

  ch->substate = SUB_NONE;
  skill = skill_table[ch->skillGsn];
  if( !check_skill( ch, skill_table[ch->skillGsn]->name, "" ) && !check_ability( ch, skill_table[ch->skillGsn]->name, "" ) )
  {
    bug( "Couldn't find called skill in do_charge. %s (sn %d)", ch->name, ch->skillGsn );
    pager_printf( ch, "&wYou forgot what you were charging...?\n\r" );
    act( AT_PLAIN, "$n forgot what $e was charging...?", ch, NULL, NULL, TO_ROOM );
    ch->skillGsn = -1;
    ch->charge = 0;
    return;
  }
  ch->skillGsn = -1;
  ch->charge = 0;
  ch->timerDo_fun = NULL;
  return;
}

ch_ret spell_sensu_bean( int sn, int level, CHAR_DATA * ch, void *vo )
{
  ch->hit = ch->max_hit;
  ch->mana += ch->max_mana * 0.25;
  ch->mana = URANGE( 0, ch->mana, ch->max_mana );
  act( AT_MAGIC, "A warm feeling fills your body.", ch, NULL, NULL, TO_CHAR );
  return rNONE;
}

ch_ret spell_sleep( int sn, int level, CHAR_DATA * ch, void *vo )
{
  AFFECT_DATA af;

  af.type = gsn_sleep;
  af.duration = number_range( 1, 20 );
  af.location = APPLY_NONE;
  af.bitvector = meb( AFF_SLEEP );
  af.modifier = -1;

  affect_to_char( ch, &af );
  act( AT_MAGIC, "An overwhelming urge to sleep washes over you.", ch, NULL, NULL, TO_CHAR );
  return rNONE;
}

ch_ret spell_poison( int sn, int level, CHAR_DATA * ch, void *vo )
{
  act( AT_MAGIC, "Your stomach aches and you feel ill.", ch, NULL, NULL, TO_CHAR );
  return rNONE;
}

ch_ret spell_paralyze( int sn, int level, CHAR_DATA * ch, void *vo )
{
  act( AT_MAGIC, "Your limbs begin to act sluggishly.", ch, NULL, NULL, TO_CHAR );
  return rNONE;
}


void do_multi_disk( CHAR_DATA * ch, char *arg )
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int shot = 0;
  int shots = 0;
  int dam = 0;
  int dam2 = 0;
  int energy = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_multi_disk ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_multi_disk]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( arg[0] == '\0' || atoi( arg ) <= 0 )
  {
    send_to_char( "Syntax: multi <# of Disks>\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  shot = atoi( arg );
  if( shot > 50 )
    shot = 50;
  if( shot < 2 )
    shot = 2;
  energy = shot * skill_table[gsn_multi_disk]->min_mana;
  shots = shot;
  strcpy( buf, num_punct( shot ) );

  if( ch->mana < energy )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < ( skill_table[gsn_multi_disk]->focus * ( 1 + ( shot / 10 ) ) ) )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= ( skill_table[gsn_multi_disk]->focus * ( 1 + ( shot / 10 ) ) );

  WAIT_STATE( ch, ( skill_table[gsn_multi_disk]->beats ) );

  if( shot >= 0 && shot <= 9 )
  {
    if( can_use_skill( ch, number_percent(  ), gsn_multi_disk ) )
    {
      switch ( number_range( 1, 100 ) )
      {
        case 100:
          if( IS_NPC( victim )
              || ( !IS_NPC( victim ) && ( !xIS_SET( ch->act, PLR_SPAR ) || !xIS_SET( ch->act, PLR_SPAR ) ) ) )
          {

            /*
             * Redone so that instant death can't be abused to instant kill
             * * players over 2x their power. -- Islvin
             */
            if( victim->pl / ch->pl >= 2 )
            {
              dam = get_attmod( ch, victim ) * number_range( 10, 13 );
              act( AT_YELLOW, "You raise your hand above your head, forming a disc of pure energy, and throw it at $N.", ch,
                   NULL, victim, TO_CHAR );
              act( AT_YELLOW,
                   "The disc flies straight toward $N, breaking apart into smaller pieces... but they're all absorbed by $s aura.",
                   ch, NULL, victim, TO_CHAR );
              act( AT_YELLOW, "$N raises $s hand above $s head, forming a disc of pure energy, and throws it at you.", ch,
                   NULL, victim, TO_VICT );
              act( AT_YELLOW,
                   "The disc flies straight toward you, breaking apart into smaller pieces... but they're all absorbed by your aura.",
                   ch, NULL, victim, TO_VICT );
              act( AT_YELLOW, "$n raises $s hand above $s head, forming a disc of pure energy, and throws it at $N.", ch,
                   NULL, victim, TO_NOTVICT );
              act( AT_YELLOW,
                   "The disc flies straight toward $N, breaking apart into smaller pieces... but they're all absorbed by $s aura.",
                   ch, NULL, victim, TO_NOTVICT );
              learn_from_success( ch, gsn_multi_disk );
              global_retcode = damage( ch, victim, dam, TYPE_HIT );
              break;
            }
            dam = 999999999;
            act( AT_YELLOW, "You raise your hand above your head, forming a disc of pure energy, and throw it at $N.", ch,
                 NULL, victim, TO_CHAR );
            act( AT_RED,
                 "The disc flies straight toward $N, breaking apart into smaller pieces which cut through $m, killing $m instantly!!",
                 ch, NULL, victim, TO_CHAR );
            act( AT_YELLOW, "$N raises $s hand above $s head, forming a disc of pure energy, and throws it at you.", ch,
                 NULL, victim, TO_VICT );
            act( AT_RED,
                 "The disc flies straight toward you, breaking apart into smaller pieces which cut through you, killing you instantly!!",
                 ch, NULL, victim, TO_VICT );
            act( AT_YELLOW, "$n raises $s hand above $s head, forming a disc of pure energy, and throws it at $N.", ch, NULL,
                 victim, TO_NOTVICT );
            act( AT_RED,
                 "The disc flies straight toward $N, breaking apart into smaller pieces which cut through $m, killing $m instantly!!",
                 ch, NULL, victim, TO_NOTVICT );

            learn_from_success( ch, gsn_multi_disk );
            global_retcode = damage( ch, victim, dam, TYPE_HIT );
            break;
          }
        default:
          while( shots > 0 )
          {
/* Changed to increase damage -- Islvin
		switch (number_range( 1, 4))
		{
			default:
			case 1:
				break;
			case 2:
			case 3:
				dam += number_range( 0, 1 );
				break;
			case 4:
				dam += number_range( 0, 2 );
				break;
		}
*/
            switch ( number_range( 1, 3 ) )
            {
              default:
              case 1:
                break;
              case 2:
                dam += number_range( 0, 1 );
                break;
              case 3:
                dam += number_range( 0, 2 );
                break;
            }

            shots--;
          }
          dam2 = number_range( 2, 5 );  // was 5-7 in the damage call
          strcpy( buf2, num_punct( get_attmod( ch, victim ) * dam * dam2 ) );

          act( AT_YELLOW, "You raise your hand above your head, palm up, and bring a crackling disk of", ch, NULL, victim,
               TO_CHAR );
          act( AT_YELLOW, "energy forth.  The disk hums as you eye $N and send it spinning $S way.", ch, NULL, victim,
               TO_CHAR );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim, TO_CHAR,
                NULL );
          act2( AT_YELLOW, "as they home in on $N. &W[$t]", ch, buf2, victim, TO_CHAR, NULL );
          act( AT_YELLOW, "$n raises $s hand above $s head, palm up, and brings a crackling disk of", ch, NULL, victim,
               TO_VICT );
          act( AT_YELLOW, "energy forth.  The disk hums as $e eyes you and sends it spinning your way.", ch, NULL, victim,
               TO_VICT );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim, TO_VICT,
                NULL );
          act2( AT_YELLOW, "as they home in on you. &W[$t]", ch, buf2, victim, TO_VICT, NULL );
          act( AT_YELLOW, "$n raises $s hand above $s head, palm up, and brings a crackling disk of", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_YELLOW, "energy forth.  The disk hums as $e eyes $N and sends it spinning $N's way.", ch, NULL, victim,
               TO_NOTVICT );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim,
                TO_NOTVICT, NULL );
          act2( AT_YELLOW, "as they home in on $N. &W[$t]", ch, buf2, victim, TO_NOTVICT, NULL );

          learn_from_success( ch, gsn_multi_disk );
          global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * dam * dam2 ), TYPE_HIT );
          break;
      }
    }
    else
    {
      act( AT_YELLOW, "You launch a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_CHAR );
      act( AT_YELLOW, "$n launches a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_VICT );
      act( AT_YELLOW, "$n launches a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_NOTVICT );
      learn_from_failure( ch, gsn_multi_disk );
      global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
  }
  if( shot >= 10 && shot <= 19 )
  {
    if( can_use_skill( ch, number_percent(  ), gsn_multi_disk ) )
    {
      switch ( number_range( 1, 100 ) )
      {
        case 100:
        case 99:
          if( IS_NPC( victim )
              || ( !IS_NPC( victim ) && ( !xIS_SET( ch->act, PLR_SPAR ) || !xIS_SET( ch->act, PLR_SPAR ) ) ) )
          {

            /*
             * Redone so that instant death can't be abused to instant kill
             * * players over 2x their power. -- Islvin
             */
            if( victim->pl / ch->pl >= 2 )
            {
              dam = get_attmod( ch, victim ) * number_range( 10, 13 );
              act( AT_YELLOW, "You raise your hand above your head, forming a disc of pure energy, and throw it at $N.", ch,
                   NULL, victim, TO_CHAR );
              act( AT_YELLOW,
                   "The disc flies straight toward $N, breaking apart into smaller pieces... but they're all absorbed by $s aura.",
                   ch, NULL, victim, TO_CHAR );
              act( AT_YELLOW, "$N raises $s hand above $s head, forming a disc of pure energy, and throws it at you.", ch,
                   NULL, victim, TO_VICT );
              act( AT_YELLOW,
                   "The disc flies straight toward you, breaking apart into smaller pieces... but they're all absorbed by your aura.",
                   ch, NULL, victim, TO_VICT );
              act( AT_YELLOW, "$n raises $s hand above $s head, forming a disc of pure energy, and throws it at $N.", ch,
                   NULL, victim, TO_NOTVICT );
              act( AT_YELLOW,
                   "The disc flies straight toward $N, breaking apart into smaller pieces... but they're all absorbed by $s aura.",
                   ch, NULL, victim, TO_NOTVICT );
              learn_from_success( ch, gsn_multi_disk );
              global_retcode = damage( ch, victim, dam, TYPE_HIT );
              break;
            }
            dam = 999999999;
            act( AT_YELLOW, "You raise your hand above your head, forming a disc of pure energy, and throw it at $N.", ch,
                 NULL, victim, TO_CHAR );
            act( AT_RED,
                 "The disc flies straight toward $N, breaking apart into smaller pieces which cut through $m, killing $m instantly!!",
                 ch, NULL, victim, TO_CHAR );
            act( AT_YELLOW, "$N raises $s hand above $s head, forming a disc of pure energy, and throws it at you.", ch,
                 NULL, victim, TO_VICT );
            act( AT_RED,
                 "The disc flies straight toward you, breaking apart into smaller pieces which cut through you, killing you instantly!!",
                 ch, NULL, victim, TO_VICT );
            act( AT_YELLOW, "$n raises $s hand above $s head, forming a disc of pure energy, and throws it at $N.", ch, NULL,
                 victim, TO_NOTVICT );
            act( AT_RED,
                 "The disc flies straight toward $N, breaking apart into smaller pieces which cut through $m, killing $m instantly!!",
                 ch, NULL, victim, TO_NOTVICT );

            learn_from_success( ch, gsn_multi_disk );
            global_retcode = damage( ch, victim, dam, TYPE_HIT );
            break;
          }
        default:
          while( shots > 0 )
          {
/* Changed to increase damage -- Islvin
		switch (number_range( 1, 4))
		{
			default:
			case 1:
				break;
			case 2:
			case 3:
				dam += number_range( 0, 1 );
				break;
			case 4:
				dam += number_range( 0, 2 );
				break;
		}
*/
            switch ( number_range( 1, 3 ) )
            {
              default:
              case 1:
                break;
              case 2:
                dam += number_range( 0, 1 );
                break;
              case 3:
                dam += number_range( 0, 2 );
                break;
            }

            shots--;
          }
          dam2 = number_range( 2, 5 );  // was 5-7 in the damage call
          strcpy( buf2, num_punct( get_attmod( ch, victim ) * dam * dam2 ) );

          act( AT_YELLOW, "You raise your hand above your head, palm up, and bring a crackling disk of", ch, NULL, victim,
               TO_CHAR );
          act( AT_YELLOW, "energy forth.  The disk hums as you eye $N and send it spinning $S way.", ch, NULL, victim,
               TO_CHAR );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim, TO_CHAR,
                NULL );
          act2( AT_YELLOW, "as they home in on $N. &W[$t]", ch, buf2, victim, TO_CHAR, NULL );
          act( AT_YELLOW, "$n raises $s hand above $s head, palm up, and brings a crackling disk of", ch, NULL, victim,
               TO_VICT );
          act( AT_YELLOW, "energy forth.  The disk hums as $e eyes you and sends it spinning your way.", ch, NULL, victim,
               TO_VICT );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim, TO_VICT,
                NULL );
          act2( AT_YELLOW, "as they home in on you. &W[$t]", ch, buf2, victim, TO_VICT, NULL );
          act( AT_YELLOW, "$n raises $s hand above $s head, palm up, and brings a crackling disk of", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_YELLOW, "energy forth.  The disk hums as $e eyes $N and sends it spinning $N's way.", ch, NULL, victim,
               TO_NOTVICT );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim,
                TO_NOTVICT, NULL );
          act2( AT_YELLOW, "as they home in on $N. &W[$t]", ch, buf2, victim, TO_NOTVICT, NULL );

          learn_from_success( ch, gsn_multi_disk );
          global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * dam * dam2 ), TYPE_HIT );
          break;
      }
    }
    else
    {
      act( AT_YELLOW, "You launch a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_CHAR );
      act( AT_YELLOW, "$n launches a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_VICT );
      act( AT_YELLOW, "$n launches a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_NOTVICT );
      learn_from_failure( ch, gsn_multi_disk );
      global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
  }
  if( shot >= 20 && shot <= 29 )
  {
    if( can_use_skill( ch, number_percent(  ), gsn_multi_disk ) )
    {
      switch ( number_range( 1, 100 ) )
      {
        case 100:
        case 99:
        case 98:
          if( IS_NPC( victim )
              || ( !IS_NPC( victim ) && ( !xIS_SET( ch->act, PLR_SPAR ) || !xIS_SET( ch->act, PLR_SPAR ) ) ) )
          {

            /*
             * Redone so that instant death can't be abused to instant kill
             * * players over 2x their power. -- Islvin
             */
            if( victim->pl / ch->pl >= 2 )
            {
              dam = get_attmod( ch, victim ) * number_range( 10, 13 );
              act( AT_YELLOW, "You raise your hand above your head, forming a disc of pure energy, and throw it at $N.", ch,
                   NULL, victim, TO_CHAR );
              act( AT_YELLOW,
                   "The disc flies straight toward $N, breaking apart into smaller pieces... but they're all absorbed by $s aura.",
                   ch, NULL, victim, TO_CHAR );
              act( AT_YELLOW, "$N raises $s hand above $s head, forming a disc of pure energy, and throws it at you.", ch,
                   NULL, victim, TO_VICT );
              act( AT_YELLOW,
                   "The disc flies straight toward you, breaking apart into smaller pieces... but they're all absorbed by your aura.",
                   ch, NULL, victim, TO_VICT );
              act( AT_YELLOW, "$n raises $s hand above $s head, forming a disc of pure energy, and throws it at $N.", ch,
                   NULL, victim, TO_NOTVICT );
              act( AT_YELLOW,
                   "The disc flies straight toward $N, breaking apart into smaller pieces... but they're all absorbed by $s aura.",
                   ch, NULL, victim, TO_NOTVICT );
              learn_from_success( ch, gsn_multi_disk );
              global_retcode = damage( ch, victim, dam, TYPE_HIT );
              break;
            }
            dam = 999999999;
            act( AT_YELLOW, "You raise your hand above your head, forming a disc of pure energy, and throw it at $N.", ch,
                 NULL, victim, TO_CHAR );
            act( AT_RED,
                 "The disc flies straight toward $N, breaking apart into smaller pieces which cut through $m, killing $m instantly!!",
                 ch, NULL, victim, TO_CHAR );
            act( AT_YELLOW, "$N raises $s hand above $s head, forming a disc of pure energy, and throws it at you.", ch,
                 NULL, victim, TO_VICT );
            act( AT_RED,
                 "The disc flies straight toward you, breaking apart into smaller pieces which cut through you, killing you instantly!!",
                 ch, NULL, victim, TO_VICT );
            act( AT_YELLOW, "$n raises $s hand above $s head, forming a disc of pure energy, and throws it at $N.", ch, NULL,
                 victim, TO_NOTVICT );
            act( AT_RED,
                 "The disc flies straight toward $N, breaking apart into smaller pieces which cut through $m, killing $m instantly!!",
                 ch, NULL, victim, TO_NOTVICT );

            learn_from_success( ch, gsn_multi_disk );
            global_retcode = damage( ch, victim, dam, TYPE_HIT );
            break;
          }
        default:
          while( shots > 0 )
          {
/* Changed to increase damage -- Islvin
		switch (number_range( 1, 4))
		{
			default:
			case 1:
				break;
			case 2:
			case 3:
				dam += number_range( 0, 1 );
				break;
			case 4:
				dam += number_range( 0, 2 );
				break;
		}
*/
            switch ( number_range( 1, 3 ) )
            {
              default:
              case 1:
                break;
              case 2:
                dam += number_range( 0, 1 );
                break;
              case 3:
                dam += number_range( 0, 2 );
                break;
            }

            shots--;
          }
          dam2 = number_range( 2, 5 );  // was 5-7 in the damage call
          strcpy( buf2, num_punct( get_attmod( ch, victim ) * dam * dam2 ) );

          act( AT_YELLOW, "You raise your hand above your head, palm up, and bring a crackling disk of", ch, NULL, victim,
               TO_CHAR );
          act( AT_YELLOW, "energy forth.  The disk hums as you eye $N and send it spinning $S way.", ch, NULL, victim,
               TO_CHAR );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim, TO_CHAR,
                NULL );
          act2( AT_YELLOW, "as they home in on $N. &W[$t]", ch, buf2, victim, TO_CHAR, NULL );
          act( AT_YELLOW, "$n raises $s hand above $s head, palm up, and brings a crackling disk of", ch, NULL, victim,
               TO_VICT );
          act( AT_YELLOW, "energy forth.  The disk hums as $e eyes you and sends it spinning your way.", ch, NULL, victim,
               TO_VICT );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim, TO_VICT,
                NULL );
          act2( AT_YELLOW, "as they home in on you. &W[$t]", ch, buf2, victim, TO_VICT, NULL );
          act( AT_YELLOW, "$n raises $s hand above $s head, palm up, and brings a crackling disk of", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_YELLOW, "energy forth.  The disk hums as $e eyes $N and sends it spinning $N's way.", ch, NULL, victim,
               TO_NOTVICT );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim,
                TO_NOTVICT, NULL );
          act2( AT_YELLOW, "as they home in on $N. &W[$t]", ch, buf2, victim, TO_NOTVICT, NULL );

          learn_from_success( ch, gsn_multi_disk );
          global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * dam * dam2 ), TYPE_HIT );
          break;
      }
    }
    else
    {
      act( AT_YELLOW, "You launch a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_CHAR );
      act( AT_YELLOW, "$n launches a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_VICT );
      act( AT_YELLOW, "$n launches a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_NOTVICT );
      learn_from_failure( ch, gsn_multi_disk );
      global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
  }
  if( shot >= 30 && shot <= 39 )
  {
    if( can_use_skill( ch, number_percent(  ), gsn_multi_disk ) )
    {
      switch ( number_range( 1, 100 ) )
      {
        case 100:
        case 99:
        case 98:
        case 97:
          if( IS_NPC( victim )
              || ( !IS_NPC( victim ) && ( !xIS_SET( ch->act, PLR_SPAR ) || !xIS_SET( ch->act, PLR_SPAR ) ) ) )
          {

            /*
             * Redone so that instant death can't be abused to instant kill
             * * players over 2x their power. -- Islvin
             */
            if( victim->pl / ch->pl >= 2 )
            {
              dam = get_attmod( ch, victim ) * number_range( 10, 13 );
              act( AT_YELLOW, "You raise your hand above your head, forming a disc of pure energy, and throw it at $N.", ch,
                   NULL, victim, TO_CHAR );
              act( AT_YELLOW,
                   "The disc flies straight toward $N, breaking apart into smaller pieces... but they're all absorbed by $s aura.",
                   ch, NULL, victim, TO_CHAR );
              act( AT_YELLOW, "$N raises $s hand above $s head, forming a disc of pure energy, and throws it at you.", ch,
                   NULL, victim, TO_VICT );
              act( AT_YELLOW,
                   "The disc flies straight toward you, breaking apart into smaller pieces... but they're all absorbed by your aura.",
                   ch, NULL, victim, TO_VICT );
              act( AT_YELLOW, "$n raises $s hand above $s head, forming a disc of pure energy, and throws it at $N.", ch,
                   NULL, victim, TO_NOTVICT );
              act( AT_YELLOW,
                   "The disc flies straight toward $N, breaking apart into smaller pieces... but they're all absorbed by $s aura.",
                   ch, NULL, victim, TO_NOTVICT );
              learn_from_success( ch, gsn_multi_disk );
              global_retcode = damage( ch, victim, dam, TYPE_HIT );
              break;
            }
            dam = 999999999;
            act( AT_YELLOW, "You raise your hand above your head, forming a disc of pure energy, and throw it at $N.", ch,
                 NULL, victim, TO_CHAR );
            act( AT_RED,
                 "The disc flies straight toward $N, breaking apart into smaller pieces which cut through $m, killing $m instantly!!",
                 ch, NULL, victim, TO_CHAR );
            act( AT_YELLOW, "$N raises $s hand above $s head, forming a disc of pure energy, and throws it at you.", ch,
                 NULL, victim, TO_VICT );
            act( AT_RED,
                 "The disc flies straight toward you, breaking apart into smaller pieces which cut through you, killing you instantly!!",
                 ch, NULL, victim, TO_VICT );
            act( AT_YELLOW, "$n raises $s hand above $s head, forming a disc of pure energy, and throws it at $N.", ch, NULL,
                 victim, TO_NOTVICT );
            act( AT_RED,
                 "The disc flies straight toward $N, breaking apart into smaller pieces which cut through $m, killing $m instantly!!",
                 ch, NULL, victim, TO_NOTVICT );

            learn_from_success( ch, gsn_multi_disk );
            global_retcode = damage( ch, victim, dam, TYPE_HIT );
            break;
          }
        default:
          while( shots > 0 )
          {
/* Changed to increase damage -- Islvin
		switch (number_range( 1, 4))
		{
			default:
			case 1:
				break;
			case 2:
			case 3:
				dam += number_range( 0, 1 );
				break;
			case 4:
				dam += number_range( 0, 2 );
				break;
		}
*/
            switch ( number_range( 1, 3 ) )
            {
              default:
              case 1:
                break;
              case 2:
                dam += number_range( 0, 1 );
                break;
              case 3:
                dam += number_range( 0, 2 );
                break;
            }

            shots--;
          }
          dam2 = number_range( 2, 5 );  // was 5-7 in the damage call
          strcpy( buf2, num_punct( get_attmod( ch, victim ) * dam * dam2 ) );

          act( AT_YELLOW, "You raise your hand above your head, palm up, and bring a crackling disk of", ch, NULL, victim,
               TO_CHAR );
          act( AT_YELLOW, "energy forth.  The disk hums as you eye $N and send it spinning $S way.", ch, NULL, victim,
               TO_CHAR );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim, TO_CHAR,
                NULL );
          act2( AT_YELLOW, "as they home in on $N. &W[$t]", ch, buf2, victim, TO_CHAR, NULL );
          act( AT_YELLOW, "$n raises $s hand above $s head, palm up, and brings a crackling disk of", ch, NULL, victim,
               TO_VICT );
          act( AT_YELLOW, "energy forth.  The disk hums as $e eyes you and sends it spinning your way.", ch, NULL, victim,
               TO_VICT );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim, TO_VICT,
                NULL );
          act2( AT_YELLOW, "as they home in on you. &W[$t]", ch, buf2, victim, TO_VICT, NULL );
          act( AT_YELLOW, "$n raises $s hand above $s head, palm up, and brings a crackling disk of", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_YELLOW, "energy forth.  The disk hums as $e eyes $N and sends it spinning $N's way.", ch, NULL, victim,
               TO_NOTVICT );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim,
                TO_NOTVICT, NULL );
          act2( AT_YELLOW, "as they home in on $N. &W[$t]", ch, buf2, victim, TO_NOTVICT, NULL );

          learn_from_success( ch, gsn_multi_disk );
          global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * dam * dam2 ), TYPE_HIT );
          break;
      }
    }
    else
    {
      act( AT_YELLOW, "You launch a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_CHAR );
      act( AT_YELLOW, "$n launches a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_VICT );
      act( AT_YELLOW, "$n launches a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_NOTVICT );
      learn_from_failure( ch, gsn_multi_disk );
      global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
  }
  if( shot >= 40 && shot <= 50 )
  {
    if( can_use_skill( ch, number_percent(  ), gsn_multi_disk ) )
    {
      switch ( number_range( 1, 100 ) )
      {
        case 100:
        case 99:
        case 98:
        case 97:
        case 96:
          if( IS_NPC( victim )
              || ( !IS_NPC( victim ) && ( !xIS_SET( ch->act, PLR_SPAR ) || !xIS_SET( ch->act, PLR_SPAR ) ) ) )
          {

            /*
             * Redone so that instant death can't be abused to instant kill
             * * players over 2x their power. -- Islvin
             */
            if( victim->pl / ch->pl >= 2 )
            {
              dam = get_attmod( ch, victim ) * number_range( 10, 13 );
              act( AT_YELLOW, "You raise your hand above your head, forming a disc of pure energy, and throw it at $N.", ch,
                   NULL, victim, TO_CHAR );
              act( AT_YELLOW,
                   "The disc flies straight toward $N, breaking apart into smaller pieces... but they're all absorbed by $s aura.",
                   ch, NULL, victim, TO_CHAR );
              act( AT_YELLOW, "$N raises $s hand above $s head, forming a disc of pure energy, and throws it at you.", ch,
                   NULL, victim, TO_VICT );
              act( AT_YELLOW,
                   "The disc flies straight toward you, breaking apart into smaller pieces... but they're all absorbed by your aura.",
                   ch, NULL, victim, TO_VICT );
              act( AT_YELLOW, "$n raises $s hand above $s head, forming a disc of pure energy, and throws it at $N.", ch,
                   NULL, victim, TO_NOTVICT );
              act( AT_YELLOW,
                   "The disc flies straight toward $N, breaking apart into smaller pieces... but they're all absorbed by $s aura.",
                   ch, NULL, victim, TO_NOTVICT );
              learn_from_success( ch, gsn_multi_disk );
              global_retcode = damage( ch, victim, dam, TYPE_HIT );
              break;
            }
            dam = 999999999;
            act( AT_YELLOW, "You raise your hand above your head, forming a disc of pure energy, and throw it at $N.", ch,
                 NULL, victim, TO_CHAR );
            act( AT_RED,
                 "The disc flies straight toward $N, breaking apart into smaller pieces which cut through $m, killing $m instantly!!",
                 ch, NULL, victim, TO_CHAR );
            act( AT_YELLOW, "$N raises $s hand above $s head, forming a disc of pure energy, and throws it at you.", ch,
                 NULL, victim, TO_VICT );
            act( AT_RED,
                 "The disc flies straight toward you, breaking apart into smaller pieces which cut through you, killing you instantly!!",
                 ch, NULL, victim, TO_VICT );
            act( AT_YELLOW, "$n raises $s hand above $s head, forming a disc of pure energy, and throws it at $N.", ch, NULL,
                 victim, TO_NOTVICT );
            act( AT_RED,
                 "The disc flies straight toward $N, breaking apart into smaller pieces which cut through $m, killing $m instantly!!",
                 ch, NULL, victim, TO_NOTVICT );

            learn_from_success( ch, gsn_multi_disk );
            global_retcode = damage( ch, victim, dam, TYPE_HIT );
            break;
          }
        default:
          while( shots > 0 )
          {
/* Changed to increase damage -- Islvin
		switch (number_range( 1, 4))
		{
			default:
			case 1:
				break;
			case 2:
			case 3:
				dam += number_range( 0, 1 );
				break;
			case 4:
				dam += number_range( 0, 2 );
				break;
		}
*/
            switch ( number_range( 1, 3 ) )
            {
              default:
              case 1:
                break;
              case 2:
                dam += number_range( 0, 1 );
                break;
              case 3:
                dam += number_range( 0, 2 );
                break;
            }

            shots--;
          }
          dam2 = number_range( 2, 5 );  // was 5-7 in the damage call
          strcpy( buf2, num_punct( get_attmod( ch, victim ) * dam * dam2 ) );

          act( AT_YELLOW, "You raise your hand above your head, palm up, and bring a crackling disk of", ch, NULL, victim,
               TO_CHAR );
          act( AT_YELLOW, "energy forth.  The disk hums as you eye $N and send it spinning $S way.", ch, NULL, victim,
               TO_CHAR );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim, TO_CHAR,
                NULL );
          act2( AT_YELLOW, "as they home in on $N. &W[$t]", ch, buf2, victim, TO_CHAR, NULL );
          act( AT_YELLOW, "$n raises $s hand above $s head, palm up, and brings a crackling disk of", ch, NULL, victim,
               TO_VICT );
          act( AT_YELLOW, "energy forth.  The disk hums as $e eyes you and sends it spinning your way.", ch, NULL, victim,
               TO_VICT );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim, TO_VICT,
                NULL );
          act2( AT_YELLOW, "as they home in on you. &W[$t]", ch, buf2, victim, TO_VICT, NULL );
          act( AT_YELLOW, "$n raises $s hand above $s head, palm up, and brings a crackling disk of", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_YELLOW, "energy forth.  The disk hums as $e eyes $N and sends it spinning $N's way.", ch, NULL, victim,
               TO_NOTVICT );
          act2( AT_YELLOW, "Mid-flight the disk breaks into $t smaller disks, each spinning madly", ch, buf, victim,
                TO_NOTVICT, NULL );
          act2( AT_YELLOW, "as they home in on $N. &W[$t]", ch, buf2, victim, TO_NOTVICT, NULL );

          learn_from_success( ch, gsn_multi_disk );
          global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * dam * dam2 ), TYPE_HIT );
          break;
      }
    }
    else
    {
      act( AT_YELLOW, "You launch a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_CHAR );
      act( AT_YELLOW, "$n launches a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_VICT );
      act( AT_YELLOW, "$n launches a crackling disk of energy toward $N but it breaks up halfway there.", ch, NULL, victim,
           TO_NOTVICT );
      learn_from_failure( ch, gsn_multi_disk );
      global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
  }
  ch->mana -= energy;
  return;
}


void do_ki_heal( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_STRING_LENGTH];
  int healTo = 100;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_ki_heal ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_ki_heal]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  argument = one_argument( argument, arg );

  if( arg[0] == '\0' )
  {
    send_to_char( "Syntax: heal <target> [<amount>]\n\r" "    or: heal <target> full\n\r", ch );
    return;
  }

  if( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    send_to_char( "I see no one here with that name.\n\r", ch );
    return;
  }

  if( is_android( victim ) || is_superandroid( victim ) )
  {
    send_to_char( "You can't heal a being without Ki.\n\r", ch );
    return;
  }

  if( victim->hit >= victim->max_hit )
  {
    send_to_char( "There's no need to heal them.\n\r", ch );
    return;
  }

  if( argument[0] != '\0' )
  {
    if( strcmp( argument, "full" ) )
    {
      healTo = victim->max_hit - victim->hit;
    }
    if( atoi( argument ) > 0 && atoi( argument ) < 100 )
    {
      healTo = atoi( argument );
    }
  }
  if( healTo > ( victim->max_hit - victim->hit ) )
    healTo = victim->max_hit - victim->hit;

  if( !IS_NPC( ch ) && ( ch->pcdata->learned[gsn_ki_heal] < 95 ) && ( healTo > ch->pcdata->learned[gsn_ki_heal] ) )
  {
    send_to_char( "You aren't skilled enough to heal that amount.\n\r", ch );
    return;
  }

  if( ch->mana < ( healTo * skill_table[gsn_ki_heal]->min_mana ) )
  {
    send_to_char( "You don't have enough energy to heal anyone.\n\r", ch );
    return;
  }

  if( who_fighting( victim ) != NULL )
  {
    send_to_char( "You'll have to wait until they aren't fighting.\n\r", ch );
    return;
  }

  if( who_fighting( ch ) != NULL )
  {
    send_to_char( "You're a bit busy to do that right now.\n\r", ch );
    return;
  }

  if( victim == ch )
  {
    send_to_char( "You can't heal yourself.\n\r", ch );
    return;
  }

  if( can_use_skill( ch, number_percent(  ), gsn_ki_heal ) )
  {
    act( AT_YELLOW, "You lay your palms upon $N's wounds, a soft glow forming as you use your energy to heal $M.", ch, NULL,
         victim, TO_CHAR );
    act( AT_YELLOW, "$n lays $s palms upon your wounds, a soft glow forming as $e uses $s energy to heal you.", ch, NULL,
         victim, TO_VICT );
    act( AT_YELLOW, "$n lays $s palms upon $N's wounds, a soft glow forming as $e uses $s energy to heal $N.", ch, NULL,
         victim, TO_NOTVICT );

    victim->hit += healTo;
    if( victim->hit > victim->max_hit )
    {
      victim->hit = victim->max_hit;
    }
    learn_from_success( ch, gsn_ki_heal );
    ch->mana -= ( healTo * skill_table[gsn_ki_heal]->min_mana );
  }
  else
  {
    act( AT_YELLOW, "You lay your palms upon $N's wounds, but nothing happens.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "$n lays $s plams upon your wounds, but nothing happens.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n lays $s palms upon $N's wounds, but nothing happens.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_ki_heal );
    ch->mana -= ( healTo * skill_table[gsn_ki_heal]->min_mana ) / 2;
  }


}

void do_growth( CHAR_DATA * ch, char *argument )
{
  int strMod = 10;
  int conMod = 5;
  int intMod = 5;
  int spdMod = -10;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_growth ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_growth]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_GROWTH ) && !xIS_SET( ( ch )->affected_by, AFF_GIANT ) )
  {
    act( AT_SKILL, "You shrink and return to your original size.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n shrinks and returns to $s original size.", ch, NULL, NULL, TO_NOTVICT );
    transStatRemove( ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_GROWTH );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_GROWTH ) && xIS_SET( ( ch )->affected_by, AFF_GIANT ) )
  {
    send_to_char( "You must revert back from giant size before doing this.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_growth]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( can_use_skill( ch, number_percent(  ), gsn_growth ) )
  {
    act( AT_SKILL, "Your body surges with power, suddenly expanding and growing to at least twice your normal size.", ch,
         NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n's body surges with power, suddenly expanding and growing to at least twice $s normal size.", ch, NULL,
         NULL, TO_NOTVICT );

    learn_from_success( ch, gsn_growth );
    xSET_BIT( ( ch )->affected_by, AFF_GROWTH );
    transStatApply( ch, strMod, spdMod, intMod, conMod );
  }
  else
  {
    act( AT_SKILL, "You expand slightly before shrinking back to your normal size and stature.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n expands slightly before shrinking back to $s normal size and stature.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_growth );
  }
  ch->mana -= skill_table[gsn_growth]->min_mana;
}

void do_giant_size( CHAR_DATA * ch, char *argument )
{
  int strMod = 25;
  int conMod = 15;
  int intMod = 10;
  int spdMod = -25;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_giant ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_giant]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_GROWTH ) && xIS_SET( ( ch )->affected_by, AFF_GIANT ) )
  {
    act( AT_SKILL, "You shrink and return to your original size.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n shrinks and returns to $s original size.", ch, NULL, NULL, TO_NOTVICT );
    transStatRemove( ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_GIANT );
    xREMOVE_BIT( ( ch )->affected_by, AFF_GROWTH );
    return;
  }

  if( !xIS_SET( ( ch )->affected_by, AFF_GROWTH ) )
  {
    send_to_char( "You must first grow before you can use this.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_giant]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( can_use_skill( ch, number_percent(  ), gsn_giant ) )
  {
    transStatRemove( ch );
    act( AT_SKILL, "You expand even further than before, transforming yourself into a Namekian giant.", ch, NULL, NULL,
         TO_CHAR );
    act( AT_SKILL, "$n expands even further than before, transforming $mself into a Namekian giant.", ch, NULL, NULL,
         TO_NOTVICT );

    learn_from_success( ch, gsn_giant );
    ch->mana -= skill_table[gsn_giant]->min_mana;
    xSET_BIT( ( ch )->affected_by, AFF_GIANT );
    transStatApply( ch, strMod, spdMod, intMod, conMod );
  }
  else
  {
    act( AT_SKILL, "Your body fills with energy but you can't seem to grow any larger.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n fills $s body with energy but can't seem to grow any larger.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_giant );
    ch->mana -= skill_table[gsn_giant]->min_mana;
  }
}

void do_split_form( CHAR_DATA * ch, char *argument )
{

  CHAR_DATA *victim;
  MOB_INDEX_DATA *pMobIndex;

  if( IS_NPC( ch ) )
  {
    send_to_char( "NPC's can't do that.\n\r", ch );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_split_form]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( IS_IMMORTAL( ch ) && get_trust( ch ) < 59 )
  {
    send_to_char( "You can't split-form.\n\r", ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) )
  {
    ch_printf( ch, "You can't use this while in Oozaru!\n\r" );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    ch_printf( ch, "You can't use this while in Golden Oozaru!\n\r" );
    return;
  }

  if( IS_NPC( ch ) && ch->pIndexData->vnum == 610 )
  {
    send_to_char( "You aren't allowed to do that.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_BIOJR ) )
  {
    ch_printf( ch, "You can't use this skill while you have clones.\n\r" );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SPLIT_FORM ) )
  {
    reuniteSplitForms( ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_TRI_FORM ) || xIS_SET( ( ch )->affected_by, AFF_MULTI_FORM ) )
  {
    reuniteSplitForms( ch );
    return;
  }

  if( ch->mana < skill_table[gsn_split_form]->min_mana )
  {
    pager_printf_color( ch, "You must have at least %s energy to split.\n\r",
                        num_punct( skill_table[gsn_split_form]->min_mana ) );
    return;
  }

  if( ( pMobIndex = get_mob_index( 610 ) ) == NULL )
  {
    bug( "do_split_form: mob does not exist" );
    send_to_char( "Hmm... Something went wrong...\n\r", ch );
    return;
  }

  victim = create_mobile( pMobIndex );
  char_to_room( victim, ch->in_room );

  if( can_use_skill( ch, number_percent(  ), gsn_split_form ) )
  {
    statsToMob( ch, victim, gsn_split_form, FALSE, 0, 1 );  // Split without error
    act( AT_SKILL, "Powering up, your body begins to separate and with a booming yell you create a double of your self!", ch,
         NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n powers up as $s body begins to separate and with a booming yell two $n's now exist!", ch, NULL, NULL,
         TO_NOTVICT );
    learn_from_success( ch, gsn_split_form );
  }
  else
  {
    statsToMob( ch, victim, gsn_split_form, TRUE, 0, 1 ); // Doh!  They botched it :(
    act( AT_SKILL,
         "You begin to create a double of your self, but just before the final separation you lose focus and something goes wrong.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL,
         "$n begins to create a double of $s self, but just before the final separation $e loses focus and something goes wrong.",
         ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_split_form );
  }

  ch->mana = ch->mana / 2;
  add_follower( victim, ch );
  victim->master = ch;
  victim->leader = ch;
  ch->leader = ch;
  xSET_BIT( ch->affected_by, AFF_SPLIT_FORM );
  xSET_BIT( victim->affected_by, AFF_SPLIT_FORM );

  return;
}

void do_tri_form( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  CHAR_DATA *victim2;
  MOB_INDEX_DATA *pMobIndex;

  if( IS_NPC( ch ) )
  {
    send_to_char( "NPC's can't do that.\n\r", ch );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }


  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_tri_form]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( IS_IMMORTAL( ch ) && get_trust( ch ) < 59 )
  {
    send_to_char( "You can't tri-form.\n\r", ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    ch_printf( ch, "You can't use this while in Oozaru!\n\r" );
    return;
  }
  if( IS_NPC( ch ) && ch->pIndexData->vnum == 610 )
  {
    send_to_char( "You aren't allowed to do that.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_BIOJR ) )
  {
    ch_printf( ch, "You can't use this skill while you have clones.\n\r" );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_TRI_FORM ) )
  {
    reuniteSplitForms( ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SPLIT_FORM ) || xIS_SET( ( ch )->affected_by, AFF_MULTI_FORM ) )
  {
    reuniteSplitForms( ch );
    return;
  }

  if( ch->mana < skill_table[gsn_tri_form]->min_mana )
  {
    pager_printf_color( ch, "You must have at least %s energy to tri-form.\n\r",
                        num_punct( skill_table[gsn_tri_form]->min_mana ) );
    return;
  }
  if( ( pMobIndex = get_mob_index( 610 ) ) == NULL )
  {
    bug( "do_tri_form: mob does not exist" );
    send_to_char( "Hmm... Something went wrong...\n\r", ch );
    return;
  }

  victim = create_mobile( pMobIndex );
  char_to_room( victim, ch->in_room );

  victim2 = create_mobile( pMobIndex );
  char_to_room( victim2, ch->in_room );

  if( can_use_skill( ch, number_percent(  ), gsn_tri_form ) )
  {
    statsToMob( ch, victim, gsn_tri_form, FALSE, 0, 1 );  // Tri without error
    statsToMob( ch, victim2, gsn_tri_form, FALSE, 0, 2 ); // Tri without error
    act( AT_YELLOW,
         "Powering up greatly, your body starts glowing bright gold as your chest and back begin to bulge. You scream out as two copies of yourself burst forth from your body and stand next to you, prepared to fight.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW,
         "$n powers up greatly; $s body starts glowing bright gold as $s chest and back begin to bulge. $n screams out as two copies of $mself bursts forth from $s body and stand next to $m, prepared to fight.",
         ch, NULL, NULL, TO_NOTVICT );

    learn_from_success( ch, gsn_tri_form );
  }
  else
  {
    statsToMob( ch, victim, gsn_tri_form, TRUE, 0, 1 ); // Doh! They botched it :(
    statsToMob( ch, victim2, gsn_tri_form, TRUE, 0, 2 );  // Doh! They botched it :(
    act( AT_YELLOW,
         "You begin to create two copies of yourself, but just before the final separation you lose focus and something goes wrong.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW,
         "$n begins to create two doubles of $mself, but just before the final separation $e loses focus and something goes wrong.",
         ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_tri_form );
  }

  ch->mana = ch->mana / 2;
  add_follower( victim, ch );
  add_follower( victim2, ch );
  victim->master = ch;
  victim->leader = ch;
  victim2->master = ch;
  victim2->leader = ch;
  ch->leader = ch;
  xSET_BIT( ch->affected_by, AFF_TRI_FORM );
  xSET_BIT( victim->affected_by, AFF_TRI_FORM );
  xSET_BIT( victim2->affected_by, AFF_TRI_FORM );

  return;
}

void do_multi_form( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  CHAR_DATA *victim2;
  CHAR_DATA *victim3;
  MOB_INDEX_DATA *pMobIndex;

  if( IS_NPC( ch ) )
  {
    send_to_char( "NPC's can't do that.\n\r", ch );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_multi_form]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( IS_IMMORTAL( ch ) && get_trust( ch ) < 59 )
  {
    send_to_char( "You can't multi-form.\n\r", ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    ch_printf( ch, "You can't use this while in Oozaru!\n\r" );
    return;
  }
  if( IS_NPC( ch ) && ch->pIndexData->vnum == 610 )
  {
    send_to_char( "You aren't allowed to do that.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_BIOJR ) )
  {
    ch_printf( ch, "You can't use this skill while you have clones.\n\r" );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_MULTI_FORM ) )
  {
    reuniteSplitForms( ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SPLIT_FORM ) || xIS_SET( ( ch )->affected_by, AFF_TRI_FORM ) )
  {
    reuniteSplitForms( ch );
    return;
  }

  if( ch->mana < skill_table[gsn_multi_form]->min_mana )
  {
    pager_printf_color( ch, "You must have at least %s energy to multi-form.\n\r",
                        num_punct( skill_table[gsn_multi_form]->min_mana ) );
    return;
  }
  if( ( pMobIndex = get_mob_index( 610 ) ) == NULL )
  {
    bug( "do_multi_form: mob does not exist" );
    send_to_char( "Hmm... Something went wrong...\n\r", ch );
    return;
  }

  victim = create_mobile( pMobIndex );
  char_to_room( victim, ch->in_room );

  victim2 = create_mobile( pMobIndex );
  char_to_room( victim2, ch->in_room );

  victim3 = create_mobile( pMobIndex );
  char_to_room( victim3, ch->in_room );

  if( can_use_skill( ch, number_percent(  ), gsn_multi_form ) )
  {
    statsToMob( ch, victim, gsn_multi_form, FALSE, 0, 1 );  // Multi without error
    statsToMob( ch, victim2, gsn_multi_form, FALSE, 0, 2 ); // Multi without error
    statsToMob( ch, victim3, gsn_multi_form, FALSE, 0, 3 ); // Multi without error
    act( AT_WHITE,
         "You cross your arms directly infront of yourself and yell 'MULTI FORM!!'. Your body turns transparent as you suddenly split into two images that move away from each other till they are a couple feet away. Then the two split, becoming four. Each copy of you, including yourself, becomes opaque again. All four of you get prepared to fight.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_WHITE,
         "$n crosses $s arms directly infront of $mself and yells 'MULTI FORM!!'. $*s body turns transparent as $e suddenly splits into two images that move away from each other till they are a couple feet away. Then the two split, becoming four. Each copy of $n, including $mself, becomes opaque again. All four of $n look prepared to fight.",
         ch, NULL, NULL, TO_NOTVICT );

    learn_from_success( ch, gsn_multi_form );
  }
  else
  {
    statsToMob( ch, victim, gsn_multi_form, TRUE, 0, 1 ); // Doh! They botched it :(
    statsToMob( ch, victim2, gsn_multi_form, TRUE, 0, 2 );  // Doh! They botched it :(
    statsToMob( ch, victim3, gsn_multi_form, TRUE, 0, 3 );  // Doh! They botched it :(
    act( AT_WHITE,
         "You begin to create three copies of yourself, but just before the final separation you lose focus and something goes wrong.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_WHITE,
         "$n begins to create three copies of $mself, but just before the final separation $e loses focus and something goes wrong.",
         ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_multi_form );
  }

  ch->mana = ch->mana / 2;
  add_follower( victim, ch );
  add_follower( victim2, ch );
  add_follower( victim3, ch );
  victim->master = ch;
  victim->leader = ch;
  victim2->master = ch;
  victim2->leader = ch;
  victim3->master = ch;
  victim3->leader = ch;
  ch->leader = ch;
  xSET_BIT( ch->affected_by, AFF_MULTI_FORM );
  xSET_BIT( victim->affected_by, AFF_MULTI_FORM );
  xSET_BIT( victim2->affected_by, AFF_MULTI_FORM );
  xSET_BIT( victim3->affected_by, AFF_MULTI_FORM );

  return;
}

void do_sanctuary( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) )
    return;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }


  /* All ranks but hylian/fierian/reploid require rank 3.. hylians rank 4; fierians rank 5; reploids rank 6 */
  if((ch->rank < 3 && !is_hylian(ch) && !is_fierian(ch) && !is_reploid(ch) && !IS_FUSED(ch)) || (ch->rank < 4 && is_hylian(ch)) || (ch->rank < 5 && is_fierian(ch)) || (ch->rank < 6 && is_reploid(ch)))
  { 
    ch_printf(ch, "Huh?\r\n");
    return;
  }



  if( ch->mana < 5000 )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( !IS_AFFECTED( ch, AFF_SANCTUARY ) )
  {
    if( is_hylian( ch ) )
    {
      act( AT_BLUE,
           "You relax and pray to the Deities of Farore, Din, and Nayru. A soft blue glow begins to radiate from your body, exploding into an aura of divine protection.",
           ch, NULL, NULL, TO_CHAR );
      act( AT_BLUE,
           "$n relaxes and prays to $s Deities of Farore, Din, and Nayru. A soft blue glow begins to radiate from $n's body, exploding into an aura of divine protection.",
           ch, NULL, NULL, TO_NOTVICT );
    }
    else if( IS_GOOD( ch ) )
    {
      act( AT_WHITE,
           "You relax and concentrate your supernatural powers. A soft white glow begins to radiate from your body, exploding into an aura of divine protection.",
           ch, NULL, NULL, TO_CHAR );
      act( AT_WHITE,
           "$n relaxes and concentrates $s supernatural powers. A soft white glow begins to radiate from $n's body, exploding into an aura of divine protection.",
           ch, NULL, NULL, TO_NOTVICT );
    }
    else if( IS_NEUTRAL( ch ) )
    {
      act( AT_GREY,
           "You relax and concentrate your incredible powers. A soft glow begins to radiate from your body, exploding into an aura of protection.",
           ch, NULL, NULL, TO_CHAR );
      act( AT_GREY,
           "$n relaxes and concentrates $s incredible powers. A soft glow begins to radiate from $n's body, exploding into an aura of protection.",
           ch, NULL, NULL, TO_NOTVICT );
    }
    else if( IS_EVIL( ch ) )
    {
      act( AT_DGREY,
           "You relax and concentrate your demonic powers. A soft black glow begins to radiate from your body, exploding into an aura of evil protection.",
           ch, NULL, NULL, TO_CHAR );
      act( AT_DGREY,
           "$n relaxes and concentrates $s demonic powers. A soft black glow begins to radiate from $n's body, exploding into an aura of evil protection.",
           ch, NULL, NULL, TO_NOTVICT );
    }
    xSET_BIT( ( ch )->affected_by, AFF_SANCTUARY );
  }
  else
  {
    if( is_hylian( ch ) )
    {
      act( AT_BLUE, "The aura of divine protection around you breaks apart and vanishes into thin air.", ch, NULL, NULL,
           TO_CHAR );
      act( AT_BLUE, "The aura of divine protection around $n breaks apart and vanishes into thin air.", ch, NULL, NULL,
           TO_NOTVICT );
    }
    else if( IS_GOOD( ch ) )
    {
      act( AT_WHITE, "The aura of divine protection around you breaks apart and vanishes into thin air.", ch, NULL, NULL,
           TO_CHAR );
      act( AT_WHITE, "The aura of divine protection around $n breaks apart and vanishes into thin air.", ch, NULL, NULL,
           TO_NOTVICT );
    }
    else if( IS_NEUTRAL( ch ) )
    {
      act( AT_GREY, "The aura of protection around you breaks apart and vanishes into thin air.", ch, NULL, NULL, TO_CHAR );
      act( AT_GREY, "The aura of protection around $n breaks apart and vanishes into thin air.", ch, NULL, NULL,
           TO_NOTVICT );
    }
    else if( IS_EVIL( ch ) )
    {
      act( AT_DGREY, "The aura of evil protection around you breaks apart and vanishes into thin air.", ch, NULL, NULL,
           TO_CHAR );
      act( AT_DGREY, "The aura of evil protection around $n breaks apart and vanishes into thin air.", ch, NULL, NULL,
           TO_NOTVICT );
    }
    xREMOVE_BIT( ( ch )->affected_by, AFF_SANCTUARY );
  }
  ch->mana -= 5000;
  save_char_obj( ch );
  return;
}

void do_clone( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  CHAR_DATA *victim2;
  CHAR_DATA *victim3;
  CHAR_DATA *victim4;
  CHAR_DATA *victim5;
/*
  CHAR_DATA *victim6;
  CHAR_DATA *victim7;
  CHAR_DATA *victim8;
  CHAR_DATA *victim9;
  CHAR_DATA *victim10;
*/
  MOB_INDEX_DATA *pMobIndex;
  char arg[MAX_INPUT_LENGTH];
  int count = 0;

  argument = one_argument( argument, arg );

  if( IS_NPC( ch ) )
  {
    send_to_char( "NPC's can't do that.\n\r", ch );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_clone]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( IS_IMMORTAL( ch ) && get_trust( ch ) < 59 )
  {
    send_to_char( "You can't clone.\n\r", ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    ch_printf( ch, "You can't use this while in Oozaru!\n\r" );
    return;
  }
  if( IS_NPC( ch ) && ch->pIndexData->vnum == 610 )
  {
    send_to_char( "You aren't allowed to do that.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_BIOJR ) )
  {
    reuniteSplitForms( ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SPLIT_FORM ) ||
      xIS_SET( ( ch )->affected_by, AFF_TRI_FORM ) || xIS_SET( ( ch )->affected_by, AFF_MULTI_FORM ) )
  {
    ch_printf( ch, "You can't create clones while seperated into multiple parts.\n\r" );
    return;
  }

  count = atoi( arg );

  if( count < 5 || count > 10 )
  {
    ch_printf( ch, "The amount of clones has to be between 5 and 10.\n\r" );
    return;
  }

  if( count != 5 )
  {
    ch_printf( ch, "Clone is currently restricted to a maximum of 5.\n\r" );
    return;
  }

  if( ch->mana < ( skill_table[gsn_clone]->min_mana * count ) )
  {
    pager_printf_color( ch, "You must have at least %s energy to create %d clones.\n\r",
                        num_punct( ( skill_table[gsn_clone]->min_mana * count ) ), count );
    return;
  }
  if( ( pMobIndex = get_mob_index( 610 ) ) == NULL )
  {
    bug( "do_clone: mob does not exist" );
    send_to_char( "Hmm... Something went wrong...\n\r", ch );
    return;
  }

  victim = create_mobile( pMobIndex );
  char_to_room( victim, ch->in_room );
  victim2 = create_mobile( pMobIndex );
  char_to_room( victim2, ch->in_room );
  victim3 = create_mobile( pMobIndex );
  char_to_room( victim3, ch->in_room );
  victim4 = create_mobile( pMobIndex );
  char_to_room( victim4, ch->in_room );
  victim5 = create_mobile( pMobIndex );
  char_to_room( victim5, ch->in_room );
/*
  if( count > 5 )
  {
    victim6 = create_mobile( pMobIndex );
    char_to_room( victim6, ch->in_room );
  }
  if( count > 6 )
  {
    victim7 = create_mobile( pMobIndex );
    char_to_room( victim7, ch->in_room );
  }
  if( count > 7 )
  {
    victim8 = create_mobile( pMobIndex );
    char_to_room( victim8, ch->in_room );
  }
  if( count > 8 )
  {
    victim9 = create_mobile( pMobIndex );
    char_to_room( victim9, ch->in_room );
  }
  if( count > 9 )
  {
    victim10 = create_mobile( pMobIndex );
    char_to_room( victim10, ch->in_room );
  }
*/
  if( can_use_skill( ch, number_percent(  ), gsn_clone ) )
  {
    statsToMob( ch, victim, gsn_clone, FALSE, count, 1 );
    statsToMob( ch, victim2, gsn_clone, FALSE, count, 2 );
    statsToMob( ch, victim3, gsn_clone, FALSE, count, 3 );
    statsToMob( ch, victim4, gsn_clone, FALSE, count, 4 );
    statsToMob( ch, victim5, gsn_clone, FALSE, count, 5 );
/*
    if( count > 5 )
      statsToMob( ch, victim6, gsn_clone, FALSE, count, 6 );
    if( count > 6 )
      statsToMob( ch, victim7, gsn_clone, FALSE, count, 7 );
    if( count > 7 )
      statsToMob( ch, victim8, gsn_clone, FALSE, count, 8 );
    if( count > 8 )
      statsToMob( ch, victim9, gsn_clone, FALSE, count, 9 );
    if( count > 9 )
      statsToMob( ch, victim10, gsn_clone, FALSE, count, 10 );
*/
    act( AT_DGREEN,
         "You clench your fists and begin to charge your power into the center of your body. You arch back as you open up your tail in your back. Suddenly, you start spitting out many small clones of yourself through your tail. After a moment, they all get onto their feet and look prepared to fight.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_DGREEN,
         "$n clenches $s fists and begins to charge $s power into the center of $s body. $*e arches back as $e opens up $s tail in $s back. Suddenly, $n starts spitting out many small clones of $mself through $s tail. After a moment, they all get onto their feet and look prepared to fight.",
         ch, NULL, NULL, TO_NOTVICT );

    learn_from_success( ch, gsn_clone );
  }
  else
  {
    extract_char( victim, TRUE );
    extract_char( victim2, TRUE );
    extract_char( victim3, TRUE );
    extract_char( victim4, TRUE );
    extract_char( victim5, TRUE );
/*
    if( count > 5 )
      extract_char( victim6, TRUE );
    if( count > 6 )
      extract_char( victim7, TRUE );
    if( count > 7 )
      extract_char( victim8, TRUE );
    if( count > 8 )
      extract_char( victim9, TRUE );
    if( count > 9 )
      extract_char( victim10, TRUE );
*/
    act( AT_DGREEN, "You try to gather energy to create clones of yourself, but something goes wrong and you fail.", ch,
         NULL, NULL, TO_CHAR );
    act( AT_DGREEN, "$n tries to gather energy to create clones of $mself, but something goes wrong and $e fails.", ch, NULL,
         NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_clone );
    return;
  }

  ch->mana -= ( skill_table[gsn_clone]->min_mana * count );
  ch->leader = ch;
  xSET_BIT( ch->affected_by, AFF_BIOJR );

  add_follower( victim, ch );
  victim->master = ch;
  victim->leader = ch;
  xSET_BIT( victim->affected_by, AFF_BIOJR );

  add_follower( victim2, ch );
  victim2->master = ch;
  victim2->leader = ch;
  xSET_BIT( victim2->affected_by, AFF_BIOJR );

  add_follower( victim3, ch );
  victim3->master = ch;
  victim3->leader = ch;
  xSET_BIT( victim3->affected_by, AFF_BIOJR );

  add_follower( victim4, ch );
  victim4->master = ch;
  victim4->leader = ch;
  xSET_BIT( victim4->affected_by, AFF_BIOJR );

  add_follower( victim5, ch );
  victim5->master = ch;
  victim5->leader = ch;
  xSET_BIT( victim5->affected_by, AFF_BIOJR );
/*
  if( count > 5 )
  {
    add_follower( victim6, ch );
    victim6->master = ch;
    victim6->leader = ch;
    xSET_BIT( victim6->affected_by, AFF_BIOJR );
  }
  if( count > 6 )
  {
    add_follower( victim7, ch );
    victim7->master = ch;
    victim7->leader = ch;
    xSET_BIT( victim7->affected_by, AFF_BIOJR );
  }
  if( count > 7 )
  {
    add_follower( victim8, ch );
    victim8->master = ch;
    victim8->leader = ch;
    xSET_BIT( victim8->affected_by, AFF_BIOJR );
  }
  if( count > 8 )
  {
    add_follower( victim9, ch );
    victim9->master = ch;
    victim9->leader = ch;
    xSET_BIT( victim9->affected_by, AFF_BIOJR );
  }
  if( count > 9 )
  {
    add_follower( victim10, ch );
    victim10->master = ch;
    victim10->leader = ch;
    xSET_BIT( victim10->affected_by, AFF_BIOJR );
  }
*/
  return;
}

void do_destructive_wave( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_destructive_wave ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_destructive_wave]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_destructive_wave]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_destructive_wave]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_destructive_wave]->focus;

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_destructive_wave]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_destructive_wave ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 15, 20 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( z, "You hold one arm straight, palm facing $N, your free hand grasping your forearm to steady your aim.", ch, NULL,
         victim, TO_CHAR );
    act( z, "A large ball of energy forms in front of your palm, blasting out towards $N. &W[$t]", ch, num_punct( dam ),
         victim, TO_CHAR );
    act( z, "$n holds one arm straight, palm facing you, $s free hand grasping $s forearm to steady $s aim.", ch, NULL,
         victim, TO_VICT );
    act( z, "A large ball of energy forms in front of $n's palm, blasting out towards you. &W[$t]", ch, num_punct( dam ),
         victim, TO_VICT );
    act( z, "$n holds one arm straight, palm facing $N, $s free hand grasping $s forearm to steady $s aim.", ch, NULL,
         victim, TO_NOTVICT );
    act( z, "A large ball of energy forms in front of $n's palm, blasting out towards $N. &W[$t]", ch, num_punct( dam ),
         victim, TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_destructive_wave );
    learn_from_success( ch, gsn_destructive_wave );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( z, "You missed $N with your destructive wave attack.", ch, NULL, victim, TO_CHAR );
    act( z, "$n misses you with $s destructive wave attack.", ch, NULL, victim, TO_VICT );
    act( z, "$n missed $N with a destructive wave attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_destructive_wave );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_destructive_wave]->min_mana;
  return;
}

void do_dodon_ray( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_dodon_ray ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_dodon_ray]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_dodon_ray]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_dodon_ray]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_dodon_ray]->focus;

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_dodon_ray]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_dodon_ray ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 22, 26 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_BLUE, "Extending a finger towards $N, a convection of energy bursts forward", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "and begins to rotate rapidly.  After coiling for a moment, the vortex compresses", ch, NULL, victim,
         TO_CHAR );
    act( AT_BLUE, "your finger slightly before being released toward the intended target.  A blue", ch, NULL, victim,
         TO_CHAR );
    act( AT_BLUE, "column emerges horizontally from the condensed origin and pummels $N", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "into a helpless state. &W[$t]", ch, num_punct( dam ), victim, TO_CHAR );

    act( AT_BLUE, "Extending a finger towards you, a convection of energy bursts forward from", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "$n and begins to rotate rapidly.  After coiling for a moment, the vortex", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "compresses $s finger slightly before being released toward the intended target.", ch, NULL, victim,
         TO_VICT );
    act( AT_BLUE, "A blue column emerges horizontally from the condensed origin and pummels", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "you into a helpless state. &W[$t]", ch, num_punct( dam ), victim, TO_VICT );

    act( AT_BLUE, "Extending a finger towards $N, a convection of energy bursts forward from", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_BLUE, "$n and begins to rotate rapidly.  After coiling for a moment, the vortex", ch, NULL, victim, TO_NOTVICT );
    act( AT_BLUE, "compresses $s finger slightly before being released toward the intended target.", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_BLUE, "A blue column emerges horizontally from the condensed origin and pummels", ch, NULL, victim, TO_NOTVICT );
    act( AT_BLUE, "$N into a helpless state. &W[$t]", ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_dodon_ray );
    dam = ki_absorb( victim, ch, dam, gsn_dodon_ray );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( z, "You missed $N with your dodon ray attack.", ch, NULL, victim, TO_CHAR );
    act( z, "$n misses you with $s dodon ray attack.", ch, NULL, victim, TO_VICT );
    act( z, "$n missed $N with a dodon ray attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_dodon_ray );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_dodon_ray]->min_mana;
  return;
}

void do_spirit_ball( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_spirit_ball ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_spirit_ball]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_spirit_ball]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_spirit_ball]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_spirit_ball]->focus;

  WAIT_STATE( ch, skill_table[gsn_spirit_ball]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_spirit_ball ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 26, 32 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_YELLOW,
         "You grab your wrist tightly with your other hand, concentrating your ki into a brilliant ball of energy that floats above your palm, your fingers curling upwards.",
         ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW,
         "The ball of light zips through the air as you gesture with two fingers, controlling its path as it zigzags wildly, finally smashing into $N with a blinding explosion. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_YELLOW,
         "$n grabs $s wrist tightly with $s other hand, concentrating $s ki into a brilliant ball of energy that floats above $s palm, $s fingers curling upwards.",
         ch, NULL, victim, TO_VICT );
    act( AT_YELLOW,
         "The ball of light zips through the air as $n gestures with two fingers, controlling its path as it zigzags wildly, finally smashing into you with a blinding explosion. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_YELLOW,
         "$n grabs $s wrist tightly with $s other hand, concentrating $s ki into a brilliant ball of energy that floats above $s palm, $s fingers curling upwards.",
         ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW,
         "The ball of light zips through the air as $n gestures with two fingers, controlling its path as it zigzags wildly, finally smashing into $N with a blinding explosion. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_spirit_ball );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_YELLOW, "You missed $N with your spirit ball attack.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "$n misses you with $s spirit ball attack.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n missed $N with a spirit ball attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_spirit_ball );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_spirit_ball]->min_mana;
  return;
}

void do_shockwave( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_shockwave ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_shockwave]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_shockwave]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_shockwave]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_shockwave]->focus;

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_shockwave]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_shockwave ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 22, 26 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( z, "You clench your fists and deeply concentrate on focusing your physical and mental powers.", ch, NULL, victim,
         TO_CHAR );
    act( z, "You then scream out as you throw one arm forward, firing a shockwave of invisible energy at $N. &W[$t]", ch,
         num_punct( dam ), victim, TO_CHAR );
    act( z, "$n clenches $s fists and deeply concentrates on focusing $s physical and mental powers.", ch, NULL, victim,
         TO_VICT );
    act( z, "$n screams out as $e throws one arm foward, firing a shockwave of invisible energy at you. &W[$t]", ch,
         num_punct( dam ), victim, TO_VICT );
    act( z, "$n clenches $s fists and deeply concentrates on focusing $s physical and mental powers.", ch, NULL, victim,
         TO_NOTVICT );
    act( z, "$n screams out as $e throws one arm foward, firing a shockwave of invisible energy at $N. &W[$t]", ch,
         num_punct( dam ), victim, TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_shockwave );
    learn_from_success( ch, gsn_shockwave );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( z, "You missed $N with your shockwave.", ch, NULL, victim, TO_CHAR );
    act( z, "$n misses you with $s shockwave.", ch, NULL, victim, TO_VICT );
    act( z, "$n missed $N with a shockwave.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_shockwave );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_shockwave]->min_mana;
  return;
}

void do_psiblast( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_psiblast ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_psiblast]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_psiblast]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_psiblast]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_psiblast]->focus;

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_psiblast]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_psiblast ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 33, 40 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( z,
         "You heavily concentrate the bulk of your mental powers all at once. Your eyes suddenly flash as you transfer a raging blast of psionic energy into $N's body. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( z,
         "$n heavily concentrates the bulk of $s mental powers all at once. $s eyes suddenly flash as $e transfers a raging blast of psionic energy into your body. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( z,
         "$n heavily concentrates the bulk of $s mental powers all at once. $s eyes suddenly flash as $e transfers a raging blast of psionic energy into $N's body. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_psiblast );
    learn_from_success( ch, gsn_psiblast );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( z, "You missed $N with your psionic blast.", ch, NULL, victim, TO_CHAR );
    act( z, "$n misses you with $s psionic blast.", ch, NULL, victim, TO_VICT );
    act( z, "$n missed $N with a psionic blast.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_psiblast );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_psiblast]->min_mana;
  return;
}

void do_divinewrath( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_divine ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_divine]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_divine]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_divine]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_divine]->focus;

  WAIT_STATE( ch, skill_table[gsn_divine]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_divine ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 68, 90 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_RED,
         "You appear directly infront of $N, clasping your wrists together in a single motion. Your power surges outward as you create a sphere of red energy that starts to push $N back.",
         ch, NULL, victim, TO_CHAR );
    act( AT_RED,
         "Your energy sphere swells with power, increasing to a massive size, trapping $N inside a torrent of searing energy. After a few intense moments, your attack shrinks back down and vanishes altogether. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_RED,
         "$n appears directly infront of you, clasping $s wrists together in a single motion. $n's power surges as $e creates a sphere of red energy that starts to push you back.",
         ch, NULL, victim, TO_VICT );
    act( AT_RED,
         "The energy sphere swells with power, increasing to a massive size, and trapping you inside a torrent of searing energy. After a few intense moments, $s attack shrinks back down and vanishes altogether. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_RED,
         "$n appears directly infront of $N, clasping $s wrists together in a single motion. $n's power surges as $e creates a sphere of red energy that starts to push $N back.",
         ch, NULL, victim, TO_NOTVICT );
    act( AT_RED,
         "The energy sphere swells with power, increasing to a massive size, and trapping $N inside a torrent of searing energy. After a few intense moments, $n's attack shrinks back down and vanishes altogether. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    // dam = ki_absorb( victim, ch, dam, gsn_divine );
    learn_from_success( ch, gsn_divine );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_RED, "You missed $N with your divine wrath.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n misses you with $s divine wrath.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n missed $N with a divine wrath.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_divine );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_divine]->min_mana;
  return;
}
void do_big_bang( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_big_bang ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_big_bang]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_big_bang]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_big_bang]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_big_bang]->focus;

  WAIT_STATE( ch, skill_table[gsn_big_bang]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_big_bang ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 33, 40 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_BLUE,
         "Turning one side of your body towards $N, you raise your hand, palm facing $M.  A huge ball of blue energy begins to form.",
         ch, NULL, victim, TO_CHAR );
    act( AT_BLUE,
         "You shout, 'BIG BANG!' as the ball of ki fires from your palm, leaving a trail of energy behind as it rockets towards $N. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_BLUE,
         "Turning one side of $n's body towards you, $e raise $s hand, palm facing you.  A huge ball of blue energy begins to form.",
         ch, NULL, victim, TO_VICT );
    act( AT_BLUE,
         "$n shouts, 'BIG BANG!' as the ball of ki fires from $s palm, leaving a trail of energy behind as it rockets towards you. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_BLUE,
         "Turning one side of $n's body towards you, $e raises $s hand, palm facing $N.  A huge ball of blue energy begins to form.",
         ch, NULL, victim, TO_NOTVICT );
    act( AT_BLUE,
         "$n shouts, 'BIG BANG!' as the ball of ki fires from $s palm, leaving a trail of energy behind as it rockets towards $N. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_big_bang );
    learn_from_success( ch, gsn_big_bang );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_BLUE, "You missed $N with your big bang attack.", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "$n misses you with $s big bang attack.", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "$n missed $N with a big bang attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_big_bang );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_big_bang]->min_mana;
  return;
}

void do_gallic_gun( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_gallic_gun ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_gallic_gun]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_gallic_gun]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_gallic_gun]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_gallic_gun]->focus;

  WAIT_STATE( ch, skill_table[gsn_gallic_gun]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_gallic_gun ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 29, 33 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_PURPLE,
         "You pull your hands to one side of your body, a dark purple sphere of energy flaring up suddenly around you.", ch,
         NULL, victim, TO_CHAR );
    act( AT_PURPLE,
         "You thrust both hands towards $N, a beam of ki blasting from your purple aura, screaming, 'Gallic Gun...FIRE!' &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_PURPLE,
         "$n pulls $s hands to one side of $s body, a dark purple sphere of energy flaring up suddenly around $m.", ch, NULL,
         victim, TO_VICT );
    act( AT_PURPLE,
         "$n thrusts both hands towards you, a beam of ki blasting from $s purple aura, screaming, 'Gallic Gun...FIRE!' &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_PURPLE,
         "$n pulls $s hands to one side of $s body, a dark purple sphere of energy flaring up suddenly around $m.", ch, NULL,
         victim, TO_NOTVICT );
    act( AT_PURPLE,
         "$n thrusts both hands towards $N, a beam of ki blasting from $s purple aura, screaming, 'Gallic Gun...FIRE! &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_gallic_gun );
    learn_from_success( ch, gsn_gallic_gun );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_PURPLE, "You missed $N with your gallic gun attack.", ch, NULL, victim, TO_CHAR );
    act( AT_PURPLE, "$n misses you with $s gallic gun attack.", ch, NULL, victim, TO_VICT );
    act( AT_PURPLE, "$n missed $N with a gallic gun attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_gallic_gun );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_gallic_gun]->min_mana;
  return;
}

void do_thunder_flash( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_thunder_flash ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_thunder_flash]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_thunder_flash]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_thunder_flash]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_thunder_flash]->focus;

  WAIT_STATE( ch, skill_table[gsn_thunder_flash]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_thunder_flash ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 85, 100 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
//    act( AT_RED, "You extend both arms to either side of yourself, and exclaim 'THUNDER' then bring your arms infront of you, fists together, exclaiming 'FLASH' then sends a massive column of fire out of your fists toward $N!!!'", ch, NULL, victim, TO_CHAR );
    act( AT_RED,
         "You extend both arms to either side of yourself, and exclaim 'THUNDER' then bring your arms infront of you, fists together, exclaiming 'FLASH' then sends a massive column of fire out of your fists toward $N!!!! &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
//    act( AT_RED, "$n puts $s arms back and cups $s hands. 'THUNDER FLASH!!!!'", ch, NULL, victim, TO_VICT );
    act( AT_RED,
         "$n extend both arms to either side of themself, and exclaim 'THUNDER' then bring their arms infront of them, fists together, exclaiming 'FLASH' then sends a massive column of fire out of $s fists toward you!!!! &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
//    act( AT_RED, "$n extend both arms to either side of themself, and exclaim 'THUNDER' then bring their arms infront of them, fists together, exclaiming 'FLASH' then sends a massive column of fire out of $s fists toward $N!!!!'", ch, NULL, victim, TO_NOTVICT );
    act( AT_RED,
         "$n extend both arms to either side of themself, and exclaim 'THUNDER' then bring their arms infront of them, fists together, exclaiming 'FLASH' then sends a massive column of fire out of $s fists toward $N!!!! &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_thunder_flash );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_RED, "You missed $N with your Thunder Flash Attack.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n misses you with $s Thunder Flash Attack.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n missed $N with a Thunder Flash.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_thunder_flash );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_thunder_flash]->min_mana;
  return;
}

void do_burning_attack( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_burning_attack ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_burning_attack]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_burning_attack]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_burning_attack]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_burning_attack]->focus;

  WAIT_STATE( ch, skill_table[gsn_burning_attack]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_burning_attack ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 24, 28 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_YELLOW,
         "You hold your hands in front of you, fingers splayed around a diamond shape formed by thumbs and fingers.", ch,
         NULL, victim, TO_CHAR );
    act( AT_YELLOW,
         "Suddenly you begin to gesture wildly, hands blurring through the air in a flurry of movement.  You freeze, hands forming the same diamond shape, a pulsing ball of light appearing and launching towards $N with surprising speed. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_YELLOW,
         "$n holds $s hands in front of $mself, fingers splayed around a diamond shape formed by thumbs and fingers.", ch,
         NULL, victim, TO_VICT );
    act( AT_YELLOW,
         "Suddenly $n begins to gesture wildly, hands blurring through the air in a flurry of movement.  $n freezes, hands forming the same diamond shape, a pulsing ball of light appearing and launching towards you with surprising speed. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_YELLOW,
         "$n holds $s hands in front of $mself, fingers splayed around a diamond shape formed by thumbs and fingers.", ch,
         NULL, victim, TO_NOTVICT );
    act( AT_YELLOW,
         "Suddenly $n begins to gesture wildly, hands blurring through the air in a flurry of movement.  $n freezes, hands forming the same diamond shape, a pulsing ball of light appearing and launching towards $N with surprising speed. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_burning_attack );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_YELLOW, "You missed $N with your burning attack.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "$n misses you with $s burning attack.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n missed $N with a burning attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_burning_attack );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_burning_attack]->min_mana;
  return;
}

void do_finishing_buster( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_finishing_buster ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_finishing_buster]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_finishing_buster]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_finishing_buster]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_finishing_buster]->focus;

  WAIT_STATE( ch, skill_table[gsn_finishing_buster]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_finishing_buster ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 65, 70 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_BLUE,
         "You raise both hands over your head, gathering your energy.  A large, blue ball of ki gathers above your head, crackling with electricity.",
         ch, NULL, victim, TO_CHAR );
    act( AT_BLUE,
         "You fling the ball of energy towards $N, putting your whole body into the motion.  'Finishing Buster!'  The sphere flies quickly, smashing soundly into $N. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_BLUE,
         "$n raises both hands over $s head, gathering $s energy.  A large, blue ball of ki gathers above $s head, crackling with electricity.",
         ch, NULL, victim, TO_VICT );
    act( AT_BLUE,
         "$n flings the ball of energy towards you, putting $s whole body into the motion.  'Finishing Buster!'  The sphere flies quickly, smashing soundly into you. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_BLUE,
         "$n raises both hands over $s head, gathering $s energy.  A large, blue ball of ki gathers above $s head, crackling with electricity.",
         ch, NULL, victim, TO_NOTVICT );
    act( AT_BLUE,
         "$n flings the ball of energy towards $N, putting $s whole body into the motion.  'Finishing Buster!'  The sphere flies quickly, smashing soundly into $N. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_finishing_buster );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_BLUE, "You missed $N with your finishing buster attack.", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "$n misses you with $s finishing buster attack.", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "$n missed $N with a finishing buster attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_finishing_buster );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_finishing_buster]->min_mana;
  return;
}

void do_heaven_splitter_cannon( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_heaven_splitter_cannon ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_heaven_splitter_cannon]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_heaven_splitter_cannon]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_heaven_splitter_cannon]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_heaven_splitter_cannon]->focus;

  WAIT_STATE( ch, skill_table[gsn_heaven_splitter_cannon]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_heaven_splitter_cannon ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 54, 58 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_YELLOW,
         "You cup both hands, holding them close together, right in front of your stomach.  A blindingly bright ball of energy begins forming, growing ever larger.",
         ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW,
         "With a howl, you thrust both hands forward, flinging the huge ball of energy into $N and creating an enormous explosion. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_YELLOW,
         "$n cups both hands, holding them close together, right in front of $s stomach.  A blindingly bright ball of energy begins forming, growing ever larger.",
         ch, NULL, victim, TO_VICT );
    act( AT_YELLOW,
         "With a howl, $n thrusts both hands forward, flinging the huge ball of energy into you and creating an enourmous explosion. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_YELLOW,
         "$n cups both hands, holding them close together, right in front of $s stomach.  A blindingly bright ball of energy begins forming, growing ever larger.",
         ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW,
         "With a howl, $n thrusts both hands forward, flinging the huge ball of energy into $N and creating an enormous explosion. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_heaven_splitter_cannon );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_YELLOW, "You missed $N with your heaven-splitter cannon attack.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "$n misses you with $s heaven-splitter cannon attack.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n missed $N with a heaven-splitter cannon attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_heaven_splitter_cannon );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_heaven_splitter_cannon]->min_mana;
  return;
}

void do_hellzone_grenade( CHAR_DATA * ch, char *arg )
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int shot = 0;
  int shots = 0;
  int dam = 0;
  int energy = 0;
  int damPerShot = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_hellzone_grenade ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_hellzone_grenade]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( arg[0] == '\0' || atoi( arg ) <= 0 )
  {
    send_to_char( "Syntax: 'hellzone grenade' <# of Shots>\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  shot = atoi( arg );
  if( shot > 50 )
    shot = 50;
  energy = shot * skill_table[gsn_hellzone_grenade]->min_mana;
  shots = shot;
  strcpy( buf, num_punct( shot ) );

  if( ch->mana < energy )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  //if (ch->focus < (skill_table[gsn_hellzone_grenade]->focus * (1+(shot/10))))
  // Replaced this with something better. -Karma.
  int focus = shots;
  if( focus < 12 )
    focus = 12; // Minimum focus
  if( ch->focus < focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= focus;
  //ch->focus -= (skill_table[gsn_hellzone_grenade]->focus * (1+(shot/10)));

  int wait = ( focus / 12 );
  if( wait < 2 )
    wait = 2;
  WAIT_STATE( ch, ( skill_table[gsn_hellzone_grenade]->beats * wait ) );
  //WAIT_STATE( ch, (skill_table[gsn_hellzone_grenade]->beats * (1+(shot/10))) );
  if( can_use_skill( ch, number_percent(  ), gsn_hellzone_grenade ) )
  {
    while( shots > 0 )
    {
      switch ( number_range( 1, 4 ) )
      {
        default:
        case 1:
          break;
        case 2:
          dam += number_range( 0, 1 );
          break;
        case 3:
          dam += 1;
          break;
        case 4:
          dam += number_range( 1, 2 );
          break;
      }
      shots--;
    }
    damPerShot = number_range( 1, 3 );
    strcpy( buf2, num_punct( get_attmod( ch, victim ) * dam * damPerShot ) );

    act( AT_YELLOW, "You power up, launching $t energy balls that all seem to miss $N by a huge margin.", ch, buf, victim,
         TO_CHAR );
    act( AT_YELLOW,
         "Suddenly, the balls freeze in midair, trapping $N in a deadly pattern.  The energy balls converge on $N, making $m the center of a gigantic explosion. &W[$t]",
         ch, buf2, victim, TO_CHAR );
    act( AT_YELLOW, "$n powers up, launching $t energy balls that all seem to miss you by a huge margin.", ch, buf, victim,
         TO_VICT );
    act( AT_YELLOW,
         "Suddenly, the balls freeze in midair, trapping you in a deadly pattern.  The energy balls converge on you, making you the center of a gigantic explosion. &W[$t]",
         ch, buf2, victim, TO_VICT );
    act( AT_YELLOW, "$n powers up, launching $t energy balls that all seem to miss $N by a huge margin.", ch, buf, victim,
         TO_VICT );
    act( AT_YELLOW,
         "Suddenly, the balls freeze in midair, trapping $N in a deadly pattern.  The energy balls converge on $N, making you the center of a gigantic explosion. &W[$t]",
         ch, buf2, victim, TO_VICT );
    act( AT_YELLOW, "$n powers up, launching $t energy balls that all seem to miss $N by a huge margin.", ch,
         num_punct( dam ), victim, TO_NOTVICT );
    act( AT_YELLOW,
         "Suddenly, the balls freeze in midair, trapping $N in a deadly pattern.  The energy balls converge on $N, making $M the center of a gigantic explosion. &W[$t]",
         ch, buf2, victim, TO_NOTVICT );

    learn_from_success( ch, gsn_hellzone_grenade );
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * dam * damPerShot ), TYPE_HIT );
  }
  else
  {
    act( AT_YELLOW, "You launch a barrage of $t energy balls towards $N but $E is to fast to hit.", ch, buf, victim,
         TO_CHAR );
    act( AT_YELLOW, "$n launches a barrage of $t energy balls towards $N but you are to fast to hit.", ch, buf, victim,
         TO_VICT );
    act( AT_YELLOW, "$n launches a barrage of $t energy balls towards $N but $E is to fast to hit.", ch, buf, victim,
         TO_NOTVICT );
    learn_from_failure( ch, gsn_hellzone_grenade );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= energy;
  return;
}

void do_makosen( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_makosen ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_makosen]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_makosen]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_makosen]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_makosen]->focus;

  WAIT_STATE( ch, skill_table[gsn_makosen]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_makosen ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 23, 27 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_YELLOW, "You pull both hands to one side of your body, cupping them as a ball of ki begins forming.", ch, NULL,
         victim, TO_CHAR );
    act( AT_YELLOW, "'Makosen!'  You thrust your hands in front of you, firing a thin beam of energy towards $N. &W[$t]", ch,
         num_punct( dam ), victim, TO_CHAR );
    act( AT_YELLOW, "$n pulls both hands to one side of $s body, cupping them as a ball of ki begins forming.", ch, NULL,
         victim, TO_VICT );
    act( AT_YELLOW, "'Makosen!'  $n thrusts $s hands in front of $m, firing a thin beam of energy towards you. &W[$t]", ch,
         num_punct( dam ), victim, TO_VICT );
    act( AT_YELLOW, "$n pulls both hands to one side of $s body, cupping them as a ball of ki begins forming.", ch, NULL,
         victim, TO_NOTVICT );
    act( AT_YELLOW, "'Makosen!'  $n thrusts $s hands in front of $m, firing a thin beam of energy towards $N. &W[$t]", ch,
         num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_makosen );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_YELLOW, "You missed $N with your makosen attack.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "$n misses you with $s makosen attack.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n missed $N with a makosen attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_makosen );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_makosen]->min_mana;
  return;
}

void do_trap_ball( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_trap_ball ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_trap_ball]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_trap_ball]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_trap_ball]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_trap_ball]->focus;

  WAIT_STATE( ch, skill_table[gsn_trap_ball]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_trap_ball ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 32, 36 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_YELLOW, "You form an energy ball, tossing it at $N.  As it impacts $M, the ball expands, trapping $M within it.",
         ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "You bat and toss the giant cage around, leaving $N trapped inside of the deadly cage.", ch, NULL,
         victim, TO_CHAR );
    act( AT_YELLOW, "The energy ball trapping $N bumps into a piece of scenery, suddenly exploding and freeing $M. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_YELLOW,
         "$n forms an energy ball, tossing it at you.  As it impacts you, the ball expands, trapping you within it.", ch,
         NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n bats and tosses the giant cage around, leaving you trapped inside of the deadly cage.", ch, NULL,
         victim, TO_VICT );
    act( AT_YELLOW, "The energy ball trapping you bumps into a piece of scenery, suddenly exploding and freeing you. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_YELLOW, "$n forms an energy ball, tossing it at $N.  As it impacts $M, the ball expands, trapping $N within it.",
         ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW, "$n bats and tosses the giant cage around, leaving $N trapped inside of the deadly cage.", ch, NULL,
         victim, TO_NOTVICT );
    act( AT_YELLOW, "The energy ball trapping $N bumps into a piece of scenery, suddenly exploding and freeing $N. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_trap_ball );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_YELLOW, "You missed $N with your trap ball attack.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "$n misses you with $s trap ball attack.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n missed $N with a trap ball attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_trap_ball );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_trap_ball]->min_mana;
  return;
}

void do_scattered_finger_beam( CHAR_DATA * ch, char *arg )
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int shot = 0;
  int shots = 0;
  int dam = 0;
  int dam2 = 0;
  int energy = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_scattered_finger_beam ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_scattered_finger_beam]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( arg[0] == '\0' || atoi( arg ) <= 0 )
  {
    send_to_char( "Syntax: 'scattered finger beam' <# of beams>\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  shot = atoi( arg );
  if( shot > 50 )
    shot = 50;
  if( shot < 2 )
    shot = 2;
  energy = shot * skill_table[gsn_scattered_finger_beam]->min_mana;
  shots = shot;
  strcpy( buf, num_punct( shot ) );

  if( ch->mana < energy )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < ( skill_table[gsn_scattered_finger_beam]->focus * ( 1 + ( shot / 10 ) ) ) )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= ( skill_table[gsn_scattered_finger_beam]->focus * ( 1 + ( shot / 10 ) ) );

  WAIT_STATE( ch, ( skill_table[gsn_scattered_finger_beam]->beats ) );

  if( can_use_skill( ch, number_percent(  ), gsn_scattered_finger_beam ) )
  {
    while( shots > 0 )
    {
      switch ( number_range( 1, 3 ) )
      {
        default:
        case 1:
          break;
        case 2:
          dam += number_range( 0, 1 );
          break;
        case 3:
          dam += number_range( 0, 2 );
          break;
      }
      shots--;
    }
    dam2 = number_range( 2, 5 );  // was 5-7
    strcpy( buf2, num_punct( get_attmod( ch, victim ) * dam * dam2 ) );

    act( AT_PINK, "You point one finger from each hand at $N, repeatedly jabbing them at $M.", ch, NULL, victim, TO_CHAR );
    act2( AT_PINK, "$N is staggered as $t tiny beams of energy are fired at $S from your stabbing fingers. &W[$T]", ch, buf,
          victim, TO_CHAR, buf2 );
    act( AT_PINK, "$n points one finger from each hand at you, repeatedly jabbing them at you.", ch, NULL, victim, TO_VICT );
    act2( AT_PINK, "You stagger as $t tiny beams of energy are fired at you from $n's stabbing fingers. &W[$T]", ch, buf,
          victim, TO_VICT, buf2 );
    act( AT_PINK, "$n points one finger from each hand at $N, repeatedly jabbing them at $M.", ch, NULL, victim,
         TO_NOTVICT );
    act2( AT_PINK, "$N staggers as $t tiny beams of energy are fired at $M from $n's stabbing fingers. &W[$T]", ch, buf,
          victim, TO_NOTVICT, buf2 );

    learn_from_success( ch, gsn_scattered_finger_beam );
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * dam * dam2 ), TYPE_HIT );
  }
  else
  {
    act( AT_PINK, "$t beams of energy fly from your finger towards $N but $E is too fast to hit.", ch, buf, victim,
         TO_CHAR );
    act( AT_PINK, "$t beams of energy fly from $n's finger towards you but you dodge them.", ch, buf, victim, TO_VICT );
    act( AT_PINK, "$t beams of energy fly from $n's finger towards $N but $E is too fast to hit.", ch, buf, victim,
         TO_NOTVICT );
    learn_from_failure( ch, gsn_scattered_finger_beam );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= energy;
  return;
}

void do_evilboost( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_evilboost ) )
      return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  double pl_mult = 0;
  int arg;
  /*
   * int sizeStr=0, sizeSpd=0, sizeCon=0, sizeInt=0;
   */

  arg = atoi( argument );

  if( xIS_SET( ( ch )->affected_by, AFF_EVILBOOST ) && ( arg == 0 ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_EVILBOOST );
    transStatRemove( ch );
    if( xIS_SET( ( ch )->affected_by, AFF_MAKEOSTAR ) )
      ch->pl = ch->exp * 10;
    else
      ch->pl = ch->exp;
    ch->evilmod = 0;
    return;
  }
  else if( !xIS_SET( ( ch )->affected_by, AFF_EVILBOOST ) && arg == 0 )
  {
    ch_printf( ch, "You're not in that form, though!\n\r" );
    return;
  }

  pl_mult = 9;

  if( arg > pl_mult )
    pl_mult = ( int )pl_mult;
  else if( arg < 3 )
    pl_mult = 3;
  else
    pl_mult = arg;

  if( ch->mana < skill_table[gsn_evilboost]->min_mana * pl_mult )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  /*
   * Added what Karma forgot -Karn 01.29.05 
   */
  if( arg < 3 || arg > 9 )
  {
    ch_printf( ch, "Syntax: 'evil boost' [3-9]\n\r" );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_evilboost]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_evilboost ) )
  {
    act( AT_RED,
         "You focus your thoughts on all that is evil, vile, and wicked. Yelling out, your body size suddenly increases as your muscles bulge with evil energy.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_RED,
         "$n focuses $s thoughts on all that is evil, vile, and wicked. $*e yells out, $s body size suddenly increasing as $s muscles bulge with evil energy.",
         ch, NULL, NULL, TO_NOTVICT );

    xSET_BIT( ( ch )->affected_by, AFF_EVILBOOST );

    if( xIS_SET( ( ch )->affected_by, AFF_EVILOVERLOAD ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_EVILOVERLOAD );

    if( xIS_SET( ( ch )->affected_by, AFF_EVILSURGE ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_EVILSURGE );

    if( xIS_SET( ( ch )->affected_by, AFF_MAKEOSTAR ) )
      ch->pl = ch->exp * ( pl_mult + 10 );
    else
      ch->pl = ch->exp * pl_mult;

    ch->evilmod = pl_mult;

    transStatApply( ch, pl_mult, pl_mult / 2, pl_mult / 2, pl_mult );


    learn_from_success( ch, gsn_evilboost );
  }
  else
  {
    act( AT_SKILL, "You can't draw out enough evil power to transform.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n can't draw out enough evil power to transform.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_evilboost );
  }

  ch->mana -= skill_table[gsn_evilboost]->min_mana * pl_mult;
  return;
}

void do_evilsurge( CHAR_DATA * ch, char *argument )
{

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_evilsurge ) )
      return;
  }

  double pl_mult = 0;
  int arg;
  /*
   * int sizeStr=0, sizeSpd=0, sizeCon=0, sizeInt=0;
   */

  arg = atoi( argument );

  if( xIS_SET( ( ch )->affected_by, AFF_EVILSURGE ) && ( arg == 0 ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_EVILSURGE );
    transStatRemove( ch );
    if( xIS_SET( ( ch )->affected_by, AFF_MAKEOSTAR ) )
      ch->pl = ch->exp * 10;
    else
      ch->pl = ch->exp;
    ch->evilmod = 0;
    return;
  }
  else if( !xIS_SET( ( ch )->affected_by, AFF_EVILSURGE ) && arg == 0 )
  {
    ch_printf( ch, "You're not in that form, though!\n\r" );
    return;
  }

  pl_mult = 22;

  if( arg > pl_mult )
    pl_mult = ( int )pl_mult;
  else if( arg < 10 )
    pl_mult = 10;
  else
    pl_mult = arg;

  if( ch->mana < skill_table[gsn_evilsurge]->min_mana * pl_mult )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  /*
   * Added what Karma forgot -Karn 01.29.05 
   */
  if( arg < 10 || arg > 22 )
  {
    ch_printf( ch, "Syntax: 'evil surge' [10-22]\n\r" );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_evilsurge]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_evilsurge ) )
  {
    act( AT_RED,
         "You flood your mind with images of unspeakable horrors and atrocities. Screaming out, the ground beneath you trembles and cracks as your body grows larger, your muscles surging with tremendous, evil power.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_RED,
         "$n floods $s mind with images of unspeakable horrors and atrocities. Screaming out, the ground beneath $m trembles and cracks as $s body grows larger, $s muscles surging with tremendous, evil power.",
         ch, NULL, NULL, TO_NOTVICT );

    xSET_BIT( ( ch )->affected_by, AFF_EVILSURGE );

    if( xIS_SET( ( ch )->affected_by, AFF_EVILBOOST ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_EVILBOOST );

    if( xIS_SET( ( ch )->affected_by, AFF_EVILOVERLOAD ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_EVILOVERLOAD );

    if( xIS_SET( ( ch )->affected_by, AFF_MAKEOSTAR ) )
      ch->pl = ch->exp * ( pl_mult + 10 );
    else
      ch->pl = ch->exp * pl_mult;

    ch->evilmod = pl_mult;

    transStatApply( ch, pl_mult, pl_mult / 2, pl_mult / 2, pl_mult );



    learn_from_success( ch, gsn_evilsurge );
  }
  else
  {
    act( AT_SKILL, "You can't draw out enough evil power to transform.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n can't draw out enough evil power to transform.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_evilsurge );
  }

  ch->mana -= skill_table[gsn_evilsurge]->min_mana * pl_mult;
  return;
}

void do_eviloverload( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_eviloverload ) )
      return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  double pl_mult = 0;
  int arg;
  /*
   * int sizeStr=0, sizeSpd=0, sizeCon=0, sizeInt=0;
   */

  arg = atoi( argument );

  if( xIS_SET( ( ch )->affected_by, AFF_EVILOVERLOAD ) && ( arg == 0 ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_EVILOVERLOAD );
    transStatRemove( ch );
    if( xIS_SET( ( ch )->affected_by, AFF_MAKEOSTAR ) )
      ch->pl = ch->exp * 10;
    else
      ch->pl = ch->exp;
    ch->evilmod = 0;
    return;
  }
  else if( !xIS_SET( ( ch )->affected_by, AFF_EVILOVERLOAD ) && arg == 0 )
  {
    ch_printf( ch, "You're not in that form, though!\n\r" );
    return;
  }

  pl_mult = 36;

  if( arg > pl_mult )
    pl_mult = ( int )pl_mult;
  else if( arg < 24 )
    pl_mult = 24;
  else
    pl_mult = arg;

  if( ch->mana < skill_table[gsn_eviloverload]->min_mana * pl_mult )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  /*
   * Added what Karma forgot -Karn 01.29.05 
   */
  if( arg < 24 || arg > 36 )
  {
    ch_printf( ch, "Syntax: 'evil overload' [24-36]\n\r" );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_eviloverload]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_eviloverload ) )
  {
    act( AT_RED,
         "As you overload your mind with countless images of pure evil, your body begins to buckle under the strain of an incredible spike in powerlevel. The very air all around begins to wave under the intensity. Then all at once, your body explodes upward to a size of almost 20 feet; every corner of your being flowing with unspeakably evil power.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_RED,
         "As $n overloads $s mind with countless images of pure evil, $s body begins to buckle under the strain of an incredible spike in powerlevel. The very air all around $n to wave under the intensity. Then all at once, $n's body explodes upward to a size of almost 20 feet; every corner of $s being flowing with unspeakably evil power.",
         ch, NULL, NULL, TO_NOTVICT );

    xSET_BIT( ( ch )->affected_by, AFF_EVILOVERLOAD );

    if( xIS_SET( ( ch )->affected_by, AFF_EVILBOOST ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_EVILBOOST );

    if( xIS_SET( ( ch )->affected_by, AFF_EVILSURGE ) )
      xREMOVE_BIT( ( ch )->affected_by, AFF_EVILSURGE );

    if( xIS_SET( ( ch )->affected_by, AFF_MAKEOSTAR ) )
      ch->pl = ch->exp * ( pl_mult + 10 );
    else
      ch->pl = ch->exp * pl_mult;

    ch->evilmod = pl_mult;

    transStatApply( ch, pl_mult, pl_mult / 2, pl_mult / 2, pl_mult );


    learn_from_success( ch, gsn_eviloverload );
  }
  else
  {
    act( AT_SKILL, "You can't draw out enough evil power to transform.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n can't draw out enough evil power to transform.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_eviloverload );
  }

  ch->mana -= skill_table[gsn_eviloverload]->min_mana * pl_mult;
  return;
}

void do_demonweapon( CHAR_DATA * ch, char *argument )
{
  OBJ_DATA *o;
  OBJ_INDEX_DATA *pObjIndex;
  //AFFECT_DATA *paf;
  AFFECT_DATA *aff_str;
  AFFECT_DATA *aff_dex;
  AFFECT_DATA *aff_int;
  AFFECT_DATA *aff_con;

  int auraColor = AT_YELLOW;

  if( IS_NPC( ch ) )
    return;

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( argument[0] == '\0' )
  {
    ch_printf( ch, "\n\rSyntax: 'Demon weapon' <item>\n\r" );
    ch_printf( ch, "\n\r" );
    ch_printf( ch, "Item can only be one of the following:\n\r" );
    ch_printf( ch, "\n\r" );
    ch_printf( ch, "scimitar shortsword longsword broadsword lance maul\n\r" );
    return;
  }

  if( !( !str_cmp( argument, "scimitar" ) ) && !( !str_cmp( argument, "shortsword" ) ) &&
      !( !str_cmp( argument, "longsword" ) ) && !( !str_cmp( argument, "broadsword" ) ) &&
      !( !str_cmp( argument, "lance" ) ) && !( !str_cmp( argument, "maul" ) ) )
  {
    do_demonweapon( ch, "" );
    return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_demonweapon]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }
  if( ch->mana < ch->max_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->hit < 100 )
  {
    ch_printf( ch, "You don't have enough health.\n\r", ch );
    return;
  }
  if( ch->carry_number >= 5 )
  {
    ch_printf( ch, "You haven't got any room.\n\r" );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_demonweapon]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_demonweapon ) )
  {
    int a = 0;
    if( !str_cmp( argument, "scimitar" ) )
      a = 1260;
    else if( !str_cmp( argument, "shortsword" ) )
      a = 1261;
    else if( !str_cmp( argument, "longsword" ) )
      a = 1262;
    else if( !str_cmp( argument, "broadsword" ) )
      a = 1263;
    else if( !str_cmp( argument, "lance" ) )
      a = 1264;
    else if( !str_cmp( argument, "maul" ) )
      a = 1265;
    if( a == 0 )
    {
      bug( "Serious problem in function demon weapon", 0 );
      return;
    }

    auraColor = ch->pcdata->auraColorPowerUp;

    if( !str_cmp( argument, "scimitar" ) )
    {
      act( auraColor,
           "You begin to push a tremendous amount of energy into your arms; bloating your muscles up to a very large size. The moment your arms feel like they are going to explode, a twin pair of demon scimitars bursts out of them and into your hands; your arms healing back up almost instantly.",
           ch, NULL, NULL, TO_CHAR );
      act( auraColor,
           "$n begins to push a tremendous amount of energy into $s arms; bloating $s muscles up to a very large size. The moment $s arms look like they are going to explode, a twin pair of demon scimitars bursts out of them and into $s hands; $s arms healing back up almost instantly.",
           ch, NULL, NULL, TO_NOTVICT );
    }
    if( !str_cmp( argument, "shortsword" ) || !str_cmp( argument, "longsword" ) || !str_cmp( argument, "broadsword" ) )
    {
      act( auraColor,
           "The pupils of your eyes suddenly glow bright red as you grab a spike sticking out of your leg in one hand and begin to pull on it. Yelling out, your flesh is torn away as you pull a gleaming demon sword out of your leg. Your leg heals back up almost instantly.",
           ch, NULL, NULL, TO_CHAR );
      act( auraColor,
           "The pupils of $n's eyes suddenly glow bright red as $e grabs a spike sticking out of $s leg in one hand and begins to pull on it. Yelling out, $s flesh is torn away as $n pulls a gleaming demon sword out of $s leg. $n's leg heals back up almost instantly.",
           ch, NULL, NULL, TO_NOTVICT );
    }
    if( !str_cmp( argument, "lance" ) )
    {
      act( auraColor,
           "With a spray of demon blood you rip off your left arm with your right hand. The severed limb begins to bulge as you pump your own energy into it. It starts to stretch out from end to end, when all of a sudden it explodes, turning into a demon lance. Your left arm regenerates almost instantly after.",
           ch, NULL, NULL, TO_CHAR );
      act( auraColor,
           "With a spray of demon blood $n rips off $s left arm with $s right hand. The severed limb begins to bulge as $n pumps $s own energy into it. It starts to stretch out from end to end, when all of a sudden it explodes, turning into a demon lance. $n's left arm regenerates almost instantly after.",
           ch, NULL, NULL, TO_NOTVICT );
    }
    if( !str_cmp( argument, "maul" ) )
    {
      act( auraColor,
           "With a spray of demon blood you rip off your right arm with your left hand. The severed limb begins to bulge as you pump your own energy into it. It starts to stretch out from end to end, when all of a sudden it explodes, turning into a demon maul. Your right arm regenerates almost instantly after.",
           ch, NULL, NULL, TO_CHAR );
      act( auraColor,
           "With a spray of demon blood $n rips off $s right arm with $s left hand. The severed limb begins to bulge as $n pumps $s own energy into it. It starts to stretch out from end to end, when all of a sudden it explodes, turning into a demon maul. $n's right arm regenerates almost instantly after.",
           ch, NULL, NULL, TO_NOTVICT );
    }
    learn_from_success( ch, gsn_demonweapon );
    pObjIndex = get_obj_index( a );
    o = create_object_new( pObjIndex, 1, ORIGIN_OINVOKE, ch->name );
    /*
     * while( (paf = o->pIndexData->first_affect) )
     * {
     * UNLINK(paf, o->pIndexData->first_affect, 
     * o->pIndexData->last_affect, next, prev );
     * DISPOSE(paf);
     * }
     */

    CREATE( aff_str, AFFECT_DATA, 1 );
    CREATE( aff_dex, AFFECT_DATA, 1 );
    CREATE( aff_int, AFFECT_DATA, 1 );
    CREATE( aff_con, AFFECT_DATA, 1 );
    // Strength aff
    aff_str->type = -1;
    aff_str->duration = -1;
    aff_str->location = APPLY_STR;
    xCLEAR_BITS( aff_str->bitvector );
    LINK( aff_str, o->first_affect, o->last_affect, next, prev );
    // Speed aff
    aff_dex->type = -1;
    aff_dex->duration = -1;
    aff_dex->location = APPLY_DEX;
    xCLEAR_BITS( aff_dex->bitvector );
    LINK( aff_dex, o->first_affect, o->last_affect, next, prev );
    // Int aff
    aff_int->type = -1;
    aff_int->duration = -1;
    aff_int->location = APPLY_INT;
    xCLEAR_BITS( aff_int->bitvector );
    LINK( aff_int, o->first_affect, o->last_affect, next, prev );
    // Con aff
    aff_con->type = -1;
    aff_con->duration = -1;
    aff_con->location = APPLY_CON;
    xCLEAR_BITS( aff_con->bitvector );
    LINK( aff_con, o->first_affect, o->last_affect, next, prev );

    int newmod = 1;
    if( ch->exp < 100000000 )
      newmod = 5;
    else if( ch->exp < 1000000000 )
      newmod = 8;
    else if( ch->exp < 10000000000ULL )
      newmod = 12;
    else if( ch->exp < 100000000000ULL )
      newmod = 15;
    else if( ch->exp < 500000000000ULL )
      newmod = 20;
    else if( ch->exp < 1000000000000ULL )
      newmod = 24;
    else if( ch->exp < 10000000000000ULL )
      newmod = 28;
    else if( ch->exp < 50000000000000ULL )
      newmod = 34;
    else if( ch->exp < 100000000000000ULL )
      newmod = 40;
    else if( ch->exp >= 100000000000000ULL )
      newmod = 50;
    else
      newmod = 1;

    if( newmod > 1 )
    {
      o->value[1] = newmod;
      o->value[2] = 2;
    }

    aff_str->modifier = newmod;
    aff_dex->modifier = newmod;
    aff_int->modifier = newmod;
    aff_con->modifier = newmod;

    o = obj_to_char( o, ch );
    ch->hit -= 80;
    save_char_obj( ch );
  }
  else
  {
    act( AT_SKILL, "You fail to use your demon powers to create a weapon.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n fails to use $s demon powers to create a weapon.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_demonweapon );
  }
  ch->mana = 0;
  return;
}

void do_wss( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  int dam = 0;
  bool scimitar = FALSE, sword = FALSE, lance = FALSE, maul = FALSE;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_wss ) )
      return;
  }

  if( !IS_DEMON( ch ) || ch->rank < 1 )
  {
    ch_printf( ch, "Huh?\n\r" );
    return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_wss]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }
  if( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
  {
    ch_printf( ch, "You must be wielding a weapon.\n\r" );
    return;
  }
  if( ch->mana < skill_table[gsn_wss]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_wss]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_wss]->focus;

  if( obj->pIndexData->vnum == 1260 )
    scimitar = TRUE;
  else if( obj->pIndexData->vnum == 1264 )
    lance = TRUE;
  else if( obj->pIndexData->vnum == 1265 )
    maul = TRUE;
  else
    sword = TRUE;

  WAIT_STATE( ch, skill_table[gsn_wss]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_wss ) )
  {
    switch ( number_range( 1, 100 ) )
    {
      case 100:
      case 99:
      case 98:
        if( IS_NPC( victim ) || ( !IS_NPC( victim ) && ( !xIS_SET( ch->act, PLR_SPAR ) || !xIS_SET( ch->act, PLR_SPAR ) ) ) )
        {

          /*
           * Redone so that instant death can't be abused to instant kill
           * * players over 2x their power.
           */
          if( victim->pl / ch->pl >= 2 )
          {
            if( scimitar )
              wss_scimitar( ch, victim, "ikfail", 0 );
            else if( lance )
              wss_lance( ch, victim, "ikfail", 0 );
            else if( maul )
              wss_maul( ch, victim, "ikfail", 0 );
            else if( sword )
              wss_sword( ch, victim, "ikfail", 0 );
            dam = 0;
            break;
          }

/* Redone so it _is_ instant death
 *      if (victim->max_hit >= victim->hit)
 *              dam = victim->max_hit * 2;
 *      else
 *              dam = victim->hit * 2;
 */
          dam = 999999999;

          if( scimitar )
            wss_scimitar( ch, victim, "ikwork", dam );
          else if( lance )
            wss_lance( ch, victim, "ikwork", dam );
          else if( maul )
            wss_maul( ch, victim, "ikwork", dam );
          else if( sword )
            wss_sword( ch, victim, "ikwork", dam );

          learn_from_success( ch, gsn_wss );
          global_retcode = damage( ch, victim, dam, TYPE_HIT );
          break;
        }
      default:
        dam = get_attmod( ch, victim ) * number_range( 60, 75 );
        if( ch->charge > 0 )
          dam = chargeDamMult( ch, dam );

        if( scimitar )
          wss_scimitar( ch, victim, "normal", dam );
        else if( lance )
          wss_lance( ch, victim, "normal", dam );
        else if( maul )
          wss_maul( ch, victim, "normal", dam );
        else if( sword )
          wss_sword( ch, victim, "normal", dam );

        learn_from_success( ch, gsn_wss );
        global_retcode = damage( ch, victim, dam, TYPE_HIT );
        break;
    }
  }
  else
  {
    act( AT_RED, "You missed $N with your weapon soul seppuku.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n misses you with $s weapon soul seppuku.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n missed $N with a weapon soul seppuku.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_wss );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_wss]->min_mana;

  obj_from_char( obj );
  extract_obj( obj );

  return;
}

void do_darkness_flare( CHAR_DATA * ch, char *argument )
{
  AFFECT_DATA af;
  CHAR_DATA *vch, *vch_next;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_darkness_flare ) )
      return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_darkness_flare]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }
  if( xIS_SET( ch->in_room->room_flags, ROOM_ARENA ) || xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
  {
    send_to_char( "There's no need to use that here.\n\r", ch );
    return;
  }
  if( who_fighting( ch ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }
  if( ch->mana < skill_table[gsn_darkness_flare]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_darkness_flare]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_darkness_flare]->focus;

  WAIT_STATE( ch, skill_table[gsn_darkness_flare]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_darkness_flare ) )
  {
    act( AT_DGREY,
         "You put your hands to your face and yell out 'DARKNESS FLARE!' emitting a blast of unpenetrable darkness.", ch,
         NULL, NULL, TO_CHAR );
    act( AT_DGREY,
         "$n puts $s hands to $s face and yells out 'DARKNESS FLARE!' blinding everyone in the room with unpenetrable darnkess!",
         ch, NULL, NULL, TO_NOTVICT );

    learn_from_success( ch, gsn_darkness_flare );
    for( vch = ch->in_room->first_person; vch; vch = vch_next )
    {

      vch_next = vch->next_in_room;
      if( !IS_AWAKE( vch ) )
        continue;

      if( IS_DEMON( vch ) || is_android( vch ) || is_superandroid( vch ) )
        continue;

      if( number_range( 1, 100 ) < ( get_curr_int( vch ) / 10 ) )
      {
        send_to_char( "You manage to look away at the last moment and shield " "your eyes.\n\r", vch );
        continue;
      }

      if( ch != vch )
      {
        af.type = gsn_darkness_flare;
        af.duration = 10;
        af.location = APPLY_HITROLL;
        af.modifier = -6;
        af.bitvector = meb( AFF_BLIND );
        affect_to_char( vch, &af );
      }

    }

  }
  else
  {
    act( AT_DGREY, "You put your hands to your face but forget the words. Nothing happens.", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "$n raises $s hands to $s head, gets a stupid look on $s face, then stands there.", ch, NULL, NULL,
         TO_NOTVICT );
    learn_from_failure( ch, gsn_darkness_flare );
  }
  ch->mana -= skill_table[gsn_darkness_flare]->min_mana;
  return;
}

void do_maliceray( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_maliceray ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_maliceray]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_maliceray]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_maliceray]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_maliceray]->focus;

  WAIT_STATE( ch, skill_table[gsn_maliceray]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_maliceray ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 22, 26 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_RED,
         "You aim your right hand at $N, and brace it with your left, quickly charging and firing a shimmering crimson blast of energy at $M. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_RED,
         "$n aims $s right hand at you, and braces it with $s left, quickly charging and firing a shimmering crimson blast of energy at you. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_RED,
         "$n aims $s right hand at $N, and braces it with $s left, quickly charging and firing a shimmering crimson blast of energy at $N. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_maliceray );
    learn_from_success( ch, gsn_maliceray );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_RED, "You missed $N with your malice ray.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n misses you with $s malice ray.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n missed $N with a malice ray.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_maliceray );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_maliceray]->min_mana;
  return;
}

void do_cleaver( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_cleaver ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_cleaver]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_cleaver]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_cleaver]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_cleaver]->focus;

  WAIT_STATE( ch, skill_table[gsn_cleaver]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_cleaver ) )
  {
    switch ( number_range( 1, 100 ) )
    {
      case 100:
        if( IS_NPC( victim ) || ( !IS_NPC( victim ) && ( !xIS_SET( ch->act, PLR_SPAR ) || !xIS_SET( ch->act, PLR_SPAR ) ) ) )
        {

          /*
           * Redone so that instant death can't be abused to instant kill
           * * players over 2x their power.
           */
          if( victim->pl / ch->pl >= 2 )
          {
            dam = get_attmod( ch, victim ) * number_range( 15, 16 );
            act( AT_DGREY, "You charge a spinning energy disk above your head and hurl it towards $N.", ch, NULL, victim,
                 TO_CHAR );
            act( AT_DGREY, "It flies staight towards $N, but its damage is absorbed by $s powerful aura.", ch, NULL, victim,
                 TO_CHAR );
            act( AT_DGREY, "$n charges a spinning energy disk above $s head and hurls it towards you.", ch, NULL, victim,
                 TO_VICT );
            act( AT_DGREY, "It flies staight towards you, but its damage is absorbed by your powerful aura.", ch, NULL,
                 victim, TO_VICT );
            act( AT_DGREY, "$n charges a spinning energy disk above $s head and hurls it towards $N.", ch, NULL, victim,
                 TO_NOTVICT );
            act( AT_DGREY, "It flies staight towards $N, but its damage is absorbed by $s powerful aura.", ch, NULL, victim,
                 TO_NOTVICT );
            learn_from_success( ch, gsn_cleaver );
            global_retcode = damage( ch, victim, dam, TYPE_HIT );
            break;
          }

/* Redone so it _is_ instant death
 *      if (victim->max_hit >= victim->hit)
 *              dam = victim->max_hit * 2;
 *      else
 *              dam = victim->hit * 2;
 */
          dam = 999999999;
          act( AT_DGREY, "You charge a spinning energy disk above your head and hurl it towards $N.", ch, NULL, victim,
               TO_CHAR );
          act( AT_RED, "IT CUTS STRAIGHT THROUGH $N!!!  Killing $M instantly.", ch, NULL, victim, TO_CHAR );
          act( AT_DGREY, "$n charges a spinning energy disk above $s head and hurls it towards you.", ch, NULL, victim,
               TO_VICT );
          act( AT_RED, "IT CUTS STRAIGHT THROUGH YOU!!!  Killing you instantly.", ch, NULL, victim, TO_VICT );
          act( AT_DGREY, "$n charges a spinning energy disk above $s head and hurls it towards $N.", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_RED, "IT CUTS STRAIGHT THROUGH $N!!!  Killing $M instantly.", ch, NULL, victim, TO_NOTVICT );

          learn_from_success( ch, gsn_cleaver );
          global_retcode = damage( ch, victim, dam, TYPE_HIT );
          break;
        }
      default:
        dam = get_attmod( ch, victim ) * number_range( 30, 42 );
        if( ch->charge > 0 )
          dam = chargeDamMult( ch, dam );
        act( AT_DGREY, "You charge a spinning energy disk above your head and hurl it towards $N. &W[$t]", ch,
             num_punct( dam ), victim, TO_CHAR );
        act( AT_DGREY, "$n charges a spinning energy disk above $s head and hurls it towards you. &W[$t]", ch,
             num_punct( dam ), victim, TO_VICT );
        act( AT_DGREY, "$n charges a spinning energy disk above $s head and hurls it towards $N. &W[$t]", ch,
             num_punct( dam ), victim, TO_NOTVICT );

        learn_from_success( ch, gsn_cleaver );
        global_retcode = damage( ch, victim, dam, TYPE_HIT );
        break;
    }
  }
  else
  {
    act( AT_DGREY, "You missed $N with your cleaver.", ch, NULL, victim, TO_CHAR );
    act( AT_DGREY, "$n misses you with $s cleaver.", ch, NULL, victim, TO_VICT );
    act( AT_DGREY, "$n missed $N with a cleaver.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_cleaver );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_cleaver]->min_mana;
  return;
}

void do_demonic_eclipse( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_demonic_eclipse ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_demonic_eclipse]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_demonic_eclipse]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_demonic_eclipse]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_demonic_eclipse]->focus;

  WAIT_STATE( ch, skill_table[gsn_demonic_eclipse]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_demonic_eclipse ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 56, 62 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_DGREY,
         "You clasp your hands together infront of yourself and begin to concentrate an immense amount of evil energy. A mixture of red and black flames begin to radiate slowly from your entire body. After a few moments, you pull your palms away from each other slowly; the energy radiating from you suddenly flows down into the space between your palms as a small, pitch black ball of energy forms, with a corona of dark red.",
         ch, NULL, victim, TO_CHAR );
    act( AT_DGREY,
         "Your eyes turn solid red as you yell out, the ground beneath you shattering and flying into the air, as you hoist the dark energy ball up over your head. 'DEMONIC ECLIPSE!!!' you shout as you throw the blast towards $N with incredible force. The attack quickly swells in size, untill it hits $N. $*E is momentarily engulfed by the energy, unspeakably tormented within a torrent of dark energies, before it explodes. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_DGREY,
         "$n clasps $s hands together infront of $mself and begins to concentrate an immense amount of evil energy. A mixture of red and black flames begin to radiate slowly from $s entire body. After a few moments, $e pull $s palms away from each other slowly; the energy radiating from $n suddenly flows down into the space between $s palms as a small, pitch black ball of energy forms, with a corona of dark red.",
         ch, NULL, victim, TO_VICT );
    act( AT_DGREY,
         "$n's eyes turn solid red as $e yells out, the ground beneath $m shattering and flying into the air, as $e hoists the dark energy ball up over $s head. 'DEMONIC ECLIPSE!!!' $n shouts as $e throws the blast towards you with incredible force. The attack quickly swells in size, untill it hits you. You are momentarily engulfed by the energy, unspeakably tormented within a torrent of dark energies, before it explodes. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_DGREY,
         "$n clasps $s hands together infront of $mself and begins to concentrate an immense amount of evil energy. A mixture of red and black flames begin to radiate slowly from $s entire body. After a few moments, $e pull $s palms away from each other slowly; the energy radiating from $n suddenly flows down into the space between $s palms as a small, pitch black ball of energy forms, with a corona of dark red.",
         ch, NULL, victim, TO_NOTVICT );
    act( AT_DGREY,
         "$n's eyes turn solid red as $e yells out, the ground beneath $m shattering and flying into the air, as $e hoists the dark energy ball up over $s head. 'DEMONIC ECLIPSE!!!' $n shouts as $e throws the blast towards $N with incredible force. The attack quickly swells in size, untill it hits $N. $*E is momentarily engulfed by the energy, unspeakably tormented within a torrent of dark energies, before it explodes. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    //dam = ki_absorb( victim, ch, dam, gsn_demonic_eclipse );
    learn_from_success( ch, gsn_demonic_eclipse );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_DGREY, "You missed $N with your demonic eclipse.", ch, NULL, victim, TO_CHAR );
    act( AT_DGREY, "$n misses you with $s demonic eclipse.", ch, NULL, victim, TO_VICT );
    act( AT_DGREY, "$n missed $N with a demonic eclipse.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_demonic_eclipse );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_demonic_eclipse]->min_mana;
  return;
}

void do_minion( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *wf;
  CHAR_DATA *victim;
  CHAR_DATA *victim2;
  MOB_INDEX_DATA *pMobIndex;

  if( IS_NPC( ch ) )
  {
    send_to_char( "NPC's can't do that.\n\r", ch );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( ch->rank < 2 || !IS_DEMON( ch ) )
  {
    send_to_char( "Huh?!?\n\r", ch );
    return;
  }

  if( IS_IMMORTAL( ch ) && get_trust( ch ) < 59 )
  {
    send_to_char( "You can't summon minions.\n\r", ch );
    return;
  }

  if( IS_NPC( ch ) && ch->pIndexData->vnum == 610 )
  {
    send_to_char( "You aren't allowed to do that.\n\r", ch );
    return;
  }

  if( is_splitformed( ch ) && !IS_NPC( ch ) )
  {
    ch_printf( ch, "You already have demons summoned!\n\r" );
    return;
  }

  if( ch->fighting && ( wf = ch->fighting->who ) )
  {
    if( IS_DEMON( ch ) && IS_DEMON( wf ) && ch->rank > 0 )
    {
      ch_printf( ch, "&RYou may not summon minions during a rank duel.\n\r" );
      return;
    }
  }

  /*
   * if( xIS_SET( (ch)->affected_by, AFF_TRI_FORM ) )
   * {
   * reuniteSplitForms(ch);
   * return;
   * }
   * 
   * if( xIS_SET( (ch)->affected_by, AFF_SPLIT_FORM ) ||
   * xIS_SET( (ch)->affected_by, AFF_MULTI_FORM ) )
   * {
   * reuniteSplitForms(ch);
   * return;
   * }
   */

  if( ch->mana < 50000 )
  {
    pager_printf_color( ch, "You must have at least 50,000 energy to summon minions.\n\r" );
    return;
  }
  if( ( pMobIndex = get_mob_index( 610 ) ) == NULL )
  {
    bug( "do_minion: mob does not exist" );
    send_to_char( "Hmm... Something went wrong...\n\r", ch );
    return;
  }

  victim = create_mobile( pMobIndex );
  char_to_room( victim, ch->in_room );

  victim2 = create_mobile( pMobIndex );
  char_to_room( victim2, ch->in_room );

  statsToMob( ch, victim, gsn_tri_form, FALSE, 0, 1 );
  statsToMob( ch, victim2, gsn_tri_form, FALSE, 0, 2 );

  act( AT_DGREY,
       "Your body begins to buckle as you start to charge an incredible amount of demonic energy. An aura of black energy flames bursts to life around your body as the ground begins to tremble.",
       ch, NULL, NULL, TO_CHAR );
  act( AT_RED,
       "The aura of energy flames around your body suddenly shifts in color to red. The amount of evil energy you're charging up suddenly becomes so great, that you manage to punch a hole in the fabric of space and time. A dark portal hole forms in the ground several feet infront of you; linking this dimension to the Shadow Earth realm.",
       ch, NULL, NULL, TO_CHAR );
  act( AT_DGREY,
       "You yell angrily towards the opened portal 'COME FORTH MY SERVANTS!!'. 2 Demons suddenly leap out from the hole and land on this side. They instantly bow before you and say 'We are here, Master $n'. You then release your dark energies, causing the portal to close up once again.",
       ch, NULL, NULL, TO_CHAR );

  act( AT_DGREY,
       "$n's body begins to buckle as $e starts to charge an incredible amount of demonic energy. An aura of black energy flames bursts to life around $n body as the ground begins to tremble.",
       ch, NULL, NULL, TO_NOTVICT );
  act( AT_RED,
       "The aura of energy flames around $n's body suddenly shifts in color to red. The amount of evil energy $e's charging up suddenly becomes so great, that $e manages to punch a hole in the fabric of space and time. A dark portal hole forms in the ground several feet infront of $n; linking this dimension to the Shadow Earth realm.",
       ch, NULL, NULL, TO_NOTVICT );
  act( AT_DGREY,
       "$n yells angrily towards the opened portal 'COME FORTH MY SERVANTS!!'. 2 Demons suddenly leap out from the hole and land on this side. They instantly bow before $n and say 'We are here, Master $n. $*e then releases $s dark energies, causing the portal to close up once again.",
       ch, NULL, NULL, TO_NOTVICT );


  ch->mana -= 50000;
  add_follower( victim, ch );
  add_follower( victim2, ch );
  victim->master = ch;
  victim->leader = ch;
  victim2->master = ch;
  victim2->leader = ch;
  ch->leader = ch;
  xSET_BIT( ch->affected_by, AFF_TRI_FORM );
  xSET_BIT( victim->affected_by, AFF_TRI_FORM );
  xSET_BIT( victim2->affected_by, AFF_TRI_FORM );

  return;
}

void do_banish( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) )
  {
    send_to_char( "NPC's can't do that.\n\r", ch );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( !IS_NPC( ch ) && ch->rank < 2 )
  {
    send_to_char( "Huh?!?\n\r", ch );
    return;
  }

  if( IS_IMMORTAL( ch ) && get_trust( ch ) < 59 )
  {
    send_to_char( "You can't banish minions.\n\r", ch );
    return;
  }

  if( IS_NPC( ch ) && ch->pIndexData->vnum == 610 )
  {
    send_to_char( "You aren't allowed to do that.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_TRI_FORM ) )
  {
    reuniteSplitForms( ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SPLIT_FORM ) || xIS_SET( ( ch )->affected_by, AFF_MULTI_FORM ) )
  {
    reuniteSplitForms( ch );
    return;
  }

  ch_printf( ch, "You have no minions summoned.\n\r" );
  return;
}

void do_mystic( CHAR_DATA *ch, char *argument )
{
  char   arg[MAX_INPUT_LENGTH];

  double pl_mult = 0;
  int    amount;

  argument = one_argument( argument, arg );

  if ( IS_NPC( ch ) )
    return;

      if( IS_SET(ch->pcdata->flags, PCFLAG_KNOWSMYSTIC) )
      {
          ch->pcdata->learned[gsn_mystic] = 95;
      }


  if ( !is_kaio( ch ) )
  {
    if ( !IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You don't know how to use this technique.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if ( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if ( IS_SET( ch->pcdata->combatFlags, CMB_NO_HEART ) && ch->rank < 3 )
  {
    ch_printf( ch, "You must first disable heart to use this technique.\n\r" );
    return;
  }

  if ( xIS_SET( ch->affected_by, AFF_KAIOKEN ) )
    xREMOVE_BIT( ch->affected_by, AFF_KAIOKEN );

  if ( !is_number( arg ) && arg[0] != '\0' )
  {
    ch_printf( ch, "Syntax: mystic [2-36].\n\r" );
    return;
  }

  if ( arg[0] == '\0' )
    amount = 0;
  else
    amount = atoi( arg );

  if ( amount <  0 ) amount =  0;
  if ( amount > 36 ) amount = 36;

  if ( amount == 0 )
  {
    if ( xIS_SET( ch->affected_by, AFF_MYSTIC ) )
    {
      ch_printf( ch, "You power down and return to normal.\n\r" );
      xREMOVE_BIT( ch->affected_by, AFF_MYSTIC );
      transStatRemove( ch );
      ch->pl = ch->exp;
      return;
    }
    else
    {
      ch_printf(ch,"You're not in mystic, though!\n\r");
      return;
    }
    return;
  }

  pl_mult = amount;

  if ( ch->mana < skill_table[gsn_mystic]->min_mana * pl_mult )
  {
    ch_printf( ch, "You don't have enough energy.\n\r" );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_mystic]->beats );

  if ( !can_use_skill( ch, number_percent( ), gsn_mystic ) )
  {
    act( AT_WHITE, "You can't quite focus well enough to draw out your hidden powers.", ch, NULL, NULL, TO_CHAR );
    act( AT_WHITE, "$n can't quite focus well enough to draw out $s hidden powers.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_mystic );    
    ch->mana -= skill_table[gsn_mystic]->min_mana * pl_mult;
    return;
  }

  act( AT_WHITE, "You focus your mind's eye on the full extent of your innermost recesses of power and try to draw it out all at once; setting off a shockwave that makes the ground beneath you tremble.", ch, NULL, NULL, TO_CHAR );
  act( AT_WHITE, "$n focuses $s mind's eye on the full extent of $s innermost recesses of power and tries to draw it out all at once; setting off a shockwave that makes the ground beneath you tremble.", ch, NULL, NULL, TO_NOTVICT );
  xSET_BIT( (ch)->affected_by, AFF_MYSTIC );
  ch->pl = ch->exp * pl_mult;
  transStatApply(ch, pl_mult, pl_mult, pl_mult, pl_mult);
  learn_from_success( ch, gsn_mystic );
  ch->mana -= skill_table[gsn_mystic]->min_mana * pl_mult;
  return;
}



//Lssj Progect - Lssj Project bad spelling Geh :P -Karn & Geh
void do_lssj( CHAR_DATA * ch, char *argument )
{
  if( is_saiyan( ch ) )
    if( is_saiyan_s( ch ) )
      if( is_saiyan_h( ch ) )
        if( is_saiyan_n( ch ) )
          if( is_saiyan_hb( ch ) )
          {
            ch_printf( ch, "You cannot use this skill..." );
            return;
          }

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_lssj ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( !IS_SET( ch->pcdata->flags, PCFLAG_KNOWSLSSJ ) )
    {
      ch_printf( ch, "You are not posessed with the power of the Legendary Super Saiyan.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    send_to_char( "You can't transform while you are an Oozaru.\n\r", ch );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->combatFlags, CMB_NO_HEART ) )
    {
      send_to_char( "You must first disable heart to use the Lssj.\n\r", ch );
      return;
    }
  }

  if( xIS_SET( ch->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You cannot use this skill with kaioken.\n\r", ch );
    return;
  }

  double pl_mult = 0;
  int arg;

  arg = atoi( argument );

  if( xIS_SET( ( ch )->affected_by, AFF_LSSJ ) && arg == 0 )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    /* Oh ya.. this should remove lighting effects to */
    ch->in_room->light--;
    xREMOVE_BIT( ( ch )->affected_by, AFF_LSSJ );
    transStatRemove( ch );
    ch->pl = ch->exp;
    if( !IS_NPC( ch ) )
    {
      ch->pcdata->haircolor = ch->pcdata->orignalhaircolor;
      ch->pcdata->eyes = ch->pcdata->orignaleyes;
    }
    return;
  }
  else if( !xIS_SET( ( ch )->affected_by, AFF_LSSJ ) && arg == 0 )
  {
    ch_printf( ch, "Syntax: lssj [2-45]\n\r" );
    return;
  }

  pl_mult = 45;

  if( arg > pl_mult )
    pl_mult = ( int )pl_mult;
  else if( arg < 1 )
    pl_mult = 1;
  else
    pl_mult = arg;

  if( ch->mana < skill_table[gsn_lssj]->min_mana * pl_mult )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  /*
   * Added this to curve confusion about the Mystic PL_MULT -Karn 01.29.05 
   */
  if( arg < 2 || arg > 45 )
  {
    send_to_char( "Syntax: Lssj [2-45]\n\r", ch );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_lssj]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_lssj ) )
  {
    act( AT_YELLOW,
         "Screaming out in rage, the wrath of the Legendary Super Saiya-Jin takes hold of your body as your muscles become much more defined.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW,
         "A flood of energy surges throughout your body as your eyes are flushed to a bleached white color. Your hair flares up as you",
         ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW,
         "complete the transformation into a Legendary Super Saiya-Jin. You become immediately influenced by the pride that accompanies",
         ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW,
         "the Saiyan race.",
         ch, NULL, NULL, TO_CHAR );


    act( AT_YELLOW,
         "Screaming out in rage, the wrath of the Legendary Super Saiya-Jin takes hold of $n's body as $s muscles become much more defined.",
         ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW,
         "A flood of energy surges throughout $s body as $s eyes are flushed to a bleached white color. $n's hair flares up as $e",
         ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW,
         "completes the transformation into a Legendary Super Saiya-Jin. $n becomes immediately influenced by the pride that accompanies",
         ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW,
         "the Saiyan race.",
         ch, NULL, NULL, TO_NOTVICT );

    //act( AT_YELLOW, "$n body begins to shift.. $n feels changing as the legendary power of the super saiyan is now within them.", ch, NULL, NULL, TO_NOTVICT );

    xSET_BIT( ( ch )->affected_by, AFF_LSSJ );
    ch->in_room->light++;

    ch->pl = ch->exp * pl_mult;

    transStatApply( ch, pl_mult, pl_mult, pl_mult, pl_mult );
    if( !IS_NPC( ch ) )
    {
      ch->pcdata->eyes = 0;
      ch->pcdata->haircolor = 3;
    }


    learn_from_success( ch, gsn_lssj );
  }
  else
  {
    act( AT_YELLOW,
         "Your body shifts slightly but returns to normal as you are unable to call upon the power of the Legendary Super Saiyan.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW,
         "$n body shifts slightly but returns to normal as $n is unable to call upon the power of the Legendary Super Saiyan.",
         ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_lssj );
  }

  ch->mana -= skill_table[gsn_lssj]->min_mana * pl_mult;
  return;
}





void do_revert_mystic( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) )
    return;

  if( IS_KAIO( ch ) )
  {
    ch_printf( ch, "Huh?!?\n\r" );
    return;
  }
  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }
  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }
  if( !IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
  {
    ch_printf( ch, "You don't know mystic!\n\r" );
    return;
  }
  REMOVE_BIT( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC );
  xREMOVE_BIT( ( ch )->affected_by, AFF_MYSTIC );
  ch_printf( ch, "You have lost the ability to use mystic.\n\r" );
  return;
}


void do_skga( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;
  int count = 0;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_skga ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_skga]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  count = atoi( argument );

  if( count < 1 || count > 20 )
  {
    ch_printf( ch, "The number of super kamikaze ghosts must be between 1 and 20.\n\r" );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_skga]->min_mana * count )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_skga]->focus * count )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_skga]->focus * count;

  WAIT_STATE( ch, skill_table[gsn_skga]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_skga ) )
  {
    int shots = count;
    int dam2 = 0;

    while( shots > 0 )
    {
      switch ( number_range( 1, 3 ) )
      {
        default:
        case 1:
        case 2:
          dam += number_range( 1, 2 );
          break;
        case 3:
          dam += number_range( 1, 3 );
          break;
      }
      shots--;
    }
    dam2 = number_range( 4, 8 );
    //dam2 = number_range( 8, 20 );
    dam = ( get_attmod( ch, victim ) * dam * dam2 );

    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    strcpy( buf, num_punct( count ) );
    strcpy( buf2, num_punct( dam ) );

    act2( AT_WHITE,
          "You cross your arms infront of yourself and charge up an incredible amount of ki into your body; your aura flaring up brightly as you do. Your lungs and mouth suddenly fill up with this energy as it transforms into a strange, airy substance. You open your mouth and begin spitting out $t puffs of greyish energy. The puffs of energy suddenly change in shape, taking on the form of miniature ghosts of yourself.",
          ch, buf, victim, TO_CHAR, NULL );
    act2( AT_WHITE,
          "You shout 'SUPER KAMIKAZE GHOST ATTACK' as you perform a series of weird poses. All $t ghosts race towards $N and slam into $M, exploding violently. &W[$T]",
          ch, buf, victim, TO_CHAR, buf2 );
    act2( AT_WHITE,
          "$n crosses $s arms infront of $mself and charges up an incredible amount of ki into $s body; $s aura flaring up brightly as $e does. $n's lungs and mouth suddenly fill up with this energy as it transforms into a strange, airy substance. $n opens $s mouth and begins spitting out $t puffs of greyish energy. The puffs of energy suddenly change in shape, taking on the form of miniature ghosts of $mself.",
          ch, buf, victim, TO_VICT, NULL );
    act2( AT_WHITE,
          "$n shouts 'SUPER KAMIKAZE GHOST ATTACK' as $e performs a series of weird poses. All $t ghosts race towards you and slam into you, exploding violently. &W[$T]",
          ch, buf, victim, TO_VICT, buf2 );
    act2( AT_WHITE,
          "$n crosses $s arms infront of $mself and charges up an incredible amount of ki into $s body; $s aura flaring up brightly as $e does. $n's lungs and mouth suddenly fill up with this energy as it transforms into a strange, airy substance. $n opens $s mouth and begins spitting out $t puffs of greyish energy. The puffs of energy suddenly change in shape, taking on the form of miniature ghosts of $mself.",
          ch, buf, victim, TO_NOTVICT, NULL );
    act2( AT_WHITE,
          "$n shouts 'SUPER KAMIKAZE GHOST ATTACK' as $e performs a series of weird poses. All $t ghosts race towards $N and slam into $M, exploding violently. &W[$T]",
          ch, buf, victim, TO_NOTVICT, buf2 );

    dam = ki_absorb( victim, ch, dam, gsn_skga );
    learn_from_success( ch, gsn_skga );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_WHITE, "You missed $N with your super kamikaze ghost attack.", ch, NULL, victim, TO_CHAR );
    act( AT_WHITE, "$n misses you with $s super kamikaze ghost attack.", ch, NULL, victim, TO_VICT );
    act( AT_WHITE, "$n missed $N with a super kamikaze ghost attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_skga );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_skga]->min_mana * count;
  return;
}

void do_monkey_gun( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_monkey_gun ) )
      return;
  }

  if( !xIS_SET( ( ch )->affected_by, AFF_OOZARU ) && !xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    ch_printf( ch, "You can only use this while in the oozaru form!\n\r" );
    return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_monkey_gun]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  /*
   * No mana cost for this skill as when in an Oozaru state, a saiyan/halfies
   * energy is pretty much unlimited. -Karma
   */

  int focus = ( get_curr_int( ch ) / 5 );

  if( focus < 2 )
    focus = 2;

  if( ch->focus < focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= focus;

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_monkey_gun]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_monkey_gun ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 40, 80 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( z,
         "You suddenly condense a tremendous amount of energy in the pit of your body. Aiming your head at $N, you open your giant mouth and essentially vomit up the energy, firing a massive column of energy at $M. You throw your clenched fists in the air and roar with all your might as $N is enveloped in a collosal explosion. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( z,
         "$n suddenly condenses a tremendous amount of energy in the pit of $s body. Aiming $s head at you, $n opens $s giant mouth and essentially vomits up the energy, firing a massive column of energy at you. $n throws $s clenched fists in the air and roars with all $s might as you are enveloped in a collosal explosion. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( z,
         "$n suddenly condenses a tremendous amount of energy in the pit of $s body. Aiming $s head at $N, $n opens $s giant mouth and essentially vomits up the energy, firing a massive column of energy at $N. $n throws $s clenched fists in the air and roars with all $s might as $N is enveloped in a collosal explosion. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_monkey_gun );
    learn_from_success( ch, gsn_monkey_gun );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( z, "You missed $N with your oozaru mouth cannon.", ch, NULL, victim, TO_CHAR );
    act( z, "$n misses you with $s oozaru mouth cannon.", ch, NULL, victim, TO_VICT );
    act( z, "$n missed $N with a oozaru mouth cannon.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_monkey_gun );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  return;
}

void do_bbk( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_bbk ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_bbk]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( !is_saiyan_s( ch ) && !is_bio( ch ) )
  {
    ch_printf( ch, "Huh?\n\r" );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_bbk]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_bbk]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_bbk]->focus;

  WAIT_STATE( ch, skill_table[gsn_bbk]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_bbk ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 120, 130 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_BLUE, "Standing upright with your arm extended outwards, you spring your flattened", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "palm into a vertical stance while staring indefinitely into $N's eyes.", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "A torrent of energy bursts fourth and flows from your arm as your scowl", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "appearance remains transfixed on $N. With immeasurable anger in your", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "tone you call out in a dual voice, &C'Big Bang Kamehameha'. &BAn immense", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "sphere emerges between you and your opponent while shrouding the area in a", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "blinding white radiance. A vortex of gusting winds and rippling energy bands", ch, NULL, victim,
         TO_CHAR );
    act( AT_BLUE, "disperse from the core of the attack whilst destroying the surrounding", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "environment with each fatal blow. Without a moments notice the orb becomes", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "unstable and erupts within $N's vicinity and devastates $M as the", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "massive outburst makes contact. &W[$t]", ch, num_punct( dam ), victim, TO_CHAR );

    act( AT_BLUE, "Standing upright with $s arm extended outwards, $n springs $s", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "flattened palm into a vertical stance while staring indefinitely into your eyes.", ch, NULL, victim,
         TO_VICT );
    act( AT_BLUE, "A torrent of energy bursts fourth and flows from $s arm as $n's", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "scowl appearance remains transfixed on you. With immeasurable anger in $s", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "tone $e calls out in a dual voice, &C'Big Bang Kamehameha'. &BAn immense sphere", ch, NULL, victim,
         TO_VICT );
    act( AT_BLUE, "emerges between you and your opponent while shrouding the area in a blinding", ch, NULL, victim,
         TO_VICT );
    act( AT_BLUE, "white radiance. A vortex of gusting winds and rippling energy bands disperse", ch, NULL, victim,
         TO_VICT );
    act( AT_BLUE, "from the core of the attack whilst destroying the surrounding environment", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "with each fatal blow. Without a moments notice the orb becomes unstable and", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "erupts within your vicinity and devastates you as the massive outburst", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "makes contact. &W[$t]", ch, num_punct( dam ), victim, TO_VICT );

    act( AT_BLUE, "Standing upright with $s arm extended outwards, $n springs $s", ch, NULL, victim, TO_NOTVICT );
    act( AT_BLUE, "flattened palm into a vertical stance while staring indefinitely into", ch, NULL, victim, TO_NOTVICT );
    act( AT_BLUE, "$N's eyes. A torrent of energy bursts fourth and flows $n's arm", ch, NULL, victim, TO_NOTVICT );
    act( AT_BLUE, "as $s scowl appearance remains transfixed on $N. With immeasurable", ch, NULL, victim, TO_NOTVICT );
    act( AT_BLUE, "anger in $s tone $e calls out in a dual voice, &C'Big Bang Kamehameha'. &BAn", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_BLUE, "immense sphere emerges between $n and $s opponent while shrouding", ch, NULL, victim, TO_NOTVICT );
    act( AT_BLUE, "the area in a blinding white radiance. A vortex of gusting winds and rippling", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_BLUE, "energy bands disperse from the core of the attack whilst destroying the", ch, NULL, victim, TO_NOTVICT );
    act( AT_BLUE, "surrounding environment with each fatal blow. Without a moments notice the orb", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_BLUE, "becomes unstable and erupts within $N's vicinity and devastates $M", ch, NULL, victim, TO_NOTVICT );
    act( AT_BLUE, "as the massive outburst makes contact. &W[$t]", ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_bbk );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_BLUE, "You missed $N with your big bang kamehameha.", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "$n misses you with $s big bang kamehameha.", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "$n missed $N with a big bang kamehameha.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_bbk );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_bbk]->min_mana;
  return;
}

void do_desperation( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( !is_human( ch ) )
  {
    send_to_char( "You can't use this.\n\r", ch );
    return;
  }

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_desperation ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_desperation]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_desperation]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_desperation]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_desperation]->focus;

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_desperation]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_desperation ) )
  {
    if( ch->hit > 70 )
      dam = get_attmod( ch, victim ) * number_range( 15, 20 );
    if( ch->hit <= 70 && ch->hit >= 51 )
      dam = get_attmod( ch, victim ) * number_range( 20, 30 );
    if( ch->hit <= 50 && ch->hit >= 21 )
      dam = get_attmod( ch, victim ) * number_range( 40, 45 );
    if( ch->hit <= 20 && ch->hit >= 0 )
      dam = get_attmod( ch, victim ) * number_range( 50, 70 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( z, "You hold your arms out straight, palms facing $N, your body surging with power, Drawing on your pain", ch, NULL,
         victim, TO_CHAR );
    act( z, "you scream as a large ball of energy forms in front of your palms, blasting out towards $N. &W[$t]", ch,
         num_punct( dam ), victim, TO_CHAR );
    act( z, "$n holds their arms out straight, palms facing you, $s body surges with power as $s focuses on their pain.", ch,
         NULL, victim, TO_VICT );
    act( z, "A large ball of energy forms in front of $n's palms, blasting out towards you. &W[$t]", ch, num_punct( dam ),
         victim, TO_VICT );
    act( z, "$n holds their arms out straight, palms facing $N, $s body surges with energy as $s focuses on $s pain.", ch,
         NULL, victim, TO_NOTVICT );
    act( z, "A large ball of energy forms in front of $n's palms, blasting out towards $N as they scream. &W[$t]", ch,
         num_punct( dam ), victim, TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_desperation );
    learn_from_success( ch, gsn_desperation );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( z, "You missed $N with your desperation attack wave.", ch, NULL, victim, TO_CHAR );
    act( z, "$n misses you with $s desperation attack wave.", ch, NULL, victim, TO_VICT );
    act( z, "$n missed $N with a desperation attack wave.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_desperation );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_desperation]->min_mana;
  return;
}

void do_spirit_bomb( CHAR_DATA * ch, char *arg )
{
  CHAR_DATA *victim;
  int dam = 0;
  char size[MAX_STRING_LENGTH];
  char facialExpression[MAX_STRING_LENGTH];
  char engeryColor[MAX_STRING_LENGTH];
  char failVerb[MAX_STRING_LENGTH];

  size[0] = '\0';
  facialExpression[0] = '\0';
  engeryColor[0] = '\0';
  failVerb[0] = '\0';

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_spirit_bomb ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_spirit_bomb]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_spirit_bomb]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_spirit_bomb]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_spirit_bomb]->focus;

  WAIT_STATE( ch, skill_table[gsn_spirit_bomb]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_spirit_bomb ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 14, 101 );

    if( dam <= 15 )
      strcpy( size, "tiny" );
    else if( dam <= 25 )
      strcpy( size, "small" );
    else if( dam <= 35 )
      strcpy( size, "little" );
    else if( dam <= 40 )
      strcpy( size, "large" );
    else if( dam <= 45 )
      strcpy( size, "big" );
    else if( dam <= 50 )
      strcpy( size, "huge" );
    else if( dam <= 55 )
      strcpy( size, "great" );
    else if( dam <= 60 )
      strcpy( size, "enormous" );
    else if( dam <= 65 )
      strcpy( size, "massive" );
    else if( dam <= 70 )
      strcpy( size, "gigantic" );
    else if( dam <= 75 )
      strcpy( size, "immense" );
    else if( dam <= 80 )
      strcpy( size, "monstrous" );
    else if( dam <= 85 )
      strcpy( size, "mammoth" );
    else if( dam <= 90 )
      strcpy( size, "vast" );
    else if( dam <= 95 )
      strcpy( size, "gargantuan" );
    else
      strcpy( size, "colossal" );

    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    if( IS_GOOD( ch ) )
    {
      strcpy( facialExpression, "smile" );
      strcpy( engeryColor, "bluish" );
    }
    else if( IS_EVIL( ch ) )
    {
      strcpy( facialExpression, "smirk" );
      strcpy( engeryColor, "reddish" );
    }
    else
    {
      strcpy( facialExpression, "grimace" );
      strcpy( engeryColor, "whitish" );
    }

    act( AT_SKILL, "You raise your hands above your head, motes of $t energy beginning to form above your palms.", ch,
         engeryColor, victim, TO_CHAR );
    act( AT_SKILL, "$n raises $s hands above $s head, motes of $t energy beginning to form above $n's palms.", ch,
         engeryColor, victim, TO_VICT );
    act( AT_SKILL, "$n raises $s hands above $s head, motes of $t energy beginning to form above $n's palms.", ch,
         engeryColor, victim, TO_NOTVICT );
    if( IS_GOOD( ch ) )
    {
      do_say( ch, "&YThe stars, the trees, all living things...lend me your energy!" );
    }
    else if( IS_EVIL( ch ) )
    {
      do_say( ch, "&YThe stars, the storm, crash of death....give me your power!" );
    }
    else
    {
      do_say( ch, "&YThe stars, the planets, the spark of life...I need your energy!" );
    }

    act2( AT_BLUE,
          "A $t ball of swirling $T energy forms above your upturned palms, growing slowly larger as you draw in more energy from the cosmos.",
          ch, size, victim, TO_CHAR, engeryColor );
    act( AT_BLUE, "You $t as you toss the brilliant sphere of pure ki towards $N.", ch, facialExpression, victim, TO_CHAR );
    act( AT_BLUE, "$N is slowly engulfed by the spirit bomb, ripped apart by the gathered energy. &W[$t]", ch,
         num_punct( dam ), victim, TO_CHAR );

    act2( AT_BLUE,
          "A $t ball of swirling $T energy forms above $n's upturned palms, growing slowly larger as $e draws in more energy from the cosmos.",
          ch, size, victim, TO_VICT, engeryColor );
    act( AT_BLUE, "$n $ts as $e tosses the brilliant sphere of pure ki towards you.", ch, facialExpression, victim,
         TO_VICT );
    act( AT_BLUE, "You are slowly engulfed by the spirit bomb, ripped apart by the gathered energy. &W[$t]", ch,
         num_punct( dam ), victim, TO_VICT );

    act2( AT_BLUE,
          "A $t ball of swirling $T energy forms above $n's upturned palms, growing slowly larger as $e draws in more energy from the cosmos.",
          ch, size, victim, TO_NOTVICT, engeryColor );
    act( AT_BLUE, "$n $ts as $e tosses the brilliant sphere of pure ki towards $N.", ch, facialExpression, victim,
         TO_NOTVICT );
    act( AT_BLUE, "$N is slowly engulfed by the spirit bomb, ripped apart by the gathered energy. &W[$t]", ch,
         num_punct( dam ), victim, TO_NOTVICT );

    /*
     * dam = 0;
     */
    /*
     * dam = ki_absorb( victim, ch, dam, gsn_spirit_bomb );
     */
    learn_from_success( ch, gsn_spirit_bomb );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    if( IS_GOOD( ch ) )
    {
      strcpy( failVerb, "cry for help" );
    }
    else if( IS_EVIL( ch ) )
    {
      strcpy( failVerb, "damand for power" );
    }
    else
    {
      strcpy( failVerb, "plea for assistance" );
    }


    act( AT_SKILL, "Only a few dots of energy gather before quickly dispersing, your $t going unanswered.", ch, failVerb,
         victim, TO_CHAR );
    act( AT_SKILL, "Only a few dots of energy gather before quickly dispersing, $n's $t going unanswered.", ch, failVerb,
         victim, TO_VICT );
    act( AT_SKILL, "Only a few dots of energy gather before quickly dispersing, $n's $t going unanswered.", ch, failVerb,
         victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_spirit_bomb );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_spirit_bomb]->min_mana;
  return;
}

void do_ussj( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_ussj ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SUPER_OOZARU ) )
  {
    send_to_char( "You have to power out of Super Oozaru to do this.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSLSSJ ) )
    {
      ch_printf( ch,
                 "You are unable to call upon those powers while you are posessed with the power of the Legendary super Saiyan.\n\r" );
      return;
    }
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You can't transform in to a super saiyan while using kaioken.\n\r", ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_SSJ )
      && xIS_SET( ( ch )->affected_by, AFF_USSJ ) && !xIS_SET( ( ch )->affected_by, AFF_USSJ2 ) )
  {
    send_to_char( "Your bulging muscles retract as you power down.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_USSJ );
    transStatRemove( ch );
    ch->pl = ch->exp * 12;
    transStatApply( ch, 12, 7, 5, 7 );

    return;
  }
  else
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ2 )
        || xIS_SET( ( ch )->affected_by, AFF_SSJ3 ) || xIS_SET( ( ch )->affected_by, AFF_SSJ4 ) )

  {
    send_to_char( "You are already powered up beyond this technique.\n\r", ch );
    return;
  }
  else
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ )
        && xIS_SET( ( ch )->affected_by, AFF_USSJ ) && xIS_SET( ( ch )->affected_by, AFF_USSJ2 ) )
  {
    send_to_char( "You have to power out of USSJ2 to do this.\n\r", ch );
    return;
  }

  if( !xIS_SET( ( ch )->affected_by, AFF_SSJ ) )
  {
    send_to_char( "You need to be powered up to super saiyan to use this!\n\r", ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    send_to_char( "You can't transform while you are an Oozaru.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_ussj]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_ussj]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_ussj ) )
  {
    act( AT_YELLOW, "You draw in energy, forcing it directly into your body.  Your muscles bulge as your hair "
         "turns slightly spikier.", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "$n draws in energy, forcing it directly into $s body.  $n's muscles bulge as $s hair "
         "turns slightly spikier.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_USSJ );
    ch->pl = ch->exp * 14;
    transStatApply( ch, 18, 7, 3, 7 );

    learn_from_success( ch, gsn_ussj );
  }
  else
  {
    act( AT_BLUE, "You grunt in exertion and your muscles bulge for a moment.", ch, NULL, NULL, TO_CHAR );
    act( AT_BLUE, "$n grunts in exertion and $s muscles bulge for a moment.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_ussj );
  }

  ch->mana -= skill_table[gsn_ussj]->min_mana;
  return;
}

void do_ussj2( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_ussj2 ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SUPER_OOZARU ) )
  {
    send_to_char( "You have to power out of Super Oozaru to do this.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSLSSJ ) )
    {
      ch_printf( ch,
                 "You are unable to call upon those powers while you are posessed with the power of the Legendary super Saiyan.\n\r" );
      return;
    }
  }


  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You can't transform in to a super saiyan while using kaioken.\n\r", ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_SSJ )
      && xIS_SET( ( ch )->affected_by, AFF_USSJ ) && xIS_SET( ( ch )->affected_by, AFF_USSJ2 ) )
  {
    send_to_char( "Your bulging muscles retract a little as you power down.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_USSJ2 );
    transStatRemove( ch );
    ch->pl = ch->exp * 14;
    transStatApply( ch, 18, 7, 5, 7 );

    return;
  }
  else
    if( xIS_SET( ( ch )->affected_by, AFF_SSJ2 )
        || xIS_SET( ( ch )->affected_by, AFF_SSJ3 ) || xIS_SET( ( ch )->affected_by, AFF_SSJ4 ) )

  {
    send_to_char( "You are already powered up beyond this technique.\n\r", ch );
    return;
  }

  if( !xIS_SET( ( ch )->affected_by, AFF_SSJ ) && !xIS_SET( ( ch )->affected_by, AFF_USSJ ) )
  {
    send_to_char( "You need to be powered up to ultra super saiyan to use this!\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SSJ ) && !xIS_SET( ( ch )->affected_by, AFF_USSJ ) )
  {
    send_to_char( "You need to be powered up to ultra super saiyan to use this!\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    send_to_char( "You can't transform while you are an Oozaru.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_ussj2]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_ussj2]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_ussj2 ) )
  {
    act( AT_YELLOW, "You draw in massive amounts of energy, forcing it directly into your body.  Your muscles "
         "bulge to inhuman sizes, your hair becomes wildly spiked.", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "$n draws in massive amounts of energy, forcing it directly into $s body.  $n's muscles "
         "bulge to inhuman sizes, $s hair becomes wildly spiked.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_USSJ2 );
    ch->pl = ch->exp * 16;
    transStatApply( ch, 26, -17, 7, 10 );

    learn_from_success( ch, gsn_ussj2 );
  }
  else
  {
    act( AT_BLUE, "You grunt in massive exertion and your muscles bulge for a moment.", ch, NULL, NULL, TO_CHAR );
    act( AT_BLUE, "$n grunts in massive exertion and $s muscles bulge for a moment.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_ussj2 );
  }

  ch->mana -= skill_table[gsn_ussj2]->min_mana;
  return;
}


void do_extreme( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) && !is_split( ch ) )
    return;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_extreme ) )
      return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_EXTREME ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_EXTREME );
    ch->pl = ch->exp;
    transStatRemove( ch );
    return;
  }
  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->combatFlags, CMB_NO_HEART ) && ch->rank < 3 )
    {
      send_to_char( "You must first disable heart to use the 'Extreme Technique.'\n\r", ch );
      return;
    }
  }
  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You cannot use the 'Extreme Technique' in combination with kaioken.\n\r", ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_HYPER ) )
  {
    send_to_char( "You may not be in 'Hyper' to use the 'Extreme Technique'.\n\r", ch );
    return;
  }
  if( ch->mana < skill_table[gsn_extreme]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_extreme]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_extreme ) )
  {
    /*
     * note to self in case this needs to be changed
     * back to white color. it originally says "bright
     * white light" in a section of these messages.
     * Changed it to bright light for now. - Karma 
     */
    act( z,
         "You close your eyes and concentrate, focusing all your energy and will to live on one point in your body.  A dot of light forms on your forehead, a bright light spreading out and bathing you in its glow as your hidden power awakens.",
         ch, NULL, NULL, TO_CHAR );
    act( z,
         "$n closes $s eyes and concentrates, focusing all of $s energy and will to live on one point in $s body.  A dot of light forms on $n's forehead, a bright light spreading out and bathing $s in its glow as $s hidden power awakens.",
         ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_EXTREME );
    ch->pl = ch->exp * 35;
    transStatApply( ch, 30, 25, 18, 35 );

    learn_from_success( ch, gsn_extreme );
  }

  else
  {
    act( z, "You close your eyes and concentrate, but can't seem to properly focus your immense energies.", ch, NULL, NULL,
         TO_CHAR );
    act( z, "$n closes $s eyes and concentrates, but can't seem to properly focus $s immense energies.", ch, NULL, NULL,
         TO_NOTVICT );
    learn_from_failure( ch, gsn_extreme );
  }

  ch->mana -= skill_table[gsn_extreme]->min_mana;
  return;
}




//majin thingy
void do_majin( CHAR_DATA * ch, char *argument )
{
  char   arg[MAX_INPUT_LENGTH];

  double pl_mult = 0;
  int    amount;

  argument = one_argument( argument, arg );
  
  if( IS_NPC( ch ) && !is_split( ch ) )
    return;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_majin ) )
      return;
  }

  if( !ch->canmajin )
  {
    ch_printf( ch, "You dont know how to tap into your darkside...\n\r" );
    return;
  }

  if( xIS_SET( ch->affected_by, AFF_OOZARU ) || xIS_SET( ch->affected_by, AFF_GOLDEN_OOZARU )
      || xIS_SET( ch->affected_by, AFF_SUPER_OOZARU ) )
  {
    send_to_char( "You can't do this in Oozaru.", ch );
    return;
  }


  if( IS_GOOD( ch ) )
  {
    send_to_char( "Your heart became to pure for this way of evil.\n\r", ch );
    return;
  }

  if ( IS_SET( ch->pcdata->combatFlags, CMB_NO_HEART ) && ch->rank < 3 )
  {
    ch_printf( ch, "You must first disable heart to use this technique.\n\r" );
    return;
  }

  if ( !is_number( arg ) && arg[0] != '\0' )
  {
    ch_printf( ch, "Syntax: majin [2-40].\n\r" );
    return;
  }

  if ( arg[0] == '\0' )
    amount = 0;
  else
    amount = atoi( arg );

  if ( amount <  0 ) amount =  0;
  if ( amount > 40 ) amount = 40;

  if ( amount == 0 )
  {  
    if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
    {
      xREMOVE_BIT( ( ch )->affected_by, AFF_MAJIN );
      transStatRemove( ch );
      ch->pl = ch->exp;
      send_to_char( "Your soul has been released from the darkness of majin.\n\r", ch );
      return;
    }
	else
	{
	  ch_printf(ch,"You're not in majin, though!\n\r");
      return;
	}
  }
  
  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }


  pl_mult = amount;

  if ( ch->mana < skill_table[gsn_majin]->min_mana * pl_mult )
  {
    ch_printf( ch, "You don't have enough energy.\n\r" );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_majin]->beats );
  
  if( can_use_skill( ch, number_percent(  ), gsn_majin ) )
  {

    act( AT_DGREY, "The control of babadi fills your mind and soul with hatred as a fluid of", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "disgust for anything commendable courses through your veins.  Lashing out", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "in torment and with an uncontrollable rage, your muscles flex and bulge", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "outwards slightly as the corruption boosts your capabilities.  The insignia", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "upon your head sears a bright glowing red and bolts of electricity flare up", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "around your figure.  As you regain your composure, a trickle of blood", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "escapes the corner of your mouth as a result from the exertion your body", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "had to withstand.", ch, NULL, NULL, TO_CHAR );

    act( AT_DGREY, "The control of babadi fills $n's mind and soul with hatred as a fluid of", ch, NULL, NULL, TO_NOTVICT );
    act( AT_DGREY, "disgust for anything commendable courses through $s veins.  Lashing out", ch, NULL, NULL, TO_NOTVICT );
    act( AT_DGREY, "in torment and with an uncontrollable rage, their muscles flex and bulge", ch, NULL, NULL, TO_NOTVICT );
    act( AT_DGREY, "outwards slightly as the corruption boosts $s capabilities.  The insignia", ch, NULL, NULL, TO_NOTVICT );
    act( AT_DGREY, "upon their head sears a bright glowing red and bolts of electricity flare up", ch, NULL, NULL,
         TO_NOTVICT );
    act( AT_DGREY, "around their figure.  As they regains their composure, a trickle of blood", ch, NULL, NULL, TO_NOTVICT );
    act( AT_DGREY, "escapes the corner of their mouth as a result from the exertion their body", ch, NULL, NULL,
         TO_NOTVICT );
    act( AT_DGREY, "had to withstand.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( (ch)->affected_by, AFF_MAJIN );
    ch->pl = ch->exp * pl_mult;
    transStatApply(ch, pl_mult * 2, pl_mult * 2, pl_mult * -2, pl_mult / 2);
    learn_from_success( ch, gsn_majin );
  }

  else
  {
    act( AT_GREY, "You hear the dark call, but your soul refuses to give itself to the eternal darkness.", ch, NULL, NULL,
         TO_CHAR );
    act( AT_GREY, "$n hears the dark call, but $s soul refuses to give itself to the eternal darkness.", ch, NULL, NULL,
         TO_NOTVICT );
    learn_from_failure( ch, gsn_majin );
  }

  ch->mana -= skill_table[gsn_mystic]->min_mana * pl_mult;
  return;
}


void do_teachmajin( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *vch;

  if( !IS_NPC( ch ) )
    if( ch->level < 63 )
      if( !IS_GENIE(ch) && ch->rank < 3 )
      {
        ch_printf( ch, "Huh?\n\r" );
        return;
      }

  if( ch->position != POS_STANDING )
  {
    ch_printf( ch, "You have to be standing.\n\r" );
    return;
  }
  if( ( vch = get_char_room( ch, argument ) ) == NULL )
  {
    ch_printf( ch, "There's nobody here by that name.\n\r" );
    return;
  }

  if( vch->canmajin )
  {
    if( IS_NPC( ch ) )
      act( AT_LBLUE, "Babadi says 'You already know how to tap into the dark side'", ch, NULL, vch, TO_VICT );
    else
      ch_printf( ch, "they already knows how to tap in to the darkside.\n\r" );
    return;
  }

  if( IS_NPC( vch ) )
  {
    ch_printf( ch, "Not on NPC's.\n\r" );
    return;
  }
  if( vch->exp < 100000000000ULL )
  {
    if( IS_NPC( ch ) )
      act( AT_LBLUE, "Babadi says 'You have to be over 100b to learn majin'", ch, NULL, vch, TO_VICT );
    else
      ch_printf( ch, "They have to be over 100bil to learn majin.\n\r" );
    return;
  }
  if( IS_KAIO( vch ) || is_android( vch ) || is_superandroid( vch ) || is_reploid( vch ) || is_fierian( vch ) || is_hylian( vch ) || IS_BIOANDROID( vch ) || IS_ICER( vch ) )
  {
    if( IS_NPC( ch ) )
      act( AT_LBLUE, "Babadi says 'you cannot be taught majin'", ch, NULL, vch, TO_VICT );
    else
      ch_printf( ch, "They cannot be taught majin.\n\r" );
    return;
  }

  if( vch->position != POS_STANDING )
  {
    ch_printf( ch, "They have to be standing.\n\r" );
    return;
  }
  ch_printf( ch, "\n\r" );
  ch_printf( vch, "\n\r" );
  act( AT_DGREY, "The evil wizard babadi raises his palms and points them your way.", ch, NULL, vch, TO_VICT );
  act( AT_DGREY,
       "Babadi then begins to utter words that are meaningless to you but $m seems perfectly aware of the incarnation he is summoning.",
       ch, NULL, vch, TO_VICT );
  act( AT_DGREY,
       "As the muttering grows louder, a tingling sensation begins to occur in your head which is quickly replaced by an anguishing pain.",
       ch, NULL, vch, TO_VICT );
  act( AT_DGREY, "Babadi abrubtly stops his chatter and replaces it with the evilest of grins.", ch, NULL, vch, TO_VICT );

  act( AT_DGREY, "The evil wizard babadi raises his palms and points them $N's way.", ch, NULL, vch, TO_NOTVICT );
  act( AT_DGREY,
       "Babadi then begins to utter words that are meaningless to $N but $m seems perfectly aware of the incarnation he is summoning.",
       ch, NULL, vch, TO_NOTVICT );
  act( AT_DGREY,
       "As the muttering grows louder, a tingling sensation begins to occur in $N's head which is quickly replaced by an anguishing pain.",
       ch, NULL, vch, TO_NOTVICT );
  act( AT_DGREY, "Babadi abrubtly stops his chatter and replaces it with the evilest of grins.", ch, NULL, vch, TO_NOTVICT );


  act( AT_GREY, "Enjoy Majin.. -Kal", ch, NULL, vch, TO_CHAR );
  vch->canmajin = TRUE;
  if( IS_GOOD( vch ) )
  {
    ch->alignment = -1000;
  }
  vch->pcdata->learned[gsn_majin] = 10;

  save_char_obj( vch );

  do_majin( vch, "\0" );

  return;
}


void do_kairestore( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
//    bool boost = FALSE;

  set_char_color( AT_SKILL, ch );
  set_pager_color( AT_SKILL, ch );
  argument = one_argument( argument, arg );

  argument = one_argument( argument, arg2 );
/*
    if( number_range( 1, 3 ) == 3 )
    {
	send_to_char( "Boosting restore!\n\r", ch );
	boost = TRUE;
    }
*/
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;

  if( !ch->pcdata )
    return;

  if( get_true_rank( ch ) < 11 && !IS_IMMORTAL(ch) )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( IS_NPC( ch ) )
    return;

  if( sysdata.kaiRestoreTimer > 0 && !IS_IMMORTAL( ch) )
  {
    pager_printf( ch, "You can't restore for another %d minutes.  (Overall timer)\n\r", sysdata.kaiRestoreTimer );
    return;
  }

  if( ch->pcdata->eKTimer > 0 )
  {
    pager_printf( ch, "You can't restore for another %d minutes.  (Personal timer)\n\r", ch->pcdata->eKTimer );
    return;
  }

  if( ch->mana < 50000 )
  {
    send_to_char( "You need 50,000 energy to restore.\n\r", ch );
    return;
  }

  ch->mana -= 50000;
/* Champ+ half the time for kairestore */
  if( IS_IMMORTAL( ch ) )
    ch->pcdata->eKTimer = 0;
  else if( ch->level >= 12 )
    ch->pcdata->eKTimer = 30;
  else
    ch->pcdata->eKTimer = 60;
  if( IS_IMMORTAL( ch ) )
    sysdata.kaiRestoreTimer = 0;
  else
    sysdata.kaiRestoreTimer = 0;
  save_char_obj( ch );
  send_to_char( "Beginning kairestore...\n\r", ch );
  for( vch = first_char; vch; vch = vch_next )
  {
    vch_next = vch->next;
    if( !IS_NPC( vch ) )
    {
/*	    if ( boost )
		vch->hit = vch->max_hit * 1.5;
	    else
*/ vch->hit = vch->max_hit;
      vch->mana = vch->max_mana;
      vch->move = vch->max_move;
      update_pos( vch );
      heart_calc( vch, "" );
      if( ch->pcdata->kaiRestoreMsg
          && ch->pcdata->kaiRestoreMsg[0] != '\0' && str_cmp( "(null)", ch->pcdata->kaiRestoreMsg ) )
      {
        //ch_printf(vch,"&wRestore by %s:\n\r",capitalize(ch->name));
        act( AT_GREY, "Restore by $n:", ch, NULL, vch, TO_VICT );
        act( AT_LBLUE, ch->pcdata->kaiRestoreMsg, ch, NULL, vch, TO_VICT );
      }
      else
      {
        act( AT_LBLUE, "An avatar of $n appears and performs some arcane symbols while", ch, NULL, vch, TO_VICT );
        act( AT_LBLUE, "muttering unheard words, and suddenly you feel refreshed as the", ch, NULL, vch, TO_VICT );
        act( AT_LBLUE, "avatar vanishes into nothingness.", ch, NULL, vch, TO_VICT );
      }
    }
  }
  send_to_char( "All restored.\n\r", ch );
}

void do_setrestoremessage( CHAR_DATA * ch, char *argument )
{
  if( !IS_NPC( ch ) )
  {
    smash_tilde( argument );
    if( argument[0] == '\0' )
    {
      pager_printf( ch, "Your current message is:\n\r%s\n\r", ch->pcdata->kaiRestoreMsg );
      pager_printf( ch, "Use any argument to set your message.\n\r" );
      pager_printf( ch, "Use clear to set it to the default.\n\r" );
      return;
    }
    DISPOSE( ch->pcdata->kaiRestoreMsg );
    if( !str_cmp( argument, "clear" ) )
    {
      ch->pcdata->kaiRestoreMsg = str_dup( "(null)" );
      send_to_char_color( "&YKai restore message reset.\n\r", ch );
      return;
    }
    else
    {
      if( has_phrase( "$n", argument ) || has_phrase( strupper( ch->name ), strupper( argument ) ) )
        ch->pcdata->kaiRestoreMsg = str_dup( argument );
      else
      {
        send_to_char( "You must have your name in your message.\n\r", ch );
        return;
      }
    }
    send_to_char_color( "&YKai restore message set to:\n\r", ch );
    ch_printf( ch, "%s\n\r", argument );
  }
  return;
}
void do_potarafuse( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *rch;
  CHAR_DATA *vch;
  CHAR_DATA *dch; /*dominant fuse char */
  CHAR_DATA *sch; /*stasis fuse char */
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  bool cright = FALSE;
  bool vright = FALSE;
  bool cleft = FALSE;
  bool vleft = FALSE;
  OBJ_DATA *ro;
  OBJ_DATA *vo;
  OBJ_DATA *po;
  OBJ_DATA *o;
  OBJ_INDEX_DATA *pObjIndex;
  int sn;
  int value;
  bool saiyan1 = FALSE, saiyan2 = FALSE;
  bool namek1 = FALSE, namek2 = FALSE;
  bool human1 = FALSE, human2 = FALSE;
  bool halfb1 = FALSE, halfb2 = FALSE;
  bool demon1 = FALSE, demon2 = FALSE;
  bool kaio1 = FALSE, kaio2 = FALSE;
  bool genie1 = FALSE, genie2 = FALSE;

  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg2 );

  if( ch->level < 60 )
  {
    ch_printf( ch, "Huh?" );
    return;
  }

  if( arg[0] == '\0' )
  {
    ch_printf( ch, "\n\rSyntax: potarafuse (first person) (second person)\n\r" );
    ch_printf( ch, "\n\rThe order in which you put the two people's names in does not matter.\n\r" );
    ch_printf( ch,
               "Whoever has the earring on the right ear will be the person in control\n\rof the resulting character.\n\r" );
    return;
  }
  if( ( rch = get_char_room( ch, arg ) ) == NULL )
  {
    ch_printf( ch, "There is nobody here named %s.\n\r", arg );
    return;
  }
  if( ( vch = get_char_room( ch, arg2 ) ) == NULL )
  {
    ch_printf( ch, "There is nobody here named %s.\n\r", arg2 );
    return;
  }
  if( IS_NPC( rch ) || IS_NPC( vch ) )
  {
    ch_printf( ch, "NPC's cannot fuse.\n\r" );
    return;
  }
  if( is_fused( rch ) )
  {
    ch_printf( ch, "%s has already fused before.\n\r", rch->name );
    return;
  }
  if( is_fused( vch ) )
  {
    ch_printf( ch, "%s has already fused before.\n\r", vch->name );
    return;
  }
  if( IS_IMMORTAL( rch ) || IS_IMMORTAL( vch ) )
  {
    ch_printf( ch, "Admins may not fuse.\n\r" );
    return;
  }
  if( is_android( rch ) || is_superandroid( rch ) || IS_ICER( rch ) || is_bio( rch ) || is_reploid( rch ) || is_hylian( rch ) || is_fierian( rch ) )
  {
    ch_printf( ch, "%s's race cannot fuse this way.\n\r", rch->name );
    return;
  }
  if( is_android( vch ) || is_superandroid( vch ) || IS_ICER( vch ) || is_bio( rch ) || is_reploid( rch ) || is_hylian( rch ) || is_fierian( rch ) )
  {
    ch_printf( ch, "%s's race cannot fuse this way.\n\r", vch->name );
    return;
  }
  if( rch->pcdata->clan )
  {
    ch_printf( ch, "%s must be outcasted from their clan first.\n\r", rch->name );
    return;
  }
  if( vch->pcdata->clan )
  {
    ch_printf( ch, "%s must be outcasted from their clan first.\n\r", vch->name );
    return;
  }

  if( IS_RANKED( rch ) )
  {
    ch_printf( ch, "%s must first abandon their rank.\n\r", rch->name );
    return;
  }
  if( IS_RANKED( vch ) )
  {
    ch_printf( ch, "%s must first abandon their rank.\n\r", vch->name );
    return;
  }

  if( is_transformed( rch ) )
  {
    ch_printf( ch, "%s must power down completely first.\n\r", rch->name );
    return;
  }
  if( is_transformed( vch ) )
  {
    ch_printf( ch, "%s must power down completely first.\n\r", vch->name );
    return;
  }
  if( ( ro = wearing_potara_right( rch ) ) == NULL )
  {
    if( ( ro = wearing_potara_left( rch ) ) == NULL )
    {
      ch_printf( ch, "%s is not wearing a potara earring.\n\r", rch->name );
      return;
    }
    else
      cleft = TRUE;
  }
  else
    cright = TRUE;
  if( ( vo = wearing_potara_right( vch ) ) == NULL )
  {
    if( ( vo = wearing_potara_left( vch ) ) == NULL )
    {
      ch_printf( ch, "%s is not wearing a potara earring.\n\r", vch->name );
      return;
    }
    else
      vleft = TRUE;
  }
  else
    vright = TRUE;

  if( cright && vright )
  {
    ch_printf( ch, "One of them must be wearing a left ear potara earring.\n\r" );
    return;
  }
  if( cleft && vleft )
  {
    ch_printf( ch, "One of them must be wearing a right ear potara earring.\n\r" );
    return;
  }
  if( cright && vleft )
  {
    dch = rch;
    sch = vch;
  }
  else if( cleft && vright )
  {
    dch = vch;
    sch = rch;
  }
  else
  {
    ch_printf( ch, "A serious bug has occured. Contant a coder immediately.\n\r" );
    return;
  }

  unequip_char( rch, ro );
  unequip_char( vch, vo );
  extract_obj( ro );
  extract_obj( vo );

  pObjIndex = get_obj_index( 78 );  /* Dual-potara-earring item */
  po = create_object_new( pObjIndex, 1, ORIGIN_OINVOKE, ch->name );
  po = obj_to_char( po, dch );
  equip_char( dch, po, WEAR_EARS );

  dch->fused[dch->fusions] = STRALLOC( sch->name );
  sch->fused[sch->fusions] = STRALLOC( dch->name );
  dch->fusions++;
  sch->fusions++;
  dch->bck_name = STRALLOC( dch->name );
  sch->bck_name = STRALLOC( sch->name );
  dch->bck_race = dch->race;
  sch->bck_race = sch->race;
  dch->bck_pl = dch->exp;
  sch->bck_pl = sch->exp;

  dch->pcdata->quest_curr += sch->pcdata->quest_curr;
  dch->pcdata->quest_accum += sch->pcdata->quest_accum;
  dch->pcdata->pkills += sch->pcdata->pkills;
  dch->pcdata->pdeaths += sch->pcdata->pdeaths;
  dch->pcdata->mkills += sch->pcdata->mkills;
  dch->pcdata->mdeaths += sch->pcdata->mdeaths;
  dch->pcdata->spar_wins += sch->pcdata->spar_wins;
  dch->pcdata->spar_loss += sch->pcdata->spar_loss;

  if( !str_cmp( get_race( dch ), "saiyan" ) )
    saiyan1 = TRUE;
  if( !str_cmp( get_race( sch ), "saiyan" ) )
    saiyan2 = TRUE;
  if( !str_cmp( get_race( dch ), "namek" ) )
    namek1 = TRUE;
  if( !str_cmp( get_race( sch ), "namek" ) )
    namek2 = TRUE;
  if( !str_cmp( get_race( dch ), "human" ) )
    human1 = TRUE;
  if( !str_cmp( get_race( sch ), "human" ) )
    human2 = TRUE;
  if( !str_cmp( get_race( dch ), "halfbreed" ) )
    halfb1 = TRUE;
  if( !str_cmp( get_race( sch ), "halfbreed" ) )
    halfb2 = TRUE;
  if( !str_cmp( get_race( dch ), "demon" ) )
    demon1 = TRUE;
  if( !str_cmp( get_race( sch ), "demon" ) )
    demon2 = TRUE;
  if( !str_cmp( get_race( dch ), "kaio" ) )
    kaio1 = TRUE;
  if( !str_cmp( get_race( sch ), "kaio" ) )
    kaio2 = TRUE;
  if( !str_cmp( get_race( dch ), "genie" ) )
    genie1 = TRUE;
  if( !str_cmp( get_race( sch ), "genie" ) )
    genie2 = TRUE;

  if( ( saiyan1 && namek2 ) )
  {
    dch->race = get_race_num( "saiyan-n" );
    dch->class = get_class_num( "saiyan-n" );
  }
  else if( ( kaio1 && kaio2 ) )
  {
    dch->race = get_race_num( "archangel" );
    dch->class = get_class_num( "archangel" );
  }
  else if( ( demon1 && kaio2 ) )
  {
    dch->race = get_race_num( "nephalim" );
    dch->class = get_class_num( "nephalim" );
  }
  else if( ( kaio1 && demon2 ) )
  {
    dch->race = get_race_num( "nephalim" );
    dch->class = get_class_num( "nephalim" );
  }
  else if( ( genie1 && genie2 ) )
  {
    dch->race = get_race_num( "candyman" );
    dch->class = get_class_num( "candyman" );
  }
  else if( ( demon1 && demon2 ) )
  {
    dch->race = get_race_num( "archdemon" );
    dch->class = get_class_num( "archdemon" );
  }
  else if( ( saiyan2 && namek1 ) )
  {
    dch->race = get_race_num( "namek-s" );
    dch->class = get_class_num( "namek-s" );
  }
  else if( ( saiyan1 && human2 ) )
  {
    dch->race = get_race_num( "saiyan-h" );
    dch->class = get_class_num( "saiyan-h" );
  }
  else if( ( saiyan2 && human1 ) )
  {
    dch->race = get_race_num( "human-s" );
    dch->class = get_class_num( "human-s" );
  }
  else if( ( saiyan1 && halfb2 ) )
  {
    dch->race = get_race_num( "saiyan-hb" );
    dch->class = get_class_num( "saiyan-hb" );
  }
  else if( ( saiyan2 && halfb1 ) )
  {
    dch->race = get_race_num( "halfbreed-s" );
    dch->class = get_class_num( "halfbreed-s" );
  }
  else if( ( saiyan1 && saiyan2 ) )
  {
    dch->race = get_race_num( "saiyan-s" );
    dch->class = get_class_num( "saiyan-s" );
  }
  else if( ( namek1 && halfb2 ) )
  {
    dch->race = get_race_num( "namek-hb" );
    dch->class = get_class_num( "namek-hb" );
  }
  else if( ( namek2 && halfb1 ) )
  {
    dch->race = get_race_num( "halfbreed-n" );
    dch->class = get_class_num( "halfbreed-n" );
  }
  else if( ( human1 && namek2 ) )
  {
    dch->race = get_race_num( "human-n" );
    dch->class = get_class_num( "human-n" );
  }
  else if( ( human2 && namek1 ) )
  {
    dch->race = get_race_num( "namek-h" );
    dch->class = get_class_num( "namek-h" );
  }
  else if( ( human1 && halfb2 ) )
  {
    dch->race = get_race_num( "human-hb" );
    dch->class = get_class_num( "human-hb" );
  }
  else if( ( human2 && halfb1 ) )
  {
    dch->race = get_race_num( "halfbreed-h" );
    dch->class = get_class_num( "halfbreed-h" );
  }
  else if( ( human1 && human2 ) )
  {
    dch->race = get_race_num( "human-h" );
    dch->class = get_class_num( "human-h" );
  }
  else if( ( namek1 && namek2 ) )
  {
    ch_printf( ch, "You cannot fuse like this." );
  }
  else if( ( halfb1 && halfb2 ) )
  {
    dch->race = get_race_num( "halfbreed-hb" );
    dch->class = get_class_num( "halfbreed-hb" );
  }

  dch->perm_str += 50;
  dch->perm_dex += 50;
  dch->perm_int += 50;
  dch->perm_con += 50;
  dch->exp += sch->exp;
  dch->pl = dch->exp;

  value = 95;
  for( sn = 0; sn < top_sn; sn++ )
  {
    if( skill_table[sn]->min_level[dch->class] == 0 || skill_table[sn]->skill_level[dch->class] == 0 )
      continue;

    if( skill_table[sn]->name && ( dch->exp >= skill_table[sn]->skill_level[dch->class] || value == 0 ) )
    {
      if( value > GET_ADEPT( dch, sn ) && !IS_IMMORTAL( dch ) )
        dch->pcdata->learned[sn] = GET_ADEPT( dch, sn );
      else
        dch->pcdata->learned[sn] = value;
    }
  }

  /*
   * message sent to dch 
   */
  ch_printf( dch, "&CThe potara earring on your right ear suddenly begins to glow,\n\r" );
  ch_printf( dch, "as does the one on %s's left ear. You are then violently hurled\n\r", sch->name );
  ch_printf( dch, "through the air towards %s, and slam into each other. A blinding\n\r", sch->name );
  ch_printf( dch, "light envelopes you as you feel your mind and body ripped apart\n\r" );
  ch_printf( dch, "and mixed together with %s's; becoming one. You are no longer who\n\r", sch->name );
  ch_printf( dch, "you once were, nor is %s. You thoughts are seperate, but one.\n\r", sch->name );
  ch_printf( dch, "Your movements are seperate, but one. Even your voice sounds as\n\r" );
  ch_printf( dch, "if the both of you were speaking at once. You have become a fused\n\r" );
  ch_printf( dch, "being.\n\r" );
  ch_printf( dch, "&z(You were wearing the earring on the right ear, so you are the\n\r" );
  ch_printf( dch, "one in control of the fused character. You will be required to\n\r" );
  ch_printf( dch, "choose a new name, based on the combination of yours, and %'s.\n\r", sch->name );
  ch_printf( dch, "You will also need to edit your bio to reflect the change in\n\r" );
  ch_printf( dch, "your state of being, unless you didn't have an authed bio to\n\r" );
  ch_printf( dch, "begin with. Congratulations.)\n\r" );

  /*
   * message sent to sch 
   */
  ch_printf( sch, "&CThe potara earring on your left ear suddenly begins to glow,\n\r" );
  ch_printf( sch, "as does the one on %s's right ear. You are then violently hurled\n\r", dch->name );
  ch_printf( sch, "through the air towards %s, and slam into each other. A blinding\n\r", dch->name );
  ch_printf( sch, "light envelopes you as you feel your mind and body ripped apart\n\r" );
  ch_printf( sch, "and mixed together with %s's; becoming one. You are no longer who\n\r", dch->name );
  ch_printf( sch, "you once were, nor is %s. You thoughts are seperate, but one.\n\r", dch->name );
  ch_printf( sch, "Your movements are seperate, but one. Even your voice sounds as\n\r" );
  ch_printf( sch, "if the both of you were speaking at once. You have become a fused\n\r" );
  ch_printf( sch, "being.\n\r" );
  ch_printf( sch, "&R(You were wearing the earring on the left ear, so you are now\n\r" );
  ch_printf( sch, "put into fusion stasis (meaning you are frozen), where you will\n\r" );
  ch_printf( sch, "remain for all time. Consider this character non-existant. You\n\r" );
  ch_printf( sch, "may not recreate as this character, nor can it be renamed. That\n\r" );
  ch_printf( sch, "is the price you pay for fusing.\n\r" );

  while( ( o = carrying_noquit( sch ) ) != NULL )
  {
    obj_from_char( o );
    obj_to_room( o, sch->in_room );
    ch_printf( sch, "&wYou drop %s&w.\n\r", o->short_descr );
  }

  SET_BIT( dch->fusionflags, FUSION_POTARA );
  SET_BIT( sch->fusionflags, FUSION_STASIS );
  do_reserve( ch, dch->name );
  do_reserve( ch, sch->name );
  char_to( sch, ROOM_VNUM_FUSIONSTASIS );
  save_char_obj( dch );
  save_char_obj( sch );

  if( is_saiyan_s( dch ) )
    dch->pcdata->learned[gsn_super_oozaru] = 95;

  /*
   * do_sset(ch,strcat(dch->name," all 95"));
   */
  ch_printf( ch, "%s and %s have been fused together.\n\r", dch->name, sch->name );
}

OBJ_DATA *fusion_item_1( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc != WEAR_NONE )
    {
      if( obj->pIndexData->vnum == 618 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *fusion_item_2( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc != WEAR_NONE )
    {
      if( obj->pIndexData->vnum == 620 )
        return obj;
    }
  }
  return NULL;
}


void do_namekfuse( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *rch;
  CHAR_DATA *vch;
  CHAR_DATA *dch; /*dominant fuse char */
  CHAR_DATA *sch; /*stasis fuse char */
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  bool cright = FALSE;
  bool vright = FALSE;
  bool cleft = FALSE;
  bool vleft = FALSE;
  OBJ_DATA *ro;
  OBJ_DATA *vo;
  OBJ_DATA *po;
  OBJ_DATA *o;
  OBJ_INDEX_DATA *pObjIndex;
  int sn;
  int value;
  bool namek1 = FALSE, namek2 = FALSE;

  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg2 );

  if( arg[0] == '\0' )
  {
    ch_printf( ch, "\n\rSyntax:  namekfuse (first person) (second person)\n\r" );
    ch_printf( ch, "\n\rThe order in which you put the two people's names in does not matter.\n\r" );
    return;
  }
  if( ( rch = get_char_room( ch, arg ) ) == NULL )
  {
    ch_printf( ch, "There is nobody here named %s.\n\r", arg );
    return;
  }
  if( ( vch = get_char_room( ch, arg2 ) ) == NULL )
  {
    ch_printf( ch, "There is nobody here named %s.\n\r", arg2 );
    return;
  }
  if( IS_NPC( rch ) || IS_NPC( vch ) )
  {
    ch_printf( ch, "NPC's cannot fuse.\n\r" );
    return;
  }
  if( is_fused( rch ) )
  {
    ch_printf( ch, "%s has already fused before.\n\r", rch->name );
    return;
  }
  if( is_fused( vch ) )
  {
    ch_printf( ch, "%s has already fused before.\n\r", vch->name );
    return;
  }
  if( IS_IMMORTAL( rch ) || IS_IMMORTAL( vch ) )
  {
    ch_printf( ch, "Admins may not fuse.\n\r" );
    return;
  }
  if( rch->pcdata->clan )
  {
    ch_printf( ch, "%s must be outcasted from their clan first.\n\r", rch->name );
    return;
  }
  if( vch->pcdata->clan )
  {
    ch_printf( ch, "%s must be outcasted from their clan first.\n\r", vch->name );
    return;
  }

  if( is_transformed( rch ) )
  {
    ch_printf( ch, "%s must power down completely first.\n\r", rch->name );
    return;
  }
  if( is_transformed( vch ) )
  {
    ch_printf( ch, "%s must power down completely first.\n\r", vch->name );
    return;
  }
  if( ( ro = namek_item_1( rch ) ) == NULL )
  {
    if( ( ro = namek_item_2( rch ) ) == NULL )
    {
      ch_printf( ch, "%s is not wearing a needed item..\n\r", rch->name );
      return;
    }
    else
      cleft = TRUE;
  }
  else
    cright = TRUE;
  if( ( vo = namek_item_1( vch ) ) == NULL )
  {
    if( ( vo = namek_item_2( vch ) ) == NULL )
    {
      ch_printf( ch, "%s is not wearing a needed item.\n\r", vch->name );
      return;
    }
    else
      vleft = TRUE;
  }
  else
    vright = TRUE;

  if( cright && vright )
  {
    ch_printf( ch, "One of them must be wearing a needed item.\n\r" );
    return;
  }
  if( cleft && vleft )
  {
    ch_printf( ch, "One of them must be wearing a needed item.\n\r" );
    return;
  }
  if( cright && vleft )
  {
    dch = rch;
    sch = vch;
  }
  else if( cleft && vright )
  {
    dch = vch;
    sch = rch;
  }
  else
  {
    ch_printf( ch, "Karn fucked up. A bug accured. Bitch him out!\n\r" );
    return;
  }

  unequip_char( rch, ro );
  unequip_char( vch, vo );
  extract_obj( ro );
  extract_obj( vo );

  pObjIndex = get_obj_index( 666 );
  po = create_object_new( pObjIndex, 1, ORIGIN_OINVOKE, ch->name );
  po = obj_to_char( po, dch );

  dch->fused[dch->fusions] = STRALLOC( sch->name );
  sch->fused[sch->fusions] = STRALLOC( dch->name );
  dch->fusions++;
  sch->fusions++;
  dch->bck_name = STRALLOC( dch->name );
  sch->bck_name = STRALLOC( sch->name );
  dch->bck_race = dch->race;
  sch->bck_race = sch->race;
  dch->bck_pl = dch->exp;
  sch->bck_pl = sch->exp;

  dch->pcdata->quest_curr += sch->pcdata->quest_curr;
  dch->pcdata->quest_accum += sch->pcdata->quest_accum;
  dch->pcdata->pkills += sch->pcdata->pkills;
  dch->pcdata->pdeaths += sch->pcdata->pdeaths;
  dch->pcdata->mkills += sch->pcdata->mkills;
  dch->pcdata->mdeaths += sch->pcdata->mdeaths;
  dch->pcdata->spar_wins += sch->pcdata->spar_wins;
  dch->pcdata->spar_loss += sch->pcdata->spar_loss;

  if( !str_cmp( get_race( dch ), "namek" ) )
    namek1 = TRUE;
  if( !str_cmp( get_race( sch ), "namek" ) )
    namek2 = TRUE;

  if( ( namek1 && namek2 ) )
  {
    dch->race = get_race_num( "super-namek" );
    dch->class = get_class_num( "super-namek" );
  }

  dch->exp += sch->exp;
  dch->pl = dch->exp;

  value = 95;
  for( sn = 0; sn < top_sn; sn++ )
  {
    if( skill_table[sn]->min_level[dch->class] == 0 || skill_table[sn]->skill_level[dch->class] == 0 )
      continue;

    if( skill_table[sn]->name && ( dch->exp >= skill_table[sn]->skill_level[dch->class] || value == 0 ) )
    {
      if( value > GET_ADEPT( dch, sn ) && !IS_IMMORTAL( dch ) )
        dch->pcdata->learned[sn] = GET_ADEPT( dch, sn );
      else
        dch->pcdata->learned[sn] = value;
    }
  }

  /*
   * message sent to dch 
   */

  ch_printf( dch, "You life force fuses with %s's causing them to merge with you!\n\r", sch->name );

  /*
   * message sent to sch 
   */
  ch_printf( sch, "Your body fuses with %s's and you agree to give your namekian power for some reasion...\n\r", dch->name );

  while( ( o = carrying_noquit( sch ) ) != NULL )
  {
    obj_from_char( o );
    obj_to_room( o, sch->in_room );
    ch_printf( sch, "&wYou drop %s&w.\n\r", o->short_descr );
  }

  SET_BIT( dch->fusionflags, FUSION_NAMEK );
  SET_BIT( sch->fusionflags, FUSION_STASIS );
  do_reserve( ch, dch->name );
  do_reserve( ch, sch->name );
  char_to( sch, ROOM_VNUM_FUSIONSTASIS );
  save_char_obj( dch );
  save_char_obj( sch );

  /*
   * do_sset(ch,strcat(dch->name," all 95"));
   */
  ch_printf( ch, "%s and %s have been fused together.\n\r", dch->name, sch->name );
}

bool is_fused( CHAR_DATA * ch )
{
  if( IS_SET( ch->fusionflags, FUSION_DANCE ) )
    return TRUE;
  else if( IS_SET( ch->fusionflags, FUSION_POTARA ) )
    return TRUE;
  else if( IS_SET( ch->fusionflags, FUSION_NAMEK ) )
    return TRUE;
  else if( IS_SET( ch->fusionflags, FUSION_STASIS ) )
    return TRUE;
  else if( IS_SET( ch->fusionflags, FUSION_SUPERANDROID ) )
    return TRUE;
  else
    return FALSE;
}

bool is_dancefused( CHAR_DATA * ch )
{
  if( IS_SET( ch->fusionflags, FUSION_DANCE ) )
    return TRUE;
  else
    return FALSE;
}

bool is_potarafused( CHAR_DATA * ch )
{
  if( IS_SET( ch->fusionflags, FUSION_POTARA ) )
    return TRUE;
  else
    return FALSE;
}

bool is_namekfused( CHAR_DATA * ch )
{
  if( IS_SET( ch->fusionflags, FUSION_NAMEK ) )
    return TRUE;
  else
    return FALSE;
}

bool fusion_stasis( CHAR_DATA * ch )
{
  if( IS_SET( ch->fusionflags, FUSION_STASIS ) )
    return TRUE;
  else
    return FALSE;
}

OBJ_DATA *carrying_noquit( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( IS_OBJ_STAT( obj, ITEM_NOQUIT ) )
    {
      return obj;
    }
  }
  return NULL;
}

OBJ_DATA *carrying_dball( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->item_type == ITEM_DRAGONBALL )
    {
      return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_potara_right( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc != WEAR_NONE )
    {
      if( obj->pIndexData->vnum == 76 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_potara_left( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc != WEAR_NONE )
    {
      if( obj->pIndexData->vnum == 77 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *namek_item_1( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc != WEAR_NONE )
    {
      if( obj->pIndexData->vnum == 100297 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *namek_item_2( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc != WEAR_NONE )
    {
      if( obj->pIndexData->vnum == 5017 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_chip( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    /*
     * Chips should have been like this a *long* time ago -Goku 09.30.04 
     */
    if( CAN_WEAR( obj, ITEM_WEAR_PANEL ) )
      return obj;
  }
  return NULL;
}

OBJ_DATA *wearing_shield( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_SHIELD )
      return obj;
  }
  return NULL;
}

bool wearing_sentient_chip( CHAR_DATA * ch )
{
  if( IS_AFFECTED( ch, AFF_SENT_CHIP ) )
    return TRUE;
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_SHIELD )
    {
      if( obj->pIndexData->vnum == 1337 )
        return TRUE;
    }
  }
  return FALSE;
}

OBJ_DATA *wearing_sentient_chip2( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_SHIELD )
    {
      if( obj->pIndexData->vnum == 1337 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_space_suit( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_FACE )
    {
      if( obj->pIndexData->vnum == 450 )
        return obj;
    }
  }
  return NULL;
}

bool seven_dballs_here( CHAR_DATA * ch, char *planet )
{
  OBJ_DATA *obj;
  bool onestar = FALSE;
  bool twostar = FALSE;
  bool threestar = FALSE;
  bool fourstar = FALSE;
  bool fivestar = FALSE;
  bool sixstar = FALSE;
  bool sevenstar = FALSE;

  if( !str_cmp( planet, "earth" ) )
  {
    if( ( obj = get_obj_vnum_room( ch, 1280 ) ) != NULL )
      onestar = TRUE;
    if( ( obj = get_obj_vnum_room( ch, 1281 ) ) != NULL )
      twostar = TRUE;
    if( ( obj = get_obj_vnum_room( ch, 1282 ) ) != NULL )
      threestar = TRUE;
    if( ( obj = get_obj_vnum_room( ch, 1283 ) ) != NULL )
      fourstar = TRUE;
    if( ( obj = get_obj_vnum_room( ch, 1284 ) ) != NULL )
      fivestar = TRUE;
    if( ( obj = get_obj_vnum_room( ch, 1285 ) ) != NULL )
      sixstar = TRUE;
    if( ( obj = get_obj_vnum_room( ch, 1286 ) ) != NULL )
      sevenstar = TRUE;

    if( onestar && twostar && threestar && fourstar && fivestar && sixstar && sevenstar )
      return TRUE;
    else
      return FALSE;
  }

  return FALSE;
}

void do_dbwhere( CHAR_DATA * ch, char *argument )
{
  ROOM_INDEX_DATA *r;
  CHAR_DATA *vch;
  OBJ_DATA *obj;
  OBJ_DATA *tobj;
  int count = 0;
  int c_total = 0;
  int r_total = 0;

  if( ch->level > 65 )
  {
    ch_printf( ch, "Huh?\n\r" );
    return;
  }
  ch_printf( ch, "The following people have dragonballs:\n\r" );
  for( vch = first_char; vch; vch = vch->next )
  {
    if( carrying_dball( vch ) )
    {
      for( obj = ch->first_carrying; obj; obj = obj->next_content )
      {
        if( obj->item_type == ITEM_DRAGONBALL )
        {
          count++;
          c_total++;
        }
        if( !obj->next_content )
          break;
      }
      ch_printf( ch, "%s has %d dragonball(s).\n\r", ch->name, count );
    }
    count = 0;
    if( !vch->next )
      break;
  }
  count = 0;
  for( obj = first_object; obj; obj = obj->next )
  {
    if( !obj->in_room )
      continue;
    if( obj->item_type != ITEM_DRAGONBALL )
      continue;
    r = obj->in_room;
    for( tobj = r->first_content; tobj; tobj = tobj->next_content )
    {
      if( tobj->item_type == ITEM_DRAGONBALL )
      {
        count++;
        r_total++;
      }
    }
    ch_printf( ch, "Room vnum %d (%s) has %d dragonballs.\n\r", r->vnum, r->name, count );
    count = 0;
    if( !obj->next )
      break;
  }
  ch_printf( ch, "A total of %d dragonballs found on characters.\n\r", c_total );
  ch_printf( ch, "A total of %d dragonballs found in rooms.\n\r", r_total );
}

void do_summon( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  /*
   * CHAR_DATA *dragon;
   * MOB_INDEX_DATA *pMobIndex;
   */
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;

  if( IS_NPC( ch ) )
    return;

  if( summon_state != SUMMON_NONE )
  {
    ch_printf( ch, "A dragon is already being summoned!\n\r" );
    return;
  }

  if( summon_room != NULL || summon_area != NULL )
  {
    ch_printf( ch, "A dragon is already summoned somewhere else.\n\r" );
    return;
  }

  if( argument[0] == '\0' )
  {
    ch_printf( ch, "\n\rSyntax: summon (dragon)\n\r" );
    ch_printf( ch, "\n\rDragon has to be one of the following:\n\r" );
    ch_printf( ch, "shenron | porunga\n\r" );
    return;
  }
  if( xIS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
  {
    ch_printf( ch, "You can't summon the dragon in a safe room.\n\r" );
    return;
  }
  if( !str_cmp( argument, "shenron" ) )
  {
    if( ( victim = get_char_room( ch, "Kalthizar" ) ) == NULL )
    {
      ch_printf( ch, "\n\r&RYou cannot summon the dragon without Kalthizar here.\n\r" );
      return;
    }
    if( !seven_dballs_here( ch, "earth" ) )
    {
      ch_printf( ch, "\n\r&RAll seven dragonballs must be here on the ground for you to be able to summon the dragon.\n\r" );
      return;
    }

    summon_state = SUMMON_SKY1;
    summon_area = ch->in_room->area;
    summon_room = ch->in_room;

    sprintf( buf, "%s has summoned the eternal dragon Shenron at room %d", ch->name, ch->in_room->vnum );
    to_channel( buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );

    if( ( obj = get_obj_vnum_room( ch, 1280 ) ) != NULL )
      REMOVE_BIT( obj->wear_flags, ITEM_TAKE );
    if( ( obj = get_obj_vnum_room( ch, 1281 ) ) != NULL )
      REMOVE_BIT( obj->wear_flags, ITEM_TAKE );
    if( ( obj = get_obj_vnum_room( ch, 1282 ) ) != NULL )
      REMOVE_BIT( obj->wear_flags, ITEM_TAKE );
    if( ( obj = get_obj_vnum_room( ch, 1283 ) ) != NULL )
      REMOVE_BIT( obj->wear_flags, ITEM_TAKE );
    if( ( obj = get_obj_vnum_room( ch, 1284 ) ) != NULL )
      REMOVE_BIT( obj->wear_flags, ITEM_TAKE );
    if( ( obj = get_obj_vnum_room( ch, 1285 ) ) != NULL )
      REMOVE_BIT( obj->wear_flags, ITEM_TAKE );
    if( ( obj = get_obj_vnum_room( ch, 1286 ) ) != NULL )
      REMOVE_BIT( obj->wear_flags, ITEM_TAKE );

    xSET_BIT( summon_room->room_flags, ROOM_SAFE );
    return;
  }
  if( !str_cmp( argument, "porunga" ) )
  {
    do_summon( ch, "" );
    return;
  }
  do_summon( ch, "" );
}

void do_grant( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) )
    return;

  if( argument[0] == '\0' )
  {
    ch_printf( ch, "\n\rSyntax: grant (option)\n\r" );
    ch_printf( ch, "\n\rOption has to be one of the following:\n\r" );
    ch_printf( ch, "  finish (for the dragons leaving scene)\n\r" );
    return;
  }

  if( !str_cmp( argument, "finish" ) )
  {
    summon_state = SUMMON_LEAVE1;
    return;
  }
  return;
}

/* Unused Transformation(s)*/

void do_ultra_namek( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_unamek ) )
      return;
  }

//if ( !IS_IMMORTAL(ch) ) {send_to_char("pfft. no. :)\n\r", ch); return;}

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  double pl_mult = 0;
  int arg;

  arg = atoi( argument );

  if( xIS_SET( ( ch )->affected_by, AFF_UNAMEK ) && arg == 0 )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_UNAMEK );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }
  else if( !xIS_SET( ( ch )->affected_by, AFF_UNAMEK ) && arg == 0 )
  {
    ch_printf( ch, "You? Ultra Namek? Pfft.\n\r" );
    return;
  }

  pl_mult = 38;

  if( arg > pl_mult )
    pl_mult = ( int )pl_mult;
  else if( arg < 1 )
    pl_mult = 1;
  else
    pl_mult = arg;

  if( ch->mana < skill_table[gsn_unamek]->min_mana * pl_mult )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( arg < 2 || arg > 38 )
  {
    send_to_char( "Syntax: ultra [2-38]\n\r", ch );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_unamek]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_unamek ) )
  {
    act( AT_SKILL,
         " As you begin to power up, you release all your power from your body, a greenish light filling the area, expanding further outwards.  Suddenly, you brace yourself, drawing the energy back into your body suddenly, imploding the power so it is all focused to a higher point within your body.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL,
         "$n begins to power up, releasing all $s energy from $s body, a greenish light filling the area, expanding further outwards. Suddenly, $e braces $s body, drawing the energy back into $s body, imploding $s power so it is all focused to a higher point within $s body.",
         ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_UNAMEK );

    ch->pl = ch->exp * pl_mult;

    transStatApply( ch, pl_mult / 2, pl_mult * 1.5, pl_mult * 2, pl_mult * 1.5 );

    learn_from_success( ch, gsn_unamek );
  }
  else
  {
    act( AT_SKILL, "You can't quite focus well enough to draw out your power.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n can't quite focus well enough to draw out $s power.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_unamek );
  }

  ch->mana -= skill_table[gsn_unamek]->min_mana * pl_mult;
  return;
}

void do_hssj( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_hssj ) )
      return;
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->combatFlags, CMB_NO_HEART ) )
    {
      send_to_char( "You must first disable heart to become a Hyper Super Saiya-Jin.\n\r", ch );
      return;
    }
  }

  if( xIS_SET( ch->affected_by, AFF_ESSJ ) )
    xREMOVE_BIT( ch->affected_by, AFF_ESSJ );

  if( xIS_SET( ch->affected_by, AFF_KAIOKEN ) )
    xREMOVE_BIT( ch->affected_by, AFF_KAIOKEN );

  double pl_mult = 0;
  int arg;

  arg = atoi( argument );

  if( xIS_SET( ( ch )->affected_by, AFF_HSSJ ) && arg == 0 )
  {
    if( !IS_NPC( ch ) )
    {
      ch->pcdata->haircolor = ch->pcdata->orignalhaircolor;
      ch->pcdata->eyes = ch->pcdata->orignaleyes;
    }
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_HSSJ );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }
  else if( !xIS_SET( ( ch )->affected_by, AFF_HSSJ ) && arg == 0 )
  {
    ch_printf( ch, "Syntax: hssj [10-24]\n\r" );
    return;
  }

  pl_mult = 24;

  if( arg > pl_mult )
    pl_mult = ( int )pl_mult;
  else if( arg < 1 )
    pl_mult = 1;
  else
    pl_mult = arg;

  if( ch->mana < skill_table[gsn_hssj]->min_mana * pl_mult )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( arg < 10 || arg > 24 )
  {
    send_to_char( "Syntax: hssj [10-24]\n\r", ch );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_hssj]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_hssj ) )
  {
    act( AT_YELLOW, "Your hair flashes blonde and your eyes turn blue as your muscles surge", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "with energy before compacting in size.  A large aura of bright white flames", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "erupts around you and an earsplitting pop can be heard as the coursing", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "force embracing your body collapses the surrounding air.", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "$n's hair flashes blonde and their eyes turn blue as $s muscles surge", ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW, "with energy before compacting in size.  A large aura of bright white flames", ch, NULL, NULL,
         TO_NOTVICT );
    act( AT_YELLOW, "erupts around them and an earsplitting pop can be heard as the coursing", ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW, "force embracing their body collapses the surrounding air.", ch, NULL, NULL, TO_NOTVICT );
    transStatRemove( ch );
    if( !IS_NPC( ch ) )
    {
      ch->pcdata->eyes = 0;
      ch->pcdata->haircolor = 3;
    }
    xSET_BIT( ( ch )->affected_by, AFF_HSSJ );

    ch->pl = ch->exp * pl_mult;
    if( pl_mult == 10 )
      transStatApply( ch, 14, 5, 3, 6 );
    if( pl_mult == 11 )
      transStatApply( ch, 15, 7, 3, 9 );
    if( pl_mult == 12 )
      transStatApply( ch, 16, 7, 4, 12 );
    if( pl_mult == 13 )
      transStatApply( ch, 15, 9, 4, 15 );
    if( pl_mult == 14 )
      transStatApply( ch, 16, 9, 5, 18 );
    if( pl_mult == 15 )
      transStatApply( ch, 17, 12, 5, 21 );
    if( pl_mult == 16 )
      transStatApply( ch, 18, 12, 6, 24 );
    if( pl_mult == 17 )
      transStatApply( ch, 19, 14, 7, 26 );
    if( pl_mult == 18 )
      transStatApply( ch, 21, 16, 8, 28 );
    if( pl_mult == 19 )
      transStatApply( ch, 23, 18, 9, 31 );
    if( pl_mult == 20 )
      transStatApply( ch, 26, 19, 12, 34 );
    if( pl_mult == 21 )
      transStatApply( ch, 26, 19, 13, 34 );
    if( pl_mult == 22 )
      transStatApply( ch, 26, 21, 15, 34 );
    if( pl_mult == 23 )
      transStatApply( ch, 28, 21, 15, 35 );
    if( pl_mult == 24 )
      transStatApply( ch, 28, 22, 17, 35 );
    else
      transStatApply( ch, pl_mult, pl_mult, pl_mult, pl_mult );

    learn_from_success( ch, gsn_hssj );
  }
  else
  {
    act( AT_YELLOW, "Your hair flashs blonde as flames of energy flicker around you,", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "before quickly dying down.", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "$n's hair flashs blonde as flames of energy flicker around them,", ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW, "before quickly dying down.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_hssj );
  }

  ch->mana -= skill_table[gsn_hssj]->min_mana * pl_mult;
  return;
}

void do_essj( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_essj ) )
      return;
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->combatFlags, CMB_NO_HEART ) )
    {
      send_to_char( "You must first disable heart to become an Extreme Super Saiya-Jin.\n\r", ch );
      return;
    }
  }

  if( !IS_NPC( ch ) )
  {
    ch->pcdata->haircolor = ch->pcdata->orignalhaircolor;
    ch->pcdata->eyes = ch->pcdata->orignaleyes;
  }

  if( xIS_SET( ch->affected_by, AFF_KAIOKEN ) )
    xREMOVE_BIT( ch->affected_by, AFF_KAIOKEN );

  if( xIS_SET( ch->affected_by, AFF_HSSJ ) )
    xREMOVE_BIT( ch->affected_by, AFF_HSSJ );

  double pl_mult = 0;
  int arg;

  arg = atoi( argument );

  if( xIS_SET( ( ch )->affected_by, AFF_ESSJ ) && arg == 0 )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_ESSJ );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }
  else if( !xIS_SET( ( ch )->affected_by, AFF_ESSJ ) && arg == 0 )
  {
    ch_printf( ch, "Syntax: essj [27-39]\n\r" );
    return;
  }

  pl_mult = 39;

  if( arg > pl_mult )
    pl_mult = ( int )pl_mult;
  else if( arg < 1 )
    pl_mult = 1;
  else
    pl_mult = arg;

  if( ch->mana < skill_table[gsn_essj]->min_mana * pl_mult )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( arg < 27 || arg > 39 )
  {
    send_to_char( "Syntax: essj [27-39]\n\r", ch );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_essj]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_essj ) )
  {
    act( AT_RED, "You close your eyes and concentrate, focusing your rage into one point", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "within your being.  A dot of red energy appears on your forehead, and then", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "quickly spreads out over your body.  Black fur sprouts over your body, with", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "a tinge of blood red highlights appearing randomly, as your hair lengthen's", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "slightly and grows darker.  A light surrounds you as your rage and power", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "are intertwined into one.", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "$n closes $s eyes and concentrates, focusing $s rage into one point", ch, NULL, NULL, TO_NOTVICT );
    act( AT_RED, "within $s body.  A dot of red energy appears on $n's forehead, and then", ch, NULL, NULL, TO_NOTVICT );
    act( AT_RED, "quickly spreads out over $s body.  Black fur sprouts over $n's body, with a", ch, NULL, NULL, TO_NOTVICT );
    act( AT_RED, "tinge of blood red highlights appearing randomly, as $s hair lengthen's", ch, NULL, NULL, TO_NOTVICT );
    act( AT_RED, "slightly and grows darker.  A light surrounds $n as $s rage and power are", ch, NULL, NULL, TO_NOTVICT );
    act( AT_RED, "intertwined into one.", ch, NULL, NULL, TO_NOTVICT );
    transStatRemove( ch );

    xSET_BIT( ( ch )->affected_by, AFF_ESSJ );

    ch->pl = ch->exp * pl_mult;
    if( pl_mult == 27 )
      transStatApply( ch, 28, 22, 17, 35 );
    if( pl_mult == 28 )
      transStatApply( ch, 29, 22, 17, 36 );
    if( pl_mult == 29 )
      transStatApply( ch, 30, 23, 18, 37 );
    if( pl_mult == 30 )
      transStatApply( ch, 31, 23, 18, 38 );
    if( pl_mult == 31 )
      transStatApply( ch, 32, 24, 19, 39 );
    if( pl_mult == 32 )
      transStatApply( ch, 33, 24, 19, 40 );
    if( pl_mult == 33 )
      transStatApply( ch, 34, 25, 20, 45 );
    if( pl_mult == 34 )
      transStatApply( ch, 35, 25, 20, 46 );
    if( pl_mult == 35 )
      transStatApply( ch, 36, 26, 21, 47 );
    if( pl_mult == 36 )
      transStatApply( ch, 37, 26, 21, 48 );
    if( pl_mult == 37 )
      transStatApply( ch, 38, 27, 22, 49 );
    if( pl_mult == 38 )
      transStatApply( ch, 40, 27, 22, 50 );
    if( pl_mult == 39 )
      transStatApply( ch, 42, 28, 23, 51 );
    else
      transStatApply( ch, pl_mult, pl_mult, pl_mult, pl_mult );

    learn_from_success( ch, gsn_essj );
  }
  else
  {
    act( AT_WHITE, "&zBlack fur begins to sprout all over your body, but quickly disappears.", ch, NULL, NULL, TO_CHAR );
    act( AT_WHITE, "&zBlack fur begins to sprout all over $n's body, but quickly disappears.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_essj );
  }

  ch->mana -= skill_table[gsn_essj]->min_mana * pl_mult;
  return;
}

/* end of unused transformations */

void do_immortalaurareset( CHAR_DATA * ch, char *argument )
{
  	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

       argument = one_argument( argument, arg1 );
       one_argument( argument, arg2 );
	if( IS_NPC(ch)  )
	{
		send_to_char("Not for mobs.",ch);
		return;
	}
	if ( !ch->desc )
	{
		send_to_char( "You have no descriptor\n\r", ch );
		return ;
	}
	if ( get_trust(ch) < 50 )
	{
		send_to_char( "Immortals only.\n\r", ch );
		return ;
	}
	if ( arg1[0] == '\0' )
	{
		send_to_char( "Syntax: resetaura <victim> aura \n\r", ch );
		return ;
	}
       if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
       {
              send_to_char( "They aren't here.\n\r", ch );
              return;
       }
	if ( !str_cmp( arg2, "aura" ) )
	{
		victim->pcdata->auraColorPowerUp = 0;
		pager_printf_color (ch, "&wOk.\n\r");
		return;
	}
  return;
}
void do_finalshine( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_finalshine ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_finalshine]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_finalshine]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_finalshine]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_finalshine]->focus;

  WAIT_STATE( ch, skill_table[gsn_finalshine]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_finalshine ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 70, 80 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( AT_YELLOW, "You outstretch your arms and put your hands together, fingers straight", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "with no gaps and making a horizontal pattern.  You begin to charge a yellow", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "ball in front of your hands, electricity spewing everywhere.  As you pour", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "more energy into it, the ball turns &Ggreen&Y.  You look at $N and scream", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "'See if you can dodge this one!  Final Shine!'  A giant green beam moving", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "at an incredible speed engulfs and incinerates $N. &W[$t]", ch, num_punct( dam ), victim, TO_CHAR );

    act( AT_YELLOW, "$n sits with their arms outstretched and hands together, fingers", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "straight with no gaps and making a horizontal pattern.  Begins to charge a", ch, NULL, victim,
         TO_VICT );
    act( AT_YELLOW, "yellow ball in front of their hands, electricity spewing everywhere.  As $n", ch, NULL, victim,
         TO_VICT );
    act( AT_YELLOW, "pours more energy into it, the ball turns green.  $n looks at you and", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "screams 'See if you can dodge this one!  Final Shine!'  A giant green beam", ch, NULL, victim,
         TO_VICT );
    act( AT_YELLOW, "moving at an incredible speed engulfs and incinerates you dealing massive", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "damage. &W[$t]", ch, num_punct( dam ), victim, TO_VICT );

    act( AT_YELLOW, "$n with arms outsretched and hands together, fingers", ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW, "straight with no gaps and making a horizontal pattern. $n", ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW, "begins to charge a yellow ball in front of their hands, electricity spewing", ch, NULL, NULL,
         TO_NOTVICT );
    act( AT_YELLOW, "everywhere.  As they pour more energy into it, the ball turns green. ", ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW, "$n looks at $N and screams 'See if you can dodge this", ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW, "one!  Final Shine!'  A giant green beam moving at an incredible speed", ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW, "engulfs and incinerates $n. &W[$t]", ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_finalshine );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_BLUE, "You missed $N with your Final Shine.", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "$n misses you with $s Final Shine.", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "$n missed $N with a Final Shine.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_finalshine );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_finalshine]->min_mana;
  return;
}

/* saiyan s only skill. */
void do_super_oozaru( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_super_oozaru ) )
      return;
  }
  if( !IS_NPC( ch ) )
  {
    if( IS_SET( ch->pcdata->flags, PCFLAG_KNOWSMYSTIC ) )
    {
      ch_printf( ch, "You are unable to call upon those powers while you know mystic.\n\r" );
      return;
    }
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) || xIS_SET( ( ch )->affected_by, AFF_SSJ ) ||
      xIS_SET( ( ch )->affected_by, AFF_SSJ2 ) || xIS_SET( ( ch )->affected_by, AFF_SSJ3 ) ||
      xIS_SET( ( ch )->affected_by, AFF_SSJ4 ) || xIS_SET( ( ch )->affected_by, AFF_USSJ ) ||
      xIS_SET( ( ch )->affected_by, AFF_USSJ2 ) )
  {
    send_to_char( "You can't transform.\n\r", ch );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) || xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    send_to_char( "You can't transform while you are already in Oozaru.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_super_oozaru]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_super_oozaru]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_super_oozaru ) )
  {
    act( AT_YELLOW, "You stand for a moment and then a complete rage enters your body, the", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "purest of all.  As if you looked into a full moon, you begin to see fur on", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "your hands and it all becomes golden and you seem as if you are rising,", ch, NULL, NULL, TO_CHAR );
    act( AT_YELLOW, "after that you lose memory. ", ch, NULL, NULL, TO_CHAR );

    act( AT_YELLOW, "$n stands for second, then begins to scream in rage, first they", ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW, "begin to get fur all over their body, which turns yellow.  Then their face", ch, NULL, NULL,
         TO_NOTVICT );
    act( AT_YELLOW, "structure turns into that of a giant ape. Their eyes, once blue turn", ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW, "completely white and their hair stands on end. At first they have a", ch, NULL, NULL, TO_NOTVICT );
    act( AT_YELLOW, "dumbfounded look, but then a look of pure hatred.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_SUPER_OOZARU );
    ch->pl = ch->exp * 40;
    transStatApply( ch, 38, 30, 15, 38 );
    learn_from_success( ch, gsn_super_oozaru );
  }
  else
  {
    act( AT_BLUE, "Hair forms on your hands, but then quickly disappears.", ch, NULL, NULL, TO_CHAR );
    act( AT_BLUE, "You think you see hair on $n's fingers, but you must have been seeing things.", ch, NULL, NULL,
         TO_NOTVICT );
    learn_from_failure( ch, gsn_super_oozaru );
  }

  ch->mana -= skill_table[gsn_super_oozaru]->min_mana;
  return;
}

void do_fakemoon( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_fakemoon ) )
      return;
  }



  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_OOZARU ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_OOZARU );
    ch->pl = ch->exp;
    transStatRemove( ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_GOLDEN_OOZARU ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_GOLDEN_OOZARU );
    ch->pl = ch->exp;
    transStatRemove( ch );
    return;
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You can't use the fakemoon technique while using kaioken.\n\r", ch );
    return;
  }
  if( ch->mana < skill_table[gsn_fakemoon]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_fakemoon]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_fakemoon ) )
  {
    act( z, "You create a fake moon launching it in the air.", ch, NULL, NULL, TO_CHAR );
    act( z, "$n creates a fake moon and launches it in the air.", ch, NULL, NULL, TO_NOTVICT );
    if( !xIS_SET( ch->affected_by, AFF_GOLDEN_OOZARU ) && ch->pcdata->learned[gsn_ssj3] >= 100 && is_saiyan( ch ) )
      transform_golden_oozaru( ch );
    learn_from_success( ch, gsn_fakemoon );

    if( !xIS_SET( ch->affected_by, AFF_OOZARU ) && !wearing_chip( ch ) && !xIS_SET( ch->affected_by, AFF_GOLDEN_OOZARU ) )
      transform_oozaru( ch );
    learn_from_success( ch, gsn_fakemoon );


    
   /* else
    * {
    * act( z, "You try to create a fake moon but it disolves.", ch, NULL, NULL, TO_CHAR );
    * act( z, "$n tries to create a fake moon but it disolves.", ch, NULL, NULL, TO_NOTVICT );
    * learn_from_failure( ch, gsn_fakemoon );
    * }
    */ 

    ch->mana -= skill_table[gsn_fakemoon]->min_mana;
    return;
  }
}

void do_clawstrike( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;
  int dam2 = get_curr_str( ch ) / 25;


  if( is_namek( ch ) )
  {
    ch_printf( ch, "You are not allowed.\n\r" );
    return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_clawstrike]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_clawstrike]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_clawstrike]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_clawstrike]->focus;

  WAIT_STATE( ch, skill_table[gsn_clawstrike]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_clawstrike ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 20, 28 );
    dam += dam2;

    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( AT_DGREY, "You dig your claws into $N's chest, rending $S flesh. As you pull your", ch, NULL, victim, TO_CHAR );
    act( AT_DGREY, "hand back, you grin and lick some of the blood on your claws. &W[$t]", ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_DGREY, "$n digs $s claws into your chest, rending your flesh. As $e pulls $s", ch, NULL, victim, TO_VICT );
    act( AT_DGREY, "hand back, $e grins and licks some of the blood on $s claws. &W[$t]", ch, num_punct( dam ), victim, TO_VICT );
    act( AT_DGREY, "$n digs $s claws into your chest, rending $S flesh. As $e pulls $s", ch, NULL, victim, TO_NOTVICT );
    act( AT_DGREY, "hand back, $e grins and licks some of the blood on $s claws. &W[$t]", ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_clawstrike );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
    ch->melee = FALSE;
  }
  else
  {
    act( AT_DGREY, "You missed $N with your claw attack", ch, NULL, victim, TO_CHAR );
    act( AT_DGREY, "$n misses you with $s claw attack.", ch, NULL, victim, TO_VICT );
    act( AT_DGREY, "$n missed $N with a claw attack", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_clawstrike );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
    ch->melee = FALSE;
  }
  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_clawstrike]->min_mana;
  return;
}

void do_daichiretsuzan( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_daichiretsuzan ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_daichiretsuzan]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_daichiretsuzan]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_daichiretsuzan]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_daichiretsuzan]->focus;

  WAIT_STATE( ch, skill_table[gsn_daichiretsuzan]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_daichiretsuzan ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 32, 36 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( AT_WHITE,
         "You extend your arm ahead and position two fingers in an upward stance while facing $N. Building up enough energy, a small white beam forms at your fingertips and disperses quickly out of sight.  The once minute ray abruptly appears once again and streaks a path toward $N while splitting the land along the way.'",
         ch, NULL, victim, TO_CHAR );
    act( AT_WHITE,
         "As the wall of energy reaches its destination, a horrific explosion emerges and leaves $N rigorously lacerated!!! &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_WHITE,
         "$n extend $s arm ahead and position two fingers in an upward stance while facing you. Building up enough energy, a small white beam forms at $n fingertips and disperses quickly out of sight.  The once minute ray abruptly appears once again and streaks a path toward you while splitting the land along the way!!!!'",
         ch, NULL, victim, TO_VICT );
    act( AT_WHITE,
         "As the wall of energy reaches its destination, a horrific explosion emerges and leaves you rigorously lacerated!!! &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_WHITE,
         "$n extend $s arm ahead and position two fingers in an upward stance while facing $N. Building up enough energy, a small white beam forms at $n fingertips and disperses quickly out of sight.  The once minute ray abruptly appears once again and streaks a path toward $N while splitting the land along the way!!!'",
         ch, NULL, victim, TO_NOTVICT );
    act( AT_WHITE,
         "As the wall of energy reaches its destination, a horrific explosion emerges and leaves $N rigorously lacerated!!!  &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_daichiretsuzan );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_LBLUE, "You missed $N with your Daichiretsuzan.", ch, NULL, victim, TO_CHAR );
    act( AT_LBLUE, "$n misses you with $s Daichiretsuzan.", ch, NULL, victim, TO_VICT );
    act( AT_LBLUE, "$n missed $N with a Daichiretsuzan.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_daichiretsuzan );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_daichiretsuzan]->min_mana;
  return;
}

void do_honoo( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_honoo ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_honoo]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_honoo]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_honoo]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_honoo]->focus;

  WAIT_STATE( ch, skill_table[gsn_honoo]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_honoo ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 35, 40 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( AT_RED, "Confident of your capabilities, you turn your back towards $N with a", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "scowling appearance plastered on your face as you predict $S demise.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "Concentrating only long enough to hone in on the precise spot you wish to", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "attack, you quickly pivot around to face $M and stretch your mouth extensively.", ch, NULL, victim,
         TO_CHAR );
    act( AT_RED, "Without delay a prodigious amount of fire streams from your jaws in an unfocused", ch, NULL, victim,
         TO_CHAR );
    act( AT_RED, "pattern to scorch the entire surroundings. The blazing inferno that once erupted", ch, NULL, victim,
         TO_CHAR );
    act( AT_RED, "from your mouth quickly subsides and leaves $N critically wounded.", ch, num_punct( dam ), victim,
         TO_CHAR );

    act( AT_RED, "Confident of $s capabilities, $n turns $s back towards you with a", ch, NULL, victim, TO_VICT );
    act( AT_RED, "scowling appearance plastered on $s face as $e predicts your demise.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "Concentrating only long enough to hone in on the precise spot $e wishes to", ch, NULL, victim, TO_VICT );
    act( AT_RED, "attack, $n quickly pivots around to face you and stretches $s mouth", ch, NULL, victim, TO_VICT );
    act( AT_RED, "extensively. Without delay a prodigious amount of fire streams from $s jaws in an", ch, NULL, victim,
         TO_VICT );
    act( AT_RED, "unfocused pattern to scorch the entire surroundings. The blazing inferno that once", ch, NULL, victim,
         TO_VICT );
    act( AT_RED, "erupted from $s mouth quickly subsides and leaves you critically wounded.", ch, num_punct( dam ), victim,
         TO_VICT );

    act( AT_RED, "Confident of $s capabilities, $n turns $s back towards $N with a", ch, NULL, victim, TO_NOTVICT );
    act( AT_RED, "scowling appearance plastered on $s face as $e predicts $N's demise.", ch, NULL, victim, TO_NOTVICT );
    act( AT_RED, "Concentrating only long enough to hone in on the precise spot $e wishes to", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_RED, "attack, $n quickly pivots around to face $N and stretches $s mouth", ch, NULL, victim, TO_NOTVICT );
    act( AT_RED, "extensively. Without delay a prodigious amount of fire streams from $s jaws in an", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_RED, "unfocused pattern to scorch the entire surroundings. The blazing inferno that once", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_RED, "erupted from $s mouth quickly subsides and leaves $N critically wounded.", ch, num_punct( dam ), victim,
         TO_NOTVICT );



    learn_from_success( ch, gsn_honoo );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_RED, "You missed $N with your honoo.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n misses you with $s honoo.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n missed $N with a honoo.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_honoo );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_honoo]->min_mana;
  return;
}

void do_buuball( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_buuball ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_buuball]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_buuball]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_buuball]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_buuball]->focus;

  WAIT_STATE( ch, skill_table[gsn_buuball]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_buuball ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 40, 45 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( AT_PURPLE, "You fly up into the air and mold yourself into a ball and dash at $N.", ch, NULL, victim, TO_CHAR );
    act( AT_PURPLE, "You hit them up into the air and comeback with a few more hits till $N falls", ch, NULL, victim,
         TO_CHAR );
    act( AT_PURPLE, "to their demise.", ch, num_punct( dam ), victim, TO_CHAR );

    act( AT_PURPLE, "$N flies up into the air and molds themself into a ball and dashes at you.", ch, NULL, victim,
         TO_VICT );
    act( AT_PURPLE, "$N hits you up into the air and comes backs with a few more hits till you fall", ch, NULL, victim,
         TO_VICT );
    act( AT_PURPLE, "to your demise.", ch, num_punct( dam ), victim, TO_VICT );


    act( AT_PURPLE, "$N flies up into the air and molds themself into a ball and dashes at $S.", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_PURPLE, "$N hits them up into the air and comes back with a few more hits till $S falls", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_PURPLE, "to their demise.", ch, num_punct( dam ), victim, TO_NOTVICT );


    learn_from_success( ch, gsn_buuball );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_RED, "You missed $N with your buu ball.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n misses you with $s buu ball.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n missed $N with a buu ball.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_buuball );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_buuball]->min_mana;
  return;
}

void do_mouth_blast(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_mouth_blast))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_mouth_blast]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_mouth_blast]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_mouth_blast]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_mouth_blast]->focus;

    WAIT_STATE( ch, skill_table[gsn_mouth_blast]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_mouth_blast ) )
    {
	switch (number_range(1,100))// 1,100
	{
    case 100:
    case 95:
 	if (IS_NPC(victim) || (!IS_NPC(victim) && (!xIS_SET(ch->act, PLR_SPAR) || !xIS_SET(ch->act, PLR_SPAR)) ))
 	{

		/* Redone so it can't be used to kill players
		 * above 5x their pl. Was going to do 2x like
		 * DDD, but if I did that, it'd remove the challenge
		 * from Coolers. -- Islvin
		 */
		if (victim->pl / ch->pl >= 2)
		{
			act( AT_RED, "You open your mouth and blast a huge ball of ki at $n.", ch, NULL, victim, TO_CHAR );
			act( AT_RED, "$N is slammed with your mouth blast, but seems unaffected.", ch, NULL, victim, TO_CHAR );
			act( AT_RED, "$n opens $s mouth and blasts a huge ball of ki at $n.", ch, NULL, victim, TO_VICT );
			act( AT_RED, "You are blasted with $n's mouth blast, but feel unaffected..", ch, NULL, victim, TO_VICT );
			act( AT_RED, "$n opens $s mouth and blasts a huge ball of ki at $N", ch, NULL, victim, TO_NOTVICT );
			act( AT_RED, "$N is blasted by $n's mouth blast, but seems unaffected", ch, NULL, victim, TO_NOTVICT );
			dam = 0;
			break;
		}

/* Redo damage so it _is_ instant death
 * Melora
 * 	if (victim->max_hit >= victim->hit)
 * 		dam = victim->max_hit * 2;
 * 	else
 * 		dam = victim->hit * 2;
 */
		dam = 999999999;
	act( AT_RED, "You open your mouth and charge a large red ball of ki, blasting it furiously at $N.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$N is BLASTED by your mouth blast, sending him straight to $S impending doom!", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n opens $s mouth and charges a large red ball of ki, then firing it directly at you!", ch, NULL, victim, TO_VICT );
	act( AT_RED, "You are BLASTED with $n's mouth blast, sending you straight to your impending doom!", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$n copens $s mouth and charges a large red ball of ki, firing it directly at $N!", ch, NULL, victim, TO_NOTVICT );
	act( AT_RED, "$N is BLASTED by $n's mouth blast, sending him straight to $S impending doom!", ch, NULL, victim, TO_NOTVICT );

	learn_from_success( ch, gsn_mouth_blast );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
	}
	default:
	dam = get_attmod(ch, victim) * number_range( 85, 100 );// 100,200
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_RED, "You raise your head to the skies, a large ball of ki forming around your mouth. You clench your fists tightly and focus your ki energy into this ball, it's power glowing with a red aura. You suddenly jerk your head forward, sending the firey ball of ki blasting into $N &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_RED, "$n raises $s head to the skies, a large ball of ki forming around $s mouth. $n clenches $s fists tightly and focuses $s ki energy into this ball, it's power glowing with a red aura. $n suddenly jerks $s head forward, sending the firey ball of ki blasting into you. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_RED, "$n raises $s head to the skies, a large ball of ki forming around $s mouth. $n clenches $s fists tightly and focuses $s ki energy into this ball, it's power glowing with a red aura. $n suddenly jerks $s head forward, sending the firey ball of ki blasting into $N. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

	learn_from_success( ch, gsn_mouth_blast );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
   	}
    }
    else
    {
	act( AT_RED, "You missed $N with your mouth blast.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n misses you with $s mouth blast.", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$n missed $N with a mouth blast.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_mouth_blast );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_mouth_blast]->min_mana;
    return;
}

void do_chou_kamehameha(CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
    int dam = 0;

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_chou_kamehameha]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_chou_kamehameha]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_chou_kamehameha]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_chou_kamehameha]->focus;

    WAIT_STATE( ch, skill_table[gsn_chou_kamehameha]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_chou_kamehameha ) )
    {
	dam = get_attmod(ch, victim) * number_range( 78, 120 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);

	act( AT_SKILL, "&CYou pull your hands back into a ball, a small ball of blue energy forming inside your cupped hands. The ball begins to grow larger and larger, forming into one massive ball of blue ki.", ch, NULL, victim, TO_CHAR );
	act( AT_SKILL, "&CYou scream loudly, &BKA-ME-HA-ME.....HA&C and release the massive chou kamehameha energy towards $N. As your energy blasts into $N, you send $M flying backwards.&W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_SKILL, "&C$n pulls $s hands back into a ball, a small ball of blue energy forming inside $s cupped hands. The ball begins to grow larger and larger, forming into one massive ball of blue ki.", ch, NULL, victim, TO_VICT );
	act( AT_SKILL, "&C$n screams loudly, &BKA-ME-HA-ME.....HA&C and releases the chou kamehameha energy towards you. As the energy blasts into you, it sends you flying backwards. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_SKILL, "&C$n pulls $s hands back into a ball, a small ball of blue energy forming inside $s cupped hands. The ball begins to grow larger and larger, forming into one massive ball of blue ki.", ch, NULL, victim, TO_NOTVICT );
	act( AT_SKILL, "&C$n screams loudly, &BKA-ME-HA-ME.....HA&C and releases the chou kamehameha energy towards $N. As the energy blasts into $N, it sends $M flying backwards. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

	learn_from_success( ch, gsn_chou_kamehameha );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_SKILL, "You missed $N with your chou kamehameha attack.", ch, NULL, victim, TO_CHAR );
	act( AT_SKILL, "$n misses you with $s chou kamehameha attack.", ch, NULL, victim, TO_VICT );
	act( AT_SKILL, "$n missed $N with a chou kamehameha attack.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_chou_kamehameha );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_chou_kamehameha]->min_mana;
	return;
}

void do_mouth_cannon(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_mouth_cannon))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_mouth_cannon]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_mouth_cannon]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_mouth_cannon]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_mouth_cannon]->focus;

    sh_int z = get_aura(ch);

    WAIT_STATE( ch, skill_table[gsn_mouth_cannon]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_mouth_cannon ) )
    {
	dam = get_attmod(ch, victim) * number_range( 20, 25 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( z, "You open your mouth and shoot a beam of energy towards $N.  &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( z, "$n opens his mouth wide and shoots a beam of energy towards you. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( z, "$n opens his mouth as he shoots a beam of energy towards $N. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

        dam = ki_absorb( victim, ch, dam, gsn_mouth_cannon );
	learn_from_success( ch, gsn_mouth_cannon );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( z, "You missed $N with your mouth cannon.", ch, NULL, victim, TO_CHAR );
	act( z, "$n misses you with $s mouth cannon.", ch, NULL, victim, TO_VICT );
	act( z, "$n missed $N with a mouth cannon.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_mouth_cannon );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }

    if( !is_android_h(ch) )
	ch->mana -= skill_table[gsn_mouth_cannon]->min_mana;
    return;
}

void do_teleport_kamehameha(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_teleport_kamehameha ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_teleport_kamehameha]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_teleport_kamehameha]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_teleport_kamehameha]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_teleport_kamehameha]->focus;

    WAIT_STATE( ch, skill_table[gsn_teleport_kamehameha]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_teleport_kamehameha ) )
    {
	dam = get_attmod(ch, victim) * number_range( 50, 65 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_LBLUE, "&CYou put your arms back and cup your hands. '&RKA-ME-HA-ME....&C' ", ch, NULL, victim, TO_CHAR );
	act( AT_LBLUE, "&CYou disappear in a blinding flash of light, teleporting behind $N. ", ch, NULL, victim, TO_CHAR );
	act( AT_LBLUE, "&C'&RHAA!!!!!!!&C' At that second, your kamehameha blast fires from your hands, blasting into $N's back!. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_LBLUE, "$n puts $s arms back and cups $s hands. '&RKA-ME-HA-ME....&C'", ch, NULL, victim, TO_VICT );
	act( AT_LBLUE, "$n disappears in a blinding flash of light. Before you notice it, $n is behind you!", ch, NULL, victim, TO_VICT );
	act( AT_LBLUE, "&C'&RHAAA!!!!!!&C' At that second, $n's kamehameha blasts into your back, sending you crashing forwards. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_LBLUE, "&C$n puts $s arms back and cups $s hands. '&CKA-ME-HA-ME....&C'", ch, NULL, victim, TO_NOTVICT );
	act( AT_LBLUE, "&C$n disappears in a blinding flash of light, appearing behind $N's back!", ch, NULL, victim, TO_NOTVICT );
	act( AT_LBLUE, "&C'&RHAA!!!!!!&C' At that second, $n's kamehameha blasts into $N's back, sending him crashing forward! &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

        dam = ki_absorb( victim, ch, dam, gsn_teleport_kamehameha );
	learn_from_success( ch, gsn_teleport_kamehameha );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_LBLUE, "You missed $N with your teleport kamehameha.", ch, NULL, victim, TO_CHAR );
	act( AT_LBLUE, "$n misses you with $s teleport kamehameha.", ch, NULL, victim, TO_VICT );
	act( AT_LBLUE, "$n missed $N with a teleport kamehameha.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_teleport_kamehameha );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_teleport_kamehameha]->min_mana;
    return;
}


bool can_give_light_effect(CHAR_DATA *ch) {

  if( xIS_SET( ch->affected_by, AFF_SSJ ) ||  
      xIS_SET( ch->affected_by, AFF_SSJ2) ||  
      xIS_SET( ch->affected_by, AFF_SSJ3 ) ||  
      xIS_SET( ch->affected_by, AFF_SSJ4 ) || 
      xIS_SET( ch->affected_by, AFF_KAIOKEN ) ||
      xIS_SET( ch->affected_by, AFF_LSSJ ) ||
      xIS_SET( ch->affected_by, AFF_ULTRAPERFECT )
      ) 
    return 1;
  return 0;
 
}

void do_earth_line_blast(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_earth_line_blast))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_earth_line_blast]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_earth_line_blast]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_earth_line_blast]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_earth_line_blast]->focus;

    WAIT_STATE( ch, skill_table[gsn_earth_line_blast]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_earth_line_blast ) )
    {
	switch (number_range(1,100))
	{
    case 100:
    case 99:
 	if (IS_NPC(victim) || (!IS_NPC(victim) && (!xIS_SET(ch->act, PLR_SPAR) || !xIS_SET(ch->act, PLR_SPAR)) ))
 	{

		/* Redone so it can't be used to kill players
		 * above 5x their pl. Was going to do 2x like
		 * DDD, but if I did that, it'd remove the challenge
		 * from Coolers. -- Islvin
		 */
		if (victim->pl / ch->pl >= 2)
		{
			act( AT_RED, "You charge a huge ball of energy on the tip of your finger and then direct it quickly towards $n.", ch, NULL, victim, TO_CHAR );
			act( AT_RED, "$n is enveloped by the death ball, but when all is done, seems unaffected.", ch, NULL, victim, TO_CHAR );
			act( AT_RED, "$n charges a huge ball of energy on the tip of $s finger and then directs it quickly towards you.", ch, NULL, victim, TO_VICT );
			act( AT_RED, "You are enveloped by the death ball, but seem unaffected when it's over.", ch, NULL, victim, TO_VICT );
			act( AT_RED, "$n charges a huge ball of energy on the tip of $s finger and then directs it quickly towards $N.", ch, NULL, victim, TO_NOTVICT );
			act( AT_RED, "$N is enveloped by the death ball, but when all is done, seems unaffected.", ch, NULL, victim, TO_NOTVICT );
			dam = 0;
			break;
		}

/* Redo damage so it _is_ instant death
 * Melora
 * 	if (victim->max_hit >= victim->hit)
 * 		dam = victim->max_hit * 2;
 * 	else
 * 		dam = victim->hit * 2;
 */
		dam = 999999999;
	act( AT_RED, "You put your right arm across your chest with two fingers pointing outwards. Your fingers begin to glow as you suddenly swing your arm forward, releasing a very long blade-like beam of energy at $N in a blinding flash of light.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "THE BLADE-LIKE BEAM CUTS STRAIGHT THROUGH $N!!!  Killing $S instantly.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n puts $e right arm across $s chest with two fingers pointing outwards. $n's fingers begin to glow as $e swings $s arm forward, releasing a very long blade-like beam of energy at you in a blinding flash of light.", ch, NULL, victim, TO_VICT );
	act( AT_RED, "THE BLADE-LIKE BEAM CUTS STRAIGHT THROUGH YOU!!! Killing you instantly.", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$n puts $e right arm across $s chest with two fingers pointing outwards. $n's fingers begin to glow as $e swings $s arm forward, releasing a very long blade-like beam of energy at $N in a blinding flash of light.", ch, NULL, victim, TO_NOTVICT );
	act( AT_RED, "THE BLADE-LIKE BEAM CUTS STRAIGHT THROUGH $N!!! Killing $S instantly.", ch, NULL, victim, TO_NOTVICT );

	learn_from_success( ch, gsn_earth_line_blast );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
	}
	default:
	dam = get_attmod(ch, victim) * number_range( 80, 90 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_PURPLE, "You put your right arm across your chest with two fingers pointing outwards. Your fingers begin to glow as you suddenly swing your arm forward, releasing a very long blade-like energy beam at $N in a blinding flash of light. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_PURPLE, "$n puts $s right arm across $s chest with two fingers pointing outwards. $n's fingers begin to glow as $e swings $s arm forward, releasing a very long blade-like beam of energy at you in a blinding flash of light. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_PURPLE, "$n puts $s right arm across $s chest with two fingers pointing outwards. $n's fingers begin to glow as $e swings $s arm forward, releasing a very long blade-like bead of energy at $N in a blinding flash of light. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

	learn_from_success( ch, gsn_earth_line_blast );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
   	}
    }
    else
    {
	act( AT_PURPLE, "You missed $N with your earth line blast.", ch, NULL, victim, TO_CHAR );
	act( AT_PURPLE, "$n misses you with $s earth line blast.", ch, NULL, victim, TO_VICT );
	act( AT_PURPLE, "$n missed $N with a earth line blast.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_earth_line_blast );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_earth_line_blast]->min_mana;
    return;
}

void do_negative_spirit_bomb( CHAR_DATA * ch, char *arg )
{
  CHAR_DATA *victim;
  int dam = 0;
  char size[MAX_STRING_LENGTH];
  char facialExpression[MAX_STRING_LENGTH];
  char engeryColor[MAX_STRING_LENGTH];
  char failVerb[MAX_STRING_LENGTH];

  size[0] = '\0';
  facialExpression[0] = '\0';
  engeryColor[0] = '\0';
  failVerb[0] = '\0';

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_negative_spirit_bomb ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_negative_spirit_bomb]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_negative_spirit_bomb]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_negative_spirit_bomb]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_negative_spirit_bomb]->focus;

  WAIT_STATE( ch, skill_table[gsn_negative_spirit_bomb]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_negative_spirit_bomb ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 35, 160 );

    if( dam <= 15 )
      strcpy( size, "tiny" );
    else if( dam <= 25 )
      strcpy( size, "small" );
    else if( dam <= 35 )
      strcpy( size, "little" );
    else if( dam <= 40 )
      strcpy( size, "large" );
    else if( dam <= 45 )
      strcpy( size, "big" );
    else if( dam <= 50 )
      strcpy( size, "huge" );
    else if( dam <= 55 )
      strcpy( size, "great" );
    else if( dam <= 60 )
      strcpy( size, "enormous" );
    else if( dam <= 65 )
      strcpy( size, "massive" );
    else if( dam <= 70 )
      strcpy( size, "gigantic" );
    else if( dam <= 75 )
      strcpy( size, "immense" );
    else if( dam <= 80 )
      strcpy( size, "monstrous" );
    else if( dam <= 85 )
      strcpy( size, "mammoth" );
    else if( dam <= 90 )
      strcpy( size, "vast" );
    else if( dam <= 95 )
      strcpy( size, "gargantuan" );
    else
      strcpy( size, "colossal" );

    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    if( IS_GOOD( ch ) )
    {
      strcpy( facialExpression, "grin" );
      strcpy( engeryColor, "purpleish" );
    }
    else if( IS_EVIL( ch ) )
    {
      strcpy( facialExpression, "smirk" );
      strcpy( engeryColor, "blackish" );
    }
    else
    {
      strcpy( facialExpression, "grimace" );
      strcpy( engeryColor, "reddish" );
    }

    act( AT_SKILL, "You raise your hands above your head, motes of $t energy beginning to form above your palms.", ch,
         engeryColor, victim, TO_CHAR );
    act( AT_SKILL, "$n raises $s hands above $s head, motes of $t energy beginning to form above $n's palms.", ch,
         engeryColor, victim, TO_VICT );
    act( AT_SKILL, "$n raises $s hands above $s head, motes of $t energy beginning to form above $n's palms.", ch,
         engeryColor, victim, TO_NOTVICT );
    if( IS_GOOD( ch ) )
    {
      do_say( ch, "&RThe stars, the powerful, all dying things...lend me your power!" );
    }
    else if( IS_EVIL( ch ) )
    {
      do_say( ch, "&RThe stars, the storm, crash of death....I demand your power!" );
    }
    else
    {
      do_say( ch, "&RThe stars, the planets, the chill of death...I need your power!" );
    }

    act2( AT_DGREY,
          "A $t ball of swirling $T energy forms above your upturned palms, growing slowly larger as you draw in more energy from the cosmos.",
          ch, size, victim, TO_CHAR, engeryColor );
    act( AT_DGREY, "You $t as you toss the brilliant sphere of pure hellish energy towards $N.", ch, facialExpression, victim, TO_CHAR );
    act( AT_DGREY, "$N is slowly engulfed by the negative spirit bomb, ripped apart by the gathered energy. &W[$t]", ch,
         num_punct( dam ), victim, TO_CHAR );

    act2( AT_DGREY,
          "A $t ball of swirling $T energy forms above $n's upturned palms, growing slowly larger as $e draws in more energy from the cosmos.",
          ch, size, victim, TO_VICT, engeryColor );
    act( AT_DGREY, "$n $ts as $e tosses the brilliant sphere of pure hellish energy towards you.", ch, facialExpression, victim,
         TO_VICT );
    act( AT_DGREY, "You are slowly engulfed by the negative spirit bomb, ripped apart by the gathered energy. &W[$t]", ch,
         num_punct( dam ), victim, TO_VICT );

    act2( AT_DGREY,
          "A $t ball of swirling $T energy forms above $n's upturned palms, growing slowly larger as $e draws in more energy from the cosmos.",
          ch, size, victim, TO_NOTVICT, engeryColor );
    act( AT_DGREY, "$n $ts as $e tosses the brilliant sphere of pure hellish energy towards $N.", ch, facialExpression, victim,
         TO_NOTVICT );
    act( AT_DGREY, "$N is slowly engulfed by the negative spirit bomb, ripped apart by the gathered energy. &W[$t]", ch,
         num_punct( dam ), victim, TO_NOTVICT );

    /*
     * dam = 0;
     */
    /*
     * dam = ki_absorb( victim, ch, dam, gsn_negative_spirit_bomb );
     */
    learn_from_success( ch, gsn_spirit_bomb );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    if( IS_GOOD( ch ) )
    {
      strcpy( failVerb, "cry for help" );
    }
    else if( IS_EVIL( ch ) )
    {
      strcpy( failVerb, "damand for power" );
    }
    else
    {
      strcpy( failVerb, "plea for assistance" );
    }


    act( AT_SKILL, "Only a few dots of energy gather before quickly dispersing, your $t going unanswered.", ch, failVerb,
         victim, TO_CHAR );
    act( AT_SKILL, "Only a few dots of energy gather before quickly dispersing, $n's $t going unanswered.", ch, failVerb,
         victim, TO_VICT );
    act( AT_SKILL, "Only a few dots of energy gather before quickly dispersing, $n's $t going unanswered.", ch, failVerb,
         victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_negative_spirit_bomb );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_negative_spirit_bomb]->min_mana;
  return;
}

void do_sharingan( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_INPUT_LENGTH];

	/*return;*/
	do_cleanse(ch, argument);
	return;
	sprintf( buf, "Your current Sharingan Feats:\n\r %s %s %s %s %s\n\r", 
		skill_table[ch->pcdata->sharingan_learned[0]]->name, skill_table[ch->pcdata->sharingan_learned[1]]->name, skill_table[ch->pcdata->sharingan_learned[2]]->name, skill_table[ch->pcdata->sharingan_learned[3]]->name, skill_table[ch->pcdata->sharingan_learned[4]]->name );
	send_to_char( buf, ch );

	send_to_char( "Don't like your Feats? You can 'cleanse' the abilities!\n\r", ch );
	return;
}

void do_cleanse( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_INPUT_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char *s1;
	char *s2;
	char *s3;
	char *s4;
	char *s5;

	argument = one_argument(argument, arg);

	s1 = skill_table[ch->pcdata->sharingan_learned[0]]->name;
	s2 = skill_table[ch->pcdata->sharingan_learned[1]]->name;
	s3 = skill_table[ch->pcdata->sharingan_learned[2]]->name;
	s4 = skill_table[ch->pcdata->sharingan_learned[3]]->name;
	s5 = skill_table[ch->pcdata->sharingan_learned[4]]->name;

	if ( arg[0] == '\0' )
	{	/*
		sprintf( buf, "Your current Sharingan Feats:\n\r %s\n\r %s\n\r %s\n\r %s\n\r %s\n\r", skill_table[ch->pcdata->sharingan_learned[0]]->name, skill_table[ch->pcdata->sharingan_learned[1]]->name, skill_table[ch->pcdata->sharingan_learned[1]]->name, skill_table[ch->pcdata->sharingan_learned[1]]->name, skill_table[ch->pcdata->sharingan_learned[1]]->name );
		send_to_char( buf, ch );
		*/
		sprintf( buf, "Your current Sharingan Feats:\n\r %s %s %s %s %s\n\r", s1,s2,s3,s4,s5 );
		send_to_char( buf, ch );
		/*
		sprintf( buf, "[(*** Link Debug ***)] -- User %s currently has %s %s %s %s %s as his Ability Slots\n\r",ch->name, s1,s2,s3,s4,s5 );
		log_string( buf );
		*/
		return;
	}
	if ( is_number(arg) )
	{
		if ( atoi(arg) < 0 || atoi(arg) > 5 )
		{
			sprintf( buf, "Your current Sharingan Feats:\n\r %s %s %s %s %s\n\r", skill_table[ch->pcdata->sharingan_learned[0]]->name, skill_table[ch->pcdata->sharingan_learned[1]]->name, skill_table[ch->pcdata->sharingan_learned[1]]->name, skill_table[ch->pcdata->sharingan_learned[1]]->name, skill_table[ch->pcdata->sharingan_learned[1]]->name );
			send_to_char( buf, ch );
			return;
		}
		sprintf( buf, "You have cleansed yourself of %s.\n\r", skill_table[ch->pcdata->sharingan_learned[atoi(arg)]]->name );
		send_to_char( buf, ch );
		ch->pcdata->learned[ch->pcdata->sharingan_learned[atoi(arg)]] = 0;
		ch->pcdata->sharingan_learned[atoi(arg)] = 0;
	}
	return;
}

/************************
***   For Sharingan   ***
************************/
void sharingan_check(CHAR_DATA *ch, CHAR_DATA *victim, int ki_sn)
{
	char buf[MAX_STRING_LENGTH];

	if(IS_NPC(ch))
		return;

	if ( can_use_skill(ch, number_percent(), gsn_sharingan ) )
	{
		if ( dice(1,75) > 1 && !IS_NPC(victim) )
		{
			/*
			act( AT_HIT, "You block $N's attack with your hand.", ch, NULL, victim, TO_CHAR );
			act( AT_HIT, "$n holds his hand up and $N's attack is shot into his body!", ch, NULL, victim, TO_ROOM );
			*/
			if ( ch->pcdata->sharingan_learned[0] != 0
				&& ch->pcdata->sharingan_learned[1] != 0
				&& ch->pcdata->sharingan_learned[2] != 0
				&& ch->pcdata->sharingan_learned[3] != 0
				&& ch->pcdata->sharingan_learned[4] != 0 )
			{
				send_to_char( "You must 'forget' a skill before you can Steal another Skill!\n\r", ch );
				return;
			}
			if ( ch->pcdata->sharingan_learned[0] == ki_sn
				|| ch->pcdata->sharingan_learned[1] == ki_sn
				|| ch->pcdata->sharingan_learned[2] == ki_sn
				|| ch->pcdata->sharingan_learned[3] == ki_sn
				|| ch->pcdata->sharingan_learned[4] == ki_sn
				|| ch->pcdata->learned[ki_sn] > 0)
			{
				sprintf( buf, "[(*** Link Debug ***)] -- User %s has just tried to gain Skill %d, User already had. Canceling.\n\r", ch->name, ki_sn );
				log_string( buf );
				return;
			}
			if ( ch->pcdata->sharingan_learned[0] == 0 )
			{
				sprintf( buf, "You concentrate hard on the attack, and feel a rush of enlightenment. You can use %s!", skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_CHAR );
				sprintf( buf, "With a twinge of disappointment, you realize that %s has copied your %s..",ch->name, skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_VICT );
				sprintf( buf, "You notice %s smirk. They must have learned how to use %s's last %s!",ch->name,victim->name,skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_NOTVICT );
				ch->pcdata->learned[ki_sn] = 10;
				ch->pcdata->terrian_learned[0] = ki_sn;
				save_char_obj( ch );
			}
			else if ( ch->pcdata->sharingan_learned[1] == 0 && ch->pcdata->sharingan_learned[0] != 0)
			{
				sprintf( buf, "You concentrate hard on the attack, and feel a rush of enlightenment. You can use %s!", skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_CHAR );
				sprintf( buf, "With a twinge of disappointment, you realize that %s has copied your %s..",ch->name, skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_VICT );
				sprintf( buf, "You notice %s smirk. They must have learned how to use %s last %s!",ch->name,victim->name,skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_NOTVICT );
				ch->pcdata->learned[ki_sn] = 10;
				ch->pcdata->terrian_learned[1] = ki_sn;
				save_char_obj( ch );
			}
			else if ( ch->pcdata->sharingan_learned[2] == 0 && ch->pcdata->sharingan_learned[1] != 0 && ch->pcdata->sharingan_learned[0] != 0)
			{
				sprintf( buf, "You concentrate hard on the attack, and feel a rush of enlightenment. You can use %s!", skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_CHAR );
				sprintf( buf, "With a twinge of disappointment, you realize that %s has copied your %s..",ch->name, skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_VICT );
				sprintf( buf, "You notice %s smirk. They must have learned how to use %s last %s!",ch->name,victim->name,skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_NOTVICT );
				ch->pcdata->learned[ki_sn] = 10;
				ch->pcdata->terrian_learned[2] = ki_sn;
				save_char_obj( ch );
			}
			else if ( ch->pcdata->sharingan_learned[3] == 0 && ch->pcdata->sharingan_learned[2] != 0 && ch->pcdata->sharingan_learned[1] != 0 && ch->pcdata->sharingan_learned[0] != 0)
			{
				sprintf( buf, "You concentrate hard on the attack, and feel a rush of enlightenment. You can use %s!", skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_CHAR );
				sprintf( buf, "With a twinge of disappointment, you realize that %s has copied your %s..",ch->name, skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_VICT );
				sprintf( buf, "You notice %s smirk. They must have learned how to use %s last %s!",ch->name,victim->name,skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_NOTVICT );
				ch->pcdata->learned[ki_sn] = 10;
				ch->pcdata->terrian_learned[3] = ki_sn;
				save_char_obj( ch );
			}
			else if ( ch->pcdata->sharingan_learned[4] == 0 && ch->pcdata->sharingan_learned[3] != 0 && ch->pcdata->sharingan_learned[2] != 0 && ch->pcdata->sharingan_learned[1] != 0 && ch->pcdata->sharingan_learned[0] != 0)
			{
				sprintf( buf, "You concentrate hard on the attack, and feel a rush of enlightenment. You can use %s!", skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_CHAR );
				sprintf( buf, "With a twinge of disappointment, you realize that %s has copied your %s..",ch->name, skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_VICT );
				sprintf( buf, "You notice %s smirk. They must have learned how to use %s last %s!",ch->name,victim->name,skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_NOTVICT );
				ch->pcdata->learned[ki_sn] = 10;
				ch->pcdata->sharingan_learned[4] = ki_sn;
				save_char_obj( ch );
			}
			learn_from_success( ch, gsn_sharingan );
		}
		else if ( !can_use_skill(ch, number_percent(), gsn_sharingan ) )
		{
			switch (number_range(1, 3))
			{
			default:

				sprintf( buf, "Though you saw the %s, your sharingan couldn't keep up!", skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, NULL, TO_CHAR );
				sprintf( buf, "Alas, %s couldn't track your movements. Good!", ch->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_VICT );
				sprintf( buf, "%s concentrates hard on copying %s's %s, but seems they couldn't keep track!", ch->name, victim->name, skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, NULL, TO_NOTVICT );
				learn_from_failure( ch, gsn_sharingan );
				break;
			}
		}
	}
}

void do_deathbeam( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_deathbeam ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_deathbeam]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_deathbeam]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_deathbeam]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_deathbeam]->focus;

  WAIT_STATE( ch, skill_table[gsn_deathbeam]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_deathbeam ) )
  {
    switch ( number_range( 1, 100 ) )
    {
      case 100:
      case 99:
      case 98:
      case 97:
      case 96:
      case 95:
      case 94:
        if( IS_NPC( victim ) || ( !IS_NPC( victim ) && ( !xIS_SET( ch->act, PLR_SPAR ) || !xIS_SET( ch->act, PLR_SPAR ) ) ) )
        {

          /*
           * Redone so it can't be used to kill players
           * * above 5x their pl. Was going to do 2x like
           * * DDD, but if I did that, it'd remove the challenge
           * * from Coolers. -- Islvin
           */
          if( victim->pl / ch->pl >= 2 )
          {
            dam = get_attmod( ch, victim ) * number_range( 5, 6 );
        act( AT_RED,
             "&pYou point your index and middle finger at $N as a purple ball of energy begins to form. You grin as the ball turns into a beam of energy coming right at $N!! It strikes $N through the chest, but $N seems unaffected.",
             ch, num_punct( dam ), victim, TO_CHAR );
        act( AT_RED,
             "&p$n points $s index and middle finger at you as a purple ball of energy begins to form. $n grins as the ball turns into a beam of energy coming right at YOU!! It strikes you through the chest, but you seem unaffected.",
             ch, num_punct( dam ), victim, TO_VICT );
        act( AT_RED,
             "&p$n points $s index and middle finger at $N as a purple ball of energy begins to form. $n grins as the ball turns into a beam of energy coming right at $N!! It strikes $N through the chest, but $N seems unaffected.",
             ch, num_punct( dam ), victim, TO_NOTVICT );
            learn_from_success( ch, gsn_deathbeam );
            global_retcode = damage( ch, victim, dam, TYPE_HIT );
            break;
          }

/* Redo damage so it _is_ instant death
 * Melora
 * 	if (victim->max_hit >= victim->hit)
 * 		dam = victim->max_hit * 2;
 * 	else
 * 		dam = victim->hit * 2;
 */
          dam = 999999999;
          act( AT_RED, "You point your index and middle finger at $N as a purple ball of energy begins to form. You grin as the ball turns into a beam of energy coming right at $N!! It strikes $N through the chest, &zKILLING THEM INSTANTLY!!!",
               ch, NULL, victim, TO_CHAR );
          act( AT_RED, "$n points $s index and middle finger at you as a purple ball of energy begins to form. $n grins as the ball turns into a beam of energy coming right at YOU!! It strikes you through the chest, &zKILLING YOU INSTANTLY!!!",
               ch, NULL, victim, TO_VICT );
          act( AT_RED, "$n points $s index and middle finger at $N as a purple ball of energy begins to form. $n grins as the ball turns into a beam of energy coming right at $N!! It strikes $N through the chest, &zKILLING THEM INSTANTLY!!!",
               ch, NULL, victim, TO_NOTVICT );

          learn_from_success( ch, gsn_deathbeam );
          global_retcode = damage( ch, victim, dam, TYPE_HIT );
          break;
        }
      default:
        dam = get_attmod( ch, victim ) * number_range( 80, 85 );
        if( ch->charge > 0 )
          dam = chargeDamMult( ch, dam );
        act( AT_RED,
             "&pYou point your index and middle finger at $N as a purple ball of energy begins to form. You grin as the ball turns into a beam of energy coming right at $N!! It strikes $N through the chest, damaging $M severely. &W[$t]",
             ch, num_punct( dam ), victim, TO_CHAR );
        act( AT_RED,
             "&p$n points $s index and middle finger at you as a purple ball of energy begins to form. $n grins as the ball turns into a beam of energy coming right at YOU!! It strikes you through the chest, damaging you severely. &W[$t]",
             ch, num_punct( dam ), victim, TO_VICT );
        act( AT_RED,
             "&p$n points $s index and middle finger at $N as a purple ball of energy begins to form. $n grins as the ball turns into a beam of energy coming right at $N!! It strikes $N through the chest, damaging $M severely. &W[$t]",
             ch, num_punct( dam ), victim, TO_NOTVICT );

        learn_from_success( ch, gsn_deathbeam );
        global_retcode = damage( ch, victim, dam, TYPE_HIT );
        break;
    }
  }
  else
  {
    act( AT_RED, "You missed $N with your deathbeam.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n misses you with $s deathbeam.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n missed $N with a deathbeam.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_deathbeam );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_deathbeam]->min_mana;
  return;
}

void do_demonic_embrace( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_demonic_embrace ) )
      return;
  }

//if ( !IS_IMMORTAL(ch) ) {send_to_char("pfft. no. :)\n\r", ch); return;}

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  double pl_mult = 0;
  int arg;

  arg = atoi( argument );

  if( xIS_SET( ( ch )->affected_by, AFF_DEMBRACE ) && arg == 0 )
  {
    send_to_char( "You calm down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_DEMBRACE );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }
  else if( !xIS_SET( ( ch )->affected_by, AFF_DEMBRACE ) && arg == 0 )
  {
    ch_printf( ch, "You embrace your demonic powers? Pfft.\n\r" );
    return;
  }

  pl_mult = 38;

  if( arg > pl_mult )
    pl_mult = ( int )pl_mult;
  else if( arg < 1 )
    pl_mult = 1;
  else
    pl_mult = arg;

  if( ch->mana < skill_table[gsn_demonic_embrace]->min_mana * pl_mult )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( arg < 2 || arg > 38 )
  {
    send_to_char( "Syntax: ultra [2-38]\n\r", ch );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_demonic_embrace]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_demonic_embrace ) )
  {
    act( AT_BLOOD,
         "You start to chant as you begin to feel the presence of demons of old. you demand the use of their power, and after much deliberation they acknowledge your worthiness, allowing you to embrace their demonic auras.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_BLOOD,
         "$n starts to chant as the presence of demons of old fill the room. $n demands the use of their power, and after much deliberation they acknowledge $s worthiness, allowing $m to embrace their demonic auras.",
         ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_DEMBRACE );

    ch->pl = ch->exp * pl_mult;

    transStatApply( ch, pl_mult * 3, pl_mult, pl_mult * -3, pl_mult * 2 );

    learn_from_success( ch, gsn_demonic_embrace );
  }
  else
  {
    act( AT_SKILL, "You fail to embrace your demonic powers.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n fails to embrace $s demonic powers.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_demonic_embrace );
  }

  ch->mana -= skill_table[gsn_demonic_embrace]->min_mana * pl_mult;
  return;
}

void do_angelicrapture( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_angelicrapture ) )
      return;
  }

//if ( !IS_IMMORTAL(ch) ) {send_to_char("pfft. no. :)\n\r", ch); return;}

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  double pl_mult = 0;
  int arg;

  arg = atoi( argument );

  if( xIS_SET( ( ch )->affected_by, AFF_ARAPTURE ) && arg == 0 )
  {
    send_to_char( "You calm down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_ARAPTURE );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }
  else if( !xIS_SET( ( ch )->affected_by, AFF_ARAPTURE ) && arg == 0 )
  {
    ch_printf( ch, "You use holy powers? Pfft.\n\r" );
    return;
  }

  pl_mult = 39;

  if( arg > pl_mult )
    pl_mult = ( int )pl_mult;
  else if( arg < 1 )
    pl_mult = 1;
  else
    pl_mult = arg;

  if( ch->mana < skill_table[gsn_angelicrapture]->min_mana * pl_mult )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( arg < 2 || arg > 39 )
  {
    send_to_char( "Syntax: angelic [2-39]\n\r", ch );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_angelicrapture]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_angelicrapture ) )
  {
    act( AT_GREY,
         "&WYou start to chant as a bright light consumes you, after what seems like a lifetime, the light subsides, leaving you surging with holy powers.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_GREY,
         "&W$n starts to chant as a bright light consumes $m, after a few short seconds, the light subsides, leaving $n surging with tremendous power.",
         ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_ARAPTURE );

    ch->pl = ch->exp * pl_mult;

    transStatApply( ch, pl_mult * -3, pl_mult, pl_mult * 3, pl_mult * 2 );

    learn_from_success( ch, gsn_angelicrapture );
  }
  else
  {
    act( AT_SKILL, "You fail to harness $s holy powers.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n fails to harness $s holy powers.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_angelicrapture );
  }

  ch->mana -= skill_table[gsn_angelicrapture]->min_mana * pl_mult;
  return;
}

void do_nephalimstorment( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_nephalimstorment ) )
      return;
  }

//if ( !IS_IMMORTAL(ch) ) {send_to_char("pfft. no. :)\n\r", ch); return;}

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  double pl_mult = 0;
  int arg;

  arg = atoi( argument );

  if( xIS_SET( ( ch )->affected_by, AFF_SERALIGHT ) && arg == 0 )
  {
    send_to_char( "You bind your powers and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_SERALIGHT );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }
  else if( !xIS_SET( ( ch )->affected_by, AFF_SERALIGHT ) && arg == 0 )
  {
    ch_printf( ch, "You call upon Unknown Powers? Pfft.\n\r" );
    return;
  }

  pl_mult = 37;

  if( arg > pl_mult )
    pl_mult = ( int )pl_mult;
  else if( arg < 1 )
    pl_mult = 1;
  else
    pl_mult = arg;

  if( ch->mana < skill_table[gsn_nephalimstorment]->min_mana * pl_mult )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( arg < 2 || arg > 37 )
  {
    send_to_char( "Syntax: nephalims [2-37]\n\r", ch );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_nephalimstorment]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_nephalimstorment ) )
  {
    act( AT_CYAN,
         "You start to chant as everything around you fades into darkness as shackles enveloped by bright blue flames begin to bound you, leaving yourself to be lost in the darkness for all eternity, suddenly, you let loose a tremendous amount of energy, shattering the shackles, unlocking your true power.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_CYAN,
         "$n starts to chant, darkness begins to fill the room. Suddenly shackles enveloped by bright blue flames begin to bound $n, leaving $m to be lost in the darkness for all eternity, suddenly, $n lets loose a tremendous amount of energy, shattering the shackles, unlocking $s true power.",
         ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_SERALIGHT );

    ch->pl = ch->exp * pl_mult;

    transStatApply( ch, pl_mult * 3, pl_mult * -3, pl_mult * 3, pl_mult );

    learn_from_success( ch, gsn_nephalimstorment );
  }
  else
  {
    act( AT_SKILL, "You fail to call upon your powers.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n fails to call upon $s powers.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_nephalimstorment );
  }

  ch->mana -= skill_table[gsn_nephalimstorment]->min_mana * pl_mult;
  return;
}

void do_mechaicer( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_mechaicer ) )
      return;
  }

//if ( !IS_IMMORTAL(ch) ) {send_to_char("pfft. no. :)\n\r", ch); return;}

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to call upon those powers while you know majin.\n\r" );
    return;
  }

  double pl_mult = 0;
  int arg;

  arg = atoi( argument );

  if( xIS_SET( ( ch )->affected_by, AFF_MICER ) && arg == 0 )
  {
    send_to_char( "You calm down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_MICER );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }
  else if( !xIS_SET( ( ch )->affected_by, AFF_MICER ) && arg == 0 )
  {
    ch_printf( ch, "You be mecha? Pfft.\n\r" );
    return;
  }

  pl_mult = 38;

  if( arg > pl_mult )
    pl_mult = ( int )pl_mult;
  else if( arg < 1 )
    pl_mult = 1;
  else
    pl_mult = arg;

  if( ch->mana < skill_table[gsn_mechaicer]->min_mana * pl_mult )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( arg < 2 || arg > 38 )
  {
    send_to_char( "Syntax: mechaicer [2-38]\n\r", ch );
    return;
  }
  WAIT_STATE( ch, skill_table[gsn_mechaicer]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_mechaicer ) )
  {
    act( AT_GREY,
         "&zYour mecha-augmentations begin to hum as they powerup, releasing overwhelming power throughout your body. &PYou have achieved Mecha-Icer Transformation!!&D",
         ch, NULL, NULL, TO_CHAR );
    act( AT_GREY,
         "&z$n's mecha-augmentations begin to hum as they powerup, releasing overwhelming power throughout $s body. &P$n has achieved Mecha-Icer Transformation!!&D",
         ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_MICER );

    ch->pl = ch->exp * pl_mult;

    transStatApply( ch, pl_mult * 2, pl_mult * .5, pl_mult * 3, pl_mult );
    if( xIS_SET( ( ch )->affected_by, AFF_MICER ) )
      ch->pl = ch->pl * 1.1;
    learn_from_success( ch, gsn_mechaicer );
  }
  else
  {
    act( AT_SKILL, "You fail to harness your mecha powers.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n fails to harness $s mecha powers.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_mechaicer );
  }

  ch->mana -= skill_table[gsn_mechaicer]->min_mana * pl_mult;
  return;
}

void do_ddmb( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;
  int count = 0;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_ddmb ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_ddmb]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  count = atoi( argument );

  if( count < 1 || count > 40 )
  {
    ch_printf( ch, "The number of Die-Die Missiles must be between 1 and 40.\n\r" );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_ddmb]->min_mana * count )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_ddmb]->focus * count )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_ddmb]->focus * count;

  WAIT_STATE( ch, skill_table[gsn_ddmb]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_ddmb ) )
  {
    int shots = count;
    int dam2 = 0;

    while( shots > 0 )
    {
      switch ( number_range( 1, 3 ) )
      {
        default:
        case 1:
        case 2:
          dam += number_range( 1, 2 );
          break;
        case 3:
          dam += number_range( 1, 3 );
          break;
      }
      shots--;
    }
    dam2 = number_range( 3, 6 );
    //dam2 = number_range( 4, 8 );
    dam = ( get_attmod( ch, victim ) * dam * dam2 );

    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    strcpy( buf, num_punct( count ) );
    strcpy( buf2, num_punct( dam ) );

    act2( AT_WHITE,
          "&zYou grab $N by the neck and throw $M into the ground with immense force, making a small impact crater, immediately following, you fly up about 20 feet in the air and begin to charge an immense amount of energy, firing $t energy blasts at $N.",
          ch, buf, victim, TO_CHAR, NULL );
    act2( AT_WHITE,
          "&zYou scream out '&RDIE-DIE MISSILE BARRAGE!!&z' as the energy blasts hit $N making a gigantic explosion, dealing extreme damage. &W[$T]",
          ch, buf, victim, TO_CHAR, buf2 );
    act2( AT_WHITE,
          "&z$n grabs you by the neck and throws you into the ground with immense force, making a small impact crater, immediately following, $e flies up about 20 feet in the air and begins to charge an immense amount of energy, firing $t energy blasts at you.",
          ch, buf, victim, TO_VICT, NULL );
    act2( AT_WHITE,
          "&z$n screams out '&RDIE-DIE MISSILE BARRAGE!!&z' as the energy blasts hit you making a gigantic explosion, dealing extreme damage. &W[$T]",
          ch, buf, victim, TO_VICT, buf2 );
    act2( AT_WHITE,
          "&z$n grabs $N by the neck and throws $M into the ground with immense force, making a small impact crater, immediately following, $n flies up about 20 feet in the air and begins to charge an immense amount of energy, firing $t energy blasts at $N.",
          ch, buf, victim, TO_NOTVICT, NULL );
    act2( AT_WHITE,
          "&z$n screams out '&RDIE-DIE MISSILE BARRAGE!!&z' as the energy blasts hit $N making a gigantic explosion, dealing extreme damage. &W[$T]",
          ch, buf, victim, TO_NOTVICT, buf2 );

    dam = ki_absorb( victim, ch, dam, gsn_ddmb );
    learn_from_success( ch, gsn_ddmb );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_WHITE, "You missed $N with your Die-Die Missile Barrage.", ch, NULL, victim, TO_CHAR );
    act( AT_WHITE, "$n misses you with $s Die-Die Missile Barrage.", ch, NULL, victim, TO_VICT );
    act( AT_WHITE, "$n missed $N with a Die-Die Missile Barrage.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_ddmb );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_ddmb]->min_mana * count;
  return;
}

void do_stripmajin( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *vch;

  if( !IS_NPC( ch ) )
    if( ch->level < 63 )
    {
      ch_printf( ch, "Huh?\n\r" );
      return;
    }

  if( ch->position != POS_STANDING )
  {
    ch_printf( ch, "You have to be standing.\n\r" );
    return;
  }
  if( ( vch = get_char_room( ch, argument ) ) == NULL )
  {
    ch_printf( ch, "There's nobody here by that name.\n\r" );
    return;
  }

  if( IS_NPC( vch ) )
  {
    ch_printf( ch, "Not on NPC's.\n\r" );
    return;
  }
  if( vch->exp < 100000000000ULL )
  {
    if( IS_NPC( ch ) )
      act( AT_LBLUE, "Babadi says 'You have to be over 100b to learn majin'", ch, NULL, vch, TO_VICT );
    else
      ch_printf( ch, "They have to be over 100bil to learn majin.\n\r" );
    return;
  }
  if( IS_KAIO( vch ) || is_android( vch ) || is_superandroid( vch ) || is_reploid( vch ) || is_fierian( vch ) || is_hylian( vch ) )
  {
    if( IS_NPC( ch ) )
      act( AT_LBLUE, "Babadi says 'you cannot be taught majin'", ch, NULL, vch, TO_VICT );
    else
      ch_printf( ch, "They cannot be taught majin.\n\r" );
    return;
  }

  if( IS_GOOD( vch ) )
  {
    if( IS_NPC( ch ) )
      act( AT_LBLUE, "Babadi says 'Your heart is too pure for this way of evil'", ch, NULL, vch, TO_VICT );
    else
      ch_printf( ch, "Thier heart is too pure for this way of evil.\n\r" );
    return;
  }

  if( vch->position != POS_STANDING )
  {
    ch_printf( ch, "They have to be standing.\n\r" );
    return;
  }

  act( AT_GREY, "&RYou have been stripped of Majin.. -Kal&D", ch, NULL, vch, TO_CHAR );
  vch->canmajin = FALSE;
  vch->pcdata->learned[gsn_majin] = 0;

  save_char_obj( vch );

  return;
}

void do_bpoint( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_bpoint ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_bpoint]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && !IS_SET( ch->pcdata->flags, PCFLAG_KNOWSLSSJ ) )
  {
    ch_printf( ch, "Only an LSSJ can build the amount of rage required.\n\r" );
    return;
  }

  if( !xIS_SET(ch->affected_by, AFF_LSSJ) )
  {
    send_to_char( "You must be in LSSJ to do this attack.\n\r" ,ch);
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_bpoint]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_bpoint]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_bpoint]->focus;

  WAIT_STATE( ch, skill_table[gsn_bpoint]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_bpoint ) )
  {
    switch ( number_range( 1, 100 ) )
    {
      case 100:
      case 99:
      case 98:
      case 97:
        if( IS_NPC( victim ) || ( !IS_NPC( victim ) && ( !xIS_SET( ch->act, PLR_SPAR ) || !xIS_SET( ch->act, PLR_SPAR ) ) ) )
        {

          /*
           * Redone so that instant death can't be abused to instant kill
           * * players over 2x their power. -- Islvin
           */
          if( victim->pl / ch->pl >= 2 )
          {
            dam = get_attmod( ch, victim ) * number_range( 10, 13 );
            act( AT_RED, "You fail to harness your rage, hitting $N with a mere energy ball. &B*FIZZLE*&D", ch, NULL, victim,
                 TO_CHAR );
            act( AT_RED, "$n fails to harness $s rage, hitting you with a mere energy ball. &B*FIZZLE*&D", ch, NULL, victim,
                 TO_VICT );
            act( AT_RED, "$n fails to harness $s rage, hitting $N with a mere energy ball. &B*FIZZLE*&D", ch, NULL, victim,
                 TO_NOTVICT );
            learn_from_success( ch, gsn_bpoint );
            global_retcode = damage( ch, victim, dam, TYPE_HIT );
            break;
          }

/* Redone so it _is_ instant death -- Melora
 * 	if (victim->max_hit >= victim->hit)
 * 		dam = victim->max_hit * 2;
 * 	else
 * 		dam = victim->hit * 2;
 */
          dam = 999999999;
          act( AT_RED, "&RThe rage deep within you burns like the most intense of fires as your legendary", ch, NULL, victim,
                 TO_CHAR );
          act( AT_RED, "&Rpower begins to fill within each of your hands.  The energy crackles madly and", ch, NULL, victim,
                 TO_CHAR );
          act( AT_RED, "&Rloudly as you channel more and more of your vast power into it.  You look at", ch, NULL, victim,
                 TO_CHAR );
          act( AT_RED, "&R$N as your bleached eyes show hatred and near berserker rage.  You", ch, NULL, victim,
                 TO_CHAR );
          act( AT_RED, "&Rslowly force the crackling spheres of energy together as your aura builds", ch, NULL, victim,
                 TO_CHAR );
          act( AT_RED, "&Rintensity and begins to grow. The aura's golden color quickly dulls and fades", ch, NULL, victim,
                 TO_CHAR );
          act( AT_RED, "&Rto an almost pitch black.", ch, NULL, victim,
                 TO_CHAR );
          act( AT_RED, "/n/r", ch, NULL, victim,
                 TO_CHAR );
          act( AT_DGREY, "&zYou laugh maniacally as you release all of your energy in one malevolent", ch, NULL, victim,
                 TO_CHAR );
          act( AT_DGREY, "&zburst. The damage caused by your blast was so grand, that anything within a 300", ch, NULL, victim,
                 TO_CHAR );
          act( AT_DGREY, "&zyard radius was incinerated.", ch, NULL, victim,
                 TO_CHAR );

          act( AT_RED, "&RThe rage deep within $n burns like intense fire as $s legendary power", ch, NULL, victim,
                 TO_NOTVICT );
          act( AT_RED, "&Rbegins to fill within each of $s hands.  The energy crackles madly and", ch, NULL, victim,
                 TO_NOTVICT );
          act( AT_RED, "&Rloudly as $e channels more and more of $s vast power into it.  $n looks at", ch, NULL, victim,
                 TO_NOTVICT );
          act( AT_RED, "&R$N as $s bleached eyes show hatred and near berserker rage.  $n", ch, NULL, victim,
                 TO_NOTVICT );
          act( AT_RED, "&Rslowly forces the crackling spheres of energy together as $s aura builds", ch, NULL, victim,
                 TO_NOTVICT );
          act( AT_RED, "&Rintensity and begins to grow. The aura's golden color quickly dulls and fades", ch, NULL, victim,
                 TO_NOTVICT );
          act( AT_RED, "&Rto an almost pitch black.", ch, NULL, victim,
                 TO_NOTVICT );
          act( AT_RED, "/n/r", ch, NULL, victim,
                 TO_NOTVICT );
          act( AT_DGREY, "&z$n laughs maniacally as $e releases all of $s energy in one malevolent", ch, NULL, victim,
                 TO_NOTVICT );
          act( AT_DGREY, "&zburst. The damage caused by the blast was so grand, that anything within a 300", ch, NULL, victim,
                 TO_NOTVICT );
          act( AT_DGREY, "&zyard radius was incinerated.", ch, NULL, victim,
                 TO_NOTVICT );

          act( AT_RED, "&RThe rage deep within $n burns like intense fire as $s legendary power", ch, NULL, victim,
                 TO_VICT );
          act( AT_RED, "&Rbegins to fill within each of $s hands.  The energy crackles madly and", ch, NULL, victim,
                 TO_VICT );
          act( AT_RED, "&Rloudly as $e channels more and more of $s vast power into it.  $n looks at", ch, NULL, victim,
                 TO_VICT );
          act( AT_RED, "&Ryou as $s bleached eyes show hatred and near berserker rage.  $n", ch, NULL, victim,
                 TO_VICT );
          act( AT_RED, "&Rslowly forces the crackling spheres of energy together as $s aura builds", ch, NULL, victim,
                 TO_VICT );
          act( AT_RED, "&Rintensity and begins to grow. The aura's golden color quickly dulls and fades", ch, NULL, victim,
                 TO_VICT );
          act( AT_RED, "&Rto an almost pitch black.", ch, NULL, victim,
                 TO_VICT );
          act( AT_RED, "/n/r", ch, NULL, victim,
                 TO_VICT );
          act( AT_DGREY, "&z$n laughs maniacally as $e releases all of $s energy in one malevolent", ch, NULL, victim,
                 TO_VICT );
          act( AT_DGREY, "&zburst. The damage caused by the blast was so grand, that anything within a 300", ch, NULL, victim,
                 TO_VICT );
          act( AT_DGREY, "&zyard radius was incinerated.", ch, NULL, victim,
                 TO_VICT );
          learn_from_success( ch, gsn_bpoint );
          global_retcode = damage( ch, victim, dam, TYPE_HIT );
          break;
        }
      default:
        dam = get_attmod( ch, victim ) * number_range( 115, 125 );
        if( ch->charge > 0 )
          dam = chargeDamMult( ch, dam );
        act( AT_RED, "&RThe rage deep within you burns like the most intense of fires as your legendary", ch, NULL, victim,
               TO_CHAR );
        act( AT_RED, "&Rpower begins to fill within each of your hands.  The energy crackles madly and", ch, NULL, victim,
               TO_CHAR );
        act( AT_RED, "&Rloudly as you channel more and more of your vast power into it.  You look at", ch, NULL, victim,
               TO_CHAR );
        act( AT_RED, "&R$N as your bleached eyes show hatred and near berserker rage.  You", ch, NULL, victim,
               TO_CHAR );
        act( AT_RED, "&Rslowly force the crackling spheres of energy together as your aura builds", ch, NULL, victim,
               TO_CHAR );
        act( AT_RED, "&Rintensity and begins to grow. Letting out a bloodlust-filled roar, you shove", ch, NULL, victim,
               TO_CHAR );
        act( AT_RED, "&Rthe entire sphere of energy into $N's chest, devastating $M from the inside", ch, NULL, victim,
               TO_CHAR );
        act( AT_RED, "&Rout. &W[$t]", ch,
             num_punct( dam ), victim, TO_CHAR );

        act( AT_RED, "&RThe rage deep within $n burns like intense fire as $s legendary power", ch, NULL, victim,
               TO_NOTVICT );
        act( AT_RED, "&Rbegins to fill within each of $s hands.  The energy crackles madly and", ch, NULL, victim,
               TO_NOTVICT );
        act( AT_RED, "&Rloudly as $e channels more and more of $s vast power into it.  $n looks at", ch, NULL, victim,
               TO_NOTVICT );
        act( AT_RED, "&R$N as $s bleached eyes show hatred and near berserker rage.  $n", ch, NULL, victim,
               TO_NOTVICT );
        act( AT_RED, "&Rslowly forces the crackling spheres of energy together as $s aura builds", ch, NULL, victim,
               TO_NOTVICT );
        act( AT_RED, "&Rintensity and begins to grow. Letting out a bloodlust-filled roar, $e shoves", ch, NULL, victim,
               TO_NOTVICT );
        act( AT_RED, "&Rthe entire sphere of energy into $N's chest, devastating $M from the inside", ch, NULL, victim,
               TO_NOTVICT );
        act( AT_RED, "&Rout. &W[$t]", ch,
             num_punct( dam ), victim, TO_NOTVICT );

        act( AT_RED, "&RThe rage deep within $n burns like intense fire as $s legendary power", ch, NULL, victim,
               TO_VICT );
        act( AT_RED, "&Rbegins to fill within each of $s hands.  The energy crackles madly and", ch, NULL, victim,
               TO_VICT );
        act( AT_RED, "&Rloudly as $e channels more and more of $s vast power into it.  $n looks at", ch, NULL, victim,
               TO_VICT );
        act( AT_RED, "&Ryou as $s bleached eyes show hatred and near berserker rage.  $n", ch, NULL, victim,
               TO_VICT );
        act( AT_RED, "&Rslowly forces the crackling spheres of energy together as $s aura builds", ch, NULL, victim,
               TO_VICT );
        act( AT_RED, "&Rintensity and begins to grow. Letting out a bloodlust-filled roar, $e shoves", ch, NULL, victim,
               TO_VICT );
        act( AT_RED, "&Rthe entire sphere of energy into your chest, devastating you from the inside", ch, NULL, victim,
               TO_VICT );
        act( AT_RED, "&Rout. &W[$t]", ch,
             num_punct( dam ), victim, TO_VICT );


        learn_from_success( ch, gsn_bpoint );
        global_retcode = damage( ch, victim, dam, TYPE_HIT );
        break;
    }
  }
  else
  {
    act( AT_RED, "You fail to harness your rage.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n fails to harness $s rage.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n fails to harness $s rage.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_bpoint );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_bpoint]->min_mana;
  return;
}

/*eof*/
