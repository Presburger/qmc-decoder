#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <assert.h>
#include <algorithm>

#define THREAD_NUM 4

using namespace std;
class Seed{
    public:
        Seed():x(-1),y(8),dx(1),index(-1){
            seedMap={
                {0x4a, 0xd6, 0xca, 0x90, 0x67, 0xf7, 0x52},
                {0x5e, 0x95, 0x23, 0x9f, 0x13, 0x11, 0x7e},
                {0x47, 0x74, 0x3d, 0x90, 0xaa, 0x3f, 0x51},
                {0xc6, 0x09, 0xd5, 0x9f, 0xfa, 0x66, 0xf9},
                {0xf3, 0xd6, 0xa1, 0x90, 0xa0, 0xf7, 0xf0},
                {0x1d, 0x95, 0xde, 0x9f, 0x84, 0x11, 0xf4},
                {0x0e, 0x74, 0xbb, 0x90, 0xbc, 0x3f, 0x92},
                {0x00, 0x09, 0x5b, 0x9f, 0x62, 0x66, 0xa1}
            };
        }

        uint8_t NextMask(){
            uint8_t ret;
            index++;
            if(x<0)
            {
                dx = 1;
                y = (8-y)%8;
                ret = (8 -y)%8;
                ret = 0xc3;
            }else if(x > 6){
                dx = -1;
                y= 7-y;
                ret = 0xd8;
            }else{
                ret = seedMap[y][x];
            }

            x += dx;
            if(index==0x8000||(index>0x8000&&(index+1)%0x8000==0))
                return NextMask();
            return ret;
        }


    private:

        int x;
        int y;
        int dx;
        int index;
        vector<vector<uint8_t> > seedMap;
};


void process(string dir)
{
    std::cout<<"decode "<<dir<<std::endl;
    std::fstream infile(dir.c_str(),std::ios::in|std::ios::binary);
    if(!infile.is_open())
    {
        cout<<"qmc file read error"<<endl;
        return;
    }


    string outloc(std::move(dir));

    string bak;
    char x;
    while((x=outloc.back())!='.')
    {
        bak.push_back(x);
        outloc.pop_back();
    }

    if(bak!="0cmq"&&bak!="calfcmq"&&bak!="3cmq")
        return;

    assert(bak.size()>3);
    for(int u=0;u<3;++u)
        bak.pop_back();
    std::reverse(bak.begin(),bak.end());
    if(bak=="0"||bak=="3")
        bak="mp3";
    outloc+=bak;


    auto len = infile.seekg(0,std::ios::end).tellg();
    infile.seekg(0,ios::beg);
    char * buffer =new char[len];

    infile.read(buffer,len);
    infile.close();

    Seed seed;
    for(int i=0;i<len;++i)
    {
        buffer[i]=seed.NextMask()^buffer[i];
    }

    std::fstream outfile(outloc.c_str(),ios::out|ios::binary);

    if(outfile.is_open())
    {
        outfile.write(buffer,len);
        outfile.close();
    }else
    {
        cerr<<"open dump file error"<<endl;
    }
    delete[] buffer;


}

void thread_block(int argc,char ** argv,int id,int td_n){
    for(int i=id;i<argc-1;i=i+td_n)
    {
        process(std::string(argv[i+1]));
    }
}


int main(int argc,char ** argv){

    if(argc<2)
    {
        std::cout<<"./decoder <qmcfile> ...."<<std::endl;
        return 1;
    }

    std::vector<std::thread> td_group;
    for(int i=1;i<THREAD_NUM;++i)
        td_group.emplace_back(thread_block,argc,argv,i,THREAD_NUM);
    thread_block(argc,argv,0,THREAD_NUM);
    for(auto &&x: td_group)
        x.join();


    return 0;
}
