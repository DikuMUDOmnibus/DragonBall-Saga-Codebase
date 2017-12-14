/****************************************************************************
*                                                                           *
*        skills_android.c - skills and abilities unique to androids         *
*                                                                           *
****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

extern void transStatApply args( ( CHAR_DATA * ch, int strMod, int spdMod, int intMod, int conMod ) );
extern void transStatRemove args( ( CHAR_DATA * ch ) );

bool removeDroidTrans( CHAR_DATA * ch )
{
  if( xIS_SET( ch->affected_by, AFF_T4 ) )
  {
    xREMOVE_BIT( ch->affected_by, AFF_T4 );
    transStatRemove( ch );
    return TRUE;
  }
  if( xIS_SET( ch->affected_by, AFF_T3 ) )
  {
    xREMOVE_BIT( ch->affected_by, AFF_T3 );
    transStatRemove( ch );
    return TRUE;
  }
  if( xIS_SET( ch->affected_by, AFF_T2 ) )
  {
    xREMOVE_BIT( ch->affected_by, AFF_T2 );
    transStatRemove( ch );
    return TRUE;
  }
  if( xIS_SET( ch->affected_by, AFF_T1 ) )
  {
    xREMOVE_BIT( ch->affected_by, AFF_T1 );
    transStatRemove( ch );
    return TRUE;
  }
  if( xIS_SET( ch->affected_by, AFF_OMEGA ) )
  {
    xREMOVE_BIT( ch->affected_by, AFF_OMEGA );
    transStatRemove( ch );
    return TRUE;
  }
  if( xIS_SET( ch->affected_by, AFF_SENT_CHIP ) )
  {
    xREMOVE_BIT( ch->affected_by, AFF_SENT_CHIP );
    transStatRemove( ch );
    return TRUE;
  }
  if( xIS_SET( ch->affected_by, AFF_VM1 ) )
  {
    xREMOVE_BIT( ch->affected_by, AFF_VM1 );
    transStatRemove( ch );
    return TRUE;
  }
  if( xIS_SET( ch->affected_by, AFF_VM2 ) )
  {
    xREMOVE_BIT( ch->affected_by, AFF_VM2 );
    transStatRemove( ch );
    return TRUE;
  }
  return FALSE;
}

void do_absorb( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  char charBuf[MAX_STRING_LENGTH];
  char victBuf[MAX_STRING_LENGTH];
  char roomBuf[MAX_STRING_LENGTH];
  int absorbGsn = 0;
  int dam = 0;

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

  if( !IS_NPC( ch ) )
  {
    if( ch->pcdata->learned[gsn_absorb2] > 0 )
    {
      absorbGsn = gsn_absorb2;
    }
    else
    {
      absorbGsn = gsn_absorb;
    }
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[absorbGsn]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( is_android( victim ) )
  {
    ch_printf( ch, "This won't work against other androids.\n\r" );
    return;
  }

  if( ch->focus < skill_table[absorbGsn]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[absorbGsn]->focus;

  WAIT_STATE( ch, skill_table[absorbGsn]->beats );
  if( can_use_skill( ch, number_percent(  ), absorbGsn ) )
  {
    if( absorbGsn == gsn_absorb2 )
      dam = get_attmod( ch, victim ) * number_range( 3, 12 );
    else
      dam = get_attmod( ch, victim ) * number_range( 2, 8 );
    if( dam >= 0 )
    {
      switch ( number_range( 1, 2 ) )
      {
        default:
          sprintf( charBuf, "You grab $N tightly, draining $M of some of $S energy. &W[$t]" );
          sprintf( victBuf, "$n grabs you tightly, draining you of some of your energy. &W[$t]" );
          sprintf( roomBuf, "$n grabs $N tightly, draining $M of some of $S energy. &W[$t]" );
          break;

        case 1:
          sprintf( charBuf, "You grab $N tightly, draining $M of some of $S energy. &W[$t]" );
          sprintf( victBuf, "$n grabs you tightly, draining you of some of your energy. &W[$t]" );
          sprintf( roomBuf, "$n grabs $N tightly, draining $M of some of $S energy. &W[$t]" );
          break;

        case 2:
          sprintf( charBuf, "You reach out and grab $N with one hand, draining $M of some of $S energy. &W[$t]" );
          sprintf( victBuf, "$n reaches out and grabs you with one hand, draining you of some of your energy. &W[$t]" );
          sprintf( roomBuf, "$n reaches out and grabs $N with one hand, draining $M of some of $S energy. &W[$t]" );
          break;
      }
/*	act( AT_SKILL, "You grab $N tightly, draining $M of some of $S energy. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
 *	act( AT_SKILL, "$n grabs you tightly, draining you of some of your energy. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
 *	act( AT_SKILL, "$n grabs $N tightly, draining $M of some of $S energy. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );
 */
      act( AT_RED, charBuf, ch, num_punct( dam ), victim, TO_CHAR );
      act( AT_RED, victBuf, ch, num_punct( dam ), victim, TO_VICT );
      act( AT_RED, roomBuf, ch, num_punct( dam ), victim, TO_NOTVICT );

      learn_from_success( ch, absorbGsn );
      if( absorbGsn == gsn_absorb2 && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
        xSET_BIT( ch->act, PLR_3XPL_GAIN );
      else
        xSET_BIT( ch->act, PLR_2XPL_GAIN );
      ch->melee = TRUE;
      global_retcode = damage( ch, victim, dam, TYPE_HIT );
      ch->melee = FALSE;

      if( xIS_SET( ch->act, PLR_2XPL_GAIN ) )
        xREMOVE_BIT( ch->act, PLR_2XPL_GAIN );
      if( xIS_SET( ch->act, PLR_3XPL_GAIN ) )
        xREMOVE_BIT( ch->act, PLR_3XPL_GAIN );
    }
    if( xIS_SET( ch->act, PLR_2XPL_GAIN ) )
      xREMOVE_BIT( ch->act, PLR_2XPL_GAIN );
    if( xIS_SET( ch->act, PLR_3XPL_GAIN ) )
      xREMOVE_BIT( ch->act, PLR_3XPL_GAIN );
  }
  else
  {
    act( AT_RED, "You missed $N while trying to grab $M.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "You quickly dodge out of the way from $n's open fist.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$N quickly dodges out of the way from $n's open fist.", ch, NULL, victim, TO_NOTVICT );

    if( xIS_SET( ch->act, PLR_2XPL_GAIN ) )
      xREMOVE_BIT( ch->act, PLR_2XPL_GAIN );
    if( xIS_SET( ch->act, PLR_3XPL_GAIN ) )
      xREMOVE_BIT( ch->act, PLR_3XPL_GAIN );

    learn_from_failure( ch, absorbGsn );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
    ch->melee = FALSE;
  }

  if( dam > 0 )
  {
    if( absorbGsn == gsn_absorb2 )
      dam = victim->mana * 0.10;
    else
      dam = victim->mana * 0.05;
    if( dam > 0 )
    {
      if( victim->mana < dam )
      {
        dam = victim->mana;
        victim->mana = 0;
      }
      else
        victim->mana -= dam;

      ch->mana += dam;
    }
    if( ch->mana > ch->max_mana )
      ch->mana = ch->max_mana;
  }

  return;
}

void do_battery( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;
  bool virus = FALSE;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    virus = TRUE;
  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_battery ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_battery]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( !is_android_e( ch ) )
  {
    ch_printf( ch, "Huh?\n\r" );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->battery < 1 )
  {
    ch_printf( ch, "You don't have any stored up ki energy!\n\r" );
    return;
  }

  if( ch->focus < skill_table[gsn_battery]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_battery]->focus;

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_battery]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_battery ) )
  {
    int min_batt = 0;
    int max_batt = 0;
    max_batt = ch->battery;
    if( virus )
      min_batt = ( int )( max_batt * 2 );
    else
      min_batt = ( int )( max_batt * 1.5 );
    dam = get_attmod( ch, victim ) * number_range( min_batt, max_batt );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( z,
         "You thrust your arm forward, and begin to charge up your stored ki energy. All of a sudden, you fire from your palm a roaring blast of energy the size of a large house. The blast wave quickly reaches $N, slamming into $M and exploding. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( z,
         "$n thrusts $s arm forward, and begins to charge up $s stored ki energy. All of a sudden, $n fires from $s palm a roaring blast of energy the size of a large house. The blast wave quickly reaches you, slamming into you and exploding. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( z,
         "$n thrusts $s arm forward, and begins to charge up $s stored ki energy. All of a sudden, $n fires from $s palm a roaring blast of energy the size of a large house. The blast wave quickly reaches $N, slamming into $M and exploding. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_battery );
    learn_from_success( ch, gsn_battery );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( z, "You missed $N with your battery blast.", ch, NULL, victim, TO_CHAR );
    act( z, "$n misses you with $s battery blast.", ch, NULL, victim, TO_VICT );
    act( z, "$n missed $N with a battery blast.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_battery );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->battery = 0;
  return;
}

void do_superandroid( CHAR_DATA * ch, char *argument )
{
  // OBJ_DATA *o;
  int extra = 0;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    extra = 5;
  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_superandroid ) )
      return;
  }

  double pl_mult = 0;

  /*
   * if( (o = get_eq_char( ch, WEAR_SHIELD )) != NULL )
   * {
   * ch_printf(ch,"You can't transform into a Super Android while you have a chip installed!\n\r");
   * return;
   * }
   */
  if( xIS_SET( ( ch )->affected_by, AFF_VM2 ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_VM2 );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_VM1 ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_VM1 );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_OMEGA ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_OMEGA );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_T1 ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_T1 );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_T2 ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_T2 );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_T3 ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_T3 );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_T4 ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_T4 );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_SUPERANDROID ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_SUPERANDROID );
    transStatRemove( ch );
    ch->pl = ch->exp;
    return;
  }

  pl_mult = 40 + extra;

  if( ch->mana < skill_table[gsn_superandroid]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_superandroid]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_superandroid ) )
  {
    act( AT_BLUE,
         "Your systems are suddenly flooded with power as you connect the absorbed android parts to your primary core. Your body bulges greatly with raw power as your very circuitry and framework changes within you, turning you into a Super Android.",
         ch, NULL, NULL, TO_CHAR );
    act( AT_BLUE,
         "$n's systems are suddenly flooded with power as $e connects absorbed android parts to $s primary core. $n's body bulges greatly with raw power as $s very circuitry and framework changes within $m, turning into a Super Android.",
         ch, NULL, NULL, TO_NOTVICT );

    xSET_BIT( ( ch )->affected_by, AFF_SUPERANDROID );

    ch->pl = ch->exp * pl_mult;

    transStatApply( ch, pl_mult * 2, pl_mult * .5, pl_mult * 3, pl_mult * -3 );

    learn_from_success( ch, gsn_superandroid );
  }
  else
  {
    act( AT_BLUE, "You failed to properly interface your absorbed android parts to your primary systems.", ch, NULL, NULL,
         TO_CHAR );
    act( AT_BLUE, "$n failed to properly interface $s absorbed android parts to $s primary systems.", ch, NULL, NULL,
         TO_NOTVICT );
    learn_from_failure( ch, gsn_superandroid );
  }

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_superandroid]->min_mana;
  return;
}

void do_ehb( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_ehb ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_ehb]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_ehb]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_ehb]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_ehb]->focus;

  WAIT_STATE( ch, skill_table[gsn_ehb]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_ehb ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 85, 125 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_DGREY,
         "You bring your palms near each other infront of yourself and begin to charge an incredible amount of energy into a small space. You create an energy ball so dense it almost resembles a small black hole.",
         ch, NULL, victim, TO_CHAR );
    act( AT_DGREY,
         "Violent bolts of red electricity stream forth from the energy ball as the energy cannot be compacted any farther. Shouting 'NOW DIE' at your enemy, you fire the attack. The energy ball impacts with $N and explodes; engulfing $m in a gigantic explosion. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_DGREY,
         "$n brings $s palms near each other infront of $mself and begins to charge an incredible amount of energy into a small space. $n creates an energy ball so dense it almost resembles a small black hole.",
         ch, NULL, victim, TO_VICT );
    act( AT_DGREY,
         "Violent bolts of red electricity stream forth from the energy ball as the energy cannot be compacted any farther. Shouting 'NOW DIE' at you, $n fires the attack. The energy ball impacts with you and explodes; engulfing you in a gigantic explosion. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_DGREY,
         "$n brings $s palms near each other infront of $mself and begins to charge an incredible amount of energy into a small space. $n creates an energy ball so dense it almost resembles a small black hole.",
         ch, NULL, victim, TO_NOTVICT );
    act( AT_DGREY,
         "Violent bolts of red electricity stream forth from the energy ball as the energy cannot be compacted any farther. Shouting 'NOW DIE' at $N, $n fires the attack. The energy ball impacts with $N and explodes; engulfing $m in a gigantic explosion. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    //dam = ki_absorb( victim, ch, dam, gsn_ehb );
    learn_from_success( ch, gsn_ehb );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_DGREY, "You missed $N with your electric hell ball.", ch, NULL, victim, TO_CHAR );
    act( AT_DGREY, "$n misses you with $s electric hell ball.", ch, NULL, victim, TO_VICT );
    act( AT_DGREY, "$n missed $N with a electric hell ball.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_ehb );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_ehb]->min_mana;
  return;
}

