# vr18
**Input**: Text queries like *“Find shots of destroyed buildings”* or *“Find shots of people shopping”*

**Output**: HTTP GET request with parameters:
* Group number (we are group "2")
* Video ID (from data set)
* Frame number (from data set)

**Example:** http://demo2.itec.aau.at:80/vbs/aau/submit?team=2&video=35678&frame=2435

(url, port and path will be different during showdown --> output needs to be flexible)

**Source:** http://demo2.itec.aau.at/vbs/aau

## Note by MrMudskip:
Please make sure to implement each descriptor (and other features) as its own class, since the methods from the buttons are very likely to change during UI development. Buttons should only call methods from the other classes and not have too mucb functionality themselves.

## TODO:
| Task | Owner | State
|--|:--:|:--:|
|~~Exploration: Welcher Input/Output wird gegeben/verlangt?~~| @MrMudskip | Done
|~~Grobe Planung der Komponenten (Erweiterung des TODO)~~ | @MrMudskip | Done
|GUI Design & Implementation |@MrMudskip| Started
|A self-designed shot-boundary detection | theuerse | Started
|Similarity measures for comparisons |?| TODO
|Descriptor: Colour Histogram | babel | Started
|Descriptor: Keypoint-Detection |?| TODO
|Descriptor: Edge Histogram | babel | Started
|Agglomerative Clustering|?| TODO
|Small CNN (downloadable)|?| TODO

## Milestones
Sa 07.04.2018 base system feature complete (whole cycle)

## IMG
![Design01](https://github.com/theuerse/vr18/blob/1ec8988db8852e3b227385d951201ffa09c0abf4/img/VR_proj_Design_01.png "Design01")
