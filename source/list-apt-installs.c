//
// Copyright 2018 Scott Caudle ( zcotcaudle@gmail.com ) - zcot ( https://github.com/zcot )
// This program is released under GPL-3.0 or any later version.
//

/*                LICENSE NOTICE:
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


/*                     list-apt-installs:
*
*   The program finds apt and apt-get install commands in apt logs that were typed in terminal.
*
*   -traversing /var/log/apt/history.log and archived history logs.
*
*   It produces output that is suitable with the restore functionality from Mint's backup tool.
*
*   You can produce a ".list" file by simply piping the output to a file.
*
*   If you have later removed any of the installed software, the install is still listed anyway.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <argp.h>

const char *argp_program_version = "0.5";
const char *argp_program_bug_address = "<zcotcaudle@gmail.com>";
static char doc[] = "list-apt-installs -- make a list of previous apt and apt-get installs that were made.\
\vDefault output is formatted for use with restore function in Linux Mint Backup tool, \
or installing with `apt-get dselect-upgrade` method.";
static char args_doc[] = "[-o|--output[=] </path/and/or/file/name>]";

// argc/argv flags
static struct argp_option options[] = {
  {"script",  's', 0,       0, "Output as `apt install` script." },
  {"output",   'o', "FILE",  0, "Output to FILE instead of standard output" },
  { 0 }
};

// flags structure
struct arguments
{
  char* arg1;          // for file output
  char** strings;      // for file output
  char* output_file;   // file arg to --output
  int script_building; // boolean --script
};

// parse each flag
static error_t parse_opt(int key, char* arg, struct argp_state* state)
{
  struct arguments* arguments = state->input;

  switch (key)
    {
    case 's':
      arguments->script_building = 1;
      break;
    case 'o':
      arguments->output_file = arg;
      break;

    case ARGP_KEY_ARG:
      arguments->arg1 = arg;
      arguments->strings = &state->argv[state->next];
      state->next = state->argc;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

// argp parser
static struct argp argp = {options, parse_opt, args_doc, doc};

char g_ending[12];
FILE* g_fout = NULL;


// a boolean type of package checking utility
int package_exists(char* name)
{
    FILE* pdpkgquery = NULL;
    char buf[4096];
    char query[4096];
    char pkgname[255];

    //strip specified version if any was provided. for example: apt install hello=2.10-3build1
    char* equal = strchr(name, '=');
    if(equal)
    {
        strncpy(pkgname, name, equal-name);
        pkgname[equal-name] = '\0';
        snprintf(query, 4096, "dpkg-query --show --showformat='${db:Status-Status}\n' %s 2>/dev/null", pkgname);
    }
    else
    snprintf(query, 4096, "dpkg-query --show --showformat='${db:Status-Status}\n' %s 2>/dev/null", name);

    pdpkgquery = popen(query, "r");
    if(!pdpkgquery)
    {
        fprintf(stderr, "lai-fail: popen: %s\n", strerror(errno));
        return 0;
    }
    if(fgets(buf, 4096, pdpkgquery))
        //if(strncmp("installed", buf, 9) == 0)
        // successful output should only be "installed" or "not-installed"
        // other than erroneous condition piped out the stderr which we dont care
        if(buf[0] == 'i')
        {
            pclose(pdpkgquery);
            return 1;
        }

    pclose(pdpkgquery);
    return 0;
}



int parse_line_and_output(char* line)
{
    // line will only be a "Commandline: " type passed to this function
    // and here 'line' starts after the "Commandline: " part
    //example: "Commandline: apt-get install whatever"
    const char aptget[] =  "apt-get "; //length is 8, we start checking at 9(index 8)
    const char aptmint[] = "/usr/bin/apt install "; //length 21, start at index 21
    const char apt[] =     "apt install "; //length 12

    //break up any multi-package calls...
    // ex. apt install screenfetch lolcat timg conky-manager2
    //and check for and ignore extra command flags (anything started with -)
    const char s[2] = " ";
    char* tok;

    //copy the package(s) based on which command was used
    //apt-get can use a flag in front of 'install' and after so check for 'apt-get' then find 'install' anywhere
    if(strncmp(aptget, line, 8) == 0)
    {
        if(strstr("install", &line[8]))
        tok = strtok(&line[8], s);
        else return 0;
    }
    else if(strncmp(apt, line, 12) == 0)
    {
        tok = strtok(&line[12], s);
    }
    else if(strncmp(aptmint, line, 21) == 0)
    {
        tok = strtok(&line[21], s);
    }
    else return 0;

    while(tok)
    {
        //truncate any newline
        char* newline = strchr(tok, '\n');
        if(newline) *newline = '\0';

        //skip the token if it's a command flag or switch
        if(tok[0] != '-')
            if(package_exists(tok))
            fprintf(g_fout, "%s %s\n", tok, g_ending);

        tok = strtok(NULL, s);
    }
    return 0;
}



// a boolean type of file checking utility
int file_exists(char* name)
{
    FILE* f = fopen(name, "r");

    if(f)
    {
        fclose(f);
        return 1;
    }

    return 0;
}



// loop through each log file and parse out the data we are interested in
int main(int argc, char* argv[])
{
    const char prefix[] = "zgrep Commandline: ";
    char* cmd;
    FILE* in;
    char buf[4096];
    char name[128];
    memset(name, '\0', sizeof(name));
    memset(g_ending, '\0', sizeof(g_ending));

    struct arguments arguments;
    // default args
    arguments.script_building = 0; // not building script
    arguments.output_file = "-"; // not using file output

    //take args and adjust
    if(argp_parse(&argp, argc, argv, 0, 0, &arguments))
        exit (0);

    cmd = malloc(sizeof(prefix) + 128 + 1);
    if(!cmd)
    {
        fprintf(stderr, "%s: malloc: %s\n", argv[0], strerror(errno));
        return 1;
    }

    
    //start here...
    strcpy(name, "/var/log/apt/history.log");

    //using stdout or file?
    if(strcmp(arguments.output_file, "-"))
    {
        g_fout = fopen(arguments.output_file, "w");
    }
    else
        g_fout = stdout;

    //building output as script or not?
    if(arguments.script_building)
    {
        fprintf(g_fout, "#/bin/bash\napt install \\\n");
        strcpy(g_ending, "\\");
    }
    else
        strcpy(g_ending, "install");

    int n = 1;
    while(file_exists(name))
    {
        //set grep command and file name
        sprintf(cmd, "%s%s", prefix, name);

        //will only return "Commandline: " lines
        in = popen(cmd, "r");
        if(!in)
        {
            fprintf(stderr, "%s: popen: %s\n", argv[0], strerror(errno));
            return 1;
        }

        while(fgets(buf, 4096, in))
        {
            //parse for data and do output
            //send line starting after "Commandline: " part
            parse_line_and_output(&buf[13]);
        }
        pclose(in);

        // seek the numbered archives
        sprintf(name, "%s%i%s", "/var/log/apt/history.log.", n, ".gz");
        n++;
    }

    fclose(g_fout);// closing stdout when file is not used?
    free(cmd);

    return 0;
}



