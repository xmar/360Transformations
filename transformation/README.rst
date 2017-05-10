Projection tool
===============

Introduction
------------

This folder contains the C++ source code of our projection tool.
The folder *Scripts* contains a Python3 script used to test our streaming architecture

The projection tool can read a 360-degree video projected on any known 2D layout, can project this video onto any known 2D layout and can extract any Field of View (FoV) from the 360-degree video.
For now the FoV extraction is static: i.e. the FoV center cannot move during the video.


How To Compile
--------------

Requirements
............

* A **C++11** compiler (for instance gcc version 5 or more recent)
* **OpenCV** version 3 or more recent
* **Boost** library version 1.3 or more recent, with the program option package
* ffmpeg **libav** library
* **CMake** version 3 or more recent

Compilation steps
.................

Create the build repository and move into it::

    mkdir build
    cd build

Generate the Makefile::

    cmake ..

Compile the software::

    make

Now a software named **trans** should be in your build repository

How To Use
----------

The **trans** software require one argument.

-c      Path to the `.ini` configuration file.

The ini file contains the configuration of the test scenario.

Description of the ini file
...........................

The **Global** section contains the global configuration of the test.

.. code-block:: ini

  [Global]
  ;The number of frame per second
  fps=24
  ;If true display the generated pictures and wait for a key to precessed before continuing
  displayFinalPict=true
  ;Path to the output video file. If empty no output video is recorded.The flow id and the name of the last layout is used as an id for the generated output file
  videoOutputName=
  ; bitrate in kbps of the output videos (0 if default crf)
  videoOutputBitRate=0
  ;Path to the quality output file. If empty no quality is computed. The flow id and the name of the last layout is used as an id for the generated output file
  qualityOutputName=
  ;Indicate which metric to use. "MS-SSIM", "SSIM", "PSNR" and "WS-PSNR" require the two final picture to have the same resolution.
  ;The "S-PSNR-NN" and "S-PSNR-I" are computed from a uniform sampling of 655362 points on the sphere. "S-PSNR-NN" uses the Nearest Neightboor interpolation and "S-PSNR-I" uses the Bicubic interpolation.
  qualityToComputeList = ["MS-SSIM", "SSIM", "PSNR", "S-PSNR-NN", "S-PSNR-I", "WS-PSNR"]
  ;Index of the first frame of the input videos to process. If equal to n then the n first frames of the input videos will be skipped
  startFrame=0
  ;Number of frame to process in the video
  nbFrames= 5
  ;The layout flow indicate for each flow the input video, its layout and which transformation to perform. It is an array of array. The first string in an array is the path to the input video. The second string is the layout of the input video and the other string are section id of the layout onto which the video should be projected.
  layoutFlow= [["../example.mp4", "Equirectangular", "EquirectangularTiled"], ["../example.mp4", "Equirectangular", "CubeMap", "FlatFixed"]]

Each section id named in the layoutFlow attribute should be defined in the ini file. In the layout flow, the first string is the path to the input video, the second string the name of the section that describe the layout of the input video. The other strings are the name of the section the describe the layout onto which we want to project the video. There can be as many layout as we want and the video will be consecutively projected on each of those layout. It is not possible to do an other projection after a flat fixed view (a FoV extraction) projection.

**equirectangular** layout

.. code-block:: ini

  [Equirectangular]
  ;type of the layout: equirectangular is the state-of-the-art equirectangular layout
  type=equirectangular
  ;Width of the reference equirectangular video
  refWidth=4096
  ;Width of the reference equirectangular video
  refHeight=2048
  ;yaw, pitch, roll of the center point of the equirectangular video (in degree)
  yaw=0
  pitch=0
  roll=0
  ;if true, the resolution of the layout is compute based on the resolution of the previous layout (or on the resolution of the reference equirectangular video)
  relativeResolution=true
  ;if relativeResolution=true ratio of the output layout width compare to the reference video; otherwise width size of the output video in pixel number
  width=1
  height=1
  ;bitrate ratio of the output video relative to the videoOutputBitRate (if it is the final layout and if the video is encoded).
  bitrate=1

**flatFixed** layout: FoV extraction

