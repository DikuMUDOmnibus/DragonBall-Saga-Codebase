#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

void
do_energy_ring(CHAR_DATA * ch, char *argument)
{
  AFFECT_DATA af;
  CHAR_DATA *vch, *vch_next;

  if (IS_NPC(ch) && is_split(ch)) {
    return;
  }

  if (!IS_NPC(ch)
      && ch->exp < skill_table[gsn_energy_ring]->skill_level[ch->class]) {
    send_to_char("You can't do that.\n\r", ch);
    return;
  }
  if (xIS_SET(ch->in_room->room_flags, ROOM_ARENA)
      || xIS_SET(ch->in_room->room_flags, ROOM_SAFE)) {
    send_to_char("There's no need to use that here.\n\r", ch);
    return;
  }
  if (who_fighting(ch) == NULL) {
    send_to_char("You aren't fighting anyone.\n\r", ch);
    return;
  }
  if (ch->mana < skill_table[gsn_energy_ring]->min_mana) {
    send_to_char("You don't have enough energy.\n\r", ch);
    return;
  }
  if (ch->focus < skill_table[gsn_energy_ring]->focus) {
    send_to_char("You need to focus more.\n\r", ch);
    return;
  } else
    ch->focus -= skill_table[gsn_energy_ring]->focus;

  WAIT_STATE(ch, skill_table[gsn_energy_ring]->beats);

  if (can_use_skill(ch, number_percent(), gsn_energy_ring)) {
    act(AT_YELLOW,
        "You hold both hands back on both sides of your body, charging your",
        ch, NULL, NULL, TO_CHAR);
    act(AT_YELLOW,
        "energy into the palms of your hands, then you toss the ball's of energy",
        ch, NULL, NULL, TO_CHAR);
    act(AT_YELLOW,
        "quickly towards your enemy. Suddenly, the ball's reform as they speed",
        ch, NULL, NULL, TO_CHAR);
    act(AT_YELLOW,
        "towards them and lengthen into bar's of light, which slam into",
        ch, NULL, NULL, TO_CHAR);
    act(AT_YELLOW,
        "them and wraps around $s, creating an ring of energy.  You do",
        ch, NULL, NULL, TO_CHAR);
    act(AT_YELLOW,
        "this once more quickly, causing four ring's of energy around them,",
        ch, NULL, NULL, TO_CHAR);
    act(AT_YELLOW, "making them unable to move momentarily.", ch, NULL,
        NULL, TO_CHAR);

    act(AT_YELLOW,
        "$n hold both hands back on both sides of their body, charging $s's",
        ch, NULL, NULL, TO_NOTVICT);
    act(AT_YELLOW,
        "energy into the palms of their hands, then $n toss the balls of energy",
        ch, NULL, NULL, TO_NOTVICT);
    act(AT_YELLOW,
        "quickly towards their enemy. Suddenly, the balls reform as they speed",
        ch, NULL, NULL, TO_NOTVICT);
    act(AT_YELLOW,
        "towards $s's enemy and lengthen into bar's of light, which slam into",
        ch, NULL, NULL, TO_NOTVICT);
    act(AT_YELLOW,
        "them and wraps around $s, creating a ring of energy.  They do",
        ch, NULL, NULL, TO_NOTVICT);
    act(AT_YELLOW,
        "this once more quickly, causing four ring's of energy around their foe,",
        ch, NULL, NULL, TO_NOTVICT);
    act(AT_YELLOW, "making them unable to move momentarily.", ch, NULL,
        NULL, TO_NOTVICT);

    //act( AT_YELLOW, "You get captured in $n's energy rings.", ch, NULL, NULL, TO_VICT );

    learn_from_success(ch, gsn_energy_ring);
    for (vch = ch->in_room->first_person; vch; vch = vch_next) {

      vch_next = vch->next_in_room;
      if (!IS_AWAKE(vch))
        continue;

      /*
       * if( number_range( 1, 100 ) < ( get_curr_int( vch ) / 10 ) )
       * {
       * send_to_char( "&zYou dodge all the energy rings tossed at you.\n\r", vch );
       * continue;
       * }
       */

      if (ch != vch && who_fighting(vch) != NULL) {
        af.type = gsn_energy_ring;
        af.duration = 3;
        af.location = APPLY_HITROLL;
        af.modifier = -100;
        af.bitvector = meb(AFF_CHARM);
        affect_to_char(vch, &af);
      }

    }

  } else {
    act(AT_WHITE,
        "You begin to charge the energy but it fizzles out as you toss it.",
        ch, NULL, NULL, TO_CHAR);
    act(AT_WHITE, "$n misses with their energy rings.", ch, NULL, NULL,
        TO_NOTVICT);
    learn_from_failure(ch, gsn_energy_ring);
  }
  ch->mana -= skill_table[gsn_energy_ring]->min_mana;
  return;
}

