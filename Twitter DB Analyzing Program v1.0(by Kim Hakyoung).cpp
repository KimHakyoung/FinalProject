#include <iostream>
#include <string.h> 
#include <malloc.h>
#include <stdlib.h>

#define idsize 255
#define tweetsize 255
#define hashsize 199
#define white 0
#define gray  1
#define black 2
#define min(a,b) \
	({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a < _b ? _a : _b; })
#define max(a,b) \
	({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a > _b ? _a : _b; })

typedef struct adj{
	struct user* id;		//상대방
	struct user* master;	//본인(친구+1)
	adj* prev;
	adj* next;
}adj;

typedef struct tweetadj{
	struct user* id;
	char content[tweetsize];
	int color;
	tweetadj* prev;
	tweetadj* next;
	tweetadj* prevhash;
	tweetadj* nexthash;
}tweetadj;

typedef struct user{
	char id[idsize];
	int color;
	int d, f;
	int weight;
	user* nexttmp;	//for function 5.
	user* prev;
	user* next;
	user* parent;
	adj* first;	//friend
	tweetadj* tweet;
} user;

typedef struct stack {
	user* stackuser;
	struct stack* next;
} stack;

int totalusers=0, totalfriends=0, totalwords=0;
int maxscc[]={0, 0, 0, 0, 0};
user* maxsccuser[]={NULL, NULL, NULL, NULL, NULL};
stack* stacklist=NULL;

void push(user* target)
{
	stack* stacknew = (stack*)malloc(sizeof(stack));
	
	stacknew->stackuser=target;
	
	stacknew->next=stacklist;
	stacklist=stacknew;
}

user* pop()
{
	stack* stacktmp=NULL;
	user* usertmp=NULL;

	stacktmp=stacklist;
	stacklist=stacklist->next;
		
	usertmp=stacktmp->stackuser;

	free(stacktmp);
	stacktmp=NULL;

	return usertmp;
}

void enqueue(user* target)
{
	stack* stacknew = (stack*)malloc(sizeof(stack));
	
	//set stack->stackuser
	stacknew->stackuser=target;
	
	if(stacklist!=NULL)
	{
		stack* stacktmp = stacklist;
		while(stacktmp->next!=NULL&&target->d>stacktmp->next->stackuser->d)
			stacktmp=stacktmp->next;
			
		//set stack->next
		stacknew->next=stacktmp->next;
		stacktmp->next=stacknew;
	}
	else
	{
		//set stack->next
		stacknew->next=NULL;
		stacklist=stacknew;
	}
	
}

user* dequeue()
{
	return pop();
}

int dfs_time = 0;
user* last=NULL;
user* userlist;
user* userfind = NULL;			//for function 5
char tweetfind[tweetsize]="";		//for function 5

void presskey(void);


char userfilename[] = "./user.txt", friendfilename[] = "./friend.txt", wordfilename[] = "./word.txt";
char userfiletempname[] = "./user_temp.txt", friendfiletempname[] = "./friend_temp.txt", wordfiletempname[] = "./word_temp.txt";
tweetadj* hash[hashsize];

void useradd(char id[])
{
	userlist=(user*)malloc(sizeof(user));

	//init user
	strcpy(userlist->id, id);
	userlist->color=white;
	userlist->prev=NULL;
	userlist->next=userlist;
	userlist->first=0;
	userlist->tweet=NULL;
	userlist->nexttmp=NULL;
	userlist->d=0;
	userlist->f=0;
	userlist->parent=NULL;
	userlist->weight=0;
	
	if(last==NULL)
	{
		last=userlist;
		userlist->prev=userlist;
	}
	else
	{
		userlist->next=last->next;
		if(last->next!=NULL)
			last->next->prev=userlist;
		last->next=userlist;
		userlist->prev=last;
		last=userlist;
	}
}

