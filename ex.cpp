#include <iostream>
#include <fstream>
#include <string>
#include <map>

#define IN_FILE_NAME1 "tp.data"
#define IN_FILE_NAME2 "ans1.txt"
#define IN_FILE_NAME3 "ans2.txt"
#define MAX_WEIGHT 2147483647
#define DAY_TIME 1440
#define WRONG_NAME_ERROR 30
#define WRONG_POINT_ERROR 31
#define WRONG_PATH_ERROR 32
#define WRONG_TIME_ERROR 33
#define WRONG_HAPPINESS_ERROR 34
#define WRONG_COST_ERROR 35
#define EXCESS_COST_ERROR 36
#define COMPLETE 37

using namespace std;
typedef pair<int, int> Pair;

ifstream in_file1;
ifstream in_file2;

int nodes_happiness_id[100];
int link_weight[100][100];
string nodes_name[100];
Pair nodes_op_time[100];
map<string, int> name_map;

class Examine{
    public:
        Examine(){}
        Examine(const int& tc){time_consider = !tc;}
        ~Examine(){}
        void out(){
            int error = examine_path();

            cout<<"Case "<<(time_consider? "1 :" : "2 :");

            if(error == WRONG_NAME_ERROR) cout<<"Wrong name.\n";
            else if(error == WRONG_POINT_ERROR) cout<<"Wrong point.\n";
            else if(error == WRONG_TIME_ERROR) cout<<"Wrong time.\n";
            else if(error == EXCESS_COST_ERROR) cout<<"Excess budget.\n";
            else if(error == WRONG_PATH_ERROR) cout<<"Wrong path.\n";
            else if(error == WRONG_HAPPINESS_ERROR) cout<<"Wrong happiness.\n";
            else if(error == WRONG_COST_ERROR) cout<<"Wrong cost.\n";
            else if(error == COMPLETE) cout<<"Complete.\n";
            else cout<<"Something wrong with the examination process.\n";
        }

    private:
        bool time_consider;
        int examine_path(){
            int nodes, links, budget, astart_time;

            ///in_file1
            //input
            in_file1>>nodes>>links>>budget>>astart_time;
            //nodes
            for(int i=0;i<nodes;i++){
                string name;
                int happiness_id, start_time, close_time;
                in_file1>>name>>happiness_id>>start_time>>close_time;
                name_map.insert({name, i});
                nodes_name[i] = name;
                nodes_happiness_id[i] = happiness_id;
                nodes_op_time[i].first = start_time;
                nodes_op_time[i].second = close_time;
            }
            //links
            for(int i=0;i<nodes;i++){
                for(int j=0;j<nodes;j++){
                    link_weight[i][j]=MAX_WEIGHT;
                }
            }
            for(int i=0;i<links;i++){
                string from, to;
                int weight;
                in_file1>>from>>to>>weight;

                auto f_idx = name_map.find(from);
                auto t_idx = name_map.find(to);

                link_weight[f_idx->second][t_idx->second]=weight;
                link_weight[t_idx->second][f_idx->second]=weight;
            }

            ///in_file2
            //input
            int ex_happiness=0, ex_cost=0;
            int happiness, cost;
            int now_idx=0, now_time=astart_time;
            int next_idx;

            string s;
            int in_time, out_time;

            in_file2>>happiness>>cost;
            in_file2>>s>>in_time>>out_time;
            if(cost>budget) return EXCESS_COST_ERROR;
            if(nodes_name[0]!=s) return WRONG_POINT_ERROR;
            if(in_time!=now_time) return WRONG_TIME_ERROR;
            if(time_consider||(nodes_op_time[0].first<=now_time && now_time<=nodes_op_time[0].second)){
                ex_happiness+=nodes_happiness_id[now_idx];
                nodes_happiness_id[now_idx]=0;
            }
            now_time = out_time;

            while(in_file2>>s>>in_time>>out_time){

                auto it = name_map.find(s);
                if(it == name_map.end()) return WRONG_NAME_ERROR;
                next_idx = it->second;

                if(link_weight[now_idx][next_idx]==MAX_WEIGHT) return WRONG_PATH_ERROR;
                if(in_time>DAY_TIME||out_time>DAY_TIME) return WRONG_TIME_ERROR;
                now_time=(now_time+link_weight[now_idx][next_idx])%DAY_TIME;
                if(in_time!=now_time) return WRONG_TIME_ERROR;
                ex_cost+=link_weight[now_idx][next_idx];

                if(time_consider||(nodes_op_time[next_idx].first<=now_time && now_time<=nodes_op_time[next_idx].second)){
                    ex_happiness+=nodes_happiness_id[next_idx];
                    nodes_happiness_id[next_idx]=0;
                }

                now_idx = next_idx;
                now_time = out_time;
            }
            if(ex_happiness!=happiness) return WRONG_HAPPINESS_ERROR;
            if(ex_cost!=cost) return WRONG_COST_ERROR;
            return COMPLETE;
        }
};

int main(void){
    ///case 1.
    in_file1.open(IN_FILE_NAME1, ios::in);
    in_file2.open(IN_FILE_NAME2, ios::in);
    if(!in_file1||!in_file2) cout<<"\nsomething wrong with the files.\n";
    Examine ex1(0);
    ex1.out();
    in_file1.close();
    in_file2.close();

    ///case 2.
    in_file1.open(IN_FILE_NAME1, ios::in);
    in_file2.open(IN_FILE_NAME3, ios::in);
    if(!in_file1||!in_file2) cout<<"\nsomething wrong with the files.\n";
    Examine ex2(1);
    ex2.out();
    in_file1.close();
    in_file2.close();
    return 0;
}
