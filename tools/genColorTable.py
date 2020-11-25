from PIL import Image
import numpy as np

def gamaCorrection(input, correct):
    return int(round( (255.0 * (input / 255) ** 2.2)))

def main():

    im = Image.open("night.png")
    pix = np.array(im.getdata()).reshape(im.size[0], im.size[1], 3)

    print("struct color table[] = {")
    for colorIndex in range(0,256):
        print("{{ .r ={0:>3}, .g={1:>3}, .b={2:>3} }},".format(gamaCorrection(pix[colorIndex][0][0],1.), gamaCorrection(pix[colorIndex][0][1],0.6), gamaCorrection(pix[colorIndex][0][2],0.5)))

    print("};")
if __name__ == "__main__":
    main()
