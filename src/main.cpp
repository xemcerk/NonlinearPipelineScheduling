//#include "utility.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <bitset>
#include <cmath>
#include <queue>
#include <numeric>

using namespace std;

bool myComparison(int a, int b)
{
    return a == b;
}

//global vairables for graph computing
vector<vector<int>> adjmat; //adjcent matrix that holds the edges
vector<int> visit(10);      //flag indicates whether it's been visited
vector<vector<int>> path;   //holds the cycle paths
int v_cnt = 1;              //holds the number of vertexs while searching cycles
vector<int> p_tmp(10);      //holds the temporary path

void getAllCycleDFS(int cur_v)
{
    visit[cur_v] = 1;
    for (int adj = 0; adj < 10; adj++)
    {
        if (adjmat[cur_v][adj] == 0)
            continue;
        if (adj > p_tmp[1] && visit[adj] == 0)
        {
            p_tmp[++v_cnt] = adj;
            getAllCycleDFS(adj);
        }
        if (adj == p_tmp[1])
        {
            int cur_path_num = ++path[0][0];
            path[cur_path_num][0] = v_cnt;
            for (int i = 1; i <= v_cnt; i++)
            {
                path[cur_path_num][i] = p_tmp[i];
            }
        }
    }
    visit[p_tmp[v_cnt]] = 0;
    v_cnt--;
}

int main()
{
    //read reservation tabel
    ifstream infile("reservation_table1.txt");
    int nrow = 1, ncol = 1, tmp = 0; //number of rows and columns of reservation table
    vector<vector<int>> rt;          //reservation table
    vector<int> row;                 //row of reservation table
    if (infile.is_open())
    {
        infile >> nrow >> ncol;
        for (int i = 0; i < nrow; i++)
        {
            for (int j = 0; j < ncol; j++)
            {
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
    vector<int> fbl;  //forbidden latencies
    vector<int> rclk; //reserved clocks of each row
    for (int i = 0; i != nrow; i++)
    {
        for (int j = 0; j != ncol; j++)
        {
            if (rt[i][j] == 1)
            {
                rclk.push_back(j);
            }
        }
        //calculate forbidden latencies of the current row
        int latency = 0;
        for (auto front = rclk.begin(); front != rclk.end() - 1; front++)
        {
            for (auto back = rclk.begin() + 1; back != rclk.end(); back++)
            {
                latency = abs(*back - *front);
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
    int bit_val = 0;
    int max_latency = 0;
    for (auto it = fbl.cbegin(); it != fbl.cend(); it++)
    {
        bit_val += pow(2, *it - 1);
        if (max_latency < *it)
            max_latency = *it;
    }
    bitset<10> clv(bit_val);
    // cout<< clv<<endl;

    //construct state diagram
    //state diagram can be represented by a graph
    //so it contains a vertex set and a adjcent matrix
    vector<bitset<10>> vt; //vertex set that holds the states
    for (int i = 0; i < 10; i++)
    {
        vector<int> tmp_vec(10, 0);
        adjmat.push_back(tmp_vec);
    }
    queue<bitset<10>> st_queue; //state queue used to store states during the construction
    vt.push_back(clv);
    st_queue.push(clv); //push the initial state
    while (!st_queue.empty())
    {                                                                           //loop until state queue is empty
        auto cur_st = st_queue.front();                                         //get the current state
        int cur_idx = distance(vt.begin(), find(vt.begin(), vt.end(), cur_st)); //get the index of the curren state in vt
        st_queue.pop();
        auto sft_st = cur_st; //used to hold the right shifting state
        for (int i = 1; i < max_latency + 1; i++)
        {
            bool cur_bit = sft_st[0]; //before right shift, get that bit in the lowest position
            sft_st >>= 1;             //right shift the current state
            if (cur_bit == false)
            { //if the bit is 0,which means this new state is permissionable
                auto new_st = sft_st | clv;
                auto new_it = find(vt.begin(), vt.end(), new_st);
                if (new_it != vt.end())
                {                                               //if the new state has been put in the vertex set
                    int new_idx = distance(vt.begin(), new_it); //get its index in the vertex set
                    adjmat[cur_idx][new_idx] = i;               //add a edge from current state to new state, i is its weight
                }
                else
                {                                       //if the new state has not been put into the vertex set
                    vt.push_back(new_st);               //put it into the vertex set
                    st_queue.push(new_st);              //put it into the states queue
                    adjmat[cur_idx][vt.size() - 1] = i; //add an edge from the curren state to the new state
                }
            }
        }
    }
    //add the edges from all the other states to the original state
    //and set all diagonal elements to 0
    size_t am_size = vt.size();
    for (int i = 0; i < am_size; i++)
    {
        adjmat[i][i] = 0;
        if (i >= 1)
        {
            adjmat[i][0] = max_latency + 1;
        }
    }
    // // print all states and edges
    // cout<< vt.size()<< endl;
    // for(auto it = vt.begin(); it != vt.end(); it++){
    //     cout<< *it << endl;
    // }
    // for(int i=0;i<10;i++){
    //     for(int j=0;j<10;j++){
    //         cout<< adjmat[i][j]<< " ";
    //     }
    //     cout<< endl;
    // }

    for (int i = 0; i < 10; i++)
    {
        visit[i] = 0;
    }
    for (int i = 0; i <= 10; i++)
    {
        vector<int> tmp_vec(10, 0);
        path.push_back(tmp_vec);
    }
    v_cnt = 1;
    int cur_v = 0;
    p_tmp[v_cnt] = cur_v;
    getAllCycleDFS(cur_v);
    cout << path[0][0] << endl;
    for (int i = 1; i < 10; i++)
    {
        if (path[i][0] == 0)
            break;
        for (int j = 0; j < 10; j++)
        {
            cout << path[i][j] << " ";
        }
        cout << endl;
    }

    vector<vector<int>> cycles;
    vector<int> cyc_tmp;
    vector<int> cyc_len;
    for (int i = 1; i < 10; i++)
    {
        if (path[i][0] == 0)
            break;
        // cout<< path[i][0]<<endl;
        cyc_len.push_back(path[i][0]);
        for (int j = 1; j <= path[i][0]; j++)
        {
            // cout<< adjmat[path[i][j]][path[i][j+1]]<<" ";
            cyc_tmp.push_back(adjmat[path[i][j]][path[i][j + 1]]);
        }
        // cout<<endl;
        cycles.push_back(cyc_tmp);
        cyc_tmp.clear();
    }

    int cyc_num = cycles.size();
    int min = 100;
    int min_idx = 0;
    for (int i = 0; i < cyc_num; i++)
    {
        int sum_tmp = accumulate(cycles[i].begin(), cycles[i].end(), 0);
        int avg_tmp = sum_tmp / cyc_len[i];
        if (avg_tmp < min)
        {
            min = avg_tmp;
            min_idx = i;
        }
    }
    cout << min << " " << min_idx << endl;

    return 0;
}