void friendadd(user* usernode , char a_id[], char b_id[])
{
	//a의 친구 b, a->b
	user* a=NULL;
	user* b=NULL;
	
	usernode=last;
	do
	{
		if(strcmp(usernode->id, a_id)==0&&a==NULL)
			a=usernode;
		else if(strcmp(usernode->id, b_id)==0&&b==NULL)
			b=usernode;

		usernode=usernode->next;
	}while((a==NULL||b==NULL)&&usernode!=last);
	
	if(a==NULL||b==NULL)
	{
		printf("Cannot match friend DB with user DB. Check your DB.");
		presskey();
	}
	else
	{
		a->weight++;
		adj* a_adj=(adj*)malloc(sizeof(adj));
		
		//adj id
		a_adj->id=b;
		
		//adj master
		a_adj->master=a;
				
		//adj next, prev
		a_adj->next=a->first;
		if(a->first!=NULL)
			a->first->prev=a_adj;
		a->first=a_adj;
		a_adj->prev=NULL;
		
		
		adj* b_adj=(adj*)malloc(sizeof(adj));
		
		//adj id
		b_adj->id=b;
		
		//adj master
		b_adj->master=a;
				
		//adj next, prev
		b_adj->next=b->first;
		if(b->first!=NULL)
			b->first->prev=b_adj;
		b->first=b_adj;
		b_adj->prev=NULL;
	}
}

int stringhash(char *s)
{
	/*
	int h = 0;
	for (int i=0; i<strlen(s); i++)
	{
		h = 31 * h + s[i];
	}
	*/
	
	
	if(strlen(s)<2)
		return abs(s[0]+s[1]*31)*31%hashsize;
	else
		return abs(s[strlen(s)-2]+s[1])*31%hashsize;
}

//0
void readdata()
{
    system("cls");
    printf("<<0. Read data files>>\n\n");
    printf("....LOADING DB....\n");
    FILE *userfile = NULL, *friendfile=NULL, *wordfile=NULL;
    
    //open files
    userfile = fopen(userfilename, "r" );
    friendfile = fopen(friendfilename, "r" );
    wordfile = fopen(wordfilename, "r" );
    
    
    if(userfile==NULL||friendfile==NULL||wordfile==NULL)
	{
		printf("Cannot open DB files. Please check your DB files.\n");
	}
	else if(last!=NULL)
	{
		printf("You already loaded DB. Please select other menu.\n");
	}
	else
	{
        char strTemp[tweetsize], strTempTemp[idsize];
        char *pStr;
        int i, hashtmp;
		
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
		
    	printf("Total users: %d\n", totalusers);
        
		//read friendfile
        for(i=0;!feof(friendfile);i++)
        {
        	strcpy(strTempTemp, strTemp);
            fgets(strTemp, sizeof(strTemp), friendfile);
            
            if(i%3==1&&strTemp[0]!='\n')
			{
            	strTemp[strlen(strTemp) - 1] = '\0';
				strTempTemp[strlen(strTempTemp) - 1] = '\0';
				friendadd(userlist, strTempTemp, strTemp);
            	
				totalfriends++;
			}
        }
        
   		printf("Total friendship records: %d\n", totalfriends);

		//read wordfile
        for(i=0;i<hashsize;i++)
        	hash[i]=NULL;
        
        for(i=0;!feof(wordfile);i++)
        {
        	tweetadj* memtmp;
        	
            fgets(strTemp, sizeof(strTemp), wordfile);
            while(strTemp[0]=='\n'&&!feof(wordfile))
            	fgets(strTemp, sizeof(strTemp), wordfile);
            
            if(i%3==0&&!feof(wordfile))
            {
            	memtmp=(tweetadj*)malloc(sizeof(tweetadj));
				strTemp[strlen(strTemp) - 1] = '\0';
				
				//init_tweetadj
				memtmp->prev=NULL;
				memtmp->next=NULL;
				memtmp->prevhash=NULL;
				memtmp->nexthash=NULL;
				memtmp->color=0;
				
				//link with userlink
				userlist=last;
				do
				{
					if(strcmp(userlist->id, strTemp)==0)
					{
						memtmp->id=userlist;
						memtmp->next=userlist->tweet;
						userlist->tweet=memtmp;
						if(memtmp->next!=NULL)
							memtmp->next->prev=memtmp;
					}
					userlist=userlist->next;
				}while(userlist!=last&&strcmp(userlist->prev->id, strTemp)!=0);
			}
            else if(i%3==2&&!feof(wordfile))
            {
				//init_tweetadj
				strTemp[strlen(strTemp) - 1] = '\0';
				strcpy(memtmp->content, strTemp);
				
				//link with hash array
				hashtmp=stringhash(strTemp);
					
				memtmp->nexthash=hash[hashtmp];
				if(memtmp->nexthash!=NULL)
					memtmp->nexthash->prevhash=memtmp;
				
				hash[hashtmp]=memtmp;
				
            	totalwords++;
			}
        }
        
        
    	printf("Total tweets: %d\n", totalwords);
	}
	
    system("cls");
    printf("<<0. Read data files>>\n\n");
	printf("Total users: %d\n", totalusers);
	printf("Total friendship records: %d\n", totalfriends);
    printf("Total tweets: %d\n", totalwords);
    
	//close files
	fclose(userfile);
	fclose(friendfile);
	fclose(wordfile);
	
	presskey();
}

