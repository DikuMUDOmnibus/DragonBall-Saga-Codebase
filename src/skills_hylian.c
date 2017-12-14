/***************************************************************************************************/
/*                                    HYRULIAN SKILLS v2.0                                         */
/*                                          Coded By                                               */
/*                                          Kalthizar                                                */
/* Attacks:  											   */
/*        'Shieldbash'      'SongOfCreation'      'Bomb'      'Light Arrow'      'Fire Arrow'	   */
/*	    'Ice Arrow'	'Farore's Wind'								   */
/*												   */
/* Transformations:                         							   */
/*                                                                                                 */
/*  'Song Of Time'      'Deku Mask'      'Goron Mask'      'Zora Mask'      'Fierce Deity Mask'    */
/*                                                                                                 */
/***************************************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

extern void transStatApply args( ( CHAR_DATA * ch, int strMod, int spdMod, int intMod, int conMod ) );
extern void transStatRemove args( ( CHAR_DATA * ch ) );

void do_shieldbash(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_shieldbash ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_shieldbash]->skill_level[ch->class] )
    {
	send_to_char("You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_shieldbash]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_shieldbash]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_shieldbash]->focus;

    WAIT_STATE( ch, skill_table[gsn_shieldbash]->beats );

    sh_int z = get_aura(ch);

    if ( can_use_skill(ch, number_percent(),gsn_shieldbash ) )
    {
	dam = get_attmod(ch, victim) * number_range( 8, 12 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( z, "You run up and bash $N over the head with your shield. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( z, "$n bashes you over the head with $s shield. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( z, "$n bashes $N over the head with $s shield. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );
        dam = ki_absorb( victim, ch, dam, gsn_shieldbash );
	learn_from_success( ch, gsn_shieldbash );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( z, "You missed $N with your shieldbash.", ch, NULL, victim, TO_CHAR );
	act( z, "$n misses you with $s shieldbash.", ch, NULL, victim, TO_VICT );
	act( z, "$n missed $N with $s shieldbash.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_shieldbash );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }

    if( !is_android_h(ch) )
      ch->mana -= skill_table[gsn_shieldbash]->min_mana;
    return;
}

void do_faroreswind( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  CHAR_DATA *fch;
  CHAR_DATA *fch_next;
  ROOM_INDEX_DATA *prev_room;

//  if( !str_cmp( get_race( ch ), "kaio" ) || !str_cmp( get_race( ch ), "yardratian" ) )
  if( IS_KAIO( ch ) || is_yardratian( ch ) )
  {
    if( ch->mana < ( skill_table[gsn_faroreswind]->min_mana / 5 ) )
    {
      send_to_char( "You don't have enough energy.\n\r", ch );
      return;
    }
  }
  else
  {
    if( ch->mana < skill_table[gsn_faroreswind]->min_mana )
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
    ch_printf( ch, "&RThis room is blocking your ability to call upon Farore.\n\r" );
    return;
  }

  WAIT_STATE( ch, skill_table[gsn_faroreswind]->beats );

  if( ( victim = get_char_ki_world( ch, argument ) ) == NULL
      || !can_astral( ch, victim )
      || !in_hard_range( ch, victim->in_room->area )
      || !in_soft_range( ch, victim->in_room->area )
      || xIS_SET( victim->in_room->room_flags, ROOM_PROTOTYPE )
      || ( is_android( victim ) && !wearing_sentient_chip( victim ) )
      || ( is_superandroid( victim ) && !wearing_sentient_chip( victim ) ) || ( victim->rank > 0 ) || IS_IMMORTAL( victim ) )
  {
    send_to_char( "You fail to call upon Farore's powers.\n\r", ch );
    return;
  }

  if( is_split( victim ) )
  {
    send_to_char( "You fail to call upon Farore's powers.\n\r", ch );
    return;
  }

  if( !IS_NPC( victim ) && !victim->desc )
  {
    send_to_char( "Farore's powers will not let you do that.\n\r", ch );
    return;
  }

  if( !victim->in_room )
  {
    ch_printf( ch, "You fail to call upon Farore's powers.\n\r" );
    return;
  }

  if( ch->in_room == victim->in_room )
  {
    ch_printf( ch, "You're already there!\n\r" );
    return;
  }
  if( victim->master == ch )
  {
    ch_printf( ch, "An avatar of Farore appears before you and slaps you for trying to use her powers unwisely.\n\r" );
    return;
  }

  if( can_use_skill( ch, number_percent(  ), gsn_faroreswind ) )
  {
    learn_from_success( ch, gsn_faroreswind );
    act( AT_GREEN, "You begin to call upon Farore's powers.", ch, NULL, NULL, TO_CHAR );
    act( AT_GREEN, "$n holds a Green Jewel in their hands and begins to chant something.", ch, NULL, NULL, TO_ROOM );
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
          act( AT_GREEN, "You begin to call upon Farore's powers.", fch, NULL, ch, TO_CHAR );
          act( AT_GREEN, "$n holds a Green Jewel in their hands and begins to chant something.", fch, NULL, ch, TO_ROOM );
        }
        else
        {
          act( AT_GREEN, "You begin to call upon Farore's powers. Omigawd u hax0r!", fch, NULL, ch, TO_CHAR );
          act( AT_GREEN, "$n holds a Green Jewel in their hands and begins to chant something. OMGHAX!!!", fch, NULL, ch, TO_ROOM );
        }
        char_to_room( fch, victim->in_room );
        act( AT_GREEN, "You enter the portal and emerge at your destination.", fch, NULL, ch, TO_CHAR );
        act( AT_GREEN, "$n walks out of a glowing portal.", fch, NULL, ch, TO_ROOM );
        do_look( fch, "auto" );
      }
    }
    act( AT_GREEN, "You enter the portal and emerge at your destination.", ch, NULL, NULL, TO_CHAR );
    act( AT_GREEN, "$n walks out of a glowing portal.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
  }
  else
  {
    learn_from_failure( ch, gsn_faroreswind );
    send_to_char( "&gYou fail to call upon Farore's powers.\n\r", ch );
  }
  if( !is_android_h( ch ) )
  {
    if( IS_KAIO( ch ) || is_yardratian( ch ) )
      ch->mana -= ( skill_table[gsn_faroreswind]->min_mana / 5 );
    else
      ch->mana -= skill_table[gsn_faroreswind]->min_mana;
  }
  return;
}

void do_songofcreation(CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *o;
    OBJ_INDEX_DATA *pObjIndex;

    if( IS_NPC(ch) )
          return;

    if( argument[0] == '\0' )
    {
	ch_printf(ch,"\n\rSyntax: songofcreation <item>\n\r");
	ch_printf(ch,"\n\r");
	ch_printf(ch,"Item can only be one of the following:\n\r");
	ch_printf(ch,"\n\r");
	ch_printf(ch,"armor leggings sword shield\n\r");
	return;
    }

    if( !(!str_cmp(argument,"armor")) && !(!str_cmp(argument,"leggings")) &&
	!(!str_cmp(argument,"sword")) && !(!str_cmp(argument,"shield")) )
    {
	do_songofcreation(ch,"");
	return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
        send_to_char( "You can't concentrate enough for that.\n\r", ch );
        return;
    }
    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_songofcreation]->skill_level[ch->class] )
    {
        send_to_char( "You can't do that.\n\r", ch );
        return;
    }
    if ( ch->mana < skill_table[gsn_songofcreation]->min_mana )
    {
        send_to_char( "You don't have enough energy.\n\r", ch );
        return;
    }
    if( ch->carry_number >= 19 )
    {
	ch_printf(ch,"You haven't got any room.\n\r");
	return;
    }


    WAIT_STATE( ch, skill_table[gsn_songofcreation]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_songofcreation ) )
    {
	int a = 0;
	if( !str_cmp(argument,"armor") )
	  a = 1345;
	else if( !str_cmp(argument,"leggings") )
	  a = 1346;
	else if( !str_cmp(argument,"sword") )
	  a = 1343;
	else if( !str_cmp(argument,"shield") )
	  a = 1344;
	if( a == 0 )
	{
	  bug("Serious problem in function Song of Creation",0);
	  return;
	}
	act( AT_SKILL, "You put your Orcarina to your mouth and play a beautiful song. An Item apears from nowhere.", ch, NULL, NULL, TO_CHAR );
	act( AT_SKILL, "$n plays a beautiful song on $s Orcarina. An Item appears out of nowhere.", ch, NULL, NULL, TO_NOTVICT );
        learn_from_success( ch, gsn_songofcreation );
	pObjIndex = get_obj_index( a );
	o = create_object_new( pObjIndex, 1 , ORIGIN_OINVOKE, ch->name );
	o = obj_to_char( o, ch );
	save_char_obj( ch );
    }
    else
    {
         act( AT_SKILL, "You play a somewhat odd song on your Orcarina, but nothing happens.", ch, NULL, NULL, TO_CHAR );
         act( AT_SKILL, "$n plays a somewhat off song on $s Orcarina, but nothing happens.", ch, NULL, NULL, TO_NOTVICT );
        learn_from_failure( ch, gsn_songofcreation );
    }
    ch->mana -= skill_table[gsn_songofcreation]->min_mana;
    return;
}

void do_hbomb(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_bomb))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && ch->exp < skill_table[gsn_bomb]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
	}

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_bomb]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_bomb]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_bomb]->focus;

    WAIT_STATE( ch, skill_table[gsn_bomb]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_bomb ) )
    {
	switch (number_range(1,100))
	{
    case 100:
    case 99:
    case 98:
 	if (IS_NPC(victim) || (!IS_NPC(victim) && (!xIS_SET(ch->act, PLR_SPAR) || !xIS_SET(ch->act, PLR_SPAR) )))
 	{

		/* Redone so that instant death can't be abused to instant kill
		 * players over 2x their power. -- Islvin
		 */
		if (victim->pl / ch->pl >= 2)
		{
			act( AT_RED, "&zYou throw a bomb at $N.", ch, NULL, victim, TO_CHAR );
			act( AT_RED, "&z$N notices the incoming bomb and quickly ducks for cover. You missed.", ch, NULL, victim, TO_CHAR );
			act( AT_RED, "&z$n throws a bomb at you.", ch, NULL, victim, TO_VICT );
			act( AT_RED, "&zYou notice the incoming bomb and quickly dive for cover. $n misses.", ch, NULL, victim, TO_VICT );
			act( AT_RED, "&z$n throws a bomb at $N.", ch, NULL, victim, TO_NOTVICT );
			act( AT_RED, "&z$N notices the incoming bomb and quickly ducks for cover. $n misses.", ch, NULL, victim, TO_NOTVICT );
			dam = 0;
			break;
		}

