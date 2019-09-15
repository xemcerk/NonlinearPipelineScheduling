//#include "utility.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <bitset>
#include <cmath>

using namespace std;

bool myComparison(int a,int b){
    return a==b;
}

int main(){
    //read reservation tabel
    ifstream infile("reservation_table1.txt");
    int nrow=1 ,ncol=1 ,tmp=0 ;//number of rows and columns of reservation table
    vector<vector<int>> rt;//reservation table
    vector<int> row;//row of reservation table
    if(infile.is_open()){
        infile >> nrow >> ncol;
        for(int i=0; i<nrow; i++){
            for(int j=0; j<ncol; j++){
                infile >> tmp;
                row.push_back(tmp);
                // cout<<tmp<<" ";
            }
            rt.push_back(row);
            row.clear();
            // cout<<endl;
        }        
    }
    infile.close();
    //print reservation table
    // for(auto rt_it=rt.begin();rt_it!=rt.end();rt_it++){
    //     auto rt_row=*rt_it;
    //     for(auto row_it=rt_row.begin(); row_it!=rt_row.end(); row_it++){
    //         cout<< *row_it<< " ";
    //     }
    //     cout<< endl;
    // }

    //calculate forbidden latencies
    vector<int> fbl;//forbidden latencies
    vector<int> rclk;//reserved clocks of each row
    for (int i=0; i!=nrow; i++){
        for (int j=0; j!=ncol; j++){
            if (rt[i][j] ==1){
                rclk.push_back(j);
            }
        }
        //calculate forbidden latencies of the current row
        int latency=0;
        for (auto front=rclk.begin(); front!=rclk.end()-1; front++){
            for(auto back=rclk.begin()+1; back!=rclk.end(); back++){
                latency=abs(*back-*front);
                fbl.push_back(latency);
            }
        }
        rclk.clear();
    }
    unique(fbl.begin(), fbl.end(), myComparison);
    // for(auto it=fbl.cbegin(); it!=fbl.cend(); it++){
    //     cout<< *it<< " "<<endl;
    // }

    //construct collision vector
    int bit_val=0;
    int fbl_tmp=0;
    for(auto it=fbl.cbegin(); it!=fbl.cend(); it++){
        bit_val+=pow(2, *it-1);
        fbl_tmp= *it;
    }
    bitset<10> clv(bit_val);
    cout<< clv<<endl; 

    return 0;
}