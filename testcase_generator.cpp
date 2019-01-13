#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <time.h>

#define OUT_FILE_NAME "tp.data"
#define IN_FILE_NAME "tp.data"
#define MAX_HAPPINESS 500
#define MIN_HAPPINESS 0
#define MAX_COST 500
#define MIN_COST 0
#define MAX_DIFF 100
#define MIN_DIFF 0
// weight define
//nodes
#define A_WEIGHT 500
#define B_WEIGHT 400
#define C_WEIGHT 300
#define D_WEIGHT 200
#define E_WEIGHT 100
#define F_WEIGHT 0
//links
#define X_WEIGHT 500
#define Y_WEIGHT 400
#define Z_WEIGHT 300
#define W_WEIGHT 200
#define S_WEIGHT 10

using namespace std;
ofstream out_file;
ifstream in_file;

class TCG{
    public:
        TCG(){}
        ~TCG(){}
        void generate_skewed_graph(int nodes, int budget, int start_time){
            srand( time(NULL) );

            int links = nodes-1;
            out_file<<nodes<<" "<<links<<" "<<budget<<" "<<start_time<<"\n";
            for(int i=0;i<nodes;i++){
                int happiness = rand()%(MAX_HAPPINESS-MIN_HAPPINESS+1)+MIN_HAPPINESS;
                out_file<<name_encoder(i)<<" "<<happiness<<" 0 1440\n";
            }
            for(int i=0;i<links;i++){
                int cost = rand()%40;
                out_file<<name_encoder(i)<<" "<<name_encoder(i+1)<<" "<<cost<<"\n";
            }
        }
        void generate_multiroute_graph(int nodes, int budget, int start_time){
            srand( time(NULL) );//nodes = 4n+1
            int links = (nodes-1)*6/4;
            int right_blocks = (((nodes-1)/4)+1)/2;
            int left_blocks = (((nodes-1)/4))/2;
            int blocks = ((nodes-1)/4);

            out_file<<nodes<<" "<<links<<" "<<budget<<" "<<start_time<<"\n";
            for(int i=0;i<nodes;i++){
                int happiness = rand()%(MAX_HAPPINESS-MIN_HAPPINESS+1)+MIN_HAPPINESS;
                out_file<<name_encoder(i)<<" "<<happiness<<" 0 1440\n";
            }
            //right
            for(int i=0;i<right_blocks;i++){
                int start_pt = i*4;
                int end_pt = (i+1)*4;

                for(int j=1;j<=3;j++){
                    int cost = rand()%(MAX_COST-MIN_COST+1)+MIN_COST;
                    int diff = rand()%(MAX_DIFF-MIN_DIFF+1)+MIN_DIFF;
                    out_file<<name_encoder(start_pt)<<" "<<name_encoder(start_pt+j)<<" "<<cost<<"\n";
                    out_file<<name_encoder(start_pt+j)<<" "<<name_encoder(end_pt)<<" "<<cost+diff<<"\n";
                }
            }
            //left
            int start_pt = 0;
            int end_pt = (right_blocks+1)*4;
            for(int j=3;j>=1;j--){
                int cost = rand()%(MAX_COST-MIN_COST+1)+MIN_COST;
                int diff = rand()%(MAX_DIFF-MIN_DIFF+1)+MIN_DIFF;
                out_file<<name_encoder(start_pt)<<" "<<name_encoder(end_pt-j)<<" "<<cost<<"\n";
                out_file<<name_encoder(end_pt-j)<<" "<<name_encoder(end_pt)<<" "<<cost+diff<<"\n";
            }
            for(int i=1;i<left_blocks;i++){
                start_pt = (i+right_blocks)*4;
                end_pt = (i+right_blocks+1)*4;

                for(int j=1;j<=3;j++){
                    int cost = rand()%(MAX_COST-MIN_COST+1)+MIN_COST;
                    int diff = rand()%(MAX_DIFF-MIN_DIFF+1)+MIN_DIFF;
                    out_file<<name_encoder(start_pt)<<" "<<name_encoder(start_pt+j)<<" "<<cost<<"\n";
                    out_file<<name_encoder(start_pt+j)<<" "<<name_encoder(end_pt)<<" "<<cost+diff<<"\n";
                }
            }
        }
        void generate_tricky_graph(int nodes, int budget, int start_time){
            //nodes = 4n+1
            int links = (nodes-1)*6/4;
            int right_blocks = (((nodes-1)/4)+1)/2;
            int left_blocks = (((nodes-1)/4))/2;
            int blocks = ((nodes-1)/4);

            out_file<<nodes<<" "<<links<<" "<<budget<<" "<<start_time<<"\n";
            ///nodes
            int happiness_init = A_WEIGHT;
            int op_time_init = start_time+(X_WEIGHT + Y_WEIGHT + 2*S_WEIGHT)*(right_blocks)-5;
            int close_time_init = start_time+(X_WEIGHT + Y_WEIGHT + 2*S_WEIGHT)*(right_blocks)+5;
            out_file<<name_encoder(0)<<" "<<happiness_init<<" "<<op_time_init<<" "<<close_time_init<<"\n";

            int back_time = start_time + (X_WEIGHT + S_WEIGHT)*right_blocks;
            //right
            for(int i=0;i<right_blocks;i++){//route
                int start_pt = i*4;

                int happiness1 = B_WEIGHT;
                int op_time_1 = start_time+(X_WEIGHT*(i+1)+S_WEIGHT*i)-5;
                int close_time_1 = start_time+(X_WEIGHT*(i+1)+S_WEIGHT*i)+5;
                out_file<<name_encoder(start_pt+1)<<" "<<happiness1<<" "<<op_time_1<<" "<<close_time_1<<"\n";

                int happiness2 = C_WEIGHT;
                int op_time_2 = back_time+(S_WEIGHT*(right_blocks-i)+Y_WEIGHT*(right_blocks-i-1))-5;
                int close_time_2 = back_time+(S_WEIGHT*(right_blocks-i)+Y_WEIGHT*(right_blocks-i-1))+5;
                out_file<<name_encoder(start_pt+2)<<" "<<happiness2<<" "<<op_time_2<<" "<<close_time_2<<"\n";

                int happiness3 = D_WEIGHT;
                int op_time_3 = start_time+(X_WEIGHT*(i+1)+S_WEIGHT*i)-5;
                int close_time_3 = start_time+(X_WEIGHT*(i+1)+S_WEIGHT*i)+5;
                out_file<<name_encoder(start_pt+3)<<" "<<happiness3<<" "<<op_time_3<<" "<<close_time_3<<"\n";

                int happiness4 = A_WEIGHT;
                int op_time_4 = (start_time+(X_WEIGHT+S_WEIGHT)*(i+1))-5;
                int close_time_4 = (start_time+(X_WEIGHT+S_WEIGHT)*(i+1))+5;
                out_file<<name_encoder(start_pt+4)<<" "<<happiness4<<" "<<op_time_4<<" "<<close_time_4<<"\n";
            }
            //left
            for(int i=0;i<left_blocks;i++){//barrier
                int start_pt = (i+right_blocks)*4;
                if(i<=left_blocks/2){
                    int happiness1 = F_WEIGHT;
                    out_file<<name_encoder(start_pt+1)<<" "<<happiness1<<" 0 10000\n";
                    int happiness2 = F_WEIGHT;
                    out_file<<name_encoder(start_pt+2)<<" "<<happiness2<<" 0 10000\n";
                    int happiness3 = A_WEIGHT;
                    out_file<<name_encoder(start_pt+3)<<" "<<happiness3<<" 0 10000\n";
                    int happiness4 = A_WEIGHT;
                    out_file<<name_encoder(start_pt+4)<<" "<<happiness4<<" 0 10000\n";
                }
                else{
                    int happiness1 = F_WEIGHT;
                    out_file<<name_encoder(start_pt+1)<<" "<<happiness1<<" 0 10\n";
                    int happiness2 = F_WEIGHT;
                    out_file<<name_encoder(start_pt+2)<<" "<<happiness2<<" 0 10\n";
                    int happiness3 = E_WEIGHT;
                    out_file<<name_encoder(start_pt+3)<<" "<<happiness3<<" 0 10\n";
                    int happiness4 = E_WEIGHT;
                    out_file<<name_encoder(start_pt+4)<<" "<<happiness4<<" 0 10\n";
                }
            }
            ///links
            //right
            for(int i=0;i<right_blocks;i++){
                int start_pt = i*4;
                int end_pt = (i+1)*4;
                out_file<<name_encoder(start_pt)<<" "<<name_encoder(start_pt+1)<<" "<<X_WEIGHT<<"\n";
                out_file<<name_encoder(start_pt+1)<<" "<<name_encoder(end_pt)<<" "<<S_WEIGHT<<"\n";

                out_file<<name_encoder(start_pt)<<" "<<name_encoder(start_pt+2)<<" "<<Y_WEIGHT<<"\n";
                out_file<<name_encoder(start_pt+2)<<" "<<name_encoder(end_pt)<<" "<<S_WEIGHT<<"\n";

                out_file<<name_encoder(start_pt)<<" "<<name_encoder(start_pt+3)<<" "<<X_WEIGHT<<"\n";
                out_file<<name_encoder(start_pt+3)<<" "<<name_encoder(end_pt)<<" "<<X_WEIGHT<<"\n";
            }
            //left
            int start_pt = 0;
            int end_pt = (right_blocks+1)*4;
            for(int j=3;j>=1;j--){
                int cost = W_WEIGHT;
                out_file<<name_encoder(start_pt)<<" "<<name_encoder(end_pt-j)<<" "<<cost<<"\n";
                out_file<<name_encoder(end_pt-j)<<" "<<name_encoder(end_pt)<<" "<<cost<<"\n";
            }
            for(int i=1;i<left_blocks;i++){
                start_pt = (i+right_blocks)*4;
                end_pt = (i+right_blocks+1)*4;
                for(int j=1;j<=3;j++){
                    int cost = X_WEIGHT;
                    out_file<<name_encoder(start_pt)<<" "<<name_encoder(start_pt+j)<<" "<<cost<<"\n";
                    out_file<<name_encoder(start_pt+j)<<" "<<name_encoder(end_pt)<<" "<<cost<<"\n";
                }
            }
        }
        void check(){
            int nodes, links;
            int budget, start_time;
            in_file>>nodes>>links>>budget>>start_time;
            if(nodes>100){cout<<"Wrong nodes!\n";return;}
            string nodes_name[100];
            int link_weight[100][100];
            for(int i=0;i<100;i++)
                for(int j=0;j<100;j++)
                    link_weight[i][j]=-1;

            for(int i=0;i<nodes;i++){
                int happiness, start_time, end_time;

                in_file>>nodes_name[i]>>happiness>>start_time>>end_time;
                if(happiness<0||happiness>500){cout<<"Wrong happiness!\n";return;}
                if(start_time<0||start_time>2147483647||end_time<0||end_time>2147483647||start_time>end_time){
                        cout<<"Wrong time!\n";return;
                }
                for(int j=0;j<i;j++){
                    if(nodes_name[i] == nodes_name[j]){cout<<"Wrong name!\n";return;}
                }
            }
            for(int i=0;i<links;i++){
                std::string from, to;
                int weight;
                in_file>>from>>to>>weight;

                int flag=0;
                int f_idx=-1, t_idx=-1;
                for(int i=0;i<nodes&&flag!=2;i++){
                    if(nodes_name[i]==from){
                        f_idx = i;
                        flag++;
                    }
                    else if(nodes_name[i]==to){
                        t_idx = i;
                        flag++;
                    }
                }
                if(f_idx == -1||t_idx == -1||f_idx == t_idx){cout<<"Wrong idx!\n";return;}
                if(link_weight[f_idx][t_idx] == -1) link_weight[f_idx][t_idx]=weight;
                else {cout<<"Wrong weight!\n";return;}
                if(link_weight[t_idx][f_idx] == -1) link_weight[t_idx][f_idx]=weight;
                else {cout<<"Wrong weight!\n";return;}
            }
            int c = 0;
            in_file>>c;
            if(c!=0){cout<<"Wrong graph!\nc is: "<<c<<"\n";return;}
            cout<<"Complete!\n";
        }

    private:
        string name_encoder(int index){
            string s("AA");
            s[1]+=(index%26);
            s[0]+=(index/26);
            return s;
        }
};

int main(void){
    //write.
    out_file.open(OUT_FILE_NAME, ios::out);
    TCG tcg;
    //tcg.generate_skewed_graph(100, 1000, 480);
    //tcg.generate_multiroute_graph(97, 10000, 480);
    tcg.generate_tricky_graph(97, 11520, 480);
    out_file.close();
    //read.
    in_file.open(IN_FILE_NAME, ios::in);
    tcg.check();
    in_file.close();
    return 0;
}
