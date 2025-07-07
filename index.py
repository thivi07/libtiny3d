from PIL import Image

# Open PPM file and save as PNG
img = Image.open("soccer_ball_render.ppm")
img.save("soccer.png")

