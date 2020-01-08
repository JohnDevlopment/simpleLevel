# simpleLevel documents and help files
This directory holds files that are useful for the one coding the game. The following list describes what each file is for:

<table>
 <tr>
  <th>File</th>
  <th>Description</th>
 </tr>
 <tr>
  <td>dewpsi.1</td>
  <td>GNU man page for dewpsi, section 1.<small><sup>[1]</sup></small></td>
 </tr>
 <tr>
  <td>dewpsi.7</td>
  <td>GNU man page for dewpsi, section 7.<small><sup>[1]</sup></small></td>
 </tr>
 <tr>
  <td>ini-file.n</td>
  <td>GNU man page for PDIniFile, no section number.<small><sup>[1]</sup></small></td>
 </tr>
 <tr>
  <td>pdcolors.tmac</td>
  <td>Install in your GNU man macro directory.</td>
 </tr>
 <tr>
  <td>pd-tech.ms</td>
  <td>Project Dewpsi technical document.<small><sup>[2]</sup></small></td>
 </tr>
 <tr>
  <td>pd-tech-refs</td>
  <td>A bibliographic database file for use with GNU refer.<small><sup>[3]</sup></small></td>
 </tr>
 <tr>
  <td>pd-tech-res.i</td>
  <td>An index file created with indxbib.<small><sup>[3]</sup></small></td>
 </tr>
 <tr>
  <td>pd.tmac</td>
  <td>Install in your GNU man macro directory.</td>
 </tr>
</table>

* * *

## Helpful Information
Sections include misc information that you would want to know.

### Music Loops
Music files and their starting positions in the loop.

<table>
 <tr>
  <th>File</th>
  <th>Seconds</th>
 </tr>
 <tr>
  <td>planning_partA.ogg</td>
  <td>3.2</td>
 </tr>
</table>

### Instructions on how to use components

**FadeComponent**
Use as a component inside of an object. The object's ini file must include a FadePattern section.
The objects are listed below

1. Mandatory options
   * type: only `pattern` is valid.
   * pattern: can be one of solid.
   * color: three integers specifying the RGB color of the pattern.
   * direction: can be one of `IN`, `OUT`. IN means that it fades in from black, whereas
     OUT fades out to black.
   * length: milliseconds for the fade to last.
2. Other options:
   * width: width of the texture. If omitted, defaults to the screen width, also true
     if the argument is `SCREEN`. `IMAGE` as an argument causes the texture's original width
     to be used. Otherwise, argument is an integer, in pixels.
   * height: height of the texture. Same arguments as with width.
   * delay: interpreted as a floating point; the seconds before fade starts. Can be a
     fractional number. If omitted, it defaults to 3\.0.

* * *

1. <a id="footnote1"></a>Read with the command `man file_path`.
2. <a id="footnote2"></a>GNU roff document that uses the ms macro set. To output formatted text for a tty device, use `groff -T utf8 -s -t -R -ms`.
	To output HTML code, change the '-T' option to html or www. And to output raw PDF or postscript data, just change the '-T' option to
	pdf or ps, respectively.
3. <a id="footnote3"></a>This file must be in the same directory as pd-tech.ms.
