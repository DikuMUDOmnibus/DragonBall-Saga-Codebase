#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

extern void transStatApply args( ( CHAR_DATA * ch, int strMod, int spdMod, int intMod, int conMod ) );
extern void transStatRemove args( ( CHAR_DATA * ch ) );

void do_mutate( CHAR_DATA * ch, char *argument )
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
      ch_printf( ch, "You are unable to mutate while you know mystic.\n\r" );
      return;
    }
  }
  
  if( ch->canmajin )
  {
    ch_printf( ch, "You are unable to mutate while you know majin.\n\r" );
    return;
  }

  if( wearing_chip( ch ) )
  {
    ch_printf( ch, "You can't while you have a chip installed.\n\r" );
    return;
  }

  if( IS_NPC( ch ) || ch->race != 17 )
  {
    send_to_char( "Huh?\n\r", ch );
    return;
  }

  if( argument[0] == '\0' )
  {
    send_to_char( "What mutation do you want to become?\n\r", ch );
    return;
  }

  if( str_prefix( argument, "radiated growth" )
      && str_prefix( argument, "accelerated mutation" ) && str_prefix( argument, "molecular degeneration" ) 
	  && str_prefix( argument, "biohazard" ) && str_prefix( argument, "meta-mutation" ) )
  {
    send_to_char( "You can't mutate into that.\n\r", ch );
    return;
  }

  if( ( !str_prefix( argument, "radiated growth" ) && ch->pcdata->learned[gsn_mutation1] <= 0 )
      || ( !str_prefix( argument, "accelerated mutation" ) && ch->pcdata->learned[gsn_mutation2] <= 0 )
	  || ( !str_prefix( argument, "molecular degeneration" ) && ch->pcdata->learned[gsn_mutation3] <= 0 )
	  || ( !str_prefix( argument, "biohazard" ) && ch->pcdata->learned[gsn_mutation4] <= 0 )
      || ( !str_prefix( argument, "meta-mutation" ) && ch->pcdata->learned[gsn_mutation5] <= 0 ) )
  {
    send_to_char( "You must become more powerful before you can mutate further.\n\r", ch );
    return;
  }

  if( !str_prefix( argument, "radiated growth" ) && ch->pcdata->learned[gsn_mutation1] > 0 )
  {
    if( ch->mana < skill_table[gsn_mutation1]->min_mana )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }

    if( xIS_SET( ch->affected_by, AFF_MUTATION1 ) )
    {
      send_to_char( "You are already in that mutation.\n\r", ch );
      return;
    }

    xSET_BIT( ch->affected_by, AFF_MUTATION1 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION2 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION3 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION4 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION5 );
//    act( AT_SKILL, "You evolve into your semi-perfect form.", ch, NULL, NULL, TO_CHAR );
//    act( AT_SKILL, "$n evolves into $s semi-perfect form.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_PURPLE, "Your body begins to react to the radiation locked deep inside you. An "
		 " unwavering feeling of pain washes over you as your body begins to "
		 " bulge, exponentially increasing your strength, but slowing you and "
		 " draining your intellect.", ch, NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "$n's body begins to react to the radiation locked deep inside. An "
		 " unwavering feeling of pain washes over $m as $s body begins to "
		 " bulge, exponentially increasing $s strength, but slowing $m and "
		 " draining $s intellect.", ch, NULL, NULL, TO_NOTVICT );
    ch->pl = ch->exp * 8;
    transStatApply( ch, 16, 4, 2, 16 );
    ch->mana -= skill_table[gsn_mutation1]->min_mana;
    return;
  }

  if( !str_prefix( argument, "accelerated mutation" ) && ch->pcdata->learned[gsn_mutation2] > 0 )
  {
    if( ch->mana < skill_table[gsn_mutation2]->min_mana )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }

    if( xIS_SET( ch->affected_by, AFF_MUTATION2 ) )
    {
      send_to_char( "You are already in that mutation.\n\r", ch );
      return;
    }

    if( !xIS_SET( ch->affected_by, AFF_MUTATION1 ) )
    {
      send_to_char( "You must be in your first mutation.\n\r", ch );
      return;
    }

    xSET_BIT( ch->affected_by, AFF_MUTATION2 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION1 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION3 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION4 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION5 );
