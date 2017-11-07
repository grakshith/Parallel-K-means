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
#include <omp.h>

#include<bits/stdc++.h>



using namespace cv;
using namespace std;


//Number of Colors
#define COL 16


Mat image;

//Vector with size number of pixels * 3
vector<vector<int> > img;

//Vector to hold rgb values of final colors 
vector<vector<int> > colors;

//Map from color number to row number in img vector
map<int,vector<int> > cluster; //compressed image





void write_compressed(map<int,vector<int> > a,vector < vector <int> > b,int row,int col, string filepath)

{
    std::ofstream ofs(filepath.c_str());
    boost::archive::text_oarchive oa(ofs);
    // oa & a;
    // oa & b;
    // oa & col;
    oa<<a<<b<<row<<col;
}

void write_vector(vector < vector <int> > a, string filepath)

{
    std::ofstream ofs(filepath.c_str());
    boost::archive::text_oarchive oa(ofs);
    oa & a;
}


std::string remove_extension(const std::string& filename) {
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot); 
}

void initCentroids()
{
//   vector<int> rgb;
  for(int i=0;i<COL;i++)
  {

    // rgb.clear();
    // rgb.push_back(rand()%255);
    // rgb.push_back(rand()%255);
    // rgb.push_back(rand()%255);

    // colors.push_back(rgb);
    
    
    /**************/

    int rand_index = rand()%img.size();
    
    colors.push_back(img[rand_index]);

    // vector<vector<int> > duplicate (img.begin(),img.end()) ;

    // for(int i=0;i<duplicate.size();i++)
    // {
    //     for(int j=0;j<colors[i].size();j++)
    //       cout<<colors[i][j]<<" ";
    //     cout<<endl;
    // }

  }

  /*for(int i=0;i<colors.size();i++)
  {
      for(int j=0;j<colors[i].size();j++)
        cout<<colors[i][j]<<" ";
      cout<<endl;
  }*/

} 


int dis(vector<int> a,vector<int> b)
{
    return sqrt(pow(a[0]-b[0],2)+pow(a[1]-b[1],2)+pow(a[2]-b[2],2));
}

//struct Compare { int val; int index; }min1;    

//#pragma omp declare reduction(minimum : struct Compare : omp_out = omp_in.val < omp_out.val ? omp_in : omp_out) initializer (omp_priv={INT_MAX,-1})

