#ifndef TRAJECTORIES_H_
#define TRAJECTORIES_H_

#include "DenseTrack.h"

using namespace cv;

/*
void SaveTrajectoriesToFile(std::vector<std::list<Track>> xyScaleTracks, int scale_num, int frame_num)
{
	for(int iScale = 0; iScale < scale_num; iScale++)  // xyScaleTracks.size()
	{
	
		int iScale = 0;

		std::list<Track>& tracks = xyScaleTracks[iScale];

		for (std::list<Track>::iterator iTrack = tracks.begin(); iTrack != tracks.end(); ++iTrack) {
			int index = iTrack->index;
			Point2f prev_point = iTrack->point[index];


		}
	}
}*/
void ClearTrackPoints(const int length)
{
	std::ofstream outfile;
	outfile.open("out_of_tracks.txt");
	outfile << "";
	outfile.close();

	std::ofstream outfile2;
	outfile2.open("out_of_tracks_debug.txt");
	outfile2 << length << std::endl;
	outfile2.close();
}

void SaveTrackPointsForDebug(const std::vector<Point2f>& point, const int length, const float scale, int frame_num)
{
	std::ofstream outfile;
	outfile.open("out_of_tracks_debug.txt", std::ios_base::app);

	outfile << frame_num << "\t";

	for (int j = 0; j <= length; j++)
	{
		Point2f point0 = point[j];
		point0 *= scale;

		outfile << point0.x << "\t";;
		outfile << point0.y << "\t";;
	}

	outfile << std::endl;
	outfile.close();
}

void SaveTrackPoints(const std::vector<Point2f>& point, std::vector<Point2f> trajectory, const int length, float mean_x, float mean_y, float var_x, float var_y, int frame_num)
{
	std::ofstream outfile;
	outfile.open("out_of_tracks.txt", std::ios_base::app);

	outfile << frame_num << "\t";
	outfile << length << "\t";
	outfile << mean_x << "\t";
	outfile << mean_y << "\t";
	outfile << var_x << "\t";
	outfile << var_y << "\t";
	
	/*
	for (int i = 0; i < length; ++i)
	{
		outfile << trajectory[i].x << "\t";
		outfile << trajectory[i].y << "\t";
	}*/
    
	for (int j = 0; j <= length; j++)
	{
		Point2f point0 = point[j];

		outfile << point0.x << "\t";;
		outfile << point0.y << "\t";;
	}

	outfile << std::endl;
	outfile.close();

	//SaveTrackPointsForDebug(point, length, scale, frame_num);
}

#endif /*TRAJECTORIES_H_*/