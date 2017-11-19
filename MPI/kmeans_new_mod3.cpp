#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<limits.h>
#include <bits/stdc++.h>
#include <omp.h>

int IMAGE_BYTES, CLUSTER_BYTES;


int *r, *g, *b, *redCentroid, *greenCentroid, *blueCentroid;
int width, height, nCentroids, nIterations,size;
const char *inputFile, *outputFile;



using namespace std;
int dis(int a, int b, int c, int x, int y, int z)
{
	return int(pow((a-x),2)+pow((b-y),2)+ pow((c-z),2));
}


int assignCluster(int r,int g, int b, int cluster_points_r[],int cluster_points_g[], int cluster_points_b[] , int K)
{	

	int min=INT_MAX, min_ind;
	for(int j=0;j<K;j++)
	{
		int dist = dis(r, g, b, cluster_points_r[j], cluster_points_g[j], cluster_points_b[j]);
		if(dist<min)
		{
			min=dist;
			min_ind = j;
		}
	}
	return min_ind;


}



bool loadRawImage(const char* filename, int* r, int* g, int* b, int size) {
	FILE *imageFile;
	imageFile = fopen(filename, "r");

	if (imageFile == NULL) {
		return false;
	} else {
		for (int i = 0; i < size; i++) {

			r[i] = fgetc(imageFile);
			g[i] = fgetc(imageFile);
			b[i] = fgetc(imageFile);
		}
		fclose(imageFile);

		/*for(int j = 0; j < h * w; j++) {
			printf("%d, %d, %d ", r[j], g[j], b[j]);
		}*/
		return true;
	}
}




bool writeRawImage(const char* filename, int* labelArray, int* redCentroid, int* greenCentroid, int* blueCentroid, int size){
	FILE *imageFile;
	imageFile = fopen(filename, "wb");
	// printf("Size is %d\n", size);
	if(imageFile == NULL) {
		return false;
	} else {
		for (int i = 0; i < size; i++) {
			fputc((char) redCentroid[labelArray[i]], imageFile);
			fputc((char) greenCentroid[labelArray[i]], imageFile);
			fputc((char) blueCentroid[labelArray[i]], imageFile);
		}
		fclose(imageFile);
		return true;
	}
}





