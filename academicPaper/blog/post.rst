Viewport-Adaptive Navigable 360-Degree Video Delivery
=====================================================

360-Degree videos are becoming more and more popular with the advent of omnidirectionnal capture devices and interactive display systems such as Head-Mounted Display |nbsp| (HMD) devices.
A 360-Degree video is an omnidirectionnal video: Images are captured from every direction and are combined into a spherical video.
What is display to the user, which is indifferently
called *Field of View (FoV)* or *viewport*, is only a portion of the full spherical video and depends on in which direction he is watching.
To prevent `simulator sickness <http://hfs.sagepub.com/content/53/3/308.short>`_ and to provide good Quality of Experience |nbsp| (QoE), the vendors of HMDs recommend that the enabling multimedia systems react to head movements as fast as the HMD refresh rate. |nbsp| [#]_
Since the refresh rate of state-of-the-art HMDs is 120 |hnbsp| Hz, [#]_ the whole system should react in less than 10 |hnbsp| ms.
Those characteristics make it challenging for service providers to deliver 360-Degree videos.
To guarantee the 10 |hnbsp| ms delay, content providers stream the full spherical video and the HMD extracts the viewport in real time, according to the user head
movements.
Therefore the majority of the bandwidth is waste transmitting portion of the video that will never be displayed.

.. figure:: images/360videos.gif
  :align: center

  Figure 1: 360-Degree delivery


.. [#] https://developer.oculus.com/documentation/intro-vr/latest/concepts/bp_intro/

.. [#] http://www.vrnerds.de/vr-brillen-vergleich/

.. |nbsp| unicode:: 0xA0
   :trim:

.. |hnbsp| unicode:: 0xA0
  :trim:
