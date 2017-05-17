// This program demonstrates replacing a substring with another string

#include <stdio.h>
#include <string.h>

char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[4096];
  char *p;

  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;

  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
}


int main()
{
    char string1[] = "tempDir$$";

    char string2[] = "10248";

    puts(replace_str(string1, "$$", string2));

    return 0;
}
