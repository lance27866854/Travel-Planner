#include <iostream>
#include <fstream>
#include <queue>
#include <stack>
#include <string>
#define IN_FILE_NAME "tp.data"
#define OUT_FILE_NAME "ans1.txt"
#define MAX_WEIGHT 2147483647
std::ifstream in_file;
std::ofstream out_file;

typedef std::pair<int, int> Pair;
typedef std::pair<int, Pair> t_Pair;

int nodes_happiness_id[100];
int link_weight[100][100];
std::string nodes_name[100];
Pair nodes_op_time[100];
int test_i=0;
class Travel{
    public:
        Travel(const int& n, const int& l, const int& b,const int& s){
            nodes = n;
            links = l;
            budget = b;
            start_time = s;
            happiness = 0;
            cost = 0;
            initialize();
        }
        ~Travel(){
            for(int i=0;i<nodes;i++) delete []min_dis[i];
            delete []min_dis;
        }
        void solve(){
            path.push_back(0);
            int from_idx = 0;
            while(find_path(from_idx)){test_i++;}
            //end the path.
            //ending_path(from_idx);
        }
        void output(){
            out_file<<happiness<<" "<<cost<<"\n";
            //std::cout<<happiness<<" "<<cost<<"\n";
            int s = path.size();
            for(int i=0;i<s;i++){
                //std::cout<<path[i];
                out_file<<nodes_name[path[i]]<<" "<<start_time<<" "<<start_time;
                if(i!=s-1) start_time+=link_weight[path[i]][path[i+1]];
                out_file<<"\n";
            }
        }

    private:
        ///private variables
        //output data
        std::vector<int> path;
        int cost;
        int happiness;
        //constant
        int budget;
        int nodes, links;
        int start_time;
        int **min_dis;

        ///tool functions
        void initialize(){
            min_dis = new int* [nodes];
            for(int i=0;i<nodes;i++) min_dis[i] = new int[nodes];

            for(int i=0;i<nodes;i++){
                for(int j=0;j<nodes;j++){
                    min_dis[i][j]=link_weight[i][j];
                }
            }

            for(int k=0;k<nodes;k++){
                for(int i=0;i<nodes;i++){
                    for(int j=0;j<nodes;j++){
                        if((min_dis[i][k]+min_dis[k][j])<min_dis[i][j]){
                            min_dis[i][j] = min_dis[i][k]+min_dis[k][j];
                        }
                    }
                }
            }
        }
        bool find_path(int& from){
            //path.push_back(from);
            //we choose paths according to the gain value (happiness / distance);
            //first, we do BFS starting from "from point".
            //second, choose the points which are can_reach and high gain.
            //examine if the chosen "to point" can back to the base in limit budget every iteration.
            ///1.
            std::queue<Pair> path_tree;
            std::vector<int> nodes_vec;

            bool* visited = new bool[nodes];
            int* StepTable = new int[nodes];
            int* route = new int[nodes];
            int route_size=0;
            int max_cnt=0;

            for(int i=0;i<nodes;i++){
                visited[i]=0;
                StepTable[i]=0;
                //push the adjacent node into path_tree.
                if(link_weight[from][i]!=MAX_WEIGHT) path_tree.push({1, i});
            }
            visited[from]=1;

            //<cnt, node_id>
            while(!path_tree.empty()){
                Pair p = path_tree.front();
                path_tree.pop();

                if(visited[p.second]==false){
                    visited[p.second]=true;
                    //count
                    int cnt = p.first;
                    StepTable[cnt]++;
                    if(cnt>max_cnt) max_cnt = cnt;

                    //push_back
                    nodes_vec.push_back(p.second);

                    //get next
                    for(int i=0;i<nodes;i++)
                        if(link_weight[p.second][i]!=MAX_WEIGHT)
                            path_tree.push({cnt+1, i});//(use link list?)
                }
            }
            StepTable[0]=0;
            for(int i=2;i<=max_cnt;i++){
                StepTable[i] += StepTable[i-1];
            }//if(test_i==1){for(int i=0;i<nodes_vec.size();i++) std::cout<<nodes_vec[i];}

            ///2.
            int choice_index = nodes_vec.size()-1;
            int first_n = nodes_vec[choice_index];
            float first_mw = (min_dis[from][first_n]==0)? 0 : (float)nodes_happiness_id[first_n]/min_dis[from][first_n];
            for(int i=StepTable[max_cnt-1];i<StepTable[max_cnt];i++){
                int sec_n = nodes_vec[i];
                float sec_mw = (min_dis[from][sec_n]==0)? 0 : (float)nodes_happiness_id[sec_n]/min_dis[from][sec_n];
                bool flag = (first_mw>=sec_mw)? 1 : 0;
                first_mw = (flag)? first_mw : sec_mw;
                choice_index = (flag)? choice_index : i;
            }
            route[route_size++] = nodes_vec[choice_index];

            //run
            for(int i=max_cnt-1;i>0;i--){
                int next_choice_index=0;
                float max_weight=0;
                int n = nodes_vec[choice_index];

                for(int j=StepTable[i-1];j<StepTable[i];j++){
                    int next_n = nodes_vec[j];
                    //choose the best adjacent node.
                    if(link_weight[n][next_n]!=MAX_WEIGHT){//can_reach
                        float new_weight = (float)nodes_happiness_id[next_n]/link_weight[n][next_n];
                        next_choice_index = ( max_weight<new_weight )? j : next_choice_index;
                        max_weight = ( max_weight<new_weight )? new_weight : max_weight;
                    }
                }
                //iteration
                //next_n = nodes_vec[next_choice_index];
                choice_index = next_choice_index;
                //renew
                route[route_size++] = nodes_vec[choice_index];
            }
            //if(test_i==1){for(int i=0;i<route_size;i++) std::cout<<route[i];}
            delete []visited;
            delete []StepTable;

            //prune the path to fit size.
            int idx;
            bool prune_flag = prune(from, route, route_size, idx);//std::cout<<idx;
            get_path(route, idx, route_size);
            from = route[idx];
            delete []route;

            return prune_flag;
        }

