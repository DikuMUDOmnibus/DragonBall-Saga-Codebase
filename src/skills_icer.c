#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

/* Place holder */
void do_mechanize( CHAR_DATA * ch, char *argument )
{
  OBJ_DATA *ro, *ri, *ru, *ry, *rt;
/*
    if( !IS_IMMORTAL(ch) )
    {
	send_to_char("Test Command. Immortals only.\n\r", ch);
	return;
    }
*/
  
  if( !IS_ICER( ch ) )
  {
    send_to_char( "You can't do that.\n\r", ch );
    return;
  }
  if( wearing_sentient_chip( ch ) )
  {
    ch_printf( ch, "You can't use this." );
    return;
  }

  if( !argument || argument[0] == '\0' )
  {
    send_to_char( "Syntax: mechanize [ START ]\n\r", ch );
    return;
  }

  if( !str_prefix( argument, "START" ) )
  {
    if( !wearing_augment( ch ) )
    {
      send_to_char( "You must be wearing the Mecha-Icer Augmentation.", ch );
      return;
    }
    if( !wearing_bodyplate( ch ) )
    {
      send_to_char( "You must be wearing the Mecha-Icer Body Plating.", ch );
      return;
    }
    if( !wearing_forearm( ch ) )
    {
      send_to_char( "You must be wearing the Mecha-Icer Forearm Segments.", ch );
      return;
    }
    if( !wearing_legseg( ch ) )
    {
      send_to_char( "You must be wearing the Mecha-Icer Leg Segments.", ch );
      return;
    }
    if( !wearing_cranial( ch ) )
    {
      send_to_char( "You must be wearing the Mecha-Icer Cranial Plating.", ch );
      return;
    }
    if( ch->pcdata->learned[gsn_mechaicer] == 100 )
    {
      send_to_char( "You have already absorbed this power! Use it well!\n\r", ch );
      return;
    }
    if( ( ( ro = wearing_augment( ch ) ) && ( ri = wearing_bodyplate( ch ) ) && ( ru = wearing_forearm( ch ) ) && ( ry = wearing_legseg( ch ) ) && ( rt = wearing_cranial( ch ) ) && ch->pcdata->learned[gsn_mechaicer] == 0 ) )
    {
      ch->pcdata->learned[gsn_mechaicer] = 100;
      unequip_char( ch, ro );
      extract_obj( ro );
      unequip_char( ch, ri );
      extract_obj( ri );
      unequip_char( ch, ru );
      extract_obj( ru );
      unequip_char( ch, ry );
      extract_obj( ry );
      unequip_char( ch, rt );
      extract_obj( rt );
      send_to_char( "&cMechanization Complete!&D\n\r", ch );
      return;
    }
  }
  do_mechanize( ch, "" );
  return;
}

OBJ_DATA *wearing_augment( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_BODY )
    {
      if( obj->pIndexData->vnum == 50007 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_bodyplate( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_BODY )
    {
      if( obj->pIndexData->vnum == 150215 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_forearm( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_ARMS )
    {
      if( obj->pIndexData->vnum == 150218 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_legseg( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_LEGS )
    {
      if( obj->pIndexData->vnum == 150216 )
        return obj;
    }
  }
  return NULL;
}

OBJ_DATA *wearing_cranial( CHAR_DATA * ch )
{
  OBJ_DATA *obj, *obj_next;
  for( obj = ch->first_carrying; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if( obj->wear_loc == WEAR_HEAD )
    {
      if( obj->pIndexData->vnum == 150217 )
        return obj;
    }
  }
  return NULL;
}
