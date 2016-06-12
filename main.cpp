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

typedef struct tweetadj{
	char id[idsize];
	char content[tweetsize];
	tweetadj* prev;
	tweetadj* next;
	tweetadj* nexthash;
}tweetadj;

typedef struct user{
	char id[idsize];
	int color;
	user* nexttmp;	//for function 5.
	user* next;
	adj* first;	//friend
	tweetadj* tweet;
} user;

char userfilename[] = "./user.txt", friendfilename[] = "./friend.txt", wordfilename[] = "./word.txt", wordfiletempname[] = "./word_temp.txt";
int totalusers=0, totalfriends=0, totalwords=0;
tweetadj* hash[hashsize];
user* userlist;
user* last = NULL;
user* userfind = NULL;		//for function 5
void presskey(void);

void useradd(char id[])
{
	userlist=(user*)malloc(sizeof(user));
	
	//init user
	strcpy(userlist->id, id);
	userlist->color=0;
	userlist->next=userlist;
	userlist->first=0;
	userlist->tweet=NULL;
	userlist->nexttmp=NULL;
	
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
	
	usernode=last->next;
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



//0.
void readdata()
{
    system("cls");
    
    FILE *userfile = NULL, *friendfile=NULL, *wordfile=NULL;
    
    //open files
    userfile = fopen(userfilename, "r" );
    friendfile = fopen(friendfilename, "r" );
    wordfile = fopen(wordfilename, "r" );
    
    
    if(userfile==NULL||friendfile==NULL||wordfile==NULL)
	{
		printf("파일을 불러올 수 없습니다. 확인해주세요.");
		presskey();
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
            
            if(i%4<1&&strTemp[0]!='\n')
            {
				strTemp[strlen(strTemp) - 1] = '\0';
				useradd(strTemp);
				
	            totalusers++;
			}
		}
        
		//read friendfile
        for(i=0;!feof(friendfile);i++)
        {
        	strcpy(strTempTemp, strTemp);
            fgets(strTemp, sizeof(strTemp), friendfile);
            
            if(i%3==1&&strTemp[0]!='\n')
			{
            	strTemp[strlen(strTemp) - 1] = '\0';
				strTempTemp[strlen(strTempTemp) - 1] = '\0';
				friendadd(userlist, strTemp, strTempTemp);
            	
				totalfriends++;
			}
        }

		//read wordfile
        for(i=0;i<hashsize;i++)
        	hash[i]=NULL;
        	
        for(i=0;!feof(wordfile);i++)
        {
        	tweetadj* memtmp;
        	
            fgets(strTemp, sizeof(strTemp), wordfile);
            
            if(i%4==0&&strTemp[0]!='\n')
            {
            	memtmp=(tweetadj*)malloc(sizeof(tweetadj));
				strTemp[strlen(strTemp) - 1] = '\0';
				
				//init_tweetadj
				strcpy(memtmp->id, strTemp);
				memtmp->prev=NULL;
				memtmp->next=NULL;
				memtmp->nexthash=NULL;
				
				//link with userlink
				userlist=last;

				do
				{
					if(strcmp(userlist->id, strTemp)==0)
					{
						memtmp->next=userlist->tweet;
						userlist->tweet=memtmp;
						if(memtmp->next!=NULL)
							memtmp->next->prev=memtmp;
						break;
					}
					userlist=userlist->next;
				}while(userlist!=last);
				
			}
            else if(i%4==2&&strTemp[0]!='\n')
            {
				//init_tweetadj
				strTemp[strlen(strTemp) - 1] = '\0';
				strcpy(memtmp->content, strTemp);
				
				//link with hash array
				memtmp->nexthash=hash[atoi(strTemp)%hashsize];
				hash[atoi(strTemp)%hashsize]=memtmp;
				
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
	
	presskey();
}

//1.
void statistics()
{
    system("cls");
	
	/*
		Average number of friends: xxx
		Minimum friends: xxx
		Maximum number of friends: xxx
		Average tweets per user: xxx
		Minium tweets per user: xxx
		Maximum tweets per user: xxx
	*/
	
	
	//min/max friends
	if(last==NULL)
	{
		printf("DB가 없습니다. 0을 눌러 DB를 읽어오거나, DB 상태를 확인하세요.\n");
		presskey();
	}
	else
	{
		int minfriend=totalfriends, maxfriend=0, mintweet=totalwords, maxtweet=0, friendtmp=0, tweettmp=0;
		adj* friendadjtmp;
		tweetadj* tweetadjtmp;
		
		userlist=last;
		do
		{
			friendadjtmp=userlist->first;
			
			while(friendadjtmp!=NULL)
			{
				friendtmp++;
				friendadjtmp=friendadjtmp->next;
			}
			if(friendtmp<minfriend)
				minfriend=friendtmp;
			if(friendtmp>maxfriend)
				maxfriend=friendtmp;
			
			tweetadjtmp=userlist->tweet;
			
			while(tweetadjtmp!=NULL)
			{
				tweettmp++;
				tweetadjtmp=tweetadjtmp->next;
			}
			if(tweettmp<mintweet)
				mintweet=tweettmp;
			if(tweettmp>maxtweet)
				maxtweet=tweettmp;
			
			friendtmp=0;
			tweettmp=0;
			userlist=userlist->next;
		}while(userlist!=last);

	//min-max tweets
		
		printf("Average number of friends: %d\n", totalfriends * 2 / totalusers);
		printf("Minimum friends: %d\n", minfriend);
		printf("Maximum number of friends: %d\n", maxfriend);
		printf("Average tweets per user: %d\n", totalwords / totalusers);
		printf("Minimum tweets per user: %d\n", mintweet);
		printf("Maximum tweets per user: %d\n", maxtweet);
		
		presskey();
	}
}

//2.

//3.
void topuser()
{
    system("cls");
    
	if(last==NULL)
	{
		printf("DB가 없습니다. 0을 눌러 DB를 읽어오거나, DB 상태를 확인하세요.\n");
		presskey();
	}
	else
	{
		int maxtweet[5]={0, 0, 0, 0, 0}, tweettmp=0;
		char* maxuser[5]={NULL, NULL, NULL, NULL, NULL};
		tweetadj* tweetadjtmp;
		
		userlist=last;
		do
		{
			tweetadjtmp = userlist->tweet;
			
			while(tweetadjtmp!=NULL)
			{
				tweettmp++;
				tweetadjtmp=tweetadjtmp->next;
			}
			for(int i=4;i>=0;i--)
			{
				if(tweettmp>maxtweet[i])
				{
					if(i<4)
					{
						maxtweet[i+1]=maxtweet[i];
						maxuser[i+1]=maxuser[i];
					}
					maxtweet[i]=tweettmp;
					maxuser[i]=userlist->id;
				}
			}
			
			tweettmp=0;
			userlist=userlist->next;
		}while(userlist!=last);
		
		for(int i=0; i<5; i++)
		{
			printf("%d위 : %s (%d개)\n", i+1, maxuser[i], maxtweet[i]);
		}
		
		presskey();
	}
}

//4.
void findworduser()
{
    system("cls");
    
	if(last==NULL)
	{
		printf("DB가 없습니다. 0을 눌러 DB를 읽어오거나, DB 상태를 확인하세요.\n");
		presskey();
	}
	else
	{
		char tweet[tweetsize];
		tweetadj* tweettmp;
		
		printf("단어를 입력하세요 : "); 
		scanf("%s", tweet);
		
		printf("'%s'를 트윗한 사용자 : ", tweet);
		tweettmp=hash[atoi(tweet)%hashsize];
		while(tweettmp!=NULL)
		{
			if(strcmp(tweet, tweettmp->content)==0)
			{
				userlist=last;
				do{
					if(strcmp(userlist->id, tweettmp->id)==0)
					{
						userlist->nexttmp=userfind;
						userfind=userlist;
						break;
					}
					userlist=userlist->next;
				}while(userlist!=last);
				
				printf("%s ", tweettmp->id);
			}
			tweettmp=tweettmp->nexthash;
		}
		
		presskey();
	}
}

//5.
void findworduserfriend()
{
    system("cls");
    
	if(last==NULL)
	{
		printf("DB가 없습니다. 0을 눌러 DB를 읽어오거나, DB 상태를 확인하세요.\n");
		presskey();
	}
	else if(userfind==NULL)
	{
		printf("4번 기능이 제대로 실행되지 않았습니다.\n");
		presskey();
	}
	else
	{
		user* usertmp=userfind;
		while(userfind!=NULL)
		{
			adj* friendtmp = userfind->first;
			
			printf("사용자 '%s'의 친구: ", userfind->id);
			
			while(friendtmp!=NULL)
			{
				printf("%s ", friendtmp->id);
				friendtmp=friendtmp->next;
			}
			
			printf("\n");
			userfind=userfind->nexttmp;
		}
		userfind=usertmp;
		
		presskey();
	}
}

//6.
void deletemention(char tweet[tweetsize])
{
    system("cls");
    
	if(last==NULL)
	{
		printf("DB가 없습니다. 0을 눌러 DB를 읽어오거나, DB 상태를 확인하세요.\n");
		presskey();
	}
	else
	{
		tweetadj* tweetadjtmp;
		tweetadj* tweetadjprev=NULL;
		
		FILE *wordfile1=NULL;
		wordfile1 = fopen(wordfilename, "r+" );
		
	    if(wordfile1==NULL)
		{
			printf("word.txt파일을 불러올 수 없습니다. 확인해주세요.");
			presskey(); 
		}
		else
		{
			//txt edit
    		char strTemp1[tweetsize], strTemp2[tweetsize], strTemp3[tweetsize], strTemp4[tweetsize];    
			FILE *wordfile2 = fopen(wordfiletempname, "w");
			
	        for(int i=0;!feof(wordfile1);i++)
			{
				fgets(strTemp1, sizeof(strTemp1), wordfile1);
				fgets(strTemp2, sizeof(strTemp2), wordfile1);
				fgets(strTemp3, sizeof(strTemp3), wordfile1);
				fgets(strTemp4, sizeof(strTemp4), wordfile1);
				
				strTemp3[strlen(strTemp3) - 1] = '\0';
				if(strcmp(strTemp3, tweet)!=0&&!feof(wordfile1))
				{
           			fputs(strTemp1, wordfile2);
           			fputs(strTemp2, wordfile2);
           			fputs(strTemp3, wordfile2);
           			fputs("\n\n", wordfile2);
				}
			}

		    fclose(wordfile1);
		    fclose(wordfile2);
		    
		    remove(wordfilename);
		    rename(wordfiletempname, wordfilename);
			
			
			tweetadjtmp=hash[atoi(tweet)%hashsize];
			
			while(tweetadjtmp!=NULL)
			{
				if(strcmp(tweetadjtmp->content, tweet)==0)
				{
					//structure edit:hash
					tweetadjprev->nexthash=tweetadjtmp->nexthash;
					
					//structure edit:user
					if(tweetadjtmp->prev!=NULL)
						tweetadjtmp->prev->next=tweetadjtmp->next;
					else
					{
						userlist=last;
						do
						{
							if(strcmp(tweetadjtmp->id, userlist->id)==0)
								userlist->tweet=tweetadjtmp->next;
							
							userlist=userlist->next;
						}while(userlist!=last);
					}
		
					if(tweetadjtmp->next!=NULL) 
						tweetadjtmp->next->prev=tweetadjtmp->prev;
					
					free(tweetadjtmp);
					
				}
				tweetadjprev=tweetadjtmp;
				tweetadjtmp=tweetadjtmp->nexthash;
			}
			
			presskey();
		}
	}
}

//7
void deletementionuser()
{
    system("cls");
    
	if(last==NULL)
	{
		printf("DB가 없습니다. 0을 눌러 DB를 읽어오거나, DB 상태를 확인하세요.\n");
		presskey();
	}
	else
	{
		char tweet[tweetsize];
		tweetadj* tweetadjtmp;
		tweetadj* tweetadjprev=NULL;
		
		scanf("%s", tweet);
		
		FILE *wordfile1=NULL;
		wordfile1 = fopen(wordfilename, "r+" );
		
		fclose(wordfile1);
		presskey();
	}
}

int menu_display(void)
{
    int select;
    system("cls");
    
	printf("0. Read data files\n");
	printf("1. display statistics\n");
	printf("2. Top 5 most tweeted words\n");
	printf("3. Top 5 most tweeted users\n");
	printf("4. Find users who tweeted a word (e.g., ’연세대’)\n");
	printf("5. Find all people who are friends of the above users\n");
	printf("6. Delete all mentions of a word\n");
	printf("7. Delete all users who mentioned a word\n");
	printf("8. Find strongly connected components\n");
	printf("9. Find shortest path from a given user\n");
	printf("99. Quit\nSelect Menu:");
	
    scanf("%d", &select);
    return select;
}

void presskey(void)
{
    char select;
    fflush(stdin);
    printf("\n\n");
    printf("Enter를 누르면 메인 메뉴로 돌아갑니다.");
    scanf("%c", &select);
}


int main(int argc, char** argv)
{
	int select;
	char tweet[tweetsize];
	
	while((select=menu_display())!=99)
	{
		switch(select)
		{
			case 0:
				readdata();
				break;
			
			case 1:
				statistics();
				break;
				
			case 2:
				break;
				
			case 3:
				topuser();
				break;
				
			case 4:
				findworduser();
				break;
				
			case 5://같은 단어를 같은 사용자가 말했을경우... 
				findworduserfriend();
				break;
				
			case 6:
				scanf("%s", tweet);
				deletemention(tweet);
				break;
				
			case 7:
				break;
				
			case 8:
				break;
				
			case 9:
				break;
				
			case 99:
				printf("안녕히 가십시오.");
				exit(0);
				break;
				
			default:
				printf("잘못된 메뉴 번호입니다.\n제대로 된 메뉴 번호를 입력해주세요.\n");
				break;
		}
	}
}
