/*
 * Original code by Xkilla
 * Ported to SWR by Atrox
 * Cleaned up and modified further by Greven and Gavin of Dark Warriors
 */
#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include "mud.h"
#include "changes.h"

/*
 * Globals
 */
char * current_date   args( ( void ) );
int num_changes   args( ( void ) );
/*
 * Local Functions
 */

int      maxChanges;
int		 immortal_changes;
#define  VERSION 0
#define  NULLSTR( str )  ( str == NULL || str[0] == '\0' )
CHANGE_DATA * changes_table;

/* Hacktastic to make sure warning doesn't show up.. thats what man pages says todo :D */
size_t my_strftime(char *s, size_t max, const char *fmt, const struct tm *tm) {
	return strftime(s, max, fmt, tm);
}


void load_changes(void)
{
    FILE *fp;
    int i;

    if ( !(fp = fopen( CHANGES_FILE, "r")) )
    {
        bug( "Could not open Changes File for reading.");
        return;
    }

	immortal_changes = 0;

    fscanf( fp, "%d\n", &maxChanges );

    changes_table = (CHANGE_DATA*)calloc(1, sizeof( CHANGE_DATA) * (maxChanges+1) );
    
    for( i = 0; i < maxChanges; i++ )
    {
        changes_table[i].change = fread_string( fp );
        changes_table[i].coder = fread_string( fp );
        changes_table[i].date = fread_string( fp );
        changes_table[i].mudtime = fread_number( fp );
// Add imm variables to each data so we know if it should only show up for imms
		changes_table[i].immchange = fread_number( fp );
		if ( changes_table[i].immchange == 1)
			immortal_changes++;
    }
    fclose(fp);
    log_string(" Done changes " );  
    return;
}

char * current_date( )
{
    static char buf [ 128 ];
    struct tm * datetime;

    datetime = localtime( &current_time );
    my_strftime( buf, 128, "%x", datetime );
    return buf;
}

void save_changes(void)
{
    FILE *fp;
    int i;

    if ( !(fp = fopen( CHANGES_FILE,"w")) )
    {
        perror( CHANGES_FILE );
        return;
    }

    fprintf( fp, "%d\n", maxChanges ); 

    for( i = 0; i < maxChanges; i++ )
    {
        fprintf (fp, "%s~\n", changes_table[i].change);
  		fprintf (fp, "%s~\n", changes_table[i].coder);
        fprintf (fp, "%s~\n", changes_table[i].date);
        fprintf (fp, "%ld\n", changes_table[i].mudtime );
		fprintf	(fp, "%d\n",  changes_table[i].immchange );
        fprintf( fp, "\n" );
    } 
    fclose(fp);
    return;
} 

void delete_change(int iChange)
{
    int i,j;
    CHANGE_DATA * new_table;

    new_table = (CHANGE_DATA*)calloc(1, sizeof( CHANGE_DATA ) * maxChanges );

    if( !new_table )
    {
       return;
    }
    
    for ( i= 0, j = 0; i < maxChanges+1; i++)
    {
        if( i != iChange )
        {
            new_table[j] = changes_table[i];
            j++;
        }
		else
		{
			if ( changes_table[i].change )
				STRFREE(changes_table[i].change);
			if ( changes_table[i].coder )
				STRFREE(changes_table[i].coder);
			if ( changes_table[i].date )
				STRFREE(changes_table[i].date);
		}
    }
    
    free( changes_table );
    changes_table = new_table;

    maxChanges--;
    
    return;
}

void do_addchange(CHAR_DATA *ch, char *argument )
{
    CHANGE_DATA * new_table;
    char buf[MAX_STRING_LENGTH];
    
    if ( IS_NPC( ch ) )
        return;
    
    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: addchange <change>\n\r", ch);
        send_to_char( "Type 'changes' to view the list.\n\r", ch);
        return;
    }

    maxChanges++;
    new_table = (CHANGE_DATA*)realloc( changes_table, sizeof( CHANGE_DATA ) *(maxChanges+1) );

    if (!new_table) /* realloc failed */
    {
        send_to_char ("Memory allocation failed. Brace for impact.\n\r",ch);
        return;
    }

    changes_table = new_table;
    
    changes_table[maxChanges-1].change  = STRALLOC( argument );
    changes_table[maxChanges-1].coder   = STRALLOC( ch->name );
    changes_table[maxChanges-1].date    = STRALLOC(current_date());
    changes_table[maxChanges-1].mudtime = current_time;
     //Auto set it to Normal Change
    changes_table[maxChanges-1].immchange = FALSE;
    
    //send_to_char("Changes Created.\n\r",ch);
    //send_to_char("Type 'changes' to see the changes.\n\r",ch);
    //snprintf (buf, MAX_STRING_LENGTH, "%s", "A new change has been added, type 'CHANGES' to see it." );
    sprintf (buf, "&c%s &whas added a new change, type '&Ychanges&w' to see what it was",ch->name);
    echo_to_all( AT_IMMORT, buf, ECHOTAR_ALL );
    save_changes();
    return;
}
/*
 * Added by greven for imm changes
 *
 */
