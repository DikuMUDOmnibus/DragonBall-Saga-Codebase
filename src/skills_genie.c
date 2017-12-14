#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

extern void transStatApply args( ( CHAR_DATA * ch, int strMod, int spdMod, int intMod, int conMod ) );
extern void transStatRemove args( ( CHAR_DATA * ch ) );
void make_pc_candy args( ( CHAR_DATA * ch ) );
void make_npc_candy args( ( CHAR_DATA * ch ) );

bool removeGenieTrans( CHAR_DATA * ch )
{
  if( xIS_SET( ch->affected_by, AFF_EVIL_TRANS ) )
  {
    xREMOVE_BIT( ch->affected_by, AFF_EVIL_TRANS );
    transStatRemove( ch );
    return TRUE;
  }
  if( xIS_SET( ch->affected_by, AFF_SUPER_TRANS ) )
  {
    xREMOVE_BIT( ch->affected_by, AFF_SUPER_TRANS );
    transStatRemove( ch );
    return TRUE;
  }
  if( xIS_SET( ch->affected_by, AFF_KID_TRANS ) )
  {
    xREMOVE_BIT( ch->affected_by, AFF_KID_TRANS );
    transStatRemove( ch );
    return TRUE;
  }
  return FALSE;
}

void make_npc_candy( CHAR_DATA * ch )
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *candy;

  candy = create_object( get_obj_index( 301101 ), 0 );

  sprintf( buf, "chocolate ball" );
  STRFREE( candy->name );
  candy->name = STRALLOC( buf );

  sprintf( buf, candy->short_descr );
  STRFREE( candy->short_descr );
  candy->short_descr = STRALLOC( buf );

  obj_to_char( candy, ch );

  return;
}

void make_pc_candy( CHAR_DATA * ch )
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *candy;

  candy = create_object( get_obj_index( 199 ), 0 );

  sprintf( buf, "chocolate figure" );
  STRFREE( candy->name );
  candy->name = STRALLOC( buf );

  sprintf( buf, candy->short_descr );
  STRFREE( candy->short_descr );
  candy->short_descr = STRALLOC( buf );

  obj_to_char( candy, ch );

  return;
}


