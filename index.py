from PIL import Image

# Open PPM file and save as PNG
img = Image.open("cube_output.ppm")
img.save("cube_output.png")