//1
void statistics()
{
    system("cls");
    printf("<<1. Display statistics>>\n\n");
	
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
		tweetadj* tweetadjtmp;
		
		userlist=last;
		do
		{
			if(userlist->weight<minfriend)
				minfriend=userlist->weight;
			if(userlist->weight>maxfriend)
				maxfriend=userlist->weight;
			
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
		
		printf("Average number of friends: %.9f\n", (float)totalfriends / (float)totalusers);
		printf("Minimum friends: %d\n", minfriend);
		printf("Maximum number of friends: %d\n", maxfriend);
		printf("Average tweets per user: %.9f\n", (float)totalwords / (float)totalusers);
		printf("Minimum tweets per user: %d\n", mintweet);
		printf("Maximum tweets per user: %d\n", maxtweet);
		
		presskey();
	}
}

//2
void toptweet()
{
    system("cls");
    printf("<<2. Top 5 Most Tweeted Words>>\n\n");
    printf("....LOADING....\n");
    
	if(last==NULL)
	{
		printf("DB가 없습니다. 0을 눌러 DB를 읽어오거나, DB 상태를 확인하세요.\n");
		presskey();
	}
	else
	{
		int i=0, numtmp=0, searchedall=0, standard;
		int maxtweet[5] = {0, 0, 0, 0, 0};
		tweetadj* maxtweetadj[5] = {NULL, NULL, NULL, NULL, NULL};
		char chartmp[tweetsize];
		tweetadj* tweettmp=NULL;
		tweetadj* searchedtweet=NULL;
		
		//init standard
		do{
			tweettmp=hash[i];
			i++;
		}while(tweettmp==NULL&&i<=hashsize);
			
		standard=(tweettmp->color==1)?0:1;
		
		//find
		for(int i=0; i<hashsize; i++)
		{
			do
			{
				tweettmp=hash[i];
				searchedall=1;
				numtmp=0;
				while(tweettmp!=NULL)
				{
					if(tweettmp->color!=standard)	//처음 보는 트윗인 경우 
					{
						searchedall=0;
						if(numtmp==0)				//세고 있지 않은 경우 
						{
							strcpy(chartmp, tweettmp->content);
							searchedtweet=tweettmp;
							tweettmp->color=standard;
							numtmp++;
						}
						else if(strcmp(chartmp, tweettmp->content)==0)					//세고 있는 경우 
						{
							tweettmp->color=standard;
							numtmp++;
						}
					}
					tweettmp=tweettmp->nexthash;
				}
				
				for(int j=4; j>=0; j--)
				{
					if(numtmp>maxtweet[j])
					{
						if(j<4)
						{
							maxtweet[j+1]=maxtweet[j];
							maxtweetadj[j+1]=maxtweetadj[j];
						}
						maxtweet[j]=numtmp;
						maxtweetadj[j]=searchedtweet;
					}
				}
			}while(searchedall==0);
		}
		
	    system("cls");
	    printf("<<2. Top 5 Most Tweeted Words>>\n\n");
	    
		for(int j=0; j<5;)
			printf("%d%s : %s (%d times)\n", ++j, (j==1)?"st":(j==2)?"nd":"th", maxtweetadj[j]->content, maxtweet[j]);
			
		presskey();
	}
}

