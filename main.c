#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <mw_api.h>

typedef struct userdef_work_t {
    int number;
} userdef_work_t;
typedef struct userdef_result_t{
    int res;
} userdef_result_t;

mw_work_t *create_work(int argc, char **argv){
    int start = atoi(argv[1]);
    int end = atoi(argv[2]);
    int count = end - start + 1;
    mw_work_t *workList[count + 1];//NULL terminator
    workList = malloc(count * sizeof(mw_work_t*));

    for(int i = start; i <= end; i++){
        mw_work_t *temp = malloc(sizeof(mw_work_t));
        temps->number = i;
        workList[i-start] = temp;
    }

    workList[count] = NULL;
    return workList;
}

int process_result(int size, mw_result_t *res){
    int sum = 0;
    mw_result_t *result = res;
    printf("process result: size %d\n", size);
    for(int i = 0;i < size; i++){
        sum += result->res;
        result++;
    }
    printf("The Sum of 1 is %d\n",sum);
    return 1;
}

mw_result_t *compute(mw_work_t *job){
    int sum = 0;
    int size = sizeof(int);
    mw_result_t *result = malloc(sizeof(mw_result_t));
    for(int i = 0; i < size; i++){
        if(((job->number >> i) && 0x1) == 1){
            sum++;
        }
    }
    result->res = sum;
    return result;
}

int main(int argc, char **argv){
    struct mw_api_spec f;
    
    MPI_Init(&argc, &argv);

    f.create = create_work;
    f.compute = process_result;
    f.compute = compute;
    f.work_sz = sizeof(mw_work_t);
    f.res_sz = sizeof(mw_result_t);

    Mw_Run(argc, argv, &f);

    return 0;

}