void do_androidfuse( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *rch;
  CHAR_DATA *vch;
  CHAR_DATA *dch; /*dominant fuse char */
  CHAR_DATA *sch; /*stasis fuse char */
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  int sn;
  int value;

  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg2 );

  if( ch->level < 60 )
  {
    ch_printf( ch, "Huh?" );
    return;
  }
  if( arg[0] == '\0' )
  {
    ch_printf( ch, "\n\rSyntax: androidfuse (first person) (second person)\n\r" );
    ch_printf( ch, "\n\rThe order in which you put the two people's names is very\n\r" );
    ch_printf( ch, "important. The first person will be the dominant person, and\n\r" );
    ch_printf( ch, "will control the resulting character. An android needs to\n\r" );
    ch_printf( ch, "fuse with two androids in order to become a super android.\n\r" );
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
  if( is_superandroid( rch ) )
  {
    ch_printf( ch, "%s already is a super android.\n\r", rch->name );
    return;
  }
  if( is_superandroid( vch ) )
  {
    ch_printf( ch, "%s already is a super android.\n\r", vch->name );
    return;
  }
  if( !is_android( rch ) || !is_android( vch ) )
  {
    ch_printf( ch, "Only androids may fuse with other androids.\n\r" );
    return;
  }
  if( ( is_android_fm( rch ) && is_android_fm( vch ) ) ||
      ( is_android_e( rch ) && is_android_e( vch ) ) || ( is_android_h( rch ) && is_android_h( vch ) ) )
  {
    ch_printf( ch, "Androids of the same type may not fuse.\n\r" );
    return;
  }

  if( ( rch->fm_core && is_android_fm( vch ) ) ||
      ( rch->e_core && is_android_e( vch ) ) || ( rch->h_core && is_android_h( vch ) ) )
  {
    ch_printf( ch, "%s has already absorbed a core of %s's android type.\n\r", rch->name, vch->name );
    return;
  }

  if( IS_IMMORTAL( rch ) || IS_IMMORTAL( vch ) )
  {
    ch_printf( ch, "Admins may not fuse.\n\r" );
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

  dch = rch;
  sch = vch;

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

  if( is_android_h( sch ) )
    dch->h_core = TRUE;
  if( is_android_e( sch ) )
    dch->e_core = TRUE;
  if( is_android_fm( sch ) )
    dch->fm_core = TRUE;

  dch->pcdata->quest_curr += sch->pcdata->quest_curr;
  dch->pcdata->quest_accum += sch->pcdata->quest_accum;
  dch->pcdata->pkills += sch->pcdata->pkills;
  dch->pcdata->pdeaths += sch->pcdata->pdeaths;
  dch->pcdata->mkills += sch->pcdata->mkills;
  dch->pcdata->mdeaths += sch->pcdata->mdeaths;
  dch->pcdata->spar_wins += sch->pcdata->spar_wins;
  dch->pcdata->spar_loss += sch->pcdata->spar_loss;
  dch->pcdata->sd_charge += sch->pcdata->sd_charge;

  /*
   * dch->exp += sch->exp;
   * dch->pl = dch->exp; 
   */

  dch->corespl += sch->exp;

  ch_printf( dch, "&G%s suddenly breaks apart. All of %'s body pieces drop the ground\n\r"
             "in a pile, except for the CPU and power core. Acting as if they had\n\r"
             "mind of their own, they fly towards you and absorb straight into\n\r" "your body.\n\r", sch->name, sch->name );

  ch_printf( sch, "&RYou suddenly lose control of your body. Your vision begins to twist\n\r"
             "and distort wildly. You try to scream out, but before the signal can\n\r"
             "reaches your voice systems, your body is ripped apart. The various\n\r"
             "pieces of your body fall to the ground in a heap; while the CPU and\n\r"
             "power core remain floating in the air, covered in a strange glow. Then,\n\r"
             "as if they had a mind of their own, they fly directly towards %s and\n\r"
             "are absorbed into %s's body. You cease to exist...\n\r", dch->name, dch->name );

  if( dch->fusions == 2 )
  {
    dch->exp += ( dch->corespl / 2 );
    dch->corespl = 0;

    dch->race = get_race_num( "super-android" );
    dch->class = get_class_num( "super-android" );
    dch->perm_str += 50;
    dch->perm_dex += 50;
    dch->perm_int += 50;
    dch->perm_con += 50;

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
    SET_BIT( dch->fusionflags, FUSION_SUPERANDROID );
    SET_BIT( sch->fusionflags, FUSION_STASIS );
    do_reserve( ch, sch->name );
    char_to( sch, ROOM_VNUM_FUSIONSTASIS );
    save_char_obj( dch );
    save_char_obj( sch );
  }
  else
  {
    SET_BIT( sch->fusionflags, FUSION_STASIS );
    do_reserve( ch, sch->name );
    char_to( sch, ROOM_VNUM_FUSIONSTASIS );
    save_char_obj( dch );
    save_char_obj( sch );
  }

  dch->pcdata->learned[gsn_sent_chip] = 0;
  dch->pcdata->learned[gsn_vm1] = 0;
  dch->pcdata->learned[gsn_vm2] = 0;

  ch_printf( ch, "%s and %s have been fused together.\n\r", dch->name, sch->name );
  return;
}

void do_rocket_punch( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( is_namek( ch ) )
  {
    ch_printf( ch, "You are not allowed.\n\r" );
    return;
  }

  bool virus = FALSE;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    virus = TRUE;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_rocket_punch]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_rocket_punch]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_rocket_punch]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_rocket_punch]->focus;

  WAIT_STATE( ch, skill_table[gsn_rocket_punch]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_rocket_punch ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 22, 27 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( AT_DGREY, "Your fist detaches, a powerful rocket firing it towards $N.", ch, NULL, victim, TO_CHAR );
    act( AT_DGREY, "$N is smashed by the flying fist, "
         "which quickly returns and reattaches to your arm. &W[$t]", ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_DGREY, "$n's fist detaches, a powerful rocket firing it toward you.", ch, NULL, victim, TO_VICT );
    act( AT_DGREY, "You are smashed by the flying fist, "
         "which quickly returns and reattaches to $n's arm. &W[$t]", ch, num_punct( dam ), victim, TO_VICT );
    act( AT_DGREY, "$n's fist detaches, a powerful rocket firing it toward $N.", ch, NULL, victim, TO_NOTVICT );
    act( AT_DGREY, "$N is smashed by the flying fist, "
         "which quickly returns and reattaches to $n's arm. &W[$t]", ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_rocket_punch );
    if( !virus )
      ch->melee = TRUE;
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
    ch->melee = FALSE;
  }
  else
  {
    act( AT_DGREY, "You missed $N with your rocket punch.", ch, NULL, victim, TO_CHAR );
    act( AT_DGREY, "$n misses you with $s rocket punch.", ch, NULL, victim, TO_VICT );
    act( AT_DGREY, "$n missed $N with a rocket punch.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_rocket_punch );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
    ch->melee = FALSE;
  }
  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_rocket_punch]->min_mana;
  return;
}

void do_drp( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( is_namek( ch ) )
  {
    ch_printf( ch, "You are not allowed.\n\r" );
    return;
  }
  bool virus = FALSE;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    virus = TRUE;
  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_drp]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_drp]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_drp]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_drp]->focus;

  WAIT_STATE( ch, skill_table[gsn_drp]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_drp ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 32, 40 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( AT_DGREY, "Both of your fists detach, powerful rockets firing them toward $N.", ch, NULL, victim, TO_CHAR );
    act( AT_DGREY, "$N is smashed by the flying fists, "
         "which quickly return and reattach to your arms. &W[$t]", ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_DGREY, "Both of $n's fists detach, powerful rockets firing them toward you.", ch, NULL, victim, TO_VICT );
    act( AT_DGREY, "You are smashed by the flying fists, "
         "which quickly return and reattach to $n's arms. &W[$t]", ch, num_punct( dam ), victim, TO_VICT );
    act( AT_DGREY, "Both of $n's fists detach, powerful rockets firing them toward $N.", ch, NULL, victim, TO_NOTVICT );
    act( AT_DGREY, "$N is smashed by the flying fists, "
         "which quickly return and reattach to $n's arms. &W[$t]", ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_drp );
    ch->melee = TRUE;
    if( !virus )
      global_retcode = damage( ch, victim, dam, TYPE_HIT );
    ch->melee = FALSE;
  }
  else
  {
    act( AT_DGREY, "You missed $N with your double rocket punch.", ch, NULL, victim, TO_CHAR );
    act( AT_DGREY, "$n misses you with $s double rocket punch.", ch, NULL, victim, TO_VICT );
    act( AT_DGREY, "$n missed $N with a double rocket punch.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_drp );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
    ch->melee = FALSE;
  }
  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_drp]->min_mana;
  return;
}

