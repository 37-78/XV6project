#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int matchhere(char*, char*);
int matchstar(int, char*, char*);
/*
 * match ^hh hhab: ^——以re开头的text
 * match hh$ abhh: $——以re结尾的text
 * match h.h ahbhc: .——任意占位字符
 * match h*h ahhhb: *——前一个字符0～倍
 * */
int match(char *re, char *text) {
	if(re[0] == '^')
		return matchhere(re+1, text);
	do{  // must look at empty string
		if(matchhere(re, text))
		return 1;
	}while(*text++ != '\0');
	return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text) {
	if(re[0] == '\0')
		return 1;
	if(re[1] == '*')
		return matchstar(re[0], re+2, text);
	if(re[0] == '$' && re[1] == '\0')
		return *text == '\0';
	if(*text!='\0' && (re[0]=='.' || re[0]==*text))
		return matchhere(re+1, text+1);
	return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
	do{  // a * matches zero or more instances
		if(matchhere(re, text))
			return 1;
	}while(*text!='\0' && (*text++==c || c=='.'));
	return 0;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(1, "Usage: match [re] [text]\n");
    exit(1);
  }
  fprintf(1, "match [%s] in [%s]: %d\n", argv[1], argv[2], match(argv[1], argv[2]));
  exit(0);
}