/* Redone so it _is_ instant death -- Melora
 * 	if (victim->max_hit >= victim->hit)
 * 		dam = victim->max_hit * 2;
 * 	else
 * 		dam = victim->hit * 2;
 */
		dam = 999999999;
	act( AT_RED, "&z$N swings at you and misses, but during the excitement a powder keg falls out of your bag.", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "&zThe powder keg rolls right towards $N, resting at $S feet, but $E has no time to get away. It exploads!!", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "&zYou swing at $n and miss, but during the excitement a powder keg falls out of $s bag.", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "&zThe powder keg rolls right towards You, resting at your feet, but you have no time to get away. It exploads!!", ch, NULL, victim, TO_VICT );
	act( AT_RED, "&z$N swings at $n and misses, but during the excitement a poweder keg falls out of $n's bag.", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "&zThe poweder keg rolls right towards $N, resting at $S feet, but $E has no time to get away. It exploads!!", ch, NULL, victim, TO_NOTVICT );

	learn_from_success( ch, gsn_bomb );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
    }
	default:
	dam = get_attmod(ch, victim) * number_range( 12, 24 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_RED, "&zYou throw a bomb at $N, it exploads in $S face. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_RED, "&z$n throws a bomb at You, it exploads in your face. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_RED, "&z$n throws a bomb at $N, it exploads in $S face. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

	learn_from_success( ch, gsn_bomb );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
    }
    }
    else
    {
	act( AT_RED, "You missed $N with your bomb.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n misses you with $s bomb.", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$n missed $N with $s bomb.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_bomb );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_bomb]->min_mana;
    return;
}

