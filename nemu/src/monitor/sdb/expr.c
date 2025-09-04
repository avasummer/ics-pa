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

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <memory/paddr.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ = 1,
  TK_NEQ = 2,
  TK_REGISTER,
  TK_DEC=10,
  TK_HEX=16,
  TK_LEFT_PAREN=3,
  TK_RIGHT_PAREN=4,
  DEREF=5
  /* TODO: Add more token types */
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},			// minus
  {"\\*", '*'},			// multi
  {"\\/", '/'},		// divide
  {"\\$[A-Za-z0-9]+", TK_REGISTER}, // reg
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},
  {"\\0\\x[0-9A-Fa-f]+", TK_HEX},
  {"[0-9]+", TK_DEC},
  {"\\(", TK_LEFT_PAREN},
  {"\\)", TK_RIGHT_PAREN},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static int save_token(int j,int substr_len,char *substr_start){
    for (int i = 0; i < 32; i ++) {
	if(tokens[i].type==0){
          tokens[i].type=rules[j].token_type;
          strncpy(tokens[i].str, substr_start, substr_len);
          return 0;
    	}
    }
    assert("Expression too long");
    return -1;
}

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //    i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:break;
          default:{
              save_token(i, substr_len, substr_start);
              nr_token ++;
              break;
            }
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p,int q) {
  if (tokens[p].type == TK_LEFT_PAREN && tokens[q].type == TK_RIGHT_PAREN) {
    int inparen=0;
    for(int i = p; i <= q; i ++) {
      if(tokens[i].type == TK_LEFT_PAREN)inparen++;
      if(tokens[i].type == TK_RIGHT_PAREN)inparen--;
      if (inparen==0&&tokens[i].type==TK_RIGHT_PAREN)
      {
        if (i!=q)return false;
        return true;
      }
    }
  }
  return false;
}

word_t eval(int p,int q,bool *success)
{
  Log("eval %d %d",p,q);
  if (*success==false)return 0;
  if (p > q) {
    printf("Bad expression at p:%d, q:%d\n",p,q);
    *success=false;
    return 0;
    /* Bad expression */
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    if(tokens[p].type == TK_DEC) return strtol(tokens[p].str,NULL, 10);
    if(tokens[p].type == TK_HEX) return strtol(tokens[p].str,NULL, 16);
    if(tokens[p].type == TK_REGISTER) return isa_reg_str2val(tokens[p].str+1,success);
    printf("Bad single token\n");
    *success=false;
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
     Log("paren %d %d",p+1,q-1);
    return eval(p + 1, q - 1,success);
  }
  else {
    int inparen=0,precedence=5,op=p;
    word_t val1=0,val2=0;
    for(int i = q; i >= p; i --) { //handle paren and find main op
      if(tokens[i].type == TK_RIGHT_PAREN)inparen++;
      if(tokens[i].type == TK_LEFT_PAREN)inparen--;
      if((tokens[i].type == '+' || tokens[i].type == '-') && inparen==0 && (precedence>1 || (op<i&&precedence==1))){
        op=i;
        precedence=1;
      }
      if((tokens[i].type == '*' || tokens[i].type == '/') && inparen==0 && (precedence>2 || (op<i&&precedence==2))){
        op=i;
        precedence=2;
      }
      if((tokens[i].type == TK_EQ || tokens[i].type == TK_NEQ || tokens[i].type == DEREF) && inparen==0 && (precedence>3 || (op<i&&precedence==3)))
      {
        op=i;
        precedence=3;
      }
    }
    if (op-1>=0) val1 = eval(p, op - 1, success);
    val2 = eval(op + 1, q, success);
    switch (tokens[op].type) {
    case '+': {return val1 + val2;}
    case '-': {return val1 - val2;}
    case '*':
      {
        return val1 * val2;
      }
    case '/':{
        if(val2 == 0)
        {
          printf("Division by zero,at token %d",q);
          *success=false;
          return 0;
        }
        return val1 / val2;
    }
    case DEREF:
      {
        return paddr_read(val2,4);
      }
    case TK_EQ:return val1 == val2;
    case TK_NEQ:return val1 != val2;
    default: break;
      /* We should do more things here. */
    }
  }
  return 0;
}

word_t expr(char *e, bool *success)
{
  memset(tokens, 0, sizeof(tokens));
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for (int i = 0; i < nr_token; i ++) {
    if (tokens[i].type == '*' && (i == 0 ||
      (tokens[i - 1].type != TK_RIGHT_PAREN &&
       tokens[i - 1].type != TK_DEC &&
       tokens[i - 1].type != TK_HEX)) ) {
      tokens[i].type = DEREF;
       }
  }
  word_t result=eval(0,nr_token-1,success);
  if (*success)return result;
  return 0;
}