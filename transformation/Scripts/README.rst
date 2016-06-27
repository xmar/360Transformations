Scenario Script
===============


Introduction
------------

This folder contains some Python3 scripts used to automatize and run our test scenarios.
The script can be run in a distributed system.

Requirements
------------

This script suppose you have ``numpy`` and ``multiprocessing`` python3 libraries available on your system.

How To Use
----------

Client
......

The client is used to run the simulations. It requests a job to the server and start executing it.
If no job is available at the server or if the server is not  running, the client wait for the server to be available and to have new jobs.

**./client.py** outputDir hostname authkey

--trans            Path to the **trans** software (default ../build/trans)
--config           Path to the temporary generated configuration file (default ./ConfigTest.ini)
-serverPort        TCP port used to contact the servef (default 5042)
-outputDir         Path to the output directory
-hostname          Hostname of the server
-authkey           Authentication key to contact the server

Only one client should run at a time on a specific host.
The client will store temporary files inside '/tmp' repository.

Server
......

The server is the one that create the jobs. It do not execute any of those job. If a result for a specific job id is already on the server, the job id is not created.
The server can also host a client or a second server.

**./server.py** configuration outputDir authkey

-serverPort         TCP port used to contact the servef (default 5042)TCP port used to contact the servef (default 5042)
-configuration      Path to the configuration ini file
-outputDir          Path to the output directory
-authkey            Authentication key to contact the server

The configuration ini file should contain at least one Section. The name of the name does not matter. Each Section should contain the attributes in the following example. Except for ``layoutIds``, each attribute can contains multiple values separated by a ','. For instance n=50,5 means the server will create a job with 50 frames and a job with 5 frames. ``layoutIds`` indicate the layouts that will be used in each test. In the following example the server will define 2 x 4 = 8 jobs.

.. code-block:: ini

  [FirstTest]
  ;list of videos to test
  inputVideo = ../bridge_jumping.mp4
  ;indicate if we encode the intermediary 360-degree layout (False should not be used anymore)
  reuse = True
  ;list of number of frames to use
  n = 50
  ;list of number of random test to do at each distance
  nbT = 40
  ;list of number of QEC to generate
  nbQEC = 1,10
  ;list of distance step to use (0.25 means test will be done at distance 0, 0.25, 0.5, ..., 3,14)
  distStep = 0.25
  ;list of bitrate goal to use (should be equal to 0)
  bitrateGoal = 0
  ;list of bitrate goal to use (0.75 means bitrate goal equal to 0.75*bitrate of the original video)
  averageEqTileRatio = 0.75
  ;list of layout to use in each tests
  layoutIds = EquirectangularTiledLower,CubeMapLower,PyramidLower,RhombicDodecaLower
