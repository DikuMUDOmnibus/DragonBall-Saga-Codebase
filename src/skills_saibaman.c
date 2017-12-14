#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

extern void transStatApply args( ( CHAR_DATA * ch, int strMod, int spdMod, int intMod, int conMod ) );
extern void transStatRemove args( ( CHAR_DATA * ch ) );


void do_headbutt( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_headbutt]->skill_level[ch->class] )
  {
    send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_headbutt]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_headbutt]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_headbutt]->focus;

  WAIT_STATE( ch, skill_table[gsn_headbutt]->beats );

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_headbutt]->min_mana;

  if( can_use_skill( ch, number_percent(  ), gsn_headbutt ) )
  {
    act( AT_SKILL, "Bursting into a frantic charge with your head held low, you strategically", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "pinpoint the location of impact only seconds before following through.", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "Making contact with your thick cranium leaves $n gasping for air", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "while an imprint of your skull is left embedded in their abdomen.", ch, NULL, victim, TO_CHAR );

    act( AT_SKILL, "$n bursts into a frantic charge with their head held low, $n strategically", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_SKILL, "pinpoint the location of impact only seconds before following through.", ch, NULL, victim, TO_NOTVICT );
    act( AT_SKILL, "Making contact with $n thick cranium leaves $N gasping for air", ch, NULL, victim, TO_NOTVICT );
    act( AT_SKILL, "while an imprint of $n's skull is left embedded in $N's abdomen. ", ch, NULL, victim, TO_NOTVICT );

    act( AT_SKILL, "$n bursts into a frantic charge with $s head held low, $n strategically", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "pinpoint the location of impact only seconds before following through.", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "Making contact with their thick cranium leaves you gasping for air", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "while an imprint of their skull is left embedded in your abdomen.", ch, NULL, victim, TO_VICT );

    learn_from_success( ch, gsn_headbutt );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 2, 8 ) ), gsn_headbutt );
    ch->melee = FALSE;
  }
  else
  {
    act( AT_SKILL, "You charge at $N full force but fall as they move away quicky.", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "$n charges at you but falls over their own feet missing you.", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "$n charges at $N full force but misses them by a mile.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_headbutt );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, 0, gsn_headbutt );
    ch->melee = FALSE;
  }
  return;
}


void do_acid_burn( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_acid_burn]->skill_level[ch->class] )
  {
    send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_acid_burn]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_acid_burn]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_acid_burn]->focus;

  WAIT_STATE( ch, skill_table[gsn_acid_burn]->beats );

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_acid_burn]->min_mana;

  if( can_use_skill( ch, number_percent(  ), gsn_acid_burn ) )
  {
    act( AT_SKILL, "Feet firmly planted, you let out a shriek of bitterness that pierces", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "everyone's ears in the nearby vicinity.  While tilting your head slightly", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "downward, an enormous crack appears on your forehead and quickly travels", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "backwards.  As if it were spring loaded, your skull quickly bursts open and", ch, NULL, victim,
         TO_CHAR );
    act( AT_SKILL, "releases an acidic substance towards $N.  The flesh eating liquid", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "quickly consumes any contacting part and leaves $s severely", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "scalded.", ch, NULL, victim, TO_CHAR );

    act( AT_SKILL, "Feet firmly planted, $n let out a shriek of bitterness that pierces", ch, NULL, victim, TO_NOTVICT );
    act( AT_SKILL, "your ears shattering your eardrums.  $n tilts their head slightly", ch, NULL, victim, TO_NOTVICT );
    act( AT_SKILL, "downward, an enormous crack appears in the middle of their forehead and quickly travels", ch, NULL,
         victim, TO_NOTVICT );
    act( AT_SKILL, "backwards.  As if it were spring loaded, $n's skull quickly bursts open and", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_SKILL, "releases an acidic substance toward $N.  The flesh eating liquid", ch, NULL, victim, TO_NOTVICT );
    act( AT_SKILL, "quickly consumes any contacting parts and leaving them severely", ch, NULL, victim, TO_NOTVICT );
    act( AT_SKILL, "scalded.", ch, NULL, victim, TO_NOTVICT );

    act( AT_SKILL, "Feet firmly planted, $n let out a shriek of bitterness that pierces", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "everyone's ears in the nearby vicinity.  While tilting $s's head slightly", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "downward, an enormous crack appears on their forehead and quickly travels", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "backwards.  As if it were spring loaded, $n's skull quickly bursts open and", ch, NULL, victim,
         TO_VICT );
    act( AT_SKILL, "releases an acidic substance toward you.  The flesh eating liquid", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "quickly consumes any contacting parts and leaving you severely", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "scalded.", ch, NULL, victim, TO_VICT );

    learn_from_success( ch, gsn_acid_burn );
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 18, 26 ) ), gsn_acid_burn );
  }
  else
  {
    act( AT_SKILL, "You open your head and spray a bolling jet of acid, but you miss $N.", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "$n opens $s head spraying a jet of acid out, but $e misses you.", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "$n opens $s head spraying a jet of acid out, but $e misses $N.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_acid_burn );
    global_retcode = damage( ch, victim, 0, gsn_acid_burn );
  }
  return;
}

