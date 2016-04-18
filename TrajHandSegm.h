#ifndef TRAJHANDSEGM_H_
#define TRAJHANDSEGM_H_

#include "DenseTrack.h"
#include "Constants.h"

using namespace std;

class TrackSegm
{
public:
    vector<Point2f> point;
    vector<Point2f> trajectory;

    //int index = -1;

    int frame_num;
    //float mean_x;
    //float mean_y;
    
    /*TrackSegm(const Point2f& point_)
    {        
        //point.push_back(point_);
    }*/

    void addPoint(const Point2f& point_)
    {
        //index++;
        point.push_back(point_);        
    }

    void addTrajectory(const Point2f& traj_)
    {
    	trajectory.push_back(traj_);
    }

    void setFrameNum(int frame)
    {
    	frame_num = frame;
    }
};

list<TrackSegm> ExtractTrajectories(list<Track> xyTracks, int frame_num, int length)
{
	list<TrackSegm> segmTracks; 

	for(list<Track>::iterator iTrack = xyTracks.begin(); iTrack != xyTracks.end(); iTrack++)
	{		
		if(frame_num <= iTrack->frame_num && iTrack->frame_num <= frame_num + length - step)
		{
			TrackSegm track;
			track.setFrameNum(iTrack->frame_num);

			int shift = iTrack->frame_num - frame_num;
			int index = length - shift - step;

			for(int i = index; i < index + step; i++)
			{
				track.addPoint(iTrack->point[i]);
				track.addTrajectory(iTrack->trajectory[i]);
			}

			// add last point
			track.addPoint(iTrack->point[index + step]);

			segmTracks.push_back(track);
		}		
	}

	return segmTracks;
}


int CountTraj(list<Track> xyTracks, int frame_num, int length)
{
	int sum = 0;

	for(list<Track>::iterator iTrack = xyTracks.begin(); iTrack != xyTracks.end(); iTrack++)
		if(frame_num <= iTrack->frame_num && iTrack->frame_num <= frame_num + length - step)
			sum++;

	return sum;
}

void DrawTrajetory(const std::vector<Point2f>& point, Mat& image, int index)
{
	int j = 1;	

	Point2f point0 = point[j-1];

	for (j; j <= step; j++) {
		Point2f point1 = point[j];

		//line(image, point0, point1, Scalar(colors[index][0],cvFloor(colors[index][1]*(j+1.0)/float(step+1.0)),colors[index][2]), 1, 8, 0);
		line(image, point0, point1, Scalar(colors[index][0], colors[index][1], colors[index][2]), 1, 8, 0);
		point0 = point1;
	}
}

void DrawTrajetories(SeqInfo* seqInfo, list<TrackSegm> segmTracks, int indexOfMax, int* clusters)
{
	namedWindow("SegmentedTrajectories", 0);
	resizeWindow("SegmentedTrajectories", seqInfo->width * 3, seqInfo->height * 3);

	VideoCapture capture;
    capture.open(seqInfo->video);

    if(!capture.isOpened())
        fprintf(stderr, "Could not initialize capturing..\n");

    // find the needed frame and show it
    int frame_num = 0;
    while(true) {
        Mat frame;
        capture >> frame;

        if(frame.empty())
            break;

        if(indexOfMax == frame_num)
        {
        	// draw all segmented trajectories
        	int index = 0;
		    for(list<TrackSegm>::iterator iTrack = segmTracks.begin(); iTrack != segmTracks.end(); iTrack++)
		    {
		    	DrawTrajetory(iTrack->point, frame, clusters[index]);
		    	index++;
		    }

			imshow( "SegmentedTrajectories", frame);
			break;
        }

        frame_num++;
	}

    cvWaitKey(0);
    destroyWindow("SegmentedTrajectories");
}

bool DoesTrajSame(vector<Point2f> point1, vector<Point2f> trajectory1, vector<Point2f> point2, vector<Point2f> trajectory2)
{
	bool issame = true;

	trajectory1[0].x;

	for(int i = 0; i < trajectory1.size(); i++)
		if(	abs(trajectory1[i].x - trajectory2[i].x) > delta_dis_d || abs(trajectory1[i].y - trajectory2[i].y) > delta_dis_d)
		{
			issame = false;
			break;
		}
	
	if(issame)
		for(int i = 0; i < point1.size(); i++)
			if(	abs(point1[i].x - point2[i].x) > delta_dis || abs(point1[i].y - point2[i].y) > delta_dis)
			{
				issame = false;
				break;
			}

	return issame;
}