void do_railgun( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;
  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_railgun ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_railgun]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_railgun]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->pcdata->sd_charge < 1 && !is_bio(ch))
  {
    ch_printf( ch, "You don't have enough SD charges.\n\r" );
    return;
  }
  if( ch->focus < skill_table[gsn_railgun]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_railgun]->focus;

  ch->pcdata->sd_charge -= 1;

  WAIT_STATE( ch, skill_table[gsn_railgun]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_railgun ) )
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
            dam = get_attmod( ch, victim ) * number_range( 19, 20 );
            act( AT_BLUE,
                 "Your hand suddenly turns into a mass of light, which disappears back down into your forearm. Your forearm then clicks, and seperates into two parrallel halves, with random arcs of electricity jumping across the gap. You aim your arm at $N and begin to charge an immense amount of energy.",
                 ch, NULL, victim, TO_CHAR );
            act( AT_BLUE,
                 "As particles of light appear in the air and rush towards the gap between the halves of your arm, bolt of lightning franticly racing between them as the energy gathers, your arm begins to spin. It spins faster, and faster, becoming a blinding, crackling blur of energy and metal. Gigantic bolts of energy arc out from your arm at random, striking at the ground and sky as the energy you charge goes critical. Suddenly, there is a deafening sound of a thunder clap as a small, pulsing dart of light shoots out from your arm at $N, leaving a spiral trail of faint blue electricity in its wake. $N dodges the projectile at the last second. Your arm returns to normal afterwards.",
                 ch, NULL, victim, TO_CHAR );
            act( AT_BLUE,
                 "$n's hand suddenly turns into a mass of light, which disappears back down into $s forearm. $n's forearm then clicks, and seperates into two parrallel halves, with random arcs of electricity jumping across the gap. $n aims $s arm at you and begins to charge an immense amount of energy.",
                 ch, NULL, victim, TO_VICT );
            act( AT_BLUE,
                 "As particles of light appear in the air and rush towards the gap between the halves of $n's arm, bolt of lightning franticly racing between them as the energy gathers, $s arm begins to spin. It spins faster, and faster, becoming a blinding, crackling blur of energy and metal. Gigantic bolts of energy arc out from $n's arm at random, striking at the ground and sky as the energy $e charges goes critical. Suddenly, there is a deafening sound of a thunder clap as a small, pulsing dart of light shoots out from $n's arm at you, leaving a spiral trail of faint blue electricity in its wake. You dodge the projectile at the last second. $n's arm returns to normal afterwards.",
                 ch, NULL, victim, TO_VICT );
            act( AT_BLUE,
                 "$n's hand suddenly turns into a mass of light, which disappears back down into $s forearm. $n's forearm then clicks, and seperates into two parrallel halves, with random arcs of electricity jumping across the gap. $n aims $s arm at $N and begins to charge an immense amount of energy.",
                 ch, NULL, victim, TO_NOTVICT );
            act( AT_BLUE,
                 "As particles of light appear in the air and rush towards the gap between the halves of $n's arm, bolt of lightning franticly racing between them as the energy gathers, $s arm begins to spin. It spins faster, and faster, becoming a blinding, crackling blur of energy and metal. Gigantic bolts of energy arc out from $n's arm at random, striking at the ground and sky as the energy $e charges goes critical. Suddenly, there is a deafening sound of a thunder clap as a small, pulsing dart of light shoots out from $n's arm at $N, leaving a spiral trail of faint blue electricity in its wake. $N dodges the projectile at the last second. $n's arm returns to normal afterwards.",
                 ch, NULL, victim, TO_NOTVICT );
            learn_from_success( ch, gsn_railgun );
            global_retcode = damage( ch, victim, dam, TYPE_HIT );
            break;
          }

          dam = 999999999;

          act( AT_BLUE,
               "Your hand suddenly turns into a mass of light, which disappears back down into your forearm. Your forearm then clicks, and seperates into two parrallel halves, with random arcs of electricity jumping across the gap. You aim your arm at $N and begin to charge an immense amount of energy.",
               ch, NULL, victim, TO_CHAR );
          act( AT_BLUE,
               "As particles of light appear in the air and rush towards the gap between the halves of your arm, bolt of lightning franticly racing between them as the energy gathers, your arm begins to spin. It spins faster, and faster, becoming a blinding, crackling blur of energy and metal. Gigantic bolts of energy arc out from your arm at random, striking at the ground and sky as the energy you charge goes critical. Suddenly, there is a deafening sound of a thunder clap as a small, pulsing dart of light shoots out from your arm at $N, leaving a spiral trail of faint blue electricity in its wake. &R$N is hit by the projectile, and instantly killed.&B Your arm returns to normal afterwards. &W[$t]",
               ch, num_punct( dam ), victim, TO_CHAR );
          act( AT_BLUE,
               "$n's hand suddenly turns into a mass of light, which disappears back down into $s forearm. $n's forearm then clicks, and seperates into two parrallel halves, with random arcs of electricity jumping across the gap. $n aims $s arm at you and begins to charge an immense amount of energy.",
               ch, NULL, victim, TO_VICT );
          act( AT_BLUE,
               "As particles of light appear in the air and rush towards the gap between the halves of $n's arm, bolt of lightning franticly racing between them as the energy gathers, $s arm begins to spin. It spins faster, and faster, becoming a blinding, crackling blur of energy and metal. Gigantic bolts of energy arc out from $n's arm at random, striking at the ground and sky as the energy $e charges goes critical. Suddenly, there is a deafening sound of a thunder clap as a small, pulsing dart of light shoots out from $n's arm at you, leaving a spiral trail of faint blue electricity in its wake. &RYou are hit by the projectile, and instantly killed.&B $n's arm returns to normal afterwards. &W[$t]",
               ch, num_punct( dam ), victim, TO_VICT );
          act( AT_BLUE,
               "$n's hand suddenly turns into a mass of light, which disappears back down into $s forearm. $n's forearm then clicks, and seperates into two parrallel halves, with random arcs of electricity jumping across the gap. $n aims $s arm at $N and begins to charge an immense amount of energy.",
               ch, NULL, victim, TO_NOTVICT );
          act( AT_BLUE,
               "As particles of light appear in the air and rush towards the gap between the halves of $n's arm, bolt of lightning franticly racing between them as the energy gathers, $s arm begins to spin. It spins faster, and faster, becoming a blinding, crackling blur of energy and metal. Gigantic bolts of energy arc out from $n's arm at random, striking at the ground and sky as the energy $e charges goes critical. Suddenly, there is a deafening sound of a thunder clap as a small, pulsing dart of light shoots out from $n's arm at $N, leaving a spiral trail of faint blue electricity in its wake. &R$N is hit by the projectile, and instantly killed.&B $n's arm returns to normal afterwards. &W[$t]",
               ch, num_punct( dam ), victim, TO_NOTVICT );

          learn_from_success( ch, gsn_railgun );
          global_retcode = damage( ch, victim, dam, TYPE_HIT );
          break;
        }
      default:
        dam = get_attmod( ch, victim ) * number_range( 80, 90 );
        if( ch->charge > 0 )
          dam = chargeDamMult( ch, dam );

        act( AT_BLUE,
             "Your hand suddenly turns into a mass of light, which disappears back down into your forearm. Your forearm then clicks, and seperates into two parrallel halves, with random arcs of electricity jumping across the gap. You aim your arm at $N and begin to charge an immense amount of energy.",
             ch, NULL, victim, TO_CHAR );
        act( AT_BLUE,
             "As particles of light appear in the air and rush towards the gap between the halves of your arm, bolt of lightning franticly racing between them as the energy gathers, your arm begins to spin. It spins faster, and faster, becoming a blinding, crackling blur of energy and metal. Gigantic bolts of energy arc out from your arm at random, striking at the ground and sky as the energy you charge goes critical. Suddenly, there is a deafening sound of a thunder clap as a small, pulsing dart of light shoots out from your arm at $N, leaving a spiral trail of faint blue electricity in its wake. $N is hit by the projectile, and dealt an incredible amount of damage. Your arm returns to normal afterwards. &W[$t]",
             ch, num_punct( dam ), victim, TO_CHAR );
        act( AT_BLUE,
             "$n's hand suddenly turns into a mass of light, which disappears back down into $s forearm. $n's forearm then clicks, and seperates into two parrallel halves, with random arcs of electricity jumping across the gap. $n aims $s arm at you and begins to charge an immense amount of energy.",
             ch, NULL, victim, TO_VICT );
        act( AT_BLUE,
             "As particles of light appear in the air and rush towards the gap between the halves of $n's arm, bolt of lightning franticly racing between them as the energy gathers, $s arm begins to spin. It spins faster, and faster, becoming a blinding, crackling blur of energy and metal. Gigantic bolts of energy arc out from $n's arm at random, striking at the ground and sky as the energy $e charges goes critical. Suddenly, there is a deafening sound of a thunder clap as a small, pulsing dart of light shoots out from $n's arm at you, leaving a spiral trail of faint blue electricity in its wake. You are hit by the projectile, and dealt an incredible amount of damage. $n's arm returns to normal afterwards. &W[$t]",
             ch, num_punct( dam ), victim, TO_VICT );
        act( AT_BLUE,
             "$n's hand suddenly turns into a mass of light, which disappears back down into $s forearm. $n's forearm then clicks, and seperates into two parrallel halves, with random arcs of electricity jumping across the gap. $n aims $s arm at $N and begins to charge an immense amount of energy.",
             ch, NULL, victim, TO_NOTVICT );
        act( AT_BLUE,
             "As particles of light appear in the air and rush towards the gap between the halves of $n's arm, bolt of lightning franticly racing between them as the energy gathers, $s arm begins to spin. It spins faster, and faster, becoming a blinding, crackling blur of energy and metal. Gigantic bolts of energy arc out from $n's arm at random, striking at the ground and sky as the energy $e charges goes critical. Suddenly, there is a deafening sound of a thunder clap as a small, pulsing dart of light shoots out from $n's arm at $N, leaving a spiral trail of faint blue electricity in its wake. $N is hit by the projectile, and dealt an incredible amount of damage. $n's arm returns to normal afterwards. &W[$t]",
             ch, num_punct( dam ), victim, TO_NOTVICT );

        learn_from_success( ch, gsn_railgun );
        global_retcode = damage( ch, victim, dam, TYPE_HIT );
        break;
    }
  }
  else
  {
    act( AT_BLUE, "You missed $N with your rail gun.", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "$n misses you with $s rail gun.", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "$n missed $N with a rail gun.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_railgun );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_railgun]->min_mana;
  return;
}

void do_cruise_punch( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  sh_int dir = 0;
  bool hasexit = TRUE;
  EXIT_DATA *exit;

  if( is_namek( ch ) )
  {
    ch_printf( ch, "You are not allowed.\n\r" );
    return;
  }
  bool virus = FALSE;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    virus = TRUE;
  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_cruise_punch]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !has_exits( ch->in_room ) )
  {
    ch_printf( ch, "This room has no exits!\n\r" );
    return;
  }

  if( IS_NPC( victim ) )
  {
    ch_printf( ch, "You can't use cruise punch on NPC's!\n\r" );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_cruise_punch]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_cruise_punch]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_cruise_punch]->focus;

  WAIT_STATE( ch, skill_table[gsn_cruise_punch]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_cruise_punch ) )
  {
    act( AT_DGREY,
         "Your fist detaches and launches towards $N. But instead of hitting $M, the arm grabs $N by the head and hits the afterburners; throwing $N out of the room at high speeds!",
         ch, NULL, victim, TO_CHAR );
    act( AT_DGREY,
         "$n's fist detaches and launches towards you. But instead of hitting you, the arm grabs you by the head and hits the afterburners; throwing you out of the room at high speeds!",
         ch, NULL, victim, TO_VICT );
    act( AT_DGREY,
         "$n's fist detaches and launches towards $N. But instead of hitting $M, the arm grabs $N by the head and hits the afterburners; throwing $N out of the room at high speeds!",
         ch, NULL, victim, TO_NOTVICT );


    if( get_exit( ch->in_room, DIR_NORTH ) )
      dir = DIR_NORTH;
    else if( get_exit( ch->in_room, DIR_EAST ) )
      dir = DIR_EAST;
    else if( get_exit( ch->in_room, DIR_WEST ) )
      dir = DIR_WEST;
    else if( get_exit( ch->in_room, DIR_SOUTH ) )
      dir = DIR_SOUTH;
    else if( get_exit( ch->in_room, DIR_NORTHEAST ) )
      dir = DIR_NORTHEAST;
    else if( get_exit( ch->in_room, DIR_NORTHWEST ) )
      dir = DIR_NORTHWEST;
    else if( get_exit( ch->in_room, DIR_SOUTHEAST ) )
      dir = DIR_SOUTHEAST;
    else if( get_exit( ch->in_room, DIR_SOUTHWEST ) )
      dir = DIR_SOUTHWEST;
    else if( get_exit( ch->in_room, DIR_DOWN ) )
      dir = DIR_DOWN;
    else if( get_exit( ch->in_room, DIR_UP ) )
      dir = DIR_UP;

    stop_fighting( ch, TRUE );

    act( AT_RED, "$N is sent flying out of the room!", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "You are sent flying out of the room!", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$N is sent flying out of the room!", ch, NULL, victim, TO_NOTVICT );

    while( hasexit )
    {
      exit = get_exit( victim->in_room, dir );
      if( !exit )
      {
        do_look( victim, "" );
        act( AT_RED, "$n is sent flying into the room!", victim, NULL, NULL, TO_ROOM );
        act( AT_RED, "$n slams into a wall!", victim, NULL, NULL, TO_ROOM );
        hasexit = FALSE;
        break;
      }
      char_from_room( victim );
      char_to_room( victim, exit->to_room );
      do_look( victim, "" );
      exit = get_exit( victim->in_room, dir );
      if( !exit || ( victim->in_room == ch->in_room ) )
      {
        act( AT_RED, "$n is sent flying into the room!", victim, NULL, NULL, TO_ROOM );
        act( AT_RED, "$n slams into a wall!", victim, NULL, NULL, TO_ROOM );
        hasexit = FALSE;
        if( virus )
          WAIT_STATE( victim, 16 );
        break;
      }
      act( AT_RED, "$n is sent flying into the room!", victim, NULL, NULL, TO_ROOM );
      act( AT_RED, "$n is sent flying back out of the room!", victim, NULL, NULL, TO_ROOM );
    }
    act( AT_RED, "Your fist returns to you.", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "$n's fist returns to him.", ch, NULL, NULL, TO_ROOM );

    learn_from_success( ch, gsn_cruise_punch );
  }
  else
  {
    act( AT_DGREY, "You missed $N with your cruise punch.", ch, NULL, victim, TO_CHAR );
    act( AT_DGREY, "$n misses you with $s cruise punch.", ch, NULL, victim, TO_VICT );
    act( AT_DGREY, "$n missed $N with a cruise punch.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_cruise_punch );
  }
  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_cruise_punch]->min_mana;
  return;
}