.. code-block:: ini

  [FlatFixed]
  ;type of the layout: flatFixed means FoV extraction
  type=flatFixed
  ;relativeResolution cannot be true for flatFixed layout
  relativeResolution=false
  ;indicates if we want to use a static position set by the yaw, pitch and roll parameter or play a traces located at positionTrace
  dynamicPositions=false
  ;The path to the position trace file. Used if dynamicPositions is set to true
  positionTrace=pathToPositionTrace
  ;yaw, pitch, roll of the center point of the FoV (in degree). Used if dynamicPositions is set to false
  yaw=0.0
  pitch=0.0
  roll=0.0
  ;width and height in pixels of the output image
  width=1920
  height=1080
  ;Horizontal angle of the display device (in degree)
  horizontalAngleOfVision=110
  ;Vertical angle of the display device (in degree)
  verticalAngleOfVision=90

**cube map** layout

.. code-block:: ini

  [CubeMapLower]
  ;type of the layout: cube map layout
  type=cubeMap2
  ;Size of the reference input equirectangular video
  refWidth=4096
  refHeight=2048
  ;use relative resolution for the face resolutions
  relativeResolution=true
  ;yaw, pitch and roll of the center of the Front face (in degree)
  yaw=0.0
  pitch=0.0
  roll=0.0
  ;the edge of each face. If relativeResolution, ratio compared to the reference input video, otherwise size in pixel.
  cubeEdgeLengthFront=1
  cubeEdgeLengthLeft=1
  cubeEdgeLengthRight=1
  cubeEdgeLengthTop=1
  cubeEdgeLengthBottom=1
  cubeEdgeLengthBack=1
  ;if useTile is set to true, each face will be encoded into an independent video with a target bitrate set by the bitrateFront, bitrateLeft, etc. parameters
  ;if useTile is set to false, the whole projection will be encoded into a unique video (all faces on the same frame) and a global bitrate target set by the bitrate parameter
  useTile=true
  ;Relative bitrate of each face (if final layout encoded). A ratio r means r*videoOutputBitRate/6
  bitrateFront=1
  bitrateLeft=0.25
  bitrateRight=0.25
  bitrateTop=0.25
  bitrateBottom=0.25
  bitrateBack=0.25
  ;bitrate is used only if useTile=false. A ratio r means a bitrate of r*videoOutputBitRate
  bitrate=1

**pyramid** layout

.. code-block:: ini

  [PyramidLower]
  ;type of the layout: pyramid map layout
  type=pyramid2
  ;Size of the reference input equirectangular video
  refWidth=4096
  refHeight=2048
  ;use relative resolution for the face resolutions
  relativeResolution=true
  ;Size of the Base face in the 3D space (the circumscribed sphere have a radius of 1). Should be strictly higher than 2.
  pyramidBaseEdge=2.5
  ;yaw, pitch and roll of the center of the Base face (in degree)
  yaw=0.0
  pitch=0.0
  roll=0.0
  ;the edge of each face. If relativeResolution, ratio compared to the reference input video, otherwise size in pixel.
  pyramidBaseEdgeLength=1
  pyramidHeightLeft=1
  pyramidHeightRight=1
  pyramidHeightTop=1
  pyramidHeightBottom=1
  ;if useTile is set to true, each face will be encoded into an independent video with a target bitrate set by the pyramidBaseBitrate, pyramidLeftBitrate, etc. parameters
  ;if useTile is set to false, the whole projection will be encoded into a unique video (all faces on the same frame) and a global bitrate target set by the bitrate parameter
  useTile=true
  ;Relative bitrate of each face (if final layout encoded). A ratio r means r*videoOutputBitRate/5
  pyramidBaseBitrate=1
  pyramidLeftBitrate=0.25
  pyramidRightBitrate=0.25
  pyramidTopBitrate=0.25
  pyramidBottomBitrate=0.25
  ;bitrate is used only if useTile=false. A ratio r means a bitrate of r*videoOutputBitRate
  bitrate=1

**rhombicDodeca** layout

