#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

extern void transStatApply args( ( CHAR_DATA * ch, int strMod, int spdMod, int intMod, int conMod ) );
extern void transStatRemove args( ( CHAR_DATA * ch ) );

void do_devil_flash( CHAR_DATA * ch, char *argument )
{
  CHAR_DATA *victim;

  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_devil_flash]->skill_level[ch->class] )
  {
    send_to_char( "You better leave the martial arts to fighters.\n\r", ch );
    return;
  }

  if( ( victim = who_fighting( ch ) ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

  if( !IS_NPC( ch ) && ch->mana < skill_table[gsn_devil_flash]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->focus < skill_table[gsn_devil_flash]->focus )
  {
    send_to_char( "You need to focus more.\n\r", ch );
    return;
  }
  else
    ch->focus -= skill_table[gsn_devil_flash]->focus;

  WAIT_STATE( ch, skill_table[gsn_devil_flash]->beats );

  if( !is_android_h( ch ) )
    ch->mana -= skill_table[gsn_devil_flash]->min_mana;

  if( can_use_skill( ch, number_percent(  ), gsn_devil_flash ) )
  {
    act( AT_ORANGE, "You pull both arms to your sides, stretching them wide as you begin to", ch, NULL, victim, TO_CHAR );
    act( AT_ORANGE, "gather energy. An orange glow surrounds you as the energy overflows into", ch, NULL, victim, TO_CHAR );
    act( AT_ORANGE, "the outside air. As the spheres reach their peak of brightness, you slam", ch, NULL, victim, TO_CHAR );
    act( AT_ORANGE, "your wrists together, howling loudly. 'DEVIL FLASH!'  Erupts from your lips", ch, NULL, victim,
         TO_CHAR );
    act( AT_ORANGE, "as a brilliant orange beam bursts forth from your hands, surrounded by two", ch, NULL, victim,
         TO_CHAR );
    act( AT_ORANGE, "smaller yellow beams, spiraling around it. The beam collides with $N,", ch, NULL, victim, TO_CHAR );
    act( AT_ORANGE, "rending flesh like paper as it ravages their body. Suddenly, the", ch, NULL, victim, TO_CHAR );
    act( AT_ORANGE, "beam shrinks until it vanishes, leaving no trace aside from the marks", ch, NULL, victim, TO_CHAR );
    act( AT_ORANGE, "seared into $N's flesh.", ch, NULL, victim, TO_CHAR );

    act( AT_ORANGE, "$n pulls both arms to their side, stretching them wide as they begin to", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_ORANGE, "gather energy. An orange glow surrounds $n as the energy overflows into", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_ORANGE, "the outside air. As the spheres reach their peak of brightness, $n slams", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_ORANGE, "their wrists together, howling loudly. 'DEVIL FLASH!'  Erupts from $n's lips", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_ORANGE, "as a brilliant orange beam bursts forth from their hands, surrounded by two", ch, NULL, victim,
         TO_NOTVICT );
    act( AT_ORANGE, "smaller yellow beams, spiraling around it. The beam collides with $N,", ch, NULL, victim, TO_NOTVICT );
    act( AT_ORANGE, "rending flesh like paper as it ravages their body. Suddenly, the", ch, NULL, victim, TO_NOTVICT );
    act( AT_ORANGE, "beam shrinks until it vanishes, leaving no trace aside from the marks", ch, NULL, victim, TO_NOTVICT );
    act( AT_ORANGE, "seared into $N's flesh.", ch, NULL, victim, TO_NOTVICT );

    act( AT_ORANGE, "$n pulls both arms to their side, stretching them wide as they begin to", ch, NULL, victim, TO_VICT );
    act( AT_ORANGE, "gather energy. An orange glow surrounds $n as the energy overflows into", ch, NULL, victim, TO_VICT );
    act( AT_ORANGE, "the outside air. As the spheres reach their peak of brightness, $n slams", ch, NULL, victim, TO_VICT );
    act( AT_ORANGE, "their wrists together, howling loudly. 'DEVIL FLASH!'  Erupts from $n's lips", ch, NULL, victim,
         TO_VICT );
    act( AT_ORANGE, "as a brilliant orange beam bursts forth from their hands, surrounded by two", ch, NULL, victim,
         TO_VICT );
    act( AT_ORANGE, "smaller yellow beams, spiraling around it. The beam collides with you,", ch, NULL, victim, TO_VICT );
    act( AT_ORANGE, "rending flesh like paper as it ravages your body. Suddenly, the", ch, NULL, victim, TO_VICT );
    act( AT_ORANGE, "beam shrinks until it vanishes, leaving no trace aside from the marks", ch, NULL, victim, TO_VICT );
    act( AT_ORANGE, "seared into your flesh.", ch, NULL, victim, TO_VICT );

    learn_from_success( ch, gsn_devil_flash );
    global_retcode = damage( ch, victim, ( get_attmod( ch, victim ) * number_range( 75, 90 ) ), gsn_devil_flash );
  }
  else
  {
    act( AT_SKILL, "You fire your deadly beam, but you miss $N.", ch, NULL, victim, TO_CHAR );
    act( AT_SKILL, "$n fires $s deadly beam, but $e misses you.", ch, NULL, victim, TO_VICT );
    act( AT_SKILL, "$n fires $s deadly beam, but $e misses $N.", ch, NULL, victim, TO_NOTVICT );
    learn_from_failure( ch, gsn_devil_flash );
    global_retcode = damage( ch, victim, 0, gsn_devil_flash );
  }
  return;
}

/* Added for people to automaticly join clans using rpp  -Karn 06.21.06*/
void do_joinclan( CHAR_DATA * ch, char *argument )
{
  CLAN_DATA *clan;

  if( IS_NPC( ch ) )
  {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
  }

  if( !argument || argument[0] == '\0' )
  {
    send_to_char( "Syntax: joinclan <clan short>\n\r", ch );
    send_to_char( "Avalible clans can be found with the 'clans' command.\n\r", ch );
    return;
  }
  if( ch->pcdata->clan != NULL )
  {
    send_to_char( "You are already in a clan.\n\r", ch );
    return;
  }
  if( ch->pcdata->quest_curr < 5 )
  {
    send_to_char( "You don't have enough RPP.\n\r", ch );
    return;
  }
  clan = get_clan( argument );
  if( !clan )
  {
    send_to_char( "No such clan.\n\r", ch );
    return;
  }
  STRFREE( ch->pcdata->clan_name );
  ch->pcdata->clan_name = QUICKLINK( clan->name );
  ch->pcdata->clan = clan;
  if( ch->exp != 100 )
    ch->pcdata->quest_curr -= 5;
  ch->pcdata->clanRank = 7;
  ch->pcdata->clanZeniDonated = 0;
  ch->pcdata->clanZeniClanTax = 0;
  ch->pcdata->clanItemsDonated = 0;
  if( !IS_IMMORTAL( ch ) )
  {
    if( ch->sex == SEX_FEMALE )
      clan->fRank7Count++;
    else
      clan->mRank7Count++;
  }
  return;
}
