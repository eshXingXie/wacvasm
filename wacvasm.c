#include "wacva.h"
#include "correction.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>

#define is_blank(c) (c==' '||c=='\n'||c=='\t')
#define npos 0

FILE*ifp;
FILE*ofp;
p_t pos = 0;

typedef char*str;

struct
lst_t{
	str s;
	struct lst_t*nxt;
};

struct
tag_lst_t{
	str s;
	p_t addr;
	struct tag_lst_t*nxt;
};

struct lst_t*token_lst_begin;
struct lst_t*token_lst_pos;
struct tag_lst_t*tag_lst_begin;
struct tag_lst_t*tag_lst_pos;

int64_t htoi(str s){
	int64_t ret = 0;
	char flg = 1;
	for (int i = 0;s[i];i++){
		if(s[i]=='-') flg*=-1;
		else if(s[i]>='0'&&s[i]<='9'){
			ret<<=4;
			ret+=s[i]-'0';
		}else if(s[i]>='a'&&s[i]<='f'){
			ret<<=4;
			ret+=s[i]-'a'+10;
		}
	}
	return ret;
}

void 
cons_token(FILE*fp,str fn){
	fseek(fp,0,SEEK_SET);
	char c;
	short l=0;
	str s=(str)malloc(sizeof(char)*64);
	while((c=tolower(fgetc(fp)))!=EOF){
		if(is_blank(c)){
			if(!l) continue;
			s[l]=0;
			if(s[0]=='!'){
				FILE*include_fp = fopen(s+1,"r");
				cons_token(include_fp,s+1);
			}
			else{
				token_lst_pos->nxt=(struct lst_t*)malloc(sizeof(struct lst_t));
				token_lst_pos=token_lst_pos->nxt;
				token_lst_pos->s=(str)malloc(sizeof(char)*64);
				strcpy(token_lst_pos->s,s);
				token_lst_pos->nxt=NULL;
			}
			l=0;
		}
		else{
			s[l++]=c;
		}
	}
	if(!l) return;
	s[l]=0;
	if(s[0]=='!'){
		FILE*include_fp = fopen(s+1,"r");
		cons_token(include_fp,s+1);
	}
	else{
		token_lst_pos->nxt=(struct lst_t*)malloc(sizeof(struct lst_t));
		token_lst_pos=token_lst_pos->nxt;
		token_lst_pos->s=(str)malloc(sizeof(char)*64);
		strcpy(token_lst_pos->s,s);
		token_lst_pos->nxt=NULL;
	}
	l=0;
}

struct tag_lst_t*
search_tag(str s){
	struct tag_lst_t*search_pos=tag_lst_begin;
	while(search_pos){
		if(strcmp(s,search_pos->s)==0){
			return search_pos;
		}
		search_pos=search_pos->nxt;
	}
	return NULL;
}

