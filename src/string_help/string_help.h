#ifndef STARTSWITH_H
#define STARTSWITH_H

int startsWith(const char *str, const char *prefix);

#endif

#ifndef SPLIT_H
#define SPLIT_H

char **split(char *str, const char *delim, int *count);

#endif // SPLIT_H

#ifndef INCLUDES_H
#define INCLUDES_H

int includes(const char *str, const char *substr);

#endif // INCLUDES_H

#ifndef REMOVESUBSTRING_H
#define REMOVESUBSTRING_H

void *removeSubString(char *str, char *subStr, int count);

#endif // REMOVESUBSTRING_H

#ifndef REMOVEALLSUBSTRING_H
#define REMOVEALLSUBSTRING_H

void *removeAllSubString(char *str, char *subStr);

#endif // REMOVEALLSUBSTRING_H

#ifndef STRIP_H
#define STRIP_H

char *strip(char *str);

#endif // STRIP_H