void
do_teachmystic(CHAR_DATA * ch, char *argument)
{
  /*
   * char arg[MAX_STRING_LENGTH];
   */
  CHAR_DATA *vch;

  if (IS_NPC(ch))
    return;

  if (!IS_KAIO(ch)) {
    ch_printf(ch, "Huh?!?\n\r");
    return;
  }
  if (ch->rank < 4) {
    ch_printf(ch, "Huh?!?\n\r");
    return;
  }

  if (wearing_chip(ch)) {
    ch_printf(ch, "You can't while you have a chip installed.\n\r");
    return;
  }

  if (ch->pcdata->learned[gsn_mystic] < 95) {
    ch_printf(ch,
              "You can't teach mystic to others until you master it yourself.\n\r");
    return;
  }
  if (ch->position != POS_STANDING) {
    ch_printf(ch, "You have to be standing.\n\r");
    return;
  }
  if (!xIS_SET(ch->in_room->room_flags, ROOM_SAFE)) {
    ch_printf(ch, "You have to be in a SAFE room.\n\r");
    return;
  }
  if ((vch = get_char_room(ch, argument)) == NULL) {
    ch_printf(ch, "There's nobody here by that name.\n\r");
    return;
  }
  if (IS_NPC(vch)) {
    ch_printf(ch, "Not on NPC's.\n\r");
    return;
  }
  if (vch->exp < 100000000000ULL) {
    ch_printf(ch, "They have to be over 100bil to learn mystic.\n\r");
    return;
  }
/* Karma only addressed Androids. I fixed. -Karn 02.08.05 */
  if (IS_DEMON(vch) || is_genie(vch) || is_android(vch)
      || is_superandroid(vch) || IS_ICER(vch) || is_bio(vch)
      || is_fierian(vch) || is_hylian(vch) || is_reploid(vch)
      || is_reploid(vch)) {
    ch_printf(ch, "They cannot be taught mystic.\n\r");
    return;
  }
  if (vch->position != POS_STANDING) {
    ch_printf(ch, "They have to be standing.\n\r");
    return;
  }
  if (IS_SET(vch->pcdata->flags, PCFLAG_KNOWSMYSTIC) || IS_KAIO(vch)) {
    ch_printf(ch, "%s already knows mystic.\n\r", vch->name);
    return;
  }

  if (IS_SET(vch->pcdata->flags, PCFLAG_KNOWSLSSJ)) {
    ch_printf(ch, "%s already knows something else.\n\r", vch->name);
    return;
  }

  if (vch->tmystic > 0) {
    ch_printf(ch, "%s is already being taught mystic.\n\r", vch->name);
    return;
  }

  ch_printf(ch, "\n\r");
  ch_printf(vch, "\n\r");

  act(AT_WHITE,
      "You start dancing around in a large circle around $N, chanting random gibberish.",
      ch, NULL, vch, TO_CHAR);

  act(AT_WHITE,
      "$n starts dancing in a large circle around you, chanting some strange sort of unintelligible gibberish.",
      ch, NULL, vch, TO_VICT);

  act(AT_WHITE,
      "$n starts dancing in a large circle around $N, chanting some strange sort of unintelligible gibberish.",
      ch, NULL, vch, TO_NOTVICT);

  ch_printf(ch,
            "&RYou have begun teaching %s how to use mystic. You must not type\n\r"
            "a single command over the course of the next 30 mins. If you do, it\n\r"
            "will interrupt the process and you will have to start all over\n\r"
            "again.\n\r", vch->name);

  ch_printf(vch,
            "&R%s has begun teaching you how to use mystic. You must not type\n\r"
            "a single command over the course of the next 30 mins. If you do, it\n\r"
            "will interrupt the process and you will have to start all over\n\r"
            "again.\n\r", ch->name);

  ch->teaching = vch;
  vch->teaching = ch;
  ch->tmystic = 1;
  vch->tmystic = 2;
  vch->mysticlearn = 30;
  return;
}