void do_addimmchange(CHAR_DATA *ch, char *argument )
{
    CHANGE_DATA * new_table;
    char buf[MAX_STRING_LENGTH];   
 
    if ( IS_NPC( ch ) )
        return;
    
    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: addimmchange <change>\n\r", ch );
        send_to_char( "Type 'changes' to view the list.\n\r", ch );
        return;
    }

    maxChanges++;
    new_table = (CHANGE_DATA*)realloc( changes_table, sizeof( CHANGE_DATA ) *(maxChanges+1) );

    if (!new_table) /* realloc failed */
    {
        send_to_char ("Memory allocation failed. Brace for impact.\n\r",ch);
        return;
    }

    changes_table = new_table;
    
    changes_table[maxChanges-1].change  = STRALLOC( argument );
    changes_table[maxChanges-1].coder   = STRALLOC( ch->name );
    changes_table[maxChanges-1].date    = STRALLOC(current_date());
    changes_table[maxChanges-1].mudtime = current_time;
// Autoset to imm change, do NOT echo it to the mud
	changes_table[maxChanges-1].immchange = TRUE;
	immortal_changes++;    
    //send_to_char("Changes Created.\n\r",ch);
    //send_to_char("Type 'changes' to see the changes.\n\r",ch);
    //sprintf( buf, "A new change has been added, type 'CHANGES' to see it." );
    sprintf (buf, "&c%s &whas added a new change, type '&Ychanges&w' to see what it was",ch->name);
    echo_to_all( AT_IMMORT, buf, ECHOTAR_IMM );
    save_changes();
    return;
}

void do_chedit( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
 
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
 
    if ( IS_NPC(ch) )
        return;
    
    if (!ch->desc || NULLSTR(arg1) )
    {
        ch_printf( ch, "Syntax: chedit load/save\n\r" );
        ch_printf( ch, "Syntax: chedit delete (change number)\n\r" );
        ch_printf( ch, "Syntax: chedit immchange (change number)\n\r" );
        return;
    }

    if ( !str_cmp(arg1,"load") )
    {
        load_changes( );
        send_to_char("Changes Loaded.\n\r",ch);
        return;
    }

    if ( !str_cmp(arg1,"save") )
    {
        save_changes( );
        send_to_char("Changes Saved.\n\r",ch);
        return;
    }

    if ( !str_cmp(arg1,"immchange") )
    {
        int num;
        
        if ( NULLSTR(arg2) || !is_number( arg2 ) )
        {
            send_to_char("&wFor chedit immchange, you must provide a change number.\n\r",ch);
            send_to_char("Syntax: chedit immchange (change number)\n\r",ch);
            return;
        }

        num = atoi( arg2 );
		num--;
        if ( num < 0 || num > maxChanges )
        {
            ch_printf( ch, "Valid changes are from 1 to %d.\n\r", maxChanges );
            return;
        }
	
		if ( changes_table[num].immchange == FALSE)
			changes_table[num].immchange = TRUE;
		else
			changes_table[num].immchange = FALSE;
		immortal_changes++;
        save_changes( );
        send_to_char("Change flag changed.\n\r",ch);
        return;
    }
    if ( !str_cmp(arg1, "delete"))
    {
        int num;
        
        if ( NULLSTR(arg2) || !is_number( arg2 ) )
        {
            send_to_char("#wFor chsave delete, you must provide a change number.{x\n\r",ch);
            send_to_char("Syntax: chsave delete (change number)\n\r",ch);
            return;
        }

        num = atoi( arg2 );
		num--;
        if ( num < 0 || num > maxChanges )
        {
            ch_printf( ch, "Valid changes are from 0 to %d.\n\r", maxChanges );
            return;
        }
        delete_change( num );
        save_changes( );
        send_to_char("Change deleted.\n\r",ch);
        return;
    }
    return;
}
char *line_indent (char *text, int wBegin, int wMax)
{
   static char buf[MAX_STRING_LENGTH];
   char *ptr;
   char *ptr2;
   int count = 0;
   bool stop = FALSE;
   int wEnd = 0;
   buf[0] = '\0';
   ptr = text;
   ptr2 = buf;

   while (!stop)
   {
      if (count == 0)
      {
	 if (*ptr == '\0')
	    wEnd = wMax - wBegin;
	 else if (strlen (ptr) < (wMax - wBegin))
	    wEnd = wMax - wBegin;
	 else
	 {
	    int x = 0;

	    while (*(ptr + (wMax - wBegin - x)) != ' ')
	       x++;
	    wEnd = wMax - wBegin - (x - 1);
	    if (wEnd < 1)
	       wEnd = wMax - wBegin;
	 }
      }
      if (count == 0 && *ptr == ' ')
	 ptr++;
      else if (++count != wEnd)
      {
	 if ((*ptr2++ = *ptr++) == '\0')
	    stop = TRUE;
      }
      else if (*ptr == '\0')
      {
	 stop = TRUE;
	 *ptr2 = '\0';
      }
      else
      {
	 int k;

	 count = 0;
	 *ptr2++ = '\n';
	 *ptr2++ = '\r';
	 for (k = 0; k < wBegin; k++)
	    *ptr2++ = ' ';
      }
   }
   return buf;
}
  
