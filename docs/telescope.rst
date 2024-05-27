Dobsonian Telescope
===================


.. image::  images/telescope_thumbnail.png
  :target: https://youtu.be/KxbAh8ndkbY


I'm not an expert in astronomy, I've never built (or even used) an astronomical
telescope before, but it is something I have always wanted to do. It hadn't
occurred to me that you could build a telescope, but years ago I ran into
some instructions to build a Dobsonian telescope. At the time I found it quite
difficult to find the optical parts and even struggled to source the specified
10-inch cardboard tube, and moved on to other projects. Recently I stumbled
across the `Stellafane guide <https://stellafane.org/tm/dob/index.html>`__, 
an excellent introduction to Amateur Telescope Making and with renewed
enthusiasm I set about sourcing the parts I would need.  I was excited to
discover that telescope parts could be sourced quite easily online and that the
cost wasn't too unreasonable. Ironically, the large cardboard tubes still seem
quite hard to find.

Design
------

The `Stellafane guide <https://stellafane.org/tm/dob/index.html>`__ is a very
good introduction and provides all the details you need to get started. I won't
try to duplicate it here, I will just document some of the design decisions I
made for my particular scope.

I opted for an 8-inch reflector, and this choice was mainly down to economics.
I found a mirror set including a 200 mm (8 inches) primary mirror and a 40mm
secondary mirror. I found that the price of the mirror seemed to increase quite
significantly beyond this size. I think that a 6 or 8-inch reflector should be
a reasonable size for a first telescope. The mirror has a focal length of 1600mm
which gives a focal ratio of about f8. I think that the relatively high focal
ratio should give quite high magnifications and should be good for observing planets.

