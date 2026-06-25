#include "test-resources.h"

#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINE_BREAK_CONTENTS "Description;value1;value 2;\"value;\";\"value 5\"\"\"\n\
This is a csv file;\"Value with line\n\
break\n\
breaking again\";123131;1231312;qweqwe\n\
line 2;value 1;value 2;value 3;value 4"

#define SIMPLE_CONTENTS "cell 0,0;cell 0,1;cell 0,2\n\
cell 1,0;cell 1,1;cell 1,2\n\
cell 2,0;cell 2,1;cell 2,2\n\
cell 3,0;cell 3,1;cell 3,2"

#define COMPLEX_CONTENTS "cell 0,0;value-1    ;qwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnm;value-1\n\
cell 1,0;\"value-2    ;  \";qwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnm;\"value-2;\"\n\
cell 2,0;\"value-3 ; , ;\";\"   \"\"qwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnmqwertyuiopasddfghjklçzxcvbnm\"\"    \";value-3"

#define LINE_END_CONTENTS "cell 0,0;cell 0,1;cell 0,2\r\n\
cell 1,0;cell 1,1;cell 1,2\r\n\
cell 2,0;cell 2,1;cell 2,2\r\n\r\n"

void write_file(char *resource_name, char *contents)
{
    FILE *fp = fopen(resource_name, "w");
    if (fp)
    {
        fprintf(fp, "%s", contents);
        fclose(fp);
    }
    else
    {
        ck_abort_msg("Could not open file! resource_name=%s", resource_name);
    }
}

char *test_resource_get(char *resource_key)
{
    char template[] = "/tmp/csvi-test-XXXXXX";
    int fd = mkstemp(template);
    if (fd == -1)
    {
        ck_abort_msg("Could not create temp file");
    }
    close(fd);

    char *resource_name = strdup(template);
    if (!resource_name)
    {
        ck_abort_msg("Could not allocate temp file path");
    }

    if (strncmp(resource_key, "LINE_BREAK_CONTENTS", 19) == 0)
    {
        write_file(resource_name, LINE_BREAK_CONTENTS);
    }
    else if (strncmp(resource_key, "SIMPLE_CONTENTS", 15) == 0)
    {
        write_file(resource_name, SIMPLE_CONTENTS);
    }
    else if (strncmp(resource_key, "COMPLEX_CONTENTS", 16) == 0)
    {
        write_file(resource_name, COMPLEX_CONTENTS);
    }
    else if (strncmp(resource_key, "LINE_END_CONTENTS", 17) == 0)
    {
        write_file(resource_name, LINE_END_CONTENTS);
    }
    else
    {
        ck_abort_msg("Resource not defined! resource_key=%s", resource_name);
    }
    return resource_name;
}