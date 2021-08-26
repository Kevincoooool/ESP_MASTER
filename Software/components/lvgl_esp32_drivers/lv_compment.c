#include "lv_compment.h"

void lv_add_compment(compment_t *compment1,compment_t *compment2)
{
    compment1->next=compment2;
}
void move_free(compment_t *compment)
{
    compment_t *comp=compment;
    while(1)
    {
        lv_anim_del(&comp->obj,comp->funcb);
        comp=comp->next;
        if(comp==NULL)
            break;
    }

}
void obj_move(compment_t *compment,Move_mode mode)
{
    compment_t *comp=compment;
    while(1)
    {
        lv_anim_init(&comp->a);
        lv_anim_set_var(&comp->a, comp->obj);
        lv_anim_set_exec_cb(&comp->a, (lv_anim_exec_xcb_t) comp->funcb);
        if(mode)
        {
            lv_anim_set_values(&comp->a, comp->start,comp->end);
            lv_anim_set_time(&comp->a, comp->start_t);
        }else
        {
            lv_anim_set_values(&comp->a, comp->end,comp->start);
            lv_anim_set_time(&comp->a, comp->end_t);
            if(comp->cb==lv_anim_path_ease_out)
            comp->cb=lv_anim_path_ease_in;
        }
        lv_anim_path_init(&comp->path);
        lv_anim_path_set_cb(&comp->path, comp->cb);
        lv_anim_set_path(&comp->a, &comp->path);
        lv_anim_start(&comp->a);
        comp=comp->next;
        if(comp==NULL)
            break;
    }
    //ANIEND
}