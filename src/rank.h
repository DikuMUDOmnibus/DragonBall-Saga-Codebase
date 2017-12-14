/* Rank Code by Chili
 */

#ifndef _RANK_H_
#define _RANK_H_


#define RANK_EFFECT(x) void x(CHAR_DATA *ch)

#define sbuf_chr(x, buf, bp) ((*(bp) - (buf) >= MAX_STRING_LENGTH - 1) ?  1 : (*(*(bp))++ = (x), 0))



struct rank_table_t {
  char *color_prefix; /* Prefixing color string for showrank */
  char *rank_name_m; /* Rank Male or Neutered Name */
  char *rank_name_f; /* Rank Female Name or NULL if default to male */
  void (*func_gain)(CHAR_DATA *); /* To apply special affects and what not obtaining this rank */
  void (*func_loss)(CHAR_DATA *); /* To apply special affects and what not losing this rank */
  bool (*race)(CHAR_DATA *); /* Race */
  double long pl_req; /* Powerlevel required to claim rank */
  short value; /* Rank Value */
};

struct rank_tables_list_t {
  char *name; /* Name of Rank Table */
  char **rank;
  short high;
  bool (*race)(CHAR_DATA *); /* function to check race */
};

void initialize_ranks();
int string_prefix(char *, char *);
void save_ranks(FILE *);
void load_rank(char *, int, char *);
char *get_ch_rank(CHAR_DATA *);
char *get_ch_rank_at(CHAR_DATA *, int);
char *mud_capstr(char *);
int get_rank_list_num(CHAR_DATA *ch);
int sbuf_format(char *, char **, const char *, ...);
int has_rank(CHAR_DATA *);

/* Effect Protos */
RANK_EFFECT(overlord_gain);
RANK_EFFECT(overlord_loss);
RANK_EFFECT(icer_king_loss);
RANK_EFFECT(icer_king_gain);
RANK_EFFECT(supreme_kai_loss);
RANK_EFFECT(supreme_kai_gain);
RANK_EFFECT(saiyan_king_loss);
RANK_EFFECT(saiyan_king_gain);
RANK_EFFECT(sanctuary_loss);
RANK_EFFECT(great_saiya_gain);
RANK_EFFECT(great_saiya_loss);

#endif /* _RANK_H_ */