void kmeans()
{

    double start=omp_get_wtime();

    bool flag=true;
    int count=0;

    while(flag&&count<100)
    {
        count++;
        //cout<<"---------------------"<<endl;
        //cout<<"Iteration:"<<count<<endl;

        cluster.clear();

        //struct Compare min; 
		
		int min_rgb,min_rgb_index;

        #pragma omp parallel for private(min_rgb,min_rgb_index)
        for(int i=0;i<img.size();i++)
        {
        	min_rgb=INT_MAX;
            min_rgb_index=-1;
			
            for(int j=0;j<colors.size();j++)
            {
                  if(j==i)
                  continue;
                  int distance=dis(img[i],colors[j]);
                 
                  //if(j==1 && i==0)
                  //cout<<min1.val<<" "<<distance<<endl;


	                  if(min_rgb>distance)
	                  {
	                    min_rgb=distance;
	                    min_rgb_index=j;
	                  }
              	 
            }
            #pragma omp critical
            {
                if(cluster.find(min_rgb_index)==cluster.end())
                {    vector<int> v;
                     v.push_back(i);
                     cluster.insert(make_pair(min_rgb_index,v));
                }
                else
                cluster[min_rgb_index].push_back(i);
            }
        }
        
        map<int,vector<int> >::iterator it;

        /*cout<<"Map key and Map value-size:"<<endl;

        for(it=cluster.begin();it!=cluster.end();it++)
        {  
            cout<<it->first<<":"<<it->second.size();
            // for(int j=0;j<it->second.size();j++)
            //   cout<<it->second[j]<<" ";
            cout<<endl;   
        }*/

        //cout<<"New centroids:"<<endl;
        
        for(int i=0;i<COL;i++)
        {
            //cout<<img[cluster[0][1]][0]<<" ";
            int blue=0,green=0,red=0,j=0;
            #pragma omp parallel for reduction(+:red,green,blue) private(j)
            for(j=0;j<cluster[i].size();j++)
            {
                //cout<<img[cluster[i][j]][0]<<endl;
                blue+=img[cluster[i][j]][0];
                green+=img[cluster[i][j]][1];
                red+=img[cluster[i][j]][2];
            }

            vector<int> new_val;
            if(cluster[i].size())
            {
                new_val.push_back(blue/cluster[i].size());
                new_val.push_back(green/cluster[i].size());
                new_val.push_back(red/cluster[i].size());
            }
            else
            {
                //cout<<"Cluster size 0, hence random initialization"<<endl;
                // new_val.push_back(rand()%255);
                // new_val.push_back(rand()%255);
                // new_val.push_back(rand()%255);
                int rand_index = rand()%img.size();
                new_val = img[rand_index];
            }

            //cout<<new_val[0]<<" "<<new_val[1]<<" "<<new_val[2]<<endl;
    
            for(int k=0;k<3;k++)
            {
                if(new_val[k]!=colors[i][k])
                {
                    colors[i][k]=new_val[k];
                    flag=flag && false;
                }
                else
                  flag=flag && true;
            }
        }
        flag=!flag;
        

    }

    //cout<<"---------------------"<<endl;
    //cout<<"Final Colors chosen:"<<endl;

    /*for(int i=0;i<colors.size();i++)
    {
      for(int j=0;j<colors[i].size();j++)
        cout<<colors[i][j]<<" ";
      cout<<endl;
    }*/

    double end=omp_get_wtime();

    cout<<"Time taken:"<<end-start<<endl;

}

//Give image as argument when executing

int main( int argc, char** argv )
{
    if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    
    //loading image
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   

    if(! image.data )                              
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    
    namedWindow( "Initial Image", WINDOW_AUTOSIZE );
    
    imshow( "Initial Image", image );                
   
    // cout<<"Image Rows:"<<image.rows<<" "<<"Image Cols:"<<image.cols<<endl;



    //converting mat object image to 2d vector img - num_pixels X 3 
    vector<int> temp;
    for(int i=0;i<image.rows;i++)
    {
     
      for(int j=0;j<image.cols;j++)
      {
        temp.clear();
        Vec3b intensity = image.at<Vec3b>(i,j);
        temp.push_back((int)intensity.val[0]);
        temp.push_back((int)intensity.val[1]);
        temp.push_back((int)intensity.val[2]);
        img.push_back(temp);
      }
      
    }

    // cout<<"Map rows:"<<img.size()<<"  Map Cols:"<<img[0].size()<<endl;


    write_vector(img,remove_extension(argv[1]).append("_initial.bin"));

    initCentroids();
    kmeans();

    write_compressed(cluster,colors,image.rows,image.cols,remove_extension(argv[1]).append("_compressed.bin"));
    // write_vector(colors,remove_extension(argv[1]).append("_compressed_image.bin"));
    // write_map(cluster,remove_extension(argv[1]).append("_x.bin"));

   
   
    // for(int i=0;i<COL;i++)
    // {  
    //     for(int j=0;j<cluster[i].size();j++)
    //     {
    //         int x=cluster[i][j] / image.cols;
    //         int y=cluster[i][j] % image.cols;
    //         Vec3b intensity = image.at<Vec3b>(x,y); 
    //         intensity.val[0]=(uchar)colors[i][0];
    //         intensity.val[1]=(uchar)colors[i][1];
    //         intensity.val[2]=(uchar)colors[i][2];
    //         image.at<Vec3b>(x,y) = intensity;
    //     }
    
    // }
    // imwrite("Final_Image_1.jpg",image);
    // namedWindow( "Final image", CV_WINDOW_AUTOSIZE );

    // imshow( "Final image", image );
    // waitKey(0);                                       
    return 0;
}