//3
void topuser()
{
    system("cls");
    printf("<<3. Top 5 Most Tweeted Users>>\n\n");
    printf("....LOADING....\n");
    
	if(last==NULL)
	{
		printf("DB가 없습니다. 0을 눌러 DB를 읽어오거나, DB 상태를 확인하세요.\n");
		presskey();
	}
	else
	{
		int maxtweet[5]={0, 0, 0, 0, 0}, tweettmp=0;
		user* maxuser[5]={NULL, NULL, NULL, NULL, NULL};
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
					maxuser[i]=userlist;
				}
			}
			
			tweettmp=0;
			userlist=userlist->next;
		}while(userlist!=last);
		
	    system("cls");
	    printf("<<3. Top 5 Most Tweeted Users>>\n\n");
	    
		for(int i=0; i<5;)
		{
			printf("%d%s : %s (%d times)\n", ++i, (i==1)?"st":(i==2)?"nd":"th", maxuser[i]->id, maxtweet[i]);
		}
		
		presskey();
	}
}

//4
void findworduser(char tweet[tweetsize])
{
	if(last==NULL)
	{
		printf("No loaded DB. Select '0' to load DB or check DB status.\n");
		presskey();
	}
	else
	{
		int finded, num=0;
		tweetadj* tweettmp;
		user* findeduser[totalusers];
		user* usertmp=userfind;
		
		//init findeduser[], nexttmp
		for(int i=0;i<totalusers;i++)
			findeduser[i]=NULL;
		
		while(userfind!=NULL)
		{
			usertmp=userfind;
			userfind=userfind->nexttmp;
			usertmp->nexttmp=NULL;
		}
		
		
		printf("--User who tweeted the word '%s'--\n\n", tweet);
		tweettmp=hash[stringhash(tweet)];
		strcpy(tweetfind, tweet);
		while(tweettmp!=NULL)
		{
			if(strcmp(tweet, tweettmp->content)==0)
			{
				finded=0;	//겹치는 id여부 확인 
				for(int i=0;i<totalusers;i++)
				{
					if(findeduser[i]==tweettmp->id)
					{
						finded=1;
						break;
					}
					else if(findeduser[i]==NULL)
					{
						findeduser[i]=tweettmp->id;
						break;
					}
				}
				
				if(finded==0)	//지금까지 겹치는 id가 없다면 
				{
					tweettmp->id->nexttmp=userfind;
					userfind=tweettmp->id;
					num++;
					printf("%s ", tweettmp->id->id);
				}
			}
			tweettmp=tweettmp->nexthash;
		}
		printf("\n\nTotally %d users tweeted the word.", num);
	}
}

//5
void findworduserfriend()
{
    system("cls");
    printf("<<5. Find all people who are friends of the above users>>\n\n");
    
	if(last==NULL)
		printf("No loaded DB. Select '0' to load DB or check DB status.\n");
	else if(strcmp(tweetfind, "")==0)
		printf("Menu '4' has not functioned properly. Please try menu '4' first.\n");
	else
	{
		findworduser(tweetfind);
		printf("\n\n--Friends of user who tweeted the word '%s'--\n\n", tweetfind);
		
		user* usertmp=userfind;
		while(userfind!=NULL)
		{
			adj* friendtmp = userfind->first;
			
			printf("Friend of the user '%s' : ", userfind->id);
			
			while(friendtmp!=NULL)
			{
				if(friendtmp->master==userfind)
					printf("%s ", friendtmp->id);
				friendtmp=friendtmp->next;
			}
			
			printf("\n");
			userfind=userfind->nexttmp;
		}
		userfind=usertmp;
		
		printf("\n\n--Friends of user who tweeted the word '%s'--", tweetfind);
	}
	
	presskey();
}

