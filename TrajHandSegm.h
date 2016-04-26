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

    int frame_num;
    float mean_x;
    float mean_y;
    float var_x;
    float var_y;

    void addPoint(const Point2f& point_)
    {
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

    void setMean(float m_x, float m_y)
    {
    	mean_x = m_x;
    	mean_y = m_y;
    }

    void setVariance(float v_x, float v_y)
    {
    	var_x = v_x;
    	var_y = v_y;
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
			vector<Point2f> trajectory(step);

			for(int i = index, j = 0; i <= index + step; i++, j++)
			{
				track.addPoint(iTrack->point[i]);
				trajectory[j] = iTrack->point[i];
			}

			float mean_x(0), mean_y(0), var_x(0), var_y(0), length(0);
			IsValid(trajectory, mean_x, mean_y, var_x, var_y, length);
			track.setMean(mean_x, mean_y);
			track.setVariance(var_x, var_y);

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
		//circle(image, point0, 1, Scalar(colors[index][0], colors[index][1], colors[index][2]), -1, 8, 0);
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

        if(indexOfMax - stepmake == frame_num)
        {
        	// draw all segmented trajectories
        	int index = 0;
        	int sum = 0;
		    for(list<TrackSegm>::iterator iTrack = segmTracks.begin(); iTrack != segmTracks.end(); iTrack++)
		    {
		    	//if(clusters[index] == 3)
		    	//{
		    		DrawTrajetory(iTrack->point, frame, clusters[index]);
		    	//	printf("Variance of x: %f, Variance of y: %f\n", iTrack->var_x, iTrack->var_y);
		    	//	sum++;
		    	//}
		    	index++;
		    }

		    //printf("Number of trajectories in the cluster: %d\n", sum);
			imshow( "SegmentedTrajectories", frame);
			break;
        }

        frame_num++;
	}

    cvWaitKey(0);
    destroyWindow("SegmentedTrajectories");
}

bool DoesTrajSame(list<TrackSegm>::iterator track0, list<TrackSegm>::iterator track1)
{
	bool issame = true;

	if(	abs(track0->var_x - track1->var_x) > delta_var || abs(track0->var_y - track1->var_y) > delta_var)
	{
		issame = false;
	}

	if(	abs(track0->mean_x - track1->mean_x) > delta_mean || abs(track0->mean_y - track1->mean_y) > delta_mean)
	{
		issame = false;
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

int* GetMatrixOfTrajectories(list<TrackSegm> segmTracks)
{
	int size = segmTracks.size();

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
				if(DoesTrajSame(iTrack0, iTrack1))
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

	printf("Number of clusters: %d\n", clus);

	// Clean up memory
	for (int i = 0; i < size; i++)
		delete []arr[i];

	delete []arr;
	delete []visited;
	arr = 0;	

	return clusters;
}

int OtsuThresholding(int* histogram, int size, int total)
{
	int _sum = 0;

	for (int i = 0; i < size; ++i)
		_sum += i * histogram[i];

	int sumB = 0, wB = 0, wF = 0, _max = 0, threshold = 0;
	float mB = 0.0, mF = 0.0, between = 0.0;
	
	for (int i = 0; i < size; ++i)
	{
		wB += histogram[i];
		if (wB == 0)
			continue;

		wF = total - wB;
		if (wF == 0)
			break;

		sumB += i * histogram[i];
		mB = sumB / wB;
		mF = (_sum - sumB) / wF;

		between = wB * wF * pow(mB - mF, 2);
		if (between > _max)
		{
			_max = between;
			threshold = i;
		}
	}

	return threshold;
}

list<TrackSegm> Thresholding(list<TrackSegm> segmTracks)
{
	int max = 0;

	for(list<TrackSegm>::iterator iTrack = segmTracks.begin(); iTrack != segmTracks.end(); iTrack++)
	{
		int num = iTrack->var_x * iTrack->var_y;
		if (num > max)
		{
			max = num;
		}
	}

	//printf("Maximum number: %d\n", max);
	
	int* histogram = new int[max];
	for (int i = 0; i < max; ++i) histogram[i] = 0;

	for(list<TrackSegm>::iterator iTrack = segmTracks.begin(); iTrack != segmTracks.end(); iTrack++)
	{
		int num = iTrack->var_x * iTrack->var_y;
		histogram[num]++;
	}

	int threshold = OtsuThresholding(histogram, max, segmTracks.size());

	printf("threshold: %d\n", threshold);

	list<TrackSegm> segmTracks_thresholded;

	while (!segmTracks.empty())
	{
		TrackSegm iTrack = segmTracks.back();
		segmTracks.pop_back();
		
		int num = iTrack.var_x * iTrack.var_y;
		
		if(num >= threshold)
			segmTracks_thresholded.push_back(iTrack);
	}

	delete []histogram;

	return segmTracks_thresholded;
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

	//segmTracks = Thresholding(segmTracks);

	/*
	for(list<TrackSegm>::iterator iTrack = segmTracks.begin(); iTrack != segmTracks.end(); iTrack++)
	{
		for(int i = 0; i < step; i++)
		{
			printf("%f,\t%f;\t", iTrack->trajectory[i].x, iTrack->trajectory[i].y);
		}		
		printf("\n");
	}
	*/

	// compute the matrix of connections between segmented trajectories and cluster them
	int* clusters = GetMatrixOfTrajectories(segmTracks);	

	// draw segmented trajectories
	DrawTrajetories(seqInfo, segmTracks, indexOfMax, clusters);

	// Clean up memory
	delete []clusters;			
}

#endif /*TRAJHANDSEGM_H_*/