void do_electric_shield( CHAR_DATA * ch, char *argument )
{
  AFFECT_DATA af;
  int shieldGsn = 0;
  int nacMod = 0;

  if( is_namek( ch ) )
  {
    ch_printf( ch, "You are not allowed.\n\r" );
    return;
  }
  bool virus = FALSE;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    virus = TRUE;
  if( !IS_NPC( ch ) )
  {
    if( ch->pcdata->learned[gsn_electric_shield4] > 0 )
    {
      shieldGsn = gsn_electric_shield4;
      nacMod = 1500;
      if( virus )
        nacMod *= 1.25;
    }
    else if( ch->pcdata->learned[gsn_electric_shield3] > 0 )
    {
      shieldGsn = gsn_electric_shield3;
      nacMod = 1000;
      if( virus )
        nacMod *= 1.25;
    }
    else if( ch->pcdata->learned[gsn_electric_shield2] > 0 )
    {
      shieldGsn = gsn_electric_shield2;
      nacMod = 750;
      if( virus )
        nacMod *= 1.25;
    }
    else
    {
      shieldGsn = gsn_electric_shield;
      nacMod = 500;
      if( virus )
        nacMod *= 1.25;
    }
  }

  if( !IS_NPC( ch ) && !xIS_SET( ch->affected_by, AFF_ELECTRICSHIELD ) && ch->mana < skill_table[shieldGsn]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( xIS_SET( ch->affected_by, AFF_ELECTRICSHIELD ) )
  {
//      send_to_char( "Your electric shield is already up.\n\r", ch );
//            return;
    act( AT_SKILL, "You clench your fists and power down your electric shield.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "The crackling electric shield around $n vanishes.", ch, NULL, NULL, TO_NOTVICT );
    xREMOVE_BIT( ch->affected_by, AFF_ELECTRICSHIELD );
    affect_strip( ch, shieldGsn );
    return;
  }

  if( can_use_skill( ch, number_percent(  ), shieldGsn ) )
  {
    act( AT_SKILL, "You clench your fists as a powerful electric shield sparks up around you.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "A crackling electric shield sparks up around $n.", ch, NULL, NULL, TO_NOTVICT );

    learn_from_success( ch, shieldGsn );
    af.type = shieldGsn;
    af.duration = ( 48 ) * DUR_CONV;
    af.location = APPLY_NATURALAC;
    af.bitvector = meb( AFF_ELECTRICSHIELD );
    af.modifier = nacMod;
    affect_to_char( ch, &af );
  }
  else
  {
    act( AT_SKILL, "Your shield sparks, but fades quickly...", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n's shield sparks, but $s shield quickly fades away.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, shieldGsn );
  }

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[shieldGsn]->min_mana;
  return;
}

void do_multi_eye( CHAR_DATA * ch, char *arg )
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
    if( !can_use_skill( ch->master, number_percent(  ), gsn_multi_eye ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_multi_eye]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( arg[0] == '\0' || atoi( arg ) <= 0 )
  {
    send_to_char( "Syntax: 'multi eye beam' <# of beams>\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  int z = get_aura( ch );

  shot = atoi( arg );
  if( shot > 25 )
    shot = 25;
  if( shot < 2 )
    shot = 2;
  energy = shot * skill_table[gsn_multi_eye]->min_mana;
  shots = shot;
  strcpy( buf, num_punct( shot ) );

  if( ch->mana < energy )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  /*
   * was (1+(shot/10)) 
   */
  int focus = ( skill_table[gsn_multi_eye]->focus * ( shot / 10 ) );
  if( focus < 12 )
    focus = 12;

  if( ch->focus < focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= focus;

  WAIT_STATE( ch, ( skill_table[gsn_multi_eye]->beats ) );

  if( can_use_skill( ch, number_percent(  ), gsn_multi_eye ) )
  {
    while( shots > 0 )
    {
      switch ( number_range( 1, 2 ) )
      {
        default:
        case 1:
          break;
        case 2:
          dam += number_range( 0, 2 );
          break;
          /*
           * case 3:
           * dam += number_range( 1, 3 );
           * break;
           */
      }
      shots--;
    }
    dam2 = number_range( 2, 5 );
    if( dam2 > 3 )
      dam2--; // caps the damage mod at x4, and lowers the chance of it
    // occuring by an additional 50 percent. - Karma.
    strcpy( buf2, num_punct( get_attmod( ch, victim ) * dam * dam2 ) );

    act2( z,
          "Your eyes glow brightly as they begin rapid-firing twin beams of energy at $N like a machine gun. $N is bombarded by $t pairs of energy beams. &W[$T]",
          ch, buf, victim, TO_CHAR, buf2 );
    act2( z,
          "$n's eyes glow brightly as they begin rapid-firing twin beams of energy at you like a machine gun. You are bombarded by $t pairs of energy beams. &W[$T]",
          ch, buf, victim, TO_VICT, buf2 );
    act2( z,
          "$n's eyes glow brightly as they begin rapid-firing twin beams of energy at $N like a machine gun. $N is bombarded by $t pairs of energy beams. &W[$T]",
          ch, buf, victim, TO_NOTVICT, buf2 );

    learn_from_success( ch, gsn_multi_eye );
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * dam * dam2 ), TYPE_HIT );
  }
  else
  {
    act( z, "$t beams of energy fly from your eyes towards $N but $E is too fast to hit.", ch, buf, victim, TO_CHAR );
    act( z, "$t beams of energy fly from $n's eyes towards you but you dodge them.", ch, buf, victim, TO_VICT );
    act( z, "$t beams of energy fly from $n's eyes towards $N but $E is too fast to hit.", ch, buf, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_multi_eye );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  if( !is_android_h( ch ) )
    ch->mana -= energy;
  return;
}

void do_self_destruct( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  CHAR_DATA *vch;
  int dam = 0;
//    int chargesUsed = 50;
//    char arg[MAX_STRING_LENGTH];
//    int addCharges = 0;

  if( is_namek( ch ) )
  {
    ch_printf( ch, "You are not allowed.\n\r" );
    return;
  }

  if( xIS_SET( ch->act, PLR_SPAR ) )
  {
    send_to_char( "Bad!  You can't kill in spars!.\n\r", ch );
    return;
  }


  {
    send_to_char( "Bad NPC!  Be nice to players.\n\r", ch );
    return;
  }

  if( IS_NPC( vch ) )
  {
    send_to_char( "&GThis is &RPKILL ONLY!!&G - Kal&D\n\r", ch );
    return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_self_destruct]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_self_destruct]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

/*        if( argument[0] != '\0' )
        {
          one_argument( argument, arg );
          if( arg[0] != '\0' && is_number( arg ) )
          {
            addCharges = atoi( arg );
            chargesUsed += addCharges;
          }
	  else
	  {
		send_to_char( "You need to specify a number of charges to use.\n\r", ch );
		return;
	  }
        }
 	else
	{
		send_to_char( "You need to specify a number of charges to use.\n\r", ch );
		return;
	}
*/
//  if ( !IS_NPC(ch) && ch->pcdata->sd_charge < chargesUsed )
  if( !IS_NPC( ch ) && ch->pcdata->sd_charge < 50 && !IS_BIOANDROID(ch) )
  {
    send_to_char( "You need to charge up more first.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_self_destruct]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_self_destruct]->focus;

  WAIT_STATE( ch, skill_table[gsn_self_destruct]->beats );
//    ch->pcdata->sd_charge = 0;
//    ch->pcdata->sd_charge -= chargesUsed;
  if ( !IS_BIOANDROID(ch) )
    ch->pcdata->sd_charge -= 50;
  if( can_use_skill( ch, number_percent(  ), gsn_self_destruct ) )
  {
    act( AT_SKILL, "You charge all your energy into a final deadly blast incinerating $N along with your self.", ch, NULL,
         victim, TO_CHAR );
    act( AT_SKILL, "$n charges all $s energy into a final deadly blast incinerating you along with $m.", ch, NULL, victim,
         TO_VICT );
    act( AT_SKILL, "$n charges all $s energy into a final deadly blast incinerating $N along with $m self.", ch, NULL,
         victim, TO_NOTVICT );

    learn_from_success( ch, gsn_self_destruct );
    //dam = victim->max_hit * 2;
    dam = 999999999;
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
//  dam = ch->max_hit * 2;
    //dam = 50 * ((chargesUsed-50)/10);
    dam = 999999999;
    global_retcode = damage( ch, ch, dam, TYPE_HIT );
  }
  else
  {
    act( AT_SKILL, "You charge all your energy into a final deadly blast, BUT $N DODGES YOUR ATTACK!", ch, NULL, victim,
         TO_CHAR );
    act( AT_SKILL, "$n charges all $s energy into a final deadly blast, you RUN!", ch, NULL, victim, TO_VICT );
    act( AT_SKILL,
         "$n changes all $s energy into a final deadly blast, but $N takes off like a bat outa hell and $n misses!", ch,
         NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_self_destruct );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
//  dam = ch->max_hit * 2;
    dam = 999999999;
    global_retcode = damage( ch, ch, dam, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_self_destruct]->min_mana;
  return;
}

void do_duplicate( CHAR_DATA * ch, char *argument )
{
  return;
}

void do_uppercut( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( is_namek( ch ) )
  {
    ch_printf( ch, "You are not allowed.\n\r" );
    return;
  }
  bool virus = FALSE;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    virus = TRUE;
  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_uppercut]->skill_level[ch->class] )
  {
    send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_uppercut]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_uppercut]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_uppercut]->focus;

  WAIT_STATE( ch, skill_table[gsn_uppercut]->beats );

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_uppercut]->min_mana;

  if( can_use_skill( ch, number_percent(  ), gsn_uppercut ) )
  {
    act( AT_SKILL, "You launch your fist upwards, hitting $N.", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "$n launchs $s fist upwards, hitting you.", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "$n launchs $s fist upwards, hitting $N.", ch, NULL, victim, TO_NOTVICT );

    learn_from_success( ch, gsn_uppercut );
    if( !virus )
      ch->melee = TRUE;
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 6, 10 ) ), gsn_uppercut );
    ch->melee = FALSE;
  }
  else
  {
    act( AT_SKILL, "You launch your fist upwards, but you miss $N.", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "$n launchs $s fist upwards, but $e misses you.", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "$n launchs $s fist upwards, but $e misses $N.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_uppercut );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, 0, gsn_uppercut );
    ch->melee = FALSE;
  }
  return;
}

void do_evolve( CHAR_DATA * ch, char *argument )
{

  if( is_namek( ch ) )
  {
    ch_printf( ch, "You are not allowed.\n\r" );
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

  if( IS_NPC( ch ) || ch->race != 6 )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }

  if( argument[0] == '\0' )
  {
    send_to_char( "How do you want to evolve?\n\r", ch );
    return;
  }

  if( str_prefix( argument, "semi-perfect form" )
      && str_prefix( argument, "perfect form" ) && str_prefix( argument, "ultra-perfect form" ) )
  {
    send_to_char( "You can't evolve into that.\n\r", ch );
    return;
  }

  if( ( !str_prefix( argument, "semi-perfect form" ) && ch->pcdata->learned[gsn_semiperfect] <= 0 )
      || ( !str_prefix( argument, "perfect form" ) && ch->pcdata->learned[gsn_perfect] <= 0 )
      || ( !str_prefix( argument, "ultra-perfect form" ) && ch->pcdata->learned[gsn_ultraperfect] <= 0 ) )
  {
    send_to_char( "You must absorb more energy before you can evolve into that form.\n\r", ch );
    return;
  }

  if( !str_prefix( argument, "semi-perfect form" ) && ch->pcdata->learned[gsn_semiperfect] > 0 )
  {
    if( ch->mana < skill_table[gsn_semiperfect]->min_mana )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }

    if( xIS_SET( ch->affected_by, AFF_SEMIPERFECT ) )
    {
      send_to_char( "You are already in that form.\n\r", ch );
      return;
    }

    xSET_BIT( ch->affected_by, AFF_SEMIPERFECT );
    xREMOVE_BIT( ch->affected_by, AFF_PERFECT );
    xREMOVE_BIT( ch->affected_by, AFF_ULTRAPERFECT );
//    act( AT_SKILL, "You evolve into your semi-perfect form.", ch, NULL, NULL, TO_CHAR );
//    act( AT_SKILL, "$n evolves into $s semi-perfect form.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_PURPLE, "Your horns shift closer together, your features becoming more humanoid as "
         "you grow in size and complete your evolution to a Semi-Perfect form.", ch, NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "$n's horns shift closer together, $s features becoming more humanoid as "
         "$e grows in size and completes $s evolution to a Semi-Perfect form.", ch, NULL, NULL, TO_NOTVICT );
    ch->pl = ch->exp * 16;
    transStatApply( ch, 16, 16, 16, 16 );
    if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
      ch->pl = ch->pl * 1.1;
    ch->mana -= skill_table[gsn_semiperfect]->min_mana;
    return;
  }

  if( !str_prefix( argument, "perfect form" ) && ch->pcdata->learned[gsn_perfect] > 0 )
  {
    if( ch->mana < skill_table[gsn_perfect]->min_mana )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }

    if( xIS_SET( ch->affected_by, AFF_PERFECT ) )
    {
      send_to_char( "You are already in that form.\n\r", ch );
      return;
    }

    xSET_BIT( ch->affected_by, AFF_PERFECT );
    xREMOVE_BIT( ch->affected_by, AFF_SEMIPERFECT );
    xREMOVE_BIT( ch->affected_by, AFF_ULTRAPERFECT );
//    act( AT_SKILL, "You evolve into your perfect form.", ch, NULL, NULL, TO_CHAR );
//    act( AT_SKILL, "$n evolves into $s perfect form.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_PURPLE, "You surge with energy as your body becomes more compact, efficient, and almost "
         "human.  Your horns fuse into a kind of hat, your tail retracts up under your "
         "wings as you evolve to a Perfect form.", ch, NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "$n surges with energy as $s body becomes more compact, efficient, and almost "
         "human.  $n's horns fuse into a kind of hat, $s tail retracts up under $s wings "
         "as $e evolves to a Perfect form.", ch, NULL, NULL, TO_NOTVICT );
    ch->pl = ch->exp * 30;
    transStatApply( ch, 30, 30, 30, 30 );
    if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
      ch->pl = ch->pl * 1.1;
    ch->mana -= skill_table[gsn_perfect]->min_mana;
    return;
  }

  if( !str_prefix( argument, "ultra-perfect form" ) && ch->pcdata->learned[gsn_ultraperfect] > 0 )
  {
    if( ch->mana < skill_table[gsn_ultraperfect]->min_mana )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }

    if( xIS_SET( ch->affected_by, AFF_ULTRAPERFECT ) )
    {
      send_to_char( "You are already in that form.\n\r", ch );
      return;
    }

    xSET_BIT( ch->affected_by, AFF_ULTRAPERFECT );
    xREMOVE_BIT( ch->affected_by, AFF_PERFECT );
    xREMOVE_BIT( ch->affected_by, AFF_SEMIPERFECT );
//    act( AT_SKILL, "You evolve into your ultra-perfect form.", ch, NULL, NULL, TO_CHAR );
//    act( AT_SKILL, "$n evolves into $s ultra-perfect form.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_PURPLE, "You draw on newfound power, your body moving with perfect grace and "
         "efficiency, your entire form free from blemish or impurity.  You calmly stand "
         "in this most human of forms, having evolved to an Ultra-Perfect being.", ch, NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "$n draws on newfound power, $s body moving with perfect grace and efficiency, "
         "$s entire form free from blemish or impurity.  $n calmly stands in this most "
         "human of forms, having evolved to an Ultra-Perfect being.", ch, NULL, NULL, TO_NOTVICT );
    ch->pl = ch->exp * 38;
    transStatApply( ch, 38, 38, 38, 38 );
    if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
      ch->pl = ch->pl * 1.1;
    ch->mana -= skill_table[gsn_ultraperfect]->min_mana;
    return;
  }

  return;
}

/*
 * Checks to see if the android is able to absorb part (or all)
 * of a Ki attack.  This function must be added to the ki attack
 * code for it to be checked.
 */
