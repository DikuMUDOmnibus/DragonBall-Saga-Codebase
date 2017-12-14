#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

extern void transStatApply args( ( CHAR_DATA * ch, int strMod, int spdMod, int intMod, int conMod ) );
extern void transStatRemove args( ( CHAR_DATA * ch ) );

void do_innerwill( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_innerwill ) )
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

  if( xIS_SET( ( ch )->affected_by, AFF_INNERWILL ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_INNERWILL );
    ch->pl = ch->exp;
    transStatRemove( ch );
    return;
  }

  
  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You can't use the Inner Will technique while using kaioken.\n\r", ch );
    return;
  }
  if( ch->mana < skill_table[gsn_innerwill]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

 
  WAIT_STATE( ch, skill_table[gsn_innerwill]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_innerwill ) )
  {
    act( AT_DGREY, "You begin to see visions of your ancestor's pains flash before", ch, NULL, NULL, TO_CHAR ); 
    act( AT_DGREY, "your eyes. Your muscles begin to tighten as you feel your power", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "increasing exponentially from your newfound knowledge. The new", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "knowlege is so overwhelming, you scream from the strain on your", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "psyche.", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "$n closes $s eyes and their muscles begin to bulge. $n lets out", ch, NULL, NULL, TO_NOTVICT );  
    act( AT_DGREY, "a terrible, deafening scream filled with hatred and pain. When", ch, NULL, NULL, TO_NOTVICT );
    act( AT_DGREY, "$n opens $s eyes, they are filled with the intent to justify", ch, NULL, NULL, TO_NOTVICT );
    act( AT_DGREY, "$s ancestor's pain.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_INNERWILL );
    ch->pl = ch->exp * 13;
    transStatApply( ch, 14, 14, 12, 12 );
    learn_from_success( ch, gsn_innerwill );
  }
  else
  {
    act( AT_DGREY, "You fail to transform.", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "$n fails to transform.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_innerwill );
  }

  ch->mana -= skill_table[gsn_innerwill]->min_mana;
  return;
}

void do_mechaevolution( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_mechaevolution ) )
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

  if( xIS_SET( ( ch )->affected_by, AFF_MECHAEVO ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_MECHAEVO );
    ch->pl = ch->exp;
    transStatRemove( ch );
    return;
  }

  
  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You can't use the Mecha Evolution technique while using kaioken.\n\r", ch );
    return;
  }
  if( ch->mana < skill_table[gsn_mechaevolution]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

 
  WAIT_STATE( ch, skill_table[gsn_mechaevolution]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_mechaevolution ) )
  {
    act( AT_DGREY, "You fall to all fours as your body begins to feel immense", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "strain. Your scales thicken and become coated in a strangely", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "metallic substance which oozes up from within your body.", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "Your insides go through a complete modification and now", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "resembles an internal combustion engine, which sudenly", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "bursts to life. Your eyes pulse from a deep, dark green", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "to a bright green with every internal explosion. ", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "$n falls to the ground as $s body is enveloped by", ch, NULL, NULL, TO_NOTVICT );
    act( AT_DGREY, "quicksilver. The metal hardens, leaving $s shiny and machine-like.", ch, NULL, NULL, TO_NOTVICT );
    act( AT_DGREY, "$s eyes pulse from dark green to light green.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_MECHAEVO );
    ch->pl = ch->exp * 20;
    transStatApply( ch, 22, 16, 20, 18 );
    learn_from_success( ch, gsn_mechaevolution );
  }
  else
  {
    act( AT_DGREY, "You fall to all fours and strain your body, however, you fail to transform.", ch, NULL, NULL, TO_CHAR );
    act( AT_DGREY, "$n falls to all fours and strains $s body, however, $e fails to transform.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_mechaevolution );
  }

  ch->mana -= skill_table[gsn_mechaevolution]->min_mana;
  return;

}

void do_ancientevolution( CHAR_DATA * ch, char *argument )
{

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_ancientevolution ) )
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

  if( xIS_SET( ( ch )->affected_by, AFF_ANCIENTEVO ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_ANCIENTEVO );
    ch->pl = ch->exp;
    transStatRemove( ch );
    return;
  }

  
  if( xIS_SET( ( ch )->affected_by, AFF_KAIOKEN ) )
  {
    send_to_char( "You can't use the Ancient Evolution technique while using kaioken.\n\r", ch );
    return;
  }
  if( ch->mana < skill_table[gsn_ancientevolution]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

 
  WAIT_STATE( ch, skill_table[gsn_ancientevolution]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_ancientevolution ) )
  {
    act( AT_RED, "You scream in agony as your body rapidly grows to enormous", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "proportions and your back bulges out obscenely. Two wings", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "burst from your skin with a loud ripping sound, blood spewing", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "onto the ground behind you. Your head elongates and your", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "fangs grow to a much larger size. Your eyes become more", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "narrowed, and your claws dig into the ground as they become", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "larger and infinitely more lethal, completing your evolution.", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "$n screams in agony as $s body grows to enormous proportions", ch, NULL, NULL, TO_NOTVICT );
    act( AT_RED, "and $s back bulges out obscenely. Two wings burst out from $s", ch, NULL, NULL, TO_NOTVICT );
    act( AT_RED, "skin with a loud ripping sound, blood spewing onto the ground", ch, NULL, NULL, TO_NOTVICT );
    act( AT_RED, "behind $s. $s head elongates and $s fangs grow to a much larger", ch, NULL, NULL, TO_NOTVICT );
    act( AT_RED, "size. $s eyes become more narrowed and $s claws dig into the", ch, NULL, NULL, TO_NOTVICT );
    act( AT_RED, "ground as they become larger and infinitely more lethal,", ch, NULL, NULL, TO_NOTVICT );
    act( AT_RED, "completing $n's evolution.", ch, NULL, NULL, TO_NOTVICT );
    
    xSET_BIT( ( ch )->affected_by, AFF_ANCIENTEVO );
    ch->pl = ch->exp * 32;
    transStatApply( ch, 29, 35, 45, 25 );
    learn_from_success( ch, gsn_ancientevolution );
  }
  else
  {
    act( AT_RED, "You fail to unlock your hidden powers.", ch, NULL, NULL, TO_CHAR );
    act( AT_RED, "$n fails to unlock $s hidden powers.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_ancientevolution );
  }

  ch->mana -= skill_table[gsn_ancientevolution]->min_mana;
  return;

}