        void ending_path(const int& from){//std::cout<<from;
            ///1.
            std::queue<t_Pair> path_tree;
            std::vector<Pair> nodes_vec;

            bool* visited = new bool[nodes];
            int* StepTable = new int[nodes];
            int* route = new int[nodes];
            int route_size=0;
            int max_cnt=0;

            for(int i=0;i<nodes;i++){
                visited[i]=0;
                StepTable[i]=0;
                //push the adjacent node into path_tree.
                if(link_weight[from][i]!=MAX_WEIGHT) path_tree.push({link_weight[from][i],{1, i}});
            }

            while(!path_tree.empty()){
                //p = <distance ,<count, value>>
                t_Pair p = path_tree.front();
                path_tree.pop();

                if(visited[p.second.second]==false){//std::cout<<p.second.second;
                    visited[p.second.second]=true;
                    //count
                    int cnt = p.second.first;
                    StepTable[cnt]++;
                    if(cnt>max_cnt) max_cnt = cnt;

                    //push_back
                    nodes_vec.push_back({p.first,p.second.second});
                    if(p.second.second == 0) break;

                    //get next
                    for(int i=0;i<nodes;i++)
                        if(link_weight[p.second.second][i]!=MAX_WEIGHT) path_tree.push({p.first+link_weight[p.second.second][i],{cnt+1, i}});//(use link list?)
                }
            }
            StepTable[0]=0;
            for(int i=2;i<=max_cnt;i++){
                StepTable[i] += StepTable[i-1];
            }
            ///2
            //t_pair = <dis, value>
            int choice_index = nodes_vec.size()-1;//std:://cout<<max_cnt<<" ";
            //int virtual_cost=budget;
            route[route_size++] = nodes_vec[choice_index].second;

            for(int i=max_cnt-1;i>0;i--){
                int next_choice_index=0;
                int min_dis = MAX_WEIGHT;
                int n = nodes_vec[choice_index].second;

                for(int j=StepTable[i-1];j<StepTable[i];j++){
                    int next_n = nodes_vec[j].second;
                    //choose the best adjacent node.
                    if(link_weight[n][next_n]!=MAX_WEIGHT){//can_reach
                        int new_dis = nodes_vec[choice_index].first;
                        next_choice_index = ( min_dis>new_dis )? j : next_choice_index;
                        min_dis = ( min_dis>new_dis )? new_dis : min_dis;
                    }
                }
                //iteration
                choice_index = next_choice_index;
                //renew

                route[route_size++] = nodes_vec[choice_index].second;
            }
            delete []visited;
            delete []StepTable;
            //prune the path to fit size.
            int n = from;
            for(int i=route_size-1;i>=0;i--){
                cost+=link_weight[n][route[i]];
                budget-=cost;
                happiness+=nodes_happiness_id[route[i]];
                nodes_happiness_id[route[i]]=0;
                n=route[i];
            }
            get_path(route, 0, route_size);
            delete []route;
        }

        bool prune(const int& from, int* route, const int& route_size, int& idx){
            bool flag = 1;
            int n = from;
            int i;
            for(i=route_size-1;i>=0;i--){
                if(min_dis[from][route[i]]>budget){//No budget!!
                    flag = 0;
                    break;
                }
                cost+=link_weight[n][route[i]];
                budget-=cost;
                happiness+=nodes_happiness_id[route[i]];
                nodes_happiness_id[route[i]]=0;
                n=route[i];
            }
            idx = i+1;
            return flag;
        }
        void get_path(int* route, const int& start_idx, const int& end_idx){
            for(int i=end_idx-1;i>=start_idx;i--){
                path.push_back(route[i]);
            }
        }
};


int main(void){
    //files
    in_file.open(IN_FILE_NAME, std::ios::in);
    out_file.open(OUT_FILE_NAME, std::ios::out);
    if(!in_file||!out_file) std::cout<<"something wrong with the files.";

    //input
    int nodes, links, budget, astart_time;
    in_file>>nodes>>links>>budget>>astart_time;
    //nodes
    for(int i=0;i<nodes;i++){
        std::string name;
        int happiness_id, start_time, close_time;
        in_file>>name>>happiness_id>>start_time>>close_time;
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
        std::string from, to;
        int weight;
        in_file>>from>>to>>weight;

        int flag=0;
        int f_idx, t_idx;
        for(int i=0;i<nodes&&flag!=2;i++){//map??
            if(nodes_name[i]==from){
                f_idx = i;
                flag++;
            }
            else if(nodes_name[i]==to){
                t_idx = i;
                flag++;
            }
        }
        link_weight[f_idx][t_idx]=weight;
        link_weight[t_idx][f_idx]=weight;
    }
    //travel
    Travel t(nodes, links, budget, astart_time);
    t.solve();std::cout<<"#";
    t.output();std::cout<<"#";

    //close files and finish
    in_file.close();
    out_file.close();
    return 0;
}

