# Travel planner
**<font color=#bf2222>Description：</font>**
In this project, I am going to deal with the trave-planning problem. We start with a file with some descriptions of a map, including the number of places, the number of paths, the start time, the time budget and the detail information od these places (happiness we would get if we go through these places, their open time, close time and trevel time between some pairs of places). See the input example below:
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
I am going to give one possible solution of this problem. Because this kind of question is to optimize rather than give a specific answear, there must be various way to solve it. So uhm, the following only represents my perspective toward this problem and it's absolutely not the best answer. But I am quite confident to my solution because I got 91 in this assignment, ha.

I use a simple list of steps to show how I optimize the answer:

**- Build the all-pair minimum distance**
**- Continuously call the <font color=#bf2222>find_path()</font> function until the time budget are rather depleted**

Let us focus on the <font color=#bf2222>find_path()</font> function, which is the most important part of this project. First, I use BFS to simulate the process of greedly going through all nodes, and try to find the best route based on the formula **(steps from start nodes + neighbor ports)×happiness** by covering improper results again and again. Then we would get a table of infomation about the time and budget to get that point. We sort this table according to the steps to get the ideal destination point and start the picking process. That is, we are going to pick the points along the virtual path ended by the ideal destination we just choose. Then we would get a sequence of points calling "partial path". We add the partial path into the output traveling list (vector), and that's the end. Call it againd and again, so we can maxmize using the time budget.

There is another important mechenism to prevent excessive use of time budget, which would lead to illegal output, building by the all-pair minimum distance table.About this part, you can search the Wikipedia to know more：
https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm

Refer to my code, and you can find more detail. Thanks to read this.