int ki_absorb( CHAR_DATA * ch, CHAR_DATA * victim, int damage, int ki_sn )
{
  char buf1[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  int newDam = 0;
  int reduction = 0;
  int kiGain = 0;
  bool dcd_fail = FALSE;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    return ( damage );
  }
  bool virus = FALSE;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    virus = TRUE;
  if( !IS_NPC( ch ) )
  {
    if( !IS_SET( ch->pcdata->combatFlags, CMB_NO_DCD ) )
    {
      if( ch->pcdata->learned[gsn_dcd] < 1 )
        dcd_fail = TRUE;

      else if( !can_use_skill( ch, number_percent(  ), gsn_dcd ) )
        dcd_fail = TRUE;

      else if( damage > 1000 )
        dcd_fail = TRUE;
    }
  }
/* Start Reflection
 */
	if( !IS_NPC(ch) )
	{
		if ( IS_AFFECTED(ch, AFF_REFLECTION) )
		{
			int realDam = 0;
			if ( dice(1,15) > 7 )
			{
				if ( number_range( 1,15 ) > 10 )
				{
					if ( damage > 1000 )
						return (damage);
					if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
					{
						realDam = damage /2;
						victim->hit -= realDam;
					}
					else if ( !IS_AFFECTED(victim, AFF_SANCTUARY) )
					{
						realDam = damage;
						victim->hit -= realDam;
					}
					damage = 0;
					sprintf( buf1, "Your body reflects $N's attack! [%s]", num_punct(realDam) );
					act( AT_BLUE, buf1, ch, NULL, victim, TO_CHAR );
					sprintf( buf2, "$n's body reflects $N's attack! [%s]", num_punct(realDam) );
					act( AT_BLUE, buf2, ch, NULL, victim, TO_ROOM );
					return 0;
				}
			}
		}
		/* End Reflection
		 */
    if( !IS_SET( ch->pcdata->combatFlags, CMB_NO_DCD ) && !dcd_fail )
    {
/*	if( ch->pcdata->learned[gsn_dcd] < 1 )
	  return (damage);

	if ( !can_use_skill(ch, number_percent(),gsn_dcd) )
	  return (damage);

	if ( damage > 1000 )  So that instant kills cannot be blocked.
	  return (damage); */

      int att = ( ( get_attmod( ch, victim ) / 2 ) * 10 );
      int revatt = ( ( get_attmod( victim, ch ) / 2 ) * 5 );
      if( att > 100 )
        att = 100;
      if( att < 0 )
        att = 0;
      if( revatt > 100 )
        revatt = 100;
      if( revatt < 0 )
        revatt = 0;

      int c = 0;
      int spd = 0;
      if( get_curr_dex( ch ) > get_curr_dex( victim ) )
      {
        if( ( ( float )get_curr_dex( ch ) / ( float )get_curr_dex( victim ) ) >= 1.25 )
          spd = 10;
        if( ( ( float )get_curr_dex( ch ) / ( float )get_curr_dex( victim ) ) >= 1.5 )
          spd = 20;
        if( ( ( float )get_curr_dex( ch ) / ( float )get_curr_dex( victim ) ) >= 2 )
          spd = 30;
        if( ( ( float )get_curr_dex( ch ) / ( float )get_curr_dex( victim ) ) >= 3 )
          spd = 50;
      }
      else if( get_curr_dex( ch ) < get_curr_dex( victim ) )
      {
        if( ( ( float )get_curr_dex( victim ) / ( float )get_curr_dex( ch ) ) >= 1.25 )
          spd = -5;
        if( ( ( float )get_curr_dex( victim ) / ( float )get_curr_dex( ch ) ) >= 1.5 )
          spd = -10;
        if( ( ( float )get_curr_dex( victim ) / ( float )get_curr_dex( ch ) ) >= 2 )
          spd = -15;
        if( ( ( float )get_curr_dex( victim ) / ( float )get_curr_dex( ch ) ) >= 3 )
          spd = -20;
      }
      else
        spd = 0;

      c = ( ( 20 + spd ) + ( att - revatt ) );

      if( c < 1 )
        c = 1;
      if( c > 100 )
        c = 100;

      if( number_range( 1, 100 ) > c )
      {
        learn_from_failure( ch, gsn_dcd );
        return ( damage );
      }
      else
        learn_from_success( ch, gsn_dcd );

      int dcd = number_range( 1, 3 );
      if( dcd == 1 )
      {
        ch->ki_dodge = TRUE;
        newDam = 0;
        return ( newDam );
      }
      else if( dcd == 2 )
      {
        ch->ki_deflect = TRUE;
        newDam = 0;
        return ( newDam );
      }
      else if( dcd == 3 )
      {
        ch->ki_cancel = TRUE;
        newDam = 0;
        return ( newDam );
      }
    }
  }

  if( !is_android( ch ) )
  {
    return ( damage );
  }

  if( !IS_NPC( ch ) )
  {
    if( ch->pcdata->learned[gsn_ki_absorb] <= 0 )
      return ( damage );
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_ki_absorb]->skill_level[ch->class] )
  {
    return ( damage );
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    return ( damage );
  }

  if( !IS_NPC( ch ) && IS_SET( ch->pcdata->combatFlags, CMB_NO_KI_ABSORB ) )
  {
    return ( damage );
  }
  int chance = 0;
  if( is_android_e( ch ) )
    chance = 50;
  else if( is_android_fm( ch ) )
    chance = 35;
  else
    chance = 25;

  if( virus )
    chance += 10;
  if( chance > 50 )
    chance = 50;

  if( number_range( 1, 100 ) > chance ) // was 25
  {
    return ( damage );
  }

/*    if (ch->focus < skill_table[gsn_ki_absorb]->focus)
    {
	return (damage);
    }
    else
    	ch->focus -= skill_table[gsn_ki_absorb]->focus;*/

  //WAIT_STATE( ch, skill_table[gsn_ki_absorb]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_ki_absorb ) )
  {
    act( AT_SKILL, "$N's Ki attack washes over your outstretched hands as you absorb some of it.", ch, NULL, victim,
         TO_CHAR );
    act( AT_SKILL, "Your Ki attack washes over $n's outstretched hands as $e absorbs some of it.", ch, NULL, victim,
         TO_VICT );
    act( AT_SKILL, "$N's Ki attack washes over $n's outstretched hands as $e absorbs some of it.", ch, NULL, victim,
         TO_NOTVICT );

    learn_from_success( ch, gsn_ki_absorb );
    switch ( number_range( 0, 4 ) )
    {
      case 0:
        reduction = 0;  // reduced to 0
        break;

      case 1:
        reduction = 25; // reduced to 25%
        break;

      case 2:
        reduction = 50; // reduced to 50%
        break;

      case 3:
        reduction = 75; // reduced to 75%
        break;

      case 4:
        reduction = 100;  // no reduction
        break;
    }
    newDam = damage * reduction / 100;
    if( reduction == 0 )
      kiGain = skill_table[ki_sn]->min_mana;
    else
      kiGain = skill_table[ki_sn]->min_mana * reduction / 100;

    ch->mana = UMAX( ch->max_mana, ch->mana + kiGain );
    if( ch->mana > ch->max_mana )
      ch->mana = ch->max_mana;

    if( !IS_NPC( ch ) )
    {
      if( ch->battery < 100 && reduction < 100 &&
          is_android_e( ch ) &&
          ch->exp >= skill_table[gsn_battery]->skill_level[ch->class] && ch->pcdata->learned[gsn_battery] > 0 )
      {
        if( reduction >= 100 )
        {
          ch_printf( ch, "&BYou fail to absorb any spare energy into your ki battery.\n\r" );
        }
        else
        {
          int battery = 0;
          battery = ( damage - newDam );
          if( battery > 10 )
            battery = 10;
          ch->battery += battery;
          if( ch->battery > 100 )
            ch->battery = 100;

          ch_printf( ch, "&BYou absorb %d points of damage directly into your ki battery.\n\r", battery );
        }
      }
    }
  }
  else
  {
    act( AT_SKILL, "You hold your hands forward in a futile attempt to absorb $N's Ki attack.", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "$n holds $s hands forward in a futile attempt to absorb your Ki attack.", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "$n holds $s hands forward in a futile attempt to absorb $N's Ki attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_ki_absorb );
    newDam = damage;
  }
  return ( newDam );
}

void do_hells_flash( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

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

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_hells_flash]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_hells_flash]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_hells_flash]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_hells_flash]->focus;

  WAIT_STATE( ch, skill_table[gsn_hells_flash]->beats );

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_hells_flash]->min_mana;

  if( can_use_skill( ch, number_percent(  ), gsn_hells_flash ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 38, 42 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    /*
     * act( AT_YELLOW, "Your hands unhinge at the wrist, exposing a frightening array of nozzles", ch, NULL, victim, TO_CHAR );
     * act( AT_YELLOW, "and barrels.  The smell of rocket fuel permeates the air as you unleash", ch, NULL, victim, TO_CHAR );
     * act( AT_YELLOW, "a hellish inferno upon $N. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
     * act( AT_YELLOW, "$n's hands unhinge at the wrist, exposing a frightening array of nozzles", ch, NULL, victim, TO_VICT );
     * act( AT_YELLOW, "and barrels.  The smell of rocket fuel permeates the air as $e unleashes", ch, NULL, victim, TO_VICT );
     * act( AT_YELLOW, "a hellish inferno upon you. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
     * act( AT_YELLOW, "$n's hands unhinge at the wrist, exposing a frightening array of nozzles", ch, NULL, victim, TO_NOTVICT );
     * act( AT_YELLOW, "and barrels.  The smell of rocket fuel permeats the air as $n unleashes", ch, NULL, victim, TO_NOTVICT );
     * act( AT_YELLOW, "a hellish inferno upon $N. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );
     */

    act( AT_YELLOW,
         "You suddenly appear infront of $N, dealing a quick punch to $S gut, then elbowing $M into the ground. You reach down and grab $N with both arms and lift $M over your head. Yelling out, you then throw $N down into the ground with tremendous force.",
         ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW,
         "You cross your arms and pull them apart just below the elbow; holding your forearms under your arms, as your arms now display a large array of energy crystals. Your arms start to glow as you charge your energy. Shouting 'HELLS FLASH', you unleash a large blast of energy into the hole $N is laying in, causing considerable damage. After you are done firing, you reattach your arms. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_YELLOW,
         "$n suddenly appears infront of you, dealing a quick punch to your gut, then elbowing you into the ground. $n reaches down and grabs you with both arms and lifts you over $s head. Yelling out, $n then throws you down into the ground with tremendous force.",
         ch, NULL, victim, TO_VICT );
    act( AT_YELLOW,
         "$n crosses $s arms and pulls them apart just below the elbow; holding $s forearms under $s arms, as $s arms now display a large array of energy crystals. $n's arms start to glow as $e charges $s energy. Shouting 'HELLS FLASH', $n unleashes a large blast of energy into the hole you are laying in, causing considerable damage. After $n is done firing, $e reattaches $s arms. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_YELLOW,
         "$n suddenly appears infront of $N, dealing a quick punch to $S gut, then elbowing $M into the ground. $n reaches down and grabs $N with both arms and lifts $M over $s head. Yelling out, $n then throws $N down into the ground with tremendous force.",
         ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW,
         "$n crosses $s arms and pulls them apart just below the elbow; holding $s forearms under $s arms, as $s arms now display a large array of energy crystals. $n's arms start to glow as $e charges $s energy. Shouting 'HELLS FLASH', $n unleashes a large blast of energy into the hole $N is laying in, causing considerable damage. After $n is done firing, $e reattaches $s arms. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_hells_flash );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    /*
     * act( AT_YELLOW, "Your hands unhinge at the wrist but only a weak puff of smoke escapes.", ch, NULL, victim, TO_CHAR );
     * act( AT_YELLOW, "$n's hands unhinge at the wrist but only a weak puff of smoke escapes.", ch, NULL, victim, TO_VICT );
     * act( AT_YELLOW, "$n's hands unhinge at the wrist but only a weak puff of smoke escapes.", ch, NULL, victim, TO_NOTVICT );
     */
    act( AT_YELLOW, "You fail to catch $N.", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "$n fails to catch you.", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n fails to catch $N.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_hells_flash );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  return;
}

void do_blast_zone( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

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

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_bz]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_bz]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_bz]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_bz]->focus;

  int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_bz]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_bz ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 34, 46 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( z,
         "You put one arm up into the air, open palm facing up. There is a deafening sound and a blinding flash of light as you suddenly release an incredible amount of energy in a single blast that damages $N and reduces the area all around to ashes. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( z,
         "$n puts one arm up into the air, open palm facing up. There is a deafening sound and a blinding flash of light as $n suddenly releases an incredible amount of energy in a single blast that damages you and reduces the area all around to ashes. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( z,
         "$n puts one arm up into the air, open palm facing up. There is a deafening sound and a blinding flash of light as $n suddenly releases an incredible amount of energy in a single blast that damages $N and reduces the area all around to ashes. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_bz );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( z, "You missed $N with your blast zone.", ch, NULL, victim, TO_CHAR );
    act( z, "$n misses you with $s blast zone.", ch, NULL, victim, TO_VICT );
    act( z, "$n missed $N with a blast zone.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_bz );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_bz]->min_mana;
  return;
}

void do_enhance( CHAR_DATA * ch, char *argument )
{
  sh_int mod = 0;
  bool strength = FALSE;
  bool speed = FALSE;
  int energy = 0;

  if( IS_NPC( ch ) )
    return;

  bool virus = FALSE;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    virus = TRUE;

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_enhance]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( argument[0] == '\0' )
  {
    ch_printf( ch, "\n\rSyntax: enhance (stat)\n\r" );
    ch_printf( ch, "\n\rValid stats to enhance are:\n\r" );
    if( is_android_fm( ch ) )
      ch_printf( ch, "  strength" );
    if( is_android_h( ch ) )
      ch_printf( ch, "  speed" );

    ch_printf( ch, "\n\r" );
    return;
  }

  mod = 5;
  //energy = skill_table[gsn_enhance]->min_mana;
  energy = 300;

  if( ch->exp > 100000000 )
  {
    mod = 10;
    energy = 1000;
//    energy = skill_table[gsn_enhance]->min_mana * 3;
  }
  if( ch->exp > 5000000000ULL )
  {
    mod = 15;
    energy = 5000;
//    energy = skill_table[gsn_enhance]->min_mana * 6;
  }
  if( ch->exp > 100000000000ULL )
  {
    mod = 20;
    energy = 10000;
//    energy = skill_table[gsn_enhance]->min_mana * 12;
  }

  if( virus )
    mod *= 2;

  if( is_android_fm( ch ) )
    strength = TRUE;
  if( is_android_h( ch ) )
    speed = TRUE;

  if( !str_cmp( argument, "strength" ) )
  {
    if( !strength )
    {
      do_enhance( ch, "" );
      return;
    }
    if( ch->add_str > 0 )
    {
      ch->perm_str -= ch->add_str;
      ch->add_str = 0;
      ch_printf( ch, "\n\r&wYour strength returns to normal.&D\n\r" );
      save_char_obj( ch );
      return;
    }
    else
    {
      if( ch->mana < energy )
      {
        ch_printf( ch, "You don't have energy energy.\n\r" );
        return;
      }
      if( can_use_skill( ch, number_percent(  ), gsn_enhance ) )
      {
        ch->add_str = mod;
        ch->perm_str += mod;
        ch_printf( ch, "\n\r&RYou enhance your strength.\n\r" );
      }
      else
      {
        ch_printf( ch, "\n\r&gYou fail to enhance your strength.\n\r" );
      }
    }
    if( !is_android_h( ch ) )
      ch->mana -= energy;
    save_char_obj( ch );
    return;
  }

  if( !str_cmp( argument, "speed" ) )
  {
    if( !speed )
    {
      do_enhance( ch, "" );
      return;
    }
    if( ch->add_dex > 0 )
    {
      ch->perm_dex -= ch->add_dex;
      ch->add_dex = 0;
      ch_printf( ch, "\n\r&wYour speed returns to normal.&D\n\r" );
      save_char_obj( ch );
      return;
    }
    else
    {
      if( ch->mana < energy )
      {
        ch_printf( ch, "You don't have energy energy.\n\r" );
        return;
      }
      if( can_use_skill( ch, number_percent(  ), gsn_enhance ) )
      {
        ch->add_dex = mod;
        ch->perm_dex += mod;
        ch_printf( ch, "\n\r&BYou enhance your speed.\n\r" );
      }
      else
      {
        ch_printf( ch, "\n\r&gYou fail to enhance your speed.\n\r" );
      }
    }
    if( !is_android_h( ch ) )
      ch->mana -= energy;
    save_char_obj( ch );
    return;
  }

  do_enhance( ch, "" );
}

