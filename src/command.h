/*
    Cifer: Automating classical cipher cracking in C
    Copyright (C) 2008  Daniel Richman & Simrun Basuita

    Cifer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cifer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cifer.  If not, see <http://www.gnu.org/licenses/>.
*/

#define CFSH_PARSE_OK           0
#define CFSH_PARSE_EBAD        -2
#define CFSH_PARSE_EMPTY       -5
#define CFSH_PARSE_QUOTEOPEN   -9

#define CFSH_FUNC_OK            0
#define CFSH_FUNC_NOEXIST      -1

typedef int(*cfsh_command)(int argc, char **argv);

typedef struct 
{
  cfsh_command command;
  int argc;
  char **argv;
} cfsh_execinfo;

int cfsh_parse(char *input, cfsh_execinfo *execinfo);
int cfsh_get_func(char *name, cfsh_command *command);
char *cfsh_get_usage(char *name);
char *cfsh_get_use(char *name);
void cfsh_free_execinfo(cfsh_execinfo *execinfo);

#define cfsh_exec(execinfo) execinfo.command(execinfo.argc, execinfo.argv);

