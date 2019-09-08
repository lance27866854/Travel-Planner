# Travel planner
**<font color=#bf2222>Description：</font>**
In this project, I aim to deal with the optimization of trave-planning problem. We start with a file with some descriptions of a map, including the number of places, the number of paths, the start time, the time budget and the detail information of these places (happiness we would get if we go through these places, their open time, close time and trevel time between some pairs of places). See the input example below:
```
2 1 100 480
TPE 3 720 900
NTHU 4 0 1440
TPE NTHU 50

//This case describes a graph with two nodes.
//The quota of total traveling time is 100 minutes and the time to start traveling is 08:00(or 480 minutes after midnight).
//Node “TPE” is open between 12:00 ~ 15:00 and node “NTHU” is open all day. The traveling time between “TPE” and “NTHU” is 50 minutes.
```
We need to maximize the result (the happiness) by entering the places in appropriate time within the time budget limit, and also output a file with detailed information about the traveling route and time. See the output example below:
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
I am going to give one possible solution of this problem. Because this kind of question is to optimize rather than give a specific answear, there must be various way to solve it. (You might also think of using supervised learning algo, but the problem is data collection.) So uhm, the following only represents my perspective toward this problem and it's absolutely not the best answer. But I am quite confident to my solution because I got 91 points (out of 100) in this assignment, ha.

I use a simple list of steps to show how I optimize the answer:
1. Build the all-pair minimum distance
2. Continuously call the <font color=#bf2222>find_path()</font> function until the time budget are rather depleted

Let us focus on the <font color=#bf2222>find_path()</font> function, which is the most important part of this project. First, I use BFS to simulate the process of greedly going through all nodes, and try to find the best route based on the formula **(steps from start nodes + neighbor ports)×happiness** by covering improper results again and again.
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
Then we would get a table of infomation about the time and the budget to get that point. We sort this table according to the steps to get the coordinate of the ideal destination and start the picking process. That is, we are going to pick the points along the virtual path ended by the ideal destination we just choose. Then we would get a sequence of points called "partial path". We add partial pathes into the output traveling list (vector), and that's the end. Call it againd and again, so we can maxmize using the time budget.
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
There is another important mechenism to prevent excessive use of time budget, which would lead to illegal output, building by the all-pair minimum distance table.About this part, you can search the Wikipedia to know more：
https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm
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
Refer to my code, and you can find more detail. Thanks to read this.
