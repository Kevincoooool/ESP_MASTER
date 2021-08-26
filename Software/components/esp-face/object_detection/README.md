# Object Detection



**Object Detection** is a lite one stage object detection library specially designed for embedded devices.



## Quick Start

Here is a pseudocode of how to achieve object detection by our library.

```c
// Here we take cat-face-detection as an example.
// Initialize/Update model configuration
update_detection_model(&cat_face_3, 0.5, 0.6, 0.3, image_height, image_width); 	// Call this when initialization and parameter changed

// Do detection
box_array_t *net_boxes = detect_object(image, &cat_face_3); 
```

The details of APIs could be seen in '**API Introduction**'.

The available detection models could be seen in '**Detection Model Market**'.



## API Introduction

```c
void update_detection_model(detection_model_t *model, fptp_t resize_scale, fptp_t score_threshold, fptp_t nms_threshold, int image_height, int image_width);
```

This update_detection_model()` helps to update model configuration.

The inputs are:

- **model**:
  - a detection model,
  - please see **Model Selection** below for more details.
- **resize_scale**:
  - Input image resize in this scale
  - Range: [16, the length of the shortest edge of the original input image). 
  - For an original input image of a fixed size, the larger the `resize_scale` is, 
    - the smaller the minimum size of a detectable face is;
    - the longer the processing takes
  - and vice versa.
- **score_threshold**:
	- Range: (0,1)
	- For an original input image of a fixed size, the larger the `score` is,
		- the larger the number of filtered out candidate bounding boxes is
		- the lower the detection ratio is
	- and vice versa.
- **nms_threshold**:
	- Range: (0,1)
	- For an original input image of a fixed size, the larger the `nms` is,
		- the higher the possibility that an overlapped face can be detected is;
		- the larger the number of detected candidate bounding boxes of a same face is
	- and vice versa.
- **image_height**: the height of original input image.
- **image_width**: the width of original input image.



```c
box_array_t *detect_object(dl_matrix3du_t *image, detection_model_t *model);
```

This `detect_object()` handles the whole object detection mission.

The inputs are:

- **image**: an image in `dl_matrix3du_t` type
- **model**: a detection model. 

The output is:

- A `box_array_t` type value contains object boxes, as well as score and landmark of each box.

This structure is defined as follows:

```c
typedef struct tag_box_list
{
    uint8_t *category;
    fptp_t *score;
    box_t *box;
    landmark_t *landmark;
    int len;
} box_array_t;
```

The structure contains heads of arrays, each array has a same length, which is the number of objects in the image.



## Detection Model Market

All available models are included in `./object_detection/include/object_detection.h`. Here are the descriptions.

| model            | function        | root                       |
| ---------------- | --------------- | -------------------------- |
| cat_face_3_model | detect cat face | ./lib/include/cat_face_3.h |