void do_saibaman1( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) && !is_split( ch ) )
    return;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_saibaman1 ) )
      return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN1 ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_SAIBAMAN1 );
    ch->pl = ch->exp;
    transStatRemove( ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN2 ) || xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN3 ) )
  {
    send_to_char( "Powerdown first!!\n\r", ch );
    return;
  }
  if( ch->mana < skill_table[gsn_saibaman1]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_saibaman1]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_saibaman1 ) )
  {
    act( z, "You firmly plant your feet into the soil below while gazing upwards.", ch, NULL, NULL, TO_CHAR );
    act( z, "Without caution, numerous roots spring forward and intertwine together", ch, NULL, NULL, TO_CHAR );
    act( z, "while progressively grasping your body tightly. Attached to the skin", ch, NULL, NULL, TO_CHAR );
    act( z, "tautly, the roots slowly peel away your existing hide as they retreat back", ch, NULL, NULL, TO_CHAR );
    act( z, "into the ground.  The muscle fibers quickly refurbish your form with fresh", ch, NULL, NULL, TO_CHAR );
    act( z, "tissue that reveals an effective and more robust combatant.", ch, NULL, NULL, TO_CHAR );

    act( z, "$n firmly plant $s feet into the soil below them while gazing upwards.", ch, NULL, NULL, TO_NOTVICT );
    act( z, "Without caution, numerous roots spring forward and intertwine together", ch, NULL, NULL, TO_NOTVICT );
    act( z, "while progressively grasping $s body tightly. Attached to the skin", ch, NULL, NULL, TO_NOTVICT );
    act( z, "tautly, the roots slowly peel away their existing hide as they retreat back", ch, NULL, NULL, TO_NOTVICT );
    act( z, "into the ground.  The muscle fibers quickly refurbish the form with fresh", ch, NULL, NULL, TO_NOTVICT );
    act( z, "tissue that reveals an effective and more robust combatant.", ch, NULL, NULL, TO_NOTVICT );
    xSET_BIT( ( ch )->affected_by, AFF_SAIBAMAN1 );
    ch->pl = ch->exp * 16;
    if( xIS_SET( ( ch )->affected_by, AFF_GROWTH ) )
      transStatApply( ch, 13, -4, 9, 10 );
    else
      transStatApply( ch, 3, 6, 4, 5 );
    if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
      ch->pl = ch->pl * 1.1;
    learn_from_success( ch, gsn_saibaman1 );
  }

  else
  {
    act( z, "You fail.", ch, NULL, NULL, TO_CHAR );
    act( z, "$n failed to transform.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_saibaman1 );
  }

  ch->mana -= skill_table[gsn_saibaman1]->min_mana;
  return;
}

