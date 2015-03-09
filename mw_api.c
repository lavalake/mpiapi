#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <mw_api.h>

void runMaster(int argc, char **argv, mw_api_spec *input);
void runWorker(int argc, char **argv, mw_api_spec *input);

void runMaster(int argc, char **argv, mw_api_spec *input){
    int workerSize = 0;
    MPI_Status status;
    mw_works **workList = NULL;
    void *results = NULL;
    void *tempResult = NULL;
    int workCount = 0;
    int resultCount = 0;
    int count = 0;
    int i = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &workerSize);

    workList = input->create(argc,argv);
    while(workList[workCount] != NULL){
        workCount++;
    }
    results = malloc(input->res_sz*workCount);
    tempResult = results;
    //distribute the work
    for(rank = 1; rank < workerSize; rank++){
        mw_works *work = workList[rank-1];
        MPI_Send(work,
                input->work_sz,
                MPI_BYTE,
                rank,
                1,
                MPI_COMM_WORLD);
    } 
    count = rank;
    for(;count <= workCount; count++){
        MPI_Recv(tempResult,
                 input->res_sz,
                 MPI_BYTE,
                 MPI_ANY_SOURCE,
                 1,
                 MPI_COMM_WORLD,
                 &status);
        tempResult = (char*)tempResult + input->res_sz;
        resultCount++;
        MPI_Send(workList[count],
                input->work_sz,
                MPI_BYTE,
                status.MPI_SOURCE,
                1,
                MPI_COMM_WORLD);
    }
    //receive the results
    for(rank = 1; rank < hostSize; rank++){
        MPI_Recv(tempResult,
                 input->res_sz,
                 MPI_BYTE,
                 MPI_ANY_SOURCE,
                 1,
                 MPI_COMM_WORLD,
                 &status);
        tempResult = (char*)tempResult + input->res_sz;
        resultCount++;

    }

    input->result(resultCount, results);
    free(results);
    //free the workList
    i = 0;
    while(workList[i] != NULL){
        free(workList[i]);
    }
    
    for(i = 0; i <= workCount; i++){
        free(workList);
        workList++;
    }
    //turn down the workers
    for(rand = 1; rank < hostSize; rank++){
        MPI_Send(0,
                0,
                MPI_BYTE,
                rank,
                0,
                MPI_COMM_WORLD);

    }

}

void runWorker(int argc, char **argv, mw_api_spec *input){
    char *work = NULL;
    MPI_Status status;
    int work_sz = input->work_sz;
    void *result = NULL;
    work = malloc(work_sz);
    while(true){
        MPI_Recv(work,
                work_sz,
                MPI_BYTE,
                0,
                MPI_ANY_TAG,
                MPI_COMM_WORLD,
                &status);
        if(status.MPI_TAG == 0){
            //quit
            free(work);
            return;
        }
        
        result = input->compute(work);
        MPI_Send(result,
                input->res_sz,
                MPI_BYTE,
                0,
                1,
                MPI_COMM_WORLD);

    }
    free(work);
}
void Mw_Run(int argc, char **argv, mw_api_spec *f){
    int hostId = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &hostId);
    //master
    if(hostId == 0){
        runMaster(argc, argv, f);
    }else{//worker
        runWorker(argc, argv, f);
    }
    MPI_Finalize();
}
