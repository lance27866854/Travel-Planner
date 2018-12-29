#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <time.h>

#define OUT_FILE_NAME "tp.data"
#define MAX_HAPPINESS 500
#define MIN_HAPPINESS 0

using namespace std;
ofstream out_file;

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

    private:
        string name_encoder(int index){
            string s("AA");
            s[1]+=(index%26);
            s[0]+=(index/26);
            return s;
        }
};

int main(void){
    out_file.open(OUT_FILE_NAME, ios::out);

    TCG tcg;
    tcg.generate_skewed_graph(100, 1000, 480);

    out_file.close();
    return 0;
}