void do_saibaman2( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) && !is_split( ch ) )
    return;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_saibaman2 ) )
      return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN2 ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_SAIBAMAN2 );
    ch->pl = ch->exp;
    transStatRemove( ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN1 ) || xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN3 ) )
  {
    send_to_char( "Powerdown first!!\n\r", ch );
    return;
  }
  if( ch->mana < skill_table[gsn_saibaman2]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_saibaman2]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_saibaman2 ) )
  {
    act( z, "Bracing yourself firmly, you send your roots even deeper into the", ch, NULL, NULL, TO_CHAR );
    act( z, "firmament of the planet, draining more sustenance from the soil.  You raise", ch, NULL, NULL, TO_CHAR );
    act( z, "your arms up high, absorbing as much light as possible.  Your fibrous", ch, NULL, NULL, TO_CHAR );
    act( z, "tissues peel away, quickly being replaced by a harder, stronger substance. ", ch, NULL, NULL, TO_CHAR );
    act( z, "The roots send a torrent of energy throughout your system, encasing you in", ch, NULL, NULL, TO_CHAR );
    act( z, "a bright aura.  The transformation completes, turning you into a much", ch, NULL, NULL, TO_CHAR );
    act( z, "more potent life form.", ch, NULL, NULL, TO_CHAR );

    act( z, "$n braces themself firmly, sending their roots even deeper into the", ch, NULL, NULL, TO_NOTVICT );
    act( z, "firmament of the planet, draining more sustenance from the soil.  $n raise", ch, NULL, NULL, TO_NOTVICT );
    act( z, "$s arms up high, absorbing as much light as possible.  $n's fibrous", ch, NULL, NULL, TO_NOTVICT );
    act( z, "tissues peel away, quickly being replaced by a harder, stronger shell. ", ch, NULL, NULL, TO_NOTVICT );
    act( z, "The roots send a torrent of visable energy throughout their system,", ch, NULL, NULL, TO_NOTVICT );
    act( z, "encasing them in a bright aura.  The transformation completes, turning", ch, NULL, NULL, TO_NOTVICT );
    act( z, "$n into a much more potent life form.", ch, NULL, NULL, TO_NOTVICT );

    xSET_BIT( ( ch )->affected_by, AFF_SAIBAMAN2 );
    ch->pl = ch->exp * 22;
    if( xIS_SET( ( ch )->affected_by, AFF_GROWTH ) )
      transStatApply( ch, 16, 2, 14, 15 );
    else
      transStatApply( ch, 6, 12, 9, 10 );
    if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
      ch->pl = ch->pl * 1.1;
    learn_from_success( ch, gsn_saibaman2 );
  }

  else
  {
    act( z, "You failed.", ch, NULL, NULL, TO_CHAR );
    act( z, "$n failed to transform.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_saibaman2 );
  }

  ch->mana -= skill_table[gsn_saibaman2]->min_mana;
  return;
}

void do_saibaman3( CHAR_DATA * ch, char *argument )
{
  if( IS_NPC( ch ) && !is_split( ch ) )
    return;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_saibaman3 ) )
      return;
  }

  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN3 ) )
  {
    send_to_char( "You power down and return to normal.\n\r", ch );
    xREMOVE_BIT( ( ch )->affected_by, AFF_SAIBAMAN3 );
    ch->pl = ch->exp;
    transStatRemove( ch );
    return;
  }
  if( xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN1 ) || xIS_SET( ( ch )->affected_by, AFF_SAIBAMAN2 ) )
  {
    send_to_char( "Powerdown first!!\n\r", ch );
    return;
  }
  if( ch->mana < skill_table[gsn_saibaman3]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }

  sh_int z = get_aura( ch );

  WAIT_STATE( ch, skill_table[gsn_saibaman3]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_saibaman3 ) )
  {
    act( z, "You gaze up into the sky, letting the light from the stars wash over", ch, NULL, NULL, TO_CHAR );
    act( z, "your face and body.  Your photosynthetic fibers absorb the ultraviolet", ch, NULL, NULL, TO_CHAR );
    act( z, "rays, converting it into raw energy.  You unlock all the restraints within", ch, NULL, NULL, TO_CHAR );
    act( z, "yourself, letting the damn of power slam through your body.  Your outer", ch, NULL, NULL, TO_CHAR );
    act( z, "layer of tissue rips away, as newer, tougher fibers rush in to replace", ch, NULL, NULL, TO_CHAR );
    act( z, "them.  Your limbs grow even longer, the stalk of your body expanding to", ch, NULL, NULL, TO_CHAR );
    act( z, "match the growth. A network of interlocked thorns sprout from your", ch, NULL, NULL, TO_CHAR );
    act( z, "bark-skin, creating a thick spiky armor. You have transformed into a giant", ch, NULL, NULL, TO_CHAR );
    act( z, "saibaman! ", ch, NULL, NULL, TO_CHAR );

    act( z, "$n gazes up into the sky, letting the light from the stars wash over", ch, NULL, NULL, TO_NOTVICT );
    act( z, "their face and body.  The photosynthetic fibers absorb the ultraviolet", ch, NULL, NULL, TO_NOTVICT );
    act( z, "rays, converting them in raw energy. $n unlocks all the restraints within", ch, NULL, NULL, TO_NOTVICT );
    act( z, "themself, letting the dam of power slam through $s body. The outer", ch, NULL, NULL, TO_NOTVICT );
    act( z, "layer of tissue rips away, as newer, tougher fibers rush in to replace", ch, NULL, NULL, TO_NOTVICT );
    act( z, "them. Their limbs grow even longer, the stalk of $n's body expanding to", ch, NULL, NULL, TO_NOTVICT );
    act( z, "match the growth. A network of interlocked thorns sprout from $n's", ch, NULL, NULL, TO_NOTVICT );
    act( z, "bark-skin, creating a thick spiky armor. $n have transformed into a giant", ch, NULL, NULL, TO_NOTVICT );
    act( z, "saibaman! ", ch, NULL, NULL, TO_NOTVICT );

    xSET_BIT( ( ch )->affected_by, AFF_SAIBAMAN3 );
    ch->pl = ch->exp * 28;
    if( xIS_SET( ( ch )->affected_by, AFF_GROWTH ) )
      transStatApply( ch, 22, 14, 23, 25 );
    else
      transStatApply( ch, 12, 24, 18, 20 );
    if( xIS_SET( ( ch )->affected_by, AFF_MAJIN ) )
      ch->pl = ch->pl * 1.1;
    learn_from_success( ch, gsn_saibaman3 );
  }

  else
  {
    act( z, "You failed.", ch, NULL, NULL, TO_CHAR );
    act( z, "$n failed to transform.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_saibaman3 );
  }

  ch->mana -= skill_table[gsn_saibaman3]->min_mana;
  return;
}

