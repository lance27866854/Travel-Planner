#include <iostream>
#include <fstream>
#include <queue>
#include <stack>
#include <string>
#include <algorithm>

#define MAX_WEIGHT 2147483647
#define DAY_TIME 1440

std::ifstream in_file;
std::ofstream out_file;

typedef std::pair<int, int> Pair;
int nodes_happiness_id[100];
int link_weight[100][100];
std::string nodes_name[100];
Pair nodes_op_time[100];

class Path_node{
    public:
        Path_node(){}
        Path_node(int i, int s, int h, int t, int sc){
            id = i;
            step = s;
            happiness = h;
            time = t;
            source = sc;
        }
        ~Path_node(){}
        int id;
        int step;
        int happiness;
        int time;
        int source;
        //friend std::ostream &operator<<(std::ostream &s, Path_node p);
};

//std::ostream &operator<<(std::ostream &s, Path_node p){
//    s<<"{ "<<p.id<<", "<<p.step<<", "<<p.happiness<<", "<<p.time<<", "<<p.source<<" }\n";
//    return s;
//}

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

bool compare_func(const Path_node& i, const Path_node& j){
    return (i.step < j.step);
}

bool compare_func_decending_time(const Path_node& i, const Path_node& j){
    return (i.time > j.time);
}

class Travel{
    public:
        Travel(const int& n, const int& l, const int& b,const int& s, const bool& tc){
            nodes = n;
            links = l;
            budget = b;
            start_time = s;
            end_time = s+b;
            happiness = 0;
            cost = 0;
            time_consider = !tc;
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
            if(time_consider||(nodes_op_time[0].first<=start_time && start_time<=nodes_op_time[0].second)){
                happiness += nodes_happiness_id[0];
                nodes_happiness_id[0] = 0;
            }

            int from_idx = 0;
            int current_time = start_time;
            while(find_path(from_idx, current_time));
            ending_path(from_idx, current_time);
            for(int i=current_time;i<=end_time;i++){
                if(time_consider||(nodes_op_time[0].first<=i && i<=nodes_op_time[0].second)){
                    happiness += nodes_happiness_id[0];
                    nodes_happiness_id[0] = 0;
                    break;
                }
            }
        }
        void output(){
            out_file<<happiness<<" "<<cost<<"\n";
            int s = path.size();
            for(int i=0;i<s;i++){
                out_file<<nodes_name[path[i]]<<" "<<start_time<<" "<<start_time;
                if(i!=s-1){
                    start_time=(start_time+link_weight[path[i]][path[i+1]])%DAY_TIME;
                    out_file<<"\n";
                }
            }
        }

    private:
        ///private variables
        //output data
        std::vector<int> path;
        int cost;
        int happiness;
        //variables
        int budget;
        int nodes, links;
        int start_time;
        int end_time;
        bool time_consider;
        int **min_dis;
        Node **node_table;

