#include <iostream>
#include <fstream>
#include <queue>
#include <stack>
#include <string>
#include <algorithm>

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


bool compare_func(const d_Pair& i, const d_Pair& j){
    return (i.first.second < j.first.second);
}


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
            for(int i=0;i<nodes;i++){
                delete_node_table(node_table[i]);
            }
            delete []node_table;
        }
        void solve(){
            path.push_back(0);
            int from_idx = 0;
            while(find_path(from_idx)) std::cout<<from_idx<<"\n";
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
                    if(i==j) min_dis[i][j]=0;
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
        void analys(const std::vector<d_Pair>& nodes_vec, int& choice_step, int& choice_index){
            ///we are going to find the optimum value of every node.
            ///The formula = (step + ports) * happiness.
            ///step is for decreasing iterations, and dead route avoidance.
            int max_value=0;
            int max_idx=0;

            //<step, <happiness, time>>
            for(int i=1;i<nodes;i++){
                int s = nodes_vec[i].first.second;
                int h = nodes_vec[i].second.first;
                int t = nodes_vec[i].second.second;
                int value = (s+node_table[i]->num)*h;//count formula.
                if(value>=max_value&&t<=budget){
                    max_value = value;
                    max_idx = i;
                }
            }
            choice_index = max_idx;
            choice_step = nodes_vec[choice_index].first.second;
        }


        bool find_path(int& from){
            ///1.
            std::queue<d_Pair> path_tree;//<<id, step>, <happiness, time>>

            //shortest_table
            t_Pair* shortest_table = new t_Pair[nodes];//<step, <happiness, time>>
            for(int i=0;i<nodes;i++){
                shortest_table[i] = {-1, {-1, -1}};
            }
            shortest_table[from] = {0, {0, 0}};
            //node_table
            Node* np = node_table[from]->next;
            while(np != nullptr){
                int i = np->num;
                path_tree.push({{i, 1}, {nodes_happiness_id[i], link_weight[from][i]}});
                np = np->next;
            }

            //BFS
            while(!path_tree.empty()){
                d_Pair p = path_tree.front();
                path_tree.pop();

                int id = p.first.first;
                int step = p.first.second;
                int acc_happ = p.second.second;
                int acc_time = p.second.second;

                float n_w = (acc_time==0)? 1000 : acc_happ/acc_time;
                float o_w = (shortest_table[id].second.second==0)? 1000 : shortest_table[id].second.first/shortest_table[id].second.second;

                if(shortest_table[id].first==-1||n_w > o_w){
                    shortest_table[id] = {step, {acc_happ, acc_time}};//<step, <happiness, time>>
                }

                //get next
                if(step<nodes){//skewed case
                    Node* n = node_table[id]->next;
                    while(n != nullptr){
                        int i = n->num;
                        path_tree.push({{i, step+1}, {acc_happ+nodes_happiness_id[i], acc_time+link_weight[id][i]}});
                        n = n->next;
                    }
                }
            }

            ///2.
            //sort the shortest_path
            std::vector<d_Pair> shortest_vec;
            for(int i=0;i<nodes;i++) shortest_vec.push_back({{i, shortest_table[i].first}, shortest_table[i].second});
            std::sort(shortest_vec.begin(), shortest_vec.end(), compare_func);

            delete []shortest_table;

            //route
            int* route = new int[nodes];
            int route_size=0;
            int choice_step;
            int choice_index;

            analys(shortest_vec, choice_step, choice_index);
            if(choice_index == 0) return 0;//from

            //run
            int last_id = shortest_vec[choice_index--].first.first;
            route[route_size++] = last_id;
            while(shortest_vec[choice_index].first.second==choice_step){choice_index--;}

            while(choice_step--){
                //get the next node.
                while(shortest_vec[choice_index].first.second == choice_step){
                    int id = shortest_vec[choice_index].first.first;
                    if(link_weight[last_id][id]!=MAX_WEIGHT){//can reach.
                        route[route_size++] = shortest_vec[choice_index].first.first;
                        last_id = id;
                    }
                    choice_index--;
                }

            }
            //prune the path to fit size.
            int idx;
            bool prune_flag = prune(from, route, route_size-1, idx);
            get_path(route, idx, route_size-1);
            from = route[idx];
            //for(int i=idx;i<route_size;i++) std::cout<<route[i]<<" ";
            //system("pause");
            delete []route;

            return prune_flag;
        }

        void ending_path(const int& from){
            if(from == 0) return;
            ///1.
            std::queue<d_Pair> path_tree;//<<id, step>, <happiness, time>>

            //shortest_table
            t_Pair* shortest_table = new t_Pair[nodes];//<step, <happiness, time>>
            for(int i=0;i<nodes;i++){
                shortest_table[i] = {-1, {-1, -1}};
            }
            shortest_table[from] = {0, {0, 0}};
            //node_table
            Node* np = node_table[from]->next;
            while(np != nullptr){
                int i = np->num;
                path_tree.push({{i, 1}, {nodes_happiness_id[i], link_weight[from][i]}});
                np = np->next;
            }

            //BFS
            while(!path_tree.empty()){
                d_Pair p = path_tree.front();
                path_tree.pop();

                int id = p.first.first;
                int step = p.first.second;
                int acc_happ = p.second.second;
                int acc_time = p.second.second;

                if(shortest_table[id].first==-1||shortest_table[id].second.second>acc_time){
                    shortest_table[id] = {step, {acc_happ, acc_time}};//<step, <happiness, time>>
                }

                //get next
                if(step<nodes){//skewed case
                    Node* n = node_table[id]->next;
                    while(n != nullptr){
                        int i = n->num;
                        path_tree.push({{i, step+1}, {acc_happ+nodes_happiness_id[i], acc_time+link_weight[id][i]}});
                        n = n->next;
                    }
                }
            }

            ///2.
            //sort the shortest_path
            std::vector<d_Pair> shortest_vec;
            for(int i=0;i<nodes;i++) shortest_vec.push_back({{i, shortest_table[i].first}, shortest_table[i].second});
            std::sort(shortest_vec.begin(), shortest_vec.end(), compare_func);

            delete []shortest_table;

            //route
            int* route = new int[nodes];
            int route_size=0;
            int choice_step=shortest_vec[0].first.second;
            int choice_index=0;

            //run
            int last_id = shortest_vec[choice_index--].first.first;
            route[route_size++] = last_id;
            while(shortest_vec[choice_index].first.second==choice_step){choice_index--;}

            while(choice_step--){
                //get the next node.
                while(shortest_vec[choice_index].first.second == choice_step){
                    int id = shortest_vec[choice_index].first.first;
                    if(link_weight[last_id][id]!=MAX_WEIGHT){//can reach.
                        route[route_size++] = shortest_vec[choice_index].first.first;
                        last_id = id;
                    }
                    choice_index--;
                }

            }
            get_path(route, 0, route_size-1);
            delete []route;
        }

        bool prune(const int& from, int* route, const int& route_size, int& idx){
            bool flag = 1;
            int n = from;
            int i;

            for(i=route_size-1;i>=0;i--){
                int v_b = budget-link_weight[n][route[i]];
                if(min_dis[0][route[i]]>v_b){//No budget!!
                    flag = 0;
                    break;
                }
                cost+=link_weight[n][route[i]];
                budget-=link_weight[n][route[i]];
                happiness+=nodes_happiness_id[route[i]];
                nodes_happiness_id[route[i]]=0;
                n=route[i];
            }

            if(route_size==1&&route[0]==0) flag=0;//special case.
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
            if(i==j) link_weight[i][j]=0;
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
