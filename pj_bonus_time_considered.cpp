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
typedef std::pair<Pair, Pair> d_Pair;

int nodes_happiness_id[100];
int link_weight[100][100];//?
std::string nodes_name[100];
Pair nodes_op_time[100];

class Node{
    public:
        Node(){
            next = nullptr;
        }
        Node(int n){
            next = nullptr;
            num = n;
        }
        ~Node(){}
        int num;
        Node* next;
};

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
            for(int i=0;i<nodes;i++){
                delete_node_table(node_table[i]);
            }
            delete []node_table;
        }
        void solve(){
            path.push_back(0);
            int from_idx = 0;
            int current_time = start_time;
            while(find_path(from_idx, current_time));
            //end the path.
            ending_path(from_idx);
        }
        void output(){
            out_file<<happiness<<" "<<cost<<"\n";
            int s = path.size();
            for(int i=0;i<s;i++){
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
        //container
        Node **node_table;

        ///tool functions
        void initialize(){
            ///build link list
            //This table represent the adjacent nodes.
            //The first node represent how many nodes are adjacent to node[i].
            //(i.e. node_table[i]->num = how many node adjacent to node[i].)
            node_table = new Node*[nodes];
            for(int i=0;i<nodes;i++){
                node_table[i]=nullptr;
                int _count = 0;
                for(int j=0;j<nodes;j++){
                    if(link_weight[i][j]!=MAX_WEIGHT){
                        Node* n = new Node(j);
                        n->next = node_table[i];
                        node_table[i] = n;
                        _count++;
                    }
                }
                Node *head = new Node(_count);
                head->next = node_table[i];
                node_table[i] = head;
            }

            ///build mis_dis
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
                        if((min_dis[i][k]+min_dis[k][j])<min_dis[i][j]&&min_dis[i][k]!=MAX_WEIGHT&&min_dis[k][j]!=MAX_WEIGHT){
                            min_dis[i][j] = min_dis[i][k]+min_dis[k][j];
                        }
                    }
                }
            }
        }
        void delete_node_table(Node * n){
            if(n!=nullptr){
                delete_node_table(n->next);
                delete n;
            }
        }
        int analys(int *StepTable, const std::vector<t_Pair>& nodes_vec, const int& from, int& max_cnt, int& max_time){
            ///we are going to find the optimum value of every node.
            ///The formula = (distance + ports) * happiness.
            ///distance is for decreasing iterations, and dead route avoidance.
            int max_value=0;
            int max_step;
            int max_idx=from;

            for(int i=max_cnt;i>0;i--){
                for(int j=StepTable[i-1];j<StepTable[i];j++){
                    int x = nodes_vec[j].first;
                    int t = nodes_vec[j].second.first;
                    int h = nodes_vec[j].second.second;
                    int value = (i+node_table[x]->num)*h;//count formula.

                    if(value>max_value&&min_dis[from][x]<=budget/2){
                        max_value = value;
                        max_step = i;
                        max_idx = j;
                        max_time = t;
                    }
                }
            }

            max_cnt = max_step;
            return max_idx;
        }
        Pair renew_BFS_info(const Pair& current_pair, const int& from_idx, const int& to_idx){
            if(current_pair.first+link_weight[from][to]>=op_time.first&&current_pair.first+link_weight[from][to]<=op_time.second)
                return {current_pair.first+link_weight[from][to], current_pair.second+nodes_happiness_id[to]};
            else
                return {current_pair.first+link_weight[from][to], current_pair.second};
        }

        bool find_path(int& from, int& current_time){
            //path.push_back(from);
            //we choose paths according to the gain value (happiness / distance);
            //first, we do BFS starting from "from point".
            //second, choose the points which are can_reach and high gain.
            //examine if the chosen "to point" can back to the base in limit budget every iteration.
            ///1.
            std::queue<d_Pair> path_tree;
            std::vector<t_Pair> nodes_vec;

            bool* visited = new bool[nodes];
            int* StepTable = new int[nodes];
            int* route = new int[nodes];
            int route_size=0;
            int max_cnt=0;

            Node* np = node_table[from]->next;
            while(np != nullptr){
                int i = np->num;
                Pair p = {current_time, 0};
                path_tree.push({renew_BFS_info(p, from, i),{1, i}});
                np = np->next;
            }

            for(int i=0;i<nodes;i++){
                visited[i]=0;
                StepTable[i]=0;
            }

            visited[from]=1;

            //BFS
            while(!path_tree.empty()){
                //p = <<time, happiness> ,<distance, value>>
                d_Pair p = path_tree.front();
                int node_id = p.second.second;
                path_tree.pop();

                if(visited[node_id]==false){
                    visited[node_id]=true;
                    //count
                    int cnt = p.second.first;
                    StepTable[cnt]++;
                    if(cnt>max_cnt) max_cnt = cnt;

                    //push_back
                    nodes_vec.push_back({node_id, p.first});

                    //get next
                    Node* n = node_table[node_id]->next;
                    while(n != nullptr){
                        int i = n->num;
                        path_tree.push({renew_BFS_info(p.first, node_id, i),{cnt+1, i}});//HERE
                        n = n->next;
                    }
                }
            }

            StepTable[0]=0;
            for(int i=2;i<=max_cnt;i++){
                StepTable[i] += StepTable[i-1];
            }

            ///2.
            int max_time = current_time;
            int choice_index = analys(StepTable, nodes_vec, from, max_cnt, max_time);
            route[route_size++] = nodes_vec[choice_index].first;

            //run
            for(int i=max_cnt-1;i>0;i--){
                int next_choice_index=0;
                float max_weight=0;
                int n = nodes_vec[choice_index].first;

                for(int j=StepTable[i-1];j<StepTable[i];j++){
                    int next_n = nodes_vec[j].first;
                    //choose the best adjacent node.
                    if(link_weight[n][next_n]!=MAX_WEIGHT){//can_reach
                        float new_weight = (float)nodes_happiness_id[next_n]/link_weight[n][next_n];
                        next_choice_index = ( max_weight<new_weight )? j : next_choice_index;
                        max_weight = ( max_weight<new_weight )? new_weight : max_weight;
                    }
                }
                //iteration
                choice_index = next_choice_index;
                //renew
                route[route_size++] = nodes_vec[choice_index].first;
            }
            delete []visited;
            delete []StepTable;

            //prune the path to fit size.
            int idx;
            bool prune_flag = prune(from, route, route_size, idx);
            get_path(route, idx, route_size);
            from = route[idx];
            delete []route;

            return prune_flag;
        }

        void ending_path(const int& from){
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
            int choice_index = nodes_vec.size()-1;
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
    t.solve();//std::cout<<"#";
    t.output();//std::cout<<"#";

    //close files and finish
    in_file.close();
    out_file.close();
    return 0;
}