void
write_file(){
	token_lst_pos = token_lst_begin->nxt;
	char is_str=0;
	while(token_lst_pos){
		if(token_lst_pos->s[0]==':'){
			struct tag_lst_t*tag_pos=search_tag(token_lst_pos->s+1);
			if(tag_pos==NULL){
				tag_lst_pos->nxt=(struct tag_lst_t*)malloc(sizeof(struct tag_lst_t));
				tag_lst_pos=tag_lst_pos->nxt;
				tag_lst_pos->s=(str)malloc(sizeof(char)*64);
				strcpy(tag_lst_pos->s,token_lst_pos->s+1);
				tag_lst_pos->addr=pos;
				tag_lst_pos->nxt=NULL;
			}
			else{
				tag_pos->addr=pos;
			}
		}
		else if(token_lst_pos->s[0]=='@'){
			struct tag_lst_t*tag_pos=search_tag(token_lst_pos->s+1);
			if(tag_pos==NULL){
				printf("tag \"%s\" not found, replaced with zero\n",token_lst_pos->s+1);
				fwrite(&(tag_lst_begin->addr),sizeof(uint32_t),1,ofp);
				pos+=ulng_step;
			}
		}
		else if(token_lst_pos->s[0]=='\"'){
			fwrite(token_lst_pos->s+1,sizeof(char),strlen(token_lst_pos->s+1),ofp);
			pos+=strlen(token_lst_pos->s+1)*ubyt_step;
		}
		else if(token_lst_pos->s[0]=='h'){	//ill improve it later
			if(token_lst_pos->s[1]=='u'){
				switch (token_lst_pos->s[2]) {
				case 'b':
					*((uint8_t*)(token_lst_pos->s))=(uint8_t)htoi(token_lst_pos->s+3);
					fwrite(token_lst_pos->s,ubyt_step,1,ofp);
					pos+=ubyt_step;
					break;
				case 'w':
					*((uint16_t*)(token_lst_pos->s))=(uint16_t)htoi(token_lst_pos->s+3);
					fwrite(token_lst_pos->s,uwrd_step,1,ofp);
					pos+=uwrd_step;
					break;
				case 'l':
					*((uint32_t*)(token_lst_pos->s))=(uint32_t)htoi(token_lst_pos->s+3);
					fwrite(token_lst_pos->s,ulng_step,1,ofp);
					pos+=ulng_step;
					break;
				}
			}
			else{
				switch (token_lst_pos->s[2]) {
				case 'b':
					*((int8_t*)(token_lst_pos->s))=(int8_t)htoi(token_lst_pos->s+3);
					fwrite(token_lst_pos->s,ubyt_step,1,ofp);
					pos+=ubyt_step;
					break;
				case 'w':
					*((int16_t*)(token_lst_pos->s))=(int16_t)htoi(token_lst_pos->s+3);
					fwrite(token_lst_pos->s,uwrd_step,1,ofp);
					pos+=uwrd_step;
					break;
				case 'l':
					*((int32_t*)(token_lst_pos->s))=(int32_t)htoi(token_lst_pos->s+3);
					fwrite(token_lst_pos->s,ulng_step,1,ofp);
					pos+=ulng_step;
					break;
				}
			}
		}
		else if(token_lst_pos->s[0]=='d'){
			if(token_lst_pos->s[1]=='u'){
				switch (token_lst_pos->s[2]) {
				case 'b':
					*((uint8_t*)(token_lst_pos->s))=(uint8_t)atoll(token_lst_pos->s+3);
					fwrite(token_lst_pos->s,ubyt_step,1,ofp);
					pos+=ubyt_step;
					break;
				case 'w':
					*((uint16_t*)(token_lst_pos->s))=(uint16_t)atoll(token_lst_pos->s+3);
					fwrite(token_lst_pos->s,uwrd_step,1,ofp);
					pos+=uwrd_step;
					break;
				case 'l':
					*((uint32_t*)(token_lst_pos->s))=(uint32_t)atoll(token_lst_pos->s+3);
					fwrite(token_lst_pos->s,ulng_step,1,ofp);
					pos+=ulng_step;
					break;
				}
			}
			else{
				switch (token_lst_pos->s[2]) {
				case 'b':
					*((int8_t*)(token_lst_pos->s))=(int8_t)atoll(token_lst_pos->s+3);
					fwrite(token_lst_pos->s,ubyt_step,1,ofp);
					pos+=ubyt_step;
					break;
				case 'w':
					*((int16_t*)(token_lst_pos->s))=(int16_t)atoll(token_lst_pos->s+3);
					fwrite(token_lst_pos->s,uwrd_step,1,ofp);
					pos+=uwrd_step;
					break;
				case 'l':
					*((int32_t*)(token_lst_pos->s))=(int32_t)atoll(token_lst_pos->s+3);
					fwrite(token_lst_pos->s,ulng_step,1,ofp);
					pos+=ulng_step;
					break;
				}
			}
		}
		token_lst_pos=token_lst_pos->nxt;
	}
}

void 
free_token_lst(){
	token_lst_pos = token_lst_begin;
	do{
		free(token_lst_pos->s);
		token_lst_begin=token_lst_pos;
		token_lst_pos=token_lst_pos->nxt;
		free(token_lst_begin);
	}while(token_lst_pos);
}

int 
main(int argc,char*argv[]){
	ifp = fopen(argv[1],"r");
	token_lst_begin=(struct lst_t*)malloc(sizeof(struct lst_t));
	token_lst_begin->s=(str)malloc(4*sizeof(char));
	strcpy(token_lst_begin->s,"abc");
	token_lst_begin->nxt=NULL;
	token_lst_pos = token_lst_begin;
	tag_lst_begin=(struct tag_lst_t*)malloc(sizeof(struct tag_lst_t));
	tag_lst_begin->s=(str)malloc(5*sizeof(char));
	strcpy(tag_lst_begin->s,"");
	tag_lst_begin->nxt=NULL;
	tag_lst_begin->addr=0x0;
	tag_lst_pos = tag_lst_begin;
	cons_token(ifp,argv[1]);
	
	free_token_lst();
	fcloseall();
	return 0;
}
