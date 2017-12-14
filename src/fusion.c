#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

void
do_fusiondance(CHAR_DATA * ch, char *argument)
{
  CHAR_DATA *rch;
  CHAR_DATA *vch;
  CHAR_DATA *dch;               /*dominant fuse char */
  CHAR_DATA *sch;               /*stasis fuse char */
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  bool cright = FALSE;
  bool vright = FALSE;
  bool cleft = FALSE;
  bool vleft = FALSE;
  OBJ_DATA *ro;
  OBJ_DATA *vo;
  OBJ_DATA *po;
  OBJ_DATA *o;
  OBJ_INDEX_DATA *pObjIndex;
  // int sn;
  // int value;
  bool saiyan1 = FALSE, saiyan2 = FALSE;
  bool namek1 = FALSE, namek2 = FALSE;
  bool human1 = FALSE, human2 = FALSE;
  bool halfb1 = FALSE, halfb2 = FALSE;
  bool genie1 = FALSE, genie2 = FALSE;


  argument = one_argument(argument, arg);
  argument = one_argument(argument, arg2);

  if (arg[0] == '\0') {
    ch_printf(ch,
              "\n\rSyntax:  fusiondance (first person) (second person)\n\r");
    ch_printf(ch,
              "\n\rThe order in which you put the two people's names in does not matter.\n\r");
    return;
  }
  if ((rch = get_char_room(ch, arg)) == NULL) {
    ch_printf(ch, "There is nobody here named %s.\n\r", arg);
    return;
  }
  if ((vch = get_char_room(ch, arg2)) == NULL) {
    ch_printf(ch, "There is nobody here named %s.\n\r", arg2);
    return;
  }
  if (IS_NPC(rch) || IS_NPC(vch)) {
    ch_printf(ch, "NPC's cannot fuse.\n\r");
    return;
  }
  if (is_fused(rch)) {
    ch_printf(ch, "%s has already fused before.\n\r", rch->name);
    return;
  }
  if (is_fused(vch)) {
    ch_printf(ch, "%s has already fused before.\n\r", vch->name);
    return;
  }
  if (IS_IMMORTAL(rch) || IS_IMMORTAL(vch)) {
    ch_printf(ch, "Admins may not fuse.\n\r");
    return;
  }
  if (rch->pcdata->clan) {
    ch_printf(ch, "%s must be outcasted from their clan first.\n\r",
              rch->name);
    return;
  }
  if (vch->pcdata->clan) {
    ch_printf(ch, "%s must be outcasted from their clan first.\n\r",
              vch->name);
    return;
  }

  if (is_transformed(rch)) {
    ch_printf(ch, "%s must power down completely first.\n\r", rch->name);
    return;
  }
  if (is_transformed(vch)) {
    ch_printf(ch, "%s must power down completely first.\n\r", vch->name);
    return;
  }
  if ((ro = fusion_item_1(rch)) == NULL) {
    if ((ro = fusion_item_2(rch)) == NULL) {
      ch_printf(ch, "%s is not wearing a needed item..\n\r", rch->name);
      return;
    } else
      cleft = TRUE;
  } else
    cright = TRUE;
  if ((vo = fusion_item_1(vch)) == NULL) {
    if ((vo = fusion_item_2(vch)) == NULL) {
      ch_printf(ch, "%s is not wearing a needed item.\n\r", vch->name);
      return;
    } else
      vleft = TRUE;
  } else
    vright = TRUE;

  if (cright && vright) {
    ch_printf(ch, "One of them must be wearing a needed item.\n\r");
    return;
  }
  if (cleft && vleft) {
    ch_printf(ch, "One of them must be wearing a needed item.\n\r");
    return;
  }
  if (cright && vleft) {
    dch = rch;
    sch = vch;
  } else if (cleft && vright) {
    dch = vch;
    sch = rch;
  } else {
    ch_printf(ch, "Cyris fucked up. Let him know...\n\r");
    return;
  }

  unequip_char(rch, ro);
  unequip_char(vch, vo);
  extract_obj(ro);
  extract_obj(vo);

  pObjIndex = get_obj_index(620);
  po = create_object_new(pObjIndex, 1, ORIGIN_OINVOKE, ch->name);
  po = obj_to_char(po, dch);

  dch->fused[dch->fusions] = STRALLOC(sch->name);
  sch->fused[sch->fusions] = STRALLOC(dch->name);
  dch->fusions++;
  sch->fusions++;
  dch->bck_name = STRALLOC(dch->name);
  sch->bck_name = STRALLOC(sch->name);
  dch->bck_race = dch->race;
  sch->bck_race = sch->race;
  dch->bck_pl = dch->exp;
  sch->bck_pl = sch->exp;
  dch->fusiontimer = 60;
  sch->fusiontimer = 60;

/* Get fusion shit for this --Cyris*/

  if (!str_cmp(get_race(dch), "saiyan"))
    saiyan1 = TRUE;
  if (!str_cmp(get_race(sch), "saiyan"))
    saiyan2 = TRUE;
  if (!str_cmp(get_race(dch), "namek"))
    namek1 = TRUE;
  if (!str_cmp(get_race(sch), "namek"))
    namek2 = TRUE;
  if (!str_cmp(get_race(dch), "human"))
    human1 = TRUE;
  if (!str_cmp(get_race(sch), "human"))
    human2 = TRUE;
  if (!str_cmp(get_race(dch), "halfbreed"))
    halfb1 = TRUE;
  if (!str_cmp(get_race(sch), "halfbreed"))
    halfb2 = TRUE;
  if (!str_cmp(get_race(sch), "genie"))
    genie2 = TRUE;
  if (!str_cmp(get_race(sch), "genie"))
    genie2 = TRUE;

  if ((saiyan1 && namek2)) {
    dch->race = get_race_num("saiyan-n");
    dch->class = get_class_num("saiyan-n");
  } else if ((saiyan2 && namek1)) {
    dch->race = get_race_num("namek-s");
    dch->class = get_class_num("namek-s");
  } else if ((saiyan1 && human2)) {
    dch->race = get_race_num("saiyan-h");
    dch->class = get_class_num("saiyan-h");
  } else if ((saiyan2 && human1)) {
    dch->race = get_race_num("human-s");
    dch->class = get_class_num("human-s");
  } else if ((saiyan1 && halfb2)) {
    dch->race = get_race_num("saiyan-hb");
    dch->class = get_class_num("saiyan-hb");
  } else if ((saiyan2 && halfb1)) {
    dch->race = get_race_num("halfbreed-s");
    dch->class = get_class_num("halfbreed-s");
  } else if ((saiyan1 && saiyan2)) {
    dch->race = get_race_num("saiyan-s");
    dch->class = get_class_num("saiyan-s");
  } else if ((namek1 && halfb2)) {
    dch->race = get_race_num("namek-hb");
    dch->class = get_class_num("namek-hb");
  } else if ((namek2 && halfb1)) {
    dch->race = get_race_num("halfbreed-n");
    dch->class = get_class_num("halfbreed-n");
  } else if ((human1 && namek2)) {
    dch->race = get_race_num("human-n");
    dch->class = get_class_num("human-n");
  } else if ((human2 && namek1)) {
    dch->race = get_race_num("namek-h");
    dch->class = get_class_num("namek-h");
  } else if ((human1 && halfb2)) {
    dch->race = get_race_num("human-hb");
    dch->class = get_class_num("human-hb");
  } else if ((human2 && halfb1)) {
    dch->race = get_race_num("halfbreed-h");
    dch->class = get_class_num("halfbreed-h");
  } else if ((human1 && human2)) {
    dch->race = get_race_num("human-h");
    dch->class = get_class_num("human-h");
  } else if ((namek1 && namek2)) {
    ch_printf(ch, "You cannot fuse like this.");
  } else if ((halfb1 && halfb2)) {
    dch->race = get_race_num("halfbreed-hb");
    dch->class = get_class_num("halfbreed-hb");
  } else if ((human1 && genie2)) {
    dch->race = get_race_num("Human-g");
    dch->class = get_class_num("Human-g");
  } else if ((genie1 && human2)) {
    dch->race = get_race_num("Human-g");
    dch->class = get_class_num("Human-g");
  }
  dch->exp += sch->exp;
  dch->pl = dch->exp;

  /*
   * message sent to dch 
   */

  ch_printf(dch,
            "You life force fuses with %s's causing them to merge with you!\n\r",
            sch->name);

  /*
   * message sent to sch 
   */
  ch_printf(sch,
            "Your body fuses with %s's and you agree to give your namekian power for some reason...\n\r",
            dch->name);

  while ((o = carrying_noquit(sch)) != NULL) {
    obj_from_char(o);
    obj_to_room(o, sch->in_room);
    ch_printf(sch, "&wYou drop %s&w.\n\r", o->short_descr);
  }

  SET_BIT(dch->fusionflags, FUSION_DANCE);
  SET_BIT(sch->fusionflags, FUSION_STASIS);
  do_reserve(ch, dch->name);
  do_reserve(ch, sch->name);
  char_to(sch, ROOM_VNUM_FUSIONSTASIS);
  save_char_obj(dch);
  save_char_obj(sch);

  /*
   * do_sset(ch,strcat(dch->name," all 95"));
   */
  ch_printf(ch, "%s and %s have been fused together.\n\r", dch->name,
            sch->name);
}