//6
void deletemention(char tweet[tweetsize])
{
	if(last==NULL)
		printf("No loaded DB. Select '0' to load DB or check DB status.\n");
	else if(strcmp(tweet, tweetfind)==0)
		printf("You already deleted the tweet on the previous step.\n Please try another word to delete.\n");
	else
	{
		FILE *wordfile1=NULL;
		wordfile1 = fopen(wordfilename, "r" );
		
	    if(wordfile1==NULL)
			printf("Cannot find 'word.txt'. Please check.");
		else
		{
			strcpy(tweetfind, tweet);
			tweetadj* tweetadjtmp;
			user* usertmp;
			
			userlist=last;
			//init userfind
			do
			{
				usertmp=userlist;
				userlist=userlist->next;
				usertmp->nexttmp=NULL;
			}while(userlist!=last);
			userfind=NULL;
			
			//txt edit
			int num=0;
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
			
			tweetadjtmp=hash[stringhash(tweet)];
			
			while(tweetadjtmp!=NULL)
			{
				if(strcmp(tweetadjtmp->content, tweet)==0)
				{
					//structure edit:hash
					if(tweetadjtmp->prevhash!=NULL)
						tweetadjtmp->prevhash->nexthash=tweetadjtmp->nexthash;
					else
						hash[stringhash(tweet)]=tweetadjtmp->nexthash;

					if(tweetadjtmp->nexthash!=NULL)
						tweetadjtmp->nexthash->prevhash=tweetadjtmp->prevhash;
					
					//structure edit:user
					if(tweetadjtmp->prev!=NULL)	//사용자의 첫번째 트윗이 아니라면
						tweetadjtmp->prev->next=tweetadjtmp->next;
					else						//사용자의 첫번째 트윗인 경우
						tweetadjtmp->id->tweet=tweetadjtmp->next;

					if(tweetadjtmp->next!=NULL)	//뒤에 다른 tweet이 있는 경우 
						tweetadjtmp->next->prev=tweetadjtmp->prev;
					
					if(tweetadjtmp->id->nexttmp==NULL)		//이미 nexttmp 등록된 사람, 즉 이미 한번 같은 글을 쓴 사람 
						tweetadjtmp->id->nexttmp=userfind;
					userfind=tweetadjtmp->id;
					//structure edit:variables
					num++;
					
					free(tweetadjtmp);
				}
				tweetadjtmp=tweetadjtmp->nexthash;
			}
			totalwords-=num;
			
			printf("%d tweet(s) is(are) deleted from DB.", num);
		}
	}
	
	presskey();
}

