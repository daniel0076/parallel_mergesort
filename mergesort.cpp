#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void Merge(vector<int>& array, vector<int>& result, int left, int mid, int right)
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

void MergeSort(vector<int>& array, vector<int>& result, int left, int right)
{
    if(left < right){
        int mid = (left+right)/2;
        MergeSort(array,result,left,mid);
        MergeSort(array,result,mid+1,right);
        Merge(array,result,left,mid+1,right);
    }
}

int main()
{
    fstream fin;
    fin.open("sort.txt",ios::in);
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

    MergeSort(unsort, result,0,unsort.size()-1);
    for(uint32_t i = 0; i < result.size(); i++) {
        cout<<result[i]<<" ";
    }
    return 0;
}