void do_bombarrow(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_bombarrow))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && ch->exp < skill_table[gsn_bombarrow]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
	}

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_bombarrow]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_bombarrow]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_bombarrow]->focus;

    WAIT_STATE( ch, skill_table[gsn_bombarrow]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_bombarrow ) )
    {
	switch (number_range(1,100))
	{
    case 100:
    case 99:
    case 98:
    case 97:
    case 96:
 	if (IS_NPC(victim) || (!IS_NPC(victim) && (!xIS_SET(ch->act, PLR_SPAR) || !xIS_SET(ch->act, PLR_SPAR) )))
 	{

		/* Redone so that instant death can't be abused to instant kill
		 * players over 2x their power. -- Islvin
		 */
		if (victim->pl / ch->pl >= 2)
		{
			act( AT_RED, "&zYou ready a bomb arrow into your bow and pull back, lining up your shot,", ch, NULL, victim, TO_CHAR );
			act( AT_RED, "&zthen release, but $N notices the incoming arrow and quickly deflects it. You missed.", ch, NULL, victim, TO_CHAR );
			act( AT_RED, "&z$n readies a bomb arrow into $s bow and pulls back, lining up $s shot,", ch, NULL, victim, TO_VICT );
			act( AT_RED, "&zYou notice the incoming arrow and quickly deflect the arrow. $n misses.", ch, NULL, victim, TO_VICT );
			act( AT_RED, "&z$n readies a bomb arrow into $s bow and pulls back, lining up $s shot,", ch, NULL, victim, TO_NOTVICT );
			act( AT_RED, "&z$N notices the incoming arrow and quickly deflects it. $n misses.", ch, NULL, victim, TO_NOTVICT );
			dam = 0;
			break;
		}

/* Redone so it _is_ instant death -- Melora
 * 	if (victim->max_hit >= victim->hit)
 * 		dam = victim->max_hit * 2;
 * 	else
 * 		dam = victim->hit * 2;
 */
		dam = 999999999;
	act( AT_RED, "&zYou ready a bomb arrow in your bow and pull back, lining your shot, then you relase it quickly.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "&zThe arrow pierces $N's skin and you smirk, watching the fuse burn away. Suddenly, $N's body", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "&zEXPLODES into many tiny pieces.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "&z$n readies a bomb arrow in $s bow and pulls back, lining the shot, then $e relases it quickly.", ch, NULL, victim, TO_VICT );
	act( AT_RED, "&zThe arrow pierces your skin and $n smirks, watching the fuse burn away. Suddenly, your body", ch, NULL, victim, TO_VICT );
	act( AT_RED, "&zEXPLODES into many tiny pieces. Your Dead.", ch, NULL, victim, TO_VICT );
	act( AT_RED, "&z$n readies a bomb arrow in $s bow and pulls back, lining the shot, then $e relases it quickly.", ch, NULL, victim, TO_NOTVICT );
	act( AT_RED, "&zThe arrow pierces $N's skin and $n smirks, watching the fuse burn away. Suddenly, $S body", ch, NULL, victim, TO_NOTVICT );
	act( AT_RED, "&zEXPLODES into many tiny pieces. Your Dead.&D", ch, NULL, victim, TO_NOTVICT );

	learn_from_success( ch, gsn_bombarrow );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
    }
	default:
	dam = get_attmod(ch, victim) * number_range( 60, 90 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_RED, "&zYou fire a bomb arrow at $N, it pierces $S skin, but does not detonate. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_RED, "&z$n throws a bomb at You, is pierces your skin, but does not detonate. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_RED, "&z$n throws a bomb at $N, it pierces $S skin, but does not detonate. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

	learn_from_success( ch, gsn_bombarrow );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
    }
    }
    else
    {
	act( AT_RED, "You missed $N with your bomb arrow.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n misses you with $s bomb arrow.", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$n missed $N with $s bomb arrow.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_bombarrow );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_bombarrow]->min_mana;
    return;
}