void BFS(int** arr, bool* visited, int* clusters, int size, int unit, int clus)
{
	int count = 0, head = 0;

	int *queue = new int[size];	
	for(int i = 0; i < size; i++) queue[i] = 0;
	
	queue[count++] = unit;
	visited[unit] = true;
	clusters[unit] = clus;

	while (head < count)
	{
		unit = queue[head++];

		for(int i = 0; i < size; i++)
			if (arr[unit][i] && !visited[i])
			{
				queue[count++] = i;
				visited[i] = true;
				clusters[i] = clus;
			}
	}
	delete []queue;
}

int* GetMatrixOfTrajectories(list<TrackSegm> segmTracks, int size)
{
	// create double array of zeros
	int** arr = 0;
	arr = new int*[size];

	for (int i = 0; i < size; i++)
	{
		arr[i] = new int[size];

		for (int j = 0; j < size; j++)
		{
			arr[i][j] = 0;
		}
	}

	// compute connections between trajectories
	int i = 0, j;

	for(list<TrackSegm>::iterator iTrack0 = segmTracks.begin(); iTrack0 != segmTracks.end(); iTrack0++)
	{
		j = 0;

		for(list<TrackSegm>::iterator iTrack1 = segmTracks.begin(); iTrack1 != segmTracks.end(); iTrack1++)
		{
			if(j > i)
				if(DoesTrajSame(iTrack0->point, iTrack0->trajectory, iTrack1->point, iTrack1->trajectory))
					arr[i][j] = 1;

			j++;
		}
		i++;
	}

	// print array
	/*
	for (int i = 0; i < size; i++)
	{
	    for (int j = 0; j < size; j++)
	    {
	    	printf("%i,", arr[i][j]);
	    }
	    printf("\n");
	}
	*/

	// find connected components with BFS
	bool* visited = new bool[size];
	int* clusters = new int[size];
	
	for (int i = 0; i < size; ++i) 
	{
		visited[i] = false;
		clusters[i] = 0;
	}

	int clus = 0;
	for (int i = 0; i < size; ++i)
	{
		if (!visited[i])
		{
			BFS(arr, visited, clusters, size, i, clus);
			clus++;
		}
	}

	// Clean up memory
	for (int i = 0; i < size; i++)
		delete []arr[i];

	delete []arr;
	delete []visited;
	arr = 0;	

	return clusters;
}

void ComputeTrajGraphs(list<Track> xyTracks, const int length, SeqInfo* seqInfo)
{
	printf("Number of trajectories: %d \n", xyTracks.size());

	int maxnum = 0;
	int indexOfMax = 0;

	for(int i = step; i<=159; ++i)
	{
		int num = CountTraj(xyTracks, i, length);
		//printf("Number of trajectories from %d to %d: %d \n", i - step, i, num);

		if(maxnum < num)
		{
			maxnum = num;
			indexOfMax = i;
		}
	}

	printf("Maximum number of traj: %d, at index: %d \n", maxnum, indexOfMax);

	// list of trajectories which was segmented and ready for graph algorithm 
	list<TrackSegm> segmTracks = ExtractTrajectories(xyTracks, indexOfMax, length);
	printf("Size of list: %d \n", segmTracks.size());	
	/*
	for(list<TrackSegm>::iterator iTrack = segmTracks.begin(); iTrack != segmTracks.end(); iTrack++)
	{
		for(int i = 0; i < step; i++)
		{
			printf("%f,\t%f;\t", iTrack->trajectory[i].x, iTrack->trajectory[i].y);
		}		
		printf("\n");
	}*/	

	// compute the matrix of connections between segmented trajectories
	int* clusters = GetMatrixOfTrajectories(segmTracks, maxnum);
	printf("Number of clusters: %d \n", sizeof(clusters));

	// draw segmented trajectories
	DrawTrajetories(seqInfo, segmTracks, indexOfMax, clusters);

	// Clean up memory
	delete []clusters;			
}

#endif /*TRAJHANDSEGM_H_*/