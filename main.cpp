#include <iostream>
#include <string.h> 
#include <malloc.h>
#include <stdlib.h>

#define idsize 100
#define tweetsize 255
#define hashsize 200


typedef struct adj{
	char id[idsize];
	adj* next;
}adj;

typedef struct user{
	char id[idsize];
	int color;
	user* next;
	adj* first;	//friend
} user;

int totalusers=0, totalfriends=0, totalwords=0;
int hash[hashsize];
user* userlist;
user* last = NULL;



void useradd(char id[])
{
	userlist=(user*)malloc(sizeof(user));
	strcpy(userlist->id, id);
	userlist->color=0;
	userlist->next=userlist;
	userlist->first=0;
	
	if(last==NULL)
		last=userlist;
	else
	{
		userlist->next=last->next;
		last->next=userlist;
		last=userlist;
	}
}

void friendadd(user* usernode , char a_id[], char b_id[])
{
	user* templast=last->next;
	user* a=NULL;
	user* b=NULL;
	
	usernode=usernode->next;
	last->next=NULL;
	
	while((a==NULL||b==NULL)&&usernode!=NULL)
	{
		if(strcmp(usernode->id, a_id)==0)
			a=usernode;
		if(strcmp(usernode->id, b_id)==0)
			b=usernode;
			
		usernode=usernode->next;
	}
	last->next=templast;
	
	if(a!=NULL)
	{
		adj* a_adj=(adj*)malloc(sizeof(adj));
		
		//adj id
		strcpy(a_adj->id, b_id);
		
		//adj next
		a_adj->next=a->first;
		a->first=a_adj;
	}
	
	if(b!=NULL)
	{
		adj* b_adj=(adj*)malloc(sizeof(adj));
		
		//adj id
		strcpy(b_adj->id, a_id);
		
		//adj next
		b_adj->next=b->first;
		b->first=b_adj;
	}
}



//1.
void readdata()
{	
    FILE *userfile = NULL, *friendfile=NULL, *wordfile=NULL;
    
    //open files
    userfile = fopen( "./user.txt", "r" );
    friendfile = fopen( "./friend.txt", "r" );
    wordfile = fopen( "./word.txt", "r" );
    
    
    if(userfile==NULL||friendfile==NULL||wordfile==NULL)
	{
		printf("파일을 불러올 수 없습니다. 확인해주세요.");
	}
	else
	{
        char strTemp[tweetsize], strTempTemp[idsize];
        char *pStr;
        int i;
		
		strTemp[0]='\n';
		//read userfile
        for(i=0;!feof(userfile);i++)
		{
			fgets(strTemp, sizeof(strTemp), userfile);
            
            // 맨뒤 /n제거
			//if( (pStr=strchr(strTemp, '\n')) != NULL ) *pStr = '\0';
			
            if(i%4<1&&strTemp[0]!='\n')
            {
	            totalusers++;
				useradd(strTemp);
			}
			
            //printf("%d",  strTemp[0]=='\n');
		}
        
		//read friendfile
        for(i=0;!feof(friendfile);i++)
        {
        	strcpy(strTempTemp, strTemp);
            fgets(strTemp, sizeof(strTemp), friendfile);
            
            if(i%3==1&&strTemp[0]!='\n')
			{
            	totalfriends++;
            	friendadd(userlist, strTemp, strTempTemp);
			}
        }

		//read wordfile
        for(i=0;i<hashsize;i++)
        	hash[i]=0;
        	
        for(i=0;!feof(wordfile);i++)
        {
            fgets(strTemp, sizeof(strTemp), wordfile);
            
            if(i%4<1&&strTemp[0]!='\n')
            {
            	hash[atoi(strTemp)%hashsize]++;
            	totalwords++;
			}
        }        
        
        printf("Total users: %d\n", totalusers);
        printf("Total friendship records: %d\n", totalfriends);
        printf("Total tweets: %d\n", totalwords);
	}
	
	//close files
	fclose(userfile);
	fclose(friendfile);
	fclose(wordfile);
}

//2.
void statistics(){
	
	/*
		Average number of friends: xxx
		Minimum friends: xxx
		Maximum number of friends: xxx
		Average tweets per user: xxx
		Minium tweets per user: xxx
		Maximum tweets per user: xxx
	*/
	
	int minfriend=totalfriends, maxfriend=0, mintweet, maxtweet, tmp=0;
	adj* adjtmp;
	
	if(userlist==NULL)
	{
		printf("DB가 없습니다. 0을 눌러 DB를 읽어오거나, DB 상태를 확인하세요.");
	}
	else
	{
	
		do
		{
			adjtmp=userlist->first;
			
			while(adjtmp!=NULL)
			{
				tmp++;
				adjtmp=adjtmp->next;
			}
			if(tmp<minfriend)
				minfriend=tmp;
			if(tmp>maxfriend)
				maxfriend=tmp;
			
			tmp=0;
			userlist=userlist->next;
		}while(userlist!=last);
		
		printf("Average number of friends: %d\n", totalfriends * 2 / totalusers);
		printf("Minimum friends: %d\n", minfriend);
		printf("Maximum number of friends: %d\n", maxfriend);
		printf("Average tweets per user: %d\n", totalwords / totalusers);
		printf("Minimum tweets per user: %d\n", -1);
		printf("Maximum tweets per user: %d\n", -1);
	}
}

//3.


int main(int argc, char** argv)
{
	int select;
	readdata();
	statistics();
	
	
	
	printf("0. Read data files\n1. display statistics\n2. Top 5 most tweeted words\n3. Top 5 most tweeted users\n4. Find users who tweeted a word (e.g., ’연세대’)\n5. Find all people who are friends of the above users\n6. Delete all mentions of a word\n7. Delete all users who mentioned a word\n8. Find strongly connected components\n9. Find shortest path from a given user\n99. Quit\nSelect Menu:");
	//scanf("%d", &select);
	printf("%d", select);
	return 0;
}
