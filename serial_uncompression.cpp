#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>

// include input and output archivers
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// include this header to serialize vectors
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/map.hpp>


#include<bits/stdc++.h>


using namespace cv;
using namespace std;


//Number of Colors
#define COL 4




//Vector with size number of pixels * 3
vector<vector<int> > img;

//Vector to hold rgb values of final colors 
vector<vector<int> > colors;

//Map from color number to row number in img vector
map<int,vector<int> > cluster; //compressed image





map<int,vector<int> > read_map(string filepath)
{
    map<int,vector<int> > a;
    std::ifstream ifs(filepath);
    boost::archive::text_iarchive ia(ifs);
    ia & a;
    return a;
}

vector < vector <int> > read_vector(string filepath)
{
    vector < vector <int> > a;
    std::ifstream ifs(filepath);
    boost::archive::text_iarchive ia(ifs);
    ia & a;
    return a;
}


std::string remove_extension(const std::string& filename) {
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot); 
}

int main( int argc, char** argv )
{
    if( argc != 2)
    {
     cout <<" Enter path to compressed image" << endl;
     return -1;
    }

    string imagepath = argv[1];
    int rows,cols;
    // string dictpath = argv[2];
    // cout<<filepath<<endl;

    std::ifstream ifs(imagepath);
    boost::archive::text_iarchive ia(ifs);
    ia>>cluster>>colors>>rows>>cols;

    // for(int i=0;i<colors.size();i++)
    // {
    //   for(int j=0;j<colors[i].size();j++)
    //     cout<<colors[i][j]<<" ";
    //   cout<<endl;
    // }

    // cout<<rows<<" "<<cols<<endl;
    Mat image(rows, cols, CV_8UC3, Scalar(0, 0, 0));
    // cluster = read_map(imagepath);
    // colors = read_vector(dictpath);

     for(int i=0;i<COL;i++)
    {  
        for(int j=0;j<cluster[i].size();j++)
        {
            int x=cluster[i][j] / cols;
            int y=cluster[i][j] % cols;
            Vec3b intensity = image.at<Vec3b>(x,y);
            intensity.val[0]=(uchar)colors[i][0];
            intensity.val[1]=(uchar)colors[i][1];
            intensity.val[2]=(uchar)colors[i][2];
            image.at<Vec3b>(x,y) = intensity;
        }
    
    }
    imwrite(remove_extension(argv[1]).append(".jpg"),image);
    
}
