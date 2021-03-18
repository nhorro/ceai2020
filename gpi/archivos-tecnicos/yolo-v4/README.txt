# Hello YOLOv4

https://github.com/AlexeyAB/darknet#how-to-train-to-detect-your-custom-objects


id	label	count
-----------------
0 	ALB		1719
1	BET		200
2	DOL		117
3	LAG		67
4	SHARK	176
5	YFT		734
6	OTHER	299
7	NoF		465

n_classes=7 (se omite NoF por ahora)

--- file: obj.names ---
ALB		1719
BET		 200
DOL		 117
LAG		  67
SHARK	 176
YFT		 734
OTHER	 299



1. change line max_batches to (classes*2000 but not less than number of training images, but not less than number of training images and not less than 6000), f.e. max_batches=6000 if you train for 3 classes

7*2000 = 14000



--- file: obj.data ---

classes = 2
train  = data/train.txt
valid  = data/test.txt
names = data/obj.names
backup = backup/