void do_ki_burst( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_ki_burst ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_ki_burst]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_ki_burst]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_ki_burst]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_ki_burst]->focus;

  //sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_ki_burst]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_ki_burst ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 65, 75 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( AT_PURPLE, "A colossal aura emerges as you convert the pure hatred for $N", ch, NULL, victim, TO_CHAR );
    act( AT_PURPLE, "accumulating in your soul into a final advancement. Clearly focused and", ch, NULL, victim, TO_CHAR );
    act( AT_PURPLE, "prepared, you course a vast amount of energy through your arms while extending", ch, NULL, victim,
         TO_CHAR );
    act( AT_PURPLE, "them outwards. Pumping a final burst of ki into the attack, a hulking sphere", ch, NULL, victim,
         TO_CHAR );
    act( AT_PURPLE, "forms into existence and radiates a boisterous crack as energy lashes into the", ch, NULL, victim,
         TO_CHAR );
    act( AT_PURPLE, "air. Without caution, the sphere progresses toward its destination at a", ch, NULL, victim, TO_CHAR );
    act( AT_PURPLE, "blinding speed and erupts as it confronts $N with a devastating blow. &W[$t]", ch, num_punct( dam ),
         victim, TO_CHAR );

    act( AT_PURPLE, "A colossal aura emerges as $n converts the pure hatred for you", ch, NULL, victim, TO_VICT );
    act( AT_PURPLE, "accumulating in $s soul into a final advancement. Clearly focused and", ch, NULL, victim, TO_VICT );
    act( AT_PURPLE, "prepared, $e courses a vast amount of energy through $s arms while extending", ch, NULL, victim,
         TO_VICT );
    act( AT_PURPLE, "them outwards. Pumping a final burst of ki into the attack, a hulking sphere", ch, NULL, victim,
         TO_VICT );
    act( AT_PURPLE, "forms into existence and radiates a boisterous crack as energy lashes into the", ch, NULL, victim,
         TO_VICT );
    act( AT_PURPLE, "air. Without caution, the sphere progresses toward its destination at a", ch, NULL, victim, TO_VICT );
    act( AT_PURPLE, "blinding speed and erupts as it confronts you with a devastating blow. &W[$t]", ch, num_punct( dam ),
         victim, TO_VICT );

    act( AT_PURPLE, "A colossal aura emerges as $n converts the pure hatred for $N", ch, NULL, victim, TO_NOTVICT );
    act( AT_PURPLE, "accumulating in $s soul into a final advancement. Clearly focused and", ch, NULL, victim, TO_NOTVICT );
    act( AT_PURPLE, "prepared, $e courses a vast amount of energy through $s arms while extending", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_PURPLE, "them outwards. Pumping a final burst of ki into the attack, a hulking sphere", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_PURPLE, "forms into existence and radiates a boisterous crack as energy lashes into the", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_PURPLE, "air. Without caution, the sphere progresses toward its destination at a", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_PURPLE, "blinding speed and erupts as it confronts $N with a devastating blow. &W[$t]", ch, num_punct( dam ),
         victim, TO_NOTVICT );

    learn_from_success( ch, gsn_ki_burst );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_PURPLE, "A colossal aura emerges as you convert the pure hatred for $N", ch, NULL, victim, TO_CHAR );
    act( AT_PURPLE, "accumulating in your soul into a final advancement. Clearly focused and", ch, NULL, victim, TO_CHAR );
    act( AT_PURPLE, "prepared, you course a vast amount of energy through your arms while extending", ch, NULL, victim,
         TO_CHAR );
    act( AT_PURPLE, "them outwards. Pumping a final burst of ki into the attack, a hulking sphere", ch, NULL, victim,
         TO_CHAR );
    act( AT_PURPLE, "forms into existence and radiates a boisterous crack as energy lashes into the", ch, NULL, victim,
         TO_CHAR );
    act( AT_PURPLE, "air. Without caution, the sphere progresses toward its destination but misses", ch, NULL, victim,
         TO_CHAR );
    act( AT_PURPLE, "and erupts after making contact with a nearby object.", ch, NULL, victim, TO_CHAR );

    act( AT_PURPLE, "A colossal aura emerges as $n converts the pure hatred for you", ch, NULL, victim, TO_VICT );
    act( AT_PURPLE, "accumulating in $s soul into a final advancement. Clearly focused and", ch, NULL, victim, TO_VICT );
    act( AT_PURPLE, "prepared, $e courses a vast amount of energy through $s arms while extending", ch, NULL, victim,
         TO_VICT );
    act( AT_PURPLE, "them outwards. Pumping a final burst of ki into the attack, a hulking sphere", ch, NULL, victim,
         TO_VICT );
    act( AT_PURPLE, "forms into existence and radiates a boisterous crack as energy lashes into the", ch, NULL, victim,
         TO_VICT );
    act( AT_PURPLE, "air. Without caution, the sphere progresses toward its destination but misses", ch, NULL, victim,
         TO_VICT );
    act( AT_PURPLE, "and erupts after making contact with a nearby object.", ch, NULL, victim, TO_VICT );

    act( AT_PURPLE, "A colossal aura emerges as $n converts the pure hatred for $N", ch, NULL, victim, TO_NOTVICT );
    act( AT_PURPLE, "accumulating in $s soul into a final advancement. Clearly focused and", ch, NULL, victim, TO_NOTVICT );
    act( AT_PURPLE, "prepared, $e courses a vast amount of energy through $s arms while extending", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_PURPLE, "them outwards. Pumping a final burst of ki into the attack, a hulking sphere", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_PURPLE, "forms into existence and radiates a boisterous crack as energy lashes into the", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_PURPLE, "air. Without caution, the sphere progresses toward its destination but misses", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_PURPLE, "and erupts after making contact with a nearby object.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_ki_burst );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_ki_burst]->min_mana;
  return;
}

void do_ssd_bomber( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

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

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_ssd_bomber]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_ssd_bomber]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_ssd_bomber]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_ssd_bomber]->focus;

  WAIT_STATE( ch, skill_table[gsn_ssd_bomber]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_ssd_bomber ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 55, 70 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_RED, "You clasp your hands forward, as if charging an "
         "enery ball.  Suddenly, it surges with incredible "
         "power, entangled in a spider-web-like sphere of "
         "energy.  You push your hands forward, sending the "
         "'Deadly Bomb' after $N which eventually causes a "
         "tremendous explosion, giving its name credit. &W[$t]", ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_RED, "$n clasps $s hands forward, as if charging an "
         "enery ball.  Suddenly, it surges with incredible "
         "power, entangled in a spider-web-like sphere of "
         "energy.  $n pushes $s hands forward, sending the "
         "'Deadly Bomb' after you which eventually causes a "
         "tremendous explosion, giving its name credit. &W[$t]", ch, num_punct( dam ), victim, TO_VICT );
    act( AT_RED, "$n clasps $s hands forward, as if charging an "
         "enery ball.  Suddenly, it surges with incredible "
         "power, entangled in a spider-web-like sphere of "
         "energy.  $n pushes $s hands forward, sending the "
         "'Deadly Bomb' after $N which eventually causes a "
         "tremendous explosion, giving its name credit. &W[$t] ", ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_ssd_bomber );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_RED, "You missed $N with your ssd bomber attack.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n misses you with $s ssd bomber attack.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n missed $N with a ssd bomber attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_ssd_bomber );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_ssd_bomber]->min_mana;
  return;
}

/*********************************************
 *    Android Chip Integration(ACII) V.1.7   * 
 *                  By: Karn                 *
 *                                           *
 * Project ACII was started in DBNA 1.5 and  *
 * was finished in DBNA 1.7. this allows     *
 * androids to break away from the normal    *
 * soft-coded android chips versions and     *
 * makes the chips hardcoded. Chips covered  *
 * in this are: T-1, T-2, T-3, T-4, OMEGA,   *
 * VM-1, VM-2, and S-X. Chips -NOT- covered  *
 * in this version are SentiChip, X-1,       *
 * and Crystal Processor. Hopefully it will  *
 * be added in version 2 or something like   *
 * that. I enjoyed coding this!              *
 *                                           *
 * Edit: Senti Chip Added by Sigma           *
 * Edit: Sigma Virus added by Sigma          *
 *                                           *
 *********************************************/