//7
void deletementionuser()
{
	if(last==NULL)
		printf("No loaded DB. Select '0' to load DB or check DB status.\n");
	else if(strcmp(tweetfind, "")==0)
		printf("Menu '6' has not functioned properly. Please try menu '6' first.\n");
	else
	{
		int agree;
		char s[1];
		
		printf("REALLY DELETE USER'S DB who tweeted the word '%s'? Enter 'y' to confirm, or 'n' to cancel : ", tweetfind);
		scanf("%s", s);
		
		agree=(strcmp(s, "y")==0)?1:0;
		
		if(agree==1)
		{
			tweetadj* tweetadjtmp;
					
			FILE *userfile1=NULL, *friendfile1=NULL, *wordfile1=NULL;
			
			userfile1 = fopen(userfilename, "r");
			friendfile1 = fopen(friendfilename, "r");
			wordfile1 = fopen(wordfilename, "r");		
			
		    if(userfile1==NULL||friendfile1==NULL||wordfile1==NULL)
				printf("Cannot open DB files. Please check your DB files.\n");
			else
			{
				int usernum=0, adjnum=0, tweetnum=0;
				int check; 
				user* userdel;
				user* usertmp=userfind;
				adj* adjtmp;
				adj* adjdel;
				adj* partneradjtmp;
				tweetadj* tweettmp;
				tweetadj* tweetdel;
				
	    		char strTemp1[idsize], strTemp2[tweetsize], strTemp3[tweetsize], strTemp4[tweetsize];
			    
			    
					
				while(userfind!=NULL)
				{
					userdel=userfind;
					userfind=userfind->nexttmp;
					
					//del first(friend)
					adjtmp=userdel->first;
					while(adjtmp!=NULL)
					{
						adjdel=adjtmp;
						adjtmp=adjtmp->next;
						//어차피 다지우므로 prev, next 정리 필요 없음
						
						//partner의 prev, next 정리
						if(adjdel->id==userdel)		//지우려는 user를 친구로 하는 adj인 경우 
						{
							partneradjtmp=adjdel->master->first;
							
							while(partneradjtmp!=NULL&&partneradjtmp->id!=userdel)		//똑같은 friendship record가 없다는 가정 하에 
								partneradjtmp=partneradjtmp->next;
								
							if(partneradjtmp->prev!=NULL)
								partneradjtmp->prev->next=partneradjtmp->next;
							else
								adjdel->master->first=partneradjtmp->next;
						}
						else						//지우려는 user가 친구로 하는 adj인 경우 
						{
							partneradjtmp=adjdel->id->first;
						
							while(partneradjtmp!=NULL&&partneradjtmp->master!=userdel)		//똑같은 friendship record가 없다는 가정 하에 
								partneradjtmp=partneradjtmp->next;
								
							if(partneradjtmp->prev!=NULL)
								partneradjtmp->prev->next=partneradjtmp->next;
							else
								adjdel->id->first=partneradjtmp->next;
						}
						
						if(partneradjtmp->next!=NULL)
							partneradjtmp->next->prev=partneradjtmp->prev;
						
						free(adjdel);
						adjdel=NULL;
						free(partneradjtmp);
						partneradjtmp=NULL;
						adjnum++;
					}
					
					
					//del tweet
					tweettmp=userdel->tweet;
					while(tweettmp!=NULL)
					{
						tweetdel=tweettmp;
						tweettmp=tweettmp->next;
						//어차피 다지우므로 prev, next 정리 필요 없음 
						
						//hash prev, next 정 
						if(tweetdel->prevhash!=NULL)
							tweetdel->prevhash->nexthash=tweetdel->nexthash;
						else
							hash[stringhash(tweetdel->content)]=tweetdel->nexthash;
						
						if(tweetdel->nexthash!=NULL)
							tweetdel->nexthash->prevhash=tweetdel->prevhash;
						
						free(tweetdel);
						tweetdel=NULL;
						tweetnum++;
					}
					//del user				
					//prev, next 정리 
					userdel->prev->next=userdel->next;
					userdel->next->prev=userdel->prev;
					if(last==userdel)
						last=userdel->prev;
					
					free(userdel);
					userdel=NULL;
					
					usernum++;
			    }
			
				totalusers-=usernum;
				totalfriends-=adjnum;
				totalwords-=tweetnum;
			    			    
				printf("%d user(s), %d friend record(s), and %d tweet(s) are deleted.(no effect to the actual DB)", usernum, adjnum, tweetnum);
			}
		}
	}
	
	presskey();
}

//8

void dfs_visit(user* usernode)
{
	int num;
	user* w=NULL;
	usernode->color = gray;
	usernode->d = dfs_time;
	usernode->f = dfs_time;
	dfs_time++;
	push(usernode);	
	
	adj* v=usernode->first;
	while(v!=NULL)
	{
		if(v->id->color==white&&v->master==usernode)
		{
			dfs_visit(v->id);
			v->master->f=min(v->master->f, v->id->f);
		}
		else if(v->id->color==gray&&v->master==usernode)
		{
			v->master->f=min(v->master->f, v->id->d);
		}
			
		v=v->next;
	}
	
	if(usernode->f==usernode->d)		//root인 경우 
	{
		num=0;
		do
		{
			w=pop();
			if(w!=NULL)
			{
				w->color=black;
				num++;
			}
		}while(w!=usernode&&w!=NULL);
		
		for(int i=4; i>=0&&maxscc[i]<num; i--)
		{
			if(i<4)
			{
				maxscc[i+1]=maxscc[i];
				maxsccuser[i+1]=maxsccuser[i];
			}
			maxscc[i]=num;
			maxsccuser[i]=usernode;
		}
	}
}

void scc(user* targetuser)
{
	if(last==NULL)
	{
		printf("No loaded DB. Select '0' to load DB or check DB status.\n");
		presskey();
	}
	else
	{
		if(targetuser==NULL)
		{
			printf("Invalid user id. Check if there is such id in DB.\n");
			presskey();
		}
		else
		{
			//init
			user* usertmp=targetuser;
			do
			{
				targetuser->color=white;
				targetuser->f=0;
				targetuser->d=0;
				targetuser=targetuser->next;
			}while(targetuser!=usertmp);
			
			dfs_time = 0;
			
			for(int i=0; i<5; i++)
			{
				maxscc[i]=0;
				maxsccuser[i]=NULL;
			}
			
			usertmp=targetuser;
			do
			{
				if(targetuser->color==white)
					dfs_visit(targetuser);
				targetuser=targetuser->next;
			}while(targetuser!=usertmp);
						
			for(int i=0; i<5; i++)
				if(maxsccuser[i]!=NULL)
					printf("%d위 : SCC of %s(%d users)\n", i+1, maxsccuser[i]->id, maxscc[i]);
				
			presskey();
		}
	}
}

