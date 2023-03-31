#include<stdio.h>
#include<stdlib.h>
#define max 50
struct node
{
    int dist[max];
    int hop[max];
};
typedef struct node node;
int main()
{
    
    int n;
    printf("\nNumber of nodes : ");
    scanf("%d",&n);
    
    int costmat[n][n];
    node table[n];
    
    int i,j,k;
    printf("\nEnter the cost matrix :\n");
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            scanf("%d",&costmat[i][j]);
            table[i].dist[j]=costmat[i][j];
            table[i].hop[j]=j;
        }
    }
    
    printf("\nThe resultant table is: \n\n");
    int f=0;
        do
        {
            f=0;
            for(i=0;i<n;i++)
		    for(j=0;j<n;j++)
			    for(k=0;k<n;k++)
				if(table[i].dist[j]>costmat[i][k]+table[k].dist[j])
				{
				    table[i].dist[j]=table[i].dist[k]+table[k].dist[j];
				    table[i].hop[j]=k;
				    f++;
				}
        }while(f!=0);
        
        
        printf("nodes	| ");
        for(i=0;i<n;i++)
        	 printf("%d          |",i+1);
        for(i=0;i<n;i++)
        {
            printf("\nrouter %d| ",i+1);
            for(j=0;j<n;j++)
            {
                printf("%d (hop: %d) |",table[i].dist[j],table[i].hop[j]+1);
            }
        }
        printf("\n");
}