void do_jibaku( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  CHAR_DATA *vch;
  int dam = 0;
//    int chargesUsed = 50;
//    char arg[MAX_STRING_LENGTH];
//    int addCharges = 0;

  if( xIS_SET( ch->act, PLR_SPAR ) )
  {
    send_to_char( "Bad!  You can't kill in spars!.\n\r", ch );
    return;
  }

  if( IS_NPC( vch ) )
  {
    send_to_char( "&GThis is &RPKILL ONLY!!&G - Kal&D\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_jibaku]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_jibaku]->min_mana )
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

  if( !IS_NPC( ch ) && ch->focus < skill_table[gsn_jibaku]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
  if( !IS_NPC( ch ) )
  {
    ch->focus -= skill_table[gsn_jibaku]->focus;
  }
  WAIT_STATE( ch, skill_table[gsn_jibaku]->beats );
//    ch->pcdata->sd_charge = 0;
//    ch->pcdata->sd_charge -= chargesUsed;
  if( can_use_skill( ch, number_percent(  ), gsn_jibaku ) )
  {
    act( AT_DGREEN, "Destined to be the victor of this battle &n run and jump onto $N", ch, NULL, victim, TO_NOTVICT );
    act( AT_DGREEN, "while bracing them tightly. With nothing left to lose $n flare up", ch, NULL, victim, TO_NOTVICT );
    act( AT_DGREEN, "$s energy and their aura gusts violently. With one last breath $n scream", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_DGREEN, "out &G'JIBAKU!'&g  and a blatant eruption of light is brought fourth. ", ch, NULL, victim, TO_NOTVICT );
    act( AT_DGREEN, "Shrouding everything in its path, the unmerciful radiance obliterates", ch, NULL, victim, TO_NOTVICT );
    act( AT_DGREEN, "everything in its path. ", ch, NULL, victim, TO_NOTVICT );

    act( AT_DGREEN, "Destined to be the victor of this battle you run and jump onto $N", ch, NULL, victim, TO_CHAR );
    act( AT_DGREEN, "while bracing them tightly.  With nothing left to lose you flare up", ch, NULL, victim, TO_CHAR );
    act( AT_DGREEN, "your energy and your aura gusts violently.  With one last breath you scream", ch, NULL, victim,
         TO_CHAR );
    act( AT_DGREEN, "out &G'JIBAKU!'&g  and a blatant eruption of light is brought fourth. ", ch, NULL, victim, TO_CHAR );
    act( AT_DGREEN, "Shrouding everything in its path, the unmerciful radiance obliterates", ch, NULL, victim, TO_CHAR );
    act( AT_DGREEN, "everything in its path. ", ch, NULL, victim, TO_CHAR );

    act( AT_DGREEN, "Destined to be the victor of this battle $n run and jump onto you", ch, NULL, victim, TO_VICT );
    act( AT_DGREEN, "while bracing you tightly.  With nothing left to lose $n flare up", ch, NULL, victim, TO_VICT );
    act( AT_DGREEN, "their energy and $s aura gusts violently.  With one last breath they", ch, NULL, victim, TO_VICT );
    act( AT_DGREEN, "scream out &G'JIBAKU!'&g  and a blatant eruption of light is brought", ch, NULL, victim, TO_VICT );
    act( AT_DGREEN, "fourth. Shrouding everything in its path, the unmerciful radiance", ch, NULL, victim, TO_VICT );
    act( AT_DGREEN, "obliterates everything in its path.", ch, NULL, victim, TO_VICT );

    learn_from_success( ch, gsn_jibaku );
    //dam = victim->max_hit * 2;
    if( IS_NPC(ch) )
    { 
      dam = 50;
    }
    else
    {
      dam = 999999999;
    }
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
//  dam = ch->max_hit * 2;
    //dam = 50 * ((chargesUsed-50)/10);
    dam = 999999999;
    global_retcode = damage( ch, ch, dam, TYPE_HIT );
  }
  else
  {
    act( AT_SKILL, "You grab ahold of $N and as you are about to self destruct, they narrowly escape as you explode.", ch,
         NULL, victim, TO_CHAR );
    act( AT_SKILL, "$n grabs ahold of you and as they are about to explode, you escape!", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "$n grabs ahold of $N and as $n is about to explode $N escapes the blast.", ch, NULL, victim,
         TO_NOTVICT );
    learn_from_failure( ch, gsn_jibaku );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
//  dam = ch->max_hit * 2;
    dam = 999999999;
    global_retcode = damage( ch, ch, dam, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_jibaku]->min_mana;
  return;
}
void do_roundhouse( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_roundhouse]->skill_level[ch->class] )
  {
    send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_roundhouse]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_roundhouse]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_roundhouse]->focus;

  WAIT_STATE( ch, skill_table[gsn_roundhouse]->beats );

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_roundhouse]->min_mana;

  if( can_use_skill( ch, number_percent(  ), gsn_roundhouse ) )
  {
    act( AT_LBLUE, "Compressing your left leg into a crouched position, you strain your leg", ch, NULL, victim, TO_CHAR );
    act( AT_LBLUE, "muscles until they can noticeably be seen bulging outwards.  In one swift", ch, NULL, victim, TO_CHAR );
    act( AT_LBLUE, "motion you spring upwards and rotate your body full circle with your right", ch, NULL, victim, TO_CHAR );
    act( AT_LBLUE, "limb extended.  Your foot firmly plants into $N's chest and sends", ch, NULL, victim, TO_CHAR );
    act( AT_LBLUE, "them tumbling to the ground.", ch, NULL, victim, TO_CHAR );

    act( AT_LBLUE, "$n compresses $s left leg into a crouched position, $n strain $s leg", ch, NULL, victim, TO_NOTVICT );
    act( AT_LBLUE, "muscles until they can noticeably be seen bulging outwards.  In one swift", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_LBLUE, "motion $n spring upwards and rotate $s body full circle with thir right", ch, NULL, victim, TO_NOTVICT );
    act( AT_LBLUE, "limb extended.  $n's foot firmly plants into $N's chest and sends", ch, NULL, victim, TO_NOTVICT );
    act( AT_LBLUE, "them tumbling to the ground. ", ch, NULL, victim, TO_NOTVICT );

    act( AT_LBLUE, "$n compressing thir left leg into a crouched position, they strain their leg", ch, NULL, victim,
         TO_VICT );
    act( AT_LBLUE, "muscles until they can noticeably be seen bulging outwards.  In one swift", ch, NULL, victim, TO_VICT );
    act( AT_LBLUE, "motion they spring upwards and rotate their body full circle with their right", ch, NULL, victim,
         TO_VICT );
    act( AT_LBLUE, "limb extended.  Their foot firmly plants into your chest and sends", ch, NULL, victim, TO_VICT );
    act( AT_LBLUE, "you tumbling to the ground. ", ch, NULL, victim, TO_VICT );

    learn_from_success( ch, gsn_roundhouse );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 68, 120 ) ), gsn_roundhouse );
    ch->melee = FALSE;
  }
  else
  {
    act( AT_SKILL, "You twist and fly at $N but miss.", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "$n kicks hard at you but misses you by an inch.", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "$n kicks hard at $N but $N dodges quickly.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_roundhouse );
    ch->melee = TRUE;
    global_retcode = damage( ch, victim, 0, gsn_roundhouse );
    ch->melee = FALSE;
  }
  return;
}

