#include "util.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>


int _seedRandomNumberGenerator=0;

/**
 * @brief seed random number generator if not alread done.
 */
void seedRandomNumberGeneratorIfNeeded() {
    if (!_seedRandomNumberGenerator) {
        srand ( time(NULL) );
        _seedRandomNumberGenerator=1;
    }
}

/**
 * @brief writes a random name of size n.
 * @param name an allocated buffer of size n+1.
 * @param n size of the random name.
 */
void writeRandomName(char*name,int n) {
    int i=0;
    seedRandomNumberGeneratorIfNeeded();
    for (i=0;i<n;++i) {
        unsigned char j=rand()%52;
        name[i]=(j<26)?'A'+j:'a'+(j-26);
    }
    name[n]=0;
    if (n>4) {
        name[0]='c';
        name[1]='n';
        name[2]='n';
        name[3]='_';
    }
}

/**
 * @brief creates a random directory
 * @param name an allocated buffer of size n+1.
 * @param n size of the created dir
 */
void createTmpDir(char*name,int n) {
    if (n<10)
        ERROR("n should be at least 10","");
    name[0]='/';
    name[1]='t';
    name[2]='m';
    name[3]='p';
    name[4]='/';
    writeRandomName(&name[5],n);
    struct stat st = {0};
    if (stat(name, &st) == -1) {
        mkdir(name, 0700);
    } else {
        ERROR("Unlucky name already exists: ",name);
    }
}

/**
 * @brief allocates a new string.
 * @param s string to copy.
 */
char * stringCopy(char * s) {
    int l = strlen(s)+1;
    char * answer = (char*)malloc(l);
    memcpy(answer,s,l);
    return answer;
}
/**
 * @brief allocates a new string which is a
 *        concatenated with b.
 * @param a a string 
 * @param b a string
 * @return concatenation of a and b
 */
char * stringAdd(char * a,char *b) {
    int l = strlen(a)+strlen(b)+1;
    char * answer = (char*)malloc(l);
    snprintf(answer,l,"%s%s",a,b);
    return answer;
}