void do_flare(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_flare))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_flare]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_flare]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_flare]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_flare]->focus;

    WAIT_STATE( ch, skill_table[gsn_flare]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_flare ) )
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
		act( AT_RED, "RYou clench your fists and fire begins rushing through your"
      	       	   "veins as you rush at $n. You grab them tightly as a sphere of fire" 
	             	   "forms around the both of you, letting them go as they get trapped" 
	                 "inside with you. Closing your eyes, you concentrate hard and increase"
	                 "the temperature of the sphere tenfold, opening your eyes again to watch" 
	                 "$T writh in searing agony. Suddenly you scream 'Now you DIE!'," 
	                 "leaving them unaffected!!!", ch, NULL, victim, TO_CHAR );
		act( AT_RED, "$N clenches his fists and fire begins rushing through $s veins as $e rushes"
	                 "at you. $e grabs you tightly as a sphere of fire forms around the both of you"
	                 "breaking free of $s grip as you get trapped inside. Closing $s eyes, $e" 
	                 "concentrates hard and increases the temperature of the sphere tenfold, opening" 
	                 "$s eyes again to watch you writh in searing agony. Suddenly $e screams 'Now you DIE!'"
	                 "the sphere explodes, leaving you unaffected", ch, NULL, victim, TO_VICT );
		act( AT_RED, "$N clenches $s fists and fire begins rushing through $s veins as $e rushes at $T. $e" 
	                 "grabs them tightly as a sphere of fire forms around the both of them. You see $N close" 
	                 "$s eyes and you feel the temperature radiating from the sphere increase tenfold as $n" 
	                 "writhes in pain. Suddenly $N screams 'Now you DIE!', and the sphere explodes, leaving" 
	                 "them unaffected.", ch, NULL, victim, TO_NOTVICT );
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
	act( AT_RED, "RYou clench your fists and fire begins rushing through your"
                 "veins as you rush at $n. You grab them tightly as a sphere of fire" 
                 "forms around the both of you, letting them go as they get trapped" 
                 "inside with you. Closing your eyes, you concentrate hard and increase"
                 "the temperature of the sphere tenfold, opening your eyes again to watch" 
                 "$T writh in searing agony. Suddenly you scream 'Now you DIE!'," 
                 "INCINERATING THEM INSTANTLY!!!", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$N clenches his fists and fire begins rushing through $s veins as $e rushes"
                 "at you. $e grabs you tightly as a sphere of fire forms around the both of you"
                 "breaking free of $s grip as you get trapped inside. Closing $s eyes, $e" 
                 "concentrates hard and increases the temperature of the sphere tenfold, opening" 
                 "$s eyes again to watch you writh in searing agony. Suddenly $e screams 'Now you DIE!'"
                 "the sphere explodes, INCINERATING YOU INSTANTLY", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$N clenches $s fists and fire begins rushing through $s veins as $e rushes at $T. $e" 
                 "grabs them tightly as a sphere of fire forms around the both of them. You see $N close" 
                 "$s eyes and you feel the temperature radiating from the sphere increase tenfold as $n" 
                 "writhes in pain. Suddenly $N screams 'Now you DIE!', and the sphere explodes, INCINERATING" 
                 "THEM INSTANTLY.", ch, NULL, victim, TO_NOTVICT );

	learn_from_success( ch, gsn_flare );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
	}
	default:
	dam = get_attmod(ch, victim) * number_range( 55, 65 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_RED, "You clench your fists and fire begins rushing through your veins as you rush at $N. You grab"
                 "them tightly as a sphere of fire forms around the both of you, letting them go as they get" 
                 "trapped inside with you. Closing your eyes, you concentrate hard and increase the temperature" 
                 "of the sphere tenfold, opening your eyes again to watch $N writh in searing agony. Suddenly "
                 "you scream 'Now you DIE!', the sphere explodes, sending $N sprawling to the floor. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_RED, "$N clenches his fists and fire begins rushing through $s veins as $e rushes at you. $e grabs you"
                 "tightly as a sphere of fire forms around the both of you, breaking free of $s grip as you get trapped"
                 "inside. Closing $s eyes, $e concentrates hard and increases the temperature of the sphere tenfold"
                 "opening $s eyes again to watch you writh in searing agony. Suddenly $e screams 'Now you DIE!', the" 
                 "sphere explodes, sending you sprawling to the floor.&W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_RED, "$N clenches $s fists and fire begins rushing through $s veins as $e rushes at $n. $e grabs them"
                 "tightly as a sphere of fire forms around the both of them. You see $N close $s eyes and you feel the"
                 "temperature radiating from the sphere increase tenfold as $n writhes in pain. Suddenly $N screams 'Now"
                 "you DIE!', and the sphere explodes, sending $n sprawling to the floor. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

	learn_from_success( ch, gsn_flare );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    break;
   	}
    }
    else
    {
	act( AT_RED, "You missed $N with your flare attack.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n misses you with $s flare attack.", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$n missed $N with a flare attack.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_flare );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_flare]->min_mana;
    return;
}