//    act( AT_SKILL, "You evolve into your perfect form.", ch, NULL, NULL, TO_CHAR );
//    act( AT_SKILL, "$n evolves into $s perfect form.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_PURPLE, "Radiation from the previous transformation goes out of control as the growth"
		 " accelerates causing deformities. Your body abnormally enlarges in size,"
		 " making you stronger than before as you go crazy through severe pain your"
		 " body is enduring as you become hideous.", ch, NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "Radiation from the previous transformation goes out of control as the growth"
		 " accelerates causing deformities. Your body abnormally enlarges in size,"
		 " making you stronger than before as you go crazy through severe pain your"
		 " body is enduring as you become hideous.", ch, NULL, NULL, TO_NOTVICT );
    ch->pl = ch->exp * 17;
    transStatApply( ch, 34, 8, 4, 34 );
    ch->mana -= skill_table[gsn_mutation2]->min_mana;
    return;
  }

  if( !str_prefix( argument, "molecular degeneration" ) && ch->pcdata->learned[gsn_mutation3] > 0 )
  {
    if( ch->mana < skill_table[gsn_mutation3]->min_mana )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }

    if( xIS_SET( ch->affected_by, AFF_MUTATION3 ) )
    {
      send_to_char( "You are already in that form.\n\r", ch );
      return;
    }
	
    if( !xIS_SET( ch->affected_by, AFF_MUTATION2 ) )
    {
      send_to_char( "You must be in your second mutation.\n\r", ch );
      return;
    }

    xSET_BIT( ch->affected_by, AFF_MUTATION3 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION1 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION2 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION4 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION5 );
//    act( AT_SKILL, "You evolve into your ultra-perfect form.", ch, NULL, NULL, TO_CHAR );
//    act( AT_SKILL, "$n evolves into $s ultra-perfect form.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_PURPLE, "The radiation starts to break down the outer layer of your skin, causing"
		 " severe aggitation making you go mad as radiated fluid travels throughout"
		 " your body, causing your body to swell even further, strengthening your"
		 " body even more.", ch, NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "The radiation starts to break down the outer layer of $n's skin, causing"
		 " severe aggitation making $m go mad as radiated fluid travels throughout"
		 " $s body, causing $s body to swell even further, strengthening $s"
		 " body even more.", ch, NULL, NULL, TO_NOTVICT );
    ch->pl = ch->exp * 24;
    transStatApply( ch, 48, 12, 6, 48 );
    ch->mana -= skill_table[gsn_mutation3]->min_mana;
    return;
  }
  
  if( !str_prefix( argument, "biohazard" ) && ch->pcdata->learned[gsn_mutation4] > 0 )
  {
    if( ch->mana < skill_table[gsn_mutation4]->min_mana )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }

    if( xIS_SET( ch->affected_by, AFF_MUTATION4 ) )
    {
      send_to_char( "You are already in that mutation.\n\r", ch );
      return;
    }

    if( !xIS_SET( ch->affected_by, AFF_MUTATION3 ) )
    {
      send_to_char( "You must be in your third mutation.\n\r", ch );
      return;
    }

    xSET_BIT( ch->affected_by, AFF_MUTATION4 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION1 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION2 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION3 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION5 );
//    act( AT_SKILL, "You evolve into your perfect form.", ch, NULL, NULL, TO_CHAR );
//    act( AT_SKILL, "$n evolves into $s perfect form.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_PURPLE, "As your body's molecular structure starts to break down from the radiation,"
		 " the fumes that form from your body react in the air as you breathe it in,"
		 " enhancing your every cell in your body to regenerate as it breaks down,"
		 " causing neural instability, but giving you extraordinary strength.", ch, NULL, NULL, TO_CHAR );
    act( AT_PURPLE, "As $n's body's molecular structure starts to break down from the radiation,"
		 " the fumes that form from $s body react in the air as $e breathes it in,"
		 " enhancing every cell in $s body to regenerate instantly as it breaks down,"
		 " causing neural instability, but giving $m extraordinary strength.", ch, NULL, NULL, TO_NOTVICT );
    ch->pl = ch->exp * 33;
    transStatApply( ch, 66, 16, 8, 66 );
    ch->mana -= skill_table[gsn_mutation4]->min_mana;
    return;
  }

  if( !str_prefix( argument, "meta-mutation" ) && ch->pcdata->learned[gsn_mutation5] > 0 )
  {
    if( ch->mana < skill_table[gsn_mutation5]->min_mana )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }

    if( xIS_SET( ch->affected_by, AFF_MUTATION5 ) )
    {
      send_to_char( "You are already in that form.\n\r", ch );
      return;
    }
	
    if( !xIS_SET( ch->affected_by, AFF_MUTATION4 ) )
    {
      send_to_char( "You must be in your fourth mutation.\n\r", ch );
      return;
    }

    xSET_BIT( ch->affected_by, AFF_MUTATION5 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION1 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION2 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION3 );
    xREMOVE_BIT( ch->affected_by, AFF_MUTATION4 );
//    act( AT_SKILL, "You evolve into your ultra-perfect form.", ch, NULL, NULL, TO_CHAR );
//    act( AT_SKILL, "$n evolves into $s ultra-perfect form.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_GREEN, "You begin to glow a neon-green, as the overwhelming pain slowly starts"
		 " to fade away. Your muscles begin to condense themselves as your physique"
		 " gains the appearence resembling that of any other humanoid. Your once"
		 " sluggish and stupid body is now highly intelligent and extraordinarily"
		 " fast, all while keeping your inhuman strength and resiliancy.", ch, NULL, NULL, TO_CHAR );
    act( AT_GREEN, "$n begins to glow a neon-green, as $s overwhelming pain slowly starts"
		 " to fade away. $n's muscles begin to condense themselves as $s"
		 " physique gains the appearence resembling that of any other humanoid."
		 " $n's once sluggish and stupid body is now highly intelligent and"
		 " extraordinarily fast, all while keeping $s inhuman strength and"
		 " resiliancy.", ch, NULL, NULL, TO_NOTVICT );
    ch->pl = ch->exp * 40;
    transStatApply( ch, 80, 80, 80, 80 );
    ch->mana -= skill_table[gsn_mutation5]->min_mana;
    return;
  }
  
  return;
}

