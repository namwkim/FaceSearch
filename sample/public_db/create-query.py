import os, Image

qdir = "../public_query"
if not os.path.exists(qdir):
	os.makedirs(qdir);

for root, dirs, files in os.walk("./faces94"):
	for file in files:
		if file.endswith(".1.png"):			
			im = Image.open(root+'/'+file)
			
			im.save(qdir+'/'+file)
			print qdir+'/'+file