int main(int argc, char const *argv[])
{
	MPI_Init(NULL, NULL);
	int num_proc, my_rank;
    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    // printf("%d %d\n", num_proc, my_rank);
    MPI_Status stat;


	inputFile = argv[1];
	outputFile = argv[2];
	width = atoi(argv[3]);
	height = atoi(argv[4]);
	nCentroids = atoi(argv[5]);
	IMAGE_BYTES = width * height * sizeof(int);
	CLUSTER_BYTES = nCentroids * sizeof(int);
	size = width * height;
	// printf("Size -- %d\n", size);
	//printf("Memset with %d\n", IMAGE_BYTES);
	r = (int*)(malloc(IMAGE_BYTES));
	//printf("Master %ld\n ", sizeof(r));
	g = (int*)(malloc(IMAGE_BYTES));
	b = (int*)(malloc(IMAGE_BYTES));

	redCentroid = (int*)(malloc(CLUSTER_BYTES));
	greenCentroid = (int*)(malloc(CLUSTER_BYTES));
	blueCentroid = (int*)(malloc(CLUSTER_BYTES));
	//printf("Image loading...\n");
	if (loadRawImage(inputFile, r, g, b, size)) {
		//printf("Image loaded! %d %d %d\n", r[size-1], g[size-1], b[size-1]);
	} else {
		printf("NOT loaded!\n");
		return -1;
	}

	
	// for(int i=0;i<10;i++)
	// {
	// 	printf("%d %d %d\n", r[i], g[i], b[i]);
	// }
	 //= {1, 2, 3, 4,5, 6};
	int tag=200;
	int K=nCentroids;
	// for(int i=0;i<180;i++)
	// {
	// 	numbers[i] = i;
	// }
	

    if(my_rank==0)
    {
    	double stime = omp_get_wtime(); 
    	printf("Master node!\n");
    	int assignment[size];

    	int points_per_node = size/(num_proc-1);

    	int cluster_points_r[K], cluster_points_g[K], cluster_points_b[K];
    	for(int i=0;i<K;i++)
    	{
    		int random_pt = rand()%size;
    		cluster_points_r[i] = r[random_pt];
    		cluster_points_g[i] = g[random_pt];
    		cluster_points_b[i] = b[random_pt];
    		printf("%d %d %d\n", cluster_points_r[i], cluster_points_g[i], cluster_points_b[i]);
    		// printf("%d  ", cluster_points[i]);
    	}
    	printf("\n");
    	for(int iter=0;iter<100;iter++)
    	{
    		int start_index=0 , end_index=-1;
    	for(int j=0;j<num_proc-2;j++)
    	{
    		start_index = j*points_per_node;
    		end_index = (j+1)*points_per_node-1;
    		int num_indices = end_index- start_index+1;
    		MPI_Send(&start_index, 1, MPI_INT, j+1, tag, MPI_COMM_WORLD);
    		MPI_Send(&end_index, 1, MPI_INT, j+1, tag, MPI_COMM_WORLD);

    	}
    	end_index+=1;	
    	MPI_Send(&end_index, 1, MPI_INT, num_proc-1, tag, MPI_COMM_WORLD);
    	// size=180;
    	int s = size;
    	MPI_Send(&(s), 1, MPI_INT, num_proc-1, tag, MPI_COMM_WORLD);

    	
    	// for(int i=0;i<K;i++)
    	// 	printf("%d  ", cluster_points[i]);
    	
    	MPI_Bcast(cluster_points_r, K, MPI_INT, 0, MPI_COMM_WORLD);
    	MPI_Bcast(cluster_points_g, K, MPI_INT, 0, MPI_COMM_WORLD);
    	MPI_Bcast(cluster_points_b, K, MPI_INT, 0, MPI_COMM_WORLD);
    	int cluster_sum_r[K], cluster_sum_g[K], cluster_sum_b[K];
    	// int cluster_count_r[K], cluster_count_g[K], cluster_count_b[K];
    	int cluster_count[K];
    	memset(cluster_count, 0, sizeof(cluster_count));
    	// memset(cluster_count_g, 0, sizeof(cluster_count_g));
    	// memset(cluster_count_b, 0, sizeof(cluster_count_b));
    	memset(cluster_sum_r, 0, sizeof(cluster_sum_r));
    	memset(cluster_sum_g, 0, sizeof(cluster_sum_g));
    	memset(cluster_sum_b, 0, sizeof(cluster_sum_b));

    	for (int proc = 1; proc < num_proc; proc++)
    	{
    		/* code */
    		
	    	for(int i=0;i<K;i++)
	    	{
	   			int sum_r, sum_g, sum_b, count;  //, count_g, count_b;
	    	 	MPI_Recv(&sum_r, 1, MPI_INT, proc, tag, MPI_COMM_WORLD, &stat);
	    	 	MPI_Recv(&sum_g, 1, MPI_INT, proc, tag, MPI_COMM_WORLD, &stat);
	    	 	MPI_Recv(&sum_b, 1, MPI_INT, proc, tag, MPI_COMM_WORLD, &stat);
	    	 	MPI_Recv(&count, 1, MPI_INT, proc, tag, MPI_COMM_WORLD, &stat);
	    	 	// MPI_Recv(&count_g, 1, MPI_INT, proc, tag, MPI_COMM_WORLD, &stat);
	    	 	// MPI_Recv(&count_b, 1, MPI_INT, proc, tag, MPI_COMM_WORLD, &stat);
	    	 	cluster_sum_r[i]+=sum_r;
	    	 	cluster_sum_g[i]+=sum_g;
	    	 	cluster_sum_b[i]+=sum_b;
	    	 	cluster_count[i]+=count;
	    	 	// cluster_count_g[i]+=count_g;
	    	 	// cluster_count_b[i]+=count_b;
	    	 	// printf("%d %d %d\n",i, cluster_sum[i], cluster_count[i]);
		    }

		}
		// printf("New points:\n");
		int flag=0;
		 for(int i=0;i<K;i++)
    	 {
    	 		// if(cluster_points_r[i]!=cluster_sum_r[i]/cluster_count[i] || cluster_points_g[i]!=cluster_sum_g[i]/cluster_count[i]|| cluster_points_b[i]!=cluster_sum_b[i]/cluster_count[i])
    	 			// flag=1;
    	 		if(cluster_count[i]==0)
    	 		{
    	 			int random_pt = rand()%size;
	    		cluster_points_r[i] = r[random_pt];
	    		cluster_points_g[i] = g[random_pt];
	    		cluster_points_b[i] = b[random_pt];
	    		printf("No points assigned... Assigning cluster randomly\n");
	    		continue;
    	 		}
    	 		cluster_points_r[i] = cluster_sum_r[i]/cluster_count[i];
    	 		cluster_points_g[i] = cluster_sum_g[i]/cluster_count[i];
    	 		cluster_points_b[i] = cluster_sum_b[i]/cluster_count[i];
    	 		// printf("%d : %d %d %d\n", i ,cluster_points_r[i], cluster_points_g[i], cluster_points_b[i]);
    	 }
 //    	 if(flag==0){
 //    	 	printf("Terminating at Iter %d\n", iter);
 //    	 	double etime = omp_get_wtime();

	// printf("%lf\n",etime-stime); 
 //    	 	// iter=99;
 //    	 }

    	 if(iter==99)
    	 {
    	 	int sti, endi ;
    	 	for(int i=1;i<num_proc;i++){
	    	 	MPI_Recv(&sti, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &stat);
	    	 	MPI_Recv(&endi, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &stat);
	    	 	int assignment_k[endi-sti+1];
	    	 	MPI_Recv(&assignment_k, endi-sti+1, MPI_INT, i, tag, MPI_COMM_WORLD, &stat);
	    	 	for(int j=sti;j<=endi;j++)
	    	 	{
	    	 		assignment[j] = assignment_k[j-sti];
	    	 		// printf("%d ---- %d\n", j, assignment[j]);
	    	 	}

    	 	}
    	 	writeRawImage(outputFile, assignment, cluster_points_r,  cluster_points_g, cluster_points_b, size);
    	 }



    	}
	
	double etime = omp_get_wtime();

	printf("Time is : %lf\n",etime-stime); 

    	printf("Answer:\n");
    	for(int i=0;i<K;i++)
    	{
    		printf("%d %d %d\n", cluster_points_r[i], cluster_points_g[i], cluster_points_b[i]);
    	}
    	// MPI_Recv(&sum, sizeof(int *), MPI_INT, 1, 200, MPI_COMM_WORLD, &stat);
    	// printf("%d at Master\n", sum);
    }

    else
    {
    	// printf("In slave: %d\n", my_rank);
    	for(int iter=0;iter<100;iter++)
    	{
	    	int start, end;
	    	MPI_Recv(&start, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &stat);
	    	MPI_Recv(&end, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &stat);
	    	// printf("%d %d from rank %d and %d", start, end, my_rank, r[size-1]);
	    	// printf("\t%d\n", numbers[start]);
	    	int cluster_points_r[K], cluster_points_g[K], cluster_points_b[K];
	    	MPI_Bcast(cluster_points_r, K, MPI_INT, 0, MPI_COMM_WORLD);
	    	MPI_Bcast(cluster_points_g, K, MPI_INT, 0, MPI_COMM_WORLD);
	    	MPI_Bcast(cluster_points_b, K, MPI_INT, 0, MPI_COMM_WORLD);
	    	// printf("Here already\n");
	    	// for(int i=0;i<K;i++)
	    	// 	printf("%d  ", cluster_points[i]);
	    	// printf("\n");
	    	map<int,  vector<int> > cluster;
	    	int assignment[end-start+1];
	    	for(int i=start;i<=end;i++)
	    	{
	    		// has to be changed
	    		// printf("Assigning cluster to %d %d %d...\n", r[i], g[i], b[i]);
	    		int c = assignCluster(r[i],g[i], b[i], cluster_points_r,cluster_points_g, cluster_points_b , K);
	    		// 
	    		// printf("%d---%d\n", i, c);
	    		if(cluster.find(c)==cluster.end())
	            {    vector<int> v;
	                 v.push_back(i);
	                 cluster.insert(make_pair(c,v));
	            }
	            else
	            cluster[c].push_back(i);

	        	assignment[i-start] = c;
	    	}

	    	 map<int,vector<int> >::iterator it;


	    	 for(int i=0;i<K;i++)
	    	 {
	    	 	int sum_r=0, sum_g=0, sum_b=0, count=0;
	    	 	{
	    	 		for(int j=0;j<cluster[i].size();j++)
	    	 		{
	    	 			sum_r+=r[cluster[i][j]];
	    	 			sum_g+=g[cluster[i][j]];
	    	 			sum_b+=b[cluster[i][j]];
	    	 		}
	    	 		count = cluster[i].size();
	    	 	}
	    	 	MPI_Send(&sum_r, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
	    	 	MPI_Send(&sum_g, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
	    	 	MPI_Send(&sum_b, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);

	    	 	MPI_Send(&count, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);


	    	 }


	    	if(iter==99)
	    	{
	    		MPI_Send(&start, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
	    		MPI_Send(&end, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
	    		MPI_Send(assignment, end-start+1, MPI_INT, 0, tag, MPI_COMM_WORLD);
	    	} 
	        // cout<<"Map key and Map value-size:"<<endl;

	        // for(it=cluster.begin();it!=cluster.end();it++)
	        // {  
	        //     cout<<it->first<<":"<<it->second.size();

	        //     // for(int j=0;j<it->second.size();j++)
	        //     //   cout<<it->second[j]<<" ";
	        //     cout<<endl;   
	        // }

 		}
	}
    MPI_Finalize();

}