void do_eliminate( CHAR_DATA * ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument( argument, arg );

  if( arg[0] == '\0' )
  {
    send_to_char( "Spar whom?\n\r", ch );
    return;
  }

  if( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  if( !IS_NPC( victim ) && !victim->desc )
  {
    send_to_char( "They are link dead, it wouldn't be right.", ch );
    return;
  }

  if( IS_NPC( victim ) )
  {
    send_to_char( "You can only spar other players.", ch );
    return;
  }

  if( victim == ch )
  {
    send_to_char( "You can't spar your self.\n\r", ch );
    return;
  }

  if( !IS_IMMORTAL( ch ) && !IS_NPC( ch ) && !IS_NPC( victim ) )
  {
    if( !xIS_SET( ch->act, PLR_QUESTING ) && xIS_SET( victim->act, PLR_QUESTING ) )
    {
      send_to_char( "You can't spar a player involved in a role playing event.\n\r", ch );
      return;
    }
    if( xIS_SET( ch->act, PLR_QUESTING ) && !xIS_SET( victim->act, PLR_QUESTING ) )
    {
      send_to_char( "You can't spar a player not involved in a role playing event.\n\r", ch );
      return;
    }
  }

  if( !xIS_SET( ch->act, PLR_SPAR ) )
    xSET_BIT( ch->act, PLR_SPAR );
  if( !xIS_SET( victim->act, PLR_SPAR ) )
    xSET_BIT( victim->act, PLR_SPAR );

  if( is_safe( ch, victim, TRUE ) )
    return;

  if( ch->position == POS_RESTING || ch->position == POS_SLEEPING || ch->position == POS_SITTING )
  {
    send_to_char( "How do you propose to do that in your current state?\n\r", ch );
    return;
  }

  if( ch->position == POS_FIGHTING
      || ch->position == POS_EVASIVE
      || ch->position == POS_DEFENSIVE || ch->position == POS_AGGRESSIVE || ch->position == POS_BERSERK )
  {
    send_to_char( "You do the best you can!\n\r", ch );
    return;
  }

  if( !xIS_SET( ch->in_room->room_flags, ROOM_ARENA ) )
  {
    send_to_char( "You must be in an arena to spar someone.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && !IS_NPC( victim ) && ch->exp <= 5000 )
  {
    send_to_char( "You can not fight other players while you are in training.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && !IS_NPC( victim ) && victim->exp <= 5000 )
  {
    send_to_char( "You can not fight other players while they are in training.\n\r", ch );
    return;
  }
  if( who_fighting( victim ) != NULL )
  {
    send_to_char( "It would not be honorable to interfere with some one else's battle.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && !IS_NPC( victim ) && victim->hit < 2 )
  {
    send_to_char( "They are too hurt to fight anymore.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && !IS_NPC( victim ) && ch->hit < 2 )
  {
    send_to_char( "You are too hurt to fight anymore.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && !IS_NPC( victim ) && !IS_HC( ch ) && !IS_HC( victim )
      && xIS_SET( ch->act, PLR_SPAR ) && xIS_SET( victim->act, PLR_SPAR )
      && ( ( IS_GOOD( ch ) && !IS_GOOD( victim ) )
           || ( IS_EVIL( ch ) && !IS_EVIL( victim ) ) || ( IS_NEUTRAL( ch ) && !IS_NEUTRAL( victim ) ) ) )
  {
    send_to_char( "You would not spar someone who is aligned that way.\n\r", ch );
    return;
  }

/*  if( !victim->pcdata->HBTCPartner )
  {
    send_to_char( "They are not accepting sparring partners at this time.\n\r", ch );
    return;
  }

  if( strcasecmp( ch->name, victim->pcdata->HBTCPartner ) )
  {
    send_to_char( "They do not want to spar with you.\n\r", ch );
    return;
  }*/
  WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
  if( !xIS_SET( ch->act, PLR_SPAR ) )
  {
    sprintf( buf, "Help!  I am being attacked by %s!", IS_NPC( ch ) ? ch->short_descr : ch->name );
    if( IS_PKILL( victim ) )
    {
      do_wartalk( victim, buf );
    }
    else
    {
      do_yell( victim, buf );
    }
  }
  check_illegal_pk( ch, victim );

  ch->spar_start = ch->exp;
  victim->spar_start = victim->exp;
  ch->delay = 0;
  check_attacker( ch, victim );
  multi_hit( ch, victim, TYPE_UNDEFINED );
  return;


}