void do_atb(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_atb ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_atb]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_atb]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_atb]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_atb]->focus;

    WAIT_STATE( ch, skill_table[gsn_atb]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_atb ) )
    {
	dam = get_attmod(ch, victim) * number_range( 120, 140 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);

	act( AT_DGREY, "You raise both your palms high into the air, facing upwards,", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "as electricty surges up your arms. A black ball of energy", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "starts to form, dark red lightning arcing chaotically from", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "it. The ball grows to a massive size and density so extreme", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "that it begins to exert a gravitational field. Wherever the", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "red lightning strikes the ground, empty voids of completely", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "atomized space remains. This ball contains all the negative", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "energy ever collected from the Dragonballs. '&GYou will now DIE!!&z'", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "you yell at $N, flinging the Antimatter Bomb at $M. $E", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "tries to block the massive ball, but the resulting explosion", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "can be seen from miles away and leaves the area utterly vaporized. &W[&R$t&W]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_DGREY, "$n raises both $s palms high into the air, facing upwards,", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "as electricty surges up $s arms. A black ball of energy", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "starts to form, dark red lightning arcing chaotically from", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "it. The ball grows to a massive size, and density so extreme", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "that it begins to exert a gravitational field. Wherever the", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "red lightning strikes the ground, empty voids of completely", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "atomized space remains. This ball contains all the negative", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "energy ever collected from the Dragonballs. '&GYou will now DIE!!&z'", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "$e yells at you, flinging the Antimatter Bomb at you. You", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "try to block the massive ball, but the resulting explosion", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "can be seen from miles away and leaves the area utterly vaporized. &W[&R$t&W]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_DGREY, "$n raises both $s palms high into the air, facing upwards,", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "as electricty surges up $s arms. A black ball of energy", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "starts to form, dark red lightning arcing chaotically from", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "it. The ball grows to a massive size, and density so extreme", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "that it begins to exert a gravitational field. Wherever the", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "red lightning strikes the ground, empty voids of completely", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "atomized space remains. This ball contains all the negative", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "energy ever collected from the Dragonballs. '&GYou will now DIE!!&z'", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "$n yells at $N, flinging the Antimatter Bomb at them. $N", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "tries to block the massive ball, but the resulting explosion", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "can be seen from miles away and leaves the area utterly vaporized. &W[&R$t&W]", ch, num_punct(dam), victim, TO_NOTVICT );
        dam = ki_absorb( victim, ch, dam, gsn_atb );
	learn_from_success( ch, gsn_atb );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
    }
    else
    {
	act( AT_DGREY, "You raise both your palms high into the air, facing upwards,", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "as electricty surges up your arms. A black ball of energy", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "starts to form, dark red lightning arcing chaotically from", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "it. The ball grows to a massive size and density so extreme", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "that it begins to exert a gravitational field. Wherever the", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "red lightning strikes the ground, empty voids of completely", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "atomized space remains. The ball contains all the negative", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "energy ever collected from the Dragonballs. '&GYou will now DIE!!&z'", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "you yell at $N, flinging the Antimatter Bomb at $M. $E", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "tries to block the massive ball, but before $E can, the", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "antimatter bomb fizzles out. &W[&REPIC FAIL&W]", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "$n raises both $s palms high into the air, facing upwards,", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "as electricty surges up $s arms. A black ball of energy", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "starts to form, dark red lightning arcing chaotically from", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "it. The ball grows to a massive size, and density so extreme", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "that it begins to exert a gravitational field. Wherever the", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "red lightning strikes the ground, empty voids of completely", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "atomized space remains. The ball contains all the negative", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "energy ever collected from the Dragonballs. '&GYou will now DIE!!&z'", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "$e yells at you, flinging the Antimatter Bomb at you. You", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "tries to block the massive ball, but before you can, the", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "antimatter bomb fizzles out. &W[&REPIC FAIL&W]", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "$n raises both $s palms high into the air, facing upwards,", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "as electricty surges up $s arms. A black ball of energy", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "starts to form, dark red lightning arcing chaotically from", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "it. The ball grows to a massive size, and density so extreme", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "that it begins to exert a gravitational field. Wherever the", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "red lightning strikes the ground, empty voids of completely", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "atomized space remains. The ball contains all the negative", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "energy ever collected from the Dragonballs. '&GYou will now DIE!!&z'", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "$n yells at $N, flinging the Antimatter Bomb at them. $N", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "tries to block the massive ball, but before $E can, the", ch, NULL, victim, TO_NOTVICT );
	act( AT_DGREY, "antimatter bomb fizzles out. &W[&REPIC FAIL&W]", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_atb );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_atb]->min_mana;
    return;
}
void do_gust(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_gust ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_gust]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	  if( !xIS_SET( ( ch )->affected_by, AFF_ANCIENTEVO ) )
	  {
	    send_to_char( "You can't use a wing-based attack without wings. Doh!\n\r", ch );
	    return;
	  }

	if ( ch->mana < skill_table[gsn_gust]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_gust]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