void do_transform( CHAR_DATA * ch, char *argument )
{
  if( !is_genie( ch ) )
  {
    send_to_char( "You can't do that.\n\r", ch );
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

  if( !argument || argument[0] == '\0' )
  {
    send_to_char( "Syntax: transform [thin|super|kid]\n\r", ch );
    return;
  }

  if( !str_prefix( argument, "thin" ) )
  {
    if( ch->exp < skill_table[gsn_thin_trans]->min_level[ch->race] )
    {
      send_to_char( "You can't do that at your level.\n\r", ch );
      return;
    }

    if( ch->mana < skill_table[gsn_thin_trans]->min_mana )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }

    WAIT_STATE( ch, skill_table[gsn_thin_trans]->beats );

    if( can_use_skill( ch, number_percent(  ), gsn_thin_trans ) )
    {
      removeGenieTrans( ch );
      act( AT_PINK, "Steam courses through the series of holes along your head, as you begin", ch, NULL, NULL, TO_CHAR );
      act( AT_PINK, "to let out a high-pitched shriek.  Your rolling layers of pink flesh", ch, NULL, NULL, TO_CHAR );
      act( AT_PINK, "rapidly compact, giving you a leaner, more sinister look.", ch, NULL, NULL, TO_CHAR );

      act( AT_PINK, "Steam courses through the series of holes along $n's head as $e begins", ch, NULL, NULL, TO_NOTVICT );
      act( AT_PINK, "to let out a high-pitched shriek.  $*s rolling layers of pink flesh rapidly", ch, NULL, NULL,
           TO_NOTVICT );
      act( AT_PINK, "compact, giving $m a leaner, more sinister look.", ch, NULL, NULL, TO_NOTVICT );
      xSET_BIT( ch->affected_by, AFF_EVIL_TRANS );
      ch->pl = ch->exp * 16;
      transStatApply( ch, 12, 15, 10, 15 );
      if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
        ch->pl = ch->pl * 1.1;
      learn_from_success( ch, gsn_thin_trans );
    }
    else
    {
      act( AT_PLAIN, "You failed.", ch, NULL, NULL, TO_CHAR );
      learn_from_failure( ch, gsn_thin_trans );
    }

    ch->mana -= skill_table[gsn_thin_trans]->min_mana;
    return;
  }

  if( !str_prefix( argument, "super" ) )
  {
    if( ch->exp < skill_table[gsn_super_trans]->min_level[ch->race] )
    {
      send_to_char( "You can't do that at your level.\n\r", ch );
      return;
    }

    if( !xIS_SET( ch->affected_by, AFF_EVIL_TRANS ) && !xIS_SET( ch->affected_by, AFF_KID_TRANS ) )
    {
      send_to_char( "You can't do that in this form.\n\r", ch );
      return;
    }

    if( ch->mana < skill_table[gsn_super_trans]->min_mana )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }

    WAIT_STATE( ch, skill_table[gsn_super_trans]->beats );

    if( can_use_skill( ch, number_percent(  ), gsn_super_trans ) )
    {
      removeGenieTrans( ch );
      act( AT_PINK, "You utter a low-pitched growl, steam blasting through the porting in", ch, NULL, NULL, TO_CHAR );
      act( AT_PINK, "your head, as you draw energy from your absorbed victims.  The tentacle", ch, NULL, NULL, TO_CHAR );
      act( AT_PINK, "atop your head lengthens to mid-back level and your slender body bulks", ch, NULL, NULL, TO_CHAR );
      act( AT_PINK, "up, taking on a powerful, muscular look.  An aura of dark purple energy", ch, NULL, NULL, TO_CHAR );
      act( AT_PINK, "surrounds you.", ch, NULL, NULL, TO_CHAR );

      act( AT_PINK, "$n utters a low-pitched growl, steam blasting through the porting", ch, NULL, NULL, TO_NOTVICT );
      act( AT_PINK, "in $s head, as $e draws energy from $s absorbed victims.  The tentacle", ch, NULL, NULL, TO_NOTVICT );
      act( AT_PINK, "atop $s head lengthens to mid-back level and $s slender body bulks up,", ch, NULL, NULL, TO_NOTVICT );
      act( AT_PINK, "taking on a powerful, muscular look.  An aura of dark purple energy", ch, NULL, NULL, TO_NOTVICT );
      act( AT_PINK, "surrounds $m.", ch, NULL, NULL, TO_NOTVICT );

      xSET_BIT( ch->affected_by, AFF_SUPER_TRANS );
      ch->pl = ch->exp * 25;
      transStatApply( ch, 22, 18, 16, 22 );
      if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
        ch->pl = ch->pl * 1.1;
      learn_from_success( ch, gsn_super_trans );
    }
    else
    {
      act( AT_PLAIN, "You failed.", ch, NULL, NULL, TO_CHAR );
      learn_from_failure( ch, gsn_super_trans );
    }

    ch->mana -= skill_table[gsn_super_trans]->min_mana;
    return;
  }

  if( !str_prefix( argument, "kid" ) )
  {
    if( ch->exp < skill_table[gsn_kid_trans]->min_level[ch->race] )
    {
      send_to_char( "You can't do that at your level.\n\r", ch );
      return;
    }

    if( !xIS_SET( ch->affected_by, AFF_SUPER_TRANS ) )
    {
      send_to_char( "You can't do that in this form.\n\r", ch );
      return;
    }

    if( ch->mana < skill_table[gsn_kid_trans]->min_mana )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }

    WAIT_STATE( ch, skill_table[gsn_kid_trans]->beats );

    if( can_use_skill( ch, number_percent(  ), gsn_kid_trans ) )
    {
      removeGenieTrans( ch );
      act( AT_PINK, "You bellow loudly, your voice starting deep and gradually becoming more", ch, NULL, NULL, TO_CHAR );
      act( AT_PINK, "shrill as the stolen energy gives way to a terrifying strength of your", ch, NULL, NULL, TO_CHAR );
      act( AT_PINK, "own.  Steam shrieks through the holes along your head, completely masking", ch, NULL, NULL, TO_CHAR );
      act( AT_PINK, "your presence.  Your form shrinks and compacts and when the steam finally", ch, NULL, NULL, TO_CHAR );
      act( AT_PINK, "clears, you have transformed into a child-sized version of yourself,", ch, NULL, NULL, TO_CHAR );
      act( AT_PINK, "radiating a horrifying power.", ch, NULL, NULL, TO_CHAR );

      act( AT_PINK, "$n bellows loudly, $s voice starting deep and gradually becoming more", ch, NULL, NULL, TO_NOTVICT );
      act( AT_PINK, "shrill as the stolen energy gives way to a terrifying strength of $s own.", ch, NULL, NULL,
           TO_NOTVICT );
      act( AT_PINK, "Steam shrieks through the holes along $s head, completely masking $s", ch, NULL, NULL, TO_NOTVICT );
      act( AT_PINK, "presence.  $*s form shrinks and compacts and when the steam finally", ch, NULL, NULL, TO_NOTVICT );
      act( AT_PINK, "clears, $e has transformed into a child-sized version of $mself,", ch, NULL, NULL, TO_NOTVICT );
      act( AT_PINK, "radiating a horrifiyng power.", ch, NULL, NULL, TO_NOTVICT );

      xSET_BIT( ch->affected_by, AFF_KID_TRANS );
      ch->pl = ch->exp * 37;
      transStatApply( ch, 30, 34, 28, 34 );
      if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
        ch->pl = ch->pl * 1.1;
      learn_from_success( ch, gsn_kid_trans );
    }
    else
    {
      act( AT_PLAIN, "You failed.", ch, NULL, NULL, TO_CHAR );
      learn_from_failure( ch, gsn_kid_trans );
    }

    ch->mana -= skill_table[gsn_kid_trans]->min_mana;
    return;
  }

  do_transform( ch, "" );

  return;
}

