#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <omp.h>

using namespace std;

void mergesort_omp(vector<int>& array, vector<int>& result, int left, int right, int threads);
void mergesort_serial(vector<int>& array, vector<int>& result, int left, int right);
void merge(vector<int>& array, vector<int>& result, int left, int mid, int right);
void p_merge(vector<int>& array, vector<int>& result,
        int l1, int r1, int l2, int r2, int l3, int);
int binary_search(int key, vector<int>& array, int left, int right);

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
    /*
    for(uint32_t i = 0; i < result.size(); i++) {
        cout<<result[i]<<" ";
    }
    */
    return 0;
}

void mergesort_omp(vector<int>& array, vector<int>& result, int left, int right, int threads)
{
    if(left < right){
        int mid = (left+right)/2;

        if(threads >= 2){

            omp_set_num_threads(2);
            #pragma omp parallel sections
            {
                #pragma omp section
                mergesort_omp(array,result,left,mid,threads/2);
                #pragma omp section
                mergesort_omp(array,result,mid+1,right,threads - threads/2);
            }
            //merge(array,result,left,mid+1,right);
            p_merge(array,result,left,mid,mid+1,right,left, 4);
        }
        else{
            mergesort_serial(array,result,left,right);
        }
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

void p_merge(vector<int>& array, vector<int>& result,
        int l1, int r1, int l2, int r2, int l3, int thread_num)
{
    int len1 = r1-l1+1;
    int len2 = r2-l2+1;

    if(len1 < len2){
        // swap left and right
        l1 ^= l2;
        l2 ^= l1;
        l1 ^= l2;

        r1 ^= r2;
        r2 ^= r1;
        r1 ^= r2;
        // swap len1 and len2
        len1 ^= len2;
        len2 ^= len1;
        len1 ^= len2;
    }
    if(len1 == 0){
        return;
    }else{
        int mid1 = (l1 + r1) / 2;
        int mid2 = binary_search(array[mid1], array, l2,r2 );
        int result_pos = l3+(mid1-l1)+(mid2-l2);
        result[result_pos] = array[mid1];

        if(thread_num <= 1){
                p_merge(array, result, l1, mid1-1, l2, mid2-1, l3, 1);
                p_merge(array, result, mid1+1, r1, mid2, r2, result_pos+1, 1);
        }else{

            omp_set_num_threads(2);
            #pragma omp parallel sections
            {
            #pragma omp section
                p_merge(array, result, l1, mid1-1, l2, mid2-1, l3, thread_num/2);
            #pragma omp section
                p_merge(array, result, mid1+1, r1, mid2, r2, result_pos+1, thread_num- thread_num/2);
            }
            // write back for next time use
            int i;
            for(i=l1;i<=r2;i++){
                array[i] = result[i];
            }
        }
    }
}

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

int binary_search(int key, vector<int>& array, int left, int right){
    int low = left;
    int high = max(left,right+1);
    int mid;
    while(low < high){
        mid = (low+high)/2;
        if(key <= array[mid]){
            high = mid;
        }else{
            low = mid+1;
        }
    }
    return high;
}
