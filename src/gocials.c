#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

void do_gocials( CHAR_DATA * ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  SOCIALTYPE *social;

  argument = one_argument( argument, arg );

  if ((social = find_social (arg)) == NULL)
    {
      send_to_char ("That is not a social.\n\r", ch);
      return;
    }

  if (!IS_NPC (ch) && xIS_SET (ch->act, PLR_NO_EMOTE))
    {
      send_to_char ("You are anti-social!\n\r", ch);
      return;
    }

  switch (ch->position)
    {
    case POS_DEAD:
      send_to_char ("Lie still; you are DEAD.\n\r", ch);
      return;
    case POS_INCAP:
    case POS_MORTAL:
      send_to_char ("You are hurt far too bad for that.\n\r", ch);
      return;
    case POS_STUNNED:
      send_to_char ("You are too stunned to do that.\n\r", ch);
      return;
    case POS_SLEEPING:
      /*
         * I just know this is the path to a 12" 'if' statement.  :(
         * But two players asked for it already!  -- Furey
       */
      if (!str_cmp (social->name, "snore"))
	break;
      send_to_char ("In your dreams, or what?\n\r", ch);
      return;
    }

  one_argument (argument, arg);
  victim = NULL;

  if (arg[0] == '\0')
    {
      sprintf (buf, "&R[&zG&GO&zC&GI&zA&GL&R]&c %s", social->others_no_arg);
      act (AT_SOCIAL, buf, ch, NULL, victim, TO_MUD);
      sprintf (buf, "&R[&zG&GO&zC&GI&zA&GL&R]&c %s", social->char_no_arg);
      act (AT_SOCIAL, buf, ch, NULL, victim, TO_CHAR);
      return;
    }

  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("You really shouldn't talk about people who aren't logged in.\n\r", ch);
      return;
    }

  if (victim == ch)
    {
      sprintf (buf, "&R[&zG&GO&zC&GI&zA&GL&R]&c %s", social->others_auto);
      act (AT_SOCIAL, buf, ch, NULL, victim, TO_MUD);
      sprintf (buf, "&R[&zG&GO&zC&GI&zA&GL&R]&c %s", social->char_auto);
      act (AT_SOCIAL, buf, ch, NULL, victim, TO_CHAR);
      return;
    }
  else
    {
      sprintf (buf, "&R[&zG&GO&zC&GI&zA&GL&R]&c %s", social->others_found);
      act (AT_SOCIAL, buf, ch, NULL, victim, TO_MUD);
      sprintf (buf, "&R[&zG&GO&zC&GI&zA&GL&R]&c %s", social->char_found);
      act (AT_SOCIAL, buf, ch, NULL, victim, TO_CHAR);
      sprintf (buf, "&R[&zG&GO&zC&GI&zA&GL&R]&c %s", social->vict_found);
      act (AT_SOCIAL, buf, ch, NULL, victim, TO_VICT);
      return;
    }
}
