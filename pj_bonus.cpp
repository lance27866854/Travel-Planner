#include <iostream>
#include <fstream>
#include <queue>
#include <stack>
#include <string>
#define IN_FILE_NAME "tp.data"
#define OUT_FILE_NAME "ans1.txt"

std::ifstream in_file;
std::ofstream out_file;

typedef std::pair<int, int> Pair;
typedef std::pair<int, Pair> t_Pair;

int nodes_happiness_id[100];
int link_weight[100][100];
std::string nodes_name[100];
Pair nodes_op_time[100];

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
            int from_idx = 0;
            while(find_path(from_idx));
        }
        void output(){
            out_file<<happiness<<" "<<cost<<"\n";
            int s = path.size();
            for(int i=0;i<s;i++){
                out_file<<nodes_name[path[i]]<<" "<<start_time<<" "<<start_time;
                if(i!=s-1) start_time+=link_weight[path[i]][path[i+1]];
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
            path.push_back(0);
            happiness+=nodes_happiness_id[0];
            nodes_happiness_id[0]=0;
        }
        bool find_path(int& from){
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
                if(link_weight[from][i]!=0) path_tree.push({1, i});
            }
            visited[0]=1;

            while(!path_tree.empty()){//O(n^2) for a path.
                //get.
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
                        if(link_weight[p.second][i]!=0) path_tree.push({cnt+1, i});//(use link list?)
                }
            }
            StepTable[0]=0;
            for(int i=2;i<=max_cnt;i++){
                StepTable[i] += StepTable[i-1];
            }

            ///2.
            int choice_index = nodes_vec.size()-1;
            int first_n = nodes_vec[choice_index];
            float first_mw = nodes_happiness_id[first_n]/link_weight[from][first_n];
            for(int i=StepTable[max_cnt-1];i<StepTable[max_cnt];i++){
                int sec_n = nodes_vec[i];
                float sec_mw = nodes_happiness_id[sec_n]/link_weight[from][sec_n];
                bool flag = (first_mw>=sec_mw)? 1 : 0;
                first_mw = (flag)? first_mw : sec_mw;
                choice_index = (flag)? choice_index : i;
            }
            first_n = nodes_vec[choice_index];

            //run
            for(int i=max_cnt;i>1;i--){
                //if that point can't reach
                if(min_dis[0][choice_index]>=budget) break;
                route[route_size++] = nodes_vec[choice_index];

                int next_choice_index;
                float max_weight;
                bool flag=0;
                int n = nodes_vec[choice_index];
                int next_n;

                for(int j=StepTable[i-1];j<StepTable[i];j++){
                    next_n = nodes_vec[j];
                    //choose the best adjacent node.
                    if(link_weight[n][next_n]>0){//can_reach
                        float new_weight = nodes_happiness_id[next_n]/link_weight[n][next_n];
                        if(flag){
                            max_weight = ( max_weight<new_weight )? new_weight : max_weight;
                            next_choice_index = j;
                        }
                        else{
                            flag = true;
                            max_weight = new_weight;
                            next_choice_index = j;
                        }
                    }
                }
                //iteration
                next_n = nodes_vec[next_choice_index];
                choice_index = next_choice_index;
                //count
                cost+=link_weight[n][next_n];
                budget-=link_weight[n][next_n];
                happiness+=nodes_happiness_id[n];
                nodes_happiness_id[n]=0;
            }
            delete []visited;
            delete []StepTable;

            //add the link(from, first station)
            cost+=link_weight[from][nodes_vec[0]];
            budget-=link_weight[from][nodes_vec[0]];
            happiness+=nodes_happiness_id[from];
            nodes_happiness_id[from]=0;
            //std::cout<<happiness<<" "<<cost;

            if(min_dis[0][nodes_vec[choice_index]]<budget){//no break
                //std::cout<<budget;
                int n = route[route_size-1];
                int next_n = nodes_vec[choice_index];
                //wait for next run.
                route[route_size++] = nodes_vec[choice_index];
                get_path(route, route_size);
                delete []route;
                //count
                cost+=link_weight[n][next_n];
                happiness+=nodes_happiness_id[n];
                budget-=link_weight[n][next_n];
                nodes_happiness_id[n]=0;
                from = next_n;
                return 1;
            }
            else{
                //end
                int n = nodes_vec[max_cnt];
                get_path(route, route_size);
                delete []route;//std::cout<<from;
                ending_path(first_n);
                return 0;
            }
        }
        void ending_path(int& from){
            //we are going to do the last step.
            //to close the traverse loop, we need to do BFS again.
            //this time, all we care about is shortest path.
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
                if(link_weight[from][i]!=0) path_tree.push({link_weight[from][i],{1, i}});
            }

            while(!path_tree.empty()){//O(n^2) for a path.
                //get.
                //p = <distance ,<count, value>>
                t_Pair p = path_tree.front();
                path_tree.pop();

                if(visited[p.second.second]==false){
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
                        if(link_weight[p.second.second][i]!=0) path_tree.push({p.first+link_weight[p.second.second][i],{cnt+1, i}});//(use link list?)
                }
            }
            //StepTable[0]=0;
            for(int i=2;i<=max_cnt;i++){
                StepTable[i] += StepTable[i-1];
            }//std::cout<<max_cnt;

            //t_pair = <dis, value>
            int choice_index = nodes_vec.size()-1;
            for(int i=max_cnt;i>1;i--){
                //if that point can't reach
                route[route_size++]= nodes_vec[choice_index].second;

                int next_choice_index;
                float max_weight;
                bool flag=0;
                int n = nodes_vec[choice_index].second;
                int next_n;

                for(int j=StepTable[i-1];j<StepTable[i];j++){
                    next_n = nodes_vec[j].second;
                    //choose the best adjacent node.
                    if(link_weight[n][next_n]>0){//can_reach
                        int new_weight = nodes_vec[choice_index].first;
                        if(flag){
                            max_weight = ( max_weight<new_weight )? new_weight : max_weight;
                            next_choice_index = j;
                        }
                        else{
                            flag = true;
                            max_weight = new_weight;
                            next_choice_index = j;
                        }
                    }
                }
                //iteration
                next_n = nodes_vec[next_choice_index].second;
                choice_index = next_choice_index;
                //count
                cost+=link_weight[n][next_n];
                budget-=link_weight[n][next_n];
                happiness+=nodes_happiness_id[n];
                nodes_happiness_id[n]=0;
            }
            //add the link(from, first station)
            cost+=link_weight[from][nodes_vec[0].second];
            budget-=link_weight[from][nodes_vec[0].second];
            happiness+=nodes_happiness_id[from];
            nodes_happiness_id[from]=0;

            get_path(route, route_size);
            delete []visited;
            delete []StepTable;
            delete []route;
        }
        void get_path(int* route, const int& route_size){
            for(int i=route_size-1;i>=0;i++){
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
    int nodes, links, budget, start_time;
    in_file>>nodes>>links>>budget>>start_time;
    for(int i=0;i<nodes;i++){
        std::string name;
        int happiness_id, start_time, close_time;
        in_file>>name>>happiness_id>>start_time>>close_time;
        nodes_name[i] = name;
        nodes_happiness_id[i] = happiness_id;
        nodes_op_time[i].first = start_time;
        nodes_op_time[i].second = close_time;
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
    Travel t(nodes, links, budget, start_time);
    t.solve();std::cout<<"#";
    t.output();std::cout<<"#";

    //close files and finish
    in_file.close();
    out_file.close();
    return 0;
}