void do_demon_wave( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_demonwave]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_demonwave]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_demonwave]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }

  ch->focus -= skill_table[gsn_demonwave]->focus;
  WAIT_STATE( ch, skill_table[gsn_demonwave]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_demonwave ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 22, 28 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_DGREY, "You cup your hands and grin at $N, \"You go bye bye now!\" ", ch, NULL, victim, TO_CHAR );
    act( AT_DGREY, "A ball of sparkling purple energy appears in your palms and you push your hands", ch, NULL, victim,
         TO_CHAR );
    act( AT_DGREY, "forward, creating a massive wave of energy that flies at $N! &W[$t]", ch, num_punct( dam ), victim,
         TO_CHAR );

    act( AT_DGREY, "$n cups $s hands and grins at you, \"You go bye bye now!\" ", ch, NULL, victim, TO_VICT );
    act( AT_DGREY, "A ball of sparkling purple energy appears in $s palms and $e pushes $s hands", ch, NULL, victim,
         TO_VICT );
    act( AT_DGREY, "forward, creating a massive wave of energy that flies at you! &W[$t]", ch, num_punct( dam ), victim,
         TO_VICT );

    act( AT_DGREY, "$n cups $s hands and grins at $N, \"You go bye bye now!\" ", ch, NULL, victim, TO_NOTVICT );
    act( AT_DGREY, "A ball of sparkling purple energy appears in $s palms and $e pushes $s hands", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_DGREY, "forward, creating a massive wave of energy that flies at $N! &W[$t]", ch, num_punct( dam ), victim,
         TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_demonwave );
    learn_from_success( ch, gsn_demonwave );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_DGREY, "You missed $N with your demon wave.", ch, NULL, victim, TO_CHAR );
    act( AT_DGREY, "$n misses you with $s demon wave.", ch, NULL, victim, TO_VICT );
    act( AT_DGREY, "$n missed $N with a demon wave.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_demonwave );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }

  ch->mana -= skill_table[gsn_demonwave]->min_mana;
  return;
}

