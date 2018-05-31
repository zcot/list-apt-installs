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

int parse_line(char* line)
{
    const char aptget[] = "Commandline: apt-get install ";
    const char apt[] = "Commandline: /usr/bin/apt ins";
    char part[30];
    memset(part, '\0', sizeof(part));
    char buffer[2048];
    memset(buffer, '\0', sizeof(buffer));

    strncpy(part, line, 29);

    //find each relevant offset length and copy good info skipping the first command parts
    if(strncmp(aptget, part, 29) == 0)
        strcpy(buffer, &line[29]);

    if(strncmp(apt, part, 29) == 0)
        strcpy(buffer, &line[34]);

    //else buffer stays null string and we fall out below

    //break up any multi-package calls...
    // ex. apt install screenfetch lolcat timg conky-manager2
    //and check for and ignore extra command flags (anything started with -)
    const char s[2] = " ";
    char* tok;

    tok = strtok(buffer, s);

    while(tok)
    {
        //truncate any newline
        char* newline = strchr(tok, '\n');
        if(newline) *newline = '\0';

        //skip the token if it's a command flag or switch
        if(tok[0] != '-')
            printf("%s install\n", tok);

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


    cmd = malloc(sizeof(prefix) + 128 + 1);
    if(!cmd)
    {
        fprintf(stderr, "%s: malloc: %s\n", argv[0], strerror(errno));
        return 1;
    }

    
    //start here...
    strcpy(name, "/var/log/apt/history.log");

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
            //parse for our data
            parse_line(buf);
        }

        // seek the numbered archives
        sprintf(name, "%s%i%s", "/var/log/apt/history.log.", n, ".gz");
        n++;
    }

    free(cmd);

    return 0;
}



