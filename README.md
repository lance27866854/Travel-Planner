# Travel planner
**<font color=#bf2222>Description of the task：</font>**
In this project, we aim to find the route with optimized value of happiness the in a given map. Imagining you are a traveler, and wanting to get a squence of sites to visite. However, as you gain the happiness from traveling those sites, you should also take limited time budget into account. For any given map conformed to the rules, we target writing a program to generate such a route by which to gain optimized happiness.

First, we will start with a file with some descriptions of a map, including the number of places, the number of paths, the start time, the time budget and the detail information of these places (happiness we would get if we go through these places, their open time, close time and trevel time between some pairs of places). See the input example below:

```
2 1 100 480
TPE 3 720 900
NTHU 4 0 1440
TPE NTHU 50

//This case describes a graph with two nodes.
//The quota of total traveling time is 100 minutes and the time to start traveling is 08:00(or 480 minutes after midnight).
//Node “TPE” is open between 12:00 ~ 15:00 and node “NTHU” is open all day. The traveling time between “TPE” and “NTHU” is 50 minutes.
```

Our goal is to maximize the result (the happiness) by entering the places within the appropriate time slot, and also output a file with detailed information about the traveling route and time. See the output example below:

```
7 100
TPE 480 480
NTHU 530 530
TPE 580 580
//A example of ideal traveling route.

3 0
TPE 480 480
//A example of no movement.
```

**<font color=#bf2222>Solution：</font>**
Rather than giving a specific answer, things become challenging as it requires us to find the possible optimum within limited time. So there must be a variety of ways to solve it. (You might also think of using reinforced learning algorithm, but the problem is the environment is not fixed.) We are going to give one solution for this problem. It's definitely not the best solution:

1. Build the all-pair minimum distance.
2. Continuously call the <font color=#bf2222>find_path()</font> function until the time budget are rather depleted.

Let us focus on the <font color=#bf2222>find_path()</font> function, which is the most important part of this project. First, I use BFS algorithm to simulate the process of greedly going through all nodes, and trying to find the best route based on the formula **(steps from start nodes + neighbor ports)×happiness** by covering improper results again and again.
```clike=
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
```

After that, we would get a table with infomation about the time and the budget to get to a certain point. We sort this table based on the steps to get the coordinate of the ideal destination before the picking process. That is, we are going to pick the points along the virtual path which is ended by the ideal destination that we just choose. Ultimately, we would get a sequence of points called "partial path", which is going to be added into the output traveling list. Call it againd and again, we can maxmize using the time budget without excess.

```clike=
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
    while(optimum_vec[choice_index].step==choice_step){
        choice_index--;
    }
}
```

There is an another important mechanism to prevent excessive use of time budget, which would lead to illegal output, by building the all-pair minimum distance table. More information on Wikipedia: https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm

```clike=
min_dis = new int* [nodes];
for(int i=0;i<nodes;i++) min_dis[i] = new int[nodes];

for(int i=0;i<nodes;i++){
    for(int j=0;j<nodes;j++){
        min_dis[i][j]=link_weight[i][j];
        if(i==j) min_dis[i][j]=0;
    }
}

for(int k=0;k<nodes;k++)
    for(int i=0;i<nodes;i++)
        for(int j=0;j<nodes;j++)
            if((min_dis[i][k]+min_dis[k][j])<min_dis[i][j]&&
            min_dis[i][k]!=MAX_WEIGHT&&min_dis[k][j]!=MAX_WEIGHT)
                min_dis[i][j] = min_dis[i][k]+min_dis[k][j];
```
