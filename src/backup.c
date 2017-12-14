/****************************************************************************
 * [S]imulated [M]edieval [A]dventure multi[U]ser [G]ame      |   \\._.//   *
 * -----------------------------------------------------------|   (0...0)   *
 * SMAUG 1.4 (C) 1994, 1995, 1996, 1998  by Derek Snider      |    ).:.(    *
 * -----------------------------------------------------------|    {o o}    *
 * SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,      |   / ' ' \   *
 * Scryn, Rennard, Swordbearer, Gorog, Grishnakh, Nivek,      |~'~.VxvxV.~'~*
 * Tricops and Fireblade                                      |             *
 * ------------------------------------------------------------------------ *
 * Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael        *
 * Chastain, Michael Quan, and Mitchell Tse.                                *
 * Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,          *
 * Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.     *
 * ------------------------------------------------------------------------ *
 *			    DB Infinity code based on DBSaga      	          *
 ****************************************************************************
 *    Some comments might have been removed please refer to the current     *
 *     copy of AFKMUD at afkmud.com or DBSaga2.5.2 from dbsdevelop.com      *
 *             Thanks for downloading! We respect coders!                   *
 ****************************************************************************/

#include <string.h>
#include <time.h>
#include "mud.h"

char *backup_fname_strings[] = {
  "pfiles_backup.tgz", "system_backup.tgz", "area_backup.tgz", "building_backup.tgz", "source_backup.tgz"
};


char *backup_directory_strings[] = {
  "../player/", "../system/", "../area/", "../building/", "../src/", "../backup/"
};


typedef enum
{
  BACKUP_PFILES, BACKUP_SYSTEM, BACKUP_AREAS, BACKUP_BUILDING, BACKUP_SOURCE, BACKUP_DIRECTORY
} backup_types;
 /*
  * ======
  * Locals
  * ======
  */
void do_backup args( ( CHAR_DATA * ch, char *argument ) );

void backup args( ( CHAR_DATA * ch, int type ) );
 /*
  * ===============
  * Backup command  -Nopey
  * ===============
  */
void do_backup( CHAR_DATA * ch, char *argument )
{

  if( !IS_IMMORTAL( ch ) || IS_NPC( ch ) )

  {

    send_to_char( "Huh?\n\r", ch );

    return;

  }


  if( argument[0] == '\0' )

  {

    send_to_char( "Syntax: backup <type>\n\r", ch );

    send_to_char( "Types being one of the following:\n\r" " pfiles systemfiles areas buildingfiles src all\n\r", ch );

    send_to_char( "Syntax: backup <all> will save a lot of time.\n\r", ch );

    return;

  }


  if( !str_cmp( argument, "pfiles" ) )

  {

    backup( ch, BACKUP_PFILES );

    send_to_char( "Backup complete.\n\r", ch );

    return;

  }

  else if( !str_cmp( argument, "systemfiles" ) || !str_cmp( argument, "system" ) )

  {

    backup( ch, BACKUP_SYSTEM );

    send_to_char( "Backup complete.\n\r", ch );

    return;

  }

  else if( !str_cmp( argument, "area" ) || !str_cmp( argument, "areas" ) )

  {

    backup( ch, BACKUP_AREAS );

    send_to_char( "Backup complete.\n\r", ch );

    return;

  }

  else if( !str_cmp( argument, "building" ) || !str_cmp( argument, "buildingfiles" ) )

  {

    backup( ch, BACKUP_BUILDING );

    send_to_char( "Backup complete.\n\r", ch );

    return;

  }

  else if( !str_cmp( argument, "src" ) || !str_cmp( argument, "source" ) )  /* Added by Chosen */

  {

    backup( ch, BACKUP_SOURCE );

    send_to_char( "Backup complete.\n\r", ch );

    return;

  }

  else if( !str_cmp( argument, "all" ) || !str_cmp( argument, "all" ) ) /* Also added by Chosen */

  {

    backup( ch, BACKUP_SOURCE );

    backup( ch, BACKUP_BUILDING );

    backup( ch, BACKUP_AREAS );

    backup( ch, BACKUP_SYSTEM );

    backup( ch, BACKUP_PFILES );

    send_to_char( "Total MUD Backup complete.\n\r", ch );

    return;

  }

  do_backup( ch, "" );

  return;

}

 /*
  * ================
  * Backup handler  -Nopey
  * ================
  */
void backup( CHAR_DATA * ch, int type )
{

  char cmd[1024];

  char buf[MAX_INPUT_LENGTH];
  /*
   * backup command 
   */
  sprintf( cmd, "tar -zcf %s %s", backup_fname_strings[type], backup_directory_strings[type] );

  system( cmd );
  /*
   * move file to backup directory 
   */
  sprintf( cmd, "mv %s %s", backup_fname_strings[type], backup_directory_strings[BACKUP_DIRECTORY] );

  system( cmd );


  sprintf( buf, "BACKUP(%s): %s file written, moved to backup directory.", ch->name, backup_fname_strings[type] );

  log_string( buf );

  return;

}
