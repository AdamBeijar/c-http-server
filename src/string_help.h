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

bool includes(const char *str, const char *substr);

#endif // INCLUDES_H