As far as possible I have tried to follow the Stellafane guide, for a first
telescope I think it makes sense to use a tried and tested approach. The most
significant deviation I have mode is in the construction of the tube. I have
struggled to find suitable cardboard tubes [#f1]_ so I wanted to explore some
other options. It is quite easy to build a square, or hexagonal tube using wood
or plywood. Aluminium or PVC tubes might also be options. A different approach
entirely would be a truss-tube-based design.

In the end, I thought it might be fun to build the tube using 2020 T-slots.
These aluminium extrusions work like a scaled-up version of Mechano and are
often used to build CNC machines and 3D printers. Although I haven't seen many
other telescopes using these profiles, I thought it might be worth a go. Apart
from being easy to get hold of, the main advantage of using this type of
material is the flexibility it affords, allowing the design and dimensions of
the scope to be easily adjusted.

The size and focal length of the primary mirror are the primary factors that
determine the dimensions of the scope. Stellafane provides an `online design
tool <https://stellafane.org/tm/newt-web/newt-web.html>`__ that can be used to
work out the main parameters.

.. image:: images/telescope_newtweb.png

The rest of the design is a balancing act, involving tradeoffs and compromises.

The first tradeoff is the size of the secondary diagonal mirror. Making the
mirror too large will block too much light from reaching the primary mirror,
too small and not all the light reflected from the primary mirror will reach
the eyepiece.

Since I already have a 40mm diagonal mirror, the decision has been made for me
and it turns out that this is a good size in any case. The only other dimension
I can change is the distance from the secondary mirror to the eyepiece, once
this has been decided, the distance from the diagonal to the primary is
calculated by the tool. The goal is to avoid vignetting (blocking the light
path) by the tube or the focuser while maximising the illuminated diameter.

The distance from the primary to the eyepiece is a combination of the tube
diameter, the thickness of the tube and the height of the focuser.
Constructing the tube using 2020 t-slots allows us to make the tube any diameter
we want, but the thickness of the tube is 20mm which is much thicker than a
cardboard tube. The focuser I bought has a fairly large minimum height and lots
of travel. A tube width of 250mm seems to be a good compromise giving a 12mm
100% illuminated diameter and no vignetting.

.. image:: images/telescope_raytrace.png

The Newtweb utility generates a handy ray trace which helps visualise the design
and diagnose the cause of any vignetting. Once the key parameters have been
designed, the utility calculates a handy table with all the measurements.

.. image:: images/telescope_dimensions.png


.. [#f1] I'm sure I could find some if I tried, but when I can have the 
         optical parts delivered to my door, the effort seems disproportionate.

Optical Tube Assembly
---------------------

Now we have the dimensions it is a simple matter to design the optical tube
assembly using the 2020 profile. 

.. image:: images/telescope_optical_tube_assembly.png

The tube is a basic cuboid, at first, I just used
corner connectors but found that the tube wasn't stiff enough and needed some
trianglation. 

.. image:: images/telescope_corner_connectors_1.jpg
  :width: 75%
  :align: center

.. image:: images/telescope_corner_connectors_2.jpg
  :width: 75%
  :align: center

.. image:: images/telescope_corner_connectors_3.jpg
  :width: 75%
  :align: center

I added corner blocks to each of the corners and aluminium
gussets in the centre and this solved the problem very nicely. I only added the
gussets to the top and bottom of the tube since the altitude bearings on the
side will add support to this dimension.

.. image:: images/telescope_gussets.jpg
  :width: 75%
  :align: center

Some of the hardware had a silver/chrome finish, I have tried to reduce
unwanted reflections by either painting these matt black or by covering them
with self-adhesive black felt. The open-tube design may also benefit from a
shroud and additional flocking, and this is something I may address in the
future.


Mirror Cell
-----------

For the mirror cell, I stuck quite closely to the Stellafane design, but I did
have to modify the lower part to fit into a square rather than a circular tube. I
incorporated plenty of ventilation into the design to assist in the cooling of
the mirror. I cut the parts from 18mm plywood and again painted them matt
black. I used 2020 corner pieces to fix the mirror cell into the tube. One
nice thing about this is that the distance between the mirror cell and
the secondary mirror can easily be adjusted.

.. image:: images/telescope_mirror_cell_1.svg
  :width: 75%
  :align: center

.. image:: images/telescope_mirror_cell_2.svg
  :width: 75%
  :align: center

.. image:: images/telescope_mirror_cell_3.png
  :width: 75%
  :align: center

I stuck with Stellafane's recommendation to use silicon adhesive to mount the
mirror, but I did take the risk of substituting 'coffee stirrers' for 'popsicle
sticks'!

Spider and Secondary Mirror Holder
----------------------------------

The spider and secondary mirror holder need to be adjustable in a couple of
different dimensions to allow the scope to be properly aligned (collimated).
This is another place where we can take advantage of the inherent adjustability
of the t-slots. The secondary mirror holder is a short length of t-slot cut off
at 45 degrees, and the mirror is glued to this. The secondary mirror holder is
suspended from the edges of the tube using a length of 2mm x 10mm aluminium
strip. I have made this strip relatively thin to reduce the diffraction caused
by blocking the light path. The strip is attached to the tube using 2 corner
blocks. This allows adjustment from side to side, into and out of the tube as
well as rotation in two axes.

.. image:: images/telescope_spider.png

.. image:: images/telescope_secondary_mirror_holder.png

Again the secondary mirror is attached using silicon adhesive, using a 'coffee
stirrer' as a spacer while the adhesive cures.

Focuser
-------

This is another area where economics plays a factor, I opted for a `relatively
inexpensive <https://www.ebay.co.uk/itm/144815207453?var=444035499240>`__ 
1.25-inch rack and pinion focuser. This seemed like a good compromise between
cost and quality. The focuser is an all-metal design that seems to be well-made. 
The focuser is attached to the tube using a 6mm sheet of black phenolic
resin, and a matching sheet sits behind the focuser.

Alignment and Focus Test
-------------------------

Before committing to the final positioning of the primary mirror, I wanted to
check that I could bring distant objects into focus. Having performed a rough
alignment I mounted a 25mm eyepiece and attempted to focus on some distant (and
not-so-distant) objects. I live about 4 miles from the `Malvern Hills
<https://en.wikipedia.org/wiki/Malvern_Hills>`_ and can see these from where I
live. I hadn't yet built the mount, so used dining room chairs to point the
telescope towards the horizon. I found that the calculated position of the
primary mirror worked almost exactly as predicted. With the focuser extended by
about 25mm from its minimum height, I was able to bring the Malvern hills into
focus and was able to see individual branches of trees that couldn't be seen
with the naked eye, which was quite promising.

The focuser I bought has quite a lot of travel and with the focuser close to
fully extended I was able to focus on objects a hundred meters or so away.

Dobsonian Mount
---------------

This second video shows the rest of the telescope build along with some early
testing.

.. image::  images/telescope_thumbnail2.jpg
  :target: https://youtu.be/aUAmSaV4Oio

The Dobsonian mount follows a traditional design.  One advantage of the 2020
profile is that the altitude bearings can be bolted directly to the OTA
eliminating the need for a tube cradle. The altitude bearing can easily be moved
along the tube to achieve a perfect balance. For simplicity, I used whole
circles for the altitude bearings, which also add rigidity to the tube.

I used vinyl tape to cover the edges of the bearings which run on square teflon
pads screwed to the rocker box. I couldn't find the recommended brand so just
went for something with a bit of surface texture. The bearings run on Teflon
pads screwed to the rocker box. I found that placing the bearings at the same
height was critical, but once this had been adjusted they moved with a smooth
and pleasing motion.

.. image:: images/telescope_dobsonian_mount.png

The base of the rocker base and the base plate are also made from plywood
disks. Including the altitude bearings that's 4 disks in total. These could
be cut accurately using a router, but I used a simple disk-cutting jig on the
band saw. This allows the disks to be made very quickly and accurately.

The sides of the rocker box need accurately cut curves and have a pattern of
cutouts to reduce weight. Although the CNC router isn't very quick it does give
very accurate results and a nice finish.

In this design, I attached the Teflon pads to the base of the rocker box and
covered the base plate with vinyl laminate. I chose to work this way round
because the top of the baseboard was completely smooth while the bottom of the
rocker box has quite a few recessed screws. This does mean that I need a full
disk for the base plate and can't make a three-legged version. I used a
roller-skate bearing in the bottom of the rocker box which rotates around an M8
bolt in the base plate. I think a simple bush would have been adequate here,
but I had a bearing on hand.

The telescope feet are made from three hockey pucks. I was quite pleased with
the finished mount and I was surprised by the smoothness of the motion and the
stability. I did find that there was some deflection in the unsupported corners
of the rocker box and added some additional bracing to add stiffness in this
area. I finished all the plywood parts using about half a dozen coats of lacquer
to provide some protection from moisture. 

Testing
-------

.. image:: images/telescope_moon.png
  :width: 75%
  :align: center

Inevitably, since completing the telescope there haven't been many clear
nights. The days are quite long at the moment, which makes it quite difficult
to observe planets. I have been able to get some good views of the moon.

Initially, I found it quite tricky to aim the telescope and track objects in the
sky. With a bit of practice and careful use of the finder, I was able to aim
the scope quite accurately. The motion of the Dobsonian mount seems quite good
too, and once I worked out the right direction was able to nudge the telescope
as objects moved out of view.

I have been very pleased with the level of detail I have been able to see, it is
possible to make out many features and navigate the surface. I can't wait to try
observing some planets.


What next?
----------

Although I haven't had much opportunity to put the telescope through its paces,
I am pleased with the results so far and I am excited to try some more
observing. There is a fair amount of light pollution in the area where I live,
and have think it would be great to take it to some better observing sights.

I think one of the biggest improvements I would now like to make is to improve
the portability of the scope. I have seen several travel-scope designs that
use truss tubes (or similar) that can be knocked down into a small portable
unit. I would be interested in exploring these types of designs and as an
ultimate goal would like to design a scope that can be 3d printed using an
entry-level FDM printer.

Another area I would like to explore is electronic control of the scope. I
would like to think about designing an electronic finder to build a
"push-to", or even a motorised "go-to" mount.
