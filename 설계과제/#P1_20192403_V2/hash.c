#include "header.h"

#define BUFSIZE	1024*16

char *do_fp_md(FILE *f)
{
	MD5_CTX c;
	unsigned char md[MD5_DIGEST_LENGTH];
	int fd;
	int i;
	static unsigned char buf[BUFSIZE];
    char *ret;
    char temp[5];

	fd=fileno(f);
	MD5_Init(&c);
	for (;;)
	{
		i=read(fd,buf,BUFSIZE);
		if (i <= 0) break;
		MD5_Update(&c,buf,(unsigned long)i);
	}
	MD5_Final(&(md[0]),&c);
    ret = (char *)calloc(40,sizeof(char));
    for (i=0; i<16; i++) {
        sprintf(temp,"%02x",md[i]);
        strcat(ret,temp);
    }
    return ret;
}

char *do_fp_sha(FILE *f)
{
	SHA_CTX c;
	unsigned char md[SHA_DIGEST_LENGTH];
	int fd;
	int i;
	unsigned char buf[BUFSIZE];
    char *ret;
    char temp[5];

	fd=fileno(f);
	SHA1_Init(&c);
	for (;;)
	{
		i=read(fd,buf,BUFSIZE);
		if (i <= 0) break;
		SHA1_Update(&c,buf,(unsigned long)i);
	}
	SHA1_Final(&(md[0]),&c);
	ret = (char *)calloc(40,sizeof(char));
    for (i=0; i<16; i++) {
        sprintf(temp,"%02x",md[i]);
        strcat(ret,temp);
    }
    return ret;
}

int isSameFile(char *originPath, char *dupPath, char *mode) {
    FILE *originFd, *dupFd;
    if((originFd = fopen(originPath,"r+")) == NULL) {
        return -1;
    }
    if((dupFd = fopen(dupPath, "r+")) == NULL) {
        fclose(originFd);
        return -1;
    }
    if(strcmp(mode,"md5"))
        if(strcmp(do_fp_md(originFd),do_fp_md(dupFd))==0)
            return 1;
    if(strcmp(mode,"sha1"))
        if(strcmp(do_fp_sha(originFd),do_fp_sha(dupFd))==0)
            return 1;
    return 0;
}