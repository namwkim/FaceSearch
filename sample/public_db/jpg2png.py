import os, Image
for root, dirs, files in os.walk("./faces94"):
	for file in files:
		if file.endswith(".jpg"):
			im = Image.open(root+'/'+file)
			print root+'/'+file.replace(".jpg", ".png")
			im.save(root+'/'+file.replace(".jpg", ".png"))
		if file.endswith(".jpg") or file.endswith(".gif"):
			print root+'/'+file
			os.remove(root+'/'+file)