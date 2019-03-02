#include <librealsense2/rs.h>
#include <librealsense2/h/rs_pipeline.h>
#include <librealsense2/h/rs_option.h>
#include <librealsense2/h/rs_frame.h>
#include "../example.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>



#define FORMAT          RS2_FORMAT_Z16    // rs2_format is identifies how binary data is encoded within a frame   //
#define WIDTH           640               // Defines the number of columns for each frame                         //
#define HEIGHT          480               // Defines the number of lines for each frame                           //
#define FPS             15                // Defines the rate of frames per second                                //
#define STREAM_INDEX    0                 // Defines the stream index, used for multiple streams of the same type //
#define METERS_TO_FEET  3.28084						// Conversion From Meters to Feet															  

float findShortestDistance(rs2_frame* frame, int startWidth, int endWidth, int height, rs2_error* e);

.int main()
{
    rs2_error* e = 0;

    rs2_context* ctx = rs2_create_context(RS2_API_VERSION, &e);
    check_error(e);

    rs2_device_list* device_list = rs2_query_devices(ctx, &e);
    check_error(e);

    int dev_count = rs2_get_device_count(device_list, &e);
    check_error(e);
    printf("There are %d connected RealSense devices.\n", dev_count);
    if (0 == dev_count)
        return EXIT_FAILURE;

    // Get the first connected device
    // The returned object should be released with rs2_delete_device(...)
    rs2_device* dev = rs2_create_device(device_list, 0, &e);
    check_error(e);

    print_device_info(dev);

    // Create a pipeline to configure, start and stop camera streaming
    rs2_pipeline* pipeline =  rs2_create_pipeline(ctx, &e);
    check_error(e);

    // Create a config instance, used to specify hardware configuration
    rs2_config* config = rs2_create_config(&e);
    check_error(e);

    // Request a specific configuration
    rs2_config_enable_stream(config, STREAM, STREAM_INDEX, WIDTH, HEIGHT, FORMAT, FPS, &e);
    check_error(e);

    // Start the pipeline streaming
    rs2_pipeline_profile* pipeline_profile = rs2_pipeline_start_with_config(pipeline, config, &e);
    if (e)
    {
        printf("The connected device doesn't support depth streaming!\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // This call waits until a new composite_frame is available
        rs2_frame* frames = rs2_pipeline_wait_for_frames(pipeline, 5000, &e);
        check_error(e);

        // Returns the number of frames embedded within the composite frame
        int num_of_frames = rs2_embedded_frames_count(frames, &e);
        check_error(e);

        int i;
        for (i = 0; i < num_of_frames; ++i)
        {
            rs2_frame* frame = rs2_extract_frame(frames, i, &e);
            check_error(e);

            if (0 == rs2_is_frame_extendable_to(frame, RS2_EXTENSION_DEPTH_FRAME, &e))
                continue;

            // Get the depth frame's dimensions
            int width = rs2_get_frame_width(frame, &e);
            check_error(e);
            int height = rs2_get_frame_height(frame, &e);
            check_error(e);

			// Create three boxes of equal width and height
			int boxWidth = width / 3;


			printf("Left: The camera is facing an object %.3f feet away.\n", findShortestDistance(frame, 30, boxWidth, height, e));
			printf("Middle: The camera is facing an object %.3f feet away.\n", findShortestDistance(frame, boxWidth, boxWidth * 2, height, e));
			printf("Right: The camera is facing an object %.3f feet away.\n", findShortestDistance(frame, boxWidth * 2, boxWidth * 3, height, e));


            rs2_release_frame(frame);
        }

        rs2_release_frame(frames);
    }

	

    // Stop the pipeline streaming
    rs2_pipeline_stop(pipeline, &e);
    check_error(e);

    // Release resources
    rs2_delete_pipeline_profile(pipeline_profile);
    rs2_delete_config(config);
    rs2_delete_pipeline(pipeline);
    rs2_delete_device(dev);
    rs2_delete_device_list(device_list);
    rs2_delete_context(ctx);

    return EXIT_SUCCESS;
}

float findShortestDistance(rs2_frame* frame, int startWidth, int endWidth, int height, rs2_error* e) {
	int i, j;
	float smallestDist =  METERS_TO_FEET * rs2_depth_frame_get_distance(frame, endWidth / 2, height / 2, &e);

	for (i = startWidth; i < endWidth; i++) {
		for (j = 1; j < height; j++) {
			float dist = METERS_TO_FEET * rs2_depth_frame_get_distance(frame, i, j, &e);
      
			if (dist < smallestDist && dist > 1.0 && dist < 15.0) {
				smallestDist = dist;
			}
		}
	}
	return smallestDist;
}