/*	if ( ch->affected_by =! ancientevolution )
	{
		send_to_char( "You need to be in your Ancient form!\n\r", ch );
		return;
	}
*/
    else
    	ch->focus -= skill_table[gsn_gust]->focus;

    WAIT_STATE( ch, skill_table[gsn_gust]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_gust ) )
    {
	dam = get_attmod(ch, victim) * number_range( 50, 59 );
	{
	if (ch->charge > 0)
	dam = chargeDamMult(ch, dam);
	
	act( AT_WHITE, "You flex your wings and launch several feet into the air. "
			"You let out a horrible shriek as you beat your wings, sending "
			"torrents of air and debris flying at $N. &W[&R$t&W]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_WHITE, "$n flexes $s wings and launches several feet into the air. "
			"$e lets out a horrible shriek as $e beats $s wings, sending "
			"torrents of air and debris flying at you! &W[&R$t&W]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_WHITE, "$n flexes $s wings and launches several feet into the air. "
			"$e lets out a horrible shriek as $e beats $s wings, sending "
			"torrents of air and debris flying at $N! &W[&R$t&W]", ch, num_punct(dam), victim, TO_NOTVICT );
			learn_from_success( ch, gsn_gust );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
	}
    }
    else
    {
	act( AT_WHITE, "You flex your wings and launch several feet into the air. "
			"You let out a horrible shriek as you beat your wings, sending "
			"torrents of air and debris flying at $N.. but wait, where'd they "
			"go? &W[&RUSUX&W]", ch, NULL, victim, TO_CHAR );
	act( AT_WHITE, "$n flexes $s wings and launches several feet into the air. "
			"$e lets out a horrible shriek as $e beats $s wings, sending "
			"torrents of air and debris flying at you, but you quickly get "
			"out of the path of the attack. &W[&RUSUX&W]", ch, NULL, victim, TO_VICT );
	act( AT_WHITE, "$n flexes $s wings and launches several feet into the air. "
			"$e lets out a horrible shriek as $e beats $s wings, sending "
			"torrents of air and debris flying at $N, but $N quickly gets "
			"out of the path of the attack. &W[&RUSUX&W]", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_gust );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_gust]->min_mana;
    return;
}
void do_buffet(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_buffet ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_buffet]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_buffet]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_buffet]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
/*	if ( ch->affected_by =! ancientevolution )
	{
		send_to_char( "You need to be in your Ancient form!\n\r", ch );
		return;
	}
*/
    else
    	ch->focus -= skill_table[gsn_buffet]->focus;

    WAIT_STATE( ch, skill_table[gsn_buffet]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_buffet ) )
    {
	dam = get_attmod(ch, victim) * number_range( 60, 69 );
	{
	if (ch->charge > 0)
	dam = chargeDamMult(ch, dam);
	
	act( AT_WHITE, "You beat your wings and lift off into the air. Accelerating "
			"to your maximum speed, you perform a wide arc in the air above "
			"$N then turn sharply, headed straight towards them. When you are "
			"just about upon them, you straigten out your wings and brake hard, "
			"sending an extremely powerful buffet of wind and debris crashing "
			"into them! &W[&R$t&W]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_WHITE, "$n beats their wings and lifts off into the air. Accelerating "
			"to $s maximum speed, $e performs a wide arc in the air above you, "
			"then turns sharply and heads straight for you! When $e is almost "
			"upon you, $e straightens out $s wings and brakes hard, sending an "
			"extremely powerful buffet of wind and debris crashing into you! &W[&R$t&W]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_WHITE, "$n beats their wings and lifts off into the air. Accelerating "
			"to $s maximum speed, $e performs a wide arc in the air above $N, "
			"then turns sharply and heads straight for them! When $e is almost "
			"upon $N, $e straightens out $s wings and brakes hard, sending an "
			"extremely powerful buffet of wind and debris crashing into them! &W[&R$t&W]", ch, num_punct(dam), victim, TO_NOTVICT );
			learn_from_success( ch, gsn_buffet );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
	}
    }
    else
    {
	act( AT_WHITE, "You prepare to lift off, but suddenly your wings malfunction. &G[&rCRAMP!!&G]", ch, NULL, victim, TO_CHAR );
	act( AT_WHITE, "$n prepares to lift off, but suddenly $s wings malfunction. &G[&rCRAMP!!&G]", ch, NULL, victim, TO_VICT );
	act( AT_WHITE, "$n prepares to lift off, but suddenly $s wings malfunction. &G[&rCRAMP!!&G]", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_buffet );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_buffet]->min_mana;
    return;
}
void do_dragonfire(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_dragonfire ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_dragonfire]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_dragonfire]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_dragonfire]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
     else
    	ch->focus -= skill_table[gsn_dragonfire]->focus;

    WAIT_STATE( ch, skill_table[gsn_dragonfire]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_dragonfire ) )
    {
	switch (number_range(1,100))
	{
    case 100:
    case 99:
 	if (IS_NPC(victim) || (!IS_NPC(victim) && (!xIS_SET(ch->act, PLR_SPAR) || !xIS_SET(ch->act, PLR_SPAR)) ))
	{
	if (victim->pl / ch->pl >= 2)
	{
	act( AT_RED, "You draw your head back to gather some Dragon Fire, but "
			"$N presses the attack and you are forced to abandon the attempt.", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "You close too quickly and $n abandons their attempt at a "
			"Dragon Fire, adopting a defensive stance.", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$N closes too quickly and $n abandons their attempt at a "
			"Dragon Fire, adopting a defensive stance.", ch, NULL, victim, TO_NOTVICT );
	dam = 0;
	break;
	}
       dam= 999;
	act( AT_RED, "You lift off into the air, hovering several feet above $N. You glare "
			"at them as you gather Dragon Fire in your throat. Feeling the napalm-like "
			"fire about to overflow, you spew a gigantic stream at them. When the smoke "
			"clears, a large crater of molten lava is all that remains of the area.\n\r"
			"\n\r&W[&rINS&RT&WA&RK&rILL!&W]&D", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n lifts off into the air, hovering several feet above you. You catch "
			"their glare just before they spew a gigantic stream of Dragon Fire at "
			"you. You have no time to react, and you feel intense heat just before "
			"everything goes white.\n\r"
			"\n\r&W[&rINS&RT&WA&RK&rILL!&W]&D", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$n lifts off into the air, hovering several feet above $N. $N catches "
			"$n's glare just before $e spews a gigantic stream of Dragon Fire at "
			"$N. $N has no time to react, and they feel intense heat just before "
			"everything goes white. When the smoke clears, a large crater of molten "
			"lava is all that remains of the area.\n\r"
			"\n\r&W[&rINS&RT&WA&RK&rILL!&W]&D", ch, NULL, victim, TO_NOTVICT );

	learn_from_success( ch, gsn_dragonfire );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
	break;
	}
	default:
	dam = get_attmod(ch, victim) * number_range( 70, 80 );

	act( AT_RED, "You lift off into the air, hovering several feet above $N. You glare "
			"at them as you gather Dragon Fire in your throat. Feeling the napalm-like "
			"fire about to overflow, you spew a gigantic stream at them. When the smoke "
			"clears, $N is horribly burned. &W[&R$t&W]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_RED, "$n lifts off into the air, hovering several feet above you. You catch "
			"their glare just before they spew a gigantic stream of Dragon Fire at "
			"you. You cannot avoid it but you at least manage not to get hit directly. "
			"Nevertheless, you feel your skin bubbling as you are horribly burned! &W[&R$t&W]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_RED, "$n lifts off into the air, hovering several feet above $N. $N catches "
			"$n's glare just before $e spews a gigantic stream of Dragon Fire at "
			"$N. $N cannot avoid it but they at least manage not to get hit directly."
			"Nevertheless, $N feels their skin bubbling as they are horribly burned! &W[&R$t&W]", ch, num_punct(dam), victim, TO_NOTVICT );

	learn_from_success( ch, gsn_dragonfire );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
   break;
     }
    }
    else
    {
	act( AT_RED, "You miss $N with your Dragon Fire! &w[&GBURN!!&w]", ch, NULL, victim, TO_CHAR );
	act( AT_RED, "$n misses you with $s Dragon Fire! &w[&GBURN!!&w]", ch, NULL, victim, TO_VICT );
	act( AT_RED, "$n misses $N with Dragon Fire! &w[&GBURN!!&w]", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_dragonfire );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_dragonfire]->min_mana;

    return;
}