void do_candy_blast( CHAR_DATA * ch, char *argument )
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

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_candyblast]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_candyblast]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_candyblast]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_candyblast]->focus;

  WAIT_STATE( ch, skill_table[gsn_candyblast]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_candyblast ) )
  {
    switch ( IS_NPC( victim ) ? number_range( 5, 100 ) : number_range( 1, 100 ) )
    {
      case 100:
        if( IS_NPC( victim ) || ( !IS_NPC( victim ) && !xIS_SET( ch->act, PLR_SPAR ) ) )
        {
          if( victim->pl / ch->pl >= 2 )
          {
            dam = number_range( 1, 2 );
            act( AT_PINK, "Your head tentacle flips forwards, shooting a bright red beam of energy at", ch, NULL, victim,
                 TO_CHAR );
            act( AT_PINK, "$N.  $N convulses as $S body begins to harden and shrink.\n", ch, NULL, victim, TO_CHAR );
            act( AT_CYAN, "Fortunately for $N, $S body begins to grow and $S skin begins to soften", ch, NULL, victim,
                 TO_CHAR );
            act( AT_CYAN, "once more, leaving $S little more than out of breath.  &W[$t]", ch, num_punct( dam ), victim,
                 TO_CHAR );

            act( AT_PINK, "$N's head tentacle flips forwards, shooting a bright red beam of energy at", ch, NULL, victim,
                 TO_VICT );
            act( AT_PINK, "you.  You convulse as your body begins to harden and shrink.\n", ch, NULL, victim, TO_VICT );
            act( AT_CYAN, "Fortunately for you, you notice you body returning to normal form and your", ch, NULL, victim,
                 TO_VICT );
            act( AT_CYAN, "skin softening.  &W[$t]", ch, num_punct( dam ), victim, TO_VICT );

            act( AT_PINK, "$N's head tentacle flips forwards, shooting a bright red beam of energy at", ch, NULL, victim,
                 TO_NOTVICT );
            act( AT_PINK, "$N.  $N convulses as $S body begins to harden and shrink.\n", ch, NULL, victim, TO_NOTVICT );
            act( AT_CYAN, "Fortunately for $N, $S body begins to grow and $S skin begins to soften", ch, NULL, victim,
                 TO_NOTVICT );
            act( AT_CYAN, "once more, leaving $S little more than out of breath.  &W[$t]", ch, num_punct( dam ), victim,
                 TO_NOTVICT );

            break;
          }

          dam = 999999999;
          if( IS_NPC( victim ) )
            make_npc_candy( ch );
          if( !IS_NPC( victim ) )
            make_pc_candy( ch );
          act( AT_PINK, "Your head tentacle flips forwards, shooting a bright red beam of energy at", ch, NULL, victim,
               TO_CHAR );
          act( AT_PINK, "$N.  $N convulses as $S body begins to harden and shrink.\n", ch, NULL, victim, TO_CHAR );
          act( AT_RED, "Suddenly a loud *POP* is heard as the ray diminishes, turning $N into a", ch, NULL, victim,
               TO_CHAR );
          act( AT_RED, "small ball of everlasting candy.  &W[$t]", ch, num_punct( dam ), victim, TO_CHAR );

          act( AT_PINK, "$n's head tentacle flips forwards, shooting a bright red beam of energy at", ch, NULL, victim,
               TO_VICT );
          act( AT_PINK, "You.  You convulses as your body begins to harden and shrink.\n", ch, NULL, victim, TO_VICT );
          act( AT_RED, "Suddenly a loud *POP* is heard as the ray diminishes, turning you into a", ch, NULL, victim,
               TO_VICT );
          act( AT_RED, "small ball of everlasting candy.  &W[$t]", ch, num_punct( dam ), victim, TO_VICT );

          act( AT_PINK, "$n's head tentacle flips forwards, shooting a bright red beam of energy at", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_PINK, "$N.  $N convulses as $S body begins to harden and shrink.\n", ch, NULL, victim, TO_NOTVICT );
          act( AT_RED, "Suddenly a loud *POP* is heard as the ray diminishes, turning $N into a", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_RED, "small ball of everlasting candy.  &W[$t]", ch, num_punct( dam ), victim, TO_NOTVICT );

          learn_from_success( ch, gsn_candyblast );
          global_retcode = damage( ch, victim, dam, TYPE_HIT );
          break;
        }
      default:
        dam = get_attmod( ch, victim ) * number_range( 35, 45 );
        if( ch->charge > 0 )
          dam = chargeDamMult( ch, dam );

        act( AT_PINK, "Your head tentacle flips towards $N and fires a pink beam of energy.  The", ch, NULL, victim,
             TO_CHAR );
        act( AT_PINK, "ray hits and engulfs $N in a pink blaze that electrifies $S body.  &W[$t]", ch, num_punct( dam ),
             victim, TO_CHAR );

        act( AT_PINK, "$n's head tentacle flips towards you and fires a pink beam of energy.  The", ch, NULL, victim,
             TO_VICT );
        act( AT_PINK, "ray hits and engulfs you in a pink blaze that electrifies your body. &W[$t]", ch, num_punct( dam ),
             victim, TO_VICT );

        act( AT_PINK, "$n's head tentacle flips towards $N and fires a pink beam of energy.  The", ch, NULL, victim,
             TO_NOTVICT );
        act( AT_PINK, "ray hits and engulfs $N in a pink blaze that electrifies $S body.  &W[$t]", ch, num_punct( dam ),
             victim, TO_NOTVICT );

        dam = ki_absorb( victim, ch, dam, gsn_candyblast );
        learn_from_success( ch, gsn_candyblast );
        global_retcode = damage( ch, victim, dam, TYPE_HIT );
        break;
    }
  }
  else
  {
    act( AT_PINK, "You missed $N with your candy blast.", ch, NULL, victim, TO_CHAR );
    act( AT_PINK, "$n misses you with $s candy blast.", ch, NULL, victim, TO_VICT );
    act( AT_PINK, "$n missed $N with a candy blast.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_candyblast );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_candyblast]->min_mana;
  return;
}

void do_tentacle_attack( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_tentacle]->skill_level[ch->class] )
  {
    send_to_char( "That isn't quite one of your natural skills.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_tentacle]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_tentacle ) )
  {
    dam = number_range( 1, ch->level );
    act( AT_PINK, "Grinning with a maniacal expression, you carefully plot where to strike your", ch, NULL, victim,
         TO_CHAR );
    act( AT_PINK, "victim next. Swaying from the back of your head, your muscular tentacle rapidly", ch, NULL, victim,
         TO_CHAR );
    act( AT_PINK, "lashes forward to attack $N with a mind numbing blow.  The whip like", ch, NULL, victim, TO_CHAR );
    act( AT_PINK, "blob overpowers $S defenses and strikes $M to the ground before $E can", ch, NULL, victim, TO_CHAR );
    act( AT_PINK, "retaliate. &W[$t]", ch, num_punct( dam ), victim, TO_CHAR );

    act( AT_PINK, "Grinning with a maniacal expression, $n carefully plots where to strike", ch, NULL, victim, TO_VICT );
    act( AT_PINK, "$s victim next. Swaying from the back of $s head, $n's muscular tentacle", ch, NULL, victim, TO_VICT );
    act( AT_PINK, "rapidly lashes forward to attack you with a mind numbing blow.  The whip", ch, NULL, victim, TO_VICT );
    act( AT_PINK, "like blob overpowers your defenses and strikes you to the ground before you", ch, NULL, victim, TO_VICT );
    act( AT_PINK, "can retaliate. &W[$t]", ch, num_punct( dam ), victim, TO_VICT );

    act( AT_PINK, "Grinning with a maniacal expression, $n carefully plots where to strike", ch, NULL, victim, TO_NOTVICT );
    act( AT_PINK, "$s victim next. Swaying from the back of $s head, $n's muscular tentacle", ch, NULL, victim, TO_NOTVICT );
    act( AT_PINK, "rapidly lashes forward to attack $N with a mind numbing blow.  The whip", ch, NULL, victim, TO_NOTVICT );
    act( AT_PINK, "like blob overpowers $N's defenses and strikes $M to the ground before", ch, NULL, victim, TO_NOTVICT );
    act( AT_PINK, "$E can retaliate. &W[$t]", ch, num_punct( dam ), victim, TO_NOTVICT );

    learn_from_success( ch, gsn_tentacle );
    global_retcode = damage( ch, victim, dam, gsn_tentacle );
  }
  else
  {
    act( AT_WHITE, "$N's speed seems to prove your tentacle attack is futile.", ch, NULL, victim, TO_CHAR );
    act( AT_WHITE, "Your speed seems to prove $n's tentacle attack is futile.", ch, NULL, victim, TO_VICT );
    act( AT_WHITE, "$N's speed seems to prove $n's tentacle attack is futile.", ch, NULL, victim, TO_NOTVICT );

    learn_from_failure( ch, gsn_tentacle );
    global_retcode = damage( ch, victim, 0, gsn_tentacle );
  }
  return;
}


void do_genocide_attack( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( is_namek(ch) )
      return;
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_genocide_attack ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_genocide_attack]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_genocide_attack]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_genocide_attack]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_genocide_attack]->focus;

  WAIT_STATE( ch, skill_table[gsn_genocide_attack]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_genocide_attack ) )
  {
    switch ( number_range( 1, 100 ) )
    {
      case 100:
      case 99:
      case 98:
      case 97:
      case 96:
        if( IS_NPC( victim ) || ( !IS_NPC( victim ) && !xIS_SET( ch->act, PLR_SPAR ) ) )
        {

          /*
           * Redone so that instant death can't be abused to instant kill
           * * players over 2x their power. -- Islvin
           */
          if( victim->pl / ch->pl >= 5 )
          {
            dam = get_attmod( ch, victim ) * number_range( 10, 11 );
            act( AT_PURPLE, "With your head tilted in a downward position, you concentrate a vast amount of", ch, NULL,
                 victim, TO_CHAR );
            act( AT_PURPLE, "energy to engage in a final onslaught. After contemplating the quantity vital to", ch, NULL,
                 victim, TO_CHAR );
            act( AT_PURPLE, "ensure $N's demise, you elevate your arm in a stiff upward stance and", ch, NULL, victim,
                 TO_CHAR );
            act( AT_PURPLE, "call out, &P'GENOCIDE ATTACK.' &pWith that, a fountain of energy pulses arise", ch, NULL,
                 victim, TO_CHAR );
            act( AT_PURPLE, "from your extended palm and rain toward $N at a remarkable pace.", ch, NULL, victim, TO_CHAR );
            act( AT_PURPLE, "While battering $M from multiple directions, the radiating discharge", ch, NULL, victim,
                 TO_CHAR );
            act( AT_PURPLE, "ambushes everything that confronts the assault from its target.", ch, NULL, victim, TO_CHAR );
            act( AT_PINK, "The assault vanishises on contact due to $N's powerfull aura.", ch, NULL, victim, TO_CHAR );

            act( AT_PURPLE, "With $n's head tilted in a downward position, $e concentrates a vast amount of", ch, NULL,
                 victim, TO_VICT );
            act( AT_PURPLE, "energy to engage in a final onslaught. After contemplating the quantity vital to", ch, NULL,
                 victim, TO_VICT );
            act( AT_PURPLE, "ensure your demise, $n elevates $s arm in a stiff upward stance and", ch, NULL, victim,
                 TO_VICT );
            act( AT_PURPLE, "calls out, &P'GENOCIDE ATTACK.' &pWith that, a fountain of energy pulses arise", ch, NULL,
                 victim, TO_VICT );
            act( AT_PURPLE, "from $n's extended palm and rains toward you at a remarkable pace.", ch, NULL, victim,
                 TO_VICT );
            act( AT_PURPLE, "While battering you from multiple directions, the radiating discharge", ch, NULL, victim,
                 TO_VICT );
            act( AT_PURPLE, "ambushes everything that confronts the assault from its target.", ch, NULL, victim, TO_VICT );
            act( AT_PINK, "$n's assault vanishises on contact due to your powerfull aura.", ch, NULL, victim, TO_VICT );

            act( AT_PURPLE, "With $n's head tilted in a downward position, $e concentrates a vast amount of", ch, NULL,
                 victim, TO_NOTVICT );
            act( AT_PURPLE, "energy to engage in a final onslaught. After contemplating the quantity vital to", ch, NULL,
                 victim, TO_NOTVICT );
            act( AT_PURPLE, "ensure $N's demise, $n elevates $s arm in a stiff upward stance and", ch, NULL, victim,
                 TO_NOTVICT );
            act( AT_PURPLE, "calls out, &P'GENOCIDE ATTACK.' &pWith that, a fountain of energy pulses arise", ch, NULL,
                 victim, TO_NOTVICT );
            act( AT_PURPLE, "from $n's extended palm and rain towards $N at a remarkable pace.", ch, NULL, victim,
                 TO_NOTVICT );
            act( AT_PURPLE, "While battering $N from multiple directions, the radiating discharge", ch, NULL, victim,
                 TO_NOTVICT );
            act( AT_PURPLE, "ambushes everything that confronts the assault from its target.", ch, NULL, victim,
                 TO_NOTVICT );
            act( AT_PINK, "$n's assault vanishises on contact due to $N's powerfull aura.", ch, NULL, victim, TO_NOTVICT );
            learn_from_success( ch, gsn_genocide_attack );
            global_retcode = damage( ch, victim, dam, TYPE_HIT );
            break;
          }

          dam = 999999999;
          act( AT_PURPLE, "With your head tilted in a downward position, you concentrate a vast amount of", ch, NULL, victim,
               TO_CHAR );
          act( AT_PURPLE, "energy to engage in a final onslaught. After contemplating the quantity vital to", ch, NULL,
               victim, TO_CHAR );
          act( AT_PURPLE, "ensure $N's demise, you elevate your arm in a stiff upward stance and", ch, NULL, victim,
               TO_CHAR );
          act( AT_PURPLE, "call out, &P'GENOCIDE ATTACK.' &pWith that, a fountain of energy pulses arise", ch, NULL, victim,
               TO_CHAR );
          act( AT_PURPLE, "from your extended palm and rain toward $N at a remarkable pace.", ch, NULL, victim, TO_CHAR );
          act( AT_PURPLE, "While battering $M from multiple directions, the radiating discharge", ch, NULL, victim,
               TO_CHAR );
          act( AT_PURPLE, "ambushes everything that confronts the assault from its target.", ch, NULL, victim, TO_CHAR );
          act( AT_PINK, "The mayhem kills $M instantly!", ch, NULL, victim, TO_CHAR );

          act( AT_PURPLE, "With $n's head tilted in a downward position, $e concentrates a vast amount of", ch, NULL, victim,
               TO_VICT );
          act( AT_PURPLE, "energy to engage in a final onslaught. After contemplating the quantity vital to", ch, NULL,
               victim, TO_VICT );
          act( AT_PURPLE, "ensure your demise, $n elevates $s arm in a stiff upward stance and", ch, NULL, victim, TO_VICT );
          act( AT_PURPLE, "calls out, &P'GENOCIDE ATTACK.' &pWith that, a fountain of energy pulses arise", ch, NULL, victim,
               TO_VICT );
          act( AT_PURPLE, "from $n's extended palm and rains toward you at a remarkable pace.", ch, NULL, victim, TO_VICT );
          act( AT_PURPLE, "While battering you from multiple directions, the radiating discharge", ch, NULL, victim,
               TO_VICT );
          act( AT_PURPLE, "ambushes everything that confronts the assault from its target.", ch, NULL, victim, TO_VICT );
          act( AT_PINK, "The mayhem kills you instantly!", ch, NULL, victim, TO_VICT );

          act( AT_PURPLE, "With $n's head tilted in a downward position, $e concentrates a vast amount of", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_PURPLE, "energy to engage in a final onslaught. After contemplating the quantity vital to", ch, NULL,
               victim, TO_NOTVICT );
          act( AT_PURPLE, "ensure $N's demise, $n elevates $s arm in a stiff upward stance and", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_PURPLE, "calls out, &P'GENOCIDE ATTACK.' &pWith that, a fountain of energy pulses arise", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_PURPLE, "from $n's extended palm and rain towards $N at a remarkable pace.", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_PURPLE, "While battering $N from multiple directions, the radiating discharge", ch, NULL, victim,
               TO_NOTVICT );
          act( AT_PURPLE, "ambushes everything that confronts the assault from its target.", ch, NULL, victim, TO_NOTVICT );
          act( AT_PINK, "The mayhem kills $M instantly!", ch, NULL, victim, TO_NOTVICT );


          learn_from_success( ch, gsn_genocide_attack );
          global_retcode = damage( ch, victim, dam, TYPE_HIT );
          break;
        }
      default:
        dam = get_attmod( ch, victim ) * number_range( 90, 115 );
        if( ch->charge > 0 )
          dam = chargeDamMult( ch, dam );

        act( AT_PURPLE, "With your head tilted in a downward position, you concentrate a vast amount of", ch,
             num_punct( dam ), victim, TO_CHAR );
        act( AT_PURPLE, "energy to engage in a final onslaught. After contemplating the quantity vital to", ch,
             num_punct( dam ), victim, TO_CHAR );
        act( AT_PURPLE, "ensure $N's demise, you elevate your arm in a stiff upward stance and", ch, num_punct( dam ),
             victim, TO_CHAR );
        act( AT_PURPLE, "call out, &P'GENOCIDE ATTACK.' &pWith that, a fountain of energy pulses arise", ch,
             num_punct( dam ), victim, TO_CHAR );
        act( AT_PURPLE, "from your extended palm and rain toward $N with a remarkable pace.", ch, num_punct( dam ), victim,
             TO_CHAR );
        act( AT_PURPLE, "While battering $M from multiple directions, the radiating discharge", ch, num_punct( dam ), victim,
             TO_CHAR );
        act( AT_PURPLE, "ambushes everything that confronts the assault from its target. &W[$t]", ch, num_punct( dam ),
             victim, TO_CHAR );

        act( AT_PURPLE, "With $n's head tilted in a downward position, $e concentrates a vast amount of", ch,
             num_punct( dam ), victim, TO_VICT );
        act( AT_PURPLE, "energy to engage in a final onslaught. After contemplating the quantity vital to", ch,
             num_punct( dam ), victim, TO_VICT );
        act( AT_PURPLE, "ensure your demise, $n elevates $s arm in a stiff upward stance and", ch, num_punct( dam ), victim,
             TO_VICT );
        act( AT_PURPLE, "calls out, &P'GENOCIDE ATTACK.' &pWith that, a fountain of energy pulses arise", ch,
             num_punct( dam ), victim, TO_VICT );
        act( AT_PURPLE, "from $n's extended palm and rains toward you at a remarkable pace.", ch, num_punct( dam ), victim,
             TO_VICT );
        act( AT_PURPLE, "While battering you from multiple directions, the radiating discharge", ch, num_punct( dam ),
             victim, TO_VICT );
        act( AT_PURPLE, "ambushes everything that confronts the assault from its target. &W[$t]", ch, num_punct( dam ),
             victim, TO_VICT );

        act( AT_PURPLE, "With $n's head tilted in a downward position, $e concentrates a vast amount of", ch,
             num_punct( dam ), victim, TO_NOTVICT );
        act( AT_PURPLE, "energy to engage in a final onslaught. After contemplating the quantity vital to", ch,
             num_punct( dam ), victim, TO_NOTVICT );
        act( AT_PURPLE, "ensure $N's demise, $n elevates $s arm in a stiff upward stance and", ch, num_punct( dam ), victim,
             TO_NOTVICT );
        act( AT_PURPLE, "calls out, &P'GENOCIDE ATTACK.' &pWith that, a fountain of energy pulses arise", ch,
             num_punct( dam ), victim, TO_NOTVICT );
        act( AT_PURPLE, "from $n's extended palm and rain towards $N with a remarkable pace.", ch, num_punct( dam ), victim,
             TO_NOTVICT );
        act( AT_PURPLE, "While battering $N from multiple directions, the radiating discharge", ch, num_punct( dam ), victim,
             TO_NOTVICT );
        act( AT_PURPLE, "ambushes everything that confronts the assault from its target. &W[$t]", ch, num_punct( dam ),
             victim, TO_NOTVICT );


        learn_from_success( ch, gsn_genocide_attack );
        global_retcode = damage( ch, victim, dam, TYPE_HIT );
        break;
    }
  }
  else
  {
    act( AT_PURPLE, "You missed $N with your genocide attack.", ch, NULL, victim, TO_CHAR );
    act( AT_PURPLE, "$n misses you with $s genocide attack.", ch, NULL, victim, TO_VICT );
    act( AT_PURPLE, "$n missed $N with a dual genocide attack.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_genocide_attack );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_genocide_attack]->min_mana;
  return;
}

void do_skin_trap( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_skin_trap]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_skin_trap]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  if( ch->focus < skill_table[gsn_skin_trap]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }

  ch->focus -= skill_table[gsn_skin_trap]->focus;
  WAIT_STATE( ch, skill_table[gsn_skin_trap]->beats );

  if( can_use_skill( ch, number_percent(  ), gsn_skin_trap ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 20, 25 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );

    act( AT_PINK, "A nodule of flesh detaches from you and soars towards $N, entrapping", ch, NULL, victim, TO_CHAR );
    act( AT_PINK, "them in its rubbery depths. &W[$t]", ch, num_punct( dam ), victim, TO_CHAR );

    act( AT_PINK, "Amidst the raging battle, a flicker of movement catches the corner", ch, NULL, victim, TO_VICT );
    act( AT_PINK, "of your eye.  A large nodule of severed pink flesh soars towards", ch, NULL, victim, TO_VICT );
    act( AT_PINK, "you, entrapping you within its rubbery depths. &W[$t]", ch, num_punct( dam ), victim, TO_VICT );

    act( AT_PINK, "A nodule of flesh detaches from $n and soars towards $N, entrapping", ch, NULL, victim, TO_NOTVICT );
    act( AT_PINK, "them in its rubbery depths. &W[$t]", ch, num_punct( dam ), victim, TO_NOTVICT );

    dam = ki_absorb( victim, ch, dam, gsn_skin_trap );
    learn_from_success( ch, gsn_skin_trap );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
    if( ( victim = who_fighting( ch ) ) != NULL )
    {
      if( number_range( 1, 4 ) < 3 )
      {
        act( AT_RED, "\nYou shudder in ecstacy as $N's lifeforce seeps into your body.", ch, NULL, victim, TO_CHAR );
        act( AT_RED, "\n$N shudders in ecstacy as your lifeforce seeps into $s body.", ch, NULL, victim, TO_VICT );
        act( AT_RED, "\n$N shudders in ecstacy as $N's lifeforce seeps out of $S body and", ch, NULL, victim, TO_NOTVICT );
        act( AT_RED, "into $s.", ch, NULL, victim, TO_NOTVICT );
        ch->hit = UMIN( 100, ch->hit + number_range( 1, 3 ) );
      }
    }
  }
  else
  {
    act( AT_PINK, "You missed $N with your skin trap.", ch, NULL, victim, TO_CHAR );

    act( AT_PINK, "A nodule of flesh detaches from you and soars towards $N,", ch, NULL, victim, TO_CHAR );
    act( AT_PINK, "but $E manages to avoid it, and you re-absorb your skin. &W[$t]", ch, num_punct( 0 ), victim, TO_CHAR );

    act( AT_PINK, "Amidst the raging battle, a flicker of movement catches the", ch, NULL, victim, TO_VICT );
    act( AT_PINK, "corner of your eye.  A large nodule of severed pink flesh soars", ch, NULL, victim, TO_VICT );
    act( AT_PINK, "towards you.  You narrowly avoid it, and $N re-absorbs it into", ch, NULL, victim, TO_VICT );
    act( AT_PINK, "self. &W[$t]", ch, num_punct( 0 ), victim, TO_VICT );

    act( AT_PINK, "A nodule of flesh detaches from $n and soars towards $N,", ch, NULL, victim, TO_NOTVICT );
    act( AT_PINK, "but $E manages to avoid it, and you re-absorb your skin. &W[$t]", ch, num_punct( 0 ), victim,
         TO_NOTVICT );

    learn_from_failure( ch, gsn_skin_trap );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }

  ch->mana -= skill_table[gsn_skin_trap]->min_mana;
  return;
}
