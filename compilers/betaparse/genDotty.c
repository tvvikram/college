#include<stdio.h>
#include<string.h>
FILE *ft,*html;
int flag=0;
void removespace(char *s)
{
	int i,j=0;
	for(i=0;s[i] != '\0';i++)
	{
		if(s[i] == '\t')
			continue;
		if(s[i] == '/' &&  s[i+1] =='/')
			break;
		s[j++]=s[i];
	}
	s[j]='\0';
	return;		
}
void disp(char *scannedline)
{
	char key[10],colors[][20]={"black","black fillcolor=red"};
	char htmlcolor[][20]={"black","red"};
	static int c=0,node=1,parent[20],i=1;
	sscanf(scannedline,"%[^ ]s",key);
	
	if(strstr(scannedline,"(") && strstr(scannedline,");")==NULL)
	{
		node++;i=1;
		parent[++i]=node;
//		printf("%s",scannedline);
		fprintf(ft,"\n%d[label=\"%s\" fillcolor=blue];",node,scannedline);
		fprintf(ft,"\n%d->%d",0,node);
	}
	else if(strcmp(key,"loop")==0)
	{
		node++;
		fprintf(ft,"\n%d[label=\"%s\" fillcolor=yellow];",node,"Loop Branch");
		fprintf(ft,"\n%d->%d",parent[i],node);
		parent[++i]=node;
		c++;node++;
		fprintf(ft,"\n%d[label=\"%s\" color=%s];",node,scannedline,colors[flag]);
		fprintf(ft,"\n%d->%d",parent[i],node);
	}
	else if(strcmp(key,"test")==0)
	{
		node++;c++;
		fprintf(ft,"\n%d[label=\"%s\" fillcolor=violet];",node,"Test Branch");
		fprintf(ft,"\n%d->%d",parent[i],node);
		parent[++i]=node;
		c++;node++;
		fprintf(ft,"\n%d[label=\"%s\" color=%s];",node,key,colors[flag]);
		fprintf(ft,"\n%d->%d",parent[i],node);

	}
	else if(strcmp(key,"otherwise")==0)
	{
		node++;
		fprintf(ft,"\n%d[label=\"%s\" color=%s];",node,key,colors[flag]);
		fprintf(ft,"\n%d->%d",parent[i],node);

	}
	else if(strcmp(key,"endt")==0)
	{
		node++;
		i--;
		fprintf(ft,"\n%d[label=\"%s\" color=%s];",node,key,colors[flag]);
		fprintf(ft,"\n%d->%d",parent[i],node);
	}
	else if(strcmp(key,"endf")==0)
	{
		node++;i--;
		fprintf(ft,"\n%d[label=\"%s\" color=%s];",node,key,colors[flag]);
		fprintf(ft,"\n%d->%d",parent[i],node);
	}
	else if(strcmp(key,"endl")==0)
	{
		node++;i--;
		fprintf(ft,"\n%d[label=\"%s\" color=%s];",node,key,colors[flag]);
		fprintf(ft,"\n%d->%d",parent[i],node);
	}
	else if(strcmp(key,"include")==0 && flag==0)
	{
		node++;
		fprintf(ft,"\n%d[label=\"%s\" fillcolor=orange];",node,scannedline);
		fprintf(ft,"\n%d->%d",parent[i],node);
	}		//*/
	else if(strcmp(key,"macro")==0 && flag==0)
	{
		node++;
		fprintf(ft,"\n%d[label=\"%s\" fillcolor=orange];",node,scannedline);
		fprintf(ft,"\n%d->%d",parent[i],node);
	}		//*/
	else 
	{
		node++;
		fprintf(ft,"\n%d[label=\"%s\" color=%s];",node,scannedline,colors[flag]);
		fprintf(ft,"\n%d->%d",parent[i],node);
	}
	return;
}
int dispDotty(int *error, int noErrors, char *file)
{
	int count=0,no=0,ignore=0;
	char scannedline[80]="";
	FILE *fp;
	fp=fopen(file,"r");
	if(fp==NULL)
		return 1;
	ft = fopen("tree.dot","w");
	if(ft==NULL)
		return 2;
	html = fopen("tree.html","w");
	if(html==NULL)
		return 3;
	fprintf(html,"<html><head><title>%s</title></head>\n<body bgcolor='0F0F0F'>",file);
	fprintf(html,"\n<center><h1>");
	fprintf(html,"<font color='BLUE'>B</font>");
	fprintf(html,"<font color='RED'>E</font>");
	fprintf(html,"<font color='GREEN'>T</font>");
	fprintf(html,"<font color='BLUE'>A </font>");
	fprintf(html,"<font color='RED'>P</font>");
	fprintf(html,"<font color='GREEN'>A</font>");
	fprintf(html,"<font color='BLUE'>R</font>");
	fprintf(html,"<font color='RED'>S</font>");
	fprintf(html,"<font color='GREEN'>E</font>");
	fprintf(html,"</h1></center>\n");
	fprintf(html,"\n<hr color='green' width='90%c' size='3'>",'%');
	fprintf(ft,"digraph prog {\n0[label=\"Program\" fillcolor=green]");
	while(strcmp(scannedline,"endp")!=0 && !feof(fp))
	{
		strcpy(scannedline,"");
		while(strcmp(scannedline,"endf")!=0 && strcmp(scannedline,"endp")!=0 )
		{
			strcpy(scannedline,"");
			strcpy(scannedline,"");
			count++;
			if(noErrors > no && count==error[no])
			{
				flag=1;
				no++;
			}
			fscanf(fp,"%[^\n]s",scannedline);
			fgetc(fp);
			if(strlen(scannedline)==0)
				continue;
			if(flag)
				fprintf(html,"\n<font color=\"red\" size='5'>");
			else
				fprintf(html,"\n<font color=\"green\" size='5'>");
			fprintf(html,"<pre>\n%s</pre>",scannedline);
			fprintf(html,"\n</font>");
			removespace(scannedline);
			if(strlen(scannedline)==0)
				continue;
			disp(scannedline);
			flag=0;
		}
	}
	fprintf(html,"</body></html");
	fprintf(ft,"\n}");
	fclose(fp);
	fclose(ft);
	fclose(html);
	// system("nohup firefox tree.html &");
	// system("nohup kgraphviewer-kde4 tree.dot &");
	return 0;
}

