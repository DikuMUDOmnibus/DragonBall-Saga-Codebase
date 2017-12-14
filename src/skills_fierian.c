/***************************************************************************************************/
/*                                     FIERIAN SKILLS v1.0                                         */
/*                                          Coded By                                               */
/*                                           Arbin                                                 */
/* Attacks:  														   */
/*        'Fireball'      'Fireforge'      'Firestorm'      'Magma Touch'      'Novawave'		   */
/*	    'Eruption'													   */
/*																   */
/* Transformations:                         									   */
/*                                                                                                 */
/*        'Puppet'      'Superflame1'      'Superflame2'      'Superflame3'      'Ultraflame'      */
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

void do_fireball(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_energy_ball ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_fireball]->skill_level[ch->class] )
    {
	send_to_char("You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_fireball]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_fireball]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_fireball]->focus;

    WAIT_STATE( ch, skill_table[gsn_fireball]->beats );

    sh_int z = get_aura(ch);

    if ( can_use_skill(ch, number_percent(),gsn_fireball ) )
    {
	dam = get_attmod(ch, victim) * number_range( 6, 8 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( z, "You burn $N with your fireball. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( z, "$n burns you with $s fireball. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( z, "$n burns $N with $n fireball. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );
        dam = ki_absorb( victim, ch, dam, gsn_fireball );
	learn_from_success( ch, gsn_fireball );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( z, "You missed $N with your fireball.", ch, NULL, victim, TO_CHAR );
	act( z, "$n misses you with $s fireball.", ch, NULL, victim, TO_VICT );
	act( z, "$n missed $N with $s fireball.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_fireball );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }

    if( !is_android_h(ch) )
      ch->mana -= skill_table[gsn_fireball]->min_mana;
    return;
}

void do_fireforge(CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *o;
    OBJ_INDEX_DATA *pObjIndex;

    if( IS_NPC(ch) )
          return;

    if( argument[0] == '\0' )
    {
	ch_printf(ch,"\n\rSyntax: fireforge <item>\n\r");
	ch_printf(ch,"\n\r");
	ch_printf(ch,"Item can only be one of the following:\n\r");
	ch_printf(ch,"\n\r");
	ch_printf(ch,"armor leggings sword\n\r");
	return;
    }

    if( !(!str_cmp(argument,"armor")) && !(!str_cmp(argument,"leggings")) &&
	!(!str_cmp(argument,"sword")) )
    {
	do_fireforge(ch,"");
	return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
        send_to_char( "You can't concentrate enough for that.\n\r", ch );
        return;
    }
    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_fireforge]->skill_level[ch->class] )
    {
        send_to_char( "You can't do that.\n\r", ch );
        return;
    }
    if ( ch->mana < skill_table[gsn_fireforge]->min_mana )
    {
        send_to_char( "You don't have enough energy.\n\r", ch );
        return;
    }
    if( ch->carry_number >= 19 )
    {
	ch_printf(ch,"You haven't got any room.\n\r");
	return;
    }


    WAIT_STATE( ch, skill_table[gsn_fireforge]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_fireforge ) )
    {
	int a = 0;
	if( !str_cmp(argument,"armor") )
	  a = 1341;
	else if( !str_cmp(argument,"leggings") )
	  a = 1342;
	else if( !str_cmp(argument,"sword") )
	  a = 1340;
	if( a == 0 )
	{
	  bug("Serious problem in function fire forge",0);
	  return;
	}
	act( AT_SKILL, "You harness the fire in your body and forge an item.", ch, NULL, NULL, TO_CHAR );
	act( AT_SKILL, "$n harnesses the mystical fires in his body and forges an item.", ch, NULL, NULL, TO_NOTVICT );
        learn_from_success( ch, gsn_fireforge );
	pObjIndex = get_obj_index( a );
	o = create_object_new( pObjIndex, 1 , ORIGIN_OINVOKE, ch->name );
	o = obj_to_char( o, ch );
	save_char_obj( ch );
    }
    else
    {
         act( AT_SKILL, "You fail to forge an item with your fire.", ch, NULL, NULL, TO_CHAR );
         act( AT_SKILL, "$n fails to use $s fire to forge an item.", ch, NULL, NULL, TO_NOTVICT );
        learn_from_failure( ch, gsn_fireforge );
    }
    ch->mana -= skill_table[gsn_fireforge]->min_mana;
    return;
}

void do_firestorm(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_firestorm))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && ch->exp < skill_table[gsn_firestorm]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
	}

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_firestorm]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_firestorm]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_firestorm]->focus;

    WAIT_STATE( ch, skill_table[gsn_firestorm]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_firestorm ) )
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
			act( AT_RED, "&zFrom the palms of your hands archs a fiery wave of fire, You can only control it enough to barely keep it off the ground. Suddenly it begins to rage out of control. It burns the heavens causing a &RFire Storm&z. Fire at well over one thousand degrees engulfs the area.", ch, NULL, victim, TO_CHAR );
			act( AT_RED, "&zAs it rains down, $N produces a large fire resistant shield, saving $S ass.", ch, NULL, victim, TO_CHAR );
			act( AT_RED, "&z$n creates a wave of fire from $s palms, but $e is unable to control it, barely keeping it on the ground. Suddenly it rages out of control and flares into the heavens, causing a &RFire Storm&z.", ch, NULL, victim, TO_VICT );
			act( AT_RED, "&zAs it rains down, you produce a large fire resistant shield, saving your ass.", ch, NULL, victim, TO_VICT );
			act( AT_RED, "&z$n creates a wave of fire, which $e loses control of, barely keeping it on the ground. Suddenly it flares into the heavens causing a &RFire Storm&z..", ch, NULL, victim, TO_NOTVICT );
			act( AT_RED, "&zAs it rains down, $N produces a large fire resistant shield, saving $S ass.", ch, NULL, victim, TO_NOTVICT );
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
	act( AT_RED, "&zFrom the palms of your hands archs a fiery wave f fire, You can only control it enough to barely keep it off the ground. Suddenly it begins to rage out of control. It burns the heavens causing a &RFire Storm&z. Fire at well over one thousand degrees engulfs the area.", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "&zAs it rains down a big fiery rain drop lands on $N, creamating $M instantly!!", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "&z$n creates a wave of fire from $s palms, but $e is unable to control it, barely keeping it on the ground. Suddenly it rages out of control and flares into the heavens, causing a &RFire Storm&z.", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "&zFire begins to rain down, you look up in time to see a big fiery rain drop land on you. It creamates you!!!", ch, NULL, victim, TO_VICT );
	act( AT_RED, "&z$n creates a wave of fire, which $e loses control of, barely keeping it on the ground. Suddenly it flares into the heavens causing a &RFire Storm&z.", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "&zFire begins to rain down, you watch in horror as a rain drop lands on $N, creamating $M, leaving nothing but a skeleton!!!  Killing $M instantly.", ch, NULL, victim, TO_NOTVICT );

	learn_from_success( ch, gsn_firestorm );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
    }
	default:
	dam = get_attmod(ch, victim) * number_range( 5, 15 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_RED, "&zYou create a &RFire Storm&z, causing rain that burns $N's flesh. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_RED, "&z$n creates a &RFire Storm&z, causing rain that burns your flesh. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_RED, "&z$n creates a &RFire Storm&z, causing rain that burns $N's flesh. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

	learn_from_success( ch, gsn_firestorm );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
    }
    }
    else
    {
	act( AT_RED, "You missed $N with your Fire Storm.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n misses you with $s Fire Storm.", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$n missed $N with $s Fire Storm.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_firestorm );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_firestorm]->min_mana;
    return;
}