void do_acid_slash( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;
  int dam = 0;

  if( IS_NPC( ch ) && is_split( ch ) )
  {
    if( !ch->master )
      return;
    if( !can_use_skill( ch->master, number_percent(  ), gsn_acid_slash ) )
      return;
  }

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_acid_slash]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( ch->mana < skill_table[gsn_acid_slash]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_acid_slash]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_acid_slash]->focus;

  WAIT_STATE( ch, skill_table[gsn_acid_slash]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_acid_slash ) )
  {
    dam = get_attmod( ch, victim ) * number_range( 68, 94 );
    if( ch->charge > 0 )
      dam = chargeDamMult( ch, dam );
    act( AT_GREEN,
         "Your feet claws dig into the ground, bracing yourself to where you stand.  You grin maniacally, before grabbing each side of your head.  You slowly pull on your head, tearing your forehead apart with a sickening squelch.  You release one side of your head to dip your claw into the gap, slowly swirling your claw around inside.  As you pull your claw out, you charge energy into it, and the acid that now covers it.  You look at your claw, as it drips with glowing green acid, the droplets burning into the ground at your feet.  Suddenly, you release your grip to the ground, and propel yourself to $N, slashing quickly at $S chest! &W[$t]",
         ch, num_punct( dam ), victim, TO_CHAR );
    act( AT_GREEN,
         "$n's feet dig into the ground, bracing $mself to where $e stands.  $n grins maniacally, before grabbing each side of $s head.  $n slowly pulls on $s head, tearing $s forehead apart with a sickening squelch as $e releases one side of $s head to dip $s claw into the gap, slowly swirling it around inside.  As $e pulls $s claw out, $e charges energy into $s claw, and the acid that now covers it.  $n looks at $s claw, as it drips with glowing green acid, the droplets burning into the ground at $s feet.  Suddenly, $e releases $s grip to the ground, and propels $mself to you, slashing quickly at your chest! &W[$t]",
         ch, num_punct( dam ), victim, TO_VICT );
    act( AT_GREEN,
         "$n's feet dig into the ground, bracing $mself to where $e stands.  $n grins maniacally, before grabbing each side of $s head.  $n slowly pulls on $s head, tearing $s forehead apart with a sickening squelch as $e releases one side of $s head to dip $s claw into the gap, slowly swirling it around inside.  As $e pulls $s claw out, $e charges energy into $s claw, and the acid that now covers it.  $n looks at $s claw, as it drips with glowing green acid, the droplets burning into the ground at $s feet.  Suddenly, $e releases $s grip to the ground, and propels $mself to $N, slashing quickly at $S chest! &W[$t]",
         ch, num_punct( dam ), victim, TO_NOTVICT );

    // dam = ki_absorb( victim, ch, dam, gsn_acid_slash );
    learn_from_success( ch, gsn_acid_slash );
    global_retcode = damage( ch, victim, dam, TYPE_HIT );
  }
  else
  {
    act( AT_GREEN, "You missed $N with your acid slash.", ch, NULL, victim, TO_CHAR );
    act( AT_GREEN, "$n misses you with $s acid slash.", ch, NULL, victim, TO_VICT );
    act( AT_GREEN, "$n missed $N with a acid slash.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_acid_slash );
    global_retcode = damage( ch, victim, 0, TYPE_HIT );
  }
  ch->mana -= skill_table[gsn_acid_slash]->min_mana;
  return;
}

/* End File */
