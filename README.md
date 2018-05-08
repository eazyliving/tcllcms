tcllcms - the tcl-interface to the Little Color Management System (lcms 1.15)

Ever wanted to use lcms in your tcl-scripts? No? Well, I do :)
So this seems to be the first interface to lcms for tcl. Since this
is a more practical aproach - i.e. what I need is implemented,
most of lcms' indepth functionality is not implemented. Yet.

But let's take a look at what we have right now:

___________________________________________________________________________________________________________


cmsOpenProfile filename|*Lab|*LabD65|XYZ|sRGB ?-whitepoint whitepoint?

cmsOpenProfile opens a profile from filesystem or one of the three (well, four...) predefined profiles
*Lab, *LabD65, XYZ, sRGB and a NULL-Profile
cmsOpenProfile returns a command-handle you should store for later reference. For *Lab you can provide a whitepoint.

This command is capable of deleteing itself and giving some useful information about the profile itself.

```
profilehandle delete
profilehandle get option
```

where option is one of:

productname, productinfo, manufacturer, model, pcs, colorspace, deviceclass, renderingintent,
isintentsupported, mediawhitepoint, mediablackpoint, iluminant, colorants.

except for isintentsupported, which needs a renderingintent (0-3 see above) and a profile direction
(0=input, 1=output, 2=proof) as integer values, no option takes any further input.


example:

```
set profile [cmsOpenProfile ./rota_22072003.icc]
puts "This profiles name is: [$profile get productname]"
puts "Its PCS is [$profile get pcs] and its colorspace is [$profile get colorspace]"
puts "The default rendering intent for this profile is\
     [lindex {perceptual {relativ col} saturation {absolut col.}} [$profile get renderingintent]]"
puts "now closing the profile..." 

$profile delete
```
___________________________________________________________________________________________________________

cmsSaveProfile profilehandle filename

Saves the profile to given filename...

___________________________________________________________________________________________________________

cmsCreateGrayProfile whitepoint gamma

Creates a gray profile from given whitepoint and gamma. Returns usual profile handle.

___________________________________________________________________________________________________________

cmsCreateTransform inputprofile outputprofle rendering-intent ?-options? ...

cmsCreateTransform does create a... well... transform :) (surprise, surprise!)
Giving the input- as well as the outputprofile and one of the four intents from 0 to 3:

0 perceptual
1 relative colorimetric
2 saturation
3 absolut colorimetric

the options are:

-matrixinput, -matrixoutput, -matrixonly, -notprecalc, -nulltranform,
-highresprecalc, -lowresprecalc, or -blackpointcompensation

you get a command-handle that is capable of deleteing itself and giving some information about itself:

transhandle delete
transhandle get option

where option is one of:
intent, proofingintent, type, inputprofile, outputprofile, proofingprofile, profiles

intent and proofingintent return the integervalues given when creating the transformation,
type returns either "transform" or "proofingtransform", and input-, output- and proofingprofile return
the corresponding profilehandles used to create the transform, whereas profiles returns a list of all
profiles so you can close them in one foreach :)

example:
```
# opening the profiles
set input_profile [cmsOpenProfile /home/lula/profiles/4C/rota_220703.icc]
set output_profile [cmsOpenProfile *Lab]

set transform [cmsCreateTransform $input_profile $output_profile 3]
puts "$transform was created out of [$transform get inputprofile] and [$transform get outputprofile] using rendering\
      intent [$transform get intent]"

# closing all used profiles

set profiles [$transform get profiles]
$transform delete
foreach profile $profiles {$profile delete}
```
Deleteing a transformation does not close the related profiles!

___________________________________________________________________________________________________________

cmsCreateProofingTransform inputprofile outputprofle proofing-profile rendering-intent proofing-intent ?-options? ...

That's a bunch of args... This is right the same as above, including a proofing-profile and intend.
I won't get into explaining that :)

___________________________________________________________________________________________________________

cmsDoTransform transform-handle input-values

This transforms single values according to the transformation-handle you provide.
The input values have to follow the transformation. So if transformation has Lab as input
and CMYK as output, the first three parameters have to be L,a and b and the last C,M,Y and K.
The other way round for CMYK to Lab of course... :)

example:

set cmyk [cmsOpenProfile /home/lula/profiles/4C/rota_220703.icc]
set lab [cmsOpenProfile *Lab]
set cmyk2lab [cmsCreateTransform $cmyk $lab 3]
set lab2cmyk [cmsCreateTransform $lab $cmyk 3]

set lab [cmsDoTransform $cmyk2lab "30 30 30 0"]
set cmyk [cmsDoTransform $lab2cmyk $lab]
puts "Input was 30/30/30/0, transformed to Lab (D50/2°): $lab and returned to $cmyk (according to this profiles separation)"
puts "that was a nice example!"

___________________________________________________________________________________________________________

cmsTransformImage transform-handle image

This is some experimental image transform for tk-images. This only works with rgb for in- and output...