void do_dragon_thunder(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_dragon_thunder ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_dragon_thunder]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_dragon_thunder]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_dragon_thunder]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }

    else
    	ch->focus -= skill_table[gsn_dragon_thunder]->focus;

    WAIT_STATE( ch, skill_table[gsn_dragon_thunder]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_dragon_thunder ) )
    {
	dam = get_attmod(ch, victim) * number_range( 60, 70 );
	{
	if (ch->charge > 0)
	dam = chargeDamMult(ch, dam);
	
	act( AT_ORANGE, "You fold your arms across your chest, jumping back a few feet "
		"as $N goes off balance. You swing your arms forward, attacking in  "
            "$N's moment of weakness, you shout, '&RDragon Thunder!&O' A giant, bright "
            "&Ww&wh&Wi&wt&We&O bolt of lightning rockets down from the sky and strikes $N in a "
		"breathtaking fury! &W[&R$t&W]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_ORANGE, "$n folds $s arms across $s chest, jumping back a few feet "
		"as you go off balance. $n flings $s arms forward, attacking in "
            "your moment of weakness, shouting, '&RDragon Thunder!&O' A giant, bright "
            "&Ww&wh&Wi&wt&We&O bolt of lightning rockets down from the sky and strikes you in "
		"a breathtaking fury! &W[&R$t&W]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_ORANGE, "$n folds $s arms across $s chest, jumping back a few feet as "
		"$N goes off balance. $n flings $s arms forward, attacking in $N's "
            "moment of weakness, shouting, '&RDragon Thunder!&O' A giant, bright &Ww&wh&Wi&wt&We&O "
		"bolt of lightning rockets down from the sky and strikes $N! &W[&R$t&W]", ch, num_punct(dam), victim, TO_NOTVICT );
	learn_from_success( ch, gsn_dragon_thunder );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
	}
    }
    else
    {
	act( AT_ORANGE, "You missed $N with your dragon thunder. &z[&YSHOCKING!&z]", ch, NULL, victim, TO_CHAR );
	act( AT_ORANGE, "$n missed you with $s dragon thunder. &z[&YSHOCKING!&z]", ch, NULL, victim, TO_VICT );
	act( AT_ORANGE, "$n misses $N with a dragon thunder. &z[&YSHOCKING!&z]", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_dragon_thunder );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_dragon_thunder]->min_mana;
    return;
}

