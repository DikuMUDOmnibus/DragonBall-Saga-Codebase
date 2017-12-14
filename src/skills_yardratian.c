#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

/* Place holder */

void do_shakkahou( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_shakkahou]->skill_level[ch->class] )
  {
    send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_shakkahou]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_shakkahou]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_shakkahou]->focus;

  WAIT_STATE( ch, skill_table[gsn_shakkahou]->beats );

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_shakkahou]->min_mana;

  if( can_use_skill( ch, number_percent(  ), gsn_shakkahou ) )
  {
    act( AT_RED, "Closing your eyes you begin to chant 'Open, oh, flames of hell and begin", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "to spin around my body.  Enhance my power; help me dispose of my foe!  Art", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "of the demon!  Skill 31!  Errupt!  ' You toss your hand out and the aura", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "around your body explodes into that of a red flame the ground begins to", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "rumble with energy.  Suddenly a spiraling red ball forms in your palm.", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "Tossing your arm out the ball begins to glow.  Your eyes begin to nerrow", ch, NULL, victim, TO_CHAR );
    act( AT_RED, "upon your enemy.  The blast ejects as a red beam crashing directly into $N.", ch, NULL, victim, TO_CHAR );

    act( AT_RED, "$n closes $s eyes and begins to chant 'Open, oh, flames of hell and begin", ch, NULL, victim, TO_NOTVICT );
    act( AT_RED, "to spin around my body.  Enhance my power; help me dispose of my foe!  Art", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_RED, "of the demon!  Skill 31!  Errupt!' $n toss $s hand out and the aura", ch, NULL, victim, TO_NOTVICT );
    act( AT_RED, "around their body explodes into that of a red flame the ground begins to", ch, NULL, victim, TO_NOTVICT );
    act( AT_RED, "rumble with energy.  Suddenly a spiraling red ball forms in $s palm. ", ch, NULL, victim, TO_NOTVICT );
    act( AT_RED, "Tossing their arm out the ball begins to glow.  $n's eyes begin to nerrow", ch, NULL, victim, TO_NOTVICT );
    act( AT_RED, "upon their enemy.  The blast ejects as a red beam crashing directly into $N.", ch, NULL, victim,
         TO_NOTVICT );

    act( AT_RED, "$n closes $s eyes and begins to chant 'Open, oh, flames of hell and begin", ch, NULL, victim, TO_VICT );
    act( AT_RED, "to spin around my body.  Enhance my power; help me dispose of my foe!  Art", ch, NULL, victim, TO_VICT );
    act( AT_RED, "of the demon!  Skill 31!  Errupt!' $n toss $s hand out and the aura", ch, NULL, victim, TO_VICT );
    act( AT_RED, "around their body explodes into that of a red flame the ground begins to", ch, NULL, victim, TO_VICT );
    act( AT_RED, "rumble with energy.  Suddenly a spiraling red ball forms in $s palm. ", ch, NULL, victim, TO_VICT );
    act( AT_RED, "Tossing their arm out the ball begins to glow.  $n's eyes begin to nerrow", ch, NULL, victim, TO_VICT );
    act( AT_RED, "upon you.  The blast ejects as a red beam crashing directly into you.", ch, NULL, victim, TO_VICT );

    learn_from_success( ch, gsn_shakkahou );
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 45, 75 ) ), gsn_shakkahou );
  }
  else
  {
    act( AT_RED, "Chanting franticly you mess up the wording, causing your tounge to slip and your beam to miss your enemy.",
         ch, NULL, victim, TO_CHAR );
    act( AT_RED, "Chanting franticly $n mess up the wording, causing their tounge to slip and the red beam to miss you.", ch,
         NULL, victim, TO_VICT );
    act( AT_RED, "Chanting franticly $n mess up the wording, causing their tounge to slip and the red beam to miss $N.", ch,
         NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_shakkahou );
    global_retcode = damage( ch, victim, 0, gsn_shakkahou );
  }
  return;
}
void do_raizou( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_raizou]->skill_level[ch->class] )
  {
    send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_raizou]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_raizou]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_raizou]->focus;

  WAIT_STATE( ch, skill_table[gsn_raizou]->beats );

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_raizou]->min_mana;

  if( can_use_skill( ch, number_percent(  ), gsn_raizou ) )
  {
    act( AT_YELLOW, "Closing your eyes you and begins to chant ' Sky, earth, planet, lend me", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "the energy to dispose of my foe.  Surround me with your most powerful", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "element, lightning! Art of the demon! Skill 19! Roar!' You toss your", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "hands into the air as clouds surround the area and a storm begins to brew. ", ch, NULL, victim,
         TO_CHAR );
    act( AT_YELLOW, "Lightning crashes and rain begins to pour. Several flashes of thunder", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "crash into the ground scaring it, thunder roars and rocks the air around", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "you and $N. One very large strike crashes into you forcing $N to jump", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "back. After you reappears you have a ball of lightning in your palm,", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "crackling with the element of lightning. You grin wildly as you thrust", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "your hand, with the lightning, into the ground. The ground breaks in two", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "and you screams 'Raizou Ikazuchi wo utte!!!' from the ground the strike of", ch, NULL, victim,
         TO_CHAR );
    act( AT_YELLOW, "lightning flys back into the sky through $N's body causing them to scream", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "in pain.", ch, NULL, victim, TO_CHAR );

    act( AT_YELLOW, "$n closes thier eyes and begins to chant 'Sky, earth, planet, lend me", ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW, "the energy to dispose of my foe. Surround me with your most powerful", ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW, "element, lightning!  Art of the demon!  Skill 19!  Roar!' $n toss their", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "hands into the air as clouds surround the area and a storm begins to brew. ", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "Lightning crashes and rain begins to pour. Several flashes of thunder", ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW, "crash into the ground scaring it, thunder roars and rocks the air around", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "the two fighters one very large strike crashes into $n forcing $N to jump", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "back. After $n reappears they have a ball of lightning in their palm,", ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW, "crackling with the element of lightning. $n grins wildly as they thrust", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "their hand, with the lightning, into the ground. The ground breaks in two", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "and $n screams 'Raizou Ikazuchi wo utte!!!' from the ground the strike of", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "lightning flys back into the sky through $N's body ccausing them to scream", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "in pain.", ch, NULL, victim, TO_NOTVICT );

    act( AT_YELLOW, "$n closes their eyes and begins to chant 'Sky, earth, planet, lend me", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "the energy to dispose of my foe. Surround me with your most powerful", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "element, lightning! Art of the demon! Skill 19!  Roar!' $n toss $s", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "hands into the air as clouds surround the area and a storm begins to brew. ", ch, NULL, victim,
         TO_VICT );
    act( AT_YELLOW, "Lightning crashes and rain begins to pour. Several flashes of thunder", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "crash into the ground scaring it, thunder roars and rocks the air around", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "$n and yourself. One very large strike crashes into you forcing you to", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "jump back.  After $n reappears $n has a ball of lightning in $s palm,", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "crackling with the element of lightning. $n grins wildly as they thrust", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "their hand, with the lightning, into the ground.  The ground breaks in two", ch, NULL, victim,
         TO_VICT );
    act( AT_YELLOW, "and $n screams 'Raizou Ikazuchi wo utte!!!' from the ground the strike of", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "lightning flys back into the sky through your body causing you to scream", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "in pain.", ch, NULL, victim, TO_VICT );
    learn_from_success( ch, gsn_raizou );
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 8, 12 ) ), gsn_raizou );
  }
  else
  {
    act( AT_RED,
         "Chanting franticly you mess up the wording, causing your tounge to slip and your storm to miss your enemy.", ch,
         NULL, victim, TO_CHAR );
    act( AT_RED, "Chanting franticly $n mess up the wording, causing their tounge to slip and the storm to miss you.", ch,
         NULL, victim, TO_VICT );
    act( AT_RED, "Chanting franticly $n mess up the wording, causing their tounge to slip and the storm to miss $N.", ch,
         NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_raizou );
    global_retcode = damage( ch, victim, 0, gsn_raizou );
  }
  return;
}
void do_raikyuu( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_raikyuu]->skill_level[ch->class] )
  {
    send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_raikyuu]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_raikyuu]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_raikyuu]->focus;

  WAIT_STATE( ch, skill_table[gsn_raikyuu]->beats );

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_raikyuu]->min_mana;

  if( can_use_skill( ch, number_percent(  ), gsn_raikyuu ) )
  {
    act( AT_YELLOW, "Tossing your palm out a yellow ball of energy forms into the palm of", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "your hand and then errupts with crakling bolts of electricity.  The energy", ch, NULL, victim,
         TO_CHAR );
    act( AT_YELLOW, "bolts form and pop at the front of the energy ball starting to spiral", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "slowly then back into your palm the same way it came.  Then with the force", ch, NULL, victim,
         TO_CHAR );
    act( AT_YELLOW, "of a rocket it is suddenly found in $N's chest spinning an crackling, but", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "not harming yet.  You say 'Silent chant.  Art of the demon.  Skill 45. ", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "Lightning ball!  EXPLODE!  ' The ball explodes violently surrounding $N in", ch, NULL, victim,
         TO_CHAR );
    act( AT_YELLOW, "a cloud of dust.", ch, NULL, victim, TO_CHAR );

    act( AT_YELLOW, "$n tosses their palm out and a yellow ball of energy forms into the palm of", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "their hand and then errupts with crakling bolts of electricity.  The energy", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "bolts form and pop at the front of the energy ball starting to spiral", ch, NULL, victim, TO_NOTVICT );
    act( AT_YELLOW, "slowly then back into $s palm the same way it came.  Then with the force", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "of a rocket it is suddenly found in $N's chest spinning and crackling, but", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "not harming yet.  $n says 'Silent chant.  Art of the demon.  Skill 45. ", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "Lightning ball!  EXPLODE!  ' The ball explodes violently surrounding $N in", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "a cloud of dust.", ch, NULL, victim, TO_NOTVICT );

    act( AT_YELLOW, "$n tosses their palm out and a yellow ball of energy forms into the palm of", ch, NULL, victim,
         TO_VICT );
    act( AT_YELLOW, "their hand and then errupts with crakling bolts of electricity.  The energy", ch, NULL, victim,
         TO_VICT );
    act( AT_YELLOW, "bolts form and pop at the front of the energy ball starting to spiral", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "slowly then back into $s palm the same way it came.  Then with the force", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "of a rocket it is suddenly found in your chest spinning and crackling, but", ch, NULL, victim,
         TO_VICT );
    act( AT_YELLOW, "not harming yet.  $n says 'Silent chant.  Art of the demon.  Skill 45. ", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "Lightning ball!  EXPLODE!  ' The ball explodes violently surrounding you in", ch, NULL, victim,
         TO_VICT );
    act( AT_YELLOW, "a cloud of dust.", ch, NULL, victim, TO_VICT );

    learn_from_success( ch, gsn_raikyuu );
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 8, 12 ) ), gsn_raikyuu );
  }
  else
  {
    act( AT_BLUE, "You toss your hand out but you are blocked by $N.", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "You notice $n's attack and quickly kick their arm.", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "$N notices $n's attack and kicks their arm into the air.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_raikyuu );
    global_retcode = damage( ch, victim, 0, gsn_raikyuu );
  }
  return;
}
void do_riku( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_riku]->skill_level[ch->class] )
  {
    send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_riku]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_riku]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_riku]->focus;

  WAIT_STATE( ch, skill_table[gsn_riku]->beats );

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_riku]->min_mana;

  if( can_use_skill( ch, number_percent(  ), gsn_riku ) )
  {
    act( AT_YELLOW, "Pointing your finger toward $N the tip flashes quickly then fades as if", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "nothing happened.  Suddenly lances of pure ki slams into $N pursing them", ch, NULL, victim, TO_CHAR );
    act( AT_YELLOW, "quickly then shatters away as if nothing happened.", ch, NULL, victim, TO_CHAR );

    act( AT_YELLOW, "$n points their finger toward $N the tip flashes quickly then fades as if", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "nothing happened.  Suddenly lances of pure ki slams into $N pursing them", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_YELLOW, "quickly then shatters away as if nothing happened.", ch, NULL, victim, TO_NOTVICT );

    act( AT_YELLOW, "$n points their finger toward you the tip flashes quickly then fades as if", ch, NULL, victim,
         TO_VICT );
    act( AT_YELLOW, "nothing happened.  Suddenly lances of pure ki slams into you pursing them", ch, NULL, victim, TO_VICT );
    act( AT_YELLOW, "quickly then shatters away as if nothing happened.", ch, NULL, victim, TO_VICT );

    learn_from_success( ch, gsn_riku );
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 8, 12 ) ), gsn_riku );
  }
  else
  {
    act( AT_BLUE, "&cYou point at $M accusingly.", ch, NULL, victim, TO_CHAR );
    act( AT_BLUE, "&c$n points at you accusingly.", ch, NULL, victim, TO_VICT );
    act( AT_BLUE, "&c$n points at $N accusingly.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_riku );
    global_retcode = damage( ch, victim, 0, gsn_riku );
  }
  return;
}
