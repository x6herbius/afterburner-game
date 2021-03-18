import sys
import zipfile
import glob
import os

def main():
	if len(sys.argv) != 3:
		print("Usage: createZipForWindows.py <input> <output>", file=sys.stderr)
		sys.exit(1)

	inDir = sys.argv[1]
	outPath = sys.argv[2]

	if not os.path.isdir(inDir):
		print(inDir, "is not a valid directory.", file=sys.stderr)
		sys.exit(1)

	allFiles = glob.glob(os.path.join(inDir, "**"), recursive=True)
	pdbFiles = glob.glob(os.path.join(inDir, "*.pdb"))
	libFiles = glob.glob(os.path.join(inDir, "*.lib"))
	allowedFiles = list(set(allFiles) - set(pdbFiles).union(set(libFiles)))

	print("Creating zip:", outPath, f"({len(allowedFiles)} files)")

	oldCwd = os.getcwd()
	os.chdir(inDir)

	with zipfile.ZipFile(outPath, 'w', zipfile.ZIP_DEFLATED) as outFile:
		for fileName in allowedFiles:
			relFileName = os.path.relpath(fileName, inDir)
			print(f"Adding to {outPath}: {relFileName}")
			outFile.write(relFileName)

	os.chdir(oldCwd)

	print("Done")

if __name__ == "__main__":
		main()
