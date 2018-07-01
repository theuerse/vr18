# vr18
**Input**: Text queries like *�Find shots of destroyed buildings�* or *�Find shots of people shopping�*

**Output**: HTTP GET request with parameters:
* Group number (we are group "2")
* Video ID (from data set)
* Frame number (from data set)

**Example:** http://demo2.itec.aau.at:80/vbs/aau/submit?team=2&video=35678&frame=2435

(url, port and path will be different during showdown --> output needs to be flexible)

**Source:** http://demo2.itec.aau.at/vbs/aau


## TODO:
| Task | Owner | State
|--|:--:|:--:|
|~~Exploration: Welcher Input/Output wird gegeben/verlangt?~~| @MrMudskip | Done
|~~Grobe Planung der Komponenten (Erweiterung des TODO)~~ | @MrMudskip | Done
|GUI Design & Implementation |@MrMudskip| Started
|A self-designed shot-boundary detection |?| TODO
|Similarity measures for comparisons |?| TODO
|Descriptor: Colour Histogram |?| TODO
|Descriptor: Keypoint-Detection |?| TODO
|Descriptor: Edge Histogram |?| TODO
|Agglomerative Clustering|?| TODO
|Small CNN (downloadable)|?| TODO
