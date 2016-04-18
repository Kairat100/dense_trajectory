#include "DenseTrack.h"
#include "Initialize.h"
#include "Descriptors.h"
#include "OpticalFlow.h"
#include "Trajectories.h"
#include "Constants.h"
#include "TrajHandSegm.h"

#include <time.h>

using namespace cv;

int show_track = 0; // set show_track = 1, if you want to visualize the trajectories

int main(int argc, char** argv)
{
	VideoCapture capture;
	char* video = argv[1];
	int flag = arg_parse(argc, argv);
	capture.open(video);

	if(!capture.isOpened()) {
		fprintf(stderr, "Could not initialize capturing..\n");
		return -1;
	}   

	int frame_num = 0;
	TrackInfo trackInfo;

	InitTrackInfo(&trackInfo, track_length, init_gap);  
	
	SeqInfo seqInfo;
	InitSeqInfo(&seqInfo, video);

	if(flag)
		seqInfo.length = end_frame - start_frame + 1;

	if(show_track == 1)
	{
		namedWindow("DenseTrack", 0);
		resizeWindow("DenseTrack", capture.get(CV_CAP_PROP_FRAME_WIDTH) * 3, capture.get(CV_CAP_PROP_FRAME_HEIGHT) * 3);
	}

	Mat image, prev_grey, grey, flow, prev_poly, poly;
	std::list<Track> xyTracks;

	int init_counter = 0; // indicate when to detect new feature points

	ClearTrackPoints(trackInfo.length);

	while(true) {
		Mat frame;
		int i, c;

		// get a new frame
		capture >> frame;
		if(frame.empty())
			break;

		if(frame_num < start_frame || frame_num > end_frame)
		{
			frame_num++;
			continue;
		}

		if(frame_num == start_frame) {
			image.create(frame.size(), CV_8UC3);
			grey.create(frame.size(), CV_8UC1);
			prev_grey.create(frame.size(), CV_8UC1);

			flow.create(frame.size(), CV_32FC2);
			prev_poly.create(frame.size(),CV_32FC(5));
			poly.create(frame.size(),CV_32FC(5));

			frame.copyTo(image);
			cvtColor(image, prev_grey, CV_BGR2GRAY);

			std::vector<Point2f> points(0);
			DenseSample(prev_grey, points, quality, min_distance);

			// save the feature points
			for(i = 0; i < points.size(); i++)
				xyTracks.push_back(Track(points[i]));

			// compute polynomial expansion
			my::FarnebackPolyExp2(prev_grey, prev_poly, 7, 1.5);

			frame_num++;
			continue;
		}

		init_counter++;
		frame.copyTo(image);
		cvtColor(image, grey, CV_BGR2GRAY);
			  

/////////////////////////////////////////////////////////////////////////////////


		// compute optical flow for all scales once
		my::FarnebackPolyExp2(grey, poly, 7, 1.5);
		my::calcOpticalFlowFarneback2(prev_poly, poly, flow, 10, 2);

		int width = grey.cols;
		int height = grey.rows;

		// track feature points
		for (std::list<Track>::iterator iTrack = xyTracks.begin(); iTrack != xyTracks.end(); ++iTrack)
		{
			if(iTrack->tracking == true)
			{
				int index = iTrack->index;

				Point2f prev_point = iTrack->point[index];
				int x = std::min<int>(std::max<int>(cvRound(prev_point.x), 0), width-1);
				int y = std::min<int>(std::max<int>(cvRound(prev_point.y), 0), height-1);

				Point2f point;
				point.x = prev_point.x + flow.ptr<float>(y)[2*x];
				point.y = prev_point.y + flow.ptr<float>(y)[2*x+1];

				//	printf("%f\n", point.x = prev_point.x);


				if(point.x <= 0 || point.x >= width || point.y <= 0 || point.y >= height)
				{
					iTrack = xyTracks.erase(iTrack);
					continue;
				}

				iTrack->addPoint(point);

				
				// if the trajectory achieves the maximal length
				if(iTrack->index >= trackInfo.length)
				{
					// draw the trajectories at the first scale
					if(show_track == 1)
						DrawTrack(iTrack->point, iTrack->index, 1.0, 10, image);

					std::vector<Point2f> trajectory(trackInfo.length+1);

					for(int i = 0; i <= trackInfo.length; ++i)
						trajectory[i] = iTrack->point[i];

					float mean_x(0), mean_y(0), var_x(0), var_y(0), length(0);
					
					if(IsValid(trajectory, mean_x, mean_y, var_x, var_y, length))
					{                       
						// Here we are trying to segment trajectories belonding to hands
						if(var_x > var_threshold || var_y > var_threshold)
						{							
							SaveTrackPoints(iTrack->point, trajectory, trackInfo.length, mean_x, mean_y, var_x, var_y, frame_num);

							iTrack->tracking = false;
							iTrack->frame_num = frame_num;
							iTrack->mean_x = mean_x;
							iTrack->mean_y = mean_y;
							iTrack->trajectory = trajectory;
						}
					}
					
					if(iTrack->tracking)
						iTrack = xyTracks.erase(iTrack);
				}
			}
		}

		if(init_counter != trackInfo.gap)
			continue;

		// detect new feature points every initGap frames
		std::vector<Point2f> points(0);
		for(std::list<Track>::iterator iTrack = xyTracks.begin(); iTrack != xyTracks.end(); iTrack++)
			if(iTrack->tracking == true)
				points.push_back(iTrack->point[iTrack->index]);
 

		DenseSample(grey, points, quality, min_distance);
		// save the new feature points
		for(i = 0; i < points.size(); i++)
			xyTracks.push_back(Track(points[i]));


/////////////////////////////////////////////////////////////////////////////////

		init_counter = 0;
		grey.copyTo(prev_grey);     
		poly.copyTo(prev_poly);

		frame_num++;

		//cvWaitKey(0);

		if( show_track == 1 ) 
		{
			imshow( "DenseTrack", image);
			c = cvWaitKey(3);
			if((char)c == 27) break;

		}
	}

	// Remove trajectories which not reached the needed length
	for(std::list<Track>::iterator iTrack = xyTracks.begin(); iTrack != xyTracks.end(); iTrack++)
			if(iTrack->tracking)
				iTrack = xyTracks.erase(iTrack);

	ComputeTrajGraphs(xyTracks, trackInfo.length, &seqInfo);


	if( show_track == 1 )
		destroyWindow("DenseTrack");

	return 0;
}