#include <mpi.h>
#include <stdio.h>
#include <math.h>

long get_elapsed();

int main(int argc, char** argv) {
	// Initialize the MPI environment
	MPI_Init(NULL, NULL);

	// Get the number of processes
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// Get the rank of the process
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	// Get the name of the processor
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	MPI_Get_processor_name(processor_name, &name_len);

	if (world_rank==0) {
		printf("Hello from cpu 0\n");
		long elapsed[world_size];
		elapsed[0]=0;
		for (int k=1; k<world_size; k++) {
			MPI_Recv(&elapsed[k],1,MPI_LONG,k,k,MPI_COMM_WORLD,MPI_STATUS_IGNORE);	
			printf("Elapsed time for cpu %d: %lu microseconds\n",k,elapsed[k]);
		}
		
		long sum=0;
		long sumsqr=0;
		for (int k=1; k<world_size; k++) {
			sum+=elapsed[k];
			sumsqr+=elapsed[k]*elapsed[k];
		}
		FILE *outf=fopen("output.txt","w");
		fprintf(outf,"avg = %g\n",(double)sum/(world_size-1));
		if (world_size>0) {
			double stdev=sqrt((double)sumsqr-sum*sum/(world_size-1))/world_size;
			fprintf(outf,"stdev = %g\n",stdev);
			fprintf(outf,"CV = %g%%\n",stdev/(sum/(world_size-1))*100);
		}
		for (int k=1; k<world_size; k++) {
			fprintf(outf,"%lu\n",elapsed[k]);
		}
		fclose(outf);
	}
	else {
		printf("Hello from cpu %d\n",world_rank);
		int k=world_rank;
		long elapsed=get_elapsed();
		MPI_Send(&elapsed,1,MPI_LONG,0,k,MPI_COMM_WORLD);
	}

	// Finalize the MPI environment.
	MPI_Finalize();
}

long get_elapsed() {
	clock_t start = clock();
	long sum=0;
	for (long i=0; i<1e7; i++) {
		sum*=i;
		sum+=i;
	}
	clock_t diff = clock() - start;
	long duration = diff * 1000 * 1000 / CLOCKS_PER_SEC;
	return duration;
}

