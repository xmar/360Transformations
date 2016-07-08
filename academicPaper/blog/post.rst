Viewport-Adaptive Navigable 360-Degree Video Delivery
=====================================================

360-Degree videos are becoming more and more popular with the advent of omnidirectionnal capture devices and interactive display systems such as Head-Mounted Display (HMD) devices.
A 360-Degree video is an omnidirectionnal video: Images are captured from every direction and are combined into a spherical video.
What is display to the user, which is indifferently
called *Field of View (FoV)* or *viewport*, is only a portion of the full spherical video and depends on in which direction he is watching. `Figure 1`_ illustrates this principle.
To prevent `simulator sickness <http://hfs.sagepub.com/content/53/3/308.short>`_ and to provide good Quality of Experience (QoE), the vendors of HMDs recommend that `the enabling multimedia systems react to head movements as fast as the HMD refresh rate <https://developer.oculus.com/documentation/intro-vr/latest/concepts/bp_intro/>`_.
Since `the refresh rate of state-of-the-art HMDs is 120 Hz <http://www.vrnerds.de/vr-brillen-vergleich/>`_, the whole system should react in less than 10 ms.
Those characteristics make it challenging for service providers to deliver 360-Degree videos.
To guarantee the 10 ms delay, content providers cannot stream directly the user viewport.
The state-of-the-art solution to maintain interactivity is to stream the full spherical video and to let the HMD extracts the viewport in real time, according to the user head position.
Therefore the majority of the bandwidth is waste transmitting portion of the video that will never be displayed.

.. figure:: images/360videos.gif
  :align: center
  :name: Figure 1

  **Figure 1**: 360-Degree delivery

To reduce the bandwidth consumption without impacting the interactivity, we propose a *viewport-adaptive streaming system* that follow the same principles as in rate-adaptive streaming technologies.
The server offers multiple *representations* of the same 360-degree video but instead of offering representations that only differ by their bit-rate, like for rate-adaptive streaming, the server offers representations that differ by having better quality in a given region of the video. Each video representation is characterized by a *Quality Emphasis Center (QEC)*, which is a given position in the spherical video around which the quality of the video is maximum. The quality of the video is lower for video parts that are far from the QEC. Then depending on the user head movement the client select the right representation to download.
The whole video is available at the client so it can generate any viewport at any time but the farther from the QEC the user is watching, the lower the viewport quality is. This system is despited in `Figure 2`_.

.. figure:: images/delivery_chain.png
  :align: center
  :name: Figure 2

  **Figure 2**: Viewport-adaptive 360-degree video delivery system:
  The server  offers video representations for three QECs. The dark grey is the part of the video encoded at high quality and the light gray the low quality. The viewport is the dotted red rectangle, the FoV center is the cross

The architecture of our proposed system, illustrated in `Figure 3`_, is very similar to DASH architecture.

**server**
  On the server, the video is split into multiple segment, which size range from 1 s to 10 s. Each segment is encoded into different representations. Each representation is characterized by its QEC and its average bit-rate. The server generate a manifest file that contains the description of all segment representations available.

**client**
  Over time the orientation of the user head change along with the available bandwidth.
  The client can estimate the available bandwidth and can measure the current head orientation using `Euler angles <https://en.wikipedia.org/wiki/Euler_angles>`_.
  Periodically, the client run an *adaptation algorithm* to choose the new segment representation to download, based on its estimation of the future available bandwidth and on the future position of the user head.
  Then, each time the HMD requires a new frame, the client extract the viewport from the current segment representation.

The size of the video segments determines how often the client can request for a new representation.
Short segments enable quick adaptation to head movement and bandwidth variations, but increase the overall number of segment, the size of the manifest files, and the network overhead due to the frequent requests. Longer segments improve the overall encoding efficiency of the video codecs but reduce the adaptation flexibility.

.. figure:: images/new_delivery.png
  :align: center
  :name: Figure 3

  **Figure 3**: Viewort-adaptive streaming system: the
  server offers 6 representations (3 QECs at 2 bit-
  rates).
  The streaming session lasts for three
  segments. The client head moves from left to right,
  while the available bandwidth varies. For each
  segment, the client requests a representation that
  matches both the FoV and the network throughput.

Current encoders cannot directly encode spherical videos.
They are made to encode rectangular 2D videos.
State-of-the-art 360-Degree videos are projected on a 2D layout using an *equirectangular* projection.
The equirectangular projection is not the only possible projection, as illustrated in `Figure 4`_. In our study we tested the *cube map* projection, the *pyramid* projection and the *rhombic dodecahedron* projection.


.. figure:: images/2d_layout_projections.png
  :align: center
  :name: Figure 4

  **Figure 4**: Projections into four geometric layouts
