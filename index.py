from PIL import Image

# Open PPM file and save as PNG
# img = Image.open("clock_demo.ppm")
# img.save("clock.png")

img = Image.open("cube_output.ppm")
img.save("cube.png")

# Load all the images
# frames = []
# for i in range(60):
#     filename = f"soccer_{i:03}.pgm"
#     img = Image.open(filename).convert("P")  # Convert to palette mode for GIF
#     frames.append(img)

# # Save as an animated GIF
# frames[0].save(
#     "soccer.gif",
#     save_all=True,
#     append_images=frames[1:],
#     duration=100,  # Duration of each frame in milliseconds
#     loop=0         # Loop forever
# )