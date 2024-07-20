#include <isa.h>
#include <regex.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {
  {"0x[0-9a-fA-F]+", 'h'}, // hex number
  {"[0-9]+",  'd'}, // decimal number
  {"\\$[0-9a-zA-Z]+", 'r'}, // register
  {"\\(",     '('}, // left bracket
  {"\\)",     ')'}, // right bracket
  {"\\+",     '+'}, // plus
  {"-",       '-'}, // minus
  {"\\*",     '*'}, // multiply
  {"/",       '/'}, // divide
  {" +",      ' '}, // spaces
  {"==",      '='}, // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX] = {};

void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++) {
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

static Token tokens[32] = {};
static int nr_token = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    if (nr_token >= (sizeof(tokens) / sizeof(Token))) {
      printf("Too many tokens\n");
      return false;
    }

    for (i = 0; i < NR_REGEX; i++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        position += substr_len;

        switch (rules[i].token_type) {
          case ' ':
            break;
          case 'd':
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            nr_token++;
            break;
          case 'h':
            tokens[nr_token].type = 'd';
            substr_start += 2;
            substr_len -= 2;
            // cover hex number to decimal number
            word_t hex = 0;
            for (int i=0; i<substr_len; i++) {
              if (substr_start[i] >= '0' && substr_start[i] <= '9') {
                hex = hex * 16 + substr_start[i] - '0';
              } else if (substr_start[i] >= 'a' && substr_start[i] <= 'f') {
                hex = hex * 16 + substr_start[i] - 'a' + 10;
              } else if (substr_start[i] >= 'A' && substr_start[i] <= 'F') {
                hex = hex * 16 + substr_start[i] - 'A' + 10;
              }
            }
            sprintf(tokens[nr_token].str, "%u", hex);
            nr_token++;
            break;
          case 'r':
            tokens[nr_token].type = 'd';
            bool success = true;
            word_t reg_val = isa_reg_str2val(substr_start + 1, substr_len - 1, &success);
            if (!success) {
              printf("Invalid register name: ");
              for (int i = 0; i < substr_len; i++)
                putchar(substr_start[i]);
              putchar('\n');
              return false;
            }
            sprintf(tokens[nr_token].str, "%u", reg_val);
            nr_token++;
            break;
          case '+':
          case '-':
          case '*':
          case '/':
          case '(':
          case ')':
          case '=':
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            nr_token++;
            break;
          default:
            TODO();
            break;
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

int find_main_operator(int p, int q) {
  int level = 0;
  int main_op = -1;

  for (int i = p; i <= q; i++) {
    if (tokens[i].type == '(') {
      level++;
    } else if (tokens[i].type == ')') {
      level--;
    } else if (level == 0) {
      if (tokens[i].type == '+' || tokens[i].type == '-') {
        main_op = i;
      } else if ((tokens[i].type == '*' || tokens[i].type == '/') && main_op == -1) {
        main_op = i;
      }
    }
  }

  return main_op;
}

/*
<expr> ::= <number>    # 一个数是表达式
  | "(" <expr> ")"     # 在表达式两边加个括号也是表达式
  | <expr> "+" <expr>  # 两个表达式相加也是表达式
  | <expr> "-" <expr>  # 接下来你全懂了
  | <expr> "*" <expr>
  | <expr> "/" <expr>
*/
int eval(int p, int q) {
  if (p > q) {
    panic("Bad expression");
  } else if (p == q) {
    if (tokens[p].type == 'd') {
      return atoi(tokens[p].str);
    } else {
      panic("Invalid token type");
    }
  } else if (tokens[p].type == '(' && tokens[q].type == ')') {
    return eval(p + 1, q - 1);
  } else {
    int op = find_main_operator(p, q);
    int val1 = eval(p, op - 1);
    int val2 = eval(op + 1, q);

    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      default: panic("Invalid operator");
    }
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  *success = true;
  return eval(0, nr_token - 1);
}