void do_lightarrow(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_lightarrow ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_lightarrow]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_lightarrow]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_lightarrow]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_lightarrow]->focus;

    WAIT_STATE( ch, skill_table[gsn_lightarrow]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_lightarrow ) )
    {
	dam = get_attmod(ch, victim) * number_range( 80, 90 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_LBLUE, "&zYou knock an arrow into your bow, it begins shining like the &YSun&z.", ch, NULL, victim, TO_CHAR );
	act( AT_LBLUE, "&zYou draw back and release, the &YA&Wr&Yr&Wo&Yw &YO&Wf &YL&Wi&Yg&Wh&Yt&z peirces $N's skin. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_LBLUE, "&z$n knocks an arrow into $s bow, it begins shining like the &YSun&z.", ch, NULL, victim, TO_VICT );
	act( AT_LBLUE, "&z$n draws back and releases, the &YA&Wr&Yr&Wo&Yw &YO&Wf &YL&Wi&Yg&Wh&Yt&z peirces Your skin. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_LBLUE, "&z$n knocks an arrow into $s bow, it begins shining like the &YSun&z. ", ch, NULL, victim, TO_NOTVICT );
	act( AT_LBLUE, "&z$n draws back and releases, the &YA&Wr&Yr&Wo&Yw &YO&Wf &YL&Wi&Yg&Wh&Yt&z peirces $N's skin. [$t]", ch, num_punct(dam), victim, TO_NOTVICT );

        dam = ki_absorb( victim, ch, dam, gsn_lightarrow );
	learn_from_success( ch, gsn_lightarrow );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_LBLUE, "You missed $N with your Light Arrow.", ch, NULL, victim, TO_CHAR );
	act( AT_LBLUE, "$n misses you with $s Light Arrow.", ch, NULL, victim, TO_VICT );
	act( AT_LBLUE, "$n missed $N with $s Light Arrow.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_lightarrow );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_lightarrow]->min_mana;
    return;
}

void do_firearrow(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_firearrow))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_firearrow]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_firearrow]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_firearrow]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_firearrow]->focus;

    WAIT_STATE( ch, skill_table[gsn_firearrow]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_firearrow ) )
    {
	dam = get_attmod(ch, victim) * number_range( 45, 55 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);

	act( AT_BLUE, "&zYou knock an arrow into your bow, it is engulfed in &RFire&z.", ch, NULL, victim, TO_CHAR );
	act( AT_BLUE, "&zYou draw back and release, the &RA&Wr&Rr&Wo&Rw O&Wf &RF&Wi&Rr&We peirces $N's skin. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_BLUE, "&z$n knocks an arrow into $s bow, it is engulfed in &RFire&z.", ch, NULL, victim, TO_VICT );
	act( AT_BLUE, "&z$n draws back and releases, the &RA&Wr&Rr&Wo&Rw O&Wf &RF&Wi&Rr&We peirces Your skin. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_BLUE, "&z$n knocks an arrow into $s bow, it is engulfed in &RFire&z.", ch, NULL, victim, TO_NOTVICT );
	act( AT_BLUE, "&z$n draws back and releases, the &RA&Wr&Rr&Wo&Rw O&Wf &RF&Wi&Rr&We peirces $N's skin. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

        dam = ki_absorb( victim, ch, dam, gsn_firearrow );
	learn_from_success( ch, gsn_firearrow );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_BLUE, "You missed $N with your Fire Arrow.", ch, NULL, victim, TO_CHAR );
	act( AT_BLUE, "$n misses you with $s Fire Arrow.", ch, NULL, victim, TO_VICT );
	act( AT_BLUE, "$n missed $N with $s Fire Arrow.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_firearrow );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_firearrow]->min_mana;
    return;
}

void do_icearrow(CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
	if( !ch->master )
	  return;
	if( !can_use_skill( ch->master, number_percent(), gsn_icearrow ))
	  return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_icearrow]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_icearrow]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_icearrow]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_icearrow]->focus;

    WAIT_STATE( ch, skill_table[gsn_icearrow]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_icearrow ) )
    {
	dam = get_attmod(ch, victim) * number_range( 60, 70 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);

	act( AT_BLUE, "&zYou knock an arrow into your bow, it &CFreezes&z solid.", ch, NULL, victim, TO_CHAR );
	act( AT_BLUE, "&zYou draw back and release, the &CA&Wr&Cr&Wo&Cw O&Wf &CI&Wc&Ce&z peirces $N's skin. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_BLUE, "&z$n knocks an arrow into $s bow, it &CFreezes&z solid.", ch, NULL, victim, TO_VICT );
	act( AT_BLUE, "&z$n draws back and releases, the &CA&Wr&Cr&Wo&Cw O&Wf &CI&Wc&Ce&z peirces Your skin. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_BLUE, "&z$n knocks an arrow into $s bow, it &CFreezes&z solid.", ch, NULL, victim, TO_NOTVICT );
	act( AT_BLUE, "&z$n draws back and releases, the &CA&Wr&Cr&Wo&Cw O&Wf &CI&Wc&Ce&z peirces $N's skin. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

        dam = ki_absorb( victim, ch, dam, gsn_icearrow );
	learn_from_success( ch, gsn_icearrow );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_BLUE, "You missed $N with your Ice Arrow.", ch, NULL, victim, TO_CHAR );
	act( AT_BLUE, "$n misses you with $s Ice Arrow.", ch, NULL, victim, TO_VICT );
	act( AT_BLUE, "$n missed $N with $s Ice Arrow.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_icearrow );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_icearrow]->min_mana;
	return;
}

void do_hoverboots (CHAR_DATA *ch, char *argument)
{
    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_hoverboots ))
          return;
    }

	if (xIS_SET((ch)->affected_by, AFF_FLYING))
	    {
		send_to_char( "You float down and land on the ground.\n\r", ch );
		xREMOVE_BIT((ch)->affected_by, AFF_FLYING);
		return;
    	}

	if ( ch->mana < skill_table[gsn_hoverboots]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	
    WAIT_STATE( ch, skill_table[gsn_hoverboots]->beats );
	if ( can_use_skill(ch, number_percent(), gsn_hoverboots ) )
	{
	act( AT_SKILL, "As you where your Hover Boots, You begin to float a foot off the ground....", ch, NULL, NULL, TO_CHAR );
	act( AT_SKILL, "As $n wears $s Hover Boots, $e begins to float a foot off the ground....", ch, NULL, NULL, TO_NOTVICT );
	xSET_BIT( (ch)->affected_by, AFF_FLYING );
	learn_from_success( ch, gsn_hoverboots );
    }
    else
    {
	act( AT_GREEN, "You try to put on your Hover Boots but you can't tie the laces.", ch, NULL, NULL, TO_CHAR );
	act( AT_GREEN, "$n tries to put on $s Hover Boots, but $e can't figure out how to tie the laces", ch, NULL, NULL, TO_NOTVICT );
	learn_from_failure( ch, gsn_hoverboots );
    }
	ch->mana -= skill_table[gsn_hoverboots]->min_mana;	
    return;
}

void do_hookshot(CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
	if( !ch->master )
	  return;
	if( !can_use_skill( ch->master, number_percent(), gsn_hookshot ))
	  return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_hookshot]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_hookshot]->min_mana )
	{
	    send_to_char( "You don't have enough magic.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_hookshot]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_hookshot]->focus;

    WAIT_STATE( ch, skill_table[gsn_hookshot]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_hookshot ) )
    {
	dam = get_attmod(ch, victim) * number_range( 30, 40 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);

	act( AT_BLUE, "&zYou reach into your bag and equip your Hook Shot.", ch, NULL, victim, TO_CHAR );
	act( AT_BLUE, "&zYou fire it at $N, it dig's deep into $S back, pulling you foward to maul $M. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_BLUE, "&z$n reaches into $s bag and equips $s Hook Shot.", ch, NULL, victim, TO_VICT );
	act( AT_BLUE, "&z$n fires it at you, it dig's deep into your back, pulling $n foward to maul you. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_BLUE, "&z$n reaches into $s bag and equips $s Hook Shot.", ch, NULL, victim, TO_NOTVICT );
	act( AT_BLUE, "&z$n fires it at $N, it dig's deep into $S back, pulling $n foward to maul $M. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

        dam = ki_absorb( victim, ch, dam, gsn_hookshot );
	learn_from_success( ch, gsn_hookshot );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_BLUE, "You missed $N with your Hook Shot.", ch, NULL, victim, TO_CHAR );
	act( AT_BLUE, "$n misses you with $s Hook Shot.", ch, NULL, victim, TO_VICT );
	act( AT_BLUE, "$n missed $N with $s Hook Shot.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_hookshot );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_hookshot]->min_mana;
	return;
}

void do_fury(CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
	if( !ch->master )
	  return;
	if( !can_use_skill( ch->master, number_percent(), gsn_hookshot ))
	  return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_fury]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_fury]->min_mana )
	{
	    send_to_char( "You don't have enough magic.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_fury]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_fury]->focus;

    WAIT_STATE( ch, skill_table[gsn_fury]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_fury ) )
    {
	dam = get_attmod(ch, victim) * number_range( 70, 100 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);

	act( AT_BLUE, "&zYou grasp your sword in your hands tightly.", ch, NULL, victim, TO_CHAR );
	act( AT_BLUE, "&RYou rush at $N, swinging the sword in a flawless 3-hit-combination attack, critically striking $M! &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_BLUE, "&z$n grasps $s sword tightly in $s hands.", ch, NULL, victim, TO_VICT );
	act( AT_BLUE, "&R$n rushes at you, swinging the sword in a flawless 3-hit-combination attack, critically striking you! &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_BLUE, "&z$n grasps $s sword tightly in $s hands.", ch, NULL, victim, TO_NOTVICT );
	act( AT_BLUE, "&R$n rushes at $N, swinging the sword in a flawless 3-hit-combination attack, critically striking $M! &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

        dam = ki_absorb( victim, ch, dam, gsn_fury );
	learn_from_success( ch, gsn_fury );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_BLUE, "You missed $N with your combo.", ch, NULL, victim, TO_CHAR );
	act( AT_BLUE, "$n misses you with $s combo.", ch, NULL, victim, TO_VICT );
	act( AT_BLUE, "$n missed $N with $s combo.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_fury );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_fury]->min_mana;
	return;
}

void do_finisher(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_bombarrow))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && ch->exp < skill_table[gsn_finisher]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
	}

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_finisher]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_finisher]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_finisher]->focus;

    WAIT_STATE( ch, skill_table[gsn_finisher]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_finisher ) )
    {
	switch (number_range(1,100))
	{
    case 100:
    case 99:
 	if (IS_NPC(victim) || (!IS_NPC(victim) && (!xIS_SET(ch->act, PLR_SPAR) || !xIS_SET(ch->act, PLR_SPAR) )))
 	{

		/* Redone so that instant death can't be abused to instant kill
		 * players over 2x their power. -- Islvin
		 */
		if (victim->pl / ch->pl >= 2)
		{
			act( AT_RED, "&zYou jump high into the air and ready your sword. As you begin to descend, you hold your sword downward, and throw your entire weight on the sword, but $N sidesteps and evades. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
			act( AT_RED, "&z$n jumps high into the air and readies $s sword. As $e begins to descend, $e holds $s sword downward, and throws $s entire weight on the sword, but you sidestep and evade $n! &W[$t]", ch, num_punct(dam), victim, TO_VICT );
			act( AT_RED, "&z$n jumps high into the air and readies $s sword. As $e begins to descend, $e holds $s sword downward, and throws $s entire weight on the sword, but $N sidesteps and evades $n! &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );
			dam = 0;
			break;
		}

/* Redone so it _is_ instant death -- Melora
 * 	if (victim->max_hit >= victim->hit)
 * 		dam = victim->max_hit * 2;
 * 	else
 * 		dam = victim->hit * 2;
 */
		dam = 999999999;
	act( AT_RED, "&zYou jump high into the air and ready your sword. As you begin to descend, you hold your sword downward, and throw your entire weight on the sword, plunging it straight into $N's chest, killing $M INSTANTLY!!! &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_RED, "&z$n jumps high into the air and readies $s sword. As $e begins to descend, $e holds $s sword downward, and throws $s entire weight on the sword, plunging it straight into your chest, killing you INSTANTLY!!! &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_RED, "&z$n jumps high into the air and readies $s sword. As $e begins to descend, $e holds $s sword downward, and throws $s entire weight on the sword, plunging it straight into $N's chest, killing $M INSTANTLY!!! &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

	learn_from_success( ch, gsn_bombarrow );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
    }
	default:
	dam = get_attmod(ch, victim) * number_range( 90, 120 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_RED, "&zYou jump high into the air and ready your sword. As you begin to descend, you hold your sword downward, and throw your entire weight on the sword, plunging it straight into $N. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_RED, "&z$n jumps high into the air and readies $s sword. As $e begins to descend, $e holds $s sword downward, and throws $s entire weight on the sword, plunging it straight into you! &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_RED, "&z$n jumps high into the air and readies $s sword. As $e begins to descend, $e holds $s sword downward, and throws $s entire weight on the sword, plunging it straight into $N! &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

	learn_from_success( ch, gsn_finisher );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
    }
    }
    else
    {
	act( AT_RED, "You missed $N with your finisher.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n misses you with $s finisher.", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$n missed $N with $s finisher.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_finisher );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_finisher]->min_mana;
    return;
}

/*****************************/
/*     Transformations       */
/*****************************/

void do_songoftime(CHAR_DATA *ch, char *argument )
{
    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
	  return;
        if( !can_use_skill( ch->master, number_percent(), gsn_dekumask ))
	  return;
    }

    if( !IS_NPC(ch) )
    {
      if( IS_SET(ch->pcdata->flags, PCFLAG_KNOWSMYSTIC) )
      {
	ch_printf(ch,"You are unable to call upon those powers while you know mystic.\n\r");
	return;
      }
    }

    if( wearing_chip(ch) )
    {
        ch_printf(ch," You can't while you have a chip installed.\n\r");
        return;
    }

      if (xIS_SET((ch)->affected_by, AFF_DEKU)
		|| xIS_SET((ch)->affected_by, AFF_GORON)
		|| xIS_SET((ch)->affected_by, AFF_ZORA)
		|| xIS_SET((ch)->affected_by, AFF_FDEITY))
	    {
		send_to_char( "You can't become an adult with a mask on.\n\r", ch );
		return;
		}

        if (xIS_SET((ch)->affected_by, AFF_ADULT))
            {
                send_to_char( "You play the Song of Time again and return to being a kid.\n\r", ch );
		if (xIS_SET((ch)->affected_by, AFF_ADULT))
                xREMOVE_BIT((ch)->affected_by, AFF_ADULT);
                ch->pl = ch->exp;
                transStatRemove(ch);
                return;
            }
        if ( ch->mana < skill_table[gsn_songoftime]->min_mana )
        {
            send_to_char( "You don't have enough energy.\n\r", ch );
            return;
        }

    WAIT_STATE( ch, skill_table[gsn_songoftime]->beats );
   if ( can_use_skill(ch, number_percent(), gsn_songoftime ) )
        {
        act( AT_RED, "&CYou begin to play the Song of Time, everything starts shifting all around you, when you are finished, you have become an Adult.", ch, NULL, NULL, TO_CHAR );
        act( AT_RED, "&z$n whips out his Ocarina and begins playing a beautiful song. You lose all memory of what happens next, but when $e is done, $e looks alot older than $e was before.", ch, NULL, NULL, TO_NOTVICT );
        xSET_BIT( (ch)->affected_by, AFF_ADULT );
            xREMOVE_BIT((ch)->affected_by, AFF_DEKU);
            xREMOVE_BIT((ch)->affected_by, AFF_ZORA);
            xREMOVE_BIT((ch)->affected_by, AFF_GORON);
            xREMOVE_BIT((ch)->affected_by, AFF_FDEITY);
        //ch->pl = ch->exp * 10;   /* Only commented out until Fierce Deity is fixed -Arbin */
        //transStatApply(ch, 12, 15, 12, 12);
        ch->pl = ch->exp * 42;
        transStatApply(ch, 42, 42, 42, 42);

        learn_from_success( ch, gsn_songoftime );
    }
     else 
     {
     switch (number_range(1, 3))
     {
             default: 
             act( AT_RED, "&zYour notes echo far, but nothing happens.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&z$n plays $s Orcarina, but nothing happens.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 1:
             act( AT_RED, "&zYour notes echo far, but nothing happens.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&z$n plays $s Orcarina, but nothing happens.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 2:
             act( AT_RED, "&zYour making that song up, aren't you?", ch, NULL, NULL, TO_CHAR );
             break;
             case 3:
             act( AT_RED, "&zYour notes echo far, but nothing happens.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&z$n plays $s Orcarina, but nothing happens.", ch, NULL, NULL, TO_NOTVICT );
             break;
     }
     learn_from_failure( ch, gsn_songoftime );
    }
     ch->mana -= skill_table[gsn_songoftime]->min_mana;
     return;
}

void do_dekumask(CHAR_DATA *ch, char *argument )
{
    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
	  return;
        if( !can_use_skill( ch->master, number_percent(), gsn_dekumask ))
	  return;
    }

    if( !IS_NPC(ch) )
    {
      if( IS_SET(ch->pcdata->flags, PCFLAG_KNOWSMYSTIC) )
      {
	ch_printf(ch,"You are unable to call upon those powers while you know mystic.\n\r");
	return;
      }
    }

    if( wearing_chip(ch) )
    {
        ch_printf(ch," You can't while you have a chip installed.\n\r");
        return;
    }

    if (xIS_SET((ch)->affected_by, AFF_ADULT))
    {
		send_to_char( "You can't wear this mask unless you are a kid.\n\r", ch );
		return;
    }

         if (xIS_SET((ch)->affected_by, AFF_DEKU))
	    {
		send_to_char( "You Remove your Mask.\n\r", ch );
            xREMOVE_BIT((ch)->affected_by, AFF_DEKU);
            ch->pl = ch->exp;
            transStatRemove(ch);
            if( !IS_NPC( ch ) )
            return;
        }

        if ( ch->mana < skill_table[gsn_dekumask]->min_mana )
        {
            send_to_char( "You don't have enough energy.\n\r", ch );
            return;
        }

    WAIT_STATE( ch, skill_table[gsn_dekumask]->beats );
   if ( can_use_skill(ch, number_percent(), gsn_dekumask ) )
        {
        act( AT_RED, "&GAs You wear the Deku Mask, You scream in horror as your body takes on the form of a tiny Deku Scrub.", ch, NULL, NULL, TO_CHAR );
        act( AT_RED, "&GAs $n wears the Deku Mask, $e screams in horror as $s body takes on the form of a tiny Deku Scrub.", ch, NULL, NULL, TO_NOTVICT );
        xSET_BIT( (ch)->affected_by, AFF_DEKU );
		    xREMOVE_BIT((ch)->affected_by, AFF_FDEITY);
            xREMOVE_BIT((ch)->affected_by, AFF_ZORA);
            xREMOVE_BIT((ch)->affected_by, AFF_GORON);
            xREMOVE_BIT((ch)->affected_by, AFF_ADULT);
        ch->pl = ch->exp * 8;
        transStatApply(ch, 2, 12, 10, 10);
        learn_from_success( ch, gsn_dekumask );
    }
     else 
     {
     switch (number_range(1, 3))
     {
             default: 
             act( AT_RED, "&GYou start to put the Deku Mask on, but you accidently drop it.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&G$n starts to put $s Deku Mask on, but $e accidently drops it.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 1:
             act( AT_RED, "&GYou start to put the Deku Mask on, but you accidently drop it.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&G$n starts radiating some extreme heat, but it dissappears quickly.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 2:
             act( AT_RED, "&GThat mask doesn't really have any powers.", ch, NULL, NULL, TO_CHAR );
             break;
             case 3:
             act( AT_RED, "&GYou start to put the Deku Mask on, but you accidently drop it.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&G$n starts to put $s Deku Mask on, but $e accidently drops it.", ch, NULL, NULL, TO_NOTVICT );
             break;
     }
     learn_from_failure( ch, gsn_dekumask );
    }
     ch->mana -= skill_table[gsn_dekumask]->min_mana;
     return;
}

void do_goronmask(CHAR_DATA *ch, char *argument )
{
    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
	  return;
        if( !can_use_skill( ch->master, number_percent(), gsn_goronmask ))
	  return;
    }

    if( !IS_NPC(ch) )
    {
      if( IS_SET(ch->pcdata->flags, PCFLAG_KNOWSMYSTIC) )
      {
	ch_printf(ch,"You are unable to call upon those powers while you know mystic.\n\r");
	return;
      }
    }

    if( wearing_chip(ch) )
    {
        ch_printf(ch," You can't while you have a chip installed.\n\r");
        return;
    }

    if (xIS_SET((ch)->affected_by, AFF_ADULT))
    {
		send_to_char( "You can't wear this mask unless you are a kid.\n\r", ch );
		return;
    }

        if (xIS_SET((ch)->affected_by, AFF_GORON))
	  {
	    send_to_char( "You remove your mask.\n\r", ch );
            xREMOVE_BIT((ch)->affected_by, AFF_GORON);
            ch->pl = ch->exp;
            transStatRemove(ch);
            if( !IS_NPC( ch ) )
            return;
          }

        if ( ch->mana < skill_table[gsn_goronmask]->min_mana )
        {
            send_to_char( "You don't have enough energy.\n\r", ch );
            return;
        }

    WAIT_STATE( ch, skill_table[gsn_goronmask]->beats );
   if ( can_use_skill(ch, number_percent(), gsn_goronmask ) )
        {
        act( AT_BLUE, "&OAs You wear the Goron Mask, You scream in horror as your body takes on the form of a huge Goron Warrior. ", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "&OAs $n wears $s Goron Mask, $e screams in horror as $s body takes on the form of a huge Goron Warrior.", ch, NULL, NULL, TO_NOTVICT );
        xSET_BIT( (ch)->affected_by, AFF_GORON );
		            xREMOVE_BIT((ch)->affected_by, AFF_DEKU);
            xREMOVE_BIT((ch)->affected_by, AFF_ZORA);
            xREMOVE_BIT((ch)->affected_by, AFF_FDEITY);
            xREMOVE_BIT((ch)->affected_by, AFF_ADULT);
	  ch->pl = ch->exp * 22;
        transStatApply(ch, 25, 20, 5, 18);
        learn_from_success( ch, gsn_goronmask );
    }
     else 
     {
     switch (number_range(1, 3))
     {
             default: 
             act( AT_RED, "&O.You start to put the Goron Mask on, but you accidently drop it.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&O$n starts to put $s Goron Mask on, but $e accidently drops it.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 1:
             act( AT_RED, "&OYou start to put the Goron Mask on, but you accidently drop it.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&O$n starts to put $s Goron Mask on, but $e accidently drops it.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 2:
             act( AT_RED, "&OThat mask doesn't really have any powers.", ch, NULL, NULL, TO_CHAR );
             break;
             case 3:
             act( AT_RED, "&OYou start to put the Goron Mask on, but you accidently drop it.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&O$n starts to put $s Goron Mask on, but $e accidently drops it.", ch, NULL, NULL, TO_NOTVICT );
             break;
     }
     learn_from_failure( ch, gsn_goronmask );
    }
     ch->mana -= skill_table[gsn_goronmask]->min_mana;
     return;
}

void do_zoramask(CHAR_DATA *ch, char *argument )
{
    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
	  return;
        if( !can_use_skill( ch->master, number_percent(), gsn_zoramask ))
	  return;
    }

    if( !IS_NPC(ch) )
    {
      if( IS_SET(ch->pcdata->flags, PCFLAG_KNOWSMYSTIC) )
      {
	ch_printf(ch,"You are unable to call upon those powers while you know mystic.\n\r");
	return;
      }
    }

    if( wearing_chip(ch) )
    {
        ch_printf(ch," You can't while you have a chip installed.\n\r");
        return;
    }

    if (xIS_SET((ch)->affected_by, AFF_ADULT))
    {
		send_to_char( "You can't wear this mask unless you are a kid.\n\r", ch );
		return;
    }

        if (xIS_SET((ch)->affected_by, AFF_ZORA))
	    {
		send_to_char( "You remove your mask.\n\r", ch );
            xREMOVE_BIT((ch)->affected_by, AFF_ZORA);
            ch->pl = ch->exp;
            transStatRemove(ch);
            if( !IS_NPC( ch ) )
            return;
          }

        if ( ch->mana < skill_table[gsn_zoramask]->min_mana )
        {
            send_to_char( "You don't have enough energy.\n\r", ch );
            return;
        }

    WAIT_STATE( ch, skill_table[gsn_zoramask]->beats );
   if ( can_use_skill(ch, number_percent(), gsn_zoramask ) )
        {
        act( AT_BLUE, "&CAs You wear the Zora Mask, You scream in horror as your body takes on the form of a tall Zoran Guitarist.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "&CAs $n wears $s Zora Mask, $e screams in horror as $s body takes on the form of a tall Zoran Guitarist.", ch, NULL, NULL, TO_NOTVICT );
	  xSET_BIT( (ch)->affected_by, AFF_ZORA ); 
            xREMOVE_BIT((ch)->affected_by, AFF_DEKU);
            xREMOVE_BIT((ch)->affected_by, AFF_FDEITY);
            xREMOVE_BIT((ch)->affected_by, AFF_GORON);
            xREMOVE_BIT((ch)->affected_by, AFF_ADULT);	  
        ch->pl = ch->exp * 16;
        transStatApply(ch, 15, 20, 24, 16);
        learn_from_success( ch, gsn_zoramask );
	if( !IS_NPC( ch ) )
	{
	  ch->pcdata->eyes      = 0;
	}
    }
     else 
     {
     switch (number_range(1, 3))
     {
             default: 
             act( AT_RED, "&CYou start to put the Zora Mask on, but you accidently drop it.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&C$n starts to put $s Zora Mask on, but $e accidently drops it.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 1:
             act( AT_RED, "&CYou start to put the Zora Mask on, but you accidently drop it.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&C$n starts to put $s Zora Mask on, but $e accidently drops it. ", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 2:
             act( AT_RED, "&CThat mask doesn't really have any powers.", ch, NULL, NULL, TO_CHAR );
             break;
             case 3:
             act( AT_RED, "&CYou start to put the Zora Mask on, but you accidently drop it.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&C$n starts to put $s Zora Mask on, but $e accidently drops it.", ch, NULL, NULL, TO_NOTVICT );
             break;
     }
     learn_from_failure( ch, gsn_zoramask );
    }
     ch->mana -= skill_table[gsn_zoramask]->min_mana;
     return;
}

void do_fiercedeity(CHAR_DATA *ch, char *argument )
{
    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
	  return;
        if( !can_use_skill( ch->master, number_percent(), gsn_fiercedeity ))
	  return;
    }

    if( !IS_NPC(ch) )
    {
      if( IS_SET(ch->pcdata->flags, PCFLAG_KNOWSMYSTIC) )
      {
	ch_printf(ch,"You are unable to call upon those powers while you know mystic.\n\r");
	return;
      }
    }

    if( wearing_chip(ch) )
    {
        ch_printf(ch," You can't while you have a chip installed.\n\r");
        return;
    }

    if (xIS_SET((ch)->affected_by, AFF_ADULT))
    {
		send_to_char( "You can't wear this mask unless you are a kid.\n\r", ch );
		return;
    }

        if (xIS_SET((ch)->affected_by, AFF_FDEITY))
	    {
		send_to_char( "You remove your mask.\n\r", ch );
            xREMOVE_BIT((ch)->affected_by, AFF_FDEITY);
            ch->pl = ch->exp;
            transStatRemove(ch);
            if( !IS_NPC( ch ) )
            return;
          }

        if ( ch->mana < skill_table[gsn_fiercedeity]->min_mana )
        {
            send_to_char( "You don't have enough energy.\n\r", ch );
            return;
        }

    WAIT_STATE( ch, skill_table[gsn_fiercedeity]->beats );
   if ( can_use_skill(ch, number_percent(), gsn_fiercedeity ) )
        {
        act( AT_BLUE, "&RAs You wear the Fierce Deity Mask, You scream in horror as your body takes on the form of a Former God.", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "&RAs $n wears $s Fierce Deity Mask, $e screams in horror as $s body takes on the form of a Former God.", ch, NULL, NULL, TO_NOTVICT );
	  xSET_BIT( (ch)->affected_by, AFF_FDEITY );
            xREMOVE_BIT((ch)->affected_by, AFF_DEKU);
            xREMOVE_BIT((ch)->affected_by, AFF_ZORA);
            xREMOVE_BIT((ch)->affected_by, AFF_GORON);
            xREMOVE_BIT((ch)->affected_by, AFF_ADULT);			
        ch->pl = ch->exp * 30;
        transStatApply(ch, 35, 35, 35, 35);
        learn_from_success( ch, gsn_fiercedeity );
	if( !IS_NPC( ch ) )
	{
	  ch->pcdata->eyes      = 0;
	}
    }
     else 
     {
     switch (number_range(1, 3))
     {
             default: 
             act( AT_RED, "&RYou start to put the Fierce Deity Mask on, but you accidently drop it.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&R$n starts to put $s Fierce Deity Mask on, but $e accidently drops it.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 1:
             act( AT_RED, "&RYou start to put the Fierce Deity Mask on, but you accidently drop it.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&R$n starts to put $s Fierce Deity Mask on, but $e accidently drops it. ", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 2:
             act( AT_RED, "&RThat mask doesn't really have any powers.", ch, NULL, NULL, TO_CHAR );
             break;
             case 3:
             act( AT_RED, "&RYou start to put the Fierce Deity Mask on, but you accidently drop it.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&R$n starts to put $s Fierce Deity Mask on, but $e accidently drops it.", ch, NULL, NULL, TO_NOTVICT );
             break;
     }
     learn_from_failure( ch, gsn_fiercedeity );
    }
     ch->mana -= skill_table[gsn_fiercedeity]->min_mana;
     return;
}

/*eof*/