void do_whirlwind_spin(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_whirlwind_spin ))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_whirlwind_spin]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

	if ( ch->mana < skill_table[gsn_whirlwind_spin]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_whirlwind_spin]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }

    else
    	ch->focus -= skill_table[gsn_whirlwind_spin]->focus;

    WAIT_STATE( ch, skill_table[gsn_whirlwind_spin]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_whirlwind_spin ) )
    {
	dam = get_attmod(ch, victim) * number_range( 110, 115 );
	{
	if (ch->charge > 0)
	dam = chargeDamMult(ch, dam);
	
	act( AT_DGREY, "You cross your arms at the wrist, making a strange 'V' with  "
		"your hands. A mass of crackling, white air forms in the small space  "
            "between your hands. You jump away from $N's attack, crossing your hands "
            "in an 'X' to unsleash the still building attack. A jagged sphere of air  "
            "splashes against $N, causing their body and clothes to be ripped to shreads "
	     " as the power washes over $N. &W[&R$t&W]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_DGREY, "$n crosses their arms at the wrist, making a strange 'V' with their "
		"hands. A mass of crackling, white air forms in the small space between their "
              "hands. They jump away from your attack, crossing their hands in an 'X' to unleash "
              "the building attack. A jagged sphere of air splashes against you, causing your "
		"body and clothes to be ripped to shreads as the power washes over you. &W[&R$t&W]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_DGREY, "$n crosses their arms at the wrist, making a strange 'V' with their hands. "
                      "A mass of crackling, white air forms in the small space between $s hands "
                      "as $n jumps away from $N's attack, crossing their hands in an 'X' to unleash "
                      "the still building attack. A jagged sphere of air splashes against $N, "
		"causing their body and clothes to be ripped to shreds as the power washes over them. &W[&R$t&W]", ch, num_punct(dam), victim, TO_NOTVICT );
	learn_from_success( ch, gsn_whirlwind_spin );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
	}
    }
    else
    {
	act( AT_DGREY, "You missed $N with your whirldwind spin. &G[&zSit & Spin&G]", ch, NULL, victim, TO_CHAR );
	act( AT_DGREY, "$n missed you with $s whirldwind spin. &G[&zSit & Spin&G]", ch, NULL, victim, TO_VICT );
	act( AT_DGREY, "$n misses $N with a whirldwind spin. &G[&zSit & Spin&G]", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_whirlwind_spin );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_whirlwind_spin]->min_mana;
    return;
}