.. code-block:: ini

  [RhombicDodeca]
  ;type of the layout: rhombicDodeca
  type=rhombicDodeca
  ;Size of the reference input equirectangular video
  refWidth=4096
  refHeight=2048
  ;use relative resolution for the face resolutions
  relativeResolution=true
  ;yaw, pitch and roll of the center of the Face 1 (in degree)
  yaw=199.4712
  pitch=-26.565
  roll=0.0
  ;the length of each face. If relativeResolution, ratio compared to the reference input video, otherwise size in pixel.
  rhombEdgeLengthFace1=1
  rhombEdgeLengthFace2=1
  rhombEdgeLengthFace3=1
  rhombEdgeLengthFace4=1
  rhombEdgeLengthFace5=1
  rhombEdgeLengthFace6=1
  rhombEdgeLengthFace7=1
  rhombEdgeLengthFace8=1
  rhombEdgeLengthFace9=1
  rhombEdgeLengthFace10=1
  rhombEdgeLengthFace11=1
  rhombEdgeLengthFace12=1
  ;if useTile is set to true, each face will be encoded into an independent video with a target bitrate set by the rhombFace1Bitrate, rhombFace2Bitrate, etc. parameters
  ;if useTile is set to false, the whole projection will be encoded into a unique video (all faces on the same frame) and a global bitrate target set by the bitrate parameter
  useTile=true
  ;Relative bitrate of each face (if final layout encoded). A ratio r means r*videoOutputBitRate/12
  rhombFace1Bitrate=1
  rhombFace5Bitrate=1
  rhombFace2Bitrate=0.5
  rhombFace3Bitrate=0.5
  rhombFace4Bitrate=0.5
  rhombFace6Bitrate=0.5
  rhombFace7Bitrate=0.5
  rhombFace8Bitrate=0.5
  rhombFace10Bitrate=0.5
  rhombFace11Bitrate=0.5
  rhombFace9Bitrate=0.25
  rhombFace12Bitrate=0.25
  ;bitrate is used only if useTile=false. A ratio r means a bitrate of r*videoOutputBitRate
  bitrate=1

**equirectangularTiled** layout

