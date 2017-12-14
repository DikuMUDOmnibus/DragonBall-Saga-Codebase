/* Terrian class idea by Kazin
*  Coded by Mazoku
*/
#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

void do_bless( CHAR_DATA * ch, char *argument )
{
  sh_int mod = 0;
  bool strength = FALSE;
  bool speed = FALSE;
  bool intelligence = FALSE;
  bool constitution = FALSE;
  bool luck = FALSE;
  int energy = 0;

  if( IS_NPC( ch ) )
    return;

  if( !IS_NPC( ch ) && ch->exp < 600000000000ULL )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }

  if( argument[0] == '\0' )
  {
    ch_printf( ch, "\n\rSyntax: bless (stat)\n\r" );
    ch_printf( ch, "\n\rValid stats to bless are:\n\r" );
      ch_printf( ch, "  strength" );
      ch_printf( ch, "  speed" );
      ch_printf( ch, "  intelligence" );
      ch_printf( ch, "  constitution" );
      ch_printf( ch, "  luck" );

    ch_printf( ch, "\n\r" );
    return;
  }

  mod = 50;
  energy = 15000;

    strength = TRUE;
    speed = TRUE;
	intelligence = TRUE;
	constitution = TRUE;
	luck = TRUE;

  if( !str_cmp( argument, "strength" ) )
  {
    if( !strength )
    {
      do_bless( ch, "" );
      return;
    }
    if( ch->add_str > 0 )
    {
      ch->perm_str -= ch->add_str;
      ch->add_str = 0;
      ch_printf( ch, "\n\r&RYou feel a little weaker than before.&D\n\r" );
      if( xIS_SET( ( ch )->affected_by, AFF_BLESS ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_BLESS );
      save_char_obj( ch );
      return;
    }
    else
    {
	  if( ch->add_dex > 0 || ch->add_int > 0 || ch->add_con > 0 || ch->add_lck > 0 )
      {
        ch_printf( ch, "You must remove your other blessing.\n\r" );
        return;
      }
      if( ch->mana < energy )
      {
        ch_printf( ch, "You don't have energy energy.\n\r" );
        return;
      }
      if( ch->exp > 600000000000ULL )
      {
        ch->add_str = mod;
        ch->perm_str += mod;
        ch_printf( ch, "\n\r&RKalthizar has blessed you with added strength.\n\r" );
    	 xSET_BIT( ( ch )->affected_by, AFF_BLESS );
      }
      else
      {
        ch_printf( ch, "\n\rYou can't do that.\n\r" );
      }
    }
    if( !is_android_h( ch ) )
      ch->mana -= energy;
    save_char_obj( ch );
    return;
  }

  if( !str_cmp( argument, "speed" ) )
  {
    if( !speed )
    {
      do_bless( ch, "" );
      return;
    }
    if( ch->add_dex > 0 )
    {
      ch->perm_dex -= ch->add_dex;
      ch->add_dex = 0;
      ch_printf( ch, "\n\r&BYou feel a little more sluggish than before.&D\n\r" );
      if( xIS_SET( ( ch )->affected_by, AFF_BLESS ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_BLESS );
      save_char_obj( ch );
      return;
    }
    else
    {
	  if( ch->add_str > 0 || ch->add_int > 0 || ch->add_con > 0 || ch->add_lck > 0 )
      {
        ch_printf( ch, "You must remove your other blessing.\n\r" );
        return;
      }
      if( ch->mana < energy )
      {
        ch_printf( ch, "You don't have energy energy.\n\r" );
        return;
      }
      if( ch->exp > 600000000000ULL )
      {
        ch->add_dex = mod;
        ch->perm_dex += mod;
        ch_printf( ch, "\n\r&BKalthizar has blessed you with added speed.\n\r" );
        xSET_BIT( ( ch )->affected_by, AFF_BLESS );
      }
      else
      {
        ch_printf( ch, "\n\rYou can't do that.\n\r" );
      }
    }
    if( !is_android_h( ch ) )
      ch->mana -= energy;
    save_char_obj( ch );
    return;
  }

    if( !str_cmp( argument, "intelligence" ) )
  {
    if( !intelligence )
    {
      do_bless( ch, "" );
      return;
    }
    if( ch->add_int > 0 )
    {
      ch->perm_int -= ch->add_int;
      ch->add_int = 0;
      ch_printf( ch, "\n\r&CYou feel a little dumber than before.&D\n\r" );
      if( xIS_SET( ( ch )->affected_by, AFF_BLESS ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_BLESS );
      save_char_obj( ch );
      return;
    }
    else
    {
	  if( ch->add_dex > 0 || ch->add_str > 0 || ch->add_con > 0 || ch->add_lck > 0 )
      {
        ch_printf( ch, "You must remove your other blessing.\n\r" );
        return;
      }
      if( ch->mana < energy )
      {
        ch_printf( ch, "You don't have energy energy.\n\r" );
        return;
      }
      if( ch->exp > 600000000000ULL )
      {
        ch->add_int = mod;
        ch->perm_int += mod;
        ch_printf( ch, "\n\r&CKalthizar has blessed you with added intelligence.\n\r" );
        xSET_BIT( ( ch )->affected_by, AFF_BLESS );
      }
      else
      {
        ch_printf( ch, "\n\rYou can't do that.\n\r" );
      }
    }
    if( !is_android_h( ch ) )
      ch->mana -= energy;
    save_char_obj( ch );
    return;
  }
  
  if( !str_cmp( argument, "constitution" ) )
  {
    if( !constitution )
    {
      do_bless( ch, "" );
      return;
    }
    if( ch->add_con > 0 )
    {
      ch->perm_con -= ch->add_con;
      ch->add_con = 0;
      ch_printf( ch, "\n\r&GYou feel a little feebler than before.&D\n\r" );
      if( xIS_SET( ( ch )->affected_by, AFF_BLESS ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_BLESS );
      save_char_obj( ch );
      return;
    }
    else
    {
	  if( ch->add_dex > 0 || ch->add_int > 0 || ch->add_str > 0 || ch->add_lck > 0 )
      {
        ch_printf( ch, "You must remove your other blessing.\n\r" );
        return;
      }
      if( ch->mana < energy )
      {
        ch_printf( ch, "You don't have enough energy.\n\r" );
        return;
      }
      if( ch->exp > 600000000000ULL )
      {
        ch->add_con = mod;
        ch->perm_con += mod;
        ch_printf( ch, "\n\r&GKalthizar has blessed you with added constitution.\n\r" );
        xSET_BIT( ( ch )->affected_by, AFF_BLESS );
      }
      else
      {
        ch_printf( ch, "\n\rYou can't do that.\n\r" );
      }
    }
    if( !is_android_h( ch ) )
      ch->mana -= energy;
    save_char_obj( ch );
    return;
  }
  
  if( !str_cmp( argument, "luck" ) )
  {
    if( !luck )
    {
      do_bless( ch, "" );
      return;
    }
    if( ch->add_lck > 0 )
    {
      ch->perm_lck -= ch->add_lck;
      ch->add_lck = 0;
      ch_printf( ch, "\n\r&YYou feel a little less lucky than before.&D\n\r" );
      if( xIS_SET( ( ch )->affected_by, AFF_BLESS ) )
        xREMOVE_BIT( ( ch )->affected_by, AFF_BLESS );
      save_char_obj( ch );
      return;
    }
    else
    {
	  if( ch->add_dex > 0 || ch->add_int > 0 || ch->add_con > 0 || ch->add_str > 0 )
      {
        ch_printf( ch, "You must remove your other blessing.\n\r" );
        return;
      }
      if( ch->mana < energy )
      {
        ch_printf( ch, "You don't have enough energy.\n\r" );
        return;
      }
      if( ch->exp > 600000000000ULL )
      {
        ch->add_lck = mod;
        ch->perm_lck += mod;
        ch_printf( ch, "\n\r&YKalthizar has blessed you with added luck.\n\r" );
        xSET_BIT( ( ch )->affected_by, AFF_BLESS );
      }
      else
      {
        ch_printf( ch, "\n\rYou can't do that.\n\r" );
      }
    }
    if( !is_android_h( ch ) )
      ch->mana -= energy;
    save_char_obj( ch );
    return;
  }
  
  do_bless( ch, "" );
}

void do_copy_ability( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_INPUT_LENGTH];

	/*return;*/
	do_forget(ch, argument);
	return;
	sprintf( buf, "Your current Ability Slots:\n\r %s %s %s %s %s\n\r", 
		skill_table[ch->pcdata->terrian_learned[0]]->name, skill_table[ch->pcdata->terrian_learned[1]]->name, skill_table[ch->pcdata->terrian_learned[2]]->name, skill_table[ch->pcdata->terrian_learned[3]]->name, skill_table[ch->pcdata->terrian_learned[4]]->name );
	send_to_char( buf, ch );

	send_to_char( "Don't like your Slots? You can 'forget' the abilities!\n\r", ch );
	return;
}

void do_scan_transformation( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	EXT_BV victaff;
	/*return;*/
	if ( ch->mana < skill_table[gsn_scan_transformation]->min_mana )
	{
		send_to_char( "Not enough Energy.\n\r", ch );
		return;
	}
	if ( ch->pcdata->terrian_scan != 0 )
	{
		send_to_char( "You have already Scanned somebody. Die or Powerdown to scan somebody else.\n\r", ch );
		return;
	}
	if ( (victim = get_char_room( ch, argument )) == NULL)
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}
	/*	if ( get_timer(ch, TIMER_PKILLED) > 0 )
	{
	send_to_char( "You must wait.\n\r", ch );
	return;
	}*/

	if ( IS_IMMORTAL(victim) && !IS_IMMORTAL(ch) ) return;
	if ( victim->race == 6 || victim->race == 5 || victim->race == 26 || victim->race == 28 || victim->race == 29 || victim->race == 30 || victim->race == 39)
	{
		send_to_char( "You cannot Scan Transform that race.\n\r", ch );
		return;

	}
	if ( is_saiyan(victim) )
		ch->pcdata->tail = 1;
	xSET_BIT(ch->act, PLR_PK2);
	WAIT_STATE( ch, skill_table[gsn_scan_transformation]->beats );
	if ( can_use_skill(ch, number_percent(), gsn_scan_transformation ) )
	{
		victaff = victim->affected_by;
		ch->pcdata->terrian_affected = ch->affected_by;
		ch->pcdata->terrian_trans_name = str_dup( victim->name );
		sprintf( buf, "You notice that %s is examining your transformation closely..\n\r", ch->name );
		act( AT_BLUE, buf, ch, NULL, victim, TO_VICT );
		sprintf( buf, "You begin to scan %s for a potential transformation..\n\r", victim->name );
		act( AT_BLUE, buf, ch, NULL, victim, TO_CHAR );
		learn_from_success( ch, gsn_scan_transformation );
	}
	else
	{
		switch (number_range(1, 3))
		{
		default:
			sprintf( buf, "You feel as though you've missed out on something..\n\r" );
			act( AT_BLUE, buf, ch, NULL, victim, TO_CHAR );
			sprintf( buf, "%s seems to have messed up his scan somehow.\n\r", ch->name );
			act( AT_BLUE, buf, ch, NULL, victim, TO_ROOM );
			break;
		}
		learn_from_failure( ch, gsn_scan_transformation );
	}

	ch->mana -= skill_table[gsn_scan_transformation]->min_mana;

	return;
}

void do_reflection( CHAR_DATA *ch, char *argument )
{

	if ( ch->mana < skill_table[gsn_reflection]->min_mana )
	{
		send_to_char( "Not enough Energy.\n\r", ch );
		return;
	}

	if ( xIS_SET( (ch)->affected_by, AFF_REFLECTION) )
	{
		xREMOVE_BIT(ch->affected_by, AFF_REFLECTION);
		act( AT_BLUE,
			"You no-longer look like a Faint Blue.", ch, NULL, NULL, TO_CHAR );
		act( AT_BLUE,
			"$n no-longer looks like a Faint Blue.", ch, NULL, NULL, TO_NOTVICT );
		return;
	}
	WAIT_STATE( ch, skill_table[gsn_reflection]->beats );
	if ( can_use_skill(ch, number_percent(), gsn_reflection ) )
	{

		act( AT_BLUE, 
			"You look like a Faint Blue.", ch, NULL, NULL, TO_CHAR );
		act( AT_BLUE, 
			"$n looks like a Faint Blue.", ch, NULL, NULL, TO_NOTVICT );

		xSET_BIT( (ch)->affected_by, AFF_REFLECTION );
		learn_from_success( ch, gsn_reflection );
	}
	else
	{
		switch (number_range(1, 3))
		{
		default:
			act( AT_BLUE, "You flash Blue, but nothing much happens.", ch, NULL, NULL, TO_CHAR );
			act( AT_BLUE, "$n's flahes Blue.", ch, NULL, NULL, TO_NOTVICT );
			break;
		}
		learn_from_failure( ch, gsn_reflection );
	}

	ch->mana -= skill_table[gsn_reflection]->min_mana;
	return;
	/*}
	return;*/
}

void do_forget( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_INPUT_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char *s1;
	char *s2;
	char *s3;
	char *s4;
	char *s5;

	argument = one_argument(argument, arg);

	s1 = skill_table[ch->pcdata->terrian_learned[0]]->name;
	s2 = skill_table[ch->pcdata->terrian_learned[1]]->name;
	s3 = skill_table[ch->pcdata->terrian_learned[2]]->name;
	s4 = skill_table[ch->pcdata->terrian_learned[3]]->name;
	s5 = skill_table[ch->pcdata->terrian_learned[4]]->name;

	if ( arg[0] == '\0' )
	{	/*
		sprintf( buf, "Your current Ability Slots:\n\r %s\n\r %s\n\r %s\n\r %s\n\r %s\n\r", skill_table[ch->pcdata->terrian_learned[0]]->name, skill_table[ch->pcdata->terrian_learned[1]]->name, skill_table[ch->pcdata->terrian_learned[1]]->name, skill_table[ch->pcdata->terrian_learned[1]]->name, skill_table[ch->pcdata->terrian_learned[1]]->name );
		send_to_char( buf, ch );
		*/
		sprintf( buf, "Your current Ability Slots:\n\r %s %s %s %s %s\n\r", s1,s2,s3,s4,s5 );
		send_to_char( buf, ch );
		/*
		sprintf( buf, "[(*** Mazoku Debug ***)] -- User %s currently has %s %s %s %s %s as his Ability Slots\n\r",ch->name, s1,s2,s3,s4,s5 );
		log_string( buf );
		*/
		return;
	}
	if ( is_number(arg) )
	{
		if ( atoi(arg) < 0 || atoi(arg) > 5 )
		{
			sprintf( buf, "Your current Ability Slots:\n\r %s %s %s %s %s\n\r", skill_table[ch->pcdata->terrian_learned[0]]->name, skill_table[ch->pcdata->terrian_learned[1]]->name, skill_table[ch->pcdata->terrian_learned[1]]->name, skill_table[ch->pcdata->terrian_learned[1]]->name, skill_table[ch->pcdata->terrian_learned[1]]->name );
			send_to_char( buf, ch );
			return;
		}
		sprintf( buf, "You have forgotten %s.\n\r", skill_table[ch->pcdata->terrian_learned[atoi(arg)]]->name );
		send_to_char( buf, ch );
		ch->pcdata->learned[ch->pcdata->terrian_learned[atoi(arg)]] = 0;
		ch->pcdata->terrian_learned[atoi(arg)] = 0;
	}
	return;
}

/************************
*** For Copy Ability ***
************************/
void copy_check(CHAR_DATA *ch, CHAR_DATA *victim, int ki_sn)
{
	char buf[MAX_STRING_LENGTH];

	if(IS_NPC(ch))
		return;

	if ( can_use_skill(ch, number_percent(), gsn_copy_ability ) )
	{
		if ( dice(1,75) > 1 && !IS_NPC(victim) )
		{
			/*
			act( AT_HIT, "You block $N's attack with your hand.", ch, NULL, victim, TO_CHAR );
			act( AT_HIT, "$n holds his hand up and $N's attack is shot into his body!", ch, NULL, victim, TO_ROOM );
			*/
			if ( ch->pcdata->terrian_learned[0] != 0
				&& ch->pcdata->terrian_learned[1] != 0
				&& ch->pcdata->terrian_learned[2] != 0
				&& ch->pcdata->terrian_learned[3] != 0
				&& ch->pcdata->terrian_learned[4] != 0 )
			{
				send_to_char( "You must 'forget' a skill before you can Steal another Skill!\n\r", ch );
				return;
			}
			if ( ch->pcdata->terrian_learned[0] == ki_sn
				|| ch->pcdata->terrian_learned[1] == ki_sn
				|| ch->pcdata->terrian_learned[2] == ki_sn
				|| ch->pcdata->terrian_learned[3] == ki_sn
				|| ch->pcdata->terrian_learned[4] == ki_sn
				|| ch->pcdata->learned[ki_sn] > 0)
			{
				sprintf( buf, "[(*** Mazoku Debug ***)] -- User %s has just tried to gain Skill %d, User already had. Canceling.\n\r", ch->name, ki_sn );
				log_string( buf );
				return;
			}
			if ( ch->pcdata->terrian_learned[0] == 0 )
			{
				sprintf( buf, "You concentrate hard on the attack, and feel a rush of enlightenment. You can use %s!", skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_CHAR );
				sprintf( buf, "With a twinge of disappointment, you realize that %s has copied your %s..",ch->name, skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_VICT );
				sprintf( buf, "You notice %s's face light up. They must have learned how to use %s's last %s!",ch->name,victim->name,skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_NOTVICT );
				ch->pcdata->learned[ki_sn] = 10;
				ch->pcdata->terrian_learned[0] = ki_sn;
				save_char_obj( ch );
			}
			else if ( ch->pcdata->terrian_learned[1] == 0 && ch->pcdata->terrian_learned[0] != 0)
			{
				sprintf( buf, "You concentrate hard on the attack, and feel a rush of enlightenment. You can use %s!", skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_CHAR );
				sprintf( buf, "With a twinge of disappointment, you realize that %s has copied your %s..",ch->name, skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_VICT );
				sprintf( buf, "You notice %s's face light up. They must have learned how to use %s last %s!",ch->name,victim->name,skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_NOTVICT );
				ch->pcdata->learned[ki_sn] = 10;
				ch->pcdata->terrian_learned[1] = ki_sn;
				save_char_obj( ch );
			}
			else if ( ch->pcdata->terrian_learned[2] == 0 && ch->pcdata->terrian_learned[1] != 0 && ch->pcdata->terrian_learned[0] != 0)
			{
				sprintf( buf, "You concentrate hard on the attack, and feel a rush of enlightenment. You can use %s!", skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_CHAR );
				sprintf( buf, "With a twinge of disappointment, you realize that %s has copied your %s..",ch->name, skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_VICT );
				sprintf( buf, "You notice %s's face light up. They must have learned how to use %s last %s!",ch->name,victim->name,skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_NOTVICT );
				ch->pcdata->learned[ki_sn] = 10;
				ch->pcdata->terrian_learned[2] = ki_sn;
				save_char_obj( ch );
			}
			else if ( ch->pcdata->terrian_learned[3] == 0 && ch->pcdata->terrian_learned[2] != 0 && ch->pcdata->terrian_learned[1] != 0 && ch->pcdata->terrian_learned[0] != 0)
			{
				sprintf( buf, "You concentrate hard on the attack, and feel a rush of enlightenment. You can use %s!", skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_CHAR );
				sprintf( buf, "With a twinge of disappointment, you realize that %s has copied your %s..",ch->name, skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_VICT );
				sprintf( buf, "You notice %s's face light up. They must have learned how to use %s last %s!",ch->name,victim->name,skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_NOTVICT );
				ch->pcdata->learned[ki_sn] = 10;
				ch->pcdata->terrian_learned[3] = ki_sn;
				save_char_obj( ch );
			}
			else if ( ch->pcdata->terrian_learned[4] == 0 && ch->pcdata->terrian_learned[3] != 0 && ch->pcdata->terrian_learned[2] != 0 && ch->pcdata->terrian_learned[1] != 0 && ch->pcdata->terrian_learned[0] != 0)
			{
				sprintf( buf, "You concentrate hard on the attack, and feel a rush of enlightenment. You can use %s!", skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_CHAR );
				sprintf( buf, "With a twinge of disappointment, you realize that %s has copied your %s..",ch->name, skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_VICT );
				sprintf( buf, "You notice %s's face light up. They must have learned how to use %s last %s!",ch->name,victim->name,skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_NOTVICT );
				ch->pcdata->learned[ki_sn] = 10;
				ch->pcdata->terrian_learned[4] = ki_sn;
				save_char_obj( ch );
			}
			learn_from_success( ch, gsn_copy_ability );
		}
		else if ( !can_use_skill(ch, number_percent(), gsn_copy_ability ) )
		{
			switch (number_range(1, 3))
			{
			default:

				sprintf( buf, "Though you saw the %s, you feel the knowledge slipping away!", skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, NULL, TO_CHAR );
				sprintf( buf, "Alas, %s could not copy your ability. Good!", ch->name );
				act( AT_HIT, buf, ch, NULL, victim, TO_VICT );
				sprintf( buf, "%s concentrates hard on copying %s's %s, but seems to have forgotten it already!", ch->name, victim->name, skill_table[ki_sn]->name );
				act( AT_HIT, buf, ch, NULL, NULL, TO_NOTVICT );
				learn_from_failure( ch, gsn_copy_ability );
				break;
			}
		}
	}
}

/*
bool transform_shit( CHAR_DATA *ch )
{
if (xIS_SET((ch)->affected_by, AFF_KAIOKEN))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_SSJ))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_SSJ2))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_SSJ3))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_SSJ4))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_USSJ))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_USSJ2))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_LSSJ))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_MECHAFIGHTER))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_INDEPENDENT_WILL))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_MUTATE))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_ICER2))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_ICER3))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_ICER4))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_ICER5))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_SNAMEK))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_OOZARU))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_GOLDEN_OOZARU))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_HEART))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_HYPER))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_EXTREME))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_ELECTRICSHIELD))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_SEMIPERFECT))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_PERFECT))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_ULTRAPERFECT))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_GROWTH))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_GIANT))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_SPLIT_FORM))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_TRI_FORM))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_MULTI_FORM))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_MYSTIC))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_SUPERANDROID))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_MAKEOSTAR))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_EVILBOOST))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_EVILSURGE))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_EVILOVERLOAD))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_BIOJR))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_SFLAME1))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_SFLAME2))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_SFLAME3))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_UFLAME))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_DEKU))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_GORON))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_ZORA))
return TRUE;
if (xIS_SET((ch)->affected_by, AFF_FDEITY))
return TRUE;
return FALSE;
}
*/