example:

# requires Img extension for jpg...

image create photo i
i read c:/temp/test.jpg -format "jpeg"

image create photo i2
i2 read c:/temp/test.jpg -format "jpeg"


set rgb [cmsOpenProfile sRGB]
set rgb2 [cmsOpenProfile C:/WINDOWS/SYSTEM32/SPOOL/DRIVERS/COLOR/ColorMatchRGB.icc]
set cmyk [cmsOpenProfile z:/sources/spotcreator/rota.icc]
set trans [cmsCreateProofingTransform $rgb $rgb $cmyk 3 3]

cmsTransformImg $trans i2

pack [button .b1 -image i] -side left
pack [button .b2 -image i2] -side left


___________________________________________________________________________________________________________


cmsTransform2DeviceLink transformhandle ?-highresprecalc? ?-lowresprecalc?

Creates a devicelink profile from a transformhandle. See lcms doc
___________________________________________________________________________________________________________


cmsSetCMYKNormalized ?value?

This is a help for all of you (and me ;) who need cmyk-values in 0..100, not 0..255.
Instead of calculating this for every damn value you got, you just switch this on and
have all cmyk-values in tcllcms in the range of 0..100. The default is off. Providing
a value (1, yes, on, 0, no, off) you switch this on or off. Without value returns
the current state.

example:

cmsSetCMYKNormalized 1
puts "now CMYKNormalized is set to [cmsSetCMYKNormalized]"
___________________________________________________________________________________________________________

cmsDeltaE method Lab1 Lab2

This calculates the color difference, using the given method of:

CIE2000, CIE94, Lab, BFD or CMC

example:

set deltaE [cmsDeltaE CIE2000 "50 44 20" "40 33 30"]

___________________________________________________________________________________________________________

cmsConversion function values ?-whitepoint whitepoint?

This converts input values according to function which is one of:

Lab2XYZ
XYZ2Lab
Lab2LCh
LCh2Lab
XYZ2xyY
xyY2XYZ

You can use an XYZ-whitepoint for Lab2XYZ and XYZ2Lab. If not provided, D50 is assumed.

example:

set profile [cmsOpenProfile sRGB]
foreach color {red green blue} XYZ [$profile get colorants] {
set Lab [cmsConversion XYZ2Lab $XYZ]
        puts "$color in sRGB is Lab([join $Lab ,])"
}
$profile delete

___________________________________________________________________________________________________________

cmsWhitePointFromTemp temp

returns an xyY whitepoint from temperature temp (K)

example:

set whitepoint [cmsWhitePointFromTemp 6500]

___________________________________________________________________________________________________________


cmsClampLab Lab amax amin bmax bmin

see lcms documentation on this :)

___________________________________________________________________________________________________________

cmsBuildGamma size gamma

creates an object command named "gammaX" which represents a gamma table.
At the moment it's just build in but does nothing special except existing :)
Further implementation is yet to be done for gamma usage, but all gamma-commands
exist.

gammaX can be deleted by "gammaX delete".

___________________________________________________________________________________________________________

cmsBuildParametricGamma
___________________________________________________________________________________________________________

cmsAllocGamma
___________________________________________________________________________________________________________

cmsReverseGamma
___________________________________________________________________________________________________________

cmsJoinGamma
___________________________________________________________________________________________________________

cmsSmoothGamma
___________________________________________________________________________________________________________



___________________________________________________________________________________________________________

cmsErrorAction abort|show|ignore

Sets the action, lcms takes in occurance of an error. Show gives a warning and aborts
the operation. Abort is warning plus aborting the complete application and ignore will
set the action to quiet but still aborts the operation (of course).

___________________________________________________________________________________________________________

cmsSetErrorHandler script

This sets a script to be executed, in case an error occurs. Script is a tcl-procedure
which takes two arguments, first the errortext from lcms, second the lcms-errorcode.
In order to get this script working, you have to set the ErrorAction to show, else lcms will terminate
your application first.

example:

proc showError {text code} {
   tk_messageBox -title "Fehler" -message "$text\nFehlercode: $code" -type ok
}

cmsErrorAction show
cmsSetErrorHandler showError

___________________________________________________________________________________________________________

Some notes on building the library:
As this is my first project of this kind ever, my knowlege of autoconf and friends is somewhat limited.
I took most code out of the sample TEA-extension provided by Active State and mixed it with a good
cup of code from mysqltcl (http://www.xdobry.de/mysqltcl/). The win32 part of this code (which is my
"native" platform of use for tcllcms) is written with VC6. I have no knowlegde of Borland or lcc or
whatever else you can use to compile on win32. Any help is appreciated :)

On *ix you should go off with a ./configure and then make. This leaves a libtcllcms0.20.so, which you can
dynamically load with... load :)

With VC6 you will have to adjust the pathes for the preprocessor and the linker, maybe the names of the
tcl- and lcms-libs.



have fun,
Christian (bednarek@rhein-zeitung.de)
