#ifndef __MTURTLECONSOLE_H_
#define __MTURTLECONSOLE_H_

struct var_list
{
    char* name;
    double val;
    struct var_list* next;
};

unsigned int strhash(char* str);

/*char* strdup(char* str);*/

struct var_list* var_get(char* name);

struct var_list* var_set(char* name, double var);

bool parseLine(char *str);

#endif