void do_magmatouch(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_kamehameha ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_magmatouch]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_magmatouch]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_magmatouch]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_magmatouch]->focus;

    WAIT_STATE( ch, skill_table[gsn_magmatouch]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_magmatouch ) )
    {
	dam = get_attmod(ch, victim) * number_range( 45, 55 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_LBLUE, "&zMagma envelopes both of your arms, causing them to become hotter than the earths core itself	", ch, NULL, victim, TO_CHAR );
	act( AT_LBLUE, "&zYou grab for $N, but miss, causing some very bad burns. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_LBLUE, "&z$n arms become eveloped in magma, $e reaches for you.	", ch, NULL, victim, TO_VICT );
	act( AT_LBLUE, "&z$e misses, but does barely touch you, HOLY SHIT, THAT IS FUCKIN HOT!!! &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_LBLUE, "&z$n arms become eveloped in magma, $e reaches for $N. ", ch, NULL, victim, TO_NOTVICT );
	act( AT_LBLUE, "&z$e misses, but does barely touch $E. [$t]", ch, num_punct(dam), victim, TO_NOTVICT );

        dam = ki_absorb( victim, ch, dam, gsn_magmatouch );
	learn_from_success( ch, gsn_magmatouch );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_LBLUE, "You missed $N with your magma touch.", ch, NULL, victim, TO_CHAR );
	act( AT_LBLUE, "$n misses you with $s magma touch.", ch, NULL, victim, TO_VICT );
	act( AT_LBLUE, "$n missed $N with $s magma touch.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_magmatouch );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_magmatouch]->min_mana;
    return;
}

void do_novawave(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_novawave))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_novawave]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_novawave]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_novawave]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_novawave]->focus;

    WAIT_STATE( ch, skill_table[gsn_novawave]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_novawave ) )
    {
//	dam = get_attmod(ch, victim) * number_range( 40, 44 );
	dam = get_attmod(ch, victim) * number_range( 70, 80 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
/*	act( AT_SKILL, "&zYou clap your hands together, at unimaginable speed, creating a wave of pure fire .", ch, NULL, victim, TO_CHAR );
 *	act( AT_SKILL, "&zIt expands across the area at the speed of light, the waves arching towards $N . &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
 *	act( AT_SKILL, "&z$n claps $s hands together, at unimaginable speed, creating a wave of pure fire.", ch, NULL, victim, TO_VICT );
 *	act( AT_SKILL, "&It expands across the area at the speed of light, the waves arching straight for you.&W[$t]", ch, num_punct(dam), victim, TO_VICT );
 *	act( AT_SKILL, "&z$n clasps his hands together, at unimaginable speed, creating a wave of pure fire.", ch, NULL, victim, TO_NOTVICT );
 *	act( AT_SKILL, "&zIt archs across the area at the speed of light, the waves arching straight for $N. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT ); */

	act( AT_BLUE, "&zYou start clapping your hands together rapidly, causing a massive outburst of pure power.", ch, NULL, victim, TO_CHAR );
	act( AT_BLUE, "&zThe fires around your body start following the pattern and expand outward, "
                       "&zarching from your hands toward $N. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_BLUE, "&z$n starts clapping $s hands together rapidly, causing a massive outburst of pure power.", ch, NULL, victim, TO_VICT );
	act( AT_BLUE, "&zThe fires around $s body start following the pattern and expand outward, "
                       "&zarching from $s hands toward you. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_BLUE, "&z$n starts clapping $s hands together rapidly, causing a massive outburst of pure power.", ch, NULL, victim, TO_NOTVICT );
	act( AT_BLUE, "&zThe fires around $n's body start following the pattern and expand outward, "
                       "&zarching from $n's hand toward $N. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

        dam = ki_absorb( victim, ch, dam, gsn_novawave );
	learn_from_success( ch, gsn_novawave );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_BLUE, "You missed $N with your nova wave.", ch, NULL, victim, TO_CHAR );
	act( AT_BLUE, "$n misses you with $s nova wave.", ch, NULL, victim, TO_VICT );
	act( AT_BLUE, "$n missed $N with a nova wave.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_novawave );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_novawave]->min_mana;
    return;
}


void do_eruption(CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
	if( !ch->master )
	  return;
	if( !can_use_skill( ch->master, number_percent(), gsn_eruption ))
	  return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_eruption]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_eruption]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_eruption]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_eruption]->focus;

    WAIT_STATE( ch, skill_table[gsn_eruption]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_eruption ) )
    {
	dam = get_attmod(ch, victim) * number_range( 60, 70 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);

	act( AT_BLUE, "&zA massive force begins to build up inside your body, you scream in rage from the immense pain.", ch, NULL, victim, TO_CHAR );
	act( AT_BLUE, "&zYou unwillingly aim your mouth towards the sky, and lava begins Erupting from it, flying high into the air, spewing down on $N burning $M badly. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_BLUE, "&z$n begins screaming in rage and points $s head towards the sky, erupting lava all over the place.", ch, NULL, victim, TO_VICT );
	act( AT_BLUE, "&zSpewing all over you, you howl in pain, it is burning you very badly. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_BLUE, "&z$n begins screaming in rage and points $s head towards the sky, erupting lava all over the place.", ch, NULL, victim, TO_NOTVICT );
	act( AT_BLUE, "&zSpewing down on $N, $E howls in pain, the lava is burning $M very badly. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

        dam = ki_absorb( victim, ch, dam, gsn_eruption );
	learn_from_success( ch, gsn_eruption );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_BLUE, "You missed $N with your eruption.", ch, NULL, victim, TO_CHAR );
	act( AT_BLUE, "$n misses you with $s eruption.", ch, NULL, victim, TO_VICT );
	act( AT_BLUE, "$n missed $N with $s eruption.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_eruption );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_eruption]->min_mana;
	return;
}

void do_htorch(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_htorch ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_htorch]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_htorch]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_htorch]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_htorch]->focus;

    WAIT_STATE( ch, skill_table[gsn_htorch]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_htorch ) )
    {
	dam = get_attmod(ch, victim) * number_range( 100, 130 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_LBLUE, "&zYou throw both of your arms into the air, and begin to pray.", ch, NULL, victim, TO_CHAR );
	act( AT_LBLUE, "&cYou utter the words, 'Please, O' Holy one, deliver onto this unfortunate soul the power of your divine wrath.'", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_LBLUE, "&zAs you say these words, a huge cylinder of fire engulfs $N, leaving $M nothing but a bloody mess. &W[&Y$t&W]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_LBLUE, "&z$n throws both of $s arms into the air, and begins to pray.", ch, NULL, victim, TO_VICT );
	act( AT_LBLUE, "&c$n utters the words, 'Please, O' Holy one, deliver onto this unfortunate soul the power of your divine wrath.'", ch, num_punct(dam), victim, TO_VICT );
	act( AT_LBLUE, "&zAs $n says these words, a huge cylinder of fire engulfs you, leaving you nothing but a bloody mess. &W[&Y$t&W]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_LBLUE, "&z$n throws both of $s arms into the air, and begins to pray.", ch, NULL, victim, TO_NOTVICT );
	act( AT_LBLUE, "&c$n utters the words, 'Please, O' Holy one, deliver onto this unfortunate soul the power of your divine wrath.'", ch, NULL, victim, TO_NOTVICT );
	act( AT_LBLUE, "&zAs $n says these words, a huge cylinder of fire engulfs $N, leaving $M nothing but a bloody mess. &W[&Y$t&W]", ch, num_punct(dam), victim, TO_NOTVICT );

        dam = ki_absorb( victim, ch, dam, gsn_htorch );
	learn_from_success( ch, gsn_htorch );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_LBLUE, "&zYou missed $N with your Heaven's Torch.", ch, NULL, victim, TO_CHAR );
	act( AT_LBLUE, "&z$n misses you with $s Heaven's Torch.", ch, NULL, victim, TO_VICT );
	act( AT_LBLUE, "&z$n missed $N with $s Heaven's Torch.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_htorch );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_htorch]->min_mana;
    return;
}

/*****************************/
/*     Transformations       */
/*****************************/

void do_puppet(CHAR_DATA *ch, char *argument )
{
    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
	  return;
        if( !can_use_skill( ch->master, number_percent(), gsn_superflame1 ))
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
        if (xIS_SET((ch)->affected_by, AFF_PUPPET)
          || xIS_SET((ch)->affected_by, AFF_SFLAME1)
	    || xIS_SET((ch)->affected_by, AFF_SFLAME2)
          || xIS_SET((ch)->affected_by, AFF_SFLAME3)
          || xIS_SET((ch)->affected_by, AFF_UFLAME))
            {
                send_to_char( "You power down and return to normal.\n\r", ch );
		if (xIS_SET((ch)->affected_by, AFF_PUPPET))
                      xREMOVE_BIT((ch)->affected_by, AFF_PUPPET);
            if (xIS_SET((ch)->affected_by, AFF_SFLAME1))
                      xREMOVE_BIT((ch)->affected_by, AFF_SFLAME1);
		if (xIS_SET((ch)->affected_by, AFF_SFLAME2))
		      xREMOVE_BIT((ch)->affected_by, AFF_SFLAME2);
		if (xIS_SET((ch)->affected_by, AFF_SFLAME3))
		      xREMOVE_BIT((ch)->affected_by, AFF_SFLAME3);
                if (xIS_SET((ch)->affected_by, AFF_UFLAME))
                  xREMOVE_BIT((ch)->affected_by, AFF_UFLAME);
                ch->pl = ch->exp;
                transStatRemove(ch);
                return;
            }
        if ( ch->mana < skill_table[gsn_puppet]->min_mana )
        {
            send_to_char( "You don't have enough energy.\n\r", ch );
            return;
        }

    WAIT_STATE( ch, skill_table[gsn_puppet]->beats );
   if ( can_use_skill(ch, number_percent(), gsn_puppet ) )
        {
        act( AT_RED, "&zAn Image of the puppet master appears and yells, &Y'PUPPETIZER BLAST!'. &zHis attack hits you, changing your body into a wooden puppet of its former self.", ch, NULL, NULL, TO_CHAR );
        act( AT_RED, "&zAn Image of the puppet master appears and yells, &Y'PUPPETIZER BLAST!'. &zHis attack changes $n into wooden puppet of $s former self.", ch, NULL, NULL, TO_NOTVICT );
        xSET_BIT( (ch)->affected_by, AFF_PUPPET );
        if( xIS_SET((ch)->affected_by, AFF_SFLAME1) )
        {
             xREMOVE_BIT((ch)->affected_by, AFF_SFLAME1);
        }
         if( xIS_SET((ch)->affected_by, AFF_SFLAME2) )
        {
             xREMOVE_BIT((ch)->affected_by, AFF_SFLAME2);
        }
        if( xIS_SET((ch)->affected_by, AFF_SFLAME3) )
        {
             xREMOVE_BIT((ch)->affected_by, AFF_SFLAME3);
        }
        if( xIS_SET((ch)->affected_by, AFF_UFLAME) )
        {
             xREMOVE_BIT((ch)->affected_by, AFF_UFLAME);
        }

        ch->pl = ch->exp * 8;
        transStatApply(ch, -5, -5, -5, -5);
        learn_from_success( ch, gsn_puppet );
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
             act( AT_RED, "&zYour body begins to shrink, turning wooden, but you turn full size again, normal.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&z$n's body is shrinking, changing into wood, but turns normal again.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 1:
             act( AT_RED, "&zYour body shrinks and becomes wooden, but you quickly revert back.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&z$n's body shrinks and becomes wooden, but $e quickly reverts back.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 2:
             act( AT_RED, "&zYou Start Shrinking, OMG! &RJ/K", ch, NULL, NULL, TO_CHAR );
             break;
             case 3:
             act( AT_RED, "&zYou freak out seeing small wooden arms and legs and revert back to normal.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&z$n starts freaking out about $s body being wooden and changes back to normal.", ch, NULL, NULL, TO_NOTVICT );
             break;
     }
     learn_from_failure( ch, gsn_puppet );
    }
     ch->mana -= skill_table[gsn_puppet]->min_mana;
     return;
}

void do_sflame1(CHAR_DATA *ch, char *argument )
{
    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
	  return;
        if( !can_use_skill( ch->master, number_percent(), gsn_superflame1 ))
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
         if (xIS_SET((ch)->affected_by, AFF_SFLAME1)
         || xIS_SET((ch)->affected_by, AFF_SFLAME2)
         || xIS_SET((ch)->affected_by, AFF_SFLAME3)
         || xIS_SET((ch)->affected_by, AFF_UFLAME))
	    {
		send_to_char( "You power down to Puppet State.\n\r", ch );
            xSET_BIT( (ch)->affected_by, AFF_PUPPET );
            xREMOVE_BIT((ch)->affected_by, AFF_SFLAME1);
            xREMOVE_BIT( (ch)->affected_by, AFF_SFLAME2 );
            xREMOVE_BIT( (ch)->affected_by, AFF_SFLAME3 );
            xREMOVE_BIT( (ch)->affected_by, AFF_UFLAME );
		ch->pl = ch->exp * 8;
		transStatApply(ch, -5, -5, -5, -5);
            if( !IS_NPC( ch ) )
            return;
        }

        if ( ch->mana < skill_table[gsn_superflame1]->min_mana )
        {
            send_to_char( "You don't have enough energy.\n\r", ch );
            return;
        }

    WAIT_STATE( ch, skill_table[gsn_superflame1]->beats );
   if ( can_use_skill(ch, number_percent(), gsn_superflame1 ) )
        {
        act( AT_RED, "A blazing fire erupts around your body, your muscles bulge to an enourmous size, everything around you being burnt to a crisp.", ch, NULL, NULL, TO_CHAR );
        act( AT_RED, "$n's body is engulfed in a raging fire, $s muscles bulge to an enourmous size, everything around $m being burnt to a crisp.", ch, NULL, NULL, TO_NOTVICT );
        xSET_BIT( (ch)->affected_by, AFF_SFLAME1 );
        xSET_BIT( (ch)->affected_by, AFF_PUPPET );
        if( xIS_SET((ch)->affected_by, AFF_SFLAME2) )
        {
          xREMOVE_BIT( (ch)->affected_by, AFF_SFLAME2 );
        }
        if( xIS_SET((ch)->affected_by, AFF_SFLAME3) )
        {
          xREMOVE_BIT( (ch)->affected_by, AFF_SFLAME3 );
        }
        if( xIS_SET((ch)->affected_by, AFF_UFLAME) )
        {
          xREMOVE_BIT( (ch)->affected_by, AFF_UFLAME );
        }
	 ch->pl = ch->exp * 12;
	 transStatApply(ch, 4, 7, 4, 5);
        learn_from_success( ch, gsn_superflame1 );
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
             act( AT_RED, "&zYour body starts to become extremely hot, but quickly cools back down.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&z$n starts radiating some extreme heat, but it dissappears quickly.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 1:
             act( AT_RED, "&zYour body starts to become extremely hot, but quickly cools back down.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&z$n starts radiating some extreme heat, but it dissappears quickly.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 2:
             act( AT_RED, "&zQuit Fuckin around buddy, your &RNOT &za Super Flame!", ch, NULL, NULL, TO_CHAR );
             break;
             case 3:
             act( AT_RED, "&zA fire starts to burn around you, but quickly goes out.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&zA fire starts to burn around $n, but quickly goes out.", ch, NULL, NULL, TO_NOTVICT );
             break;
     }
     learn_from_failure( ch, gsn_superflame1 );
    }
     ch->mana -= skill_table[gsn_superflame1]->min_mana;
     return;
}

void do_sflame2(CHAR_DATA *ch, char *argument )
{
    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
	  return;
        if( !can_use_skill( ch->master, number_percent(), gsn_superflame2 ))
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
        if (xIS_SET((ch)->affected_by, AFF_SFLAME2)
         || xIS_SET((ch)->affected_by, AFF_SFLAME3)
         || xIS_SET((ch)->affected_by, AFF_UFLAME))
	    {
		send_to_char( "You power down to Super Flame.\n\r", ch );
	      xSET_BIT( (ch)->affected_by, AFF_SFLAME1 );            
            xSET_BIT( (ch)->affected_by, AFF_PUPPET );
            xREMOVE_BIT((ch)->affected_by, AFF_SFLAME2);
            xREMOVE_BIT((ch)->affected_by, AFF_SFLAME3);
            xREMOVE_BIT((ch)->affected_by, AFF_UFLAME);
		ch->pl = ch->exp * 12;
		transStatApply(ch, 4, 7, 4, 5);
            if( !IS_NPC( ch ) )
            return;
          }

        if ( ch->mana < skill_table[gsn_superflame2]->min_mana )
        {
            send_to_char( "You don't have enough energy.\n\r", ch );
            return;
        }

    WAIT_STATE( ch, skill_table[gsn_superflame2]->beats );
   if ( can_use_skill(ch, number_percent(), gsn_superflame2 ) )
        {
        act( AT_BLUE, "&zYou scream in rage as the fires around you begin to burn even more viciously. Just when you think you can't take any more, the fire starts to burn a &Bblueish &zcolor. ", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "&z$n screams in rage as the fires around $m begin to burn even more viciously. Suddenly the fire starts to burn a &Bblueish &zcolor.", ch, NULL, NULL, TO_NOTVICT );
        xSET_BIT( (ch)->affected_by, AFF_PUPPET );
	  xSET_BIT( (ch)->affected_by, AFF_SFLAME2 );
        if( xIS_SET((ch)->affected_by, AFF_SFLAME1) )
        {
             xREMOVE_BIT( (ch)->affected_by, AFF_SFLAME1 );
        }    
        if( xIS_SET((ch)->affected_by, AFF_SFLAME3) )
        {
             xREMOVE_BIT((ch)->affected_by, AFF_SFLAME3);
        }
        if( xIS_SET((ch)->affected_by, AFF_UFLAME) )
        {
          xREMOVE_BIT( (ch)->affected_by, AFF_UFLAME );
        }
	 ch->pl = ch->exp * 20;
	 transStatApply(ch, 8, 13, 8, 13);
        learn_from_success( ch, gsn_superflame2 );
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
             act( AT_RED, "&zYour body starts to become extremely hot, but quickly cools back down.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&z$n starts radiating some extreme heat, but it dissappears quickly.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 1:
             act( AT_RED, "&zYour body starts to become extremely hot, but quickly cools back down.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&z$n starts radiating some extreme heat, but it dissappears quickly.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 2:
             act( AT_RED, "&Whoopy Fuckin do, you got superflame, your not gonna get the second one!", ch, NULL, NULL, TO_CHAR );
             break;
             case 3:
             act( AT_RED, "&zA fire starts to burn around you, but quickly goes out.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&zA fire starts to burn around $n, but quickly goes out.", ch, NULL, NULL, TO_NOTVICT );
             break;
     }
     learn_from_failure( ch, gsn_superflame2 );
    }
     ch->mana -= skill_table[gsn_superflame2]->min_mana;
     return;
}
void do_sflame3(CHAR_DATA *ch, char *argument )
{
    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
	  return;
        if( !can_use_skill( ch->master, number_percent(), gsn_superflame3 ))
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
        if (xIS_SET((ch)->affected_by, AFF_SFLAME3)
           || xIS_SET((ch)->affected_by, AFF_UFLAME))
	    {
		send_to_char( "You power down to Super Flame 2. You feel your body reverting back to its puppet state.\n\r", ch );
	      xSET_BIT( (ch)->affected_by, AFF_SFLAME2 );
            xSET_BIT( (ch)->affected_by, AFF_PUPPET );
            xREMOVE_BIT((ch)->affected_by, AFF_SFLAME3);
            xREMOVE_BIT((ch)->affected_by, AFF_UFLAME);
		ch->pl = ch->exp * 20;
		transStatApply(ch, 8, 13, 8, 13);
            if( !IS_NPC( ch ) )
            return;
          }

        if ( ch->mana < skill_table[gsn_superflame3]->min_mana )
        {
            send_to_char( "You don't have enough energy.\n\r", ch );
            return;
        }

    WAIT_STATE( ch, skill_table[gsn_superflame3]->beats );
   if ( can_use_skill(ch, number_percent(), gsn_superflame3 ) )
        {
        act( AT_BLUE, "&zYou feel your body beginning to double in size, releasing you from the horror that is called puppet. Your fiery aura turns &YYellow&z, and begins to flare up around you. ", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "&zYou watch in awe as $n's body doubles in size, becoming pure fire, and loosing its wooden puppet like qualities. $s fiery aura turns &YYellow&z and flares up around him", ch, NULL, NULL, TO_NOTVICT );
	  xSET_BIT( (ch)->affected_by, AFF_SFLAME3 );   
        if( xIS_SET((ch)->affected_by, AFF_PUPPET) )
        {
             xREMOVE_BIT( (ch)->affected_by, AFF_PUPPET );
        }
             if( xIS_SET((ch)->affected_by, AFF_SFLAME1) )
        {
             xREMOVE_BIT( (ch)->affected_by, AFF_SFLAME1 );
        }
        if( xIS_SET((ch)->affected_by, AFF_SFLAME2) )
        {
             xREMOVE_BIT( (ch)->affected_by, AFF_SFLAME2 );
        }
        if( xIS_SET((ch)->affected_by, AFF_UFLAME) )
        {
          xREMOVE_BIT( (ch)->affected_by, AFF_UFLAME );
        }
	 ch->pl = ch->exp * 24;
	 transStatApply(ch, 28, 30, 26, 28);
        learn_from_success( ch, gsn_superflame3 );
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
             act( AT_RED, "&zYour hair is flaring widly, but it quickly calms back down.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&z$n's hair is flaring widly, but it quickly calms back down.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 1:
             act( AT_RED, "&zYour hair begins to flare up widly, the fire in it acting as if it has a mind of its own.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&zIt quickly loses its flare and reverts to normal. ", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&z$n's hair begins to flare up widly, the fire in it acting as if it has a mind of its own.", ch, NULL, NULL, TO_NOTVICT );
             act( AT_RED, "&zIt quickly loses its flare and reverts to normal. ", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 2:
             act( AT_RED, "&zWhat the fuck man! Your still screwin around... there is no way for you to get super flame 3", ch, NULL, NULL, TO_CHAR );
             break;
             case 3:
             act( AT_RED, "&zThe fires around you begin to flicker from blue to yellow, but the flickering stops.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&zThe fires around $n begin to flicker from blue to yellow, but the flickering stops.", ch, NULL, NULL, TO_NOTVICT );
             break;
     }
     learn_from_failure( ch, gsn_superflame3 );
    }
     ch->mana -= skill_table[gsn_superflame3]->min_mana;
     return;
}
void do_uflame(CHAR_DATA *ch, char *argument )
{
    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
	  return;
        if( !can_use_skill( ch->master, number_percent(), gsn_superflame3 ))
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
        if (xIS_SET((ch)->affected_by, AFF_UFLAME))
	    {
		send_to_char( "You power down to Super Flame 3, losing some muscle mass.\n\r", ch );
            xSET_BIT( (ch)->affected_by, AFF_SFLAME3 );
            xREMOVE_BIT((ch)->affected_by, AFF_UFLAME);
		ch->pl = ch->exp * 24;
		transStatApply(ch, 28, 30, 26, 28);
            if( !IS_NPC( ch ) )
            return;
          }

        if ( ch->mana < skill_table[gsn_ultraflame]->min_mana )
        {
            send_to_char( "You don't have enough energy.\n\r", ch );
            return;
        }

    WAIT_STATE( ch, skill_table[gsn_superflame3]->beats );
   if ( can_use_skill(ch, number_percent(), gsn_ultraflame ) )
        {
        act( AT_BLUE, "&zYour Aura turns Jet Black, you start laughing insanely, 'MUHAHAHAHAHAHAHA!!!!'. Maybe you belong in a mental institution with the rest of the crazies. ", ch, NULL, NULL, TO_CHAR );
        act( AT_BLUE, "&zYou watch in fright as $n's Aura turns Jet Black. You cower when $e starts laughing like he belongs in a mental institution", ch, NULL, NULL, TO_NOTVICT );
	  xSET_BIT( (ch)->affected_by, AFF_UFLAME );   
        if( xIS_SET((ch)->affected_by, AFF_PUPPET) )
        {
             xREMOVE_BIT( (ch)->affected_by, AFF_PUPPET );
        }
             if( xIS_SET((ch)->affected_by, AFF_SFLAME1) )
        {
             xREMOVE_BIT( (ch)->affected_by, AFF_SFLAME1 );
        }
        if( xIS_SET((ch)->affected_by, AFF_SFLAME2) )
        {
             xREMOVE_BIT( (ch)->affected_by, AFF_SFLAME2 );
        }
        if( xIS_SET((ch)->affected_by, AFF_SFLAME3) )
        {
             xREMOVE_BIT( (ch)->affected_by, AFF_SFLAME3 );
        }
        ch->pl = ch->exp * 35;
        transStatApply(ch, 36, 40, 25, 35);
        learn_from_success( ch, gsn_ultraflame );
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
             act( AT_RED, "&Your Fiery aura starts to turn black, but changes back", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&z$n's fiery aura starts to turn black, but changes back.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 1:
             act( AT_RED, "&zYour aura expands ten feet all around you, solid black. Just Kidding.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&zYou look pretty pissed to find that out. ", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&z$n looks pretty pissed to know we were kiddin with $m about $s aura changing.", ch, NULL, NULL, TO_NOTVICT );
             break;
             case 2:
             act( AT_RED, "&zWhat are you doin? Theres no fucking Ultra Flame. You got all the transformations, Ass....", ch, NULL, NULL, TO_CHAR );
             break;
             case 3:
             act( AT_RED, "&zThe fires around you begin to flicker from yellow to black, but the flickering stops.", ch, NULL, NULL, TO_CHAR );
             act( AT_RED, "&zThe fires around $n begin to flicker from yellow to black, but the flickering stops.", ch, NULL, NULL, TO_NOTVICT );
             break;
     }
     learn_from_failure( ch, gsn_ultraflame );
    }
     ch->mana -= skill_table[gsn_ultraflame]->min_mana;
     return;
}