        ///tool functions
        void initialize(){
            ///build node_table
            //This table represent the adjacent nodes.
            //The first node represent how many nodes are adjacent to node[i].
            //(i.e. node_table[i]->num = how many node adjacent to node[i].)
            node_table = new Node*[nodes];
            for(int i=0;i<nodes;i++){
                node_table[i]=nullptr;
                int _count = 0;
                for(int j=0;j<nodes;j++){
                    if(link_weight[i][j]!=MAX_WEIGHT&&i!=j){
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
        void analys(const std::vector<Path_node>& nodes_vec, int& choice_step, int& choice_index){
            ///we are going to find the optimum value of every node.
            ///The formula_1 = (step + ports) * happiness.
            ///step is for decreasing iterations, and dead route avoidance.
            //debug
            int max_value=0;
            int max_idx=0;

            for(int i=1;i<nodes;i++){
                int s = nodes_vec[i].step;
                int h = nodes_vec[i].happiness;
                int t = nodes_vec[i].time;
                int p = (node_table[i]->num)/2;
                int value = (s+p)*h;//count formula.
                if(value>=max_value&&t<=budget){
                    max_value = value;
                    max_idx = i;
                }
            }
            choice_index = max_idx;
            choice_step = nodes_vec[choice_index].step;
        }

        int get_happiness(const int& arrive_time, const int& id){
            if(time_consider||(arrive_time>=nodes_op_time[id].first && arrive_time<=nodes_op_time[id].second)){
                return nodes_happiness_id[id];
            }
            else return 0;
        }

        bool find_path(int& from, int& current_time){//optimum oriented
            ///1.
            std::queue<Path_node> path_tree;
            //optimum_table
            Path_node* optimum_table = new Path_node[nodes];
            for(int i=0;i<nodes;i++) optimum_table[i] = {i, -1, -1, -1, -1};
            optimum_table[from] = {from, 0, 0, 0, from};
            //node_table
            Node* np = node_table[from]->next;
            while(np != nullptr){
                int i = np->num;
                int renew_time = link_weight[from][i];
                int real_time = (current_time+renew_time)%DAY_TIME;
                int renew_happiness = get_happiness(real_time, i);
                path_tree.push({i, 1, renew_happiness, renew_time, from});
                np = np->next;
            }

            //BFS
            while(!path_tree.empty()){
                Path_node p = path_tree.front();
                path_tree.pop();

                int id = p.id;
                int step = p.step;
                int acc_happ = p.happiness;
                int acc_time = p.time;
                int source = p.source;
                bool flag=0;

                if(optimum_table[id].step == -1){
                    optimum_table[id] = {id, step, acc_happ, acc_time, source};
                    flag=1;
                }
                else{
                    if(acc_time < optimum_table[id].time && acc_happ > optimum_table[id].happiness){
                        optimum_table[id] = {id, step, acc_happ, acc_time, source};
                        flag=1;
                    }
                }

                //get next
                if(flag&&step<nodes){//skewed case
                    std::vector<Path_node> temp_vec;
                    Node* n = node_table[id]->next;
                    while(n != nullptr){
                        int i = n->num;
                        if(i==from){ n=n->next;continue;}
                        int renew_time = acc_time+link_weight[id][i];
                        int real_time = (current_time+renew_time)%DAY_TIME;
                        int renew_happiness = acc_happ+get_happiness(real_time, i);
                        temp_vec.push_back({i, step+1, renew_happiness, renew_time, id});
                        n = n->next;
                    }
                    //sort
                    std::sort(temp_vec.begin(), temp_vec.end(), compare_func_decending_time);
                    int s = temp_vec.size();
                    for(auto it = temp_vec.begin();it!=temp_vec.end();it++) path_tree.push(*it);
                }
            }
            ///2.
            //sort the optimum_table
            std::vector<Path_node> optimum_vec;
            for(int i=0;i<nodes;i++) optimum_vec.push_back(optimum_table[i]);
            std::sort(optimum_vec.begin(), optimum_vec.end(), compare_func);

            //route
            int* route = new int[nodes];
            int route_size=0;
            int choice_step;
            int choice_index;

            analys(optimum_vec, choice_step, choice_index);
            if(choice_index == 0) return 0;//from

            //run
            int last_id = optimum_vec[choice_index].id;
            int find_id = optimum_vec[choice_index].source;
            choice_index--;

            route[route_size++] = last_id;
            while(optimum_vec[choice_index].step==choice_step){choice_index--;}

            while(choice_step--){
                //get the next node.
                while(optimum_vec[choice_index].step == choice_step){
                    int id = optimum_vec[choice_index].id;
                    if(find_id ==id){
                        route[route_size++] = optimum_vec[choice_index].id;
                        find_id = optimum_vec[choice_index].source;
                        last_id = id;
                        break;
                    }
                    choice_index--;
                }
                while(optimum_vec[choice_index].step==choice_step){choice_index--;}
            }
            //prune the path to fit size.
            int idx;
            bool prune_flag = prune(from, route, route_size, idx);
            
            //renew
            int route_happ = optimum_table[route[idx]].happiness;
            int route_time = optimum_table[route[idx]].time;
            cost+=route_time;
            budget-=route_time;
            happiness+=route_happ;
            current_time=(current_time+route_time)%DAY_TIME;
            bool happ_exhausting = (route_happ==0);
            
            //get path
            get_path(route, idx, route_size-1);
            from = route[idx];
            delete []route;
            delete []optimum_table;
            return (prune_flag&&happ_exhausting);
        }

        void ending_path(const int& from, int& current_time){//shortest oriented.
            if(from == 0) return;
            ///1.
            std::queue<Path_node> path_tree;
            //optimum_table
            Path_node* optimum_table = new Path_node[nodes];
            for(int i=0;i<nodes;i++) optimum_table[i] = {i, -1, -1, -1, -1};
            optimum_table[from] = {from, 0, 0, 0, from};
            //node_table
            Node* np = node_table[from]->next;
            while(np != nullptr){
                int i = np->num;
                int renew_time = link_weight[from][i];
                int real_time = (current_time+renew_time)%DAY_TIME;
                int renew_happiness = get_happiness(real_time, i);
                path_tree.push({i, 1, renew_happiness, renew_time, from});
                np = np->next;
            }

            //BFS
            while(!path_tree.empty()){
                Path_node p = path_tree.front();
                path_tree.pop();

                int id = p.id;
                int step = p.step;
                int acc_happ = p.happiness;
                int acc_time = p.time;
                int source = p.source;
                bool flag=0;

                if(optimum_table[id].step == -1){
                    optimum_table[id] = {id, step, acc_happ, acc_time, source};
                    flag=1;
                }
                else{
                    if(acc_time < optimum_table[id].time){
                        optimum_table[id] = {id, step, acc_happ, acc_time, source};
                        flag=1;
                    }
                }

                //get next
                if(flag&&step<nodes){
                    Node* n = node_table[id]->next;
                    while(n != nullptr){
                        int i = n->num;
                        if(i==from){ n=n->next;continue;}

                        int renew_time = acc_time+link_weight[id][i];
                        int real_time = (current_time+renew_time)%DAY_TIME;
                        int renew_happiness = acc_happ+get_happiness(real_time, i);
                        path_tree.push({i, step+1, renew_happiness, renew_time, id});
                        n = n->next;
                    }
                }
            }
            ///2.
            //sort the optimum_table
            std::vector<Path_node> optimum_vec;
            for(int i=0;i<nodes;i++) optimum_vec.push_back(optimum_table[i]);
            std::sort(optimum_vec.begin(), optimum_vec.end(), compare_func);

            //route
            int* route = new int[nodes];
            int route_size = 0;
            int choice_index;
            int choice_step;
            for(int i=0;i<nodes;i++){
                if(optimum_vec[i].id == 0){
                    choice_index = i;
                    choice_step = optimum_vec[i].step;
                }
            }

            //run
            int last_id = optimum_vec[choice_index].id;
            int find_id = optimum_vec[choice_index].source;
            choice_index--;

            route[route_size++] = last_id;
            while(optimum_vec[choice_index].step==choice_step){choice_index--;}

            while(choice_step--){
                //get the next node.
                while(optimum_vec[choice_index].step == choice_step){
                    int id = optimum_vec[choice_index].id;
                    if(find_id ==id){
                        route[route_size++] = optimum_vec[choice_index].id;
                        find_id = optimum_vec[choice_index].source;
                        last_id = id;
                        break;
                    }
                    choice_index--;
                }
                while(optimum_vec[choice_index].step==choice_step){choice_index--;}
            }
            //renew
            int route_happ = optimum_table[route[0]].happiness;//back_point.
            int route_time = optimum_table[route[0]].time;
            cost+=route_time;
            budget-=route_time;
            happiness+=route_happ;
            current_time=(current_time+route_time)%DAY_TIME;

            //get path
            get_path(route, 0, route_size-1);
            delete []route;
            delete []optimum_table;
        }

        bool prune(const int& from, int* route, const int& route_size, int& idx){
            //special case
            if(route_size==1){idx = 0;return 0;}
            //normal case
            bool flag = 1;
            int n = from;
            int i;
            int v_b = budget;

            for(i=route_size-2;i>=0;i--){
                v_b -= link_weight[n][route[i]];
                if(min_dis[0][route[i]]>v_b){
                    flag = 0;
                    break;
                }
                n=route[i];
            }
            idx = i+1;
            return flag;
        }
        void get_path(int* route, const int& start_idx, const int& end_idx){
            for(int i=end_idx-1;i>=start_idx;i--){
                path.push_back(route[i]);
                nodes_happiness_id[route[i]]=0;
            }
        }
};

int main(int argc, char *argv[]){
    ///case 1.
    ///file settings.
    char buffer[30];
    sprintf(buffer, "./%s/tp.data", argv[1]);
    in_file.open(buffer,std::ios::in);
    sprintf(buffer, "./%s/ans1.txt", argv[1]);
    out_file.open(buffer,std::ios::out);
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
    Travel t1(nodes, links, budget, astart_time, 0);
    t1.solve();//std::cout<<"#";
    t1.output();//std::cout<<"#\n";
    //close files and finish
    in_file.close();
    out_file.close();

    ///case 2.
    ///file settings.
    sprintf(buffer, "./%s/tp.data", argv[1]);
    in_file.open(buffer,std::ios::in);
    sprintf(buffer, "./%s/ans2.txt", argv[1]);
    out_file.open(buffer,std::ios::out);
    //input
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
    Travel t2(nodes, links, budget, astart_time, 1);
    t2.solve();//std::cout<<"#";
    t2.output();//std::cout<<"#";
    //close files and finish
    in_file.close();
    out_file.close();
    return 0;
}