void do_manifestpotara( CHAR_DATA * ch, char *argument )
{
  OBJ_DATA *o;
  OBJ_INDEX_DATA *pObjIndex;

  if( IS_NPC( ch ) )
    return;

  if( argument[0] == '\0' )
  {
    ch_printf( ch, "\n\rSyntax: manifest potara <left/right>\n\r" );
    ch_printf( ch, "\n\r" );
    return;
  }

  if( !( !str_cmp( argument, "left" ) ) && !( !str_cmp( argument, "right" ) ) )
  {
    do_kaiocreate( ch, "" );
    return;
  }
  if( IS_AFFECTED( ch, AFF_KAIOKEN ) )
  {
    send_to_char( "You should powerdown first.\n\r", ch );
    return;
  }
  if( IS_AFFECTED( ch, AFF_MYSTIC ) )
  {
    send_to_char( "You should powerdown first.\n\r", ch );
    return;
  }
  if( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
  {
    send_to_char( "You can't concentrate enough for that.\n\r", ch );
    return;
  }
  if( !IS_NPC( ch ) && ch->exp < skill_table[gsn_manifestpotara]->skill_level[ch->class] )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }
  if( ch->mana < skill_table[gsn_manifestpotara]->min_mana )
  {
    send_to_char( "You don't have enough energy.\n\r", ch );
    return;
  }
  if( ch->carry_number >= 2 )
  {
    ch_printf( ch, "You haven't got any room.\n\r" );
    return;
  }


  WAIT_STATE( ch, skill_table[gsn_manifestpotara]->beats );
  if( can_use_skill( ch, number_percent(  ), gsn_manifestpotara ) )
  {
    int a = 0;
    if( !str_cmp( argument, "left" ) )
      a = 77;
    else if( !str_cmp( argument, "right" ) )
      a = 76;
    if( a == 0 )
    {
      bug( "Serious problem in function manifest potara", 0 );
      return;
    }
    act( AT_SKILL, "You focus your powers as the Supreme Kai and manifest a potara earring.", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n focuses $s powers as the Supreme Kai and manifests a potara earring.", ch, NULL, NULL, TO_NOTVICT );
    learn_from_success( ch, gsn_manifestpotara );
    pObjIndex = get_obj_index( a );
    o = create_object_new( pObjIndex, 1, ORIGIN_OINVOKE, ch->name );
    ch->exp -= ch->exp / 10;
    o = obj_to_char( o, ch );

    save_char_obj( ch );
  }
  else
  {
    act( AT_SKILL, "You fail to manifest your holy powers.. &RYou = FAIL.&D", ch, NULL, NULL, TO_CHAR );
    act( AT_SKILL, "$n fails to manifes $s holy powers.. &R$n is so FAIL.&D", ch, NULL, NULL, TO_NOTVICT );
    learn_from_failure( ch, gsn_manifestpotara );
  }
  ch->mana -= skill_table[gsn_manifestpotara]->min_mana;
  return;
}
