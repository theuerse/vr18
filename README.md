# vr18



## TODO:
**Exploration**: Welcher Input/Output wird gegeben/verlangt?   (owner: Julian - Done)
*Input*: Text queries like �Find shots of destroyed buildings� or �Find shots of people shopping�
*Output*: HTTP GET request with parameters:
* Group Number (we are group "2")
* Video ID (from data set)
* Frame number (from data set)

*Example:* http://demo2.itec.aau.at:80/vbs/aau/submit?team=2&video=35678&frame=2435
(url, port and path will be different during showdown --> input needs to be flexible)
*Source:* http://demo2.itec.aau.at/vbs/aau


**Grobe Planung der Komponenten** (Erweiterung des TODO) (owner: Julian)