//9
void dijkstra(char beginningid[])
{
	if(last==NULL)
	{
		printf("No loaded DB. Select '0' to load DB or check DB status.\n");
		presskey();
	}
	else
	{
		adj* adjtmp=NULL;
		user* beginning=NULL;
		user* current=NULL;
		user* parenttmp=NULL;
		user* listtmp=userlist;
		
		int tmp=0;
		
		//init
		stacklist=NULL;
		listtmp=last;
		do
		{
			listtmp->color=white;
			listtmp->d=1E7;
			listtmp->parent=NULL;
			
			if(strcmp(beginningid, listtmp->id)==0)
				beginning=listtmp;
			
			listtmp=listtmp->next;
		}while(listtmp!=last);
		
		if(beginning==NULL)
		{
			printf("Invalid user id. Check if there is such id in DB.\n");
			presskey();
		}
		else
		{
			beginning->color=gray;
			beginning->d=0;
			beginning->parent=NULL;
			int num=0;
			 
			enqueue(beginning);
			while(stacklist!=NULL)
			{
				current=dequeue();
				adjtmp=current->first;
				while(adjtmp!=NULL)
				{
					if(adjtmp->id->color==white)
					{
						adjtmp->id->color=gray;
						adjtmp->id->d=current->d + adjtmp->id->weight;
						adjtmp->id->parent=current;
						enqueue(adjtmp->id);
					}
					adjtmp=adjtmp->next;
				}
				
				if(current->parent!=NULL)
				{
					num++;
					printf("%d : %s(distance %d)\n", num, current->id, current->d);
					if(num<6)
					{
						printf("	path : ");
						parenttmp=current;
						while(parenttmp!=NULL)
						{
							printf("%s", parenttmp->id);
							if(parenttmp->parent!=NULL)
								printf(" ← "); 
							parenttmp=parenttmp->parent;
						}
						printf("\n");
					}
				}
				current->color=black;
			}
			
			presskey();
		}
	}
}

int menu_display(void)
{
    int select;
    system("cls");
    
	printf("Twitter Analyzing Program v1.0 (by 2010100086 Kim Hakyoung) \n\n");
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
    printf("Press 'Enter' to return to main menu.");
    scanf("%c", &select);
}


int main(int argc, char** argv)
{
	int select;
	char tweet[tweetsize], userid[idsize], destination[idsize];
	
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
				toptweet();
				break;
				
			case 3:
				topuser();
				break;
				
			case 4:
			    system("cls");
			    printf("<<4. Find users who tweeted a word>>\n\n");
				printf("What word would you like to search? : ");
				scanf("%s", tweet);
			    system("cls");
			    printf("<<4. Find users who tweeted a word>>\n\n");
				findworduser(tweet);
				presskey();
				break;
				
			case 5://같은 단어를 같은 사용자가 말했을경우... 
				findworduserfriend();
				break;
				
			case 6:
    			system("cls");
    			printf("<<6. Delete all mentions of a word>>\n\n");
				printf("What word/tweet would you like to delete : ");
				scanf("%s", tweet);
				deletemention(tweet);
				break;
				
			case 7:
    			system("cls");
    			printf("<<7. Delete all users who mentioned a word>>\n\n");
				deletementionuser();
				break;
				
			case 8:				
    			system("cls");
    			printf("<<8. Find strongly connected components>>\n\n");
				scc(userlist);
				break;
				
			case 9:
    			system("cls");
    			printf("<<9. Find shortest path from a specific user.>>\n\n");
				printf("The beginning node(user) : ");
				scanf("%s", userid);
				dijkstra(userid);
				break;
				
			default:
				printf("This is a wrong menu number.\n Please press a proper menu number.\n");
				presskey();
		}
	}
	system("cls");
	printf("안녕히 가십시오.");
}
