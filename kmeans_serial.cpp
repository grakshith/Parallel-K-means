#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <bits/stdc++.h>

using namespace cv;
using namespace std;


//Number of Colors
#define COL 4


Mat image;

//Vector with size number of pixels * 3
vector<vector<int> > img;

//Vector to hold rgb values of final colors 
vector<vector<int> > colors;

//Map from color number to row number in img vector
map<int,vector<int> > cluster;



void initCentroids()
{
  vector<int> rgb;
  for(int i=0;i<COL;i++)
  {

    rgb.clear();
    rgb.push_back(rand()%255);
    rgb.push_back(rand()%255);
    rgb.push_back(rand()%255);

    colors.push_back(rgb); 
  }

  for(int i=0;i<colors.size();i++)
  {
      for(int j=0;j<colors[i].size();j++)
        cout<<colors[i][j]<<" ";
      cout<<endl;
  }

} 


int dis(vector<int> a,vector<int> b)
{
    return sqrt(pow(a[0]-b[0],2)+pow(a[1]-b[1],2)+pow(a[2]-b[2],2));
}

void kmeans()
{
    bool flag=true;
    int count=0;

    while(flag)
    {
        count++;
        cout<<"---------------------"<<endl;
        cout<<"Iteration:"<<count<<endl;

        cluster.clear();

        for(int i=0;i<img.size();i++)
        {
            
            int min_rgb=INT_MAX;
            int min_rgb_index=-1;
            for(int j=0;j<colors.size();j++)
            {
                  if(j==i)
                  continue;
                  int distance=dis(img[i],colors[j]);
                 
                  if(min_rgb>distance)
                  {
                    min_rgb=distance;
                    min_rgb_index=j;
                  }
            }
            if(cluster.find(min_rgb_index)==cluster.end())
            {    vector<int> v;
                 v.push_back(i);
                 cluster.insert(make_pair(min_rgb_index,v));
            }
            else
            cluster[min_rgb_index].push_back(i);

        }
        
        map<int,vector<int> >::iterator it;

        cout<<"Map key and Map value-size:"<<endl;

        for(it=cluster.begin();it!=cluster.end();it++)
        {  
            cout<<it->first<<":"<<it->second.size();
            // for(int j=0;j<it->second.size();j++)
            //   cout<<it->second[j]<<" ";
            cout<<endl;   
        }

        cout<<"New centroids:"<<endl;
        
        for(int i=0;i<COL;i++)
        {
            //cout<<img[cluster[0][1]][0]<<" ";
            int blue=0,green=0,red=0;
            for(int j=0;j<cluster[i].size();j++)
            {
                //cout<<img[cluster[i][j]][0]<<endl;
                blue+=img[cluster[i][j]][0];
                green+=img[cluster[i][j]][1];
                red+=img[cluster[i][j]][2];
            }

            vector<int> new_val;
            new_val.push_back(blue/cluster[i].size());
            new_val.push_back(green/cluster[i].size());
            new_val.push_back(red/cluster[i].size());

            cout<<new_val[0]<<" "<<new_val[1]<<" "<<new_val[2]<<endl;
    
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

    cout<<"---------------------"<<endl;
    cout<<"Final Colors chosen:"<<endl;

    for(int i=0;i<colors.size();i++)
    {
      for(int j=0;j<colors[i].size();j++)
        cout<<colors[i][j]<<" ";
      cout<<endl;
    }

}

//Give image as argument when executing

int main( int argc, char** argv )
{
    if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   

    if(! image.data )                              
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    namedWindow( "Initial Image", WINDOW_AUTOSIZE );
    
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

    cout<<"Map rows:"<<img.size()<<"  Map Cols:"<<img[0].size()<<endl;
    imshow( "Initial Image", image );                
   
    cout<<"Image Rows:"<<image.rows<<" "<<"Image Cols:"<<image.cols<<endl;

    initCentroids();
    kmeans();
   
    for(int i=0;i<COL;i++)
    {  
        for(int j=0;j<cluster[i].size();j++)
        {
            int x=cluster[i][j] / image.cols;
            int y=cluster[i][j] % image.cols;
            Vec3b intensity = image.at<Vec3b>(x,y); 
            intensity.val[0]=(uchar)colors[i][0];
            intensity.val[1]=(uchar)colors[i][1];
            intensity.val[2]=(uchar)colors[i][2];
            image.at<Vec3b>(x,y) = intensity;
        }
    
    }
    imwrite("Final_Image_1.jpg",image);
    namedWindow( "Final image", CV_WINDOW_AUTOSIZE );

    imshow( "Final image", image );
    waitKey(0);                                       
    return 0;
}