void do_integrate( CHAR_DATA * ch, char *argument )
{
  OBJ_DATA *ro;

/*
    if( !IS_IMMORTAL(ch) )
    {
	send_to_char("Test Command. Immortals only.\n\r", ch);
	return;
    }
*/
  
  if( !is_android( ch ) && !is_superandroid( ch ) )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }
  if( wearing_sentient_chip( ch ) && !is_android( ch ) && !is_superandroid( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( !argument || argument[0] == '\0' )
  {
    send_to_char( "Syntax: integrate [ T4 | T3 | T2 | T1 | OMEGA | VM1 | VM2 | SUP | SENTIENT ]\n\r", ch );
    return;
  }

  if( !str_prefix( argument, "T4" ) )
  {
    if( !wearing_t4( ch ) )
    {
      send_to_char( "You must be wearing the T-4 Chip.", ch );
      return;
    }
    if( ch->pcdata->learned[gsn_t4] == 100 )
    {
      send_to_char( "You have already absorbed this power! Use it well!\n\r", ch );
      return;
    }
    if( ( ( ro = wearing_t4( ch ) ) && ch->pcdata->learned[gsn_t4] == 0 ) )
    {
      ch->pcdata->learned[gsn_t4] = 100;
      unequip_char( ch, ro );
      extract_obj( ro );
      send_to_char( "&cInstalling status:&R Complete!&D\n\r", ch );
      return;
    }
  }
  if( !str_prefix( argument, "T3" ) )
  {
    if( !wearing_t3( ch ) )
    {
      send_to_char( "You must be wearing the T-3 Chip.", ch );
      return;
    }
    if( ch->pcdata->learned[gsn_t3] == 100 )
    {
      send_to_char( "You have already absorbed this power! Use it well!\n\r", ch );
      return;
    }
    if( ( ( ro = wearing_t3( ch ) ) && ch->pcdata->learned[gsn_t3] == 0 ) )
    {
      ch->pcdata->learned[gsn_t3] = 100;
      unequip_char( ch, ro );
      extract_obj( ro );
      send_to_char( "&cInstalling status:&R Complete!&D\n\r", ch );
      return;
    }
  }
  if( !str_prefix( argument, "T2" ) )
  {
    if( !wearing_t2( ch ) )
    {
      send_to_char( "You must be wearing the T-2 Chip.", ch );
      return;
    }
    if( ch->pcdata->learned[gsn_t2] == 100 )
    {
      send_to_char( "You have already absorbed this power! Use it well!\n\r", ch );
      return;
    }
    if( ( ( ro = wearing_t2( ch ) ) && ch->pcdata->learned[gsn_t2] == 0 ) )
    {
      ch->pcdata->learned[gsn_t2] = 100;
      unequip_char( ch, ro );
      extract_obj( ro );
      send_to_char( "&cInstalling status:&R Complete!&D\n\r", ch );
      return;
    }
  }
  if( !str_prefix( argument, "T1" ) )
  {
    if( !wearing_t1( ch ) )
    {
      send_to_char( "You must be wearing the T-1 Chip.", ch );
      return;
    }
    if( ch->pcdata->learned[gsn_t1] == 100 )
    {
      send_to_char( "You have already absorbed this power! Use it well!\n\r", ch );
      return;
    }
    if( ( ( ro = wearing_t1( ch ) ) && ch->pcdata->learned[gsn_t1] == 0 ) )
    {
      ch->pcdata->learned[gsn_t1] = 100;
      unequip_char( ch, ro );
      extract_obj( ro );
      send_to_char( "&cInstalling status:&R Complete!&D\n\r", ch );
      return;
    }
  }
  if( !str_prefix( argument, "OMEGA" ) )
  {
    if( !wearing_omega( ch ) )
    {
      send_to_char( "You must be wearing the Omega Chip.", ch );
      return;
    }
    if( ch->pcdata->learned[gsn_omega] == 100 )
    {
      send_to_char( "You have already absorbed this power! Use it well!\n\r", ch );
      return;
    }
    if( ( ( ro = wearing_omega( ch ) ) && ch->pcdata->learned[gsn_omega] == 0 ) )
    {
      ch->pcdata->learned[gsn_omega] = 100;
      unequip_char( ch, ro );
      extract_obj( ro );
      send_to_char( "&cInstalling status:&R Complete!&D\n\r", ch );
      return;
    }
  }
  if( !str_prefix( argument, "VM1" ) )
  {
    if( !wearing_vm1( ch ) )
    {
      send_to_char( "You must be wearing the VM-1 Chip.", ch );
      return;
    }
    if( ch->pcdata->learned[gsn_vm1] == 100 )
    {
      send_to_char( "You have already absorbed this power! Use it well!\n\r", ch );
      return;
    }
    if( ( ( ro = wearing_vm1( ch ) ) && ch->pcdata->learned[gsn_vm1] == 0 ) )
    {
      ch->pcdata->learned[gsn_vm1] = 100;
      unequip_char( ch, ro );
      extract_obj( ro );
      send_to_char( "&cInstalling status:&R Complete!&D\n\r", ch );
      return;
    }
  }
  if( !str_prefix( argument, "VM2" ) )
  {
    if( !wearing_vm2( ch ) )
    {
      send_to_char( "You must be wearing the VM-2 Chip.", ch );
      return;
    }
    if( ch->pcdata->learned[gsn_vm2] == 100 )
    {
      send_to_char( "You have already absorbed this power! Use it well!\n\r", ch );
      return;
    }
    if( ( ( ro = wearing_vm2( ch ) ) && ch->pcdata->learned[gsn_vm2] == 0 ) )
    {
      ch->pcdata->learned[gsn_vm2] = 100;
      unequip_char( ch, ro );
      extract_obj( ro );
      send_to_char( "&cInstalling status:&R Complete!&D\n\r", ch );
      return;
    }
  }
  if( !str_prefix( argument, "sup" ) )
  {
    if( !wearing_asuppress( ch ) )
    {
      send_to_char( "You must be wearing the Suppression Chip.", ch );
      return;
    }
    if( ch->pcdata->learned[gsn_suppress] == 100 )
    {
      send_to_char( "You have already absorbed this power! Use it well!\n\r", ch );
      return;
    }
    if( ( ( ro = wearing_asuppress( ch ) ) && ch->pcdata->learned[gsn_suppress] == 0 ) )
    {
      ch->pcdata->learned[gsn_suppress] = 100;
      unequip_char( ch, ro );
      extract_obj( ro );
      send_to_char( "&cInstalling status:&R Complete!&D\n\r", ch );
      return;
    }
  }
  if( !str_prefix( argument, "sent" ) )
  {
    if( !wearing_sentient_chip( ch ) )
    {
      send_to_char( "You must be wearing the sentient chip first.", ch );
      return;
    }
    if( ch->pcdata->learned[gsn_sent_chip] == 100 )
    {
      send_to_char( "&zYou already have the device installed.\n\r", ch );
      return;
    }
    if( ( ro = wearing_sentient_chip2( ch ) ) )
    {
      ch->pcdata->learned[gsn_sent_chip] = 100;
      unequip_char( ch, ro );
      extract_obj( ro );
      send_to_char( "&cInstalling status:&R Complete!&D\n\r", ch );
      return;
    }
    ch_printf( ch, "Error.\n\r" );
    return;
  }

  do_integrate( ch, "" );
  return;
}

OBJ_DATA *wearing_t4( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_SHIELD )
    {
      if( obj->pIndexData->vnum == 2303 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_t3( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_SHIELD )
    {
      if( obj->pIndexData->vnum == 2304 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_t2( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_SHIELD )
    {
      if( obj->pIndexData->vnum == 2305 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_t1( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_SHIELD )
    {
      if( obj->pIndexData->vnum == 2306 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_omega( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_SHIELD )
    {
      if( obj->pIndexData->vnum == 2313 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_vm1( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_SHIELD )
    {
      if( obj->pIndexData->vnum == 100701 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_vm2( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_SHIELD )
    {
      if( obj->pIndexData->vnum == 100704 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_asuppress( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_SHIELD )
    {
      if( obj->pIndexData->vnum == 2311 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_sigma_virus( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_SHIELD )
    {
      if( obj->pIndexData->vnum == 50101 )
        return obj;
    }
  }
  return NULL;
}

void do_t4( CHAR_DATA * ch, char *argument )
{
  sh_int z = get_aura( ch );
  int mod = 4;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    mod = 6;

  if( xIS_SET( ( ch )->affected_by, AFF_SUPERANDROID ) )
  {
    send_to_char( "You must powerdown first.\n\r", ch );
    return;
  }

  if( IS_AFFECTED( ch, AFF_T4 ) )
  {
    ch_printf( ch, "&wYou return to normal.\n\r" );
    act( AT_GREY, "&w$n returns to normal.", ch, NULL, NULL, TO_ROOM );
    removeDroidTrans( ch );
    ch->pl = ch->exp;
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this.\n\r" );
    return;
  }
  if( ch->exp < skill_table[gsn_t4]->min_level[ch->race] )
  {
    send_to_char( "You can't do that at your level.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_t4]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_t4]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_t4 ) )
  {
    removeDroidTrans( ch );
    act( z, "As the chip is activated you feel an incredible amount of energy", ch, NULL, NULL, TO_CHAR );
    act( z, "flow through your circuitry enhancing your hardware and boosting your aura", ch, NULL, NULL, TO_CHAR );
    act( z, "in size and power.", ch, NULL, NULL, TO_CHAR );

    act( z, "$n's aura begins to boost in size and power.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ch->affected_by, AFF_T4 );
    ch->pl = ch->exp * mod;
    transStatApply( ch, mod, mod, mod, mod );
    learn_from_success( ch, gsn_t4 );
  }
  else
  {
    act( AT_PLAIN, "You failed.", ch, NULL, NULL, TO_CHAR );
    learn_from_failure( ch, gsn_t4 );
  }

  ch->mana -= skill_table[gsn_t4]->min_mana;
  return;
}

void do_t3( CHAR_DATA * ch, char *argument )
{
  sh_int z = get_aura( ch );
  int mod = 8;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    mod = 10;

  if( xIS_SET( ( ch )->affected_by, AFF_SUPERANDROID ) )
  {
    send_to_char( "You must powerdown first.\n\r", ch );
    return;
  }

  if( IS_AFFECTED( ch, AFF_T3 ) )
  {
    ch_printf( ch, "&wYou return to normal.\n\r" );
    act( AT_GREY, "&w$n returns to normal.", ch, NULL, NULL, TO_ROOM );
    removeDroidTrans( ch );
    ch->pl = ch->exp;
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }
  if( ch->exp < skill_table[gsn_t3]->min_level[ch->race] )
  {
    send_to_char( "You can't do that at your level.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_t3]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_t3]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_t3 ) )
  {
    removeDroidTrans( ch );
    act( z, "As the chip's programs surge through your internal processors, power", ch, NULL, NULL, TO_CHAR );
    act( z, "floods through your systems, heightening your power, physical, and mental", ch, NULL, NULL, TO_CHAR );
    act( z, "abilities.", ch, NULL, NULL, TO_CHAR );

    act( z, "$n's aura fades slightly and their energy seems to become stronger.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ch->affected_by, AFF_T3 );
    ch->pl = ch->exp * mod;
    transStatApply( ch, mod, mod, mod, mod );
    learn_from_success( ch, gsn_t3 );
  }
  else
  {
    act( AT_PLAIN, "You failed.", ch, NULL, NULL, TO_CHAR );
    learn_from_failure( ch, gsn_t3 );
  }

  ch->mana -= skill_table[gsn_t3]->min_mana;
  return;
}

void do_t2( CHAR_DATA * ch, char *argument )
{
  sh_int z = get_aura( ch );
  int mod = 12;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    mod = 14;

  if( xIS_SET( ( ch )->affected_by, AFF_SUPERANDROID ) )
  {
    send_to_char( "You must powerdown first.\n\r", ch );
    return;
  }

  if( IS_AFFECTED( ch, AFF_T2 ) )
  {
    ch_printf( ch, "&wYou return to normal.\n\r" );
    act( AT_GREY, "&w$n returns to normal.", ch, NULL, NULL, TO_ROOM );
    removeDroidTrans( ch );
    ch->pl = ch->exp;
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }
  if( ch->exp < skill_table[gsn_t2]->min_level[ch->race] )
  {
    send_to_char( "You can't do that at your level.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_t2]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_t2]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_t2 ) )
  {
    removeDroidTrans( ch );
    act( z, "As power floods through your internal power core, your abilities are", ch, NULL, NULL, TO_CHAR );
    act( z, "suddenly heightened as the chip's programming is accepted into your", ch, NULL, NULL, TO_CHAR );
    act( z, "computer core.  A flash of power surges through the room as a aura of", ch, NULL, NULL, TO_CHAR );
    act( z, "electricity flows around you.", ch, NULL, NULL, TO_CHAR );

    act( z, "$n's aura begins to flow throughout the room surrounded in electricity.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ch->affected_by, AFF_T2 );
    ch->pl = ch->exp * mod;
    transStatApply( ch, mod, mod, mod, mod );
    learn_from_success( ch, gsn_t2 );
  }
  else
  {
    act( AT_PLAIN, "You failed.", ch, NULL, NULL, TO_CHAR );
    learn_from_failure( ch, gsn_t2 );
  }

  ch->mana -= skill_table[gsn_t2]->min_mana;
  return;
}

void do_t1( CHAR_DATA * ch, char *argument )
{
  int mod = 16;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    mod = 18;

  if( xIS_SET( ( ch )->affected_by, AFF_SUPERANDROID ) )
  {
    send_to_char( "You must powerdown first.\n\r", ch );
    return;
  }

  if( IS_AFFECTED( ch, AFF_T1 ) )
  {
    ch_printf( ch, "&wYou return to normal.\n\r" );
    act( AT_GREY, "&w$n returns to normal.", ch, NULL, NULL, TO_ROOM );
    removeDroidTrans( ch );
    ch->pl = ch->exp;
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }
  if( ch->exp < skill_table[gsn_t1]->min_level[ch->race] )
  {
    send_to_char( "You can't do that at your level.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_t1]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_t1]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_t1 ) )
  {
    removeDroidTrans( ch );
    act( AT_RED, "As the chip’s programming surges through your core, a blast of red energy flows", ch, NULL, NULL,
         TO_CHAR );
    act( AT_RED, "from your body and begins to swirl as your components surge with raw energy. A", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "vigorous glow emerges from your eyes as you experience a newly acquired power", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "that has been heightened from the upgrade.", ch, NULL, NULL, TO_CHAR );

    act( AT_RED, "Energy flares up and swirls around $n's body suddenly, forming a vortex of", ch, NULL, NULL, TO_NOTVICT );
    act( AT_RED, "crimson energy which hides $m from view.  Seconds later, the maelstrom", ch, NULL, NULL, TO_NOTVICT );
    act( AT_RED, "dissipates and $n becomes visible, eyes glowing with red light.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ch->affected_by, AFF_T1 );
    ch->pl = ch->exp * mod;
    transStatApply( ch, mod, mod, mod, mod );
    learn_from_success( ch, gsn_t1 );
  }
  else
  {
    act( AT_PLAIN, "You failed.", ch, NULL, NULL, TO_CHAR );
    learn_from_failure( ch, gsn_t1 );
  }

  ch->mana -= skill_table[gsn_t1]->min_mana;
  return;
}

void do_omega( CHAR_DATA * ch, char *argument )
{
  sh_int z = get_aura( ch );
  int mod = 20;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    mod = 22;

  if( xIS_SET( ( ch )->affected_by, AFF_SUPERANDROID ) )
  {
    send_to_char( "You must powerdown first.\n\r", ch );
    return;
  }

  if( IS_AFFECTED( ch, AFF_OMEGA ) )
  {
    ch_printf( ch, "&wYou return to normal.\n\r" );
    act( AT_GREY, "&w$n returns to normal.", ch, NULL, NULL, TO_ROOM );
    removeDroidTrans( ch );
    ch->pl = ch->exp;
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }
  if( ch->exp < skill_table[gsn_omega]->min_level[ch->race] )
  {
    send_to_char( "You can't do that at your level.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_omega]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_omega]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_omega ) )
  {
    removeDroidTrans( ch );
    act( z, "As the chip's programming surges through your systems, your internal", ch, NULL, NULL, TO_CHAR );
    act( z, "circuitry surge with heightened power, and a blast of power flushs through the", ch, NULL, NULL, TO_CHAR );
    act( z, "area as an aura envelopes you.", ch, NULL, NULL, TO_CHAR );

    act( z, "$n's aura bursts forth from them covering the area then shrouds them in", ch, NULL, NULL, TO_NOTVICT );
    act( z, "a mist of pure energy.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ch->affected_by, AFF_OMEGA );
    ch->pl = ch->exp * mod;
    transStatApply( ch, mod, mod, mod, mod );
    learn_from_success( ch, gsn_omega );
  }
  else
  {
    act( AT_PLAIN, "You failed.", ch, NULL, NULL, TO_CHAR );
    learn_from_failure( ch, gsn_omega );
  }

  ch->mana -= skill_table[gsn_omega]->min_mana;
  return;
}

void do_vm1( CHAR_DATA * ch, char *argument )
{
  sh_int z = get_aura( ch );
  int mod = 14;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    mod = 16;

  if( xIS_SET( ( ch )->affected_by, AFF_SUPERANDROID ) )
  {
    send_to_char( "You must powerdown first.\n\r", ch );
    return;
  }

  if( IS_AFFECTED( ch, AFF_VM1 ) )
  {
    ch_printf( ch, "&wYou return to normal.\n\r" );
    act( AT_GREY, "&w$n returns to normal.", ch, NULL, NULL, TO_ROOM );
    removeDroidTrans( ch );
    ch->pl = ch->exp;
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }
  if( ch->exp < skill_table[gsn_vm1]->min_level[ch->race] )
  {
    send_to_char( "You can't do that at your level.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_vm1]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_vm1]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_vm1 ) )
  {
    removeDroidTrans( ch );
    act( z, "As your systems assimilate the knowledge of VM-1, your eyes glow briefly", ch, NULL, NULL, TO_CHAR );
    act( z, "with purple energy, and your systems rush with power, heightening your", ch, NULL, NULL, TO_CHAR );
    act( z, "physical and mental abilities higher then your basic programming was made", ch, NULL, NULL, TO_CHAR );
    act( z, "for.", ch, NULL, NULL, TO_CHAR );

    act( z, "$n's eyes glow purple for a second as $e accesses their deepest power.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ch->affected_by, AFF_VM1 );
    ch->pl = ch->exp * mod;
    transStatApply( ch, mod, mod, mod, mod );
    learn_from_success( ch, gsn_vm1 );
  }
  else
  {
    act( AT_PLAIN, "You failed.", ch, NULL, NULL, TO_CHAR );
    learn_from_failure( ch, gsn_vm1 );
  }

  ch->mana -= skill_table[gsn_vm1]->min_mana;
  return;
}

void do_vm2( CHAR_DATA * ch, char *argument )
{
  sh_int z = get_aura( ch );
  int mod = 22;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    mod = 24;

  if( xIS_SET( ( ch )->affected_by, AFF_SUPERANDROID ) )
  {
    send_to_char( "You must powerdown first.\n\r", ch );
    return;
  }

  if( IS_AFFECTED( ch, AFF_VM2 ) )
  {
    ch_printf( ch, "&wYou return to normal.\n\r" );
    act( AT_GREY, "&w$n returns to normal.", ch, NULL, NULL, TO_ROOM );
    removeDroidTrans( ch );
    ch->pl = ch->exp;
    return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }
  if( ch->exp < skill_table[gsn_vm2]->min_level[ch->race] )
  {
    send_to_char( "You can't do that at your level.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_vm2]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_vm2]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_vm2 ) )
  {
    removeDroidTrans( ch );
    act( z, "As the programming kicks in, the VM programming sends out bolts of", ch, NULL, NULL, TO_CHAR );
    act( z, "purple electricity from your body, swirling around you, licking at your", ch, NULL, NULL, TO_CHAR );
    act( z, "frame.  A large amount of energy surges through all of your systems,", ch, NULL, NULL, TO_CHAR );
    act( z, "raising them to a height that almost equals a super android. ,", ch, NULL, NULL, TO_CHAR );

    act( z, "$n's begins to send bolts of purple power from $e body.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ch->affected_by, AFF_VM2 );
    ch->pl = ch->exp * mod;
    transStatApply( ch, mod, mod, mod, mod );
    learn_from_success( ch, gsn_vm2 );
  }
  else
  {
    act( AT_PLAIN, "You failed.", ch, NULL, NULL, TO_CHAR );
    learn_from_failure( ch, gsn_vm2 );
  }

  ch->mana -= skill_table[gsn_vm2]->min_mana;
  return;
}

/*     This is the code for -THE- Suppression Chip. -- Karn 04.01.05
void do_android_suppress( CHAR_DATA *ch, char *argument )
{
	if( wearing_sentient_chip(ch) )
	{
	    ch_printf(ch,"You can't use this.");
	    return;
	}
	if( ch->exp < skill_table[gsn_suppress]->min_level[ch->race] )
	{
	    send_to_char( "You can't do that at your level.\n\r", ch );
	    return;
	}

	if( ch->mana < skill_table[gsn_suppress]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}

	WAIT_STATE( ch, skill_table[gsn_suppress]->beats );

	if ( can_use_skill( ch, number_percent(), gsn_suppress ) )
	{
	switch (number_range(1,100))
	{
    case 100, 90, 80, 70, 60, 50, 40, 30, 20, 10:
	    removeDroidTrans( ch );
	    act( z, "Your system begins to slooooow down....(x30)", ch, NULL, NULL, TO_CHAR );

	    act( z, "$n's body begins to slow in it's motions.", ch, NULL, NULL, TO_NOTVICT );
	    ch->pl = ch->exp / 30;
	    learn_from_success( ch, gsn_suppress );
        break;
	}
    case 95, 80, 70, 60, 50, 40, 30, 20, 10:
	    removeDroidTrans( ch );
	    act( z, "Your system begins to slooooow down....(x10)", ch, NULL, NULL, TO_CHAR );

	    act( z, "$n's body begins to slow in it's motions.", ch, NULL, NULL, TO_NOTVICT );
	    ch->pl = ch->exp / 10;
	    learn_from_success( ch, gsn_suppress );
        break;
	}
    default:
	    removeDroidTrans( ch );
	    act( z, "Your system begins to slooooow down....(x3)", ch, NULL, NULL, TO_CHAR );

	    act( z, "$n's body begins to slow in it's motions.", ch, NULL, NULL, TO_NOTVICT );
	    ch->pl = ch->exp / 3;
	    learn_from_success( ch, gsn_asuppress );
        break;
	}
	else
	{
	    act( AT_PLAIN, "You failed.", ch, NULL, NULL, TO_CHAR );
	    learn_from_failure( ch, gsn_asuppress );
	}

	ch->mana -= skill_table[gsn_asuppress]->min_mana;
	return;
}*/

void do_sent_chip( CHAR_DATA * ch, char *argument )
{
  sh_int z = get_aura( ch );
  int extra = 0;
  if( !IS_NPC( ch ) && xIS_SET( ch->affected_by, AFF_SIGMA_VIRUS ) )
    extra = 2;

  if( xIS_SET( ( ch )->affected_by, AFF_SUPERANDROID ) )
  {
    send_to_char( "You must powerdown first.\n\r", ch );
    return;
  }

  if( IS_AFFECTED( ch, AFF_SENT_CHIP ) )
  {
    ch_printf( ch, "&wYou return to normal.\n\r" );
    act( AT_GREY, "&w$n returns to normal.", ch, NULL, NULL, TO_ROOM );
    removeDroidTrans( ch );
    ch->pl = ch->exp;
    return;
  }

  if( ch->exp < skill_table[gsn_sent_chip]->min_level[ch->race] )
  {
    send_to_char( "You can't do that at your level.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_sent_chip]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_sent_chip]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_sent_chip ) )
  {
    removeDroidTrans( ch );
    act( z,
         "&zYour vision blurs and shifts as your entire body begins to transform. Electricity begins to race up and down your body as your artificial skin and armoring melts away, replaced by a flawless sheet of chrome-looking metal. Your powerlevel surges as you complete the transformation into a Meta-Android.",
         ch, NULL, NULL, TO_CHAR );
    act( z,
         "&zElectricity begins to race up and down $n's body as $e begins to transform. $s artificial skin and armoring melts away, replaced by a flawless sheet of chrome-looking metal; becoming a Meta-Android.",
         ch, NULL, NULL, TO_ROOM );
    xSET_BIT( ch->affected_by, AFF_SENT_CHIP );
    ch->pl = ch->exp * ( 22 + extra );
    if( extra == 0 )
      transStatApply( ch, 25, 30, 30, 25 );
    else
      transStatApply( ch, 30, 40, 40, 30 );
    learn_from_success( ch, gsn_sent_chip );
  }
  else
  {
    act( AT_PLAIN, "You failed.", ch, NULL, NULL, TO_CHAR );
    learn_from_failure( ch, gsn_sent_chip );
  }

  ch->mana -= skill_table[gsn_sent_chip]->min_mana;
  return;
}

void do_satellite_jump(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
	CHAR_DATA *fch;
	CHAR_DATA *fch_next;
	ROOM_INDEX_DATA *prev_room;

    if( !str_cmp(get_race(ch),"kaio") )
    {
	if ( ch->mana < (skill_table[gsn_satellite_jump]->min_mana / 5) )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
    }
    else
    {
        if ( ch->mana < skill_table[gsn_satellite_jump]->min_mana )
        {
            send_to_char( "You don't have enough energy.\n\r", ch );
            return;
        }
    }

    if( IS_NPC(ch) )
        return;

    if( ch->fighting )
    {
        send_to_char( "You cannot program the satellites, your processor is too busy.\n\r", ch );
        return;
    }

    if( xIS_SET(ch->in_room->room_flags, ROOM_ASTRALSHIELD) )
    {
	ch_printf(ch,"&RThis room is blocking your ability to de-materialize.\n\r");
	return;
    }

    WAIT_STATE( ch, skill_table[gsn_satellite_jump]->beats );

    if ( (victim = get_char_ki_world(ch, argument)) == NULL
    ||   !can_astral(ch, victim)
	||   !in_hard_range(ch, victim->in_room->area)
	||   !in_soft_range(ch, victim->in_room->area)
	||	 xIS_SET(victim->in_room->room_flags, ROOM_PROTOTYPE )
	||   IS_IMMORTAL(victim))
    {
	    send_to_char( "You cannot locate them with the satellites.\n\r", ch );
            return;
    }

	if( is_split(victim) )
        {
            send_to_char( "You cannot loacate them with the satellites.\n\r", ch );
            return;
        }

	if ( !IS_NPC(victim) && !victim->desc )
	{
		send_to_char("You cannot satellite jump to someone who has lost link.\n\r",ch);
		return;
	}

	if( !victim->in_room )
	{
		ch_printf(ch,"You can't satellite jump to them at the moment.\n\r");
		return;
	}

	if( ch->in_room == victim->in_room )
	{
	  ch_printf(ch,"You're already there!\n\r");
	  return;
	}
	if( victim->master == ch )
	{
	  ch_printf(ch,"You can't satellite jump to someone that is following you!\n\r");
	  return;
	}

	if ( can_use_skill(ch, number_percent(), gsn_satellite_jump ) )
	{
	learn_from_success( ch, gsn_satellite_jump );
	act( AT_MAGIC, "Your body dematerializes!", ch, NULL, NULL, TO_CHAR );
	act( AT_MAGIC, "$n's body dematerializes!", ch, NULL, NULL, TO_ROOM );
    prev_room = ch->in_room;
    char_from_room( ch );
    if ( ch->mount )
    {
		char_from_room( ch->mount );
		char_to_room( ch->mount, victim->in_room );
    }
    char_to_room( ch, victim->in_room );
    for ( fch = prev_room->first_person; fch; fch = fch_next )
    {
	fch_next = fch->next_in_room;
	if ( ( fch != ch ) && fch->master && fch->master == ch )
        {
		char_from_room (fch);
		if( !is_leet(fch) )
		{
	    	  act( AT_ACTION, "You send a signal to a nearby satellite!", fch, NULL, ch, TO_CHAR );
	    	  act( AT_ACTION, "Your body turns into data as you are transported to it!", fch, NULL, ch, TO_CHAR );
	    	  act( AT_ACTION, "$n's body dematerializes!", fch, NULL, ch, TO_ROOM );
		}
		else
		{
		 act( AT_ACTION, "You haxed a satellite! Omigawd u hax0r!",fch, NULL, ch, TO_CHAR );
               act( AT_ACTION, "$n's body dematerializes! OMGHAX!!!",fch, NULL, ch, TO_ROOM );
		}
		char_to_room( fch, victim->in_room );
		act( AT_MAGIC, "You program the satellite to send you to your destination!", fch, NULL, ch, TO_CHAR );
	    	act( AT_ACTION, "Your body rematerializes!", fch, NULL, ch, TO_CHAR );
		act( AT_MAGIC, "$n materializes in front of you!", fch, NULL, ch, TO_ROOM );
		do_look( fch, "auto" );
        }
    }
    act( AT_MAGIC, "$n rematerializes in front of you!", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
  }
  else
  {
    learn_from_failure( ch, gsn_satellite_jump );
    send_to_char( "&BYou cannot locate them with the Satellites.\n\r", ch );
  }
  return;

}

void do_biodrain(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_biodrain ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_biodrain]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }
      if( is_android( victim ) || is_superandroid( victim ) || is_reploid( victim ) )
      {
        send_to_char( "You can't drain lifeforce from the soulless.\n\r", ch );
        return;
      }
	if ( ch->mana < skill_table[gsn_biodrain]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_biodrain]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_biodrain]->focus;

    WAIT_STATE( ch, skill_table[gsn_biodrain]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_biodrain ) )
    {
	dam = get_attmod(ch, victim) * number_range( 50, 65 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_LBLUE, "&RYou stab $N in the arm with your tail and attempt to drain some of $N's lifeforce.", ch, NULL, victim, TO_CHAR );
	act( AT_LBLUE, "&RAs you draw $N's lifeforce into your body, you let out a sound of exhileration. [$t] ", ch, num_punct( dam ), victim, TO_CHAR );
	act( AT_LBLUE, "&R$n stabs you in the arm with $s tail and attempts to drain your lifeforce.", ch, NULL, victim, TO_VICT );
	act( AT_LBLUE, "&RAs $e draws your lifeforce into $s body, $e lets out a sound of exhileration. [$t] ", ch, num_punct( dam ), victim, TO_VICT );
	act( AT_LBLUE, "&R$n stabs $N in the arm with $s tail and attempt to drain some of $N's lifeforce.", ch, NULL, victim, TO_NOTVICT );
	act( AT_LBLUE, "&RAs $n draws $N's lifeforce into $s body, $e let out a sound of exhileration. [$t] ", ch, num_punct( dam ), victim, TO_NOTVICT );

        dam = ki_absorb( victim, ch, dam, gsn_biodrain );
	learn_from_success( ch, gsn_biodrain );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
       if( ( victim = who_fighting( ch ) ) != NULL )
       {
         ch->hit += dam / 2;
         if( ch->hit > ch->max_hit )
         {
           ch->hit = ch->max_hit;
         }
       }
    }
    else
    {
	act( AT_RED, "Your tail stab misses $N.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n misses you with $s tail.", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$n missed $N with a tail stab.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_biodrain );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_biodrain]->min_mana;
    return;
}