.. code-block:: ini

  [EquirectangularTiledLower]
  type=equirectangularTiled
  ;Size of the reference input equirectangular video
  refWidth=4096
  refHeight=2048
  ;use relative resolution for the face resolutions
  relativeResolution=true
  ;yaw, pitch, roll of the center of face 4x3 (in degree)
  yaw=45.0
  pitch=-22.5
  roll=0.0
  ;nbHTiles (resp. nbVTiles) indicate the number of horizontal (resp. vertical) tiles
  ;You have to compile the software with the RANGE_NB_H_TILES and RANGE_NB_V_TILES macro to be able to use this layout.
  ;For instance in this example the value 8 should be in the macro RANGE_NB_H_TILES and RANGE_NB_V_TILES otherwise a runtime error will be raised
  nbHTiles=8
  nbVTiles=8
  ;hTileRation_X and vTileRation_X indicate the relative horizontal and vertical ratio of the tile X. In this example each tile get 1/8 of the equirectangular picture
  hTileRation_0=1
  hTileRation_1=1
  hTileRation_2=1
  hTileRation_3=1
  hTileRation_4=1
  hTileRation_5=1
  hTileRation_6=1
  hTileRation_7=1
  vTileRation_0=1
  vTileRation_1=1
  vTileRation_2=1
  vTileRation_3=1
  vTileRation_4=1
  vTileRation_5=1
  vTileRation_6=1
  vTileRation_7=1
  ;if upscale is set to true, all the tiles will be upscaled to the original tile resolution
  upscale=false
  ;if useTile is set to true, each face will be encoded into an independent video with a target bitrate set by the equirectangularTileBitrate_0_0, equirectangularTileBitrate_0_1, etc. parameters
  ;if useTile is set to false, the whole projection will be encoded into a unique video (all faces on the same frame) and a global bitrate target set by the bitrate parameter
  useTile=true
  ;resolution of the tile (i,j)=(0,0)
  equirectangularTile_0_0=1
  ; bitrate of the tile (i,j) = (0,0). Ratio of r means r*videoOutputBitRate/64
  equirectangularTileBitrate_0_0=0.25
  equirectangularTile_0_1=1
  equirectangularTileBitrate_0_1=0.25
  equirectangularTile_0_2=1
  equirectangularTileBitrate_0_2=0.25
  equirectangularTile_0_3=1
  equirectangularTileBitrate_0_3=0.25
  equirectangularTile_0_4=1
  equirectangularTileBitrate_0_4=0.25
  equirectangularTile_0_5=1
  equirectangularTileBitrate_0_5=0.25
  equirectangularTile_0_6=1
  equirectangularTileBitrate_0_6=0.25
  equirectangularTile_0_7=1
  equirectangularTileBitrate_0_7=0.25
  equirectangularTile_1_0=1
  equirectangularTileBitrate_1_0=0.25
  equirectangularTile_1_1=1
  equirectangularTileBitrate_1_1=0.25
  equirectangularTile_1_2=1
  equirectangularTileBitrate_1_2=0.25
  equirectangularTile_1_3=1
  equirectangularTileBitrate_1_3=0.25
  equirectangularTile_1_4=1
  equirectangularTileBitrate_1_4=0.25
  equirectangularTile_1_5=1
  equirectangularTileBitrate_1_5=0.25
  equirectangularTile_1_6=1
  equirectangularTileBitrate_1_6=0.25
  equirectangularTile_1_7=1
  equirectangularTileBitrate_1_7=0.25
  equirectangularTile_2_0=1
  equirectangularTileBitrate_2_0=0.25
  equirectangularTile_2_1=1
  equirectangularTileBitrate_2_1=1
  equirectangularTile_2_2=1
  equirectangularTileBitrate_2_2=1
  equirectangularTile_2_3=1
  equirectangularTileBitrate_2_3=1
  equirectangularTile_2_4=1
  equirectangularTileBitrate_2_4=1
  equirectangularTile_2_5=1
  equirectangularTileBitrate_2_5=1
  equirectangularTile_2_6=1
  equirectangularTileBitrate_2_6=0.25
  equirectangularTile_2_7=1
  equirectangularTileBitrate_2_7=0.25
  equirectangularTile_3_0=1
  equirectangularTileBitrate_3_0=0.25
  equirectangularTile_3_1=1
  equirectangularTileBitrate_3_1=1
  equirectangularTile_3_2=1
  equirectangularTileBitrate_3_2=1
  equirectangularTile_3_3=1
  equirectangularTileBitrate_3_3=1
  equirectangularTile_3_4=1
  equirectangularTileBitrate_3_4=1
  equirectangularTile_3_5=1
  equirectangularTileBitrate_3_5=1
  equirectangularTile_3_6=1
  equirectangularTileBitrate_3_6=0.25
  equirectangularTile_3_7=1
  equirectangularTileBitrate_3_7=0.25
  equirectangularTile_4_0=1
  equirectangularTileBitrate_4_0=0.25
  equirectangularTile_4_1=1
  equirectangularTileBitrate_4_1=1
  equirectangularTile_4_2=1
  equirectangularTileBitrate_4_2=1
  equirectangularTile_4_3=1
  equirectangularTileBitrate_4_3=1
  equirectangularTile_4_4=1
  equirectangularTileBitrate_4_4=1
  equirectangularTile_4_5=1
  equirectangularTileBitrate_4_5=1
  equirectangularTile_4_6=1
  equirectangularTileBitrate_4_6=0.25
  equirectangularTile_4_7=1
  equirectangularTileBitrate_4_7=0.25
  equirectangularTile_5_0=1
  equirectangularTileBitrate_5_0=0.25
  equirectangularTile_5_1=1
  equirectangularTileBitrate_5_1=1
  equirectangularTile_5_2=1
  equirectangularTileBitrate_5_2=1
  equirectangularTile_5_3=1
  equirectangularTileBitrate_5_3=1
  equirectangularTile_5_4=1
  equirectangularTileBitrate_5_4=1
  equirectangularTile_5_5=1
  equirectangularTileBitrate_5_5=1
  equirectangularTile_5_6=1
  equirectangularTileBitrate_5_6=0.25
  equirectangularTile_5_7=1
  equirectangularTileBitrate_5_7=0.25
  equirectangularTile_6_0=1
  equirectangularTileBitrate_6_0=0.25
  equirectangularTile_6_1=1
  equirectangularTileBitrate_6_1=1
  equirectangularTile_6_2=1
  equirectangularTileBitrate_6_2=1
  equirectangularTile_6_3=1
  equirectangularTileBitrate_6_3=1
  equirectangularTile_6_4=1
  equirectangularTileBitrate_6_4=1
  equirectangularTile_6_5=1
  equirectangularTileBitrate_6_5=1
  equirectangularTile_6_6=1
  equirectangularTileBitrate_6_6=0.25
  equirectangularTile_6_7=1
  equirectangularTileBitrate_6_7=0.25
  equirectangularTile_7_0=1
  equirectangularTileBitrate_7_0=0.25
  equirectangularTile_7_1=1
  equirectangularTileBitrate_7_1=0.25
  equirectangularTile_7_2=1
  equirectangularTileBitrate_7_2=0.25
  equirectangularTile_7_3=1
  equirectangularTileBitrate_7_3=0.25
  equirectangularTile_7_4=1
  equirectangularTileBitrate_7_4=0.25
  equirectangularTile_7_5=1
  equirectangularTileBitrate_7_5=0.25
  equirectangularTile_7_6=1
  equirectangularTileBitrate_7_6=0.25
  equirectangularTile_7_7=1
  equirectangularTileBitrate_7_7=0.25
  ;bitrate is used only if useTile=false. A ratio r means a bitrate of r*videoOutputBitRate
  bitrate=1
