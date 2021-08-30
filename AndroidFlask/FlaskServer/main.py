from flask import Flask, request, send_from_directory

from os import mkdir
from os.path import exists, join
import uuid
import cv2
import numpy as np


Original_image = './Original_image'
New_image = './New_image'

app = Flask(__name__)

if not exists(Original_image):
    mkdir(Original_image)

if not exists(New_image):
    mkdir(New_image)


@app.route('/', methods=['POST'])
def handle_request():
    image = request.files['image']
    filter = request.form.get('filter')
    filename = f"{str(uuid.uuid4())}.{image.filename.split('.')[-1]}"
    path = join(Original_image, filename)
    image.save(path)
    img = cv2.imread(path, 1)
    image_process(img, filter)
    # dim = (255,255)
    # cv2.resize(img,dim)
    # cv2.imshow("slika", img)
    # cv2.waitKey(0)
    return "Image Uploaded Successfully."


def image_process(image, filter):
    picture = image
    width, height = 150, 150
    # image_resize = cv2.resize(image, (width, height))
    # cv2.imshow("Original", image_resize)
    # cv2.waitKey(1500)

    if filter == "Edge Detection":
        picture = cv2.Canny(image, 100, 200)
        # cv2.imshow("Filtered", picture)
        # cv2.waitKey(1500)
    elif filter == "Sharpening":
        kernel = np.array([[-1, -1, -1],
                          [-1, 9, -1],
                          [-1, -1, -1]])
        picture = cv2.filter2D(image, -1, kernel)
        # cv2.imshow("Filtered", picture)
        # cv2.waitKey(1500)

    elif filter == "Blur":
        picture = cv2.medianBlur(image, 5)
        # cv2.imshow("Filtered", picture)
        # cv2.waitKey(1500)
    elif filter == "Gaussian Blur":
        picture = cv2.GaussianBlur(image, (3, 3), 0)
        # cv2.imshow("Filtered", picture)
        # cv2.waitKey(1500)

    cv2.imwrite("./New_image/new_image.jpg", picture)
    return "Image filtered correctly"


@app.route('/', methods=['GET'])
def send():
    return send_from_directory(New_image, "new_image.jpg")


app.run(host='0.0.0.0', port=5000)
