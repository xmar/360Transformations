Examples:
=========


Set Up:
-------

In this folder we suppose you already have an up-to-date version of 360Transformations
compiled on your computer.

You can follow the intructions from the previous folder to compile the software
directly on your computer, or use the following vagrant box:

http://dash.ipv6.enstb.fr/headMovements/archives/360TransformationsDebianVagrant.box

How to Use the Vagrant Box?
---------------------------

1. Download the ``360TransformationsDebianVagrant.box`` box with the link from the
Set Up Section.

2. Add the box on your local vagrant folder::

  vagrant box add '360TransformationsDebianVagrant' '360TransformationsDebianVagrant.box'

3. Go into the examples folder if you are not yet in

4. Initialize the virtual machine and the vagrant configuration in this folder::

  vagrant init '360TransformationsDebianVagrant'

4bis. If you want you can increase the number of vCPU used by the VM by editing the
``Vagrantfile`` file (look on google to see how to do that). The trans software will
automatically use the extra CPUs. By default there is only one CPU on the VM.

5. Start the virtual machine and log-in::

  vagrant up
  vagrant ssh

6. Inside the virtual machine , you will find a folder ~/360Transformation/transformation
that contains the source code of the software. You can pull the last version of the code
with ``git pull``, and compile the code by going into the ``build`` folder and running
the ``make`` command.

7. Inside the folder ``/vagrant`` you should find the ``example`` folder from your
computer::

  cd /vagrant

8. You can run the example scenario from inside this  folder

9. You can quit the terminal with <ctrl-d> and shutdown the virtual machine with
``vagrant halt``.

Example Scenarios:
------------------

Description of the Files:
_________________________

Inside the example folder, you should find a mp4 file and some INI files.

The input.mp4 file is a 4K HEVC Equirectangular 360-degree video. This video constains
only 5 frames to avoid wasting storage space.

Multiple INI file named Config_X.ini with X an integer number. X is the ordinal
of the test. Test should be run starting by Config_1.ini, followed by Config_2.ini,
etc. The order is important because output from previous test may be input to next test scenario.

Scenario 1:
___________

Run inside the example folder (or inside the /vagrant folder inside the VM)::

  trans -c Config_1.ini

This scenario reads the input.mp4 equirectangular video and transforms it into
a 4x3 Cubemap video.

Outputs::

  output1_1CubeMap.mp4  -> an HEVC 4x3 cubemap 360-degree video

Scenario 2:
___________

Run inside the example folder (or inside the /vagrant folder inside the VM)::

  trans -c Config_2.ini

This scenario reads the input.mp4 equirectangular video and transforms and
the output1_1CubeMap.mp4 cubemap 360-degree video, and compute the S-PSNR of the
two videos.

Outputs::

  quality2_2CubeMap.txt  -> contains the computed S-PSNR-I
  the command line output also print the computed S-PSNR-I

Scenario 3:
___________

Run inside the example folder (or inside the /vagrant folder inside the VM)::

  trans -c Config_3.ini

Project back the 4x3 Cubemap video into an Equirectangular video.

Outputs::

  output3_1Equirectangular.mp4 -> an HEVC 4x3 cubemap 360-degree video

Scenario 4:
___________

Run inside the example folder (or inside the /vagrant folder inside the VM)::

  trans -c Config_4.ini

Project the input.mp4 video into multiple packing. Multiple transformation flow
in parallel.

Outputs::

  output4_1Equirectangular.mp4
  output4_2CubeMap.mp4
  output4_3CubeMapCompact.mp4
  output4_4Pyramid.mp4
  output4_6RhombicDodeca.mp4
  output4_7EquirectangularTiled.mp4

Scenario 5:
___________

Run inside the example folder (or inside the /vagrant folder inside the VM)::

  trans -c Config_5.ini

Example of rotations. The two output video use different rotated projections (rotation 45° around (1, 0, 1)).

Outputs::

  output5_1EquirectangularRot.mp4
  output5_2CubeMap.mp4

Scenario 6:
___________

Run inside the example folder (or inside the /vagrant folder inside the VM)::

  trans -c Config_6.ini

Example of offset projection. Offset Equirectangular and offset cubemap with 0.7 offset parameter.

Outputs::

  output6_1EquirectangularOffset.mp4
  output6_2CubeMap.mp4

Scenario 7:
___________

Run inside the example folder (or inside the /vagrant folder inside the VM)::

  trans -c Config_7.ini

Static viewport extraction. Viewport extracted at the same position from two different projections.

Outputs::

  output7_1FlatFixed.mp4 -> from the original Equirectangular projection
  output7_2FlatFixed.mp4 -> from the 4x3 cubemap projection

Scenario 8:
___________

Run inside the example folder (or inside the /vagrant folder inside the VM)::

  trans -c Config_8.ini

Dynamic viewport extraction. Viewport extracted using a Head Movement trajectory from the head movement dataset available here: http://dash.ipv6.enstb.fr/headMovements/ .

Outputs::

  output8_1FlatFixed.mp4 -> from the original Equirectangular projection
  output8_2FlatFixed.mp4 -> from an offset Equirectangular projection