// Modified by greven for immortal stuff, as well as fixing crash if there are less than 10 changes
// Further Modifications needed to implement a no-disconnect status for players. - Kalthizar 3.31.09

void do_changes (CHAR_DATA * ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH], *test;
   int i, page = 0, maxpage = 0, today;
   int display = 0;
   argument = one_argument (argument, arg);
   argument = one_argument (argument, arg2);

   if (IS_NPC (ch))
      return;
   if (maxChanges < 1)
      return;

   i = 0;
   test = current_date ();
   today = 0;


   for (i = 0; i < maxChanges; i++)
   {
      if (!str_cmp (test, changes_table[i].date))
	 today++;
   }

   if (is_number (arg))
      page = atoi (arg);
   //maxpage = (maxChanges/10)+1;
   maxpage = (maxChanges + 9) / 10;

   if (page > 0)
   {
      if (page > maxpage)
      {
	 sprintf (buf, "Show which page 1-%d\n\r", maxpage);
	 send_to_char (buf, ch);
	 return;
      }
      page *= 10;
   }
   send_to_char ("&wNUMBER &cIMMORTAL         &YDATE         &wCHANGE                                        &D\n\r", ch);

   send_to_char ("&w-&z==============================================================================&w-\n\r", ch);


   if (!str_cmp (arg, "search"))
   {
      int dsp = 0;

      if (arg2[0] == '\0')
      {
	 send_to_char ("What do you want to search for??.\n\r", ch);
	 return;
      }

      for (i = 0; i < maxChanges; i++)
      {
	 if (!str_infix (arg2, changes_table[i].change)
	     || !str_infix (arg2, changes_table[i].coder))

//            if (strstr(changes_table[i].change, arg2))
	 {
	    sprintf (buf, "&z[&w%4d&z] &c%-13s &Y%11s &c- &w%-52s\n\r",
		     (++dsp),
		     changes_table[i].coder,
		     changes_table[i].date,
		     line_indent (changes_table[i].change, 35, 79));
	    send_to_char (buf, ch);
	 }
      }
      if (dsp == 0)
	 send_to_char ("There is NO match with what you have entered!.", ch);
   send_to_char ("&w-&z==============================================================================&w-\n\r", ch);
      return;
   }


   for (i = 0; i < maxChanges; i++)
   {
      if (page == 0
	  && changes_table[i].mudtime + (2 * 24L * 3600L) < current_time)
	 continue;
      display++;

      if (page > 0 && (page > 0 && (i < (page - 10) || i >= page)))
	 continue;

      sprintf (buf, "&z[&w%4d&z] &c%-13s &Y%11s &c- &w%-55s\n\r",
	       (i + 1),
	       changes_table[i].coder,
	       changes_table[i].date,
	       line_indent (changes_table[i].change, 35, 79));
      send_to_char (buf, ch);
   }
   send_to_char ("&w-&z==============================================================================&w-\n\r", ch);
   if (today > 0)
      sprintf (buf,
	       "&wThere is a total of [&r%d&w] changes of which [&c%d&w] %s been added today.&D\n\r",
	       maxChanges, today, today > 1 ? "have" : "has");
   else
      sprintf (buf, "&wThere is a total of [&r%d&w] changes.&D", maxChanges);
   send_to_char (buf, ch);

   send_to_char ("&w-&z==============================================================================&w-\n\r", ch);
   sprintf (buf, "&wTo see pages of changes use: 'changes <&c1&w-&r%d&w>'\n\r",
	    (maxChanges + 9) / 10);
   send_to_char (buf, ch);
   send_to_char
      ("&wTo search all changes for a change use: 'changes search <&cword&w>'&D\n\r",
       ch);

   send_to_char ("&w-&z==============================================================================&w-\n\r", ch);

}
int num_changes(void)
{
	char *test;
    int   today;
    int   i;
    
    i = 0;
    test = current_date();
    today = 0;
    
     for ( i = 0; i < maxChanges; i++)
        if (!str_cmp(test,changes_table[i].date))
            today++;
            
     return today;
}
void nnum_changes (CHAR_DATA *ch)
{
	char buf[MAX_INPUT_LENGTH];
	char *test;
	int today;
	int i;

	i = 0;
	test = current_date ();
	today = 0;

	for (i = 0; i < maxChanges; i++)
   {
      if (!str_cmp (test, changes_table[i].date))
	 today++;
   }

   if ( today > 0 )
   {
	   sprintf( buf, "&CThere have been &R%d&C changes added to the MUD today&B( &G%d&R total&B )&D\n\r&CType &b'&cchanges&b'&Cto view them&D\n\r", today - (IS_IMMORTAL(ch) ? 0 : immortal_changes),  maxChanges - (IS_IMMORTAL(ch) ? 0 : immortal_changes) );
	   send_to_char( buf, ch );
	   return;
   }
   return;
}

