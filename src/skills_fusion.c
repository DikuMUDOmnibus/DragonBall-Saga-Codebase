/*****************************************/
/*            Fusion Skills               */
/*             By Kalthizar               */
/*                                        */
/* [ ] Hellstorm       [X] Wrath of God   */
/* [ ] Chaotic Eruption[ ] Holy Kamehameha*/
/* [X] Judgement Day   [ ]                */
/* [ ]                 [ ]                */              
/* [ ]                 [ ]                */
/* [ ]                 [ ]                */
/* [ ]                                    */
/******************************************/

#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

void do_wog( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_wog  ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_wog]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_wog]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_wog]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_wog]->focus;

  WAIT_STATE( ch, skill_table[gsn_wog]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_wog ) )
  {
    switch ( number_range( 1, 100 ) )
    {
      case 100:
      case 99:
      case 98:
      case 97:
      case 96:
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
            dam = get_attmod( ch, victim ) * number_range( 5, 6 );
        act( AT_RED,
             "&RYou bow your head and clasp your hands together in prayer, praying with all your might for the power you need to defeat $N. A blinding, white holy light envelops your entire being as a powerful energy builds up within you.  You throw your arms wide, closing your legs, your whole body glowing like a white, flaming holy cross.  In a flash of pure power, the energy of god himself flows through you, within you, and then out of you as your blast explodes into $N, who seems UNAFFECTED. &W[$t]",
             ch, num_punct( dam ), victim, TO_CHAR );
        act( AT_RED,
             "&R$n bows $s head and clasps $s hands together in prayer, praying with all $s might for the power $e needs to defeat you. A blinding, white holy light envelops $n's entire being as a powerful energy builds up within $n.  $n throws $s arms wide, closing $s legs, $s whole body glowing like a white, flaming holy cross.  In a flash of pure power, the energy of god himself flows through $n, within $n, and then out of $n as $s blast explodes into you, but it is INEFFECTIVE. &W[$t]",
             ch, num_punct( dam ), victim, TO_VICT );
        act( AT_RED,
             "&R$n bows $s head and clasps $s hands together in prayer, praying with all $s might for the power $e needs to defeat $N. A blinding, white holy light envelops $n's entire being as a powerful energy builds up within $n.  $n throws $s arms wide, closing $s legs, $s whole body glowing like a white, flaming holy cross.  In a flash of pure power, the energy of god himself flows through $n, within $n, and then out of $n as $s blast explodes into $N who seems UNAFFECTED. &W[$t]",
             ch, num_punct( dam ), victim, TO_NOTVICT );
            learn_from_success( ch, gsn_wog );
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
        act( AT_RED,
             "&RYou bow your head and clasp your hands together in prayer, praying with all your might for the power you need to defeat $N. A blinding, white holy light envelops your entire being as a powerful energy builds up within you.  You throw your arms wide, closing your legs, your whole body glowing like a white, flaming holy cross.  In a flash of pure power, the energy of god himself flows through you, within you, and then out of you as your blast explodes into $N &zOBLITERATING THEM! &W[$t]",
             ch, num_punct( dam ), victim, TO_CHAR );
        act( AT_RED,
             "&R$n bows $s head and clasps $s hands together in prayer, praying with all $s might for the power $e needs to defeat you. A blinding, white holy light envelops $n's entire being as a powerful energy builds up within $n.  $n throws $s arms wide, closing $s legs, $s whole body glowing like a white, flaming holy cross.  In a flash of pure power, the energy of god himself flows through $n, within $n, and then out of $n as $s blast explodes into you &zOBLITERATING YOU! &W[$t]",
             ch, num_punct( dam ), victim, TO_VICT );
        act( AT_RED,
             "&R$n bows $s head and clasps $s hands together in prayer, praying with all $s might for the power $e needs to defeat $N. A blinding, white holy light envelops $n's entire being as a powerful energy builds up within $n.  $n throws $s arms wide, closing $s legs, $s whole body glowing like a white, flaming holy cross.  In a flash of pure power, the energy of god himself flows through $n, within $n, and then out of $n as $s blast explodes into $N &zOBLITERATING THEM! &W[$t]",
             ch, num_punct( dam ), victim, TO_NOTVICT );

          learn_from_success( ch, gsn_wog );
          global_retcode = damage( ch, victim, dam, TYPE_HIT );
          break;
        }
      default:
        dam = get_attmod( ch, victim ) * number_range( 95, 110 );
        if( ch->charge > 0 )
          dam = chargeDamMult( ch, dam );
        act( AT_RED,
             "&RYou bow your head and clasp your hands together in prayer, praying with all your might for the power you need to defeat $N. A blinding, white holy light envelops your entire being as a powerful energy builds up within you.  You throw your arms wide, closing your legs, your whole body glowing like a white, flaming holy cross.  In a flash of pure power, the energy of god himself flows through you, within you, and then out of you as your blast explodes into $N while leaving the surrounding area untouched. &W[$t]",
             ch, num_punct( dam ), victim, TO_CHAR );
        act( AT_RED,
             "&R$n bows $s head and clasps $s hands together in prayer, praying with all $s might for the power $e needs to defeat you. A blinding, white holy light envelops $n's entire being as a powerful energy builds up within $n.  $n throws $s arms wide, closing $s legs, $s whole body glowing like a white, flaming holy cross.  In a flash of pure power, the energy of god himself flows through $n, within $n, and then out of $n as $s blast explodes into you while leaving the surrounding area untouched. &W[$t]",
             ch, num_punct( dam ), victim, TO_VICT );
        act( AT_RED,
             "&R$n bows $s head and clasps $s hands together in prayer, praying with all $s might for the power $e needs to defeat $N. A blinding, white holy light envelops $n's entire being as a powerful energy builds up within $n.  $n throws $s arms wide, closing $s legs, $s whole body glowing like a white, flaming holy cross.  In a flash of pure power, the energy of god himself flows through $n, within $n, and then out of $n as $s blast explodes into $N while leaving the surrounding area untouched. &W[$t]",
             ch, num_punct( dam ), victim, TO_NOTVICT );

        learn_from_success( ch, gsn_wog );
        global_retcode = damage( ch, victim, dam, TYPE_HIT );
        break;
    }
  }
  else
  {
    act( AT_RED, "Your wrath of god missed $N.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n misses you with $s wrath of god.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n missed $N with $s wrath of god.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_wog );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_wog]->min_mana;
  return;
}

void do_jday( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_jday  ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_jday]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_jday]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_jday]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_jday]->focus;

  WAIT_STATE( ch, skill_table[gsn_jday]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_jday ) )
  {
    switch ( number_range( 1, 100 ) )
    {
      case 100:
      case 99:
      case 98:
      case 97:
      case 96:
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
             "&rYour eyes turn jet black and the world around you seems to fade out, suddenly a man in armor stands behind $n and looks down upon $N.  &B'Your day has come!'&r he states, suddenly he swings his sword, cleaving $N's body and soul in half, then swings again and again, but $N is somehow UNAFFECTED!!&D &W[$t]",
             ch, num_punct( dam ), victim, TO_CHAR );
        act( AT_RED,
             "&r$n's eyes turn jet black and the world around you seems to fade out, suddenly a man in armor stands behind $n and looks down upon you.  &B'Your day has come!'&r he states, suddenly he swings his sword, cleaving your body and soul in half, then swings again and again, but you are somehow UNAFFECTED!!&D &W[$t]",
             ch, num_punct( dam ), victim, TO_VICT );
        act( AT_RED,
             "&r$n's eyes turn jet black and the world around you seems to fade out, suddenly a man in armor stands behind $n and looks down upon $N.  &B'Your day has come!'&r he states, suddenly he swings his sword, cleaving $N's body and soul in half, then swings again and again, but $N is somehow UNAFFECTED!!&D &W[$t]",
             ch, num_punct( dam ), victim, TO_NOTVICT );
            learn_from_success( ch, gsn_jday );
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
        act( AT_RED,
             "&rYour eyes turn jet black and the world around you seems to fade out, suddenly a man in armor stands behind $n and looks down upon $N.  &B'Your day has come!'&r he states, suddenly he swings his sword, cleaving $N's body and soul in half, then swings again and again, leaving nothing behind but free floating atoms.&D &W[$t]",
             ch, num_punct( dam ), victim, TO_CHAR );
        act( AT_RED,
             "&r$n's eyes turn jet black and the world around you seems to fade out, suddenly a man in armor stands behind $n and looks down upon you.  &B'Your day has come!'&r he states, suddenly he swings his sword, cleaving your body and soul in half, then swings again and again, leaving nothing behind but free floating atoms.&D &W[$t]",
             ch, num_punct( dam ), victim, TO_VICT );
        act( AT_RED,
             "&r$n's eyes turn jet black and the world around you seems to fade out, suddenly a man in armor stands behind $n and looks down upon $N.  &B'Your day has come!'&r he states, suddenly he swings his sword, cleaving $N's body and soul in half, then swings again and again, leaving nothing behind but free floating atoms.&D &W[$t]",
             ch, num_punct( dam ), victim, TO_NOTVICT );

          learn_from_success( ch, gsn_jday );
          global_retcode = damage( ch, victim, dam, TYPE_HIT );
          break;
        }
      default:
        dam = get_attmod( ch, victim ) * number_range( 100, 110 );
        if( ch->charge > 0 )
          dam = chargeDamMult( ch, dam );
        act( AT_RED,
             "&rYour eyes turn jet black and the world around you seems to fade out, suddenly a man in armor stands behind you and looks down upon $N.  &B'You have been judged'&r he states and swings his mighty sword, slashing it across $N's body.  Suddenly the world returns to normal but the pain still remains in $N.&D &W[$t]",
             ch, num_punct( dam ), victim, TO_CHAR );
        act( AT_RED,
             "&r$n's eyes turn jet black and the world around you seems to fade out, suddenly a man in armor stands behind $n and looks down upon you. &B'You have been judged'&r he states and swings his mighty sword, slashing it across your body.  Suddenly the world returns to normal but the pain still remains.&D &W[$t]",
             ch, num_punct( dam ), victim, TO_VICT );
        act( AT_RED,
             "&&r$n's eyes turn jet black and the world around you seems to fade out, suddenly a man in armor stands behind $n and looks down upon $N.  &B'You have been judged'&r he states and swings his mighty sword, slashing it across $N's body.  Suddenly the world returns to normal but the pain still remains in $N.&D&W[$t]",
             ch, num_punct( dam ), victim, TO_NOTVICT );

        learn_from_success( ch, gsn_jday );
        global_retcode = damage( ch, victim, dam, TYPE_HIT );
        break;
    }
  }
  else
  {
    act( AT_RED, "Your Judgement Day missed $N.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "$n misses you with $s judgement day.", ch, NULL, victim, TO_VICT );
    act( AT_RED, "$n missed $N with $s judgement day.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_jday );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_jday]->min_mana;
  return;
}
