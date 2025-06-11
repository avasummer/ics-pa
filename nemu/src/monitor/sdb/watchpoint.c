/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <sdb.h>
#include <stdlib.h>
#include <string.h>

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  char* expr;
  struct watchpoint *next;
  word_t result;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

int new_wp(char* args){
  WP *p=NULL,*q=NULL;
  if (free_==NULL)return -1;
  p=free_;
  p->expr = strdup(args);
  Log("%s",p->expr);
  free_=free_->next;
  if (head!=NULL)
  {
    q=head;
    while (q->next!=NULL)q=q->next;
    q->next=p;
    return 1;
  }
  head=p;
  return 1;
}

void free_wp(int no)
{
  if (head==NULL)
  {
    printf("Watchpoint %d not found.\n",no);
    return;
  }
  if (head->NO==no)
  {
    WP *wp=head;
    head=wp->next;
    wp->next=free_;
    free_=wp;
    return;
  }
  WP *p=NULL;
  if (head!=NULL)
  {
    p=head;
    while (1){
      if (p->next->NO==no){
        WP *wp=p->next;
        p->next=wp->next;
        wp->next=free_;
        free_=wp;
        return;
      }
      if (p->next==NULL)
      {
        printf("Watchpoint %d not found.\n",no);
        return;
      }
      p=p->next;
    }
  }
}

bool check_expr(){
  if (head==NULL)return 0;
  WP *p;
  bool success=true;
  p=head;
  Log("%s",p->expr);
  word_t result = expr(p->expr,&success);
  if (result!=p->result && success)
  {
    printf("Watchpoint %d changed.",p->NO);
    return 1;
  }
  return 0;
}
/* TODO: Implement the functionality of watchpoint */

