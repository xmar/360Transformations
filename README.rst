README
======

Introduction
------------

This repository contains tools to manipulate 360-degree videos and in particular to prepare videos for our proposal, which is `a viewport-adaptive streaming system for 360-degree video <https://github.com/xmar/360Transformations/blob/master/academicPaper/blog/post.rst>`_. To study this proposal, we have developed a tool, which takes any 360-degree video in input and outputs another 360-degree video, which is mapped into another geometric projections (currently available mappings are equirectangular, cube map, rhombic dodecahedron, and pyramid), and which is tiled with various encoding quality.

Structure of the repository
---------------------------

:transformation: C++ code of our 360-degree video projection tool and Python3 scripts used to generate the results presented in the academic paper

:juliaCode: Julia files to help compute 3D rotation matrices and coordinates of intersection between a plan and a straight line

:dataset: Python3 scripts used to analyze the head movement dataset. (The dataset itself is not provided)

:academicPaper: LaTeX source code of a paper submitted to an ACM conference and the text of a typical *blog post* related to the proposal



