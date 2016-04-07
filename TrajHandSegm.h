#ifndef TRAJHANDSEGM_H_
#define TRAJHANDSEGM_H_

#include "DenseTrack.h"

using namespace std;

const int step = 5;


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

void ComputeTrajGraphs(list<Track> xyTracks, const int length)
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

	list<TrackSegm> segmTracks = ExtractTrajectories(xyTracks, indexOfMax, length);
	printf("Size of list: %d \n", segmTracks.size());	


	for(list<TrackSegm>::iterator iTrack = segmTracks.begin(); iTrack != segmTracks.end(); iTrack++)
	{
		for(int i = 0; i < step; i++)
		{
			printf("%f,\t%f;\t", iTrack->trajectory[i].x, iTrack->trajectory[i].y);
		}		
		printf("\n");
		
	}
}

#endif /*TRAJHANDSEGM_H_*/