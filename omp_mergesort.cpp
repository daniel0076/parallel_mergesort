#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <omp.h>

using namespace std;

void merge(vector<int>& array, vector<int>& result, int left, int mid, int right)
{
    int l = left;
    int r = mid;
    int k = left;
    while((l <= mid-1) && (r <= right)){
        if(array[l]<=array[r]){
            result[k++] = array[l++];
        }else{
            result[k++] = array[r++];
        }
    }
    while(l <= mid-1){
        result[k++] = array[l++];
    }
    while(r <= right){
        result[k++] = array[r++];
    }
    // write back for next time use
    int i;
    for(i=left;i<=right;i++){
        array[i] = result[i];
    }
}

void mergesort_serial(vector<int>& array, vector<int>& result, int left, int right)
{
    if(left < right){
        int mid = (left+right)/2;
        mergesort_serial(array,result,left,mid);
        mergesort_serial(array,result,mid+1,right);
        merge(array,result,left,mid+1,right);
    }
}

void mergesort_omp(vector<int>& array, vector<int>& result, int left, int right, int threads)
{
    if(left < right){
        int mid = (left+right)/2;
        if(threads == 1){
            mergesort_serial(array,result,left,right);
        }
        else if(threads >= 2){

            omp_set_num_threads(2);
            #pragma omp parallel sections
            {
                #pragma omp section
                mergesort_omp(array,result,left,mid,threads/2);
                #pragma omp section
                mergesort_omp(array,result,mid+1,right,threads- threads/2);
            }

            merge(array,result,left,mid+1,right);
        }
    }
}

int main(int argc, char* argv[])
{
    int thread_num=1;
    if(argc != 2){
        cerr<<"Usage "<<argv[0]<<" thread_num"<<endl;
        return -1;
    }else{
        thread_num = atoi(argv[1]);
    }

    fstream fin;
    fin.open("/tmp/sort.txt",ios::in);
    string line;
    vector<int> unsort;
    while(getline(fin,line)){
        static stringstream ss(line);
        static int tmp;
        while(ss >> tmp){
            unsort.push_back(tmp);
        }
    }
    vector<int> result;
    result.resize(unsort.size());

    omp_set_nested(1);
    mergesort_omp(unsort, result,0,unsort.size()-1,thread_num);
    for(uint32_t i = 0; i < result.size(); i++) {
        cout<<result[i]<<" ";
    }
    return 0;
}