void do_gasattack(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dam = 0;

    if( IS_NPC(ch) && is_split(ch) )
    {
        if( !ch->master )
          return;
        if( !can_use_skill( ch->master, number_percent(), gsn_gasattack))
          return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You can't concentrate enough for that.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch)
    &&   ch->exp < skill_table[gsn_gasattack]->skill_level[ch->class] )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if ( ( victim = who_fighting( ch ) ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }
 	if (IS_NPC(victim) || (!IS_NPC(victim) && xIS_SET(ch->act, PLR_SPAR)) || xIS_SET(ch->act, PLR_SPAR) )
	{
	    send_to_char( "&RPKILL ONLY N00B - Kal.\n\r", ch );
	    return;
	}
	if ( ch->mana < skill_table[gsn_gasattack]->min_mana )
	{
	    send_to_char( "You don't have enough energy.\n\r", ch );
	    return;
	}
	if (ch->focus < skill_table[gsn_gasattack]->focus)
    {
		send_to_char( "You need to focus more.\n\r", ch );
		return;
    }
    else
    	ch->focus -= skill_table[gsn_gasattack]->focus;

    WAIT_STATE( ch, skill_table[gsn_gasattack]->beats );
    if ( can_use_skill(ch, number_percent(),gsn_gasattack ) )
    {
	dam = get_attmod(ch, victim) * number_range( 10, 15 );
	if (ch->charge > 0)
		dam = chargeDamMult(ch, dam);
	act( AT_GREEN, "You close your eyes and begin to focus your energy as you breath in real deep and release"
                 "it slowly and methodically. What once was air that entered your lungs is now exhaled as" 
                 "a large cloud of neurotoxic gases. It slowly grows large enough, envolping $N and yourself." 
                 "You slowly open your eyes and watch as $N begins to buckle under the toxins power. As"
                 "time withers away, so does $S transformation, until $E is completely powerd down. &W[$t]", ch, num_punct(dam), victim, TO_CHAR );
	act( AT_GREEN, "$n closes $s eyes and begins to focus $s energy as $e breathes in real deep and releases"
                 "it slowly and methodically. What once was air that entered $s lungs is now exhaled as" 
                 "a large cloud of neurotoxic gases. It slowly grows large enough, envolping you and $n." 
                 "$n slowly opens $s eyes and watches as you begin to buckle under the toxins power. As"
                 "time withers away, so does your transformation, until you is completely powerd down. &W[$t]", ch, num_punct(dam), victim, TO_VICT );
	act( AT_GREEN, "$n closes $s eyes and begins to focus $s energy as $e breathes in real deep and releases"
                 "it slowly and methodically. What once was air that entered $s lungs is now exhaled as" 
                 "a large cloud of neurotoxic gases. It slowly grows large enough, envolping $N and $n." 
                 "$n slowly opens $s eyes and watches as $N begins to buckle under the toxins power. As"
                 "time withers away, so does $N's transformation, until $E is completely powerd down. &W[$t]", ch, num_punct(dam), victim, TO_NOTVICT );

	learn_from_success( ch, gsn_gasattack );
	global_retcode = damage( ch, victim, dam, TYPE_HIT );
      if( xIS_SET( ( victim )->affected_by, AFF_SSJ ) )
      {
        xREMOVE_BIT( ( victim )->affected_by, AFF_SSJ );
        if( !IS_NPC( victim ) )
        {
          ch->pcdata->haircolor = ch->pcdata->orignalhaircolor;
          ch->pcdata->eyes = ch->pcdata->orignaleyes;
        }
      }
      if( xIS_SET( ( victim )->affected_by, AFF_USSJ ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_USSJ );
      if( xIS_SET( ( victim )->affected_by, AFF_USSJ2 ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_USSJ2 );
      if( xIS_SET( ( victim )->affected_by, AFF_SSJ2 ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_SSJ2 );
      if( xIS_SET( ( victim )->affected_by, AFF_SSJ3 ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_SSJ3 );
      if( xIS_SET( ( victim )->affected_by, AFF_SSJ4 ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_SSJ4 );
      if( xIS_SET( ( victim )->affected_by, AFF_KAIOKEN ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_KAIOKEN );
      if( xIS_SET( ( victim )->affected_by, AFF_SNAMEK ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_SNAMEK );
      if( xIS_SET( ( victim )->affected_by, AFF_HYPER ) )
        xREMOVE_BIT( victim->affected_by, AFF_HYPER );
      if( xIS_SET( ( victim )->affected_by, AFF_EXTREME ) )
        xREMOVE_BIT( ( victim )->affected_by, AFF_EXTREME );
      if( xIS_SET( ( victim )->affected_by, AFF_GROWTH ) )
        xREMOVE_BIT( victim->affected_by, AFF_GROWTH );
      if( xIS_SET( ( victim )->affected_by, AFF_GIANT ) )
        xREMOVE_BIT( victim->affected_by, AFF_GIANT );
      if( xIS_SET( ( victim )->affected_by, AFF_EVIL_TRANS ) )
        xREMOVE_BIT( victim->affected_by, AFF_EVIL_TRANS );
      if( xIS_SET( ( victim )->affected_by, AFF_SUPER_TRANS ) )
        xREMOVE_BIT( victim->affected_by, AFF_SUPER_TRANS );
      if( xIS_SET( ( victim )->affected_by, AFF_KID_TRANS ) )
        xREMOVE_BIT( victim->affected_by, AFF_KID_TRANS );
      if( xIS_SET( ( victim )->affected_by, AFF_MYSTIC ) )
        xREMOVE_BIT( victim->affected_by, AFF_MYSTIC );
      if( xIS_SET( ( victim )->affected_by, AFF_LSSJ ) )
        xREMOVE_BIT( victim->affected_by, AFF_LSSJ );
      if( xIS_SET( ( victim )->affected_by, AFF_MAJIN ) )
        xREMOVE_BIT( victim->affected_by, AFF_MAJIN );
      if( xIS_SET( ( victim )->affected_by, AFF_SIGMA_VIRUS ) )
        xREMOVE_BIT( victim->affected_by, AFF_SIGMA_VIRUS );
      if( xIS_SET( ( victim )->affected_by, AFF_SENT_CHIP ) )
        xREMOVE_BIT( victim->affected_by, AFF_SENT_CHIP );
      if( xIS_SET( ( victim )->affected_by, AFF_EVILBOOST ) )
        xREMOVE_BIT( victim->affected_by, AFF_EVILBOOST );
      if( xIS_SET( ( victim )->affected_by, AFF_EVILSURGE ) )
        xREMOVE_BIT( victim->affected_by, AFF_EVILSURGE );
      if( xIS_SET( ( victim )->affected_by, AFF_EVILOVERLOAD ) )
        xREMOVE_BIT( victim->affected_by, AFF_EVILOVERLOAD );
      if( xIS_SET( ( victim )->affected_by, AFF_UNAMEK ) )
        xREMOVE_BIT( victim->affected_by, AFF_UNAMEK );
      if( xIS_SET( ( victim )->affected_by, AFF_DEMBRACE ) )
        xREMOVE_BIT( victim->affected_by, AFF_DEMBRACE );
      if( xIS_SET( ( victim )->affected_by, AFF_ARAPTURE ) )
        xREMOVE_BIT( victim->affected_by, AFF_ARAPTURE );
      if( xIS_SET( ( victim )->affected_by, AFF_SERALIGHT ) )
        xREMOVE_BIT( victim->affected_by, AFF_SERALIGHT );
      if( xIS_SET( ( victim )->affected_by, AFF_SUPER_OOZARU ) )
        xREMOVE_BIT( victim->affected_by, AFF_SUPER_OOZARU );
      if( xIS_SET( ( victim )->affected_by, AFF_HSSJ ) )
        xREMOVE_BIT( victim->affected_by, AFF_HSSJ );
      if( xIS_SET( ( victim )->affected_by, AFF_ESSJ ) )
        xREMOVE_BIT( victim->affected_by, AFF_ESSJ );
      if( xIS_SET( ( victim )->affected_by, AFF_SAIBAMAN1 ) )
        xREMOVE_BIT( victim->affected_by, AFF_SAIBAMAN1 );
      if( xIS_SET( ( victim )->affected_by, AFF_SAIBAMAN2 ) )
        xREMOVE_BIT( victim->affected_by, AFF_SAIBAMAN2 );
      if( xIS_SET( ( victim )->affected_by, AFF_SAIBAMAN3 ) )
        xREMOVE_BIT( victim->affected_by, AFF_SAIBAMAN3 );
      if( xIS_SET( ( victim )->affected_by, AFF_PUPPET ) )
        xREMOVE_BIT( victim->affected_by, AFF_PUPPET );
      if( xIS_SET( ( victim )->affected_by, AFF_SFLAME1 ) )
        xREMOVE_BIT( victim->affected_by, AFF_SFLAME1 );
      if( xIS_SET( ( victim )->affected_by, AFF_SFLAME2 ) )
        xREMOVE_BIT( victim->affected_by, AFF_SFLAME2 );	
      if( xIS_SET( ( victim )->affected_by, AFF_SFLAME3 ) )
        xREMOVE_BIT( victim->affected_by, AFF_SFLAME3 );
      if( xIS_SET( ( victim )->affected_by, AFF_UFLAME ) )
        xREMOVE_BIT( victim->affected_by, AFF_UFLAME );
      if( xIS_SET( ( victim )->affected_by, AFF_ADULT ) )
        xREMOVE_BIT( victim->affected_by, AFF_ADULT );
      if( xIS_SET( ( victim )->affected_by, AFF_DEKU ) )
        xREMOVE_BIT( victim->affected_by, AFF_DEKU );
      if( xIS_SET( ( victim )->affected_by, AFF_GORON ) )
        xREMOVE_BIT( victim->affected_by, AFF_GORON );
      if( xIS_SET( ( victim )->affected_by, AFF_ZORA ) )
        xREMOVE_BIT( victim->affected_by, AFF_ZORA );	
      if( xIS_SET( ( victim )->affected_by, AFF_FDEITY ) )
        xREMOVE_BIT( victim->affected_by, AFF_FDEITY );
      if( xIS_SET( ( victim )->affected_by, AFF_INNERWILL ) )
        xREMOVE_BIT( victim->affected_by, AFF_INNERWILL );
      if( xIS_SET( ( victim )->affected_by, AFF_MECHAEVO ) )
        xREMOVE_BIT( victim->affected_by, AFF_MECHAEVO );	
      if( xIS_SET( ( victim )->affected_by, AFF_ANCIENTEVO ) )
        xREMOVE_BIT( victim->affected_by, AFF_ANCIENTEVO );
      if( !is_android( victim ) && !is_superandroid( victim ) && !IS_BIOANDROID( victim ) && !is_reploid( victim ) && !IS_ICER( victim ) )
      {
        ch->powerup = 0;
        if( xIS_SET( ( victim )->affected_by, AFF_HEART ) )
          xREMOVE_BIT( victim->affected_by, AFF_HEART );
        transStatRemove( victim );
        victim->pl = victim->exp;
      }
    }
    else
    {
	act( AT_GREEN, "Your neurotoxic gas left $N unaffected.", ch, NULL, victim, TO_CHAR );
	act( AT_GREEN, "$n's neurotoxic gas leaves you unaffected.", ch, NULL, victim, TO_VICT );
	act( AT_GREEN, "$n's neurotoxic gas left $N unaffected.", ch, NULL, victim, TO_NOTVICT );
	learn_from_failure( ch, gsn_gasattack );
	global_retcode = damage( ch, victim, 0, TYPE_HIT );
    }
	ch->mana -= skill_table[gsn_gasattack]->min_mana;
    return;
}
