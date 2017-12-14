#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"


void do_tuffle_blow( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  CHAR_DATA *victim;
  int dam = 0;
  /*
   * sh_int nvict = 0; 
   */
  sh_int nhit = 0;
  sh_int percent;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_tuffle_blow ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_tuffle_blow]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ( float )ch->exp / victim->exp > 5 && !IS_HC( victim ) )
  {
    ch_printf( ch, "You are more than 5 times stronger.\n\r" );
    return;
  }

  if( xIS_SET( victim->act, PLR_PK1 ) || xIS_SET( victim->act, PLR_WAR1 ) )
  {
    if( ( float )ch->exp / victim->exp > 5 )
    {
      send_to_char( "They have a yellow PK flag and you are more than 5 times stronger.\n\r", ch );

      if( !xIS_SET( ch->act, PLR_PK2 ) && !IS_HC( ch ) )
        xSET_BIT( ch->act, PLR_PK1 );
      return;
    }
  }
  if( ( float )ch->exp / victim->exp > 5 )
  {
    ch_printf( ch, "You are more than 5 times stronger.\n\r" );
    return;
  }

  if( ch->mana < skill_table[gsn_tuffle_blow]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_tuffle_blow]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }

  percent = LEARNED( ch, gsn_tuffle_blow );
  for( vch = ch->in_room->first_person; vch; vch = vch_next )
  {
    vch_next = vch->next_in_room;
    if( is_same_group( ch, vch ) || !can_see( ch, vch ) || is_safe( ch, vch, TRUE ) )
      continue;
    check_illegal_pk( ch, vch );
    if( chance( ch, percent ) )
    {
      nhit++;
      WAIT_STATE( ch, skill_table[gsn_tuffle_blow]->beats );
      if( can_use_skill( ch, number_percent(  ), gsn_tuffle_blow ) )
      {
        dam = get_attmod( ch, vch ) * number_range( 3, 4 );
        if( ch->charge > 0 )
          dam = chargeDamMult( ch, dam );
        act( AT_RED, "You sneak a hard blow in, hitting $N. &W[$t]", ch, num_punct( dam ), vch, TO_CHAR );
        act( AT_RED, "$n sneaks a hard blow in, hitting you. &W[$t]", ch, num_punct( dam ), vch, TO_VICT );
        act( AT_RED, "$n sneaks a hard blow in, hitting $N &W[$t]", ch, num_punct( dam ), vch, TO_NOTVICT );

        dam = ki_absorb( vch, ch, dam, gsn_tuffle_blow );
        global_retcode = damage( ch, vch, dam, TYPE_HIT );
        if( global_retcode == rCHAR_DIED || global_retcode == rBOTH_DIED || char_died( ch ) )
          return;
      }
    }
  }
  if( nhit )
    learn_from_success( ch, gsn_tuffle_blow );
  else
    learn_from_failure( ch, gsn_tuffle_blow );
  return;
}

void do_wff( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_wff]->skill_level[ch->class] )
  {
    send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_wff]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_wff]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_wff]->focus;

  WAIT_STATE( ch, skill_table[gsn_wff]->beats );

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_wff]->min_mana;

  if( can_use_skill( ch, number_percent(  ), gsn_wff ) )
  {
    act( AT_SKILL, "&rCrouching down with your right leg in front, you retain all your focus on", ch, NULL, victim,
         TO_CHAR );
    act( AT_SKILL, "&r$N. Without warning, you burst into a charge and call out &R'Wolf Fang", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "&RFist'. &rAn impression of a savage wolf emerges from your clasping palms", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "&rand howls out with fury as your sprint increases. As you slam into", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "&r$N, the brutal images dematerializes and reveals your fist lodged ", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "&rdeep within $S abdomen.", ch, NULL, victim, TO_CHAR );

    act( AT_SKILL, "&rCrouching down with $s right leg in front, $n retains all $s focus on", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "&ryou. Without warning, $e bursts into a charge and calls out &R'Wolf", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "&RFang Fist' &rAn impression of a savage wolf emerges from $s clasping", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "&rpalms and howls out with fury as $s sprint increases. As $e slams", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "&into you, the brutal rimage dematerializes and reveals $s fist", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "&rlodged deep within your abdomen.", ch, NULL, victim, TO_VICT );

    act( AT_SKILL, "&rCrouching down with $s right leg in front, $n retains all $s focus on", ch, NULL, victim, TO_NOTVICT );
    act( AT_SKILL, "&r$N. Without warning, $n bursts into a charge and calls out &R'Wolf Fang", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_SKILL, "&RFist'. &rAn impression of a savage wolf emerges from $s clasping palms", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_SKILL, "&rand howls out with fury as $s sprint increases. As $n slams into $N,", ch, NULL, victim, TO_NOTVICT );
    act( AT_SKILL, "&rthe brutal image dematerializes and reveals $s fist lodged deep", ch, NULL, victim, TO_NOTVICT );
    act( AT_SKILL, "&rwithin $N's abdomen.", ch, NULL, victim, TO_NOTVICT );

    learn_from_success( ch, gsn_wff );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 8, 12 ) ), gsn_wff );
    ch->melee = FALSE;
  }
  else
  {
    act( AT_SKILL, "You launch your fist upwards, but you miss $N.", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "$n launchs $s fist upwards, but $e misses you.", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "$n launchs $s fist upwards, but $e misses $N.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_wff );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, 0, gsn_wff );
    ch->melee = FALSE;
  }
  return;
}

/* Human-g */

void do_vanishing_ball( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_vanishing ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_vanishing]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_vanishing]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_vanishing]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_vanishing]->focus;

  WAIT_STATE( ch, skill_table[gsn_vanishing]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_vanishing ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 60, 85 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( AT_LBLUE,
         "A large ball of black and blue, swirling, energy begins to form in your hands. With an evil grin on your face you yells out 'Super Vanishing Spirit Ball'! suddenly this omnious ball filled with soul energy from both the good and evil intentions blasts toward $N hitting them with such force it goes directly into their body and explodes from the inside ripping their soul appart from the inside. &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_LBLUE,
         "$n forms a large black and blue, swirling, energy ball into $s hands. With an evil grin on their face they yell 'Super Vanishing Spirit Ball'! The ball flys tword you with such intensity it feels like it went inside you. After several seconds you feel your soul being ripped appart from inside your own being. &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_LBLUE,
         "$n forms a large black and blue, swirling, energy ball into $s hands.With an evil grin on their face they yell 'Super Vanishing Spirit Ball'! The ball flys tword $N with such intensity it looks like it went inside of them. After several seconds $N begins to scream as if their very soul were being ripped out. &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    // dam = ki_absorb( victim, ch, dam, gsn_vanishing );
    learn_from_success( ch, gsn_vanishing );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_LBLUE, "You missed $N with your vanishing ball.", ch, NULL, victim, TO_CHAR );
    act( AT_LBLUE, "$n misses you with $s vanishing ball.", ch, NULL, victim, TO_VICT );
    act( AT_LBLUE, "$n missed $N with a vanishing ball.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_vanishing );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_vanishing]->min_mana;